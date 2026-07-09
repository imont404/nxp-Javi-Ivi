# 4 Memory

Source: `docs/mcxn947/MCXNx4xRM.pdf`
Pages: 46-62

<!-- page 46 -->

NXP Semiconductors

Chapter 4
Memory

4.1 Memory architecture
The memory architecture supports a wide range of target applications and maximizes performance while maintaining low power
consumption. The memory system of the device includes SRAM, ROM, internal flash, and external memory.

4.2 System memory map
The following table shows the chip's high-level memory map.
Additionally, the various sections are split into "Secure" and "Non-Secure" apertures for each IP. Although these apertures
address the same IP, access through the "Secure" aperture can only be performed by code with the appropriate security settings.

                                           NOTE
                                  The secure space is the alias of the non-secure region.

Table 16. System memory map
     Start     End address   TZ-M State1                      Description                       Size      Cached
   address
 0000_0000   001F_FFFF    Non-Secure                  Program flash                  2 MB     LPCAC
                                             Note: Program flash has two flash memory arrays/
                                                  banks of up to 1 MB each.

 0020_0000   00FF_FFFF                                 Reserved            —     —

 0100_0000   0100_7FFF                                Flash Bank 0 IFR 0                32 KB      No

 0100_8000   0100_FFFF                                Flash Bank 1 IFR 0                32 KB      No

 0101_0000   010F_FFFF                                 Reserved            —     —

 0110_0000   0110_1FFF                                Flash Bank 0 IFR 1                8 KB       No

 0110_2000   0110_3FFF                                Flash Bank 1 IFR 1                8 KB       No

 0110_4000   02FF_FFFF                                 Reserved            —     —

 0300_0000   0303_FFFF                         ROM-BOOT                  256 KB      No

 0304_0000   03FF_FFFF                                 Reserved            —     —

 0400_0000   0401_7FFF                          RAMX                     96 KB      No

 0401_8000   07FF_FFFF                                 Reserved            —     —
 0800_0000  0FFF_FFFF                        Flexible Serial Peripheral Interface (FlexSPI)     128 MB    LPCAC/
                                                                             CACHE64
                                                  Note: The 128 MB FlexSPI at address
                                         0x0800_0000 is aliased to the system memory
                                   map FlexSPI region at address 0x8000_0000
                                                                                       - 0x87FF_FFFF.

 1000_0000   101F_FFFF      Secure                   Program Flash                  2 MB     LPCAC

                                  Tablecontinuesonthenextpage...




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     46 / 3791

<!-- page 47 -->

NXP Semiconductors
                                                                                                   Memory

Table 16. System memory map (continued)
     Start     End address   TZ-M State1                      Description                       Size      Cached
   address

                                             Note: Program flash has two flash memory arrays/
                                                  banks of up to 1 MB each.

 1020_0000   10FF_FFFF                                 Reserved            —     —

 1100_0000   1100_7FFF                                Flash Bank 0 IFR 0                32 KB      No

 1100_8000   1100_FFFF                                Flash Bank 1 IFR 0                32 KB      No

 1101_0000   110F_FFFF                                 Reserved            —     —

 1110_0000   1110_1FFF                                Flash Bank 0 IFR 1                8 KB       No

 1110_2000   1110_3FFF                                Flash Bank 1 IFR 1                8 KB       No

 1110_4000   12FF_FFFF                                 Reserved            —     —

 1300_0000   1303_FFFF                         ROM-BOOT                  256 KB      No

 1304_0000   13FF_FFFF                                 Reserved            —     —

 1400_0000   1401_7FFF                          RAMX                     96 KB      No

 1401_8000   17FF_FFFF                                 Reserved            —     —

 1800_0000  1FFF_FFFF                        Flexible Serial Peripheral Interface (FlexSPI)     128 MB    LPCAC/
                                                                             CACHE64                                                      Note: The 128 MB FlexSPI at
                                               address 0x1800_0000 is aliased to the
                                             system memory map FlexSPI region at
                                               address 0x9000_0000-0x97FF_FFFF.

 2000_0000   2000_7FFF    Non-Secure                RAMA                     32 KB      No

 2000_8000   2000_FFFF                          RAMB                     32 KB      No

 2001_0000   2001_FFFF                         RAMC                     64 KB      No

 2002_0000   2002_FFFF                         RAMD                     64 KB      No

 2003_0000   2003_FFFF                          RAME                     64 KB      No

 2004_0000   2004_FFFF                          RAMF                     64 KB      No
 2005_0000   2005_FFFF                           RAMG2                     64 KB      No
 2006_0000   2006_7FFF                                RAMH3,2                    32 KB      No

 2006_8000   27FF_FFFF                                 Reserved            —     —

 2800_0000  2FFF_FFFF                                 Reserved            —     —

 3000_0000   3000_7FFF      Secure                 RAMA                     32 KB      No

 3000_8000   3000_FFFF                          RAMB                     32 KB      No

 3001_0000   3001_FFFF                         RAMC                     64 KB      No

 3002_0000   3002_FFFF                         RAMD                     64 KB      No

 3003_0000   3003_FFFF                          RAME                     64 KB      No

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     47 / 3791

<!-- page 48 -->

NXP Semiconductors
                                                                                                   Memory

