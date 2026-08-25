#ifndef NXPC_BUTTON_H_
#define NXPC_BUTTON_H_

#include <stdbool.h>
#include <stdint.h>

#define BUTTON_POLARITY_LOW_ACTIVE (0U)
#define BUTTON_POLARITY_HIGH_ACTIVE (1U)

typedef enum
{
    BUTTON_ID_LEFT = 0,
    BUTTON_ID_RIGHT,
    BUTTON_ID_CENTER,
    BUTTON_ID_COUNT
} button_id_t;

typedef struct
{
    bool held;
    bool release_pending;
    uint32_t press_sequence;
    uint32_t release_sequence;
    uint32_t release_hold_ms;
} button_state_snapshot_t;

typedef struct
{
    button_state_snapshot_t button[BUTTON_ID_COUNT];
} button_snapshot_t;

/* Configure one board button before the 1 ms system tick is enabled. */
void button__configure(button_id_t id, uint8_t io_port, uint8_t io_bit, uint8_t polarity,
                       uint32_t debounce_time_ms);

/* Sole event producer. Called once from the system tick interrupt. */
void button__process_all(uint32_t process_time_ms);

/* Copy all held states and event epochs in one interrupt-coherent operation. */
void button__snapshot(button_snapshot_t *snapshot);

#endif /* NXPC_BUTTON_H_ */
