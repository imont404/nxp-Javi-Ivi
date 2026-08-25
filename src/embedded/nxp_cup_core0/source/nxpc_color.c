#include "nxpc_color.h"

/*
 * Regenerate or verify the flash-resident 256 KiB lookup table with:
 *
 *   uv run python scripts/tools/generate_rgb565_color_lut.py
 *   uv run python scripts/tools/generate_rgb565_color_lut.py --check
 */
const uint32_t nxpc_rgb565_yhsv_lut[NXPC_RGB565_COLOR_LUT_ENTRY_COUNT] = {
#include "nxpc_rgb565_yhsv_lut.inc"
};