Table 16. System memory map (continued)
     Start     End address   TZ-M State1                      Description                       Size      Cached
   address

 3004_0000   3004_FFFF                          RAMF                     64 KB      No
 3005_0000   3005_FFFF                           RAMG2                     64 KB      No
 3006_0000   3006_7FFF                                RAMH2,3                    32 KB      No

 3006_8000   37FF_FFFF                                 Reserved            —     —

 3800_0000  3FFF_FFFF      Secure                      Reserved            —     —

 4000_0000   4005_FFFF    Non-Secure              Peripheral Bridge 0 - PBRG0            384 KB      No

 4006_0000   4007_FFFF                                  Reserved            —     —

 4008_0000   4009_FFFF    Non-Secure              Peripheral Bridge 1 - PBRG1            128 KB      No

 400A_0000  400B_FFFF                              Peripheral Bridge 2 - PBRG2            128 KB      No

 400C_0000  400D_FFFF                              Peripheral Bridge 3 - PBRG3            128 KB      No

 400E_0000   400F_FFFF                                 Reserved            —     —

 4010_0000   4013_FFFF    Non-Secure              Peripheral Bridge 4 - PBRG4            256 KB      No

 4014_0000  4FFF_FFFF                                 Reserved            —     —

 5000_0000   5005_FFFF      Secure                Peripheral Bridge 0 - PBRG0            384 KB      No

 5006_0000   5007_FFFF                                  Reserved            —     —

 5008_0000   5009_FFFF      Secure                Peripheral Bridge 1 - PBRG1            128 KB      No

 500A_0000  500B_FFFF                              Peripheral Bridge 2 - PBRG2            128 KB      No

 500C_0000  500D_FFFF                              Peripheral Bridge 3 - PBRG3            128 KB      No

 500E_0000   500F_FFFF                                 Reserved            —     —

 5010_0000   5013_FFFF      Secure                Peripheral Bridge 4 - PBRG4            256 KB      No

 5014_0000  5FFF_FFFF                                 Reserved            —     —

 6000_0000  6FFF_FFFF                                 Reserved            —     —

 7000_0000  7FFF_FFFF                                 Reserved            —     —

 8000_0000  8FFF_FFFF    Non-Secure       Flexible Serial Peripheral Interface (FlexSPI)     256 MB   CACHE64

 9000_0000  9FFF_FFFF      Secure         Flexible Serial Peripheral Interface (FlexSPI)     256 MB   CACHE64

 A000_0000  AFFF_FFFF    Non-Secure       Flexible Serial Peripheral Interface (FlexSPI)     256 MB   CACHE64

 B000_0000  BFFF_FFFF      Secure         Flexible Serial Peripheral Interface (FlexSPI)     256 MB   CACHE64

 C000_0000  CFFF_FFFF                                 Reserved            —     —

 D000_0000  DFFF_FFFF                                 Reserved            —     —

 E000_0000  E003_FFFF                             Private peripheral bus (internal)           256 KB      No

 E004_0000  E00F_FFFF                     Private peripheral bus (external) (includes NVIC    768 KB      No
                                                  and SYSTICK timer)

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     48 / 3791

<!-- page 49 -->

NXP Semiconductors
                                                                                                   Memory

Table 16. System memory map (continued)
     Start     End address   TZ-M State1                      Description                       Size      Cached
   address

 E010_0000  FFFF_FFFF                                 Reserved            —     —

1.  This is the default Targeted Security Attribute for the memory region at reset. It is set by the IDAU and the SAU configuration.
   The attribute listed is the intended use case for users of TrustZone-M (TZM) security function.
2. On device part numbers that support optional ECC, RAMG and RAMH can be used to provide ECC data for other RAM blocks.
  RAMG and RAMH are not accessible as regular memory when used for ECC, so their availability in the memory map depends
   on the selected ECC configuration.
3. On device part numbers that support optional ECC, RAMB and RAMX ECC is enabled by default. This means that RAMH is
   used to provide ECC and is not accessible as regular RAM by default.

                                           NOTE
                          For detailed connection between masters and slaves, see Bus matrix block diagram.

4.3 Cache
To help increase system performance, there are several caches included in the device.
The 16 KB Low Power Cache Controller (LPCAC) is connected to the Code bus of the primary CM33 core.
Contents of this cache are only visible to the CM33 core. The LPCAC can be used to cache CM33 access to the
program flash (0x0000_0000-0x001F_FFFF and 0x1000_0000-0x101F_FFFF) and FlexSPI (0x0800_0000-0x0FFF_FFFF and
0x1800_0000-0x1FFF_FFFF) code bus memory regions. The LPCAC chapter provides the functional description of the cache,
but the SYSCON LPCAC Control (LPCAC_CTRL) register is used to control operation of the cache.
The Flash controller includes a small (64 byte) cache. Because the cache is a part of the memory controller, an access from any
master to the flash can potentially be cached. The SYSCON NVM Control (NVM_CTRL) register is used to control and enable the
FMC cache.
The 16 KB cache controller (CACHE64) is interfaced directly to the FlexSPI memory controller. Because the CACHE64
is integrated at the controller, an access from any master to the FlexSPI can be cached as long as the corresponding
address is configured as cacheable. The CACHE64_POLSEL module is used to define the cache policy (non-cacheable,
write-thru cacheable, or write-back cacheable) for up to three regions in the FlexSPI. The CACHE64_POLSEL and
CACHE64 use a remapping scheme to map the FlexSPI system memory map regions (0x0800_0000-0x0FFF_FFFF,
0x1800_0000-0x1FFF_FFFF, 0x8000_0000-0x8FFF_FFFF, 0x9000_0000-0x9FFF_FFFF, 0xA000_0000-0xAFFF_FFFF, and
0xB000_0000-0xBFFF_FFFF) to a single, contiguous 512 Mbyte region used to access the FlexSPI. Refer to Table 35 for details
on the remapping.

