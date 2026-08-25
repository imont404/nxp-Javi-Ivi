"""L0_011 - execute the real 1 ms button debouncer through bounce and holds."""

import shutil
import subprocess
from pathlib import Path

import pytest


REPO = Path(__file__).resolve().parents[2]
IO_SOURCE = REPO / "src/embedded/nxp_cup_core0/source/nxpc_io"
DRIVER_INCLUDE = REPO / "src/embedded/nxp_cup_core0/drivers"
BUTTON = IO_SOURCE / "button.c"


STUB_HEADER = r"""
#ifndef NXPC_BUTTON_TRACE_STUBS_H_
#define NXPC_BUTTON_TRACE_STUBS_H_

#include <stdint.h>

#define FSL_COMMON_H_
#define FSL_GPIO_H_
#define GPIO3 ((void *)0)

uint32_t DisableGlobalIRQ(void);
void EnableGlobalIRQ(uint32_t state);
uint32_t GPIO_PinRead(void *base, uint32_t pin);

#endif
"""


HARNESS = r"""
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "button.h"

static uint32_t pin_levels = UINT32_MAX;
static uint32_t irq_disabled;

static int fail(const char *message)
{
    (void)fprintf(stderr, "%s\n", message);
    return 1;
}

uint32_t DisableGlobalIRQ(void)
{
    uint32_t previous = irq_disabled;
    irq_disabled = 1U;
    return previous;
}
void EnableGlobalIRQ(uint32_t state) { irq_disabled = state; }
uint32_t GPIO_PinRead(void *base, uint32_t pin)
{
    (void)base;
    return (pin_levels >> pin) & 1U;
}

static void set_pressed(uint32_t pin, bool pressed)
{
    if (pressed)
        pin_levels &= ~(1UL << pin);
    else
        pin_levels |= 1UL << pin;
}

static void process(uint32_t milliseconds)
{
    uint32_t tick;
    for (tick = 0U; tick < milliseconds; tick++)
        button__process_all(1U);
}

int main(void)
{
    button_snapshot_t first;
    button_snapshot_t second;

    button__configure(BUTTON_ID_LEFT, 3U, 1U, BUTTON_POLARITY_LOW_ACTIVE, 50U);
    button__configure(BUTTON_ID_RIGHT, 3U, 2U, BUTTON_POLARITY_LOW_ACTIVE, 50U);
    button__configure(BUTTON_ID_CENTER, 3U, 3U, BUTTON_POLARITY_LOW_ACTIVE, 50U);

    set_pressed(1U, true);
    process(10U);
    button__snapshot(&first);
    if (first.button[BUTTON_ID_LEFT].held ||
        !first.button[BUTTON_ID_LEFT].release_pending)
        return fail("raw press was not pending during qualification");

    set_pressed(1U, false);
    process(5U);
    set_pressed(1U, true);
    process(50U);
    button__snapshot(&first);
    if (first.button[BUTTON_ID_LEFT].held)
        return fail("bounced press qualified too early");
    process(1U);
    button__snapshot(&first);
    if (!first.button[BUTTON_ID_LEFT].held ||
        (first.button[BUTTON_ID_LEFT].press_sequence != 1U))
        return fail("stable press did not qualify exactly once");

    process(100U);
    set_pressed(1U, false);
    process(20U);
    set_pressed(1U, true);
    process(5U);
    set_pressed(1U, false);
    process(50U);
    button__snapshot(&first);
    if (!first.button[BUTTON_ID_LEFT].held ||
        (first.button[BUTTON_ID_LEFT].release_sequence != 0U))
        return fail("bounced release qualified too early");
    process(1U);
    button__snapshot(&first);
    button__snapshot(&second);
    if (first.button[BUTTON_ID_LEFT].held ||
        first.button[BUTTON_ID_LEFT].release_pending ||
        (first.button[BUTTON_ID_LEFT].release_sequence != 1U))
        return fail("stable release did not qualify exactly once");
    if (first.button[BUTTON_ID_LEFT].release_hold_ms < 150U)
        return fail("qualified hold duration was not preserved");
    if (second.button[BUTTON_ID_LEFT].release_sequence !=
        first.button[BUTTON_ID_LEFT].release_sequence)
        return fail("snapshot destructively consumed the release epoch");

    irq_disabled = 1U;
    button__snapshot(&second);
    if (irq_disabled != 1U)
        return fail("snapshot did not restore an already-disabled IRQ state");
    irq_disabled = 0U;

    set_pressed(1U, true);
    set_pressed(2U, true);
    process(51U);
    set_pressed(1U, false);
    set_pressed(2U, false);
    process(51U);
    button__snapshot(&first);
    if ((first.button[BUTTON_ID_LEFT].release_sequence != 2U) ||
        (first.button[BUTTON_ID_RIGHT].release_sequence != 1U))
        return fail("simultaneous buttons did not retain independent epochs");

    set_pressed(3U, true);
    process(51U);
    button__process_all(UINT32_MAX);
    set_pressed(3U, false);
    process(51U);
    button__snapshot(&first);
    if (first.button[BUTTON_ID_CENTER].release_hold_ms != UINT32_MAX)
        return fail("hold duration did not saturate at UINT32_MAX");

    return 0;
}
"""


def test_real_button_debouncer_trace(tmp_path: Path):
    compiler = shutil.which("clang") or shutil.which("gcc") or shutil.which("cc")
    if compiler is None:
        pytest.skip("a native C compiler is required for the button debounce trace")

    stub = tmp_path / "nxpc_button_trace_stubs.h"
    harness = tmp_path / "nxpc_button_trace_harness.c"
    executable = tmp_path / (
        "nxpc_button_trace_harness.exe"
        if Path(compiler).suffix.lower() == ".exe"
        else "nxpc_button_trace_harness"
    )
    stub.write_text(STUB_HEADER, encoding="utf-8")
    harness.write_text(HARNESS, encoding="utf-8")

    compile_result = subprocess.run(
        [
            compiler,
            "-std=c99",
            "-Wall",
            "-Wextra",
            "-Werror",
            "-include",
            str(stub),
            str(harness),
            str(BUTTON),
            f"-I{IO_SOURCE}",
            f"-I{DRIVER_INCLUDE}",
            "-o",
            str(executable),
        ],
        cwd=REPO,
        capture_output=True,
        text=True,
        check=False,
    )
    assert compile_result.returncode == 0, compile_result.stdout + compile_result.stderr

    run_result = subprocess.run(
        [str(executable)],
        cwd=REPO,
        capture_output=True,
        text=True,
        check=False,
    )
    assert run_result.returncode == 0, run_result.stdout + run_result.stderr
