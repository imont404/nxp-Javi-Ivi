/*
 * Copyright 2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef FSL_RUN_BOOTLOADER_H_
#define FSL_RUN_BOOTLOADER_H_

#include "fsl_common.h"

#define FSL_ROMAPI_RUNBOOTLOADER_DRIVER_VERSION (MAKE_VERSION(2, 0, 0))

/* MCXN947 ROM runBootloader argument. For ISP mode, boot_interface 5 selects
 * USB HS HID. Source: NXP mcux-devices-mcx, MCXN947 runbootloader driver. */
typedef struct
{
    union
    {
        struct
        {
            uint32_t reserved : 8;
            uint32_t boot_image_index : 4;
            uint32_t instance : 4;
            uint32_t boot_interface : 4;
            uint32_t mode : 4;
            uint32_t tag : 8;
        } B;
        uint32_t U;
    } option;
} user_app_boot_invoke_option_t;

#ifdef __cplusplus
extern "C" {
#endif

void bootloader_user_entry(void *arg);

#ifdef __cplusplus
}
#endif

#endif /* FSL_RUN_BOOTLOADER_H_ */
