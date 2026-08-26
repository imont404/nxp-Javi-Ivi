#ifndef CAMERA_H
#define CAMERA_H

#include "fsl_common.h"

/* Symbols to be used with GPIO driver */
#define BOARD_INITCAMERAPINS_CAM_RST_GPIO GPIO1   /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITCAMERAPINS_CAM_RST_GPIO_PIN 19U /*!<@brief GPIO pin number */
#define BOARD_INITCAMERAPINS_CAM_RST_GPIO_PIN_MASK (1U << 19U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_INITCAMERAPINS_CAM_RST_PORT PORT1           /*!<@brief PORT peripheral base pointer */
#define BOARD_INITCAMERAPINS_CAM_RST_PIN 19U              /*!<@brief PORT pin number */
#define BOARD_INITCAMERAPINS_CAM_RST_PIN_MASK (1U << 19U) /*!<@brief PORT pin mask */
                                                          /* @} */

/* Symbols to be used with GPIO driver */
#define BOARD_INITCAMERAPINS_CAM_PDWN_GPIO GPIO1   /*!<@brief GPIO peripheral base pointer */
#define BOARD_INITCAMERAPINS_CAM_PDWN_GPIO_PIN 18U /*!<@brief GPIO pin number */
#define BOARD_INITCAMERAPINS_CAM_PDWN_GPIO_PIN_MASK (1U << 18U) /*!<@brief GPIO pin mask */

/* Symbols to be used with PORT driver */
#define BOARD_INITCAMERAPINS_CAM_PDWN_PORT PORT1 /*!<@brief PORT peripheral base pointer */
#define BOARD_INITCAMERAPINS_CAM_PDWN_PIN 18U    /*!<@brief PORT pin number */
#define BOARD_INITCAMERAPINS_CAM_PDWN_PIN_MASK (1U << 18U) /*!<@brief PORT pin mask */

void nxpc_camera__init();
void nxpc_camera__service();
void nxpc_camera__prepare_frame(uint16_t *buffer);
void nxpc__next_frame(uint16_t *buffer);

#endif
