#ifndef LPSPI1_H_
#define LPSPI1_H_

#include <stdint.h>
#include "fsl_lpspi.h"
#include "fsl_lpspi_edma.h"

void lpspi1_init(uint8_t transaction_bits);
/* Asynchronous: the source must remain unchanged until lpspi1_wait_idle(). */
void lpspi1_transfer_block(const void *block, uint32_t block_size);
/* Synchronous because next_byte is local to the helper. */
void lpspi1_transfer_byte(uint8_t next_byte);
void lpspi1_wait_idle(void);
uint8_t lpspi1_get_frame_size(void);
void lpspi1_report_clock(void);

#endif
