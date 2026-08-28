#include "coins.h"
#include <math.h>

/* Saturacion minima para considerar que un pixel tiene color real.
 * El blanco de la pista y el gris del pavimento quedan por debajo. */
#define COIN_SAT_MIN 90U

/* Valor (brillo) minimo: descarta sombras oscuras que tienen hue ruidoso */
#define COIN_VAL_MIN 30U

/* Ancho minimo en pixeles para no confundir ruido con una moneda */
#define COIN_MIN_WIDTH 7



/* Rangos de hue. El hue del framework es 0-255 (no 0-360).
 * Rojo cruza el wrap-around en 0, por eso son dos rangos. */
#define HUE_RED_LO_A    0U
#define HUE_RED_HI_A    20U
#define HUE_RED_LO_B    235U
#define HUE_RED_HI_B    255U
#define HUE_YELLOW_LO   30U
#define HUE_YELLOW_HI   60U
#define HUE_BLUE_LO     140U
#define HUE_BLUE_HI     180U


static coin_color_t classify_hue(uint8_t h)
{
    if ((h <= HUE_RED_HI_A) || (h >= HUE_RED_LO_B)) { return COIN_RED; }
    if ((h >= HUE_YELLOW_LO) && (h <= HUE_YELLOW_HI)) { return COIN_YELLOW; }
    if ((h >= HUE_BLUE_LO) && (h <= HUE_BLUE_HI)) { return COIN_BLUE; }
    return COIN_NONE;
}

void coins_scan_row(const color_features_t *scanline, uint16_t width, coin_list_t *out)
{
    coin_color_t run_color = COIN_NONE;
    int32_t run_start = -1;

    out->count = 0U;

    for (uint16_t x = 0U; x < width; x++)
    {
        coin_color_t c = COIN_NONE;

        if ((scanline[x].s >= COIN_SAT_MIN) && (scanline[x].v >= COIN_VAL_MIN))
        {
            c = classify_hue(scanline[x].h);
        }

        if (c != run_color)
        {
            /* Cerrar la corrida anterior si era una moneda valida */
            if ((run_color != COIN_NONE) && (run_start >= 0))
            {
                int32_t run_end = (int32_t)x - 1;
                if (((run_end - run_start + 1) >= COIN_MIN_WIDTH) && (out->count < COINS_MAX))
                {
                    out->items[out->count].color = run_color;
                    out->items[out->count].left_edge = run_start;
                    out->items[out->count].right_edge = run_end;
                    out->items[out->count].center = (run_start + run_end) / 2;
                    out->count++;
                }
            }
            run_color = c;
            run_start = (c != COIN_NONE) ? (int32_t)x : -1;
        }
    }

    /* Cerrar una corrida que llegue hasta el borde derecho */
    if ((run_color != COIN_NONE) && (run_start >= 0) && (out->count < COINS_MAX))
    {
        int32_t run_end = (int32_t)width - 1;
        if ((run_end - run_start + 1) >= COIN_MIN_WIDTH)
        {
            out->items[out->count].color = run_color;
            out->items[out->count].left_edge = run_start;
            out->items[out->count].right_edge = run_end;
            out->items[out->count].center = (run_start + run_end) / 2;
            out->count++;
        }
    }
}

/* Margen extra a cada lado de una moneda roja para no rozarla */
#define RED_CLEARANCE 8

bool coins_adjust_center(const coin_list_t *coins,
                          int32_t base_center,
                          int32_t track_left,
                          int32_t track_right,
                          int32_t *out_center)
{
    int32_t target = base_center;
    bool has_target = false;
    int32_t best_dist = 0;
    uint8_t i;

    if (coins->count == 0U)
    {
        *out_center = base_center;
        return true;
    }

    /* Paso 1: buscar la moneda buena mas cercana al centro actual.
     * Si hay varias, la mas cercana es la mas facil de alcanzar. */
    for (i = 0U; i < coins->count; i++)
    {
        if ((coins->items[i].color == COIN_YELLOW) || (coins->items[i].color == COIN_BLUE))
        {
            int32_t d = coins->items[i].center - base_center;
            if (d < 0) { d = -d; }

            if (!has_target || (d < best_dist))
            {
                target = coins->items[i].center;
                best_dist = d;
                has_target = true;
            }
        }
    }

    /* Paso 2: verificar que el objetivo no choque con una roja.
     * Si una roja bloquea el camino, la situacion es complicada:
     * devolvemos false y el llamador usa el centro normal. */
    for (i = 0U; i < coins->count; i++)
    {
        if (coins->items[i].color == COIN_RED)
        {
            int32_t rl = coins->items[i].left_edge - RED_CLEARANCE;
            int32_t rr = coins->items[i].right_edge + RED_CLEARANCE;

            if ((target >= rl) && (target <= rr))
            {
                /* El objetivo cae encima de una roja: ambiguo, ignorar monedas */
                return false;
            }
        }
    }

    /* Paso 3: si no hay moneda buena pero si rojas, esquivar.
     * Se mueve el centro al lado con mas espacio libre dentro de la pista. */
    if (!has_target)
    {
        for (i = 0U; i < coins->count; i++)
        {
            if (coins->items[i].color != COIN_RED) { continue; }

            int32_t rl = coins->items[i].left_edge - RED_CLEARANCE;
            int32_t rr = coins->items[i].right_edge + RED_CLEARANCE;

            if ((base_center >= rl) && (base_center <= rr))
            {
                int32_t space_left = rl - track_left;
                int32_t space_right = track_right - rr;

                if ((space_left <= 0) && (space_right <= 0))
                {
                    /* No cabe por ningun lado: ignorar monedas */
                    return false;
                }

                target = (space_right > space_left) ? (rr + 1) : (rl - 1);
                has_target = true;
            }
        }
    }

    /* Paso 4: el objetivo tiene que quedar dentro de la pista */
    if (target < track_left) { target = track_left; }
    if (target > track_right) { target = track_right; }

    *out_center = target;
    return true;
}