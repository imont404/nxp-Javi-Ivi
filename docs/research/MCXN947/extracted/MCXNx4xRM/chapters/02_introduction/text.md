# 2 Introduction

Source: `docs/mcxn947/MCXNx4xRM.pdf`
Pages: 20-36

<!-- page 20 -->

NXP Semiconductors

Chapter 2
Introduction

2.1 Overview
The MCX Nx4x series microcontrollers combine the Arm Cortex-M33 TrustZone® core with a CoolFlux BSP32, a PowerQuad
DSP Co-processor, and multiple high-speed connectivity options running at 150 MHz. To support a wide variety of applications,
the MCX N-series includes advanced serial peripherals, timers, high-precision analog, and state-of-the-art security features like
secure user code, data, and communications. All MCX Nx4x products include dual-bank flash which supports read while write
operation from internal flash. The MCX Nx4x series also supports large external serial memory configurations.
The MCX Nx4x are as follows:
   • N54x: Mainstream MCU with a second M33 core, advanced timers, analog and high-speed connectivity (including Hi-Speed
    USB), 10/100 Ethernet, and FlexIO which can be programmed as an LCD controller.
   • N94x: Integration CPU and DSP of serial connectivity, advanced timers, high precision analog, and high-speed connectivity
    including Hi-Speed USB, CAN 2.0, 10/100 Ethernet, and FlexIO which can be programmed as an LCD controller.
The chip includes these key features:

Table 1. Key features

 Function                         Features
 Security                                      • TrustZone for Armv8M
                                                 • Secure boot, firmware update, and debug authentication using ROM
                                                 • EdgeLock® secure subsystem (ELS) S50
                                                 • Public-key cryptography (PKC)
                                                 • External and internal flash memory interface with on-the-fly PRINCE decryption and
                                        encryption
                                                 • Physically Unclonable Function (PUF) hardware options
                                                 • Factory Root of Trust (RoT) programming
                                                 • Tamper detection
                                                 • Analog and digital glitch detection
                                                 • Code Watchdog (CDOG)
                                                 • Intrusion and Tamper Response Controller (ITRC)

 Industrial strength                          • Industrial temperature rating
                                                 • Industrial communication protocol support (CAN-FD)
                                                 • High-resolution mixed signal analog
                                                 • BLDC and PMSM motor control support
                                                 • Integrated sensor interfaces: I3C, I2C, SPI, and UART
                                                 • 15-year longevity
                                                 • Configurable RAM ECC
                                                 • Ethernet (ENET) with QoS

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     20 / 3791

<!-- page 21 -->

NXP Semiconductors
                                                                                                                                     Introduction

Table 1. Key features (continued)

 Function                         Features
 Power-efficient operating modes       • Down to 57 µA/MHz (3.3 V, @25 ℃) in OD Active Mode (while(1) executing from
                                              flash, DC/DC enabled)
                                                 • 170 µA in Deep Sleep mode (full 512 KB SRAM retention, 3.3 V, @25 ℃)
                                                 • 5.2 µA in Power Down mode (full 512 KB SRAM retention, 3.3 V, @25 ℃)
                                                 • Down to 2.0 µA in Deep Power Down mode, 5.3 ms wake-up (RTC enabled 8 KB
                         RAM and Reset pin enabled, @25 ℃)

2.2 Target applications
The MCX Nx4x MCUs are ideal solutions for:
   • Consumer IoT and computing products
   • Energy-efficient smart appliances with automation and industrial control
   • Automotive aftermarket accessories
   • Secure communication hubs, smart IoT gateways, and secure industrial gateways
   • Smart meters, industrial controls, and automation
   • Diagnostic equipment, data collectors, electronic instruments, and navigation

2.3 Block diagram
Figure 2 shows a top-level view of the modules within the chip, organized by functional category.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     21 / 3791

<!-- page 22 -->

NXP Semiconductors
                                                                                                                                     Introduction




                              System                               Core                             Debug

                         EVTG                                                     BSCAN

                                                    CPU0 (primary)
                           PowerQuad                                                    DAP
                                                       Arm Cortex-M33 core

                       eDMA (×2)             DSP        FPU       MPU             DWT

                          Debug Mailbox            SIMD       ETM         TZ-M                ETB

                              CoolFlux BSP32            ETB                                    ETM


                          MAILBOX                                                               ITM

                                                   CPU1 (secondary)
                        INPUTMUX                                                         JTAG                                                                    Micro Cortex-M33 core

                      NPU (N1-16)                                              SWD


                      WUU                                                                 TPIU


                        AXBS (×2)
                                                             Clock           Human-machine interface         Memory [1]

                             AIPS (×5)             OSC_SYS             GPIO (0-5)            Flash memory (up to 2 MB)


                          SmartDMA            OSC_RTC                 MICFIL              FMU


                       SYSCON              FRO_16K            PORT (0-5)                  FlexSPI


                       ERM               FRO_12M                 PINT             ROM_BOOT


                              EIM               FRO_144M                   TSI           SRAM (up to 512 KB)


                            INTM               SCG                                   LPCAC

                       CMC                                                     CACHE64_CTRL

                         SEMA42                                                   CACHE64_POLSEL
                                                                  Security               Communication

                         SPC               CRC              CAN (×2)        CMX_PERFMON (×2)

                                                                                                                             Notes:
                           PLU               MBC                      FlexIO                  [1]. Refer to the device data sheet to
                                                                                                                          determine the sizes of Flash memory
                                                                                                             and RAM for a given part number.
                      DCDC_CORE           GDET (×2)
                                                            LP_FLEXCOMM (×10)            Timers
                                                                         (LPSPI/LPUART/LPI2C)
                       LDO_CORE              TDET                            PWM (×2)

                        LDO_SYS              OTPC                   I3C (×2)            QDC (×2)

                           VBAT             CDOG (×2)                SAI (×2)               RTC

                                             NPX               USBHS             LPTMR (×2)

                                                    IPED              USBDCD             CTIMER (×5)

                                           AHBSC               USBFS               MRT
                            Analog modules

                       ADC (×2)               PKC              USBHS_PHY              UTICK

                      CMP (×3)               PUF               ENET Qos             OSTIMER

                       DAC (×3)                 ITRC               uSDHC              SCTIMER

                          VREF                 ELS              EMVSIM (×2)           FREQME


                      OPAMP (×3)                                          WWDT (×2)


                            SINC                                              EWM



 Figure 2. Features block diagram
                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     22 / 3791

