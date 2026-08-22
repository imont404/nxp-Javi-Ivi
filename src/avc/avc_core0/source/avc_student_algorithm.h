#ifndef AVC_STUDENT_ALGORITHM_H_
#define AVC_STUDENT_ALGORITHM_H_

#include <stdint.h>

/*
 * Student-owned algorithm boundary.
 *
 * The framework calls this once for each latest RGB565 camera frame while the
 * system is in STUDENT_RUNNING. Students may use the documented AVC actuator
 * APIs, but the organizer framework intentionally supplies no driving logic.
 */
void avc_student_algorithm__process_frame(uint16_t *frame);

#endif /* AVC_STUDENT_ALGORITHM_H_ */
