#ifndef COINS_H_
#define COINS_H_

#include <stdint.h>
#include <stdbool.h>
#include "nxp_cup.h"

typedef enum {
    COIN_NONE = 0,
    COIN_RED,
    COIN_YELLOW,
    COIN_BLUE
} coin_color_t;

typedef struct {
    coin_color_t color;
    int32_t center;      /* columna del centro de la moneda */
    int32_t left_edge;
    int32_t right_edge;
} coin_t;

#define COINS_MAX 4

typedef struct {
    coin_t items[COINS_MAX];
    uint8_t count;
} coin_list_t;

/*
 * Escanea una fila ya convertida a YHSV y devuelve las monedas encontradas.
 * No decide nada sobre la trayectoria; solo reporta que vio.
 */
void coins_scan_row(const color_features_t *scanline, uint16_t width, coin_list_t *out);

/*
 * Ajusta un centro de masa base segun las monedas de esa fila.
 *
 * base_center: el centro que dio white_center() para esa fila.
 * Devuelve true si pudo calcular un centro ajustado (lo escribe en
 * out_center); false si la situacion es ambigua y conviene ignorar las
 * monedas y usar el centro original.
 */
bool coins_adjust_center(const coin_list_t *coins,
                          int32_t base_center,
                          int32_t track_left,
                          int32_t track_right,
                          int32_t *out_center);

#endif /* COINS_H_ */