<!-- page 23 -->

NXP Semiconductors
                                                                                                                                     Introduction

Figure 3 shows the chip's block diagram, including bus connections.


    TZ  DSP FPU MPU  interfaceDebug                                                                                                                                                                 Clocks,            PoR
                                                                                                                                                                          power                                                                                                                                                                                                                                         control,                                                                                                                                                                        LVD/HVD          Coprocessor                          interface
                                                                                                                                               DC-DC                                                                                                                                                                                                                          converter,                                      Power                                                    4× 4 KB             with math function
                                                                                                                                                  FRO                                                                                                                                                                   LDOs,                                    Quad           private                                           RAM
        Arm Cortex-M33 core                                                                                                       2× 2                                                                                            KB                                                         system functions      PLL
                                                                                     PKC                                                                                  RAM
                        Code              System

          LPCAC                    NPU                                  (N1-16)                                                   CoolFlux
            16 KB         with                            2 KB                                  cache                                         BSP32              Smart DMA   Micr-CM33core     DMA0    DMA1    PKC  ELS   USB FS  ENET   USB HS   uSDHC
                      D    O      P  X  Y         D               I  C            System



                                                                                                                                                                                                                 Flash memory
         M0   M1     M11      M7   M8 M9  M10         M2     M3         M4      M5      M6 P0       M12          M13      M14         FMC  PRINCE FMU      1 MB
                                                                                                                                                                                                                 Flash memory
                                                                                                                                                                                           1 MB

                                                                                                      P1                                   ROM
                                                                                                         P16                                                 CACHE64                                                                                                                                                                                IPED    FlexSPI
                                                                                                                                                                                                         (16 KB)   W/GCM

                                                                                                      P2                                                      RAMX
                                                                                                                                                                                         96 KB

                                                                                                      P3                                      RAMA
              Multilayer                                                                                                                                                  32 KB
        AHB matrix                                                                                     P4                                                      RAMB
                                                                                                                                                                                         32 KB

                                                                                                      P5                                      RAMC
                                                                                                      P6                                                  64 KB            RAMD
                                                                                                                                                                                         64 KB
                                                                                                      P7                                      RAME
                                                                                                      P8                                                  64 KB            RAMF
                                                                                                      P9                                     RAMG               64 KB
                                                                                                                                                                    64 KB
                                                                                                         P10                             RAMH
                                                                                                                                                          32 KB


                                                                                                         P12
                                                                                                         P13

                                                                                                         P14                              AIPS
                                                                                                                                                                              bridge 3
                                                                                                         P15   AIPS
                                                                                                              P11  bridge 4

                                                APB
                                                                   bridge 0
                                                                     AIPS                 SYSCON
                                                                           0                      (clock,                             reset, wake-up)               Peripheral input            bridge
                                              muxes [2]
                   WWDT(0,1)                                    FMU                                         EMVSIM (0,1)             EWM               USBFS RAM               MAILBOX IPC
                                                                                   CMP2                                          CTIMER 0
                                                                                   TSI                                                                                                                                                     FlexIO                FlexSPI-CMX_PERF          Debug Mailbox          LP_FLEXCOMM (2,3) [1]
                                                                                SCG                                               UTICK
                  GPIO PINT                                                               PORT0                                                                                                                                       SAI (0,1)              LPCAC-CMX_PERF      LP_FLEXCOMM (4-9) [1]        LP_FLEXCOMM (0,1)
                                        CTIMER (1,2,3,4)                                    OSTIMER
                  MRT                                    CMC                                                SINC                  MBC               CDOG0               SCTIMER / PWM
                                      FREQME                                                            14-bit DAC
               CACHE64_POLSEL                                 LPTMR 0,1                                       uSDHC                        FlexSPI              CDOG1                    GPIO0
                                                                                 PORT (1,2,3,4)                                                APB
                                                                                                       ADC (0,1)                 OTPC                 Power Quad                GPIO 1,2,3,4
                                                                   bridge                                                              1          WUU
                      GDET(0,1)                                      SPC                                                               12-bit DAC (0,1)               CRC                NPU                  PKC RAM

                                                        CMP (0,1)                                    OPAMP (0,1,2)                NPX                          AIPS
                                                                                                                                                                                                                                    bridge 2        PKC RAM interface                                                                                                  EIM                                                     I3C0
                   PKC                                                              RTC [3]                                                                                                              VREF              PWM (0,1)
                                                      I3C1                                     ERM                                                                         eDMA 1
                                                                 TDET                                                                                                             ENET                                                                                                                             CAN                                                                                                                                                                                                                           (0,1)                    PUF
                                                                                               INTM                                            PLU                                                                                                                                                             eDMA 1 CH0-15         bridgeAIPS 1                                                                     GPIO5                                                                                                                                      MICFIL                                                                                                                       QDC                                                                                                                                                                                                                            (0,1)                     CoolFlux                       BSP32
                                                                                            ELS                                            SmartDMA                                                                                                                                                                         SEMA42                 eDMA 0
                                                               PORT5                                                                                                          USBHS                  EVTG
                                                                   VBAT                                       USB HS PHY                USBFS                                               eDMA 0 CH0-15
                                                                                                                                  and DCD
                                                                                                                               USB FS DCD
                                                                                                          AHBSC


                                  CORE_WAKE                                                     CORE_MAIN               VBAT      SYSTEM
                 domain                             domain                                              domain                                                                   domain                   Multiple layer AHB matrix 0


                  Notes:
                        [1] : Each LP_FLEXCOMM includes UART, SPI and I2C.
                        [2] : Peripheral input muxes of peripherals in the Wake domain must be put into the Wake domain.
                        [3] : RTC is partitioned to rtc_lp (VBAT domain) and rtc_hp (Core domain).

 Figure 3. Bus matrix block diagram

