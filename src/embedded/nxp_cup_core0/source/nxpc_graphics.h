#ifndef NXPC_GRAPHICS_H_
#define NXPC_GRAPHICS_H_

#include <stdint.h>

typedef struct
{
    uint16_t *pixels;
    int32_t width;
    int32_t height;
    int32_t stride_pixels;
} nxpc_rgb565_surface_t;

void nxpc_graphics__pixel(const nxpc_rgb565_surface_t *surface, int32_t x, int32_t y,
                          uint16_t color);
void nxpc_graphics__horizontal_line(const nxpc_rgb565_surface_t *surface, int32_t x0, int32_t x1,
                                    int32_t y, uint16_t color);
void nxpc_graphics__vertical_line(const nxpc_rgb565_surface_t *surface, int32_t x, int32_t y0,
                                  int32_t y1, uint16_t color);
void nxpc_graphics__line(const nxpc_rgb565_surface_t *surface, int32_t x0, int32_t y0, int32_t x1,
                         int32_t y1, uint16_t color);
void nxpc_graphics__fill_rectangle(const nxpc_rgb565_surface_t *surface, int32_t x0, int32_t y0,
                                   int32_t x1, int32_t y1, uint16_t color);
void nxpc_graphics__text(const nxpc_rgb565_surface_t *surface, int32_t x, int32_t y,
                         const char *text, uint16_t color);
void nxpc_graphics__text_large(const nxpc_rgb565_surface_t *surface, int32_t x, int32_t y,
                               const char *text, uint16_t color);
int32_t nxpc_graphics__text_large_width(const char *text);

#endif /* NXPC_GRAPHICS_H_ */
