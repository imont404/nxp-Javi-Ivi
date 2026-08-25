#ifndef NXPC_COLOR_H_
#define NXPC_COLOR_H_

#include <stdint.h>

#define NXPC_RGB565_COLOR_LUT_ENTRY_COUNT (65536U)

typedef struct
{
    uint8_t y;
    uint8_t h;
    uint8_t s;
    uint8_t v;
} nxpc_color_features_t;

extern const uint32_t nxpc_rgb565_yhsv_lut[NXPC_RGB565_COLOR_LUT_ENTRY_COUNT];

static inline nxpc_color_features_t nxpc_color__rgb565_to_yhsv(uint16_t pixel)
{
    uint32_t packed = nxpc_rgb565_yhsv_lut[pixel];
    nxpc_color_features_t features = {
        .y = (uint8_t)(packed >> 0U),
        .h = (uint8_t)(packed >> 8U),
        .s = (uint8_t)(packed >> 16U),
        .v = (uint8_t)(packed >> 24U),
    };
    return features;
}

#endif /* NXPC_COLOR_H_ */