4.4 SRAM
Devices in this family support up to 512 KB of on-chip RAM (with ECC disabled). Refer to the device data sheet to determine the
RAM size for a given part number. Refer to Table 17 for the RAM configurations.
RAMX (up to 96 KB) is connected to the CM33 Code buses. RAMX is the preferred RAM block to use for code storage.
RAMA which is always four 8 KB banks (32 KB total) is the preferred RAM block to use for data retention. The RAMA banks
can be retained in device low-power modes. It can also optionally be powered from VBAT using LDO_RAM. To optimize
power consumption, RAMA is split into 4 banks, where the low-power mode and VBAT retention for each bank is individually
programmable. Low power configuration (LDO_RAM enable/disable and bank retention) are controlled by the VBAT module.
The other RAM blocks and partitions (other than RAMA) all have independent power switches that can be turned on/off depending
on the application's RAM needs. CMC_SRAMDIS0 can be used to completely power gate a RAM partition (applies for all power
modes). CMC_SRAMRET0 can be used to turn off the periphery of RAM partitions while retaining the contents of those RAMs
during Power Down mode.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     49 / 3791

<!-- page 50 -->

NXP Semiconductors
                                                                                                   Memory

4.4.1 RAM ECC
RAMA always supports software configurable ECC (enabled by default). Each 8 KB RAM bank support 32+7 ECC, which provides
one bit correction and two bits detection capability.
Depending on the specific device part number, ECC might be supported for the other RAM blocks. Refer to the device datasheet
to determine if a given part number supports ECC for RAMs other than RAMA. For non-RAMA blocks, the ECC bits are provided
by re-purposing RAMG and/or RAMH sub-blocks to provide ECC. RAMG and RAMH are used for ECC bits even for devices that
don't support the full 512 KB of RAM, where RAMG and RAMH are not available as normal RAM memory. For devices with less
than 512 KB of total RAM, other RAM blocks are disabled as ECC is enabled to keep the total amount of RAM being used including
ECC blocks balanced. Therefore, enabling optional ECC will decrease the overall available memory as RAMG, RAMH, or other
memory blocks become inaccessible.

                                           NOTE
            The SYSCON_ECC_ENABLE_CTRL bits must be set sequentially starting with bit 0 and working up to bit 3.
              Enabling ECC for RAMA and RAMB/RAMX is allowed. It is not legal to enable ECC for RAMA and RAMD/RAMC
            and leave ECC disabled on RAMB/RAMX (setting bit 0 and bit 2 but skipping bit 1).

The SYSCON's ECC_ENABLE_CTRL register is used to configure ECC functionality. ECC is always enabled by default for
RAMA. RAMX/RAMB ECC is enabled, by default, for part numbers that support ECC. Other RAM blocks default to ECC disabled,
even on ECC enabled part numbers.
Refer to the table below for details on the RAM configurations.

Table 17. RAM configurations

 RAM     Start      End        Sub-Block  Size (KB) Memory block      ECC RAM  SYSCON  CMC
 Block    Address    Address                                  availability based             ECC_ENA  SRAMDIS0/
                                                 on device total                BLE_CTRL SRAMRET01
                                             memory size

                                                 352 KB   512 KB
                                                        (no        (no
                                                  ECC)/288 ECC)/416
                                         KB (with  KB (with
                                            ECC)    ECC)

 RAMX   0400_0000 0400_7FFF RAMX0    32      Y       Y      RAMH1       bit 1            bit 0

         0400_8000 0400_FFFF RAMX1    32      Y       Y                                               bit 1

         0401_0000 0401_7FFF RAMX2    32      Y       Y                                               bit 2

 RAMA   2000_0000 2000_1FFF RAMA0    8       Y       Y          n/a - ECC    bit 0      VBAT
                                                                          always              LDORAMC[8]
                                                                                      available
         2000_2000 2000_3FFF RAMA1    8       Y       Y                         VBAT
                                                                                 LDORAMC[9]

         2000_4000 2000_5FFF RAMA2    8       Y       Y                         VBAT
                                                                            LDORAMC[1
                                                                                                                      0]

         2000_6000 2000_7FFF RAMA3    8       Y       Y                         VBAT
                                                                            LDORAMC[1
                                                                                                                      1]

 RAMB   2000_8000 2000_FFFF RAMB0    32      Y       Y      RAMH0       bit 1            bit 3

 RAMC   2001_0000 2001_7FFF RAMC0    32      Y       Y      RAMG3       bit 2            bit 4

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     50 / 3791

<!-- page 51 -->

NXP Semiconductors
                                                                                                   Memory

