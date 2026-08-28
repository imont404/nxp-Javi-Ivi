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
    int32_t center;
    int32_t left_edge;
    int32_t right_edge;
} coin_t;

#define COINS_MAX 4
#define COIN_ROWS 3

typedef struct {
    coin_t items[COINS_MAX];
    uint8_t count;
} coin_list_t;

/* Datos de una fila: lo que vio white_center + las monedas de esa fila */
typedef struct {
    bool found;
    int32_t center;
    int32_t track_left;
    int32_t track_right;
    coin_list_t coins;
} coin_row_t;

/* Resultado: un solo desplazamiento lateral para todo el frame */
typedef struct {
    int32_t offset;          /* pixeles; positivo = mover a la derecha */
    coin_color_t acting_on;  /* que lo causo, para dibujar/depurar */
    int32_t target_x;        /* columna objetivo, para dibujar */
    int32_t target_row;      /* indice de fila que decidio, -1 si ninguna */
} coin_decision_t;

void coins_scan_row(const color_features_t *scanline, uint16_t width,
                     int32_t track_left, int32_t track_right, coin_list_t *out);

coin_decision_t coins_decide(const coin_row_t *rows, uint8_t row_count);

#endif /* COINS_H_ */