2.4 System bus priority and arbitration
As shown in Figure 3 , this device includes a number of masters that can access the various slave ports. The multilayer AHB matrix
allows for concurrent accesses when multiple masters are attempting to access different slave ports. If multiple masters attempt
an access to the same slave port at the same time then arbitration is required.
The SYSCON_AHBMATPRIO register is where the programmable priorities for each of the master ports can be configured.
Masters are assigned a priority value between zero and three with three being the highest priority. If two ports have the same
priority, then the lowest port number is given priority.
There are some master ports that are shared between two masters. Where the port is shared, only one of the masters can have
an active access at a time. The priority between two masters sharing a port uses a fixed arbitration scheme. The table below lists
the master or masters for each of the ports. In the case where the port is shared, the high-priority master is specified:


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     23 / 3791

<!-- page 24 -->

NXP Semiconductors
                                                                                                                                     Introduction

Table 2. AHB bus matrix ports

  Master port               Master                 Accessible slave ports      Inaccessible slave ports    AHB ID

    M0    CPU0 (CM33) code bus          P0 - P2 and P16          P3 - P15                 0

    M1    CPU0 (CM33) system bus        P3 - P16               P0 - P2                  1

    M2    CPU1 (Micr-CM33) code bus (low-  P0 and P2 - P16          P1                     2
                   priority)

           SmartDMA data bus (high-priority)                                                  3

    M3    CPU1 (Micr-CM33) system       P0 - P10 and P16         P11 - P15                4
             bus (low-priority)

           SmartDMA instruction bus (high-                                                   5
                   priority)

    M4    DMA0                      P0 - P16                None                   6

    M5    DMA1                      P0 and P2 - P16          P1                     7

    M6    PKC (low-priority)              P0 - P11 and P16         P12 - P15                8

           ELS (high-priority)                                                               9

    M7     PowerQuad (low-priority)           P0, P2 - P10, P13, and P16  P1, P11 - P12, and P14     10
                                                                                                              - P15
          NPU operand bus (high-priority)                                                    11

    M8      CoolFlux BSP32 program          P0, P2 - P10, and P16     P1 and P11 - P15          12
            memory bus

    M9      CoolFlux BSP32 X-data            P0, P2 - P10, P12 - P13,    P1, P11, and P14 - P15     13
            memory bus                    and P16

    M10     CoolFlux BSP32 Y-data            P0, P2 - P10, and P16     P1 and P11 - P15          14
            memory bus

    M11    NPU data bus                    P0, P2 - P10, P13, and P16  P1, P11 - P12, and P14     16
                                                                                                              - P15

    M12    USBFS (low-priority)               P0, P2 - P10, and P16     P1 and P11 - P15          17

               Ethernet (high-priority)                                                           18

    M13    USBHS                          P0, P2 - P10, and P16     P1 and P11 - P15          19

    M14    uSDHC                     P2 - P10               P0 - P1 and P11 - P16      20

2.5  Functional overview
Table 3 shows the chip modules organized by functional category.

Table 3. Module functional categories

 Module category                                    Description
 Core                                                                • The Arm Cortex®-M33 is part of the Cortex-M Series of
                                                     processors targeting microcontroller cores focused on cost-
                                                              sensitive, deterministic, and interrupt-driven environments.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     24 / 3791

<!-- page 25 -->

NXP Semiconductors
                                                                                                                                     Introduction