Table 17. RAM configurations (continued)

 RAM     Start      End        Sub-Block  Size (KB) Memory block      ECC RAM  SYSCON  CMC
 Block    Address    Address                                  availability based             ECC_ENA  SRAMDIS0/
                                                 on device total                BLE_CTRL SRAMRET01
                                             memory size

                                                 352 KB   512 KB
                                                        (no        (no
                                                  ECC)/288 ECC)/416
                                         KB (with  KB (with
                                            ECC)    ECC)

         2001_8000 2001_FFFF RAMC1    32      Y       Y                                               bit 5

 RAMD   2002_0000 2002_7FFF RAMD0    32      Y       Y      RAMG2                        bit 6

         2002_8000 2002_FFFF RAMD1    32      Y       Y                                               bit 7
 RAME   2003_0000 2003_7FFF RAME0    32       Y2      Y      RAMG1       bit 3            bit 8
         2003_8000 2003_FFFF RAME1    32       Y3      Y                                               bit 9

 RAMF   2004_0000 2004_7FFF RAMF0    32     N       Y      RAMG0                        bit 10

         2004_8000 2004_FFFF RAMF1    32     N       Y                                               bit 11
 RAMG  2005_0000 2005_3FFF RAMG0    16     N        Y4            -                 -                bit 12
         2005_4000 2005_7FFF RAMG1    16     N        Y4
         2005_8000 2005_BFFF RAMG2    16      ECC5     Y4                                             bit 13
        2005_C00  2005_FFFF RAMG3    16      ECC5     Y4
         0
 RAMH   2006_0000 2006_1FFF RAMH0    8       ECC5    Y                                               bit 14
         2006_2000 2006_7FFF RAMH1    24      ECC5     Y4

1. See SRAMDIS0 register and SRAMRET0 register.
2.  Block becomes inaccessible when RAMD and RAMC ECC is enabled
   (SYSCON_ECC_ENABLE_CTRL[RAMD_RAMC_ECC_ENABLE] = 1).
3.  Block becomes inaccessible when RAMB and RAMX ECC is enabled
   (SYSCON_ECC_ENABLE_CTRL[RAMB_RAMX_ECC_ENABLE] = 1).
4.  Block is not accessible if ECC is enabled for its pair.
5.  Block is not accessible as part of main memory but is used if ECC is enabled for its pair.

                                           NOTE
              For parts that support ECC on RAM blocks other than RAMA, ECC is enabled by default for the RAMB and RAMX
               blocks. This means the RAMH block is not accessible by default.

                                           NOTE
              While the RAMA-RAMH blocks are implemented as contiguous regions in the system memory map, each of the
         RAM blocks uses a different physical AHB slave port for access. This means that misaligned or burst accesses
              across the boundary from one RAM block to another are not allowed.

4.4.2 RAM ECC Error
The Error Recording Module (ERM) provides information and optional interrupt notification on SRAM error events associated with
ECC. The syndrome and error address information is captured along with error event in ERM registers.



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     51 / 3791

<!-- page 52 -->

NXP Semiconductors
                                                                                                   Memory

The Error Injection Module (EIM) can be used to induce artificial errors in the RAM ECC. EIM can inject single-bit and
multi-bit inversions on data. Injecting faults on memory accesses can be used to exercise the SEC-DED ECC function of the
related system.

4.4.3 RAM clock gating
To reduce power consumption, the RAM blocks support an auto clock gating feature. When auto clock gating is enabled, if the
RAM block is not accessed for 16 bus clocks, then the clock to the RAM block is automatically gated off. If the clock is off, then
there is a one bus cycle delay for the next access to the RAM block.
The auto clock gating feature is configurable on a per RAM block basis. The auto clock gating function is configured by the
SYSCON's AUTOCLKGATEOVERRIDE and AUTOCLKGATEOVERRIDEC registers. By default, auto clock gating is enabled for
the RAMX and RAMA blocks. The auto clock gating feature should be disabled, for RAM blocks that are used for code and data
sections that require time-critical or deterministic execution.
In addition to the auto clock gating, the clock for each RAM block other than RAMX and RAMA can be manually disabled
if the RAM block is not used or not used in a particular mode. The clock gate controls for the RAM blocks are found in the
SYSCON_AHBCLKCTRL0 register. By default, the clocks are disabled for most RAM blocks; however, the ROM will enable the
clock for all RAM blocks when booting.

                                           NOTE
                        If ECC is enabled for blocks other than RAMA, then the clock for the corresponding ECC RAM must be enabled.

4.5 Read Only Memory (ROM)
The internal ROM memory is used to store the boot code and time-critical software library routines. After a reset, the Cortex-M33
processor starts its code execution from this memory.

4.5.1 ROM size
The Boot ROM of this device is 256 KB.

4.6  Internal flash
This device embeds up to 2 MB of flash. It is implemented as 2 x 1MB flash block instances.
Refer to the device datasheet to determine the flash size of a given part number.
The following figure shows the high-level block diagram of the on-chip flash.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     52 / 3791

<!-- page 53 -->

NXP Semiconductors
                                                                                                   Memory


  AHB Slave Bus I / F                                         Peripheral Bus I / F



       Control                              Program
        Logic                                                     Control and                                       Buffer
                                                              Status Registers


       Prefetch
        Buffer                       Flash                                FMU
                                   Block 0



       Flash
      Cache                     Flash                 Command
                                  Block 1                         Controller
    FMC



4.6.1  Flash memory controller
The Flash Memory Controller (FMC) manages accesses performed by the bus masters of the system to the flash memory. The
FMC accelerates flash memory transfers to allow program code execution at higher clock frequency than flash memory.
The FMC provides two separate mechanisms for accelerating read operations to the flash memory:
   • A 128-bit prefetch buffer, which can prefetch the next 128-bit flash memory location.
   • A 64-Byte cache organized as a one set, four-way associative cache with 128-bit (or 16-Byte) size entries.
