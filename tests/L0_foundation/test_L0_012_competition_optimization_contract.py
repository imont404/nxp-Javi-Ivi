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
    display = (PROJECT / "source/nxpc_io/nxpc_display.c").read_text(encoding="utf-8")
    spi = (PROJECT / "source/nxpc_io/lpspi1.c").read_text(encoding="utf-8")

    assert "SDK_DelayAtLeastUs(1000U, SystemCoreClock);" in panel
    assert "for(j=0;j<16000;j++);" not in panel.replace(" ", "")
    assert "volatile bool isTransferCompleted" in spi
    assert "while (!isTransferCompleted)" in spi

    # The command helper owns a stack byte, so it must wait before returning.
    # Keeping command and parameter writes on that helper also holds RS stable
    # until DMA has consumed each byte.
    assert "lpspi1_wait_idle();" in _function_body(spi, "void lpspi1_transfer_byte")
    for signature in ("static void st7789__write_command", "static void st7789__write_parameter"):
        assert "lpspi1_transfer_byte(" in _function_body(panel, signature)

    # Pixel writes may submit several chunks from caller-owned storage, but the
    # last submission must complete before the display API returns.
    display_write = _function_body(display, "void nxpc_display__write")
    assert "lpspi1_transfer_block(buffer, block_size);" in display_write
    assert "lpspi1_wait_idle();" in display_write


def test_display_cleanup_has_one_owned_transport_boundary():
    generated = (PROJECT / "cmake/mcuxpresso_debug.cmake").read_text(encoding="utf-8")
    display = PROJECT / "source/nxpc_io/nxpc_display.c"
    legacy_driver = PROJECT / "source/nxpc_io/eGFX_Driver_ER-TFT020-3.c"

    assert display.exists()
    assert not legacy_driver.exists()
    assert '"${MCUX_PROJECT_ROOT}/source/nxpc_io/nxpc_display.c"' in generated
    assert "eGFX_Driver_ER-TFT020-3.c" not in generated


def test_isr_owned_adc_and_framework_state_are_not_cached():
    adc = (PROJECT / "source/nxpc_io/nxpc__adc.c").read_text(encoding="utf-8")
    framework = (PROJECT / "source/nxpc_framework.c").read_text(encoding="utf-8")

    assert "static volatile uint16_t adc_values" in adc
    assert "interrupt_state = DisableGlobalIRQ();" in adc
    for declaration in (
        r"static\s+uint16_t\s+\*\s*volatile\s+g_latest_frame;",
        r"static\s+volatile\s+bool\s+g_frame_ready;",
        r"static\s+volatile\s+uint32_t\s+g_last_frame_ms;",
        r"static\s+volatile\s+uint32_t\s+g_motor_lease_remaining_ms;",
        r"static\s+volatile\s+bool\s+g_motor_lease_expired;",
    ):
        assert re.search(declaration, framework)
