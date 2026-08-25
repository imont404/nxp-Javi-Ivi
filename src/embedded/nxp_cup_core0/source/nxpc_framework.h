#ifndef NXPC_FRAMEWORK_H_
#define NXPC_FRAMEWORK_H_

#include <stdbool.h>
#include <stdint.h>

#include "nxpc_system.h"

void nxpc_framework__init(void);
void nxpc_framework__service(void);
uint16_t *nxpc_framework__take_latest_frame(void);
void nxpc_framework__begin_callback(void);
void nxpc_framework__end_callback(void);
void nxpc_framework__no_callback(void);
void nxpc_framework__finish_frame(uint16_t *frame);
bool nxpc_framework__select_test_page(nxpc_test_page_t page);

#endif /* NXPC_FRAMEWORK_H_ */