Note the FMC and its cache and buffers has no visibility into flash memory erase and program cycles because the Flash Memory
module manages them directly.
The speculation logic is tuned to work with the flash cache enabled. If the flash cache is present, the speculation logic assumes it
may immediately request the next sequential flash phrase as soon as any access hits the speculation buffer since this will cause
the phrase to be loaded in the flash cache. If the flash cache is present and disabled, the speculation logic may move to the next
flash phrase before the current flash phrase in the speculation buffer is fully accessed, causing degradation in the speculation
logic performance. For best performance on implementation which have both flash cache and speculation logic options available,
enable the flash cache whenever the speculation logic is enabled.

4.6.1.1  Prefetch buffer
When speculative reads are enabled, the FMC immediately requests the next sequential address after a read completes. The next
128-bit memory location is read. The speculative prefetch mechanism improves performance by reducing or even eliminating wait
states when accessing sequential code and/or data.
The FMC provides invalidation control for the prefetch buffer but the SYSCON's NVM_CTRL register is used to enable and
configure speculative prefetching.
While NVM_CTRL[DIS_DATA_SPEC] and NVM_CTRL[DIS_FLASH_SPEC] are cleared by default, the operation
of these fields interacts with the NVM_CTRL[DIS_MBECC_ERR_DATA] and NVM_CTRL[DIS_MBECC_ERR_INST].


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     53 / 3791

<!-- page 54 -->

NXP Semiconductors
                                                                                                   Memory

NVM_CTRL[DIS_MBECC_ERR_DATA] is set by default which disables the flash speculation even though
NVM_CTRL[DIS_FLASH_SPEC] is cleared. For best performance, NVM_CTRL[DIS_MBECC_ERR_DATA] should be cleared
early on in startup code.

4.6.1.2  Flash cache
Cache memory stores already fetched data. This code is immediately available for repeated execution without any wait states,
if needed.
The FMC provides controls for flash replacement algorithm, lock per way, and invalidation per way. The ways are numbered 0-3,
and the sets are numbered 0-3. The cache supports Least Recently Used (LRU) replacement algorithm per set across all 4 ways.
The SYSCON's NVM_CTRL register is used to enable/disable the cache and set other configurations. The flash cache is enabled
by default.
The cache entries, both data and tag/valid, can be read at any time.
Software is required to maintain memory coherence when any segment of the flash cache is programmed. For example, all buffer
data associated with the reprogrammed flash should be invalidated. Accordingly, cache program visible writes must occur after
a programming or erase event is completed and before the new memory image is accessed.
The access to on-chip flash is checked in the Memory Block Controller (MBC).

4.7  External memory
This device has one FlexSPI subsystem supporting Octal and Quad SPI memory devices. The FlexSPI instance is primarily
intended for code execution from off-chip SPI NOR and NAND flash memory. The FlexSPI interfaces also support HyperFlash,
HyperRAM and Xccela memory types.
The FlexSPI interface supports execute-in-place and on-the-fly decryption using the IPED with GCM module. The FlexSPI
interface includes a 16 KB cache with a CACHE64 AHB-cache controller. The CACHE64_POLSEL module is provided at the
CACHE64 interface to enable different caching policies for different address regions. These policies include:
   • Write-back
   • Write-through
   • Non-cached

4.8  Peripheral Bridge (PBRG)
The Peripheral Bridge (PBRG) is the portion of the bus fabric that connects the peripherals to the processor elements. Each
peripheral has a base address where the processor elements can access them. The following sections provide the memory map
of the peripherals connected to each of the Peripheral Bridges.

4.8.1  Peripheral Bridge 0 (PBRG0) memory map

Table 18. Peripheral bridge 0

  Base address           Slot       Module                                                                     Alias
   4000_0000                                    APB 0

   4000_0000           0        System controller                                  SYSCON

   4000_1000           1        Reserved                             —

   4000_2000           2        Reserved                             —

   4000_3000           3        Reserved                             —

   4000_4000           4         Pin Interrupt and Pattern Match                                 PINT0

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     54 / 3791

<!-- page 55 -->

NXP Semiconductors
                                                                                                   Memory

Table 18. Peripheral bridge 0 (continued)

  Base address           Slot       Module                                                                     Alias

   4000_5000           5        Reserved                             —

   4000_6000           6          Input multiplexing                                    INPUTMUX0

   4000_7000           7        Reserved                             —

   4000_8000           8        Reserved                             —

   4000_9000           9        Reserved                             —

   4000_A000          10        Reserved                             —

   4000_B000          11        Reserved                             —

   4000_C000          12        Standard counter/timers                                  CTIMER0

   4000_D000          13        Standard counter/timers                                  CTIMER1

   4000_E000          14        Standard counter/timers                                  CTIMER2

   4000_F000          15        Standard counter/timers                                  CTIMER3

   4001_0000          16        Standard counter/timers                                  CTIMER4

   4001_1000          17        Frequency Measurement Unit                           FREQME0

   4001_2000          18         Micro-Tick Timer                                          UTICK0

   4001_3000          19         Multi-Rate Timer                                     MRT0

   4001_4000          20        Reserved                             —

   4001_5000          21        Reserved                             —

   4001_6000          22       Windowed Watchdog Timer                          WWDT0

   4001_7000          23       Windowed Watchdog Timer                          WWDT1

   4001_8000          24        Reserved                             —

   4001_9000          25        Reserved                             —

   4001_A000          26        Reserved                             —

   4001_B000          27      CACHE64 Policy Select                              CACHE64_POLS
                                                                                      EL0

   4001_C000          28        Reserved                             —

   4001_D000          29        Reserved                             —

   4001_E000          30        Reserved                             —

   4001_F000          31        Reserved                             —
   4002_0000                                    APB 1

   4002_0000           0        Reserved                             —

   4002_1000           1        Improved Inter-Integrated Circuit                                   I3C0

   4002_2000           2        Improved Inter-Integrated Circuit                                   I3C1

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     55 / 3791

