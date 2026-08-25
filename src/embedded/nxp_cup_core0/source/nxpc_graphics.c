#include "nxpc_graphics.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "eGFX.h"
#include "nxp_cup.h"

#define NXPC_GRAPHICS_TEXT_MAX_BYTES (48U)
#define NXPC_GRAPHICS_SURFACE_LIMIT (4096)
#define NXPC_GRAPHICS_COORDINATE_LIMIT (1048576)

#define NXPC_CLIP_LEFT (1U << 0)
#define NXPC_CLIP_RIGHT (1U << 1)
#define NXPC_CLIP_TOP (1U << 2)
#define NXPC_CLIP_BOTTOM (1U << 3)

static bool nxpc_graphics__valid(const nxpc_rgb565_surface_t *surface)
{
    return (surface != NULL) && (surface->pixels != NULL) && (surface->width > 0) &&
           (surface->height > 0) && (surface->width <= NXPC_GRAPHICS_SURFACE_LIMIT) &&
           (surface->height <= NXPC_GRAPHICS_SURFACE_LIMIT) &&
           (surface->stride_pixels >= surface->width) &&
           (surface->stride_pixels <= NXPC_GRAPHICS_SURFACE_LIMIT);
}

static int32_t nxpc_graphics__bounded_coordinate(int32_t coordinate)
{
    if (coordinate < -NXPC_GRAPHICS_COORDINATE_LIMIT)
    {
        return -NXPC_GRAPHICS_COORDINATE_LIMIT;
    }
    if (coordinate > NXPC_GRAPHICS_COORDINATE_LIMIT)
    {
        return NXPC_GRAPHICS_COORDINATE_LIMIT;
    }
    return coordinate;
}

static void nxpc_graphics__swap(int32_t *left, int32_t *right)
{
    int32_t temporary = *left;
    *left = *right;
    *right = temporary;
}

static uint32_t nxpc_graphics__outcode(const nxpc_rgb565_surface_t *surface, int64_t x, int64_t y)
{
    uint32_t code = 0U;

    if (x < 0)
    {
        code |= NXPC_CLIP_LEFT;
    }
    else if (x >= surface->width)
    {
        code |= NXPC_CLIP_RIGHT;
    }

    if (y < 0)
    {
        code |= NXPC_CLIP_TOP;
    }
    else if (y >= surface->height)
    {
        code |= NXPC_CLIP_BOTTOM;
    }

    return code;
}

static bool nxpc_graphics__clip_line(const nxpc_rgb565_surface_t *surface, int32_t *x0_out,
                                     int32_t *y0_out, int32_t *x1_out, int32_t *y1_out)
{
    int64_t x0 = *x0_out;
    int64_t y0 = *y0_out;
    int64_t x1 = *x1_out;
    int64_t y1 = *y1_out;
    int64_t x_max = (int64_t)surface->width - 1;
    int64_t y_max = (int64_t)surface->height - 1;

    for (;;)
    {
        uint32_t code0 = nxpc_graphics__outcode(surface, x0, y0);
        uint32_t code1 = nxpc_graphics__outcode(surface, x1, y1);
        uint32_t outside;
        int64_t x;
        int64_t y;

        if ((code0 | code1) == 0U)
        {
            *x0_out = (int32_t)x0;
            *y0_out = (int32_t)y0;
            *x1_out = (int32_t)x1;
            *y1_out = (int32_t)y1;
            return true;
        }
        if ((code0 & code1) != 0U)
        {
            return false;
        }

        outside = (code0 != 0U) ? code0 : code1;
        if ((outside & NXPC_CLIP_TOP) != 0U)
        {
            if (y1 == y0)
            {
                return false;
            }
            x = x0 + ((x1 - x0) * -y0) / (y1 - y0);
            y = 0;
        }
        else if ((outside & NXPC_CLIP_BOTTOM) != 0U)
        {
            if (y1 == y0)
            {
                return false;
            }
            x = x0 + ((x1 - x0) * (y_max - y0)) / (y1 - y0);
            y = y_max;
        }
        else if ((outside & NXPC_CLIP_RIGHT) != 0U)
        {
            if (x1 == x0)
            {
                return false;
            }
            y = y0 + ((y1 - y0) * (x_max - x0)) / (x1 - x0);
            x = x_max;
        }
        else
        {
            if (x1 == x0)
            {
                return false;
            }
            y = y0 + ((y1 - y0) * -x0) / (x1 - x0);
            x = 0;
        }

        if (outside == code0)
        {
            x0 = x;
            y0 = y;
        }
        else
        {
            x1 = x;
            y1 = y;
        }
    }
}

void nxpc_graphics__pixel(const nxpc_rgb565_surface_t *surface, int32_t x, int32_t y,
                          uint16_t color)
{
    if (!nxpc_graphics__valid(surface) || (x < 0) || (x >= surface->width) || (y < 0) ||
        (y >= surface->height))
    {
        return;
    }

    surface->pixels[((size_t)y * (size_t)surface->stride_pixels) + (size_t)x] = color;
}