Table 3. Module functional categories (continued)

 Module category                                    Description

                                                                       • The Cortex-M33 processor is based on the Armv8-M Architecture
                                               and ThumbR-2 ISA and is upward compatible with the Cortex-M7,
                                               M4, M3, M1, M0, and M0+.

 System                                                             • PowerQuad DSP Coprocessor and Accelerator (PowerQuad)
                                                                       • Debug Mailbox
                                                                       • SmartDMA Controller
                                                                       • Neural Processing Unit (NPU N1-16)
                                                                       • CoolFlux BSP32
                                                                       • Inter-CPU Mailbox (MAILBOX)
                                                                       • Enhanced Direct Memory Access (eDMA)
                                                                       • Wake-Up Unit (WUU)
                                                                       • Event Generator (EVTG)
                                                                       • AHB Cross-Bar Switch (AXBS) Lite
                                                                       • Peripheral Inputmux
                                                                       • Semaphore (SEMA42)
                                                                       • Interrupt Monitor (INTM)
                                                                       • Error Injection Module (EIM)
                                                                       • Error Reporting Module (ERM)
                                                                       • Programmable Logic Unit (PLU)
                                                                       • System Power Controller (SPC)
                                                                       • Core Mode Controller (CMC)

 Memory                                                            • ROM-BOOT and ROM-CODE
                                                                       • Static Random Access Memory (SRAM)
                                                                       • AHB Low Power Cache Controller (LPCAC)
                                                                       • CACHE64 Controller (CACHE64_CTRL)
                                                                       • CACHE64 Policy Select (CACHE64_POLSEL)
                                                                       • Flash Management Unit (FMU)
                                                                       • Flash Memory Controller (FMC) supporting counter mode PRINCE
                                                                       • Flexible Serial Peripheral Interface (FlexSPI) supporting IPED with
                                        GCM. FlexSPI also supports Octal and Quad memory interfaces.
                                                                       • Performance Monitor (CMX_PERFMON)

 Clock                                                               • VBAT
                           — Crystal Oscillator - Real Time Clock (OSC_RTC)

                                  Tablecontinuesonthenextpage...




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     25 / 3791

<!-- page 26 -->

NXP Semiconductors
                                                                                                                                     Introduction

Table 3. Module functional categories (continued)

 Module category                                    Description

                           — 16 K Free Running Oscillator (FRO_16K)
                                                                       • System Clock Generator (SCG)
                           — Crystal Oscillator - System (OSC_SYS)
                           — 144 M Free Running Oscillator (FRO_144M)
                           — 12 M Free Running Oscillator (FRO_12M)
                           — PLL and USB PLL

 Security                                                            • EdgeLock Secure Subsystem (ELS) S50
                                                                       • Public Key Cryptography (PKC)
                                                                       • Physical Unclonable Function (PUF)
                                                                       • Cyclic Redundancy Check (CRC)
                                                                       • PRINCE encryption/decryption
                                                                       • Digital tamper (TDET)
                                                                       • Code Watchdog (CDOG)
                                                                       • Digital and analog Glitch Detect (GDET)
                                                                       • OTP Controller (OTPC)
                                                                       • Intrusion and Tamper Response Controller (ITRC)
                                                                       • AHB Secure Controller (AHBSC)
                                                                       • Memory Block Checker (MBC)
                                                                       • TrustZone

 Analog                                                             • 16/12-bit Analog-to-Digital Converter (ADC)
                                                                       • 14-bit Digital-to-Analog Converter (DAC)
                                                                       • 12-bit Digital-to-Analog Converter
                                                                       • Low Power Comparator (LPCMP)
                                                                       • Voltage Reference (VREF)
                                                                       • Operational Amplifier (OPAMP)
                                                                       • Sigma-Delta Modulator (SINC)

 Timer                                                               • Standard Counter/Timer (CTIMER)
                                                                       • SCTimer
                                                                       • Multi Rate Timer (MRT)
                                                                       • Windowed Watchdog Timer (WWDT)
                                                                       • Micro-Tick (UTICK) Timer
                                                                       • OS Event Timer (OSTIMER)
                                                                       • Enhanced Flex Pulse Width Modulator (PWM)

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     26 / 3791

<!-- page 27 -->

NXP Semiconductors
                                                                                                                                     Introduction

Table 3. Module functional categories (continued)

 Module category                                    Description

                                                                       • Quadrature Decoder (QDC)
                                                                       • Real Time Clock (RTC)
                                                                       • Frequency Measurement (FREQME)
                                                                       • Low-power Timer (LPTMR)
                                                                       • External Watchdog Monitor (EWM)

 Communication                                                  • Universal Serial Bus - Full Speed (USBFS) and Transceiver
                                                                       • USB High-Speed (USBHS) Controller and Physical Layer Interface
                                                (PHY)
                                                                       • Universal Serial Bus - Device Charger Detect (USBDCD)
                                                                       • Ultra Secured Digital Host Controller (uSDHC)
                                                                       • LP_FLEXCOMM with Low Power Inter-Integrated Circuit (LPI2C),
                                          Low Power Serial Peripheral Interface (LPSPI), and Low Power
                                                        Universal Asynchronous Receive/Transmit (LPUART) support
                                                                       • Serial Audio Interface (SAI)
                                                                       • FlexCAN with Flexible Data rate (FlexCAN FD)
                                                                       • Euro, MasterCard, and Visa Subscriber Identification Module
                                             (EMVSIM)
                                                                       • Ethernet QoS (ENET)
                                                                       • Flexible Input/Output (FlexIO)
                                                                       • Improved Inter-Integrated Circuit (I3C)

 Human Machine Interface (HMI)                              • General Purpose Input/Output (GPIO)
                                                                       • Port Control (PORT)
                                                                       • Pin Interrupt and Pattern Match (PINT)
                                                                       • PDM Microphone Interface (MICFIL)
                                                                       • Touch Sense Interface (TSI)