<!-- page 56 -->

NXP Semiconductors
                                                                                                   Memory

Table 18. Peripheral bridge 0 (continued)

  Base address           Slot       Module                                                                     Alias

   4002_3000           3        Reserved                             —

   4002_4000           4         Refer device's Security Reference Manual               —

   4002_5000           5         Refer device's Security Reference Manual               —

   4002_6000           6         Refer device's Security Reference Manual               —

   4002_7000           7        Reserved                             —

   4002_8000           8        Reserved                             —

   4002_9000           9        Reserved                             —

   4002_A000          10        Reserved                             —

   4002_B000          11        Refer device's Security Reference Manual               —

   4002_C000          12        Refer device's Security Reference Manual               —

   4002_D000          13        Refer device's Security Reference Manual               —

   4002_E000          14        Refer device's Security Reference Manual               —

   4002_F000          15        Refer device's Security Reference Manual               —

   4003_0000          16        Reserved                             —

   4003_1000          17        Refer device's Security Reference Manual               —

   4003_2000          18        CoolFlux BSP32                                        BSP32

   4003_3000          19       SmartDMA                                            SmartDMA

   4003_4000          20        Programable Logic Unit                                    PLU0

   4003_5000          21        Reserved                             —

   4003_6000          22        Reserved                             —

   4003_7000          23        Reserved

   4003_8000          24        Reserved                             —

   4003_9000          25        Reserved                             —

   4003_A000          26        Reserved                             —

   4003_B000          27        Reserved                             —

   4003_C000          28        Reserved                             —

   4003_D000          29        Reserved                             —

   4003_E000          30        Reserved                             —

   4003_F000          31        Reserved                             —
   4004_0000                                          AIPS 0

   4004_0000           0         General Purpose Input/Output                               GPIO5

   4004_1000           1         General Purpose Input/Output (alias 1) - alias of GPIO 5            GPIO5_alias1

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     56 / 3791

<!-- page 57 -->

NXP Semiconductors
                                                                                                   Memory

Table 18. Peripheral bridge 0 (continued)

  Base address           Slot       Module                                                                     Alias

   4004_2000           2         Port5                                             PORT5

   4004_3000           3         Flash Management Unit                                FMU0

   4004_4000           4        System Clock Generator                                SCG0

   4004_5000           5        System Power Controller                                 SPC0

   4004_6000           6       Wake-Up Unit                                    WUU0

   4004_7000           7        Reserved                             —

   4004_8000           8        Core Mode Controller                                 CMC0

   4004_9000           9      OS Event Timer                                      OSTIMER0

   4004_A000          10       Low-Power Timer                                     LPTMR0

   4004_B000          11       Low-Power Timer                                     LPTMR1

   4004_C000          12        Real Time Clock Subsystem and Real Time Clock             RTC_SUBSYST
                                                                      EM0 and RTC0

   4004_D000          13        Reserved                             —

   4004_E000          14        Reserved                             —

   4004_F000          15        Reserved                             —

   4005_0000          16       Touch Sensing Interface                                          TSI0

   4005_1000          17       Low Power Comparator                                CMP0

   4005_2000          18       Low Power Comparator                                CMP1

   4005_3000          19       Low Power Comparator                                CMP2

   4005_4000          20        Refer device's Security Reference Manual               —

   4005_5000          21        Refer device's Security Reference Manual               —

   4005_6000          22        Refer device's Security Reference Manual               —

   4005_7000          23        Refer device's Security Reference Manual               —

   4005_8000          24        Refer device's Security Reference Manual               —

   4005_9000          25      VBAT                                               VBAT0

   4005_A000          26        Refer device's Security Reference Manual               —

   4005_B000          27         Error Injection Module                                        EIM0

   4005_C000          28         Error Recording Module                                ERM0

   4005_D000          29          Interrupt Monitor                                          INTM0

   4005_E000          30        Reserved                             —

   4005_F000          31        Reserved                             —





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     57 / 3791

<!-- page 58 -->

NXP Semiconductors
                                                                                                   Memory

4.8.2  Peripheral Bridge 1 (PBRG1) memory map