void nxpc_graphics__horizontal_line(const nxpc_rgb565_surface_t *surface, int32_t x0, int32_t x1,
                                    int32_t y, uint16_t color)
{
    int32_t x;
    uint16_t *row;

    if (!nxpc_graphics__valid(surface) || (y < 0) || (y >= surface->height))
    {
        return;
    }
    if (x1 < x0)
    {
        nxpc_graphics__swap(&x0, &x1);
    }
    if ((x1 < 0) || (x0 >= surface->width))
    {
        return;
    }
    if (x0 < 0)
    {
        x0 = 0;
    }
    if (x1 >= surface->width)
    {
        x1 = surface->width - 1;
    }

    row = &surface->pixels[(size_t)y * (size_t)surface->stride_pixels];
    for (x = x0; x <= x1; x++)
    {
        row[x] = color;
    }
}

void nxpc_graphics__vertical_line(const nxpc_rgb565_surface_t *surface, int32_t x, int32_t y0,
                                  int32_t y1, uint16_t color)
{
    int32_t y;

    if (!nxpc_graphics__valid(surface) || (x < 0) || (x >= surface->width))
    {
        return;
    }
    if (y1 < y0)
    {
        nxpc_graphics__swap(&y0, &y1);
    }
    if ((y1 < 0) || (y0 >= surface->height))
    {
        return;
    }
    if (y0 < 0)
    {
        y0 = 0;
    }
    if (y1 >= surface->height)
    {
        y1 = surface->height - 1;
    }

    for (y = y0; y <= y1; y++)
    {
        surface->pixels[((size_t)y * (size_t)surface->stride_pixels) + (size_t)x] = color;
    }
}

void nxpc_graphics__line(const nxpc_rgb565_surface_t *surface, int32_t x0, int32_t y0, int32_t x1,
                         int32_t y1, uint16_t color)
{
    int64_t delta_x;
    int64_t delta_y;
    int64_t error;
    int32_t step_x;
    int32_t step_y;

    x0 = nxpc_graphics__bounded_coordinate(x0);
    y0 = nxpc_graphics__bounded_coordinate(y0);
    x1 = nxpc_graphics__bounded_coordinate(x1);
    y1 = nxpc_graphics__bounded_coordinate(y1);

    if (!nxpc_graphics__valid(surface) || !nxpc_graphics__clip_line(surface, &x0, &y0, &x1, &y1))
    {
        return;
    }

    delta_x = (x1 >= x0) ? (int64_t)x1 - x0 : (int64_t)x0 - x1;
    delta_y = (y1 >= y0) ? (int64_t)y0 - y1 : (int64_t)y1 - y0;
    step_x = (x0 < x1) ? 1 : -1;
    step_y = (y0 < y1) ? 1 : -1;
    error = delta_x + delta_y;

    for (;;)
    {
        int64_t twice_error;

        surface->pixels[((size_t)y0 * (size_t)surface->stride_pixels) + (size_t)x0] = color;
        if ((x0 == x1) && (y0 == y1))
        {
            return;
        }

        twice_error = error * 2;
        if (twice_error >= delta_y)
        {
            error += delta_y;
            x0 += step_x;
        }
        if (twice_error <= delta_x)
        {
            error += delta_x;
            y0 += step_y;
        }
    }
}

void nxpc_graphics__fill_rectangle(const nxpc_rgb565_surface_t *surface, int32_t x0, int32_t y0,
                                   int32_t x1, int32_t y1, uint16_t color)
{
    int32_t y;

    if (!nxpc_graphics__valid(surface))
    {
        return;
    }
    if (x1 < x0)
    {
        nxpc_graphics__swap(&x0, &x1);
    }
    if (y1 < y0)
    {
        nxpc_graphics__swap(&y0, &y1);
    }
    if ((x1 < 0) || (y1 < 0) || (x0 >= surface->width) || (y0 >= surface->height))
    {
        return;
    }
    if (x0 < 0)
    {
        x0 = 0;
    }
    if (y0 < 0)
    {
        y0 = 0;
    }
    if (x1 >= surface->width)
    {
        x1 = surface->width - 1;
    }
    if (y1 >= surface->height)
    {
        y1 = surface->height - 1;
    }

    for (y = y0; y <= y1; y++)
    {
        nxpc_graphics__horizontal_line(surface, x0, x1, y, color);
    }
}