2.5.1 Core
The following core modules are available on this chip.

Table 4. Core modules

 Module                                             Description
 CPU0                                  An Arm Cortex-M33 processor that runs at a frequency of up to 150
                                   MHz (device revision 1B only). The configuration of this instance includes
                                      MPU, FPU, DSP, ETM, TrustZone, and coprocessor interface.

 CPU1                                  An Arm Cortex-M33 coprocessor that runs at a frequency of up to
                                           150 MHz (device revision 1B only). The configuration of this instance

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     27 / 3791

<!-- page 28 -->

NXP Semiconductors
                                                                                                                                     Introduction

Table 4. Core modules (continued)

 Module                                             Description

                                                  supports system tick timer but does not include MPU, FPU, DSP, ETM,
                                           and TrustZone.

 Digital Signal Processing Extensions (DSP)        The Cortex-M33 processor features extended single-cycle Multiply
                                              Accumulate (MAC) instructions, optimized Single Instruction Multiple
                                              Data (SIMD) arithmetic, and saturating arithmetic instructions.

 Floating Point Unit (FPU)                          Provides single-precision floating point computation, compliant to the
                                          IEEE Standard for Floating-Point Arithmetic (IEEE 754).

 Memory Protection Unit (MPU)                     Provides support for eight unified protection regions, overlapping
                                                       protection regions with ascending region priority, and access
                                                   permissions. MPU mismatches and permission violations invoke the
                                                 HardFault handler.

 Nested Vectored Interrupt Controller (NVIC)       The Armv8M exception model and Nested-Vectored Interrupt
                                                       Controller (NVIC) implement a relocatable vector table supporting
                                                      external interrupts, a single non-maskable interrupt (NMI), and priority
                                                           levels.

 System Tick Timer (SysTick)                  See the Armv8MArchitectureReferenceManualfor more information
                                                about this system timer.

2.5.2 Debug
The following debug modules are available on this chip.

Table 5. Debug modules

 Module                                             Description
 Boundary Scan (BSCAN)                          Allows external circuitry testing of a chip. The serial scan chain also
                                                   permits the system signals flowing into and out of the system logic to
                                           be sampled and examined without causing interference with the normal
                                                  device operation.

 Cross Trigger Interface (CTI)                     Enables the debug logic of the chip to interact with each other (cross-
                                                               trigger).

 Data Watchpoint and Trace (DWT)            A generic name for modules that allow debug access of the Cortex-M33.
                                          The DWT consists of the Debug Watchpoint and Trace (DWT) module
                                           and the Flash Patch and Breakpoint (FPB) unit.

 Debug Access Port (DAP)                       Enables real-time access to the chip registers from an external debugger
                                                     without halting the processor cores.

 Embedded Trace Buffer (ETB)                    On-chip memory used to store real-time trace instruction and data trace
                                                     information from the Embedded Trace Macrocell (ETM).

 Embedded Trace Macrocell (ETM)                  Real-time trace module providing real-time instruction and data tracing of
                                           a processor core.

                                  Tablecontinuesonthenextpage...




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     28 / 3791

<!-- page 29 -->

NXP Semiconductors
                                                                                                                                     Introduction

Table 5. Debug modules (continued)

 Module                                             Description
 Instruction Trace Macrocell (ITM)                  Provides a memory-mapped register interface that applications can use
                                                          to write logging or event words for profiling software.

 Joint Test Action Group (JTAG)                   Implements serial communication protocol for communicating with the
                                                 Test Access Port (TAP).

 Serial Wire Debug (SWD)                  A serial communication interface used for debugging devices with
                                                        multiple cores while only requiring a single external interface.

 Trace Port Interface Unit (TPIU)                    Acts as a bridge between the on-chip trace data from the modules (such
                                              as ETM or ITM which have separate system IDs) to the external world.

2.5.3 System
The following system modules are available on this chip.

Table 6. System modules

 Module                                             Description
 Power Quad                                 Hardware accelerator targeting common calculations in
                                   DSP applications.

 Debug mailbox                                  Supports Arm Serial Wire Debug mode.

 SmartDMA                                      Supports unique reduced instruction sets and performs event- and IO-
                                                     driven handling to offload the work from the Arm processor.

 CoolFlux BSP32                                  Dual-Harvard, dual-multiplier processor based on the ultra-low power
                                                 CoolFlux architecture.
                                                Supports 16-bit and 32-bit scalar operations, as well as complex SIMD
                                                    operations and data types.

 Inter-CPU Mailbox (MAILBOX)                     Provides simple communication and resource allocation between CPUs.

 Event Generator (EVTG)                         Supports the generation of a configurable number of EVENT
                                                         signals. EVTG includes two AND/OR/INVERT (AOI) modules and one
                                                     configurable flip-flop.

 AHB Cross-Bar Switch (AXBS) Lite               Connects bus masters and bus slaves. This allows the bus masters
                                                          to access different bus slaves simultaneously and provides arbitration
                                       among the bus masters when they access the same slave.

 Peripheral Bridge(AIPS_Lite)                      Converts an AMBA AHB interface to a peripheral interface that allows
                                                    the Peripheral Bridge2 (PBRIDGE2) controller to interface to multiple
                                                       peripherals. This device has five peripheral bridges.

 System Controller (SYSCON)                      Provides controls and configurations of the system and peripherals for
                                                    the multiple functions.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     29 / 3791