Table 19. Peripheral bridge 1

      Base               Slot       Module                                                                     Alias
   4008_0000                                          AIPS 1

   4008_0000           0      eDMA 0 Management Page                             eDMA_0_MP

   4008_1000           1      eDMA 0 CH0 Control and Configuration                     eDMA_0_CH0

   4008_2000           2      eDMA 0 CH1 Control and Configuration                     eDMA_0_CH1

   4008_3000           3      eDMA 0 CH2 Control and Configuration                     eDMA_0_CH2

   4008_4000           4      eDMA 0 CH3 Control and Configuration                     eDMA_0_CH3

   4008_5000           5      eDMA 0 CH4 Control and Configuration                     eDMA_0_CH4

   4008_6000           6      eDMA 0 CH5 Control and Configuration                     eDMA_0_CH5

   4008_7000           7      eDMA 0 CH6 Control and Configuration                     eDMA_0_CH6

   4008_8000           8      eDMA 0 CH7 Control and Configuration                     eDMA_0_CH7

   4008_9000           9      eDMA 0 CH8 Control and Configuration                     eDMA_0_CH8

   4008_A000          10      eDMA 0 CH9 Control and Configuration                     eDMA_0_CH9

   4008_B000          11      eDMA 0 CH10 Control and Configuration                    eDMA_0_CH10

   4008_C000          12      eDMA 0 CH11 Control and Configuration                    eDMA_0_CH11

   4008_D000          13      eDMA 0 CH12 Control and Configuration                    eDMA_0_CH12

   4008_E000          14      eDMA 0 CH13 Control and Configuration                    eDMA_0_CH13

   4008_F000          15      eDMA 0 CH14 Control and Configuration                    eDMA_0_CH14

   4009_0000          16      eDMA 0 CH15 Control and Configuration                    eDMA_0_CH15
   4009_1000                                AHB Peripherals

   4009_1000          17      SCTIMER                                            SCT0

   4009_2000          18        Low-power Flexible Communications Interface               LP_FLEXCOMM
                                                                                            0

   4009_3000          19        Low-power Flexible Communications Interface               LP_FLEXCOMM
                                                                                            1

   4009_4000          20        Low-power Flexible Communications Interface               LP_FLEXCOMM
                                                                                            2

   4009_5000          21        Low-power Flexible Communications Interface               LP_FLEXCOMM
                                                                                            3

   4009_6000          22        General Purpose Input/Output                               GPIO0

   4009_7000          23       GPIO 0 (alias 1)                                              GPIO0_alias1

   4009_8000          24        General Purpose Input/Output                               GPIO1

   4009_9000          25       GPIO 1 (alias 1)                                              GPIO1_alias1

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     58 / 3791

<!-- page 59 -->

NXP Semiconductors
                                                                                                   Memory

Table 19. Peripheral bridge 1 (continued)

      Base               Slot       Module                                                                     Alias

   4009_A000          26        General Purpose Input/Output                               GPIO2

   4009_B000          27       GPIO 2 (alias 1)                                              GPIO2_alias1

   4009_C000          28        General Purpose Input/Output                               GPIO3

   4009_D000          29       GPIO 3 (alias 1)                                              GPIO3_alias1

   4009_E000          30        General Purpose Input/Output                               GPIO4

   4009_F000          31       GPIO 4 (alias 1)                                              GPIO4_alias1

4.8.3  Peripheral Bridge 2 (PBRG2) memory map

Table 20. Peripheral bridge 2

      Base               Slot       Module                                                                     Alias
   400A_0000                                          AIPS 2

   400A_0000          0      eDMA 1 Management Page                             eDMA_1_MP

   400A_1000          1      eDMA 1 CH0 Control and Configuration                     eDMA_1_CH0

   400A_2000          2      eDMA 1 CH1 Control and Configuration                     eDMA_1_CH1

   400A_3000          3      eDMA 1 CH2 Control and Configuration                     eDMA_1_CH2

   400A_4000          4      eDMA 1 CH3 Control and Configuration                     eDMA_1_CH3

   400A_5000          5      eDMA 1 CH4 Control and Configuration                     eDMA_1_CH4

   400A_6000          6      eDMA 1 CH5 Control and Configuration                     eDMA_1_CH5

   400A_7000          7      eDMA 1 CH6 Control and Configuration                     eDMA_1_CH6

   400A_8000          8      eDMA 1 CH7 Control and Configuration                     eDMA_1_CH7

   400A_9000          9      eDMA 1 CH8 Control and Configuration                     eDMA_1_CH8

   400A_A000          10      eDMA 1 CH9 Control and Configuration                     eDMA_1_CH9

   400A_B000          11      eDMA 1 CH10 Control and Configuration                    eDMA_1_CH10

   400A_C000          12      eDMA 1 CH11 Control and Configuration                    eDMA_1_CH11

   400A_D000          13      eDMA 1 CH12 Control and Configuration                    eDMA_1_CH12

   400A_E000          14      eDMA 1 CH13 Control and Configuration                    eDMA_1_CH13

   400A_F000          15      eDMA 1 CH14 Control and Configuration                    eDMA_1_CH14

   400B_0000          16      eDMA 1 CH15 Control and Configuration                    eDMA_1_CH15

   400B_1000          17       Semaphore 2                                         SEMA42_0
   400B_2000                                AHB Peripherals

   400B_2000          18        Inter-CPU Mailbox                                     MAILBOX0

   400B_3000          19        Refer device's Security Reference Manual               —

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     59 / 3791

<!-- page 60 -->

NXP Semiconductors
                                                                                                   Memory

