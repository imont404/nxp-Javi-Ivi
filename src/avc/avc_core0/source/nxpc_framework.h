#ifndef NXPC_FRAMEWORK_H_
#define NXPC_FRAMEWORK_H_

#include <stdint.h>

#include "avc_system.h"

void nxpc_framework__init(void);
void nxpc_framework__service(void);
uint16_t *nxpc_framework__take_latest_frame(void);
void nxpc_framework__begin_callback(void);
void nxpc_framework__end_callback(void);
void nxpc_framework__finish_frame(uint16_t *frame);

#endif /* NXPC_FRAMEWORK_H_ */
