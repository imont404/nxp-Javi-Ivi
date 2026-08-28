#include "coins.h"

/* --- Deteccion --- */

#define COIN_SAT_MIN   90U
#define COIN_VAL_MIN   40U
#define COIN_MIN_WIDTH 6

/* Margen dentro de la pista: monedas pegadas al borde se ignoran porque
 * suelen ser reflejos o cosas fuera de pista, no monedas reales. */
#define TRACK_MARGIN 3

#define HUE_RED_HI_A    18U
#define HUE_RED_LO_B    238U
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

static void push_coin(coin_list_t *out, coin_color_t color,
                       int32_t start, int32_t end,
                       int32_t track_left, int32_t track_right)
{
    int32_t center;

    if ((end - start + 1) < COIN_MIN_WIDTH) { return; }
    if (out->count >= COINS_MAX) { return; }

    center = (start + end) / 2;

    /* Solo cuentan las monedas que estan sobre la pista */
    if (center < (track_left + TRACK_MARGIN)) { return; }
    if (center > (track_right - TRACK_MARGIN)) { return; }

    out->items[out->count].color = color;
    out->items[out->count].left_edge = start;
    out->items[out->count].right_edge = end;
    out->items[out->count].center = center;
    out->count++;
}

void coins_scan_row(const color_features_t *scanline, uint16_t width,
                     int32_t track_left, int32_t track_right, coin_list_t *out)
{
    coin_color_t run_color = COIN_NONE;
    int32_t run_start = -1;
    uint16_t x;

    out->count = 0U;

    for (x = 0U; x < width; x++)
    {
        coin_color_t c = COIN_NONE;

        if ((scanline[x].s >= COIN_SAT_MIN) && (scanline[x].v >= COIN_VAL_MIN))
        {
            c = classify_hue(scanline[x].h);
        }

        if (c != run_color)
        {
            if ((run_color != COIN_NONE) && (run_start >= 0))
            {
                push_coin(out, run_color, run_start, (int32_t)x - 1,
                           track_left, track_right);
            }
            run_color = c;
            run_start = (c != COIN_NONE) ? (int32_t)x : -1;
        }
    }

    if ((run_color != COIN_NONE) && (run_start >= 0))
    {
        push_coin(out, run_color, run_start, (int32_t)width - 1,
                   track_left, track_right);
    }
}

/* --- Decision --- */

#define RED_CLEARANCE 10

/* Tope de desvio. Sin esto, una deteccion falsa manda el carro fuera de pista. */
#define MAX_OFFSET 45

static bool blocked_by_red(const coin_list_t *coins, int32_t x)
{
    uint8_t i;

    for (i = 0U; i < coins->count; i++)
    {
        if (coins->items[i].color != COIN_RED) { continue; }

        if ((x >= (coins->items[i].left_edge - RED_CLEARANCE)) &&
            (x <= (coins->items[i].right_edge + RED_CLEARANCE)))
        {
            return true;
        }
    }

    return false;
}

/* Decide el objetivo de UNA fila. Devuelve false si esa fila no aporta nada. */
static bool decide_row(const coin_row_t *row, int32_t *out_target, coin_color_t *out_color)
{
    int32_t target = row->center;
    int32_t best_dist = 0;
    bool has_good = false;
    coin_color_t color = COIN_NONE;
    uint8_t i;

    if (!row->found || (row->coins.count == 0U)) { return false; }

    /* Moneda buena mas cercana que no este tapada por una roja */
    for (i = 0U; i < row->coins.count; i++)
    {
        int32_t d;

        if ((row->coins.items[i].color != COIN_YELLOW) &&
            (row->coins.items[i].color != COIN_BLUE))
        {
            continue;
        }
        if (blocked_by_red(&row->coins, row->coins.items[i].center)) { continue; }

        d = row->coins.items[i].center - row->center;
        if (d < 0) { d = -d; }

        if (!has_good || (d < best_dist))
        {
            target = row->coins.items[i].center;
            best_dist = d;
            color = row->coins.items[i].color;
            has_good = true;
        }
    }

    /* Esquivar rojas: corre SIEMPRE, tambien cuando ya hay objetivo bueno */
    if (blocked_by_red(&row->coins, target))
    {
        int32_t left_exit = -1;
        int32_t right_exit = -1;
        int32_t x;

        for (x = target; x >= row->track_left; x--)
        {
            if (!blocked_by_red(&row->coins, x)) { left_exit = x; break; }
        }
        for (x = target; x <= row->track_right; x++)
        {
            if (!blocked_by_red(&row->coins, x)) { right_exit = x; break; }
        }

        if ((left_exit < 0) && (right_exit < 0)) { return false; }

        if (left_exit < 0)       { target = right_exit; }
        else if (right_exit < 0) { target = left_exit; }
        else
        {
            target = ((target - left_exit) <= (right_exit - target)) ? left_exit : right_exit;
        }

        color = COIN_RED;
        has_good = true;
    }

    if (!has_good) { return false; }

    if (target < row->track_left)  { target = row->track_left; }
    if (target > row->track_right) { target = row->track_right; }

    *out_target = target;
    *out_color = color;
    return true;
}

coin_decision_t coins_decide(const coin_row_t *rows, uint8_t row_count)
{
    coin_decision_t d;
    uint8_t i;

    d.offset = 0;
    d.acting_on = COIN_NONE;
    d.target_x = -1;
    d.target_row = -1;

    /* Se recorre de la fila mas cercana a la mas lejana y se usa la primera
     * que tenga algo que decir. Lo cercano manda: una moneda a punto de
     * pasar debajo del carro importa mas que una que se ve al fondo. */
    for (i = row_count; i > 0U; i--)
    {
        uint8_t idx = i - 1U;
        int32_t target;
        coin_color_t color;

        if (!decide_row(&rows[idx], &target, &color)) { continue; }

        d.offset = target - rows[idx].center;
        d.acting_on = color;
        d.target_x = target;
        d.target_row = (int32_t)idx;
        break;
    }

    if (d.offset > MAX_OFFSET)  { d.offset = MAX_OFFSET; }
    if (d.offset < -MAX_OFFSET) { d.offset = -MAX_OFFSET; }

    return d;
}