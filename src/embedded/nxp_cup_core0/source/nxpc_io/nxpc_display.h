#ifndef NXPC_DISPLAY_H_
#define NXPC_DISPLAY_H_

#include <stdint.h>

#define NXPC_DISPLAY_WIDTH ((uint16_t)320U)
#define NXPC_DISPLAY_HEIGHT ((uint16_t)240U)

void nxpc_display__init(void);
void nxpc_display__write(const uint8_t *buffer, uint32_t length, uint32_t x0, uint32_t x1,
                         uint32_t y0, uint32_t y1);

#endif /* NXPC_DISPLAY_H_ */
