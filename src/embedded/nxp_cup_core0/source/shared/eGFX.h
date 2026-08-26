/*
 * Minimal compatibility surface for the two generated eGFX font assets.
 *
 * The firmware owns its rasterizer and display transport. The generated font
 * files need only these two data structures and the 1-BPP type tag, so keep
 * the full eGFX renderer and UI headers out of the active build.
 */
#ifndef NXPC_MINIMAL_EGFX_H_
#define NXPC_MINIMAL_EGFX_H_

#include <stddef.h>
#include <stdint.h>

#define eGFX_IMAGE_PLANE_1BPP 0x0100U

typedef struct
{
    uint32_t Type;
    uint8_t *Data;
    uint16_t SizeX;
    uint16_t SizeY;
    void *User;
} eGFX_ImagePlane;

typedef struct
{
    const eGFX_ImagePlane *CharacterSprites[0x80U - 0x20U];
    int32_t Ascent;
    int32_t Descent;
    int32_t LineSpacing;
    uint8_t SpacesPerTab;
    uint8_t InterCharacterSpacing;
    uint8_t Padding[2];
} eGFX_Font;

#include "Core/Fonts/FONT_5_7_1BPP.h"
#include "Core/Fonts/FONT_10_14_1BPP.h"

#endif /* NXPC_MINIMAL_EGFX_H_ */
