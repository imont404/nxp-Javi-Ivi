#include "nxpc__line_processor.h"


/*
 * The generator keeps the 256 KiB data artifact reproducible and reviewable:
 *
 *   uv run python scripts/tools/generate_rgb565_color_lut.py
 *   uv run python scripts/tools/generate_rgb565_color_lut.py --check
 *
 * const places the table in flash; no camera-buffer SRAM is consumed.
 */
const uint32_t nxpc_rgb565_yhsv_lut[NXPC_RGB565_COLOR_LUT_ENTRY_COUNT] = {
#include "nxpc_rgb565_yhsv_lut.inc"
};


void nxpc__convert_rgb565_to_yhsv(const uint16_t *input,
                                 nxpc_color_features_t *output,
                                 uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        output[i] = nxpc__rgb565_to_yhsv(input[i]);
    }
}


void nxpc__convert_rgb565_to_y8(const uint16_t *input,
                               uint8_t *output,
                               uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        output[i] = nxpc__rgb565_to_yhsv(input[i]).y;
    }
}


uint32_t nxpc__detect_edges(const uint8_t *filter,
                           uint32_t length,
                           edge_t *edges,
                           uint32_t max_edges)
{
    uint32_t edge_count = 0;

    if ((length < 2u) || (max_edges == 0u))
    {
        return 0;
    }

    for (uint32_t i = 1; (i < length) && (edge_count < max_edges); i++)
    {
        if ((filter[i - 1u] == 0u) && (filter[i] == 1u))
        {
            edges[edge_count].position = (uint16_t)i;
            edges[edge_count].type = 1u;
            edge_count++;
        }
        else if ((filter[i - 1u] == 1u) && (filter[i] == 0u))
        {
            edges[edge_count].position = (uint16_t)i;
            edges[edge_count].type = 0u;
            edge_count++;
        }
    }

    return edge_count;
}


uint32_t nxpc__find_line_segments(const uint8_t *filter,
                                 uint32_t length,
                                 line_segment_t *segments,
                                 uint32_t max_segments)
{
    uint32_t segment_count = 0;
    uint32_t segment_start = 0;
    bool in_segment = false;

    for (uint32_t i = 0; (i < length) && (segment_count < max_segments); i++)
    {
        if ((filter[i] != 0u) && !in_segment)
        {
            segment_start = i;
            in_segment = true;
        }
        else if ((filter[i] == 0u) && in_segment)
        {
            segments[segment_count].start = (uint16_t)segment_start;
            segments[segment_count].end = (uint16_t)(i - 1u);
            segments[segment_count].width = (uint16_t)(i - segment_start);
            segments[segment_count].center = (uint16_t)((segment_start + i - 1u) / 2u);
            segment_count++;
            in_segment = false;
        }
    }

    if (in_segment && (segment_count < max_segments))
    {
        segments[segment_count].start = (uint16_t)segment_start;
        segments[segment_count].end = (uint16_t)(length - 1u);
        segments[segment_count].width = (uint16_t)(length - segment_start);
        segments[segment_count].center = (uint16_t)((segment_start + length - 1u) / 2u);
        segment_count++;
    }

    return segment_count;
}
