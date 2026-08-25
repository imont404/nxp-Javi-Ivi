#ifndef TEST_MODE_H_
#define TEST_MODE_H_

#include <stdint.h>

/* Dispatch one accepted TEST frame to the selected student-reference page. */
void test_mode_on_frame(uint16_t *frame);

#endif /* TEST_MODE_H_ */
