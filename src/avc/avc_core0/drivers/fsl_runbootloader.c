/*
 * Copyright 2021 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "fsl_runbootloader.h"

#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.runBootloader"
#endif

#define BOOTLOADER_API_TREE_POINTER ((bootloader_tree_t *)0x1303fc00U)

typedef struct BootloaderTree
{
    void (*runBootloader)(void *arg);
    const uint32_t version;
    const char *copyright;
    const uint32_t reserved0;
    const uint32_t flashDriver;
    const uint32_t reserved1[5];
    const uint32_t nbootDriver;
    const uint32_t flexspiNorDriver;
    const uint32_t reserved2;
    const uint32_t memoryInterface;
} bootloader_tree_t;

void bootloader_user_entry(void *arg)
{
    assert(BOOTLOADER_API_TREE_POINTER);
    BOOTLOADER_API_TREE_POINTER->runBootloader(arg);
}
