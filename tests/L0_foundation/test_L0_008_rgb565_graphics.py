"""L0_008 - exercise the bounded RGB565 rasterizer against canary buffers."""

import shutil
import subprocess
from pathlib import Path

import pytest


REPO = Path(__file__).resolve().parents[2]
SOURCE = REPO / "src/embedded/nxp_cup_core0/source"
GRAPHICS = SOURCE / "nxpc_graphics.c"
FONT = REPO / "src/common/egfx/src/Core/Fonts/FONT_5_7_1BPP.c"
EGFX_INCLUDE = REPO / "src/common/egfx/src"
NXPC_CONFIG_INCLUDE = SOURCE / "nxpc_config"


HARNESS = r"""
#include <limits.h>
#include <stdint.h>
#include <stdio.h>

#include "nxp_cup.h"
#include "nxpc_graphics.h"

#define GUARD 8
#define WIDTH 8
#define HEIGHT 6
#define STRIDE 10
#define CANARY 0xA55Au
#define COLOR 0x5AA5u

static uint16_t storage[GUARD + (STRIDE * HEIGHT) + GUARD];
static uint16_t camera_storage[GUARD + (CAMERA_WIDTH * CAMERA_HEIGHT) + GUARD];

static int fail(const char *message)
{
    (void)fprintf(stderr, "%s\n", message);
    return 1;
}

static void reset_storage(void)
{
    unsigned index;
    for (index = 0; index < sizeof(storage) / sizeof(storage[0]); index++)
    {
        storage[index] = CANARY;
    }
}

static int canaries_ok(void)
{
    unsigned index;
    unsigned y;

    for (index = 0; index < GUARD; index++)
    {
        if ((storage[index] != CANARY) ||
            (storage[GUARD + (STRIDE * HEIGHT) + index] != CANARY))
        {
            return 0;
        }
    }
    for (y = 0; y < HEIGHT; y++)
    {
        if ((storage[GUARD + (y * STRIDE) + WIDTH] != CANARY) ||
            (storage[GUARD + (y * STRIDE) + WIDTH + 1] != CANARY))
        {
            return 0;
        }
    }
    return 1;
}

int main(void)
{
    nxpc_rgb565_surface_t surface = {
        .pixels = &storage[GUARD],
        .width = WIDTH,
        .height = HEIGHT,
        .stride_pixels = STRIDE,
    };
    unsigned x;
    unsigned y;

    {
        nxpc_rgb565_surface_t invalid = surface;

        reset_storage();
        invalid.width = 0;
        nxpc_graphics__fill_rectangle(&invalid, INT_MIN, INT_MIN, INT_MAX, INT_MAX, COLOR);
        invalid = surface;
        invalid.stride_pixels = WIDTH - 1;
        nxpc_graphics__line(&invalid, INT_MIN, INT_MIN, INT_MAX, INT_MAX, COLOR);
        if (!canaries_ok()) return fail("invalid surface descriptor changed storage");
    }

    reset_storage();
    nxpc_graphics__pixel(NULL, 0, 0, COLOR);
    nxpc_graphics__pixel(&surface, -1, 0, COLOR);
    nxpc_graphics__pixel(&surface, WIDTH, HEIGHT, COLOR);
    if (!canaries_ok()) return fail("out-of-range pixel changed a canary");

    nxpc_graphics__fill_rectangle(&surface, -100, -100, 100, 100, COLOR);
    for (y = 0; y < HEIGHT; y++)
    {
        for (x = 0; x < WIDTH; x++)
        {
            if (surface.pixels[(y * STRIDE) + x] != COLOR)
                return fail("clipped fill missed a visible pixel");
        }
    }
    if (!canaries_ok()) return fail("clipped fill changed a canary");

    reset_storage();
    nxpc_graphics__horizontal_line(&surface, -20, 20, 2, COLOR);
    nxpc_graphics__vertical_line(&surface, 3, -20, 20, COLOR);
    if (!canaries_ok()) return fail("axis line changed a canary");
    for (x = 0; x < WIDTH; x++)
        if (surface.pixels[(2 * STRIDE) + x] != COLOR)
            return fail("horizontal line clipping failed");
    for (y = 0; y < HEIGHT; y++)
        if (surface.pixels[(y * STRIDE) + 3] != COLOR)
            return fail("vertical line clipping failed");

    reset_storage();
    nxpc_graphics__horizontal_line(&surface, INT_MAX, INT_MIN, 1, COLOR);
    nxpc_graphics__vertical_line(&surface, 4, INT_MAX, INT_MIN, COLOR);
    nxpc_graphics__fill_rectangle(&surface, INT_MAX, INT_MAX, INT_MIN, INT_MIN, COLOR);
    if (!canaries_ok()) return fail("extreme reversed primitives changed a canary");
    for (y = 0; y < HEIGHT; y++)
        for (x = 0; x < WIDTH; x++)
            if (surface.pixels[(y * STRIDE) + x] != COLOR)
                return fail("extreme reversed rectangle did not clip to the surface");

    reset_storage();
    nxpc_graphics__line(&surface, INT_MIN, INT_MIN, INT_MAX, INT_MAX, COLOR);
    if (!canaries_ok()) return fail("general line changed a canary");
    for (x = 0; x < HEIGHT; x++)
        if (surface.pixels[(x * STRIDE) + x] != COLOR)
            return fail("general line clipping failed");

    reset_storage();
    nxpc_graphics__line(&surface, -100, 0, -100, HEIGHT - 1, COLOR);
    for (y = 0; y < HEIGHT; y++)
        for (x = 0; x < WIDTH; x++)
            if (surface.pixels[(y * STRIDE) + x] != CANARY)
                return fail("rejected line drew a pixel");

    reset_storage();
    for (x = 0; x < 1024; x++)
    {
        int end_y = (int)(x % 401) - 200;
        nxpc_graphics__line(&surface, -1000, -200, 1000, end_y, COLOR);
    }
    nxpc_graphics__line(&surface, -100, HEIGHT + 20, WIDTH + 20, -100, COLOR);
    nxpc_graphics__line(&surface, WIDTH + 20, -100, -100, HEIGHT + 20, COLOR);
    if (!canaries_ok()) return fail("corner-crossing lines changed a canary");

    {
        uint16_t text_storage[8 + (302 * 10) + 8];
        nxpc_rgb565_surface_t text_surface = {
            .pixels = &text_storage[8],
            .width = 300,
            .height = 10,
            .stride_pixels = 302,
        };
        const char *long_text =
            "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!";
        unsigned index;

        for (index = 0; index < sizeof(text_storage) / sizeof(text_storage[0]); index++)
            text_storage[index] = CANARY;
        nxpc_graphics__text(&text_surface, 0, 0, long_text, COLOR);
        if (text_surface.pixels[284] != COLOR)
            return fail("48th text character was not rendered");
        if (text_surface.pixels[290] != CANARY)
            return fail("text exceeded the 48-character bound");
        nxpc_graphics__text(&text_surface, -3, -3, "CLIPPED", COLOR);
        for (index = 0; index < 8; index++)
            if ((text_storage[index] != CANARY) ||
                (text_storage[8 + (302 * 10) + index] != CANARY))
                return fail("text changed an outer canary");
        for (y = 0; y < 10; y++)
            if ((text_surface.pixels[(y * 302) + 300] != CANARY) ||
                (text_surface.pixels[(y * 302) + 301] != CANARY))
                return fail("text changed row padding");

        for (index = 0; index < sizeof(text_storage) / sizeof(text_storage[0]); index++)
            text_storage[index] = CANARY;
        nxpc_graphics__text(&text_surface, 0, 0, "!\x01!", COLOR);
        if (text_surface.pixels[2] != COLOR)
            return fail("text did not render before a non-printable byte");
        if (text_surface.pixels[14] != CANARY)
            return fail("text continued after a non-printable byte");
    }

    for (x = 0; x < sizeof(camera_storage) / sizeof(camera_storage[0]); x++)
        camera_storage[x] = CANARY;
    frame_draw_pixel(&camera_storage[GUARD], -1, -1, COLOR);
    frame_draw_line(&camera_storage[GUARD], INT_MIN, INT_MIN, INT_MAX, INT_MAX, COLOR);
    frame_fill_rectangle(&camera_storage[GUARD], 10, 10, 12, 12, COLOR);
    frame_draw_text(&camera_storage[GUARD], CAMERA_WIDTH - 2, CAMERA_HEIGHT - 2, "EDGE", COLOR);
    for (x = 0; x < GUARD; x++)
        if ((camera_storage[x] != CANARY) ||
            (camera_storage[GUARD + (CAMERA_WIDTH * CAMERA_HEIGHT) + x] != CANARY))
            return fail("public frame wrapper changed a canary");

    return 0;
}
"""


def test_rgb565_rasterizer_is_clipped_and_bounded(tmp_path: Path):
    compiler = shutil.which("clang") or shutil.which("gcc") or shutil.which("cc")
    if compiler is None:
        pytest.skip("a native C compiler is required for the RGB565 canary test")

    harness = tmp_path / "rgb565_canary.c"
    executable = tmp_path / ("rgb565_canary.exe" if Path(compiler).suffix.lower() == ".exe" else "rgb565_canary")
    harness.write_text(HARNESS, encoding="utf-8")

    compile_result = subprocess.run(
        [
            compiler,
            "-std=c99",
            "-Wall",
            "-Wextra",
            str(harness),
            str(GRAPHICS),
            str(FONT),
            f"-I{SOURCE}",
            f"-I{EGFX_INCLUDE}",
            f"-I{NXPC_CONFIG_INCLUDE}",
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