<!-- page 30 -->

NXP Semiconductors
                                                                                                                                     Introduction

Table 6. System modules (continued)

 Module                                             Description
 Core Mode Controller (CMC)                      Provides control and protection on entry and exit to each power mode,
                                                        control for the System Power Controller (SPC), and reset entry and exit
                                                              for the complete device.

 Enhanced Direct Memory Access (eDMA)          Performs source and destination address calculations and data-
                                         movement operations.
                                             Capable of performing complex data transfers with minimal intervention
                                                from a host processor.

 NPU                                A Neural Network processing accelerator. It is a configurable architecture
                                                  comprising weight compression, compute engine, and data canvas.

 Wake-up Unit (WUU)                              Allows selection of external pins and internal modules as interrupt wake-
                                           up sources from Power Down and Deep Power Down modes.

 Peripheral Input Multiplexing (INPUTMUX)          Allows the trigger output of one peripheral to be connected to the trigger
                                                       input of a second peripheral.

 Semaphore (SEMA42)                    A memory-mapped module that provides the robust hardware support
                                           needed in multi-core systems for implementing semaphores. It provides
                                           a simple mechanism to achieve lock and unlock operations via a single
                                                         write access. The hardware semaphore module provides hardware-
                                                 enforced gates as well as other useful system functions related to the
                                                     gating mechanisms.

 Interrupt monitor (INTM)                           Provides a mechanism to monitor the latency of the responses on
                                                           interrupt requests.

 Error Injection Module (EIM)                       Provides a method for diagnostic coverage of internal memories (for
                                               example, system RAM, cache RAMs, and peripheral memories).

 Error Reporting Module (ERM)                     Provides information and optional interrupt notification on memory error
                                                 events associated with error correction code (ECC) and parity.

 System Power Controller (SPC)                    Provides control over the operation and configuration of the system
                                            power generation modules to optimize power consumption for the level of
                                                           functionality needed.

 Direct Current/Direct Current Converter - Core     A voltage converter for generating the core voltage for the chip.
 (DCDC_CORE)
 Low Drop Out Regulator - Core (LDO_CORE)     A voltage regulator for generating the core voltage for the chip.

 Low Drop Out Regulator - System (LDO_SYS)     A voltage regulator for generating the system voltage for the chip.

 Programmable Logic Unit (PLU)                    Monitors the state of input devices and makes decisions based on a
                                             custom program to control the state of output devices.

2.5.4 Memory
The following memory modules are available on this chip.



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     30 / 3791

<!-- page 31 -->

NXP Semiconductors
                                                                                                                                     Introduction

Table 7. Memory modules

 Module                                             Description
 Static Random Access Memory (SRAM)              Internal system SRAM memory. Each individual block of SRAM can be
                                                    configured to be retained in low-power modes.

 AHB Low Power Cache Controller (LPCAC)      A processor-local level 1 (L1) bus cache controller for use with cores
                                                  using AMBA-AHB input/output buses.

 CACHE64 Controller (CACHE64_CTRL)            Includes cache memory controller (CACHE64), which is a general
                                               purpose AMBA AHB bus protocol cache.

 CACHE64 Policy Select (CACHE64_POLSEL)      Monitors incoming AHB addresses to define the policy that the cache
                                                          controller (CACHE64_CTRL) uses.

 Flash Management Unit (FMU)                 Manages the interface between the chip and the on-chip flash memory.

 Flash Memory Controller (FMC)              A programmable, non-volatile flash memory that can store executable
                                             program code or data.

 Flexible Serial Peripheral Interface (FlexSPI)         Interfaces with external serial flash devices.

 Performance Monitor (CMX_PERFMON)           Contains counters which can be configured to count events in order to
                                                       calculate performance of a CPU, cache, or memory.

2.5.5  Clock
The following clock modules are available on this chip.

Table 8. Clock sources

 Module                                             Description
 Crystal Oscillator - Real Time Clock (OSC_RTC)    Generates, in conjunction with an external 32 kHz crystal, a 32 kHz
                                                   reference clock for the RTC that can also be used by the chip.

 Free Running Oscillator - 16 K (FRO_16K)        An ultra low-power internal 16.384 kHz clock source. It is functional down
                                                          to VBAT mode. The FRO_16K is trimmed to +/- 6% accuracy over the
                                                          entire voltage and temperature range.

 System Clock Generator (SCG)                    Provides you with access to the configuration control registers for the
                                              system level clock sources.

 Crystal Oscillator - System (OSC_SYS)             Generates, in conjunction with an external crystal or resonator, a
                                                   reference clock for the chip.

 Free Running Oscillator - 144 MHz (FRO_144M)    An internal clock source that generates a 144 MHz reference clock for the
                                   RTC that can also be used by the chip.

 Free Running Oscillator - 12 MHz (FRO_12M)     An internal clock source that generates a 12 MHz frequency for use by
                                                    the chip.

                                  Tablecontinuesonthenextpage...





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     31 / 3791

