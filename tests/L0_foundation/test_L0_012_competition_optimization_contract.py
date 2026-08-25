"""L0_012 - keep optimized competition builds explicit and hardware-safe."""

from pathlib import Path
import re


REPO = Path(__file__).resolve().parents[2]
EMBEDDED = REPO / "src/embedded"
PROJECT = EMBEDDED / "nxp_cup_core0"


def _function_body(source: str, signature: str) -> str:
    match = re.search(rf"{re.escape(signature)}\s*\([^)]*\)\s*\{{(.*?)\n\}}", source, re.S)
    assert match, f"missing function: {signature}"
    return match.group(1)


def test_mcuxpresso_and_generated_cmake_use_one_global_o2_level():
    cproject = (PROJECT / ".cproject").read_text(encoding="utf-8")
    generated = (PROJECT / "cmake/mcuxpresso_debug.cmake").read_text(encoding="utf-8")

    assert 'value="gnu.c.optimization.level.more"' in cproject
    assert 'value="gnu.cpp.optimization.level.more"' in cproject
    assert "optimization.level.most" not in cproject
    assert '    "-O2"' in generated
    assert '    "-O0"' not in generated
    assert '    "-O3"' not in generated
    assert "set_property(SOURCE" not in generated


def test_builds_validate_effective_last_option_wins_commands():
    normal_build = (EMBEDDED / "build.ps1").read_text(encoding="utf-8")
    maintainer_build = (EMBEDDED / "tools/maintainer/build_cmake.ps1").read_text(
        encoding="utf-8"
    )
    checker = (EMBEDDED / "tools/maintainer/check_compile_flags.ps1").read_text(
        encoding="utf-8"
    )

    for build in (normal_build, maintainer_build):
        assert "check_compile_flags.ps1" in build
        assert '-ExpectedOptimization "-O2"' in build
    assert '"--clean-first"' in normal_build
    assert "optimization.Count -ne 1" in checker
    assert '"-g3", "-gdwarf-4", "-fno-builtin"' in checker


def test_lcd_waits_survive_optimization_and_dma_lifetimes_are_bounded():
    panel = (PROJECT / "source/nxpc_io/st7789.c").read_text(encoding="utf-8")
    display = (PROJECT / "source/nxpc_io/eGFX_Driver_ER-TFT020-3.c").read_text(
        encoding="utf-8"
    )
    spi = (PROJECT / "source/nxpc_io/lpspi1.c").read_text(encoding="utf-8")

    assert "SDK_DelayAtLeastUs(1000U, SystemCoreClock);" in panel
    assert "for(j=0;j<16000;j++);" not in panel.replace(" ", "")
    assert "lpspi1_wait_idle();" in display
    assert "volatile bool isTransferCompleted" in spi
    assert "while (!isTransferCompleted)" in spi

    # DMA submissions that reference local storage must complete before the
    # helper returns. Command bytes also keep RS stable through completion.
    for signature in (
        "void lpspi1_transfer_byte",
        "void  Write_Data_U16",
        "inline void  Write_Data_U32",
        "void Write_Data",
    ):
        source = spi if "lpspi1" in signature else panel
        assert "lpspi1_wait_idle();" in _function_body(source, signature)
    for signature in ("void ST7789__display_img", "void ST7789__display_row"):
        assert "lpspi1_wait_idle();" in _function_body(panel, signature)


def test_isr_owned_adc_and_framework_state_are_not_cached():
    adc = (PROJECT / "source/nxpc_io/nxpc__adc.c").read_text(encoding="utf-8")
    framework = (PROJECT / "source/nxpc_framework.c").read_text(encoding="utf-8")

    assert "static volatile uint16_t adc_values" in adc
    assert "interrupt_state = DisableGlobalIRQ();" in adc
    for declaration in (
        "static uint16_t * volatile g_latest_frame;",
        "static volatile bool g_frame_ready;",
        "static volatile uint32_t g_last_frame_ms;",
        "static volatile uint32_t g_motor_lease_remaining_ms;",
        "static volatile bool g_motor_lease_expired;",
    ):
        assert declaration in framework