Table 20. Peripheral bridge 2 (continued)

      Base               Slot       Module                                                                     Alias

   400B_4000          20        Low-power Flexible Communications Interface               LP_FLEXCOMM
                                                                                            4

   400B_5000          21        Low-power Flexible Communications Interface               LP_FLEXCOMM
                                                                                            5

   400B_6000          22        Low-power Flexible Communications Interface               LP_FLEXCOMM
                                                                                            6

   400B_7000          23        Low-power Flexible Communications Interface               LP_FLEXCOMM
                                                                                            7

   400B_8000          24        Low-power Flexible Communications Interface               LP_FLEXCOMM
                                                                                            8

   400B_9000          25        Low-power Flexible Communications Interface               LP_FLEXCOMM
                                                                                            9

   400B_A000          26      USB FS RAM                                      USB0_FS_RAM

   400B_B000          27        Refer device's Security Reference Manual               —

   400B_C000          28        Refer device's Security Reference Manual               —

   400B_D000          29        Refer device's Security Reference Manual               —

   400B_E000          30      NPU                                              NPU0

   400B_F000          31       PowerQuad                                   POWERQUAD0

4.8.4  Peripheral Bridge 3 (PBRG3) memory map

Table 21. Peripheral bridge 3

      Base               Slot       Module                                                                     Alias
   400C_0000                                          AIPS 3

   400C_0000          0          External Watchdog Monitor                           EWM0

   400C_1000          1        Performance Monitor - LPCAC                        CMX_PERFMON
                                                                                            0

   400C_2000          2        Performance Monitor - FlexSPI                        CMX_PERFMON
                                                                                            1

   400C_3000          3        Reserved                             —

   400C_4000          4        Reserved                             —

   400C_5000          5        Reserved                             —

   400C_6000          6        Reserved                             —

   400C_7000          7         Refer device's Security Reference Manual               —

   400C_8000          8          Flexible Serial Peripheral Interface                            FLEXSPI0

   400C_9000          9         Refer device's Security Reference Manual               —

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     60 / 3791

<!-- page 61 -->

NXP Semiconductors
                                                                                                   Memory

Table 21. Peripheral bridge 3 (continued)

      Base               Slot       Module                                                                     Alias

   400C_A000          10        Reserved                             —

   400C_B000          11         Cyclic Redundancy Check                               CRC0

   400C_C000          12        Refer device's Security Reference Manual               —

   400C_D000          13        Reserved                             —

   400C_E000          14       Enhanced Flex Pulse Width Modulator                     PWM0

   400C_F000          15        Quadrature Decoder                                  QDC0

   400D_0000          16       Enhanced Flex Pulse Width Modulator                     PWM1

   400D_1000          17        Quadrature Decoder                                  QDC1

   400D_2000          18        Event Generator                                      EVTG0

   400D_3000          19        Reserved                             —

   400D_4000          20       FlexCAN with FD                                      CAN0

   400D_8000          21       FlexCAN with FD                                      CAN1

   400D_C000          22      USB FS DCD                                      USBDCD0

   400D_D000          23      USB FS Controller                                     USBFS0

   400D_E000          24        Reserved                             —

   400D_F000          25        Reserved                             —

4.8.5  Peripheral Bridge 4 (PBRG4) memory map

Table 22. Peripheral bridge 4

      Base               Slot       Module                                                                     Alias
   4010_0000                                          AIPS 4

   4010_0000           0         Ethernet with QoS                                      ENET0

   4010_2000           1        Reserved                             —

   4010_3000           2       EMVSIM0                                           EMVSIM0

   4010_4000           3       EMVSIM1                                           EMVSIM1

   4010_5000           4          Flexible Input/Output                                       FLEXIO0

   4010_6000           5          Serial Audio Interface                                           SAI0

   4010_7000           6          Serial Audio Interface                                           SAI1

   4010_8000           7       SINC Filter                                               SINC0

   4010_9000           8          Ultra Secured Digital Host Controller                        uSDHC0

   4010_A000          9      USB HS PHY & DCD                                 USBHS1_PHY

   4010_B000          10      USB HS Controller                                    USBHS1

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     61 / 3791

<!-- page 62 -->

NXP Semiconductors
                                                                                                   Memory

Table 22. Peripheral bridge 4 (continued)

      Base               Slot       Module                                                                     Alias

   4010_C000          11        Microphone Interface                                          MICFIL0

   4010_D000          12         Analog-to-Digital Converter                               ADC0

   4010_E000          13         Analog-to-Digital Converter                               ADC1

   4010_F000          14          12-bit Digital-to-Analog Converter                           DAC0

   4011_0000          15         Operational Amplifier                                 OPAMP0

   4011_1000          16        Voltage Reference                                      VREF0

   4011_2000          17          12-bit Digital-to-Analog Converter                           DAC1

   4011_3000          18         Operational Amplifier                                 OPAMP1

   4011_4000          19          14-bit Digital-to-Analog Converter                           DAC2

   4011_5000          20         Operational Amplifier                                 OPAMP2

   4011_6000          21         Port Control                                         PORT0

   4011_7000          22         Port Control                                         PORT1

   4011_8000          23         Port Control                                         PORT2

   4011_9000          24         Port Control                                         PORT3

   4011_A000          25         Port Control                                         PORT4

   4011_B000          26        Reserved                             —

   4011_C000          27        Reserved                             —

   4011_D000          28        Reserved                             —

   4011_E000          29        Reserved                             —

   4011_F000          30        Reserved                             —
   4012_0000                                AHB Peripherals

   4012_0000          32        Secure AHB Controller                                AHBSC

   4012_1000          33        Secure AHB Controller Alias 1                                AHBSC_alias1

   4012_2000          34        Secure AHB Controller Alias 2                                AHBSC_alias2

   4012_3000          35        Secure AHB Controller Alias 3                                AHBSC_alias3

   4012_4000                    Reserved                             —





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     62 / 3791