static void nxpc_graphics__font_text(const nxpc_rgb565_surface_t *surface,
                                     int32_t x,
                                     int32_t y,
                                     const char *text,
                                     uint16_t color,
                                     const eGFX_Font *font)
{
    uint32_t character_index;
    int32_t cursor_x = x;

    if (!nxpc_graphics__valid(surface) || (text == NULL) || (font == NULL))
    {
        return;
    }

    cursor_x = nxpc_graphics__bounded_coordinate(cursor_x);
    y = nxpc_graphics__bounded_coordinate(y);

    for (character_index = 0U; character_index < NXPC_GRAPHICS_TEXT_MAX_BYTES; character_index++)
    {
        uint8_t character = (uint8_t)text[character_index];
        const eGFX_ImagePlane *glyph;
        uint32_t row;
        uint32_t row_bytes;

        if (character == 0U)
        {
            return;
        }
        if ((character < 0x20U) || (character > 0x7EU))
        {
            return;
        }

        glyph = font->CharacterSprites[character - 0x20U];
        if (character != 0x20U)
        {
            row_bytes = (glyph->SizeX + 7U) / 8U;
            for (row = 0U; row < glyph->SizeY; row++)
            {
                uint32_t column;

                for (column = 0U; column < glyph->SizeX; column++)
                {
                    uint8_t bits = glyph->Data[(row * row_bytes) + (column >> 3U)];
                    if ((bits & (uint8_t)(1U << (column & 7U))) != 0U)
                    {
                        nxpc_graphics__pixel(surface, cursor_x + (int32_t)column, y + (int32_t)row,
                                             color);
                    }
                }
            }
        }

        cursor_x += (int32_t)glyph->SizeX + 1;
        if ((cursor_x >= surface->width) && (x >= 0))
        {
            return;
        }
    }
}

static int32_t nxpc_graphics__font_text_width(const char *text, const eGFX_Font *font)
{
    uint32_t character_index;
    int32_t width = 0;

    if ((text == NULL) || (font == NULL))
    {
        return 0;
    }

    for (character_index = 0U; character_index < NXPC_GRAPHICS_TEXT_MAX_BYTES; character_index++)
    {
        uint8_t character = (uint8_t)text[character_index];
        const eGFX_ImagePlane *glyph;

        if ((character == 0U) || (character < 0x20U) || (character > 0x7EU))
        {
            return width;
        }
        glyph = font->CharacterSprites[character - 0x20U];
        if (character_index != 0U)
        {
            width++;
        }
        width += (int32_t)glyph->SizeX;
    }
    return width;
}

void nxpc_graphics__text(const nxpc_rgb565_surface_t *surface, int32_t x, int32_t y,
                         const char *text, uint16_t color)
{
    nxpc_graphics__font_text(surface, x, y, text, color, &FONT_5_7_1BPP);
}

void nxpc_graphics__text_large(const nxpc_rgb565_surface_t *surface, int32_t x, int32_t y,
                               const char *text, uint16_t color)
{
    nxpc_graphics__font_text(surface, x, y, text, color, &FONT_10_14_1BPP);
}

int32_t nxpc_graphics__text_large_width(const char *text)
{
    return nxpc_graphics__font_text_width(text, &FONT_10_14_1BPP);
}

static nxpc_rgb565_surface_t nxpc_graphics__camera_surface(uint16_t *frame)
{
    nxpc_rgb565_surface_t surface = {
        .pixels = frame,
        .width = (int32_t)CAMERA_WIDTH,
        .height = (int32_t)CAMERA_HEIGHT,
        .stride_pixels = (int32_t)CAMERA_STRIDE_PIXELS,
    };
    return surface;
}

void frame_draw_pixel(uint16_t *frame, int32_t x, int32_t y, uint16_t color)
{
    nxpc_rgb565_surface_t surface = nxpc_graphics__camera_surface(frame);
    nxpc_graphics__pixel(&surface, x, y, color);
}

void frame_draw_horizontal_line(uint16_t *frame, int32_t x0, int32_t x1, int32_t y, uint16_t color)
{
    nxpc_rgb565_surface_t surface = nxpc_graphics__camera_surface(frame);
    nxpc_graphics__horizontal_line(&surface, x0, x1, y, color);
}

void frame_draw_vertical_line(uint16_t *frame, int32_t x, int32_t y0, int32_t y1, uint16_t color)
{
    nxpc_rgb565_surface_t surface = nxpc_graphics__camera_surface(frame);
    nxpc_graphics__vertical_line(&surface, x, y0, y1, color);
}

void frame_draw_line(uint16_t *frame, int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                     uint16_t color)
{
    nxpc_rgb565_surface_t surface = nxpc_graphics__camera_surface(frame);
    nxpc_graphics__line(&surface, x0, y0, x1, y1, color);
}

void frame_fill_rectangle(uint16_t *frame, int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                          uint16_t color)
{
    nxpc_rgb565_surface_t surface = nxpc_graphics__camera_surface(frame);
    nxpc_graphics__fill_rectangle(&surface, x0, y0, x1, y1, color);
}

void frame_draw_text(uint16_t *frame, int32_t x, int32_t y, const char *text, uint16_t color)
{
    nxpc_rgb565_surface_t surface = nxpc_graphics__camera_surface(frame);
    nxpc_graphics__text(&surface, x, y, text, color);
}