<!-- page 32 -->

NXP Semiconductors
                                                                                                                                     Introduction

Table 8. Clock sources (continued)

 Module                                             Description
 PLL and USB PLL                           PLL0 and PLL1 allow the chip operation up to the maximum CPU rate
                                                     without the need for a high-frequency external clock. PLL0 and PLL1 can
                                                  run from the internal FRO_144M, the 40 MHz OSC, or the 32.768 kHz
                                   RTC oscillator. Both support fractional division and spread-spectrum.
                                              There is a PLL in USBHS PHY to provide 480 MHz clock for USBHS.
                                   USB PLL uses external 16 MHz, 19.2 MHz, 20 MHz, 24 MHz, or 32
                                   MHz as a reference clock. The USB PLL clock can be used as a system
                                                     clock source.

2.5.6  Security
The following security modules are available on this chip.

Table 9. Security modules

 Module                                             Description
 ELS S50                                            Security subsystem ensuring key isolation.

 PKC                                               Public-key cryptography accelerator

 Physically Unclonable Function (PUF) SRAM        Includes PUFs that are virtually impossible to duplicate or clone
 controller/Quiddikey                           and consequently helps in secure key generation and storage and
                                                  device authentication.

 PUF subsystem                                  Stores the applications keys, which are provisioned to ELS S50 key
                                                          store, securely.

 Cyclic Redundancy Check (CRC)                  Provides error detection for all single and double errors, and many
                                                              multi-bit errors.

 PRINCE                                      Ensures confidentiality protection of the content stored on internal flash.
                                                 Also ensures both confidentiality and integrity protection of the content
                                                    stored on external flash. Run-time code integrity protection is ensured
                                                    during execution from external flash.

 Digital Tamper (TDET)                           Supports tamper detection.

 Code Watchdog Timer (CDOG)                   Helps protect the integrity of software by detecting unexpected changes
                                                               (faults) in the code execution flow.

 Digital and analog Glitch Detect (GDET)         A fully digital detector connected to the supply lines to provide
                                                              glitch detection.

 OTPC w 4kb Fuse                         An on-chip EFUSE OTP controller from TSMC which supports loading
                                           and housing of EFUSE content into shadow registers.

 Intrusion and Tamper Response Controller (ITRC)   Provides mechanism to configure the response action for an intrusion
                                                 event detected by an on-chip security sensor.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     32 / 3791

<!-- page 33 -->

NXP Semiconductors
                                                                                                                                     Introduction

Table 9. Security modules (continued)

 Module                                             Description
 Secure AHB bus and AHB Controller (AHBSC)     The device implements a second layer of protection with secure AHB Bus
                                                          to support secure trusted execution at the system level. The secure
                                   AHB Controller provides access policies for all the bus slaves via
                                                checker functions.

 Memory Block Checker (MBC)                     Provides read, write, and execute access control per block to internal
                                                         flash memory.

 TrustZone - M (TZM)                                     Is an Arm security feature related to hardware-enforced access control
                                             mechanisms.

2.5.7  Timer
The following timer modules are available on this chip.

Table 10. Timer modules

 Module                                             Description
 Standard Counter/Timer (CTIMER)              Each Counter/timer is designed to count cycles of the CTIMER
                                                       function clock.

 SCTIMER                             A powerful and flexible timer module capable of creating complex PWM
                                             waveforms with minimal or no CPU intervention.

 Multi Rate Timer (MRT)                           Provides a repetitive 24-bit interrupt timer with four channels.

 Windowed Watchdog Timer (WWDT)              Helps reset or interrupt an erroneous microcontroller within a
                                             programmable time.

 Micro-Tick (UTICK) Timer                  A 31-bit timer that provides a fixed time interval between interrupts.

 OS Event Timer (OSTIMER)                 A 42-bit Gray code counter.

 Enhanced Flex Pulse Width Modulator (PWM)      Generates various switching patterns, including highly sophisticated
                                               waveforms, and controls different Switched Mode Power Supplies
                                         (SMPS) topologies.

 Quadrature Decoder (QDC)                          Interfaces to position and speed sensors that are used in industrial motor
                                                        control applications.

 Real Time Clock Subsystem (RTC_SUBSYSTEM)   Supports subsecond and wake timer features.

 Real Time Clock (RTC)                           Provides time keeping and calendaring functions and additionally
                                                   provides protection against spurious memory and register updates and
                                                       battery operation.

 Frequency Measurement (FREQME)               Provides high-accuracy frequency measurement function for on-chip and
                                                           off-chip clocks.

 Low-power Timer (LPTMR)                 A 16-bit timer or pulse counter with compare feature.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     33 / 3791

<!-- page 34 -->

NXP Semiconductors
                                                                                                                                     Introduction

