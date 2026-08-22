#ifndef AVC__LINE_PROCESSOR_H_
#define AVC__LINE_PROCESSOR_H_

#include <stdbool.h>
#include <stdint.h>


#define AVC_RGB565_COLOR_LUT_ENTRY_COUNT (65536u)

/*
 * Generic uint8 color features for one RGB565 pixel.
 *
 * Hue is circular: 0..255 maps to 0..<360 degrees. Saturation, value, and
 * BT.601 luma use 0..255. The table supplies color-space conversion only; it
 * deliberately does not classify track or obstacle colors.
 */
typedef struct
{
    uint8_t y;
    uint8_t h;
    uint8_t s;
    uint8_t v;
} avc_color_features_t;

extern const uint32_t avc_rgb565_yhsv_lut[AVC_RGB565_COLOR_LUT_ENTRY_COUNT];

static inline avc_color_features_t avc__rgb565_to_yhsv(uint16_t pixel)
{
    uint32_t packed = avc_rgb565_yhsv_lut[pixel];
    avc_color_features_t features = {
        .y = (uint8_t)(packed >> 0),
        .h = (uint8_t)(packed >> 8),
        .s = (uint8_t)(packed >> 16),
        .v = (uint8_t)(packed >> 24),
    };
    return features;
}

void avc__convert_rgb565_to_yhsv(const uint16_t *input,
                                 avc_color_features_t *output,
                                 uint32_t length);

void avc__convert_rgb565_to_y8(const uint16_t *input,
                               uint8_t *output,
                               uint32_t length);


typedef struct
{
    uint16_t position;
    uint8_t type; /* 0 = falling edge (1->0), 1 = rising edge (0->1). */
} edge_t;

typedef struct
{
    uint16_t start;
    uint16_t end;
    uint16_t center;
    uint16_t width;
} line_segment_t;

uint32_t avc__detect_edges(const uint8_t *filter,
                           uint32_t length,
                           edge_t *edges,
                           uint32_t max_edges);

uint32_t avc__find_line_segments(const uint8_t *filter,
                                 uint32_t length,
                                 line_segment_t *segments,
                                 uint32_t max_segments);

#endif /* AVC__LINE_PROCESSOR_H_ */