Table 10. Timer modules (continued)

 Module                                             Description
 External Watchdog Monitor (EWM)                 Monitors external circuits, as well as the MCU software flow. Also
                                                   provides a backup mechanism to the internal watchdog that resets the
                                         MCU's CPU and peripherals.

2.5.8 Communication
The following communication modules are available on this chip.

Table 11. Communication modules

 Module                                             Description
 Universal Serial Bus Full Speed Host and Device    Supports OTG protocol. The OTG implementation in this module
 Controller (USBFS)                                provides a device solution for a full speed compliant peripheral along with
                                                          limited host functionality for implementing a USB 2.0 full speed/low speed
                                                   compliant embedded host.

 Universal Serial Bus 2.0 High-Speed Integrated      This chip contains USB 2.0 PHY macrocells capable of connecting
 Physical Layer Interface (USBHS_PHY)               to USB host/device systems at the USB low-speed (LS) rate of 1.5
                                                        Mbits/s, full-speed (FS) rate of 12 Mbits/s, or at the USB 2.0 high-speed
                                               (HS) rate of 480 Mbits/s. The USB PHY supports USB Battery Charger
                                                       Specification Revision 1.2.

 Universal Serial Bus - Device Charger Detect        Monitors the USB data lines to detect attachment to a USB charging port
 (USBDCD)                                     meeting the USB Battery Charging Specification Rev1.2.

 Universal Serial Bus - High Speed Controller        Provides high performance USB functionality that conforms to the
 (USBHS)                                          Universal Serial Bus Specification, Rev. 2.0.

 Ultra Secured Digital Host Controller (uSDHC)       Provides the interface between the host system and the SD/
                                      SDIO/MMC cards.

 Low-power Flexible Communications Interface      Provides an option to choose one peripheral function from a choice
 (LP_FLEXCOMM)                                     of several peripherals, such as Low Power Inter-Integrated Circuit
                                                    (LPI2C), Low Power Serial Peripheral Interface (LPSPI), and Low Power
                                                    Universal Asynchronous Receive/Transmit (LPUART).

 Serial Audio Interface (SAI)                 A full-duplex, serial port that allows the chip to communicate with serial
                                                  devices that implement the Inter-IC sound bus (I2S) and the Intel®
                                      AC97 standards.

 FlexCAN with FD                        A communication controller implementing the Controller Area Network
                                           (CAN) protocol according to the ISO 11898-1 standard and CAN 2.0 B
                                                      protocol specifications.

 Smart Card Interface Module (EMVSIM)               Facilitates communication to Smart Cards compatible with the EMV
                                                   v4.3 standard (Book 1) and Smart Cards compatible with ISO/IEC
                                              7816-3 Standard.

                                  Tablecontinuesonthenextpage...





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     34 / 3791

<!-- page 35 -->

NXP Semiconductors
                                                                                                                                     Introduction

Table 11. Communication modules (continued)

 Module                                             Description
 Ethernet QoS                                  Enables a host to transmit and receive data over Ethernet in compliance
                                                      with the IEEE 802.3-2008 standard.

 Flexible Input/Output (FlexIO)                     Emulation of a variety of serial/parallel communication protocols. Flexible
                                                          16-bit timers with support for a variety of trigger, reset, enable and
                                                     disable conditions.

 Improved Inter-Integrated Circuit (I3C)           An extension of the I2C bus protocol supporting higher speeds.

2.5.9 Human Machine Interface (HMI)
The following HMI modules are available on this chip.

Table 12. HMI modules

 Module                                             Description
 General Purpose Input/Output (GPIO)                   All GPIO pins support interrupt and DMA request generation.

 Port Control (PORT)                              Provides support for pad control functions.

 Pin Interrupt and Pattern Match (PINT)             Pins with configurable functions can serve as external interrupts or inputs
                                                          to the pattern match engine.
                                          Uses standard GPIO functions as inputs.

 PDM Microphone Interface (MICFIL)               Supports audio delivery from microphones to the processor in several
                                                        applications, such as mobile telephones.

 Touch Sense Interface (TSI)                       Provides touch sensing detection on capacitive touch sensors.

2.5.10 Analog
The following analog modules are available on this chip.

Table 13. Analog modules

 Module                                             Description
 Analog-to-Digital Converter (ADC)             A dual 16-bit successive approximation ADC designed for operation
                                                        within an integrated microcontroller system-on-chip.

 14-bit Digital-to-Analog Converter (DAC)         A high performance, 14-bit general-purpose DAC the output of which can
                                           be placed on an external pin or set as one of the inputs to the analog
                                                comparator or ADC.

 12-bit Digital-to-Analog Converter             A low power, 12-bit general-purpose DAC the output of which can be
                                                 placed on an external pin or set as one of the inputs to the analog
                                                comparator or ADC.

 Low Power Comparator (LPCMP)                  Provides a circuit for comparing two analog input voltages.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     35 / 3791

<!-- page 36 -->

NXP Semiconductors
                                                                                                                                     Introduction

Table 13. Analog modules (continued)

 Module                                             Description
 Voltage Reference (VREF)                         Supplies an accurate voltage output that can be used by internal or
                                                      external peripherals.

 Operational Amplifier (OPAMP)                    Supports more than 100 dB amplifier gain.

 SINC Filter                                      Converts external ADC sigma-delta modulator bit stream to data stream.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                     36 / 3791
