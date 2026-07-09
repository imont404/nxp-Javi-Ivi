# 17 System Controller (SYSCON)

Source: `docs/mcxn947/MCXNx4xRM.pdf`
Pages: 477-637

<!-- page 477 -->

NXP Semiconductors

Chapter 17
System Controller (SYSCON)

17.1  Chip-specific SYSCON information

Table 264. Reference links to related information

         Topic             Related module                                Reference

      Full description        SYSCON                          SYSCON

  System memory map                                           Memory map

        Clocking                                                        Clock distribution

   Signal multiplexing          Port control                                  Signal multiplexing

                                           NOTE
                     The reset values of some registers may change depending on the device's boot settings.

17.1.1 Module instances
This device has one instance of SYSCON module, SYSCON0.

                                           NOTE
            The SYSCON module includes some security functionality that is not described here. Refer to the MCX Nx4x
               Security RM for more details.

17.1.2  Security considerations
For secure applications, NXP recommends that this module is configured for secure and privileged access only to prevent
unintentional or malicious modification of the system operation by nonsecure software. The secure AHB controller controls the
security level for access to peripherals and does default to secure and privileged access for all peripherals.

17.1.3  Configuration
Configure the SYSCON block as follows:
   • No clock configuration is needed. The clock to the SYSCON block is always enabled.
   • The SYSCON block controls use of the CLKOUT pin, which must also be configured through Port control module.

17.2 Overview
The SYSCON module provides controls and configurations on the system and peripherals for the multiple functions.

17.2.1  Features
The SYSCON module supports the following features and configurations:
   • System and bus configuration
   — AHB matrix priority
   — CPUs control and status
   — CPU and DSP debug access
   — CPU LPCAC control



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    477 / 3791

<!-- page 478 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

   — NMI source select
   — Calibrate system tick timer
   — Boot control
   • System/Peripherals - clock select and control
   — Allows enabling and selection of clocks to individual peripherals and memories
   — Allows configuration of clock dividers
   • Memory configuration
   — ROM access
   — RAM ECC
   — FLASH cache, bus error
   • Gray-2-Binary converter
   — Allows decoding gray value coming from OS Event Timer
   • Peripherals configuration
   — SmartDMA interrupt hijack
   — PWM0/1 control
   — Ethernet PHY selection and flow control
   — CTIMER global start
   • Security configuration
   — Security control - ELS, GDET0/1
   — Security boot
   — Security attestation
   • Reset control
   — Monitors and release resets to individual peripherals
   • Device ID register

17.3  Signals

Table 265. SYSCON pin description

 Function    Type    Pin                                                             Description

 CLKOUT   O      PIO0_6, PIO2_2, PIO3_6                           CLKOUT clock output. Refer to the
                                                                                    Pinouts information.

17.4 Memory map and register definition
This section includes the SYSCON module memory map and detailed descriptions of all registers.

17.4.1 SYSCON register descriptions

17.4.1.1 SYSCON memory map
SYSCON0 base address: 4000_0000h




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    478 / 3791

<!-- page 479 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


     Offset     Register                                                        Width    Access   Reset value
                                                                                                     (In bits)

     10h    AHB Matrix Priority Control (AHBMATPRIO)                        32    RW    See section

     38h      Secure CPU0 System Tick Calibration (CPU0STCKCAL)              32    RW    See section

    3Ch      Non-Secure CPU0 System Tick Calibration (CPU0NSTCKCAL)         32    RW    See section

     40h     System tick calibration for CPU1 (CPU1STCKCAL)                  32    RW    See section

     48h     NMI Source Select (NMISRC)                                   32    RW    See section

    100h      Peripheral Reset Control 0 (PRESETCTRL0)                       32    RW    See section

    104h      Peripheral Reset Control 1 (PRESETCTRL1)                       32    RW    See section

    108h      Peripheral Reset Control 2 (PRESETCTRL2)                       32    RW    0000_0000h

    10Ch      Peripheral Reset Control 3 (PRESETCTRL3)                       32    RW    0000_0000h

 120h - 12Ch  Peripheral Reset Control Set (PRESETCTRLSET0 -                  32    W     0000_0000h
            PRESETCTRLSET3)

 140h - 14Ch  Peripheral Reset Control Clear (PRESETCTRLCLR0 -                32    W     0000_0000h
            PRESETCTRLCLR3)

    200h    AHB Clock Control 0 (AHBCLKCTRL0)                            32    RW    0000_0603h

    204h    AHB Clock Control 1 (AHBCLKCTRL1)                            32    RW    0000_0000h

    208h    AHB Clock Control 2 (AHBCLKCTRL2)                            32    RW    0000_0000h

    20Ch    AHB Clock Control 3 (AHBCLKCTRL3)                            32    RW    0000_0000h

 220h - 22Ch  AHB Clock Control Set (AHBCLKCTRLSET0 - AHBCLKCTRLSET3)     32    W     0000_0000h

 240h - 24Ch  AHB Clock Control Clear (AHBCLKCTRLCLR0 -                     32    W     0000_0000h
            AHBCLKCTRLCLR3)

    260h    CPU0 System Tick Timer Source Select (SYSTICKCLKSEL0)          32    RW    0000_0007h

    264h    CPU1 System Tick Timer Source Select (SYSTICKCLKSEL1)          32    RW    See section

    268h     Trace Clock Source Select (TRACECLKSEL)                       32    RW    See section

 26Ch - 27Ch  CTIMER Clock Source Select (CTIMERCLKSEL0 -                  32    RW    See section
            CTIMERCLKSEL4)

    288h    CLKOUT Clock Source Select (CLKOUTSEL)                       32    RW    See section

    2A4h    ADC0 Clock Source Select (ADC0CLKSEL)                        32    RW    See section

    2A8h    USB-FS Clock Source Select (USB0CLKSEL)                      32    RW    See section

 2B0h - 2D4h  LP_FLEXCOMM Clock Source Select for Fractional Rate Divider        32    RW    See section
            (FCCLKSEL0 - FCCLKSEL9)

    2F0h     SCTimer/PWM Clock Source Select (SCTCLKSEL)                  32    RW    See section

    300h    CPU0 System Tick Timer Divider (SYSTICKCLKDIV0)                32    RW    4000_0000h

    304h    CPU1 System Tick Timer Divider (SYSTICKCLKDIV1)                32    RW    4000_0000h

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    479 / 3791

<!-- page 480 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

     Offset     Register                                                        Width    Access   Reset value
                                                                                                     (In bits)

    308h    TRACE Clock Divider (TRACECLKDIV)                            32    RW    4000_0000h

    350h     TSI Function Clock Source Select (TSICLKSEL)                     32    RW    See section

    360h     SINC FILTER Function Clock Source Select (SINCFILTCLKSEL)       32    RW    See section

    378h    SLOW_CLK Clock Divider (SLOWCLKDIV)                         32    RW    See section

    37Ch     TSI Function Clock Divider (TSICLKDIV)                           32    RW    4000_0000h

    380h     System Clock Divider (AHBCLKDIV)                              32    RW    0000_0000h

    384h    CLKOUT Clock Divider (CLKOUTDIV)                             32    RW    4000_0000h

    388h     FRO_HF_DIV Clock Divider (FROHFDIV)                          32    RW    4000_0000h

    38Ch    WDT0 Clock Divider (WDT0CLKDIV)                              32    RW    See section

    394h    ADC0 Clock Divider (ADC0CLKDIV)                              32    RW    See section

    398h     USB-FS Clock Divider (USB0CLKDIV)                            32    RW    4000_0000h

    3B4h    SCT/PWM Clock Divider (SCTCLKDIV)                            32    RW    4000_0000h

    3C4h     PLL Clock Divider (PLLCLKDIV)                                 32    RW    4000_0000h

 3D0h - 3E0h  CTimer Clock Divider (CTIMER0CLKDIV - CTIMER4CLKDIV)          32    RW    4000_0000h

    3E4h     PLL1 Clock 0 Divider (PLL1CLK0DIV)                             32    RW    4000_0000h

    3E8h     PLL1 Clock 1 Divider (PLL1CLK1DIV)                             32    RW    4000_0000h

    3FCh     Clock Configuration Unlock (CLKUNLOCK)                         32    RW    0000_0000h

    400h    NVM Control (NVM_CTRL)                                     32    RW    0002_0410h

    404h    ROM Wait State (ROMCR)                                      32    RW    0000_0000h

    414h     SmartDMA Interrupt Hijack (SmartDMAINT)                        32    RW    See section

    464h    ADC1 Clock Source Select (ADC1CLKSEL)                        32    RW    See section

    468h    ADC1 Clock Divider (ADC1CLKDIV)                              32    RW    See section

    490h    DAC0 Functional Clock Selection (DAC0CLKSEL)                   32    RW    0000_0007h

    494h    DAC0 functional clock divider (DAC0CLKDIV)                       32    RW    4000_0000h

    498h    DAC1 Functional Clock Selection (DAC1CLKSEL)                   32    RW    0000_0007h

    49Ch    DAC1 functional clock divider (DAC1CLKDIV)                       32    RW    4000_0000h

    4A0h    DAC2 Functional Clock Selection (DAC2CLKSEL)                   32    RW    0000_0007h

    4A4h    DAC2 functional clock divider (DAC2CLKDIV)                       32    RW    4000_0000h

    4A8h     FlexSPI Clock Selection (FlexSPICLKSEL)                         32    RW    0000_000Fh

    4ACh     FlexSPI Clock Divider (FlexSPICLKDIV)                           32    RW    4000_0000h

    52Ch     PLL Clock Divider Clock Selection (PLLCLKDIVSEL)                 32    RW    0000_0007h

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    480 / 3791

<!-- page 481 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

     Offset     Register                                                        Width    Access   Reset value
                                                                                                     (In bits)

    530h     I3C0 Functional Clock Selection (I3C0FCLKSEL)                    32    RW    0000_0007h

    534h     I3C0 FCLK_STC Clock Selection (I3C0FCLKSTCSEL)                32    RW    0000_0007h

    538h     I3C0 FCLK_STC Clock Divider (I3C0FCLKSTCDIV)                  32    RW    4000_0000h

    53Ch     I3C0 FCLK Slow Clock Divider (I3C0FCLKSDIV)                    32    RW    4000_0000h

    540h     I3C0 Functional Clock FCLK Divider (I3C0FCLKDIV)                 32    RW    4000_0000h

    544h     I3C0 FCLK Slow Selection (I3C0FCLKSSEL)                       32    RW    0000_0000h

    548h     MICFIL Clock Selection (MICFILFCLKSEL)                         32    RW    0000_000Fh

    54Ch     MICFIL Clock Division (MICFILFCLKDIV)                          32    RW    4000_0000h

    558h    uSDHC Clock Selection (uSDHCCLKSEL)                         32    RW    0000_0007h

    55Ch    uSDHC Function Clock Divider (uSDHCCLKDIV)                    32    RW    4000_0000h

    560h     FLEXIO Clock Selection (FLEXIOCLKSEL)                         32    RW    0000_0007h

    564h     FLEXIO Function Clock Divider (FLEXIOCLKDIV)                    32    RW    4000_0000h

    5A0h    FLEXCAN0 Clock Selection (FLEXCAN0CLKSEL)                   32    RW    0000_0007h

    5A4h    FLEXCAN0 Function Clock Divider (FLEXCAN0CLKDIV)              32    RW    0000_0000h

    5A8h    FLEXCAN1 Clock Selection (FLEXCAN1CLKSEL)                   32    RW    0000_0007h

    5ACh    FLEXCAN1 Function Clock Divider (FLEXCAN1CLKDIV)              32    RW    4000_0000h

    5B0h      Ethernet RMII Clock Selection (ENETRMIICLKSEL)                  32    RW    0000_0007h

    5B4h      Ethernet RMII Function Clock Divider (ENETRMIICLKDIV)             32    RW    4000_0000h

    5B8h      Ethernet PTP REF Clock Selection (ENETPTPREFCLKSEL)           32    RW    0000_0007h

    5BCh      Ethernet PTP REF Function Clock Divider (ENETPTPREFCLKDIV)      32    RW    4000_0000h

    5C0h      Ethernet PHY Interface Select (ENET_PHY_INTF_SEL)               32    RW    0000_0000h

    5C4h     Sideband Flow Control (ENET_SBD_FLOW_CTRL)                  32    RW    0000_0000h

    5D4h    EWM0 Clock Selection (EWM0CLKSEL)                           32    RW    0000_0001h

    5D8h    WDT1 Clock Selection (WDT1CLKSEL)                           32    RW    0000_0003h

    5DCh    WDT1 Function Clock Divider (WDT1CLKDIV)                      32    RW    4000_0000h

    5E0h    OSTIMER Clock Selection (OSTIMERCLKSEL)                     32    RW    0000_0003h

    5F0h    CMP0 Function Clock Selection (CMP0FCLKSEL)                   32    RW    0000_0007h

    5F4h    CMP0 Function Clock Divider (CMP0FCLKDIV)                     32    RW    4000_0000h

    5F8h    CMP0 Round Robin Clock Selection (CMP0RRCLKSEL)              32    RW    0000_0007h

    5FCh    CMP0 Round Robin Clock Divider (CMP0RRCLKDIV)                32    RW    4000_0000h

    600h    CMP1 Function Clock Selection (CMP1FCLKSEL)                   32    RW    0000_0007h

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    481 / 3791

<!-- page 482 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

     Offset     Register                                                        Width    Access   Reset value
                                                                                                     (In bits)

    604h    CMP1 Function Clock Divider (CMP1FCLKDIV)                     32    RW    4000_0000h

    608h    CMP1 Round Robin Clock Source Select (CMP1RRCLKSEL)          32    RW    0000_0007h

    60Ch    CMP1 Round Robin Clock Division (CMP1RRCLKDIV)               32    RW    4000_0000h

    610h    CMP2 Function Clock Source Select (CMP2FCLKSEL)               32    RW    0000_0007h

    614h    CMP2 Function Clock Division (CMP2FCLKDIV)                     32    RW    4000_0000h

    618h    CMP2 Round Robin Clock Source Select (CMP2RRCLKSEL)          32    RW    0000_0007h

    61Ch    CMP2 Round Robin Clock Division (CMP2RRCLKDIV)               32    RW    4000_0000h

    800h    CPU Control for Multiple Processors (CPUCTRL)                    32    RW    0000_0028h

    804h     Coprocessor Boot Address (CPBOOT)                            32    RW    0000_0000h

    80Ch    CPU Status (CPUSTAT)                                        32     R     See section

    824h    LPCAC Control (LPCAC_CTRL)                                 32    RW    0000_0031h

  850h - 874h  LP_FLEXCOMM Clock Divider (FLEXCOMM0CLKDIV -               32    RW    4000_0000h
            FLEXCOMM9CLKDIV)

    880h     SAI0 Function Clock Source Select (SAI0CLKSEL)                   32    RW    0000_0007h

    884h     SAI1 Function Clock Source Select (SAI1CLKSEL)                   32    RW    0000_0007h

    888h     SAI0 Function Clock Division (SAI0CLKDIV)                        32    RW    4000_0000h

    88Ch     SAI1 Function Clock Division (SAI1CLKDIV)                        32    RW    4000_0000h

    890h    EMVSIM0 Clock Source Select (EMVSIM0CLKSEL)                  32    RW    0000_0007h

    894h    EMVSIM1 Clock Source Select (EMVSIM1CLKSEL)                  32    RW    0000_0007h

    898h    EMVSIM0 Function Clock Division (EMVSIM0CLKDIV)               32    RW    4000_0000h

    89Ch    EMVSIM1 Function Clock Division (EMVSIM1CLKDIV)               32    RW    4000_0000h

    A18h     Clock Control (CLOCK_CTRL)                                   32    RW    See section

    B30h     I3C1 Functional Clock Selection (I3C1FCLKSEL)                    32    RW    0000_0007h

    B34h      Selects the I3C1 Time Control clock (I3C1FCLKSTCSEL)             32    RW    0000_0007h

    B38h     I3C1 FCLK_STC Clock Divider (I3C1FCLKSTCDIV)                  32    RW    4000_0000h

    B3Ch     I3C1 FCLK Slow clock Divider (I3C1FCLKSDIV)                     32    RW    4000_0000h

    B40h     I3C1 Functional Clock FCLK Divider (I3C1FCLKDIV)                 32    RW    4000_0000h

    B44h     I3C1 FCLK Slow Selection (I3C1FCLKSSEL)                       32    RW    0000_0000h

    B50h    ETB Counter Status Register (ETB_STATUS)                       32    RW    0000_0000h

    B54h    ETB Counter Control Register (ETB_COUNTER_CTRL)               32    RW    0000_0000h

    B58h    ETB Counter Reload Register (ETB_COUNTER_RELOAD)            32    RW    0000_0000h

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    482 / 3791

<!-- page 483 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

     Offset     Register                                                        Width    Access   Reset value
                                                                                                     (In bits)

    B5Ch    ETB Counter Value Register (ETB_COUNTER_VALUE)               32     R     0000_0000h

    B60h     Gray to Binary Converter Gray code_gray[31:0] (GRAY_CODE_LSB)    32    RW    0000_0000h

    B64h     Gray to Binary Converter Gray code_gray[41:32]                    32    RW    0000_0000h
            (GRAY_CODE_MSB)

    B68h     Gray to Binary Converter Binary Code [31:0] (BINARY_CODE_LSB)     32     R     0000_0000h

    B6Ch     Gray to Binary Converter Binary Code [41:32]                       32     R     0000_0000h
            (BINARY_CODE_MSB)

    E04h      Control Automatic Clock Gating (AUTOCLKGATEOVERRIDE)         32    RW    0000_FFFFh

    E2Ch      Control Automatic Clock Gating C (AUTOCLKGATEOVERRIDEC)      32    RW    0000_0000h

    E38h    PWM0 Submodule Control (PWM0SUBCTL)                        32    RW    0000_0000h

    E3Ch   PWM1 Submodule Control (PWM1SUBCTL)                        32    RW    0000_0000h

    E40h    CTIMER Global Start Enable (CTIMERGLOBALSTARTEN)            32    RW    0000_0000h

    E44h    RAM ECC Enable Control (ECC_ENABLE_CTRL)                   32    RW    0000_0003h

    FF0h    JTAG Chip ID (JTAG_ID)                                       32     R     See section

    FF4h     Device Type (DEVICE_TYPE)                                   32     R     See section

    FF8h     Device ID (DEVICE_ID0)                                       32     R     See section

    FFCh     Chip Revision ID and Number (DIEID)                             32     R     See section

17.4.1.2 AHB Matrix Priority Control (AHBMATPRIO)

Offset


 Register                   Offset

 AHBMATPRIO          10h


Function
The Multilayer AHB Matrix arbitrates between masters, when they attempt to access the same matrix slave port at the same
time. The priority values are 3 = highest, 0 = lowest. When the priority is the same, the master with the lower master number is
given priority.

                                           NOTE
               Be careful when modifying this register as improper settings can seriously degrade the performance.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    483 / 3791

<!-- page 484 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                           PRI_USB_FS_E               PRI_COOLFLU  PRI_COOLFLU  PRI_COOLFLU
           Reserved     PRI_USDHC    PRI_USB_HS                 PRI_NPU_D
   W                                          NET                               X_Y_...         X_X              X_I

Reset     u      u      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                        PRI_CPU1_CB   PRI_CPU1_SB   PRI_CPU0_SB  PRI_CPU0_CB
        PRI_NPU_PQ      Reserved       DMA1        DMA0
   W                                                                          US_S...          US_S...        US          US

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-30      Reserved
    —

     29-28    USDHC bus master priority level
  PRI_USDHC        00b - level 0
                     01b - level 1
                     10b - level 2
                     11b - level 3

     27-26     USB-HS bus master priority level
  PRI_USB_HS        00b - level 0
                     01b - level 1
                     10b - level 2
                     11b - level 3

     25-24     USB-FS and ENET bus master priority level
 PRI_USB_FS_E       00b - level 0
     NET            01b - level 1
                     10b - level 2
                     11b - level 3

     23-22    NPU D bus master priority level
  PRI_NPU_D         00b - level 0
                     01b - level 1
                     10b - level 2
                     11b - level 3

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    484 / 3791

<!-- page 485 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
     21-20      CoolFlux Y bus master priority level
 PRI_COOLFLU       00b - level 0
   X_Y_ESPI         01b - level 1
                     10b - level 2
                     11b - level 3

     19-18      CoolFlux X bus master priority level
 PRI_COOLFLU       00b - level 0
     X_X            01b - level 1
                     10b - level 2
                     11b - level 3

     17-16      CoolFlux I bus master priority level
 PRI_COOLFLU       00b - level 0
       X_I            01b - level 1
                     10b - level 2
                     11b - level 3

     15-14    NPU O bus and Powerquad bus master priority level
 PRI_NPU_PQ        00b - level 0
                     01b - level 1
                     10b - level 2
                     11b - level 3

     13-12      Reserved
    —

     11-10    DMA1 controller bus master priority level
    DMA1           00b - level 0
                     01b - level 1
                     10b - level 2
                     11b - level 3

       9-8     DMA0 controller bus master priority level
    DMA0           00b - level 0
                     01b - level 1
                     10b - level 2
                     11b - level 3

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    485 / 3791

<!-- page 486 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       7-6     CPU1 C-AHB/SmartDMA-I bus master priority level
 PRI_CPU1_CB       00b - level 0
 US_SmartDMA       01b - level 1
         _I
                     10b - level 2
                     11b - level 3

       5-4     CPU1 S-AHB/SmartDMA-D bus master priority level
 PRI_CPU1_SB       00b - level 0
 US_SmartDMA       01b - level 1
      _D
                     10b - level 2
                     11b - level 3

       3-2     CPU0 S-AHB bus master priority level
 PRI_CPU0_SB       00b - level 0
     US            01b - level 1
                     10b - level 2
                     11b - level 3

       1-0     CPU0 C-AHB bus master priority level
 PRI_CPU0_CB       00b - level 0
     US            01b - level 1
                     10b - level 2
                     11b - level 3


17.4.1.3  Secure CPU0 System Tick Calibration (CPU0STCKCAL)

Offset


 Register                   Offset

 CPU0STCKCAL        38h


Function
The CPU0STCKCAL register allows software to set up a default value for the SYST_CALIB register (refer to Arm
documentation) in the System Tick Timer of secure part of the CPU0.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    486 / 3791

<!-- page 487 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25      24      23     22     21     20      19     18     17     16

    R                                    NORE
                          Reserved                    SKEW                      TENMS
   W                                            F

Reset     u      u      u      u       u      u      0       0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                  TENMS
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-26      Reserved
    —

      25       Whether the device provides a reference clock to the processor.
   NOREF          0b - Reference clock is provided
                     1b - No reference clock is provided

      24       Whether the TENMS value is exact.
   SKEW           0b - TENMS value is exact
                     1b - TENMS value is not exact or not given

      23-0      Reload value for 10 ms (100 Hz) timing, subject to system clock skew errors. If the value reads as zero,
                  the calibration value is not known.
   TENMS


17.4.1.4  Non-Secure CPU0 System Tick Calibration (CPU0NSTCKCAL)

Offset


 Register                   Offset

 CPU0NSTCKCAL      3Ch


Function
The CPU0NSTCKCAL register allows software to set up a default value for the SYST_CALIB register in the System Tick Timer
of non-secure part of the CPU0.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    487 / 3791

<!-- page 488 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25      24      23     22     21     20      19     18     17     16

    R                                        NORE
                                             SKEW                      TENMS
                                                  F   W                    Reserved

Reset     u      u      u      u       u      u      0       0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                  TENMS
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-26      Reserved
    —

      25         Indicates whether the device provides a reference clock to the processor.
   NOREF          0b - Reference clock is provided
                     1b - No reference clock is provided

      24         Indicates whether the TENMS value is exact.
   SKEW           0b - TENMS value is exact
                     1b - TENMS value is not exact or not given

      23-0      Reload value for 10 ms (100 Hz) timing, subject to system clock skew errors. If the value reads as zero,
                  the calibration value is not known.
   TENMS


17.4.1.5  System tick calibration for CPU1 (CPU1STCKCAL)

Offset


 Register                   Offset

 CPU1STCKCAL        40h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    488 / 3791

<!-- page 489 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25      24      23     22     21     20      19     18     17     16

    R                                        NORE
                                             SKEW                      TENMS
                                                  F   W                    Reserved

Reset     u      u      u      u       u      u      0       0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                  TENMS
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-26      Reserved
    —

      25         Indicates whether the device provides a reference clock to the processor.
   NOREF          0b - Reference clock is provided
                     1b - No reference clock is provided

      24         Indicates whether the TENMS value is exact.
   SKEW           0b - TENMS value is exact
                     1b - TENMS value is not exact or not given

      23-0      Reload value for 10 ms (100 Hz) timing, subject to system clock skew errors. If the value reads as zero,
                  the calibration value is not known.
   TENMS


17.4.1.6 NMI Source Select (NMISRC)

Offset


 Register                   Offset

 NMISRC              48h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    489 / 3791

<!-- page 490 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R NMIEN  NMIEN
                                                                         Reserved
   W   CP...    CP...

Reset     0       0      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                             IRQCPU1                                             IRQCPU0
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31       Enables the Non-Maskable Interrupt (NMI) source selected by IRQCPU0.
  NMIENCPU0        0b - Disable.
                     1b - Enable.

      30       Enables the Non-Maskable Interrupt (NMI) source selected by IRQCPU1.
  NMIENCPU1        0b - Disable.
                     1b - Enable.

     29-16      Reserved
    —

      15-8      The IRQ number of the interrupt that acts as the Non-Maskable Interrupt (NMI) for CPU1, if enabled by
              NMIENCPU1.
   IRQCPU1

       7-0      The IRQ number of the interrupt that acts as the Non-Maskable Interrupt (NMI) for CPU0, if enabled by
              NMIENCPU0.
   IRQCPU0


17.4.1.7  Peripheral Reset Control 0 (PRESETCTRL0)

Offset


 Register                   Offset

 PRESETCTRL0         100h


Function
The PRESETCTRL0 register allows software to reset specific peripherals. Writing a 0 to any assigned bit in this register clears
the reset and allows the specified peripheral to operate. Writing a 1 asserts the reset.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    490 / 3791

<!-- page 491 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

                                           NOTE
          When modifying the PRESETCTRL registers, use the related PRESETCTRLSET and PRESETCTRLCLR
                registers to avoid setting or clearing bits unintentionally.


Diagram

 Bits      31     30     29     28     27     26     25     24      23     22     21     20      19     18     17     16

    R
       MAILB                    CRC_  DMA0  PINT_         GPIO4  GPIO3  GPIO2  GPIO1  GPIO0        PORT  PORT
           OX...   Reserv              RST   _RST   RST   Reserv    _R...     _R...     _R...     _R...     _R...   Reserv   4_R...   3_R...   W                    Reserved
                 ed                                          ed                                           ed

Reset     0      0      u      u      0      0      0      u       0      0      0      0       0      u      0      0

 Bits      15      14      13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
       PORT  PORT  PORT  MUX_  FLEXS        FMU_
           2_R...    1_R...    0_R...   RST      PI...   Reserv  RST   W                                                                                      Reserved
                                               ed

Reset     u       u       u      0       u      u       0      1       0      0      0      0       0      0      u      u


Fields


       Field       Function
      31        Inter-CPU communication Mailbox reset control
 MAILBOX_RST       0b - Block is not reset
                     1b - Block is reset

      30       Reserved Read value is undefined, only zero should be written.
    —

     29-28      Reserved
    —

      27     CRC reset control
   CRC_RST         0b - Block is not reset
                     1b - Block is reset

      26     DMA0 reset control
  DMA0_RST         0b - Block is not reset
                     1b - Block is reset

      25       PINT reset control
   PINT_RST         0b - Block is not reset
                     1b - Block is reset

      24       Reserved
    —

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    491 / 3791

<!-- page 492 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      23      GPIO4 reset control
  GPIO4_RST        0b - Block is not reset
                     1b - Block is reset

      22      GPIO3 reset control
  GPIO3_RST        0b - Block is not reset
                     1b - Block is reset

      21      GPIO2 reset control
  GPIO2_RST        0b - Block is not reset
                     1b - Block is reset

      20      GPIO1 reset control
  GPIO1_RST        0b - Block is not reset
                     1b - Block is reset

      19      GPIO0 reset control
  GPIO0_RST        0b - Block is not reset
                     1b - Block is reset

      18       Reserved
    —

      17      PORT4 reset control
  PORT4_RST        0b - Block is not reset
                     1b - Block is reset

      16      PORT3 reset control
  PORT3_RST        0b - Block is not reset
                     1b - Block is reset

      15      PORT2 reset control
  PORT2_RST        0b - Block is not reset
                     1b - Block is reset

      14      PORT1 reset control
  PORT1_RST        0b - Block is not reset
                     1b - Block is reset

      13      PORT0 controller reset control
  PORT0_RST        0b - Block is not reset
                     1b - Block is reset

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    492 / 3791

<!-- page 493 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      12      INPUTMUX reset control
  MUX_RST         0b - Block is not reset
                     1b - Block is reset

      11        FlexSPI reset control
 FLEXSPI_RST        0b - Block is not reset
                     1b - Block is reset

      10       Reserved
    —

       9        Flash management unit reset control
   FMU_RST         0b - Block is not reset
                     1b - Block is reset

       8-0       Reserved
    —


17.4.1.8  Peripheral Reset Control 1 (PRESETCTRL1)

Offset


 Register                   Offset

 PRESETCTRL1         104h


Function
The PRESETCTRL1 register allows software to reset specific peripherals. Writing a 0 to any assigned bit in this register clears
the reset and allows the specified peripheral to operate. Writing a 1 asserts the reset.

                                           NOTE
          When modifying the PRESETCTRL registers, use the related PRESETCTRLSET and PRESETCTRLCLR
                registers to avoid setting or clearing bits unintentionally.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    493 / 3791

<!-- page 494 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
         Smart                      TIMER  TIMER  USB0_  USB0_  Reserv  TIMER  MICFI   FC9_   FC8_   FC7_   FC6_   FC5_
          DM...   Reserv  Reserv  Reserv    1_...      0_...     FS...     FS...     ed       2_...      L_...   RST   RST   RST   RST   RST   W
                 ed     ed     ed

Reset     0      u      0      u       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
        FC4_   FC3_   FC2_   FC1_   FC0_  UTICK  EVSIM  EVSIM        RTC_  DAC0_  ADC1_  ADC0_  SCT_  OSTIM  MRT_
       RST   RST   RST   RST   RST     _R...      1_...      0_...   Reserv  RST   RST   RST   RST   RST     ER...   RST   W
                                                                     ed

Reset     0      0      0      0       0      0      0      0       u      u      0      0       0      0      0      0


Fields


       Field       Function
      31      SmartDMA reset control
 SmartDMA_RS       0b - Block is not reset
      T             1b - Block is reset

      30       Reserved
    —

      29       Reserved
    —

      28       Reserved
    —

      27      CTIMER1 reset control
  TIMER1_RST        0b - Block is not reset
                     1b - Block is reset

      26      CTIMER0 reset control
  TIMER0_RST        0b - Block is not reset
                     1b - Block is reset

      25      USB FS reset control
 USB0_FS_RST       0b - Block is not reset
                     1b - Block is reset

      24      USB FS DCD reset control
 USB0_FS_DCD       0b - Block is not reset
    _RST           1b - Block is reset

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    494 / 3791

<!-- page 495 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      23       Reserved
    —

      22      CTIMER2 reset control
  TIMER2_RST        0b - Block is not reset
                     1b - Block is reset

      21       MICFIL reset control
  MICFIL_RST        0b - Block is not reset
                     1b - Block is reset

      20      LP_FLEXCOMM9 reset control
   FC9_RST          0b - Block is not reset
                     1b - Block is reset

      19      LP_FLEXCOMM8 reset control
   FC8_RST          0b - Block is not reset
                     1b - Block is reset

      18      LP_FLEXCOMM7 reset control
   FC7_RST          0b - Block is not reset
                     1b - Block is reset

      17      LP_FLEXCOMM6 reset control
   FC6_RST          0b - Block is not reset
                     1b - Block is reset

      16      LP_FLEXCOMM5 reset control
   FC5_RST          0b - Block is not reset
                     1b - Block is reset

      15      LP_FLEXCOMM4 reset control
   FC4_RST          0b - Block is not reset
                     1b - Block is reset

      14      LP_FLEXCOMM3 reset control
   FC3_RST          0b - Block is not reset
                     1b - Block is reset

      13      LP_FLEXCOMM2 reset control
   FC2_RST          0b - Block is not reset
                     1b - Block is reset

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    495 / 3791

<!-- page 496 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      12      LP_FLEXCOMM1 reset control
   FC1_RST          0b - Block is not reset
                     1b - Block is reset

      11      LP_FLEXCOMM0 reset control
   FC0_RST          0b - Block is not reset
                     1b - Block is reset

      10      UTICK reset control
  UTICK_RST         0b - Block is not reset
                     1b - Block is reset

       9       EVSIM1 reset control
  EVSIM1_RST        0b - Block is not reset
                     1b - Block is reset

       8       EVSIM0 reset control
  EVSIM0_RST        0b - Block is not reset
                     1b - Block is reset

       7        Reserved
    —

       6      RTC reset control
   RTC_RST   RTC and Sub-second counter reset control

                                                 NOTE
                           To reset RTC correctly, you must write 1 to RTC.CTRL[SWR] when writing 1 to RTC_RST,
                            and write 0 to RTC.CTRL[SWR] after writing 0 to RTC_RST.

                     0b - Block is not reset
                     1b - Block is reset

       5      DAC0 reset control
  DAC0_RST         0b - Block is not reset
                     1b - Block is reset

       4      ADC1 reset control
  ADC1_RST         0b - Block is not reset
                     1b - Block is reset

       3      ADC0 reset control
  ADC0_RST

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    496 / 3791

<!-- page 497 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Block is not reset
                     1b - Block is reset

       2      SCT reset control
   SCT_RST         0b - Block is not reset
                     1b - Block is reset

       1      OS Event Timer reset control
 OSTIMER_RST       0b - Block is not reset
                     1b - Block is reset

       0      MRT reset control
   MRT_RST         0b - Block is not reset
                     1b - Block is reset


17.4.1.9  Peripheral Reset Control 2 (PRESETCTRL2)

Offset


 Register                   Offset

 PRESETCTRL2         108h


Function
The PRESETCTRL2 register allows software to reset specific peripherals. Writing a 0 to any assigned bit in this register clears
the reset and allows the specified peripheral to operate. Writing a 1 asserts the reset.

                                           NOTE
          When modifying the PRESETCTRL registers, use the related PRESETCTRLSET and PRESETCTRLCLR
                registers to avoid setting or clearing bits unintentionally.


Diagram

 Bits      31     30     29     28     27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                  TIMER  TIMER  PLU_   PQ_         USB_   USB_
        Reserv  Reserv                                        Reserv  Reserv    4_...      3_...   RST   RST   Reserv   HS_...   HS_...   W                               Reserved
          ed     ed                                          ed     ed                                   ed

Reset     0      0      0      0      0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12     11     10      9      8       7      6      5      4       3      2      1      0

    R
        FLEX   FLEX  Reserv                         FREQ          SAI1_   SAI0_   FLEXI  USDH  ENET_  DMA1  Reserv
         CAN...  CAN...    ed                                       ME_...  Reserv  RST   RST     O_...    C_R...   RST   _RST    ed   W                                   Reserved
                                                                     ed

Reset     0      0      0      0      0      0      0      0       0      0      0      0       0      0      0      0


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    497 / 3791

<!-- page 498 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Fields


       Field       Function
      31       Reserved
    —

      30       Reserved
    —

     29-25      Reserved
    —

      24       Reserved
    —

      23       Reserved
    —

      22      CTIMER4 reset control
  TIMER4_RST        0b - Block is not reset
                     1b - Block is reset

      21      CTIMER3 reset control
  TIMER3_RST        0b - Block is not reset
                     1b - Block is reset

      20      PLU reset control
   PLU_RST         0b - Block is not reset
                     1b - Block is reset

      19      PowerQuad reset control
   PQ_RST          0b - Block is not reset
                     1b - Block is reset

      18       Reserved
    —

      17      USB HS PHY reset control
 USB_HS_PHY_       0b - Block is not reset
     RST            1b - Block is reset

      16      USB HS reset control
 USB_HS_RST        0b - Block is not reset
                     1b - Block is reset

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    498 / 3791

<!-- page 499 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      15      CAN1 reset control
 FLEXCAN1_RS       0b - Block is not reset
      T             1b - Block is reset

      14      CAN0 reset control
 FLEXCAN0_RS       0b - Block is not reset
      T             1b - Block is reset

      13       Reserved
    —

      12-9      Reserved
    —

       8      FREQME reset control
 FREQME_RST       0b - Block is not reset
                     1b - Block is reset

       7        Reserved Should keep default value.
    —

       6        SAI1 reset control
   SAI1_RST         0b - Block is not reset
                     1b - Block is reset

       5        SAI0 reset control
   SAI0_RST         0b - Block is not reset
                     1b - Block is reset

       4       FLEXIO reset control
  FLEXIO_RST        0b - Block is not reset
                     1b - Block is reset

       3      uSDHC reset control
 USDHC_RST        0b - Block is not reset
                     1b - Block is reset

       2         Ethernet reset control
  ENET_RST         0b - Block is not reset
                     1b - Block is reset

       1      DMA1 reset control

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    499 / 3791

<!-- page 500 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  DMA1_RST         0b - Block is not reset
                     1b - Block is reset

       0        Reserved
    —


17.4.1.10  Peripheral Reset Control 3 (PRESETCTRL3)

Offset


 Register                   Offset

 PRESETCTRL3        10Ch


Function
The PRESETCTRL3 register allows software to reset specific peripherals. Writing a 0 to any assigned bit in this register clears
the reset and allows the specified peripheral to operate. Writing a 1 asserts the reset.

                                           NOTE
          When modifying the PRESETCTRL registers, use the related PRESETCTRLSET and PRESETCTRLCLR
                registers to avoid setting or clearing bits unintentionally.


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22      21     20      19     18     17     16

    R                         SEMA                 EIM_  EWM_   TSI_   NPU_  COOL  VREF_  CMP2
                   Reserved                       Reserved                                                              Reserved
   W                                           42_...               RST   RST   RST   RST    FLU...   RST   _RST

Reset     0      0      0      0       0      0      0      0       0      0       0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R OPAM  OPAM  OPAM  DAC2_  DAC1_  Reserv  Reserv  AOI0_  PWM1 PWM0  QDC1  QDC0  COOL  SINC_   I3C1_   I3C0_
   W   P2_...    P1_...    P0_...   RST   RST     ed     ed    RST   _RST   _RST   _RST   _RST   FLU...   RST   RST   RST

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-28      Reserved
    —

      27       Semaphore reset control

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    500 / 3791

<!-- page 501 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

 SEMA42_RST        0b - Block is not reset
                     1b - Block is reset

     26-25      Reserved
    —

      24      EIM reset control
   EIM_RST          0b - Block is not reset
                     1b - Block is reset

      23     EWM reset control
  EWM_RST         0b - Block is not reset
                     1b - Block is reset

      22       TSI reset control
   TSI_RST          0b - Block is not reset
                     1b - Block is reset

      21     NPU reset control
   NPU_RST         0b - Block is not reset
                     1b - Block is reset

      20        CoolFlux APB reset control
 COOLFLUX_AP       0b - Block is not reset
    B_RST           1b - Block is reset

      19      VREF reset control
  VREF_RST         0b - Block is not reset
                     1b - Block is reset

      18     CMP2 reset control
  CMP2_RST         0b - Block is not reset
                     1b - Block is reset

     17-16      Reserved
    —

      15     OPAMP2 reset control
 OPAMP2_RST       0b - Block is not reset
                     1b - Block is reset

      14     OPAMP1 reset control

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    501 / 3791

<!-- page 502 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

 OPAMP1_RST       0b - Block is not reset
                     1b - Block is reset

      13     OPAMP0 reset control
 OPAMP0_RST       0b - Block is not reset
                     1b - Block is reset

      12      DAC2 reset control
  DAC2_RST         0b - Block is not reset
                     1b - Block is reset

      11      DAC1 reset control
  DAC1_RST         0b - Block is not reset
                     1b - Block is reset

      10       Reserved Read value is undefined, only zero should be written.
    —

       9        Reserved Read value is undefined, only zero should be written.
    —

       8       AOI0 reset control
   AOI0_RST         0b - Block is not reset
                     1b - Block is reset

       7     PWM1 reset control
  PWM1_RST         0b - Block is not reset
                     1b - Block is reset

       6     PWM0 reset control
  PWM0_RST         0b - Block is not reset
                     1b - Block is reset

       5      QDC1 reset control
  QDC1_RST         0b - Block is not reset
                     1b - Block is reset

       4      QDC0 reset control
  QDC0_RST         0b - Block is not reset
                     1b - Block is reset

       3        CoolFlux reset control

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    502 / 3791

<!-- page 503 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

 COOLFLUX_R        0b - Block is not reset
     ST                     1b - Block is reset

       2       SINC reset control
   SINC_RST         0b - Block is not reset
                     1b - Block is reset

       1        I3C1 reset control
   I3C1_RST         0b - Block is not reset
                     1b - Block is reset

       0        I3C0 reset control
   I3C0_RST         0b - Block is not reset
                     1b - Block is reset


17.4.1.11  Peripheral Reset Control Set (PRESETCTRLSET0 - PRESETCTRLSET3)

Offset


 Register                   Offset

 PRESETCTRLSET0     120h

 PRESETCTRLSET1     124h

 PRESETCTRLSET2     128h

 PRESETCTRLSET3     12Ch


Function
Writing a 1 to a bit position in a write-only PRESETCTRLSETn register sets the corresponding position in PRESETCTRLn.

                                           NOTE
            To reset RTC correctly, you must write 1 to RTC.CTRL[SWR] when writing 1 to PRESETCTRL1[RTC_RST], and
                write 0 to RTC.CTRL[SWR] after writing 0 to PRESETCTRL1[RTC_RST].





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    503 / 3791

<!-- page 504 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

   W                                                DATA

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R

   W                                                DATA

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-0      Data array value, refer to corresponding position in PRESETCTRLn.
    DATA


17.4.1.12  Peripheral Reset Control Clear (PRESETCTRLCLR0 - PRESETCTRLCLR3)

Offset


 Register                   Offset

 PRESETCTRLCLR0     140h

 PRESETCTRLCLR1     144h

 PRESETCTRLCLR2     148h

 PRESETCTRLCLR3     14Ch


Function
Writing a 1 to a bit position in a write-only PRESETCTRLCLRn register clears the corresponding position in PRESETCTRLn.

                                           NOTE
            To reset RTC correctly, you must write 1 to RTC.CTRL[SWR] when writing 1 to PRESETCTRL1[RTC_RST], and
                write 0 to RTC.CTRL[SWR] after writing 0 to PRESETCTRL1[RTC_RST].





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    504 / 3791

<!-- page 505 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

   W                                                DATA

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R

   W                                                DATA

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-0      Data array value, refer to corresponding position in PRESETCTRLn.
    DATA


17.4.1.13 AHB Clock Control 0 (AHBCLKCTRL0)

Offset


 Register                   Offset

 AHBCLKCTRL0         200h


Function
The AHBCLKCTRLn registers enable the clocks of the individual modules.

                                           NOTE
          When modifying the AHBCLKCTRL registers, use the related AHBCLKCTRLSET and AHBCLKCTRLCLR
                registers to avoid setting or clearing bits unintentionally.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    505 / 3791

<!-- page 506 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
       MAILB      WWD  WWD                                                             PORT  PORT
                              CRC  DMA0   PINT         GPIO4  GPIO3  GPIO2  GPIO1  GPIO0       OX   Reserv   T1     T0                           Reserv                                         Reserv    4      3   W
                 ed                                          ed                                          ed

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
       PORT  PORT  PORT        FLEXS             RAMH  RAMG  RAMF  RAME  RAMD  RAMC  RAMB
                       MUX        FMC   FMU                                        ROM          2      1      0                PI                        _CT...   _CT...   _CT...   _CT...   _CT...   _CT...   _CT...          Reserv   W
                                                                                                                         ed

Reset     0      0      0      0       0      1      1      0       0      0      0      0       0      0      1      1


Fields


       Field       Function
      31       Enables the clock for the Inter CPU communication Mailbox.
   MAILBOX         0b - Disables clock
                     1b - Enables clock

      30       Reserved Read value is undefined, only zero should be written.
    —

      29       Enables the clock for WWDT1
   WWDT1          0b - Disables clock
                     1b - Enables clock

      28       Enables the clock for WWDT0
   WWDT0          0b - Disables clock
                     1b - Enables clock

      27       Enables the clock for CRC
    CRC            0b - Disables clock
                     1b - Enables clock

      26       Enables the clock for DMA0
    DMA0           0b - Disables clock
                     1b - Enables clock

      25       Enables the clock for PINT
     PINT           0b - Disables clock
                     1b - Enables clock

      24       Reserved

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    506 / 3791

<!-- page 507 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

    —

      23       Enables the clock for GPIO4
    GPIO4           0b - Disables clock
                     1b - Enables clock

      22       Enables the clock for GPIO3
    GPIO3           0b - Disables clock
                     1b - Enables clock

      21       Enables the clock for GPIO2
    GPIO2           0b - Disables clock
                     1b - Enables clock

      20       Enables the clock for GPIO1
    GPIO1           0b - Disables clock
                     1b - Enables clock

      19       Enables the clock for GPIO0
    GPIO0           0b - Disables clock
                     1b - Enables clock

      18       Reserved
    —

      17       Enables the clock for PORT4
    PORT4          0b - Disables clock
                     1b - Enables clock

      16       Enables the clock for PORT3
    PORT3          0b - Disables clock
                     1b - Enables clock

      15       Enables the clock for PORT2
    PORT2          0b - Disables clock
                     1b - Enables clock

      14       Enables the clock for PORT1
    PORT1          0b - Disables clock
                     1b - Enables clock

      13       Enables the clock for PORT0 controller

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    507 / 3791

<!-- page 508 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

    PORT0          0b - Disables clock
                     1b - Enables clock

      12       Enables the clock for INPUTMUX
    MUX           0b - Disables clock
                     1b - Enables clock

      11       Enables the clock for FlexSPI
   FLEXSPI          0b - Disables clock
                     1b - Enables clock

      10       Enables the clock for the Flash Memory Controller
    FMC            0b - Disables clock
                     1b - Enables clock

       9        Enables the clock for the Flash Management Unit
    FMU            0b - Disables clock
                     1b - Enables clock

       8        Enables the clock for the RAMH Controller
 RAMH_CTRL        0b - Disables clock
                     1b - Enables clock

       7        Enables the clock for the RAMG Controller
 RAMG_CTRL        0b - Disables clock
                     1b - Enables clock

       6        Enables the clock for the RAMF Controller
  RAMF_CTRL        0b - Disables clock
                     1b - Enables clock

       5        Enables the clock for the RAME Controller
  RAME_CTRL        0b - Disables clock
                     1b - Enables clock

       4        Enables the clock for the RAMD Controller
 RAMD_CTRL        0b - Disables clock
                     1b - Enables clock

       3        Enables the clock for the RAMC Controller
 RAMC_CTRL        0b - Disables clock
                     1b - Enables clock

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    508 / 3791

<!-- page 509 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       2        Enables the clock for the RAMB Controller
  RAMB_CTRL        0b - Disables clock
                     1b - Enables clock

       1        Enables the clock for the ROM
    ROM           0b - Disables clock
                     1b - Enables clock

       0        Reserved Read value is undefined, only zero should be written.
    —


17.4.1.14 AHB Clock Control 1 (AHBCLKCTRL1)

Offset


 Register                   Offset

 AHBCLKCTRL1         204h


Function
The AHBCLKCTRLn registers enable the clocks of the individual modules.

                                           NOTE
          When modifying the AHBCLKCTRL registers, use the related AHBCLKCTRLSET and AHBCLKCTRLCLR
                registers to avoid setting or clearing bits unintentionally.


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
         Smart                      TIMER  TIMER  USB0_  USB0_  Reserv  TIMER  MICFI
                                                                                FC9    FC8    FC7    FC6    FC5       DMA   Reserv  Reserv  Reserv    1      0     FS      FS...     ed      2      L   W
                 ed     ed     ed

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                               EVSIM  EVSIM                                         OSTIM
        FC4    FC3    FC2    FC1    FC0   UTICK                    RTC   DAC0  ADC1  ADC0   SCT         MRT                                                       1      0    Reserv                                ER   W
                                                                     ed

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    509 / 3791

<!-- page 510 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Fields


       Field       Function
      31       Enables the clock for SmartDMA
   SmartDMA         0b - Disables clock
                     1b - Enables clock

      30       Reserved
    —

      29       Reserved
    —

      28       Reserved
    —

      27       Enables the clock for CTIMER1
    TIMER1          0b - Disables clock
                     1b - Enables clock

      26       Enables the clock for CTIMER0
    TIMER0          0b - Disables clock
                     1b - Enables clock

      25       Enables the clock for USB-FS
   USB0_FS          0b - Disables clock
                     1b - Enables clock

      24       Enables the clock for USB-FS DCD
 USB0_FS_DCD       0b - Disables clock
                     1b - Enables clock

      23       Reserved
    —

      22       Enables the clock for CTIMER2
    TIMER2          0b - Disables clock
                     1b - Enables clock

      21       Enables the clock for MICFIL
    MICFIL           0b - Disables clock
                     1b - Enables clock

      20       Enables the clock for LP_FLEXCOMM9
     FC9

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    510 / 3791

<!-- page 511 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Disables clock
                     1b - Enables clock

      19       Enables the clock for LP_FLEXCOMM8
     FC8            0b - Disables clock
                     1b - Enables clock

      18       Enables the clock for LP_FLEXCOMM7
     FC7            0b - Disables clock
                     1b - Enables clock

      17       Enables the clock for LP_FLEXCOMM6
     FC6            0b - Disables clock
                     1b - Enables clock

      16       Enables the clock for LP_FLEXCOMM5
     FC5            0b - Disables clock
                     1b - Enables clock

      15       Enables the clock for LP_FLEXCOMM4
     FC4            0b - Disables clock
                     1b - Enables clock

      14       Enables the clock for LP_FLEXCOMM3
     FC3            0b - Disables clock
                     1b - Enables clock

      13       Enables the clock for LP_FLEXCOMM2
     FC2            0b - Disables clock
                     1b - Enables clock

      12       Enables the clock for LP_FLEXCOMM1
     FC1            0b - Disables clock
                     1b - Enables clock

      11       Enables the clock for LP_FLEXCOMM0
     FC0            0b - Disables clock
                     1b - Enables clock

      10       Enables the clock for UTICK
    UTICK           0b - Disables clock
                     1b - Enables clock

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    511 / 3791

<!-- page 512 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       9        Enables the clock for EVSIM1
    EVSIM1          0b - Disables clock
                     1b - Enables clock

       8        Enables the clock for EVSIM0
    EVSIM0          0b - Disables clock
                     1b - Enables clock

       7        Reserved
    —

       6        Enables the clock for RTC
    RTC            0b - Disables clock
                     1b - Enables clock

       5        Enables the clock for DAC0
    DAC0           0b - Disables clock
                     1b - Enables clock

       4        Enables the clock for ADC1
    ADC1           0b - Disables clock
                     1b - Enables clock

       3        Enables the clock for ADC0
    ADC0           0b - Disables clock
                     1b - Enables clock

       2        Enables the clock for SCT
     SCT            0b - Disables clock
                     1b - Enables clock

       1        Enables the clock for the OS Event Timer
   OSTIMER         0b - Disables clock
                     1b - Enables clock

       0        Enables the clock for MRT
    MRT            0b - Disables clock
                     1b - Enables clock





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    512 / 3791

<!-- page 513 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

17.4.1.15 AHB Clock Control 2 (AHBCLKCTRL2)

Offset


 Register                   Offset

 AHBCLKCTRL2         208h


Function
The AHBCLKCTRLn registers enable the clocks of the individual modules.

                                           NOTE
          When modifying the AHBCLKCTRL registers, use the related AHBCLKCTRLSET and AHBCLKCTRLCLR
                registers to avoid setting or clearing bits unintentionally.


Diagram

 Bits      31     30     29     28     27     26     25     24     23     22     21     20      19     18     17     16

    R
                                                                   TIMER  TIMER  PLU_                USB_  USB_
                                   SCG                                     PQ        Reserv  Reserv  Reserv                         Reserv                   4      3     LUT           Reserv   HS_...   HS   W                            Reserved                       Reserved
          ed     ed     ed                            ed                                                  ed

Reset     0      0      0      0      0      0      0      0      0      0      0      0       0      0      0      0

 Bits      15     14     13     12     11     10      9      8       7      6      5      4       3      2      1      0

    R
        FLEX   FLEX                               FREQ                         FLEXI  uSDH                  Reserv
                                                                             SAI1    SAI0               ENET  DMA1       CAN1  CAN0  Reserv                        ME   Reserv             O     C                     ed   W                                   Reserved
                         ed                                          ed

Reset     0      0      0      0      0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31       Reserved
    —

      30       Reserved
    —

      29       Reserved
    —

     28-27      Reserved
    —

      26       Enables the clock for SCG

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    513 / 3791

<!-- page 514 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

    SCG            0b - Disables clock
                     1b - Enables clock

      25       Reserved Read value is undefined, only zero should be written.
    —

     24-23      Reserved
    —

      22       Enables the clock for CTIMER4
    TIMER4          0b - Disables clock
                     1b - Enables clock

      21       Enables the clock for CTIMER3
    TIMER3          0b - Disables clock
                     1b - Enables clock

      20       Enables the clock for PLU_LUT
   PLU_LUT          0b - Disables clock
                     1b - Enables clock

      19       Enables the clock for Powerquad
     PQ            0b - Disables clock
                     1b - Enables clock

      18       Reserved
    —

      17       Enables the clock for USB HS PHY
 USB_HS_PHY        0b - Disables clock
                     1b - Enables clock

      16       Enables the clock for USB HS
   USB_HS          0b - Disables clock
                     1b - Enables clock

      15       Enables the clock for FLEXCAN1
  FLEXCAN1         0b - Disables clock
                     1b - Enables clock

      14       Enables the clock for FLEXCAN0
  FLEXCAN0

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    514 / 3791

<!-- page 515 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Disables clock
                     1b - Enables clock

      13       Reserved
    —

      12-9      Reserved
    —

       8        Enables the clock for the Frequency meter
   FREQME          0b - Disables clock
                     1b - Enables clock

       7        Reserved Should keep default value.
    —

       6        Enables the clock for SAI1
     SAI1            0b - Disables clock
                     1b - Enables clock

       5        Enables the clock for SAI0
     SAI0            0b - Disables clock
                     1b - Enables clock

       4        Enables the clock for FlexIO
    FLEXIO          0b - Disables clock
                     1b - Enable clock

       3        Enables the clock for uSDHC
   uSDHC          0b - Disables clock
                     1b - Enables clock

       2        Enables the clock for Ethernet
    ENET           0b - Disables clock
                     1b - Enables clock

       1        Enables the clock for DMA1
    DMA1           0b - Disables clock
                     1b - Enables clock

       0        Reserved
    —




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    515 / 3791

<!-- page 516 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

17.4.1.16 AHB Clock Control 3 (AHBCLKCTRL3)

Offset


 Register                   Offset

 AHBCLKCTRL3        20Ch


Function
The AHBCLKCTRLn registers enable the clocks of the individual modules.

                                           NOTE
          When modifying the AHBCLKCTRL registers, use the related AHBCLKCTRLSET and AHBCLKCTRLCLR
                registers to avoid setting or clearing bits unintentionally.


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                             SEMA                                     COOL
                  Reserved                  INTM  ERM    EIM   EWM    TSI   NPU         VREF  CMP2
                                       42                                                            FLU...   W                                                                                                                 Reserved

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R OPAM  OPAM  OPAM                  Reserv  Reserv                               COOL
                         DAC2  DAC1              EVTG  PWM1 PWM0  QDC1  QDC0         SINC    I3C1    I3C0
   W   P2     P1     P0                    ed     ed                                    FLUX

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-28      Reserved
    —

      27       Enables the clock for Semaphore
   SEMA42          0b - Disables clock
                     1b - Enables clock

      26       Enables the clock for INTM
     INTM           0b - Disables clock
                     1b - Enables clock

      25       Enables the clock for ERM
    ERM           0b - Disables clock
                     1b - Enables clock

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    516 / 3791

<!-- page 517 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      24       Enables the clock for EIM
     EIM            0b - Disables clock
                     1b - Enables clock

      23       Enables the clock for EWM
   EWM           0b - Disables clock
                     1b - Enables clock

      22       Enables the clock for TSI
      TSI            0b - Disables clock
                     1b - Enables clock

      21       Enables the clock for NPU
    NPU            0b - Disables clock
                     1b - Enables clock

      20       Enables the clock for CoolFlux APB
 COOLFLUX_AP       0b - Disables clock
      B             1b - Enables clock (CoolFlux needs to be properly programmed before the clock enabled.)

      19       Enables the clock for VREF
    VREF           0b - Disables clock
                     1b - Enables clock

      18       Enables the clock for CMP2
    CMP2           0b - Disables clock
                     1b - Enables clock

     17-16      Reserved
    —

      15       Enables the clock for OPAMP2
   OPAMP2          0b - Disables clock
                     1b - Enables clock

      14       Enables the clock for OPAMP1
   OPAMP1          0b - Disables clock
                     1b - Enables clock

      13       Enables the clock for OPAMP0
   OPAMP0          0b - Disables clock
                     1b - Enables clock

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    517 / 3791

<!-- page 518 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      12       Enables the clock for DAC2
    DAC2           0b - Disables clock
                     1b - Enables clock

      11       Enables the clock for DAC1
    DAC1           0b - Disables clock
                     1b - Enables clock

      10       Reserved Read value is undefined, only zero should be written.
    —

       9        Reserved Read value is undefined, only zero should be written.
    —

       8        Enables the clock for EVTG
    EVTG           0b - Disables clock
                     1b - Enables clock

       7        Enables the clock for PWM1
   PWM1           0b - Disables clock
                     1b - Enables clock

       6        Enables the clock for PWM0
   PWM0           0b - Disables clock
                     1b - Enables clock

       5        Enables the clock for QDC1
    QDC1           0b - Disables clock
                     1b - Enables clock

       4        Enables the clock for QDC0
    QDC0           0b - Disables clock
                     1b - Enables clock

       3        Enables the clock for CoolFlux
  COOLFLUX         0b - Disables clock
                     1b - Enables clock

       2        Enables the clock for SINC
     SINC           0b - Disables clock
                     1b - Enables clock

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    518 / 3791

<!-- page 519 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       1        Enables the clock for I3C1
      I3C1            0b - Disables clock
                     1b - Enables clock

       0        Enables the clock for I3C0
      I3C0            0b - Disables clock
                     1b - Enables clock


17.4.1.17 AHB Clock Control Set (AHBCLKCTRLSET0 - AHBCLKCTRLSET3)

Offset


 Register                   Offset

 AHBCLKCTRLSET0     220h

 AHBCLKCTRLSET1     224h

 AHBCLKCTRLSET2     228h

 AHBCLKCTRLSET3     22Ch


Function
Writing a 1 to a bit position in a write-only AHBCLKCTRLSETn register sets the corresponding position in AHBCLKCTRLn.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

   W                                                DATA

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R

   W                                                DATA

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-0      Data array value
    DATA




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    519 / 3791

<!-- page 520 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

17.4.1.18 AHB Clock Control Clear (AHBCLKCTRLCLR0 - AHBCLKCTRLCLR3)

Offset


 Register                   Offset

 AHBCLKCTRLCLR0     240h

 AHBCLKCTRLCLR1     244h

 AHBCLKCTRLCLR2     248h

 AHBCLKCTRLCLR3     24Ch


Function
Writing a 1 to a bit position in a write-only AHBCLKCTRLCLRn register clears the corresponding position in AHBCLKCTRLn.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

   W                                                DATA

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R

   W                                                DATA

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-0      Data array value
    DATA


17.4.1.19 CPU0 System Tick Timer Source Select (SYSTICKCLKSEL0)

Offset


 Register                   Offset

 SYSTICKCLKSEL0      260h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    520 / 3791

<!-- page 521 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

   W                                                          Reserved

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                                                                    SEL
   W                                               Reserved

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the System Tick Timer for CPU0 source
     SEL            000b - SYSTICKCLKDIV0 output
                     001b - Clk 1 MHz clock
                     010b - LP Oscillator clock
                     011b - No clock
                     100b - No clock
                     101b - No clock
                     110b - No clock
                     111b - No clock


17.4.1.20 CPU1 System Tick Timer Source Select (SYSTICKCLKSEL1)

Offset


 Register                   Offset

 SYSTICKCLKSEL1      264h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    521 / 3791

<!-- page 522 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

   W                                                          Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                                                                    SEL
   W                                               Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the System Tick Timer for CPU1 source.
     SEL            000b - SYSTICKCLKDIV1 output
                     001b - Clk 1 MHz clock
                     010b - LP Oscillator clock
                     011b - No clock
                     100b - No clock
                     101b - No clock
                     110b - No clock
                     111b - No clock


17.4.1.21  Trace Clock Source Select (TRACECLKSEL)

Offset


 Register                   Offset

 TRACECLKSEL         268h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    522 / 3791

<!-- page 523 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

   W                                                          Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                                                                    SEL
   W                                               Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the trace clock source.
     SEL            000b - TRACECLKDIV output
                     001b - Clk 1 MHz clock
                     010b - LP Oscillator clock
                     011b - No clock
                     100b - No clock
                     101b - No clock
                     110b - No clock
                     111b - No clock


17.4.1.22 CTIMER Clock Source Select (CTIMERCLKSEL0 - CTIMERCLKSEL4)

Offset


 Register                   Offset

 CTIMERCLKSEL0      26Ch

 CTIMERCLKSEL1       270h

 CTIMERCLKSEL2       274h

 CTIMERCLKSEL3       278h

 CTIMERCLKSEL4      27Ch





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    523 / 3791

<!-- page 524 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

   W                                                          Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                                                                SEL
   W                                           Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       1      1      1      1


Fields


       Field       Function
      31-4      Reserved
    —

       3-0        Selects the CTIMER clock source.
     SEL            0000b - FRO_1M clock
                     0001b - PLL0 clock
                     0010b - PLL1_clk0 clock
                     0011b - FRO_HF clock
                     0100b - FRO 12MHz clock
                     0101b - SAI0 MCLK IN clock
                     0110b - LP Oscillator clock
                     0111b - No clock
                     1000b - SAI1 MCLK IN clock
                     1001b - SAI0 TX_BCLK clock
                     1010b - SAI0 RX_BCLK clock
                     1011b - SAI1 TX_BCLK clock
                     1100b - SAI1 RX_BCLK clock
                     1101b - No clock
                     1110b - No clock
                     1111b - No clock





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    524 / 3791

<!-- page 525 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

17.4.1.23 CLKOUT Clock Source Select (CLKOUTSEL)

Offset


 Register                   Offset

 CLKOUTSEL           288h


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

   W                                                          Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                                                                SEL
   W                                           Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       1      1      1      1


Fields


       Field       Function
      31-4      Reserved
    —

       3-0        Selects the CLKOUT clock source.
     SEL            0000b - Main clock (main_clk)
                     0001b - PLL0 clock (pll0_clk)
                     0010b - CLKIN clock (clk_in)
                     0011b - FRO_HF clock (fro_hf)
                     0100b - FRO 12 MHz clock (fro_12m)
                     0101b - PLL1_clk0 clock (pll1_clk)
                     0110b - LP Oscillator clock (lp_osc)
                     0111b - USB PLL clock (usb_pll_clk)
                     1000b - No clock
                     1001b - No clock
                     1010b - No clock
                     1011b - No clock
                     1100b - No clock

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    525 / 3791

<!-- page 526 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1101b - No clock
                     1110b - No clock
                     1111b - No clock


17.4.1.24 ADC0 Clock Source Select (ADC0CLKSEL)

Offset


 Register                   Offset

 ADC0CLKSEL         2A4h


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

   W                                                          Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                                                                    SEL
   W                                               Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the ADC0 clock source.
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - FRO_HF clock
                     011b - FRO 12 MHz clock
                     100b - Clk_in
                     101b - PLL1_clk0 clock
                     110b - USB PLL clock
                     111b - No clock


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    526 / 3791

<!-- page 527 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

17.4.1.25 USB-FS Clock Source Select (USB0CLKSEL)

Offset


 Register                   Offset

 USB0CLKSEL         2A8h


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

   W                                                          Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                                                                    SEL
   W                                               Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the USB-FS clock source.
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - No clock
                     011b - Clk 48 MHz clock
                     100b - Clk_in
                     101b - PLL1_clk0 clock
                     110b - USB PLL clock
                     111b - No clock


17.4.1.26 LP_FLEXCOMM Clock Source Select for Fractional Rate Divider (FCCLKSEL0 - FCCLKSEL9)

Offset
For n = 0 to 9:





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    527 / 3791

<!-- page 528 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


 Register                   Offset

 FCCLKSELn           2B0h + (n × 4h)


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

   W                                                          Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                                                                    SEL
   W                                               Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the LP_FLEXCOMM clock source for Fractional Rate Divider.
     SEL            000b - No clock
                     001b - PLL divided clock
                     010b - FRO 12 MHz clock
                     011b - fro_hf_div clock
                     100b - clk_1m clock
                     101b - USB PLL clock
                     110b - LP Oscillator clock
                     111b - No clock


17.4.1.27 SCTimer/PWM Clock Source Select (SCTCLKSEL)

Offset


 Register                   Offset

 SCTCLKSEL           2F0h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    528 / 3791

<!-- page 529 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

   W                                                          Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                                                                SEL
   W                                           Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       1      1      1      1


Fields


       Field       Function
      31-4      Reserved
    —

       3-0        Selects the SCTimer/PWM clock source.
     SEL            0000b - No clock
                     0001b - PLL0 clock
                     0010b - CLKIN clock
                     0011b - FRO_HF clock
                     0100b - PLL1_clk0 clock
                     0101b - SAI0 MCLK_IN clock
                     0110b - USB PLL clock
                     0111b - No clock
                     1000b - SAI1 MCLK_IN clock
                     1001b - No clock
                     1010b - No clock
                     1011b - No clock
                     1100b - No clock
                     1101b - No clock
                     1110b - No clock
                     1111b - No clock





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    529 / 3791

<!-- page 530 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

17.4.1.28 CPU0 System Tick Timer Divider (SYSTICKCLKDIV0)

Offset


 Register                   Offset

 SYSTICKCLKDIV0      300h


Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset.

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV      The divider value = (DIV + 1)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    530 / 3791

<!-- page 531 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

17.4.1.29 CPU1 System Tick Timer Divider (SYSTICKCLKDIV1)

Offset


 Register                   Offset

 SYSTICKCLKDIV1      304h


Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV      The divider value = (DIV + 1)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    531 / 3791

<!-- page 532 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

17.4.1.30 TRACE Clock Divider (TRACECLKDIV)

Offset


 Register                   Offset

 TRACECLKDIV         308h


Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV      The divider value = (DIV + 1)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    532 / 3791

<!-- page 533 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

17.4.1.31  TSI Function Clock Source Select (TSICLKSEL)

Offset


 Register                   Offset

 TSICLKSEL            350h


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

   W                                                          Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                                                                    SEL
   W                                               Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the TSI function clock source.
     SEL            000b - No clock
                     001b - No clock
                     010b - clk_in
                     011b - No clock
                     100b - FRO_12Mhz clock
                     101b - No clock
                     110b - No clock
                     111b - No clock


17.4.1.32 SINC FILTER Function Clock Source Select (SINCFILTCLKSEL)

Offset


 Register                   Offset

 SINCFILTCLKSEL       360h



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    533 / 3791

<!-- page 534 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

   W                                                          Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                                                                    SEL
   W                                               Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the SINC FILTER function clock source.
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - clk_in
                     011b - FRO_HF clock
                     100b - FRO_12Mhz clock
                     101b - PLL1_clk0 clock
                     110b - USB PLL clock
                     111b - No clock


17.4.1.33 SLOW_CLK Clock Divider (SLOWCLKDIV)

Offset


 Register                   Offset

 SLOWCLKDIV          378h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    534 / 3791

<!-- page 535 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       0       0      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                                 Reserved
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-0      Reserved
    —


17.4.1.34  TSI Function Clock Divider (TSICLKDIV)

Offset


 Register                   Offset

 TSICLKDIV           37Ch





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    535 / 3791

<!-- page 536 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value:
      DIV             • 0 - Divide by 1
                           • 1 - Divide by 2
                           •  ...
                           • value - Divide by (DIV+1)


17.4.1.35  System Clock Divider (AHBCLKDIV)

Offset


 Register                   Offset

 AHBCLKDIV           380h




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    536 / 3791

<!-- page 537 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R
        AB                                                                     Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30-8      Reserved
    —

       7-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.36 CLKOUT Clock Divider (CLKOUTDIV)

Offset


 Register                   Offset

 CLKOUTDIV           384h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    537 / 3791

<!-- page 538 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.37 FRO_HF_DIV Clock Divider (FROHFDIV)

Offset


 Register                   Offset

 FROHFDIV            388h


Function
This register is used to generate fro_hf_div clock from fro_hf clock.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    538 / 3791

<!-- page 539 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R         HALT
        AB                                                                         Reserved
   W

Reset     0       1      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running, this bit is set to 0 when the register is written.
                     1b - Divider clock is stopped

      29-8      Reserved
    —

       7-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.38 WDT0 Clock Divider (WDT0CLKDIV)

Offset


 Register                   Offset

 WDT0CLKDIV         38Ch





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    539 / 3791

<!-- page 540 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      DIV
   W

Reset     u      u      u      u       u      u      u      u       u      u      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-6      Reserved
    —

       5-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.39 ADC0 Clock Divider (ADC0CLKDIV)

Offset


 Register                   Offset

 ADC0CLKDIV          394h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    540 / 3791

<!-- page 541 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                                      DIV
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-3      Reserved
    —

       2-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.40 USB-FS Clock Divider (USB0CLKDIV)

Offset


 Register                   Offset

 USB0CLKDIV          398h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    541 / 3791

<!-- page 542 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.41 SCT/PWM Clock Divider (SCTCLKDIV)

Offset


 Register                   Offset

 SCTCLKDIV           3B4h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    542 / 3791

<!-- page 543 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.42 PLL Clock Divider (PLLCLKDIV)

Offset


 Register                   Offset

 PLLCLKDIV           3C4h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    543 / 3791

<!-- page 544 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.43  CTimer Clock Divider (CTIMER0CLKDIV - CTIMER4CLKDIV)

Offset


 Register                   Offset

 CTIMER0CLKDIV       3D0h

 CTIMER1CLKDIV       3D4h

 CTIMER2CLKDIV       3D8h

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    544 / 3791

<!-- page 545 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

 Register                   Offset

 CTIMER3CLKDIV      3DCh

 CTIMER4CLKDIV       3E0h


Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R  UNST        RESE
              HALT                                                       Reserved
   W   AB            T

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Stable divider clock
                     1b - Unstable clock frequency

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock has stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV      The divider value = (DIV + 1)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    545 / 3791

<!-- page 546 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

17.4.1.44 PLL1 Clock 0 Divider (PLL1CLK0DIV)

Offset


 Register                   Offset

 PLL1CLK0DIV         3E4h


Function
This register is used to generate pll1_clk0 clock from pll1_clk clock.

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV      The divider value = (DIV + 1)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    546 / 3791

<!-- page 547 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

17.4.1.45 PLL1 Clock 1 Divider (PLL1CLK1DIV)

Offset


 Register                   Offset

 PLL1CLK1DIV         3E8h


Function
This register is used to generate pll1_clk1 clock from pll1_clk clock.

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV      The divider value = (DIV + 1)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    547 / 3791

<!-- page 548 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

17.4.1.46  Clock Configuration Unlock (CLKUNLOCK)

Offset


 Register                   Offset

 CLKUNLOCK          3FCh


Function
This register controls access to the clock select and divider configuration registers.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                                                          UNLO
                                                             Reserved
   W                                                                                           CK

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-1      Reserved
    —

       0         Controls clock configuration registers access (for example, xxxDIV, xxxSEL)
   UNLOCK          0b - Updates are allowed to all clock configuration registers
                     1b - Freezes all clock configuration registers update


17.4.1.47 NVM Control (NVM_CTRL)

Offset


 Register                   Offset

 NVM_CTRL            400h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    548 / 3791

<!-- page 549 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17      16

    R                                                                                                  DIS_M  DIS_M
                                                         Reserved
   W                                                                                                                                        BE...     BE...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      1       0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2       1       0

    R                                 FLASH  Reserv                     CLR_F  DIS_F   DIS_F   DIS_F  DIS_D  DIS_F
                      Reserved                                      Reserved
   W                                                       _S...     ed                                    LA...     LA...     LA...     LA...     AT...     LA...

Reset     0      0      0      0       0      1       0      0       0      0      0      1       0      0       0       0


Fields


       Field       Function
     31-18      Reserved
    —

      17       Bus error on data multi-bit ECC error control
 DIS_MBECC_E       0b - Enables bus error on multi-bit ECC error for data
   RR_DATA         1b - Disables bus error on multi-bit ECC error for data

      16       Bus error on instruction multi-bit ECC error control
 DIS_MBECC_E       0b - Enables bus error on multi-bit ECC error for instruction
   RR_INST          1b - Disables bus error on multi-bit ECC error for instruction

     15-11      Reserved
    —

      10      FLASH stall on busy control
 FLASH_STALL       0b - No stall on FLASH busy
     _EN            1b - Stall on FLASH busy

       9        Reserved Keep the default value.
    —

       8-6       Reserved
    —

       5         Clear flash cache control
 CLR_FLASH_C       0b - No clear flash cache
    ACHE           1b - Clears flash cache

       4        Flash data cache control

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    549 / 3791

<!-- page 550 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

 DIS_FLASH_D        0b - Enables flash data cache when DIS_FLASH_CACHE=0
     ATA                     1b - Disables flash data cache

       3        Flash instruction cache control
 DIS_FLASH_IN       0b - Enables flash instruction cache when DIS_FLASH_CACHE=0
     ST            1b - Disables flash instruction cache

       2        Flash cache control
 DIS_FLASH_C        0b - Enables flash cache
    ACHE           1b - Disables flash cache

       1        Flash data speculation control
 DIS_DATA_SP                                                 NOTE
     EC                                                         If DIS_MBECC_ERR_DATA and/or DIS_MBECC_ERR_INST are set, then speculation will
                                  not be enabled, even if this bit is cleared.

                     0b - Enables data speculation
                     1b - Disables data speculation

       0        Flash speculation control
 DIS_FLASH_SP                                                 NOTE
     EC                                                         If DIS_MBECC_ERR_DATA and/or DIS_MBECC_ERR_INST are set, then speculation will
                                  not be enabled, even if this bit is cleared.

                     0b - Enables flash speculation
                     1b - Disables flash speculation


17.4.1.48 ROM Wait State (ROMCR)

Offset


 Register                   Offset

 ROMCR               404h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    550 / 3791

<!-- page 551 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                                                         ROM_
                                                             Reserved
   W                                                                                                     WAIT

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-1      Reserved
    —

       0     ROM waiting Arm core and other masters for one cycle
  ROM_WAIT         0b - Disabled
                     1b - Enabled


17.4.1.49 SmartDMA Interrupt Hijack (SmartDMAINT)

Offset


 Register                   Offset

 SmartDMAINT          414h


Function
Bits directly control the SmartDMA hijacking the system interrupts.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22      21     20      19     18      17      16

    R
                                                                    INT23   INT22   INT21   INT20   INT19   INT18   INT17   INT16
   W                           Reserved

Reset     u      u      u      u       u      u      u      u       0      0       0      0       0      0       0       0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
        INT15   INT14   INT13   INT12   INT11   INT10   INT9    INT8    INT7    INT6    INT5    INT4    INT3    INT2    INT1    INT0
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    551 / 3791

<!-- page 552 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Fields


       Field       Function
     31-24      Reserved
    —

      23      SmartDMA hijack NVIC IRQ77
     INT23           0b - Disable
                     1b - Enable

      22      SmartDMA hijack NVIC IRQ67
     INT22           0b - Disable
                     1b - Enable

      21      SmartDMA hijack NVIC IRQ66
     INT21           0b - Disable
                     1b - Enable

      20      SmartDMA hijack NVIC IRQ51
     INT20           0b - Disable
                     1b - Enable

      19      SmartDMA hijack NVIC IRQ50
     INT19           0b - Disable
                     1b - Enable

      18      SmartDMA hijack NVIC IRQ47
     INT18           0b - Disable
                     1b - Enable

      17      SmartDMA hijack NVIC IRQ45
     INT17           0b - Disable
                     1b - Enable

      16      SmartDMA hijack NVIC IRQ42
     INT16           0b - Disable
                     1b - Enable

      15      SmartDMA hijack NVIC IRQ41
     INT15           0b - Disable
                     1b - Enable

      14      SmartDMA hijack NVIC IRQ40
     INT14           0b - Disable
                     1b - Enable

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    552 / 3791

<!-- page 553 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      13      SmartDMA hijack NVIC IRQ39
     INT13           0b - Disable
                     1b - Enable

      12      SmartDMA hijack NVIC IRQ38
     INT12           0b - Disable
                     1b - Enable

      11      SmartDMA hijack NVIC IRQ37
     INT11           0b - Disable
                     1b - Enable

      10      SmartDMA hijack NVIC IRQ36
     INT10           0b - Disable
                     1b - Enable

       9       SmartDMA hijack NVIC IRQ35
     INT9            0b - Disable
                     1b - Enable

       8       SmartDMA hijack NVIC IRQ34
     INT8            0b - Disable
                     1b - Enable

       7       SmartDMA hijack NVIC IRQ33
     INT7            0b - Disable
                     1b - Enable

       6       SmartDMA hijack NVIC IRQ32
     INT6            0b - Disable
                     1b - Enable

       5       SmartDMA hijack NVIC IRQ31
     INT5            0b - Disable
                     1b - Enable

       4       SmartDMA hijack NVIC IRQ30
     INT4            0b - Disable
                     1b - Enable

       3       SmartDMA hijack NVIC IRQ29
     INT3

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    553 / 3791

<!-- page 554 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Disable
                     1b - Enable

       2       SmartDMA hijack NVIC IRQ18
     INT2            0b - Disable
                     1b - Enable

       1       SmartDMA hijack NVIC IRQ17
     INT1            0b - Disable
                     1b - Enable

       0       SmartDMA hijack NVIC IRQ1
     INT0            0b - Disable
                     1b - Enable


17.4.1.50 ADC1 Clock Source Select (ADC1CLKSEL)

Offset


 Register                   Offset

 ADC1CLKSEL          464h


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

   W                                                          Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                                                                    SEL
   W                                               Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      1      1      1


Fields


       Field       Function
      31-3      Reserved

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    554 / 3791

<!-- page 555 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

    —

       2-0        Selects the ADC1 clock source
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - FRO_HF clock
                     011b - FRO 12 MHz clock
                     100b - Clk_in clock
                     101b - PLL1_clk0 clock
                     110b - USB PLL clock
                     111b - No clock


17.4.1.51 ADC1 Clock Divider (ADC1CLKDIV)

Offset


 Register                   Offset

 ADC1CLKDIV          468h


Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                                      DIV
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    555 / 3791

<!-- page 556 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-3      Reserved
    —

       2-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.52 DACn Functional Clock Selection (DAC0CLKSEL - DAC2CLKSEL)

Offset


 Register                   Offset

 DAC0CLKSEL          490h

 DAC1CLKSEL          498h

 DAC2CLKSEL         4A0h


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    556 / 3791

<!-- page 557 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the DAC clock source
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - Clk_in
                     011b - FRO_HF
                     100b - FRO_12M
                     101b - PLL1_clk0 clock
                     110b - No clock
                     111b - No clock


17.4.1.53 DACn functional clock divider (DAC0CLKDIV - DAC2CLKDIV)

Offset


 Register                   Offset

 DAC0CLKDIV          494h

 DAC1CLKDIV          49Ch

 DAC2CLKDIV          4A4h


Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    557 / 3791

<!-- page 558 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-3      Reserved
    —

       2-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.54  FlexSPI Clock Selection (FlexSPICLKSEL)

Offset


 Register                   Offset

 FlexSPICLKSEL        4A8h


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                  Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       1      1      1      1





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    558 / 3791

<!-- page 559 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Fields


       Field       Function
      31-4      Reserved
    —

       3-0        Selects the FlexSPI clock
     SEL            0000b - No clock
                     0001b - PLL0 clock
                     0010b - No clock
                     0011b - FRO_HF
                     0100b - No clock
                     0101b - pll1_clock
                     0110b - USB PLL clock
                     0111b - No clock
                     1000b - No clock
                     1001b - No clock
                     1010b - No clock
                     1011b - No clock
                     1100b - No clock
                     1101b - No clock
                     1110b - No clock
                     1111b - No clock


17.4.1.55  FlexSPI Clock Divider (FlexSPICLKDIV)

Offset


 Register                   Offset

 FlexSPICLKDIV        4ACh





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    559 / 3791

<!-- page 560 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-3      Reserved
    —

       2-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.56 PLL Clock Divider Clock Selection (PLLCLKDIVSEL)

Offset


 Register                   Offset

 PLLCLKDIVSEL        52Ch





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    560 / 3791

<!-- page 561 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the PLL Clock Divider source clock
     SEL            000b - PLL0 clock
                     001b - pll1_clk0
                     010b - No clock
                     011b - No clock
                     100b - No clock
                     101b - No clock
                     110b - No clock
                     111b - No clock


17.4.1.57  I3C0 Functional Clock Selection (I3C0FCLKSEL)

Offset


 Register                   Offset

 I3C0FCLKSEL          530h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    561 / 3791

<!-- page 562 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the I3C0 clock
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - CLKIN clock
                     011b - FRO_HF clock
                     100b - No clock
                     101b - PLL1_clk0 clock
                     110b - USB PLL clock
                     111b - No clock


17.4.1.58  I3C0 FCLK_STC Clock Selection (I3C0FCLKSTCSEL)

Offset


 Register                   Offset

 I3C0FCLKSTCSEL      534h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    562 / 3791

<!-- page 563 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the I3C0 Time Control clock
     SEL            000b - I3C0 functional clock I3C0FCLK
                     001b - FRO_1M clock
                     010b - No clock
                     011b - No clock
                     100b - No clock
                     101b - No clock
                     110b - No clock
                     111b - No clock


17.4.1.59  I3C0 FCLK_STC Clock Divider (I3C0FCLKSTCDIV)

Offset


 Register                   Offset

 I3C0FCLKSTCDIV       538h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    563 / 3791

<!-- page 564 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.60  I3C0 FCLK Slow Clock Divider (I3C0FCLKSDIV)

Offset


 Register                   Offset

 I3C0FCLKSDIV        53Ch





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    564 / 3791

<!-- page 565 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.61  I3C0 Functional Clock FCLK Divider (I3C0FCLKDIV)

Offset


 Register                   Offset

 I3C0FCLKDIV          540h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    565 / 3791

<!-- page 566 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.62  I3C0 FCLK Slow Selection (I3C0FCLKSSEL)

Offset


 Register                   Offset

 I3C0FCLKSSEL         544h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    566 / 3791

<!-- page 567 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the I3C FCLK Slow clock
     SEL            000b - FRO_1M clock
                     001b - No clock
                     010b - No clock
                     011b - No clock
                     100b - No clock
                     101b - No clock
                     110b - No clock
                     111b - No clock


17.4.1.63  MICFIL Clock Selection (MICFILFCLKSEL)

Offset


 Register                   Offset

 MICFILFCLKSEL        548h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    567 / 3791

<!-- page 568 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                  Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       1      1      1      1


Fields


       Field       Function
      31-4      Reserved
    —

       3-0        Selects the MICFIL clock
     SEL            0000b - FRO_12M clock
                     0001b - PLL0 clock
                     0010b - CLKIN clock
                     0011b - FRO_HF clock
                     0100b - PLL1_clk0 clock
                     0101b - SAI0_MCLK clock
                     0110b - USB PLL clock
                     0111b - No clock
                     1000b - SAI1_MCLK clock
                     1001b - No clock
                     1010b - No clock
                     1011b - No clock
                     1100b - No clock
                     1101b - No clock
                     1110b - No clock
                     1111b - No clock





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    568 / 3791

<!-- page 569 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

17.4.1.64  MICFIL Clock Division (MICFILFCLKDIV)

Offset


 Register                   Offset

 MICFILFCLKDIV       54Ch


Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-3      Reserved
    —

       2-0       Clock divider value
      DIV      The divider value = (DIV + 1)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    569 / 3791

<!-- page 570 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

17.4.1.65 uSDHC Clock Selection (uSDHCCLKSEL)

Offset


 Register                   Offset

 uSDHCCLKSEL         558h


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the uSDHC clock
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - CLKIN clock
                     011b - FRO_HF clock
                     100b - FRO_12M clock
                     101b - pll1_clk1 clock
                     110b - USB PLL clock
                     111b - No clock


17.4.1.66 uSDHC Function Clock Divider (uSDHCCLKDIV)

Offset


 Register                   Offset

 uSDHCCLKDIV        55Ch




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    570 / 3791

<!-- page 571 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV             • 0 - Divide by 1
                           • 1 - Divide by 2
                           •  ...
                           • value - Divide by (DIV+1)


17.4.1.67 FLEXIO Clock Selection (FLEXIOCLKSEL)

Offset


 Register                   Offset

 FLEXIOCLKSEL        560h




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    571 / 3791

<!-- page 572 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the FLEXIO clock
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - CLKIN clock
                     011b - FRO_HF clock
                     100b - FRO_12M clock
                     101b - PLL1_clk0 clock
                     110b - USB PLL clock
                     111b - No clock


17.4.1.68 FLEXIO Function Clock Divider (FLEXIOCLKDIV)

Offset


 Register                   Offset

 FLEXIOCLKDIV         564h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    572 / 3791

<!-- page 573 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV             • 0 - Divide by 1
                           • 1 - Divide by 2
                           •  ...
                           • value - Divide by (DIV+1)


17.4.1.69 FLEXCAN0 Clock Selection (FLEXCAN0CLKSEL)

Offset


 Register                   Offset

 FLEXCAN0CLKSEL     5A0h




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    573 / 3791

<!-- page 574 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the FLEXCAN0 clock
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - CLKIN clock
                     011b - FRO_HF clock
                     100b - No clock
                     101b - PLL1_clk0 clock
                     110b - USB PLL clock
                     111b - No clock


17.4.1.70 FLEXCAN0 Function Clock Divider (FLEXCAN0CLKDIV)

Offset


 Register                   Offset

 FLEXCAN0CLKDIV     5A4h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    574 / 3791

<!-- page 575 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       0       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV             • 0 - Divide by 1
                           • 1 - Divide by 2
                           •  ...
                           • value - Divide by (DIV+1)


17.4.1.71 FLEXCAN1 Clock Selection (FLEXCAN1CLKSEL)

Offset


 Register                   Offset

 FLEXCAN1CLKSEL     5A8h




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    575 / 3791

<!-- page 576 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the FLEXCAN1 clock
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - CLKIN clock
                     011b - FRO_HF clock
                     100b - No clock
                     101b - PLL1_clk0 clock
                     110b - USB PLL clock
                     111b - No clock


17.4.1.72 FLEXCAN1 Function Clock Divider (FLEXCAN1CLKDIV)

Offset


 Register                   Offset

 FLEXCAN1CLKDIV     5ACh





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    576 / 3791

<!-- page 577 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV             • 0 - Divide by 1
                           • 1 - Divide by 2
                           •  ...
                           • value - Divide by (DIV+1)


17.4.1.73  Ethernet RMII Clock Selection (ENETRMIICLKSEL)

Offset


 Register                   Offset

 ENETRMIICLKSEL      5B0h




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    577 / 3791

<!-- page 578 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the Ethernet RMII clock
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - CLKIN clock
                     011b - No clock
                     100b - No clock
                     101b - PLL1_clk0 clock
                     110b - No clock
                     111b - No clock


17.4.1.74  Ethernet RMII Function Clock Divider (ENETRMIICLKDIV)

Offset


 Register                   Offset

 ENETRMIICLKDIV      5B4h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    578 / 3791

<!-- page 579 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV             • 0 - Divide by 1
                           • 1 - Divide by 2
                           •  ...
                           • value - Divide by (DIV+1)


17.4.1.75  Ethernet PTP REF Clock Selection (ENETPTPREFCLKSEL)

Offset


 Register                   Offset

 ENETPTPREFCLKSEL   5B8h




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    579 / 3791

<!-- page 580 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the Ethernet PTP REF clock
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - CLKIN clock
                     011b - No clock
                     100b - enet0_tx_clk clock
                     101b - pll1_clk1 clock
                     110b - No clock
                     111b - No clock


17.4.1.76  Ethernet PTP REF Function Clock Divider (ENETPTPREFCLKDIV)

Offset


 Register                   Offset

 ENETPTPREFCLKDIV   5BCh





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    580 / 3791

<!-- page 581 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV             • 0 - Divide by 1
                           • 1 - Divide by 2
                           •  ...
                           • value - Divide by (DIV+1)


17.4.1.77  Ethernet PHY Interface Select (ENET_PHY_INTF_SEL)

Offset


 Register                   Offset

 ENET_PHY_INTF_SEL   5C0h




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    581 / 3791

<!-- page 582 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                                                      PHY_
                                                     Reserved                                                          Reserved
   W                                                                                       SEL

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-3      Reserved
    —

       2         Selects the PHY interface
   PHY_SEL         0b - Selects MII PHY Interface
                     1b - Selects RMII PHY Interface

       1-0       Reserved
    —


17.4.1.78  Sideband Flow Control (ENET_SBD_FLOW_CTRL)

Offset


 Register                   Offset

 ENET_SBD_FLOW_CTR 5C4h
 L


Function
When set high, instructs the MAC to transmit Pause frames in the Full-duplex mode. In the half-duplex mode, the MAC
enables the back-pressure function until this signal is made low again.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    582 / 3791

<!-- page 583 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1       0

    R                                                                                                  SEL_   SEL_
                                                         Reserved
   W                                                                                                              ch1     ch0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0       0


Fields


       Field       Function
      31-2      Reserved
    —

       1        Sideband Flow Control for channel1
    SEL_ch1          0b - No trigger flow control
                     1b - Trigger flow control

       0        Sideband Flow Control for channel0
    SEL_ch0          0b - No trigger flow control
                     1b - Trigger flow control


17.4.1.79 EWM0 Clock Selection (EWM0CLKSEL)

Offset


 Register                   Offset

 EWM0CLKSEL         5D4h


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                             Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      1


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    583 / 3791

<!-- page 584 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Fields


       Field       Function
      31-1      Reserved
    —

       0         Selects the EWM0 clock
     SEL            0b - clk_16k[2]
                     1b - xtal32k[2]


17.4.1.80 WDT1 Clock Selection (WDT1CLKSEL)

Offset


 Register                   Offset

 WDT1CLKSEL         5D8h


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                         Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      1      1


Fields


       Field       Function
      31-2      Reserved
    —

       1-0        Selects the WDT1 clock
     SEL            00b - FRO16K clock 2
                     01b - fro_hf_div clock
                     10b - clk_1m clock
                     11b - clk_1m clock





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    584 / 3791

<!-- page 585 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

17.4.1.81 WDT1 Function Clock Divider (WDT1CLKDIV)

Offset


 Register                   Offset

 WDT1CLKDIV         5DCh


Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-6      Reserved
    —

       5-0       Clock divider value
      DIV             • 0 - Divide by 1
                           • 1 - Divide by 2
                           •  ...
                           • value - Divide by (DIV+1)




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    585 / 3791

<!-- page 586 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

17.4.1.82 OSTIMER Clock Selection (OSTIMERCLKSEL)

Offset


 Register                   Offset

 OSTIMERCLKSEL      5E0h


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                         Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      1      1


Fields


       Field       Function
      31-2      Reserved
    —

       1-0        Selects the OS Event Timer clock
     SEL            00b - clk_16k[2]
                     01b - xtal32k[2]
                     10b - clk_1m clock
                     11b - No clock


17.4.1.83 CMP0 Function Clock Selection (CMP0FCLKSEL)

Offset


 Register                   Offset

 CMP0FCLKSEL        5F0h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    586 / 3791

<!-- page 587 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the CMP0 function clock
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - FRO_HF clock
                     011b - FRO_12M clock
                     100b - CLKIN clock
                     101b - PLL1_clk0 clock
                     110b - USB PLL clock
                     111b - No clock


17.4.1.84 CMP0 Function Clock Divider (CMP0FCLKDIV)

Offset


 Register                   Offset

 CMP0FCLKDIV         5F4h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    587 / 3791

<!-- page 588 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-4      Reserved
    —

       3-0       Clock divider value
      DIV             • 0 - Divide by 1
                           • 1 - Divide by 2
                           •  ...
                           • value - Divide by (DIV+1)


17.4.1.85 CMP0 Round Robin Clock Selection (CMP0RRCLKSEL)

Offset


 Register                   Offset

 CMP0RRCLKSEL       5F8h




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    588 / 3791

<!-- page 589 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the CMP0 round robin clock
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - FRO_HF clock
                     011b - FRO_12M clock
                     100b - CLKIN clock
                     101b - PLL1_clk0 clock
                     110b - USB PLL clock
                     111b - No clock


17.4.1.86 CMP0 Round Robin Clock Divider (CMP0RRCLKDIV)

Offset


 Register                   Offset

 CMP0RRCLKDIV       5FCh





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    589 / 3791

<!-- page 590 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-4      Reserved
    —

       3-0       Clock divider value
      DIV             • 0 - Divide by 1
                           • 1 - Divide by 2
                           •  ...
                           • value - Divide by (DIV+1)


17.4.1.87 CMP1 Function Clock Selection (CMP1FCLKSEL)

Offset


 Register                   Offset

 CMP1FCLKSEL         600h




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    590 / 3791

<!-- page 591 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the CMP1 function clock
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - FRO_HF clock
                     011b - FRO_12M clock
                     100b - CLKIN clock
                     101b - PLL1_clk0 clock
                     110b - USB PLL clock
                     111b - No clock


17.4.1.88 CMP1 Function Clock Divider (CMP1FCLKDIV)

Offset


 Register                   Offset

 CMP1FCLKDIV         604h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    591 / 3791

<!-- page 592 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-4      Reserved
    —

       3-0       Clock divider value
      DIV             • 0 - Divide by 1
                           • 1 - Divide by 2
                           •  ...
                           • value - Divide by (DIV+1)


17.4.1.89 CMP1 Round Robin Clock Source Select (CMP1RRCLKSEL)

Offset


 Register                   Offset

 CMP1RRCLKSEL       608h




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    592 / 3791

<!-- page 593 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the CMP1 round robin clock
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - FRO_HF clock
                     011b - FRO_12M clock
                     100b - CLKIN clock
                     101b - PLL1_clk0 clock
                     110b - USB PLL clock
                     111b - No clock


17.4.1.90 CMP1 Round Robin Clock Division (CMP1RRCLKDIV)

Offset


 Register                   Offset

 CMP1RRCLKDIV       60Ch





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    593 / 3791

<!-- page 594 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-4      Reserved
    —

       3-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.91 CMP2 Function Clock Source Select (CMP2FCLKSEL)

Offset


 Register                   Offset

 CMP2FCLKSEL         610h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    594 / 3791

<!-- page 595 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the CMP2 function clock
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - FRO_HF clock
                     011b - FRO_12M clock
                     100b - CLKIN clock
                     101b - PLL1_clk0 clock
                     110b - USB PLL clock
                     111b - No clock


17.4.1.92 CMP2 Function Clock Division (CMP2FCLKDIV)

Offset


 Register                   Offset

 CMP2FCLKDIV         614h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    595 / 3791

<!-- page 596 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-4      Reserved
    —

       3-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.93 CMP2 Round Robin Clock Source Select (CMP2RRCLKSEL)

Offset


 Register                   Offset

 CMP2RRCLKSEL       618h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    596 / 3791

<!-- page 597 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the CMP2 round robin clock
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - FRO_HF clock
                     011b - FRO_12M clock
                     100b - CLKIN clock
                     101b - PLL1_clk0 clock0
                     110b - USB PLL clock
                     111b - No clock


17.4.1.94 CMP2 Round Robin Clock Division (CMP2RRCLKDIV)

Offset


 Register                   Offset

 CMP2RRCLKDIV       61Ch





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    597 / 3791

<!-- page 598 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-4      Reserved
    —

       3-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.95 CPU Control for Multiple Processors (CPUCTRL)

Offset


 Register                   Offset

 CPUCTRL             800h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    598 / 3791

<!-- page 599 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

   W                                              PROT

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5       4       3      2      1      0

    R                                                               CPU1  Reserv  CPU1
                                          Reserved                                                                 Reserved
   W                                                                                         RST...    ed     CLK...

Reset     0      0      0      0       0      0      0      0       0      0      1       0       1      0      0      0


Fields


       Field       Function

     31-16       Write Protect
    PROT     Must be written as 0xC0C4 for the write to have an effect
                     1100_0000_1100_0100b - For write operation to have an effect.

      15-6      Reserved
    —

       5      CPU1 reset
 CPU1RSTEN   Controls the reset signal to CPU1. This bit can be cleared to release CPU1 from reset after a valid boot
                address has been written to CPBOOT.
                     0b - The CPU1 is not reset.
                     1b - The CPU1 is reset.

       4        Reserved
    —

       3        Enables the CPU1 clock
  CPU1CLKEN        0b - The CPU1 clock is not enabled
                     1b - The CPU1 clock is enabled

       2-0       Reserved
    —


17.4.1.96  Coprocessor Boot Address (CPBOOT)

Offset


 Register                   Offset

 CPBOOT              804h


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    599 / 3791

<!-- page 600 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                 CPBOOT
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                             CPBOOT                                                      Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-7      Coprocessor Boot VTOR Address [31:7] for CPU1
   CPBOOT

       6-0       Reserved
    —


17.4.1.97 CPU Status (CPUSTAT)

Offset


 Register                   Offset

 CPUSTAT            80Ch


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2       1       0

                                                                                   CPU1L  CPU0L  CPU1  CPU0
    R
                                                                                                                    OC...    OC...    SLE...   SLE...                                                 Reserved
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       0      0       0       0





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    600 / 3791

<!-- page 601 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Fields


       Field       Function
      31-4      Reserved
    —

       3      CPU1 lockup state
 CPU1LOCKUP       0b - CPU is not in lockup
                     1b - CPU is in lockup

       2      CPU0 lockup state
 CPU0LOCKUP       0b - CPU is not in lockup
                     1b - CPU is in lockup

       1      CPU1 sleeping state
 CPU1SLEEPIN       0b - CPU is not sleeping
     G             1b - CPU is sleeping

       0      CPU0 sleeping state
 CPU0SLEEPIN       0b - CPU is not sleeping
     G             1b - CPU is sleeping


17.4.1.98 LPCAC Control (LPCAC_CTRL)

Offset


 Register                   Offset

 LPCAC_CTRL          824h


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6       5      4       3      2       1       0

    R                              0                                                         LPCA  PARIT  LIM_L   DIS_L  PARIT  FRC_  CLR_L  DIS_L
                                                                                  C_X...     Y_...     PC...    PC...     Y_...    NO_...    PC...    PC...   W

Reset     0      0      0      0       0      0      0      0       0      0       1      1       0      0       0       1





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    601 / 3791

<!-- page 602 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Fields


       Field       Function
      31-8      Reserved
    —

       7      LPCAC XOM(eXecute-Only-Memory) attribute control
 LPCAC_XOM   Controls if the instruction fetch attribute is used as part of the address input to the LPCAC. When XOM
                  regions in the internal flash are not configured at the MBC, then this option should be disabled so that
                    instructions and data can be stored within the same cache line. This provides the best cache efficiency
                     for non-XOM applications. When XOM areas in the internal flash are configured at the MBC, then this bit
               must be set so that instructions and data are cached using separate lines within the LPCAC.
                     0b - Disabled.
                     1b - Enabled.

       6        Enable parity error report.
 PARITY_FAUL       0b - Disables parity error report
    T_EN           1b - Enables parity error report

       5          Limit LPCAC Write Through Buffer.
 LIM_LPCAC_W       0b - Write buffer enabled when transaction is bufferable.
     TBF            1b - Write buffer enabled when transaction is cacheable and bufferable

       4         Disable LPCAC Write Through Buffer.
 DIS_LPCAC_W       0b - Enables write through buffer
     TBF            1b - Disables write through buffer

       3        Enables parity miss.
 PARITY_MISS_       0b - Disabled
     EN            1b - Enables parity, miss on parity error

       2        Forces no allocation.
 FRC_NO_ALLO       0b - Forces allocation
     C             1b - Forces no allocation

       1        Clears the cache function.
  CLR_LPCAC        0b - Unclears the cache
                     1b - Clears the cache

       0         Disables/enables the cache function.
  DIS_LPCAC         0b - Enabled
                     1b - Disabled





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    602 / 3791

<!-- page 603 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

17.4.1.99 LP_FLEXCOMM Clock Divider (FLEXCOMM0CLKDIV - FLEXCOMM9CLKDIV)

Offset
For n = 0 to 9:


 Register                   Offset

 FLEXCOMMnCLKDIV    850h + (n × 4h)


Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV             • 0 - Divide by 1
                           • 1 - Divide by 2
                           •  ...
                           • value - Divide by (DIV+1)


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    603 / 3791

<!-- page 604 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

17.4.1.100  SAI0 Function Clock Source Select (SAI0CLKSEL)

Offset


 Register                   Offset

 SAI0CLKSEL           880h


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the clock source
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - CLKIN clock
                     011b - FRO_HF clock
                     100b - PLL1_CLK0 clock
                     101b - No clock
                     110b - USB PLL clock
                     111b - No clock


17.4.1.101  SAI1 Function Clock Source Select (SAI1CLKSEL)

Offset


 Register                   Offset

 SAI1CLKSEL           884h




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    604 / 3791

<!-- page 605 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the clock source
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - CLKIN clock
                     011b - FRO_HF clock
                     100b - PLL1_CLK0 clock
                     101b - No clock
                     110b - USB PLL clock
                     111b - No clock


17.4.1.102  SAI0 Function Clock Division (SAI0CLKDIV)

Offset


 Register                   Offset

 SAI0CLKDIV           888h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    605 / 3791

<!-- page 606 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-3      Reserved
    —

       2-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.103  SAI1 Function Clock Division (SAI1CLKDIV)

Offset


 Register                   Offset

 SAI1CLKDIV          88Ch





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    606 / 3791

<!-- page 607 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-3      Reserved
    —

       2-0       Clock divider value
      DIV      The divider value = (DIV + 1).


17.4.1.104 EMVSIM0 Clock Source Select (EMVSIM0CLKSEL)

Offset


 Register                   Offset

 EMVSIM0CLKSEL       890h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    607 / 3791

<!-- page 608 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the EMVSIM0 function clock source
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - CLKIN clock
                     011b - FRO_HF clock
                     100b - FRO_12M clock
                     101b - PLL1_clk0 clock0
                     110b - No clock
                     111b - No clock


17.4.1.105 EMVSIM1 Clock Source Select (EMVSIM1CLKSEL)

Offset


 Register                   Offset

 EMVSIM1CLKSEL       894h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    608 / 3791

<!-- page 609 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0        Selects the EMVSIM1 function clock source
     SEL            000b - No clock
                     001b - PLL0 clock
                     010b - CLKIN clock
                     011b - FRO_HF clock
                     100b - FRO_12M clock
                     101b - PLL1_clk0 clock0
                     110b - No clock
                     111b - No clock


17.4.1.106 EMVSIM0 Function Clock Division (EMVSIM0CLKDIV)

Offset


 Register                   Offset

 EMVSIM0CLKDIV       898h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    609 / 3791

<!-- page 610 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-3      Reserved
    —

       2-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.107 EMVSIM1 Function Clock Division (EMVSIM1CLKDIV)

Offset


 Register                   Offset

 EMVSIM1CLKDIV      89Ch





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    610 / 3791

<!-- page 611 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-3      Reserved
    —

       2-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.108  Clock Control (CLOCK_CTRL)

Offset


 Register                   Offset

 CLOCK_CTRL         A18h


Function
Various system clocks enable controls





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    611 / 3791

<!-- page 612 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6       5      4       3      2       1       0

    R                                        PLU_D              FRO1  CLKIN  FRO_  FRO12  FRO1  CLKIN  Reserv
                          Reserved                              Reserved
   W                                                        EG...                   MHZ...    _E...    HF_...   MH...   MHZ...    _E...     ed

Reset     u      u      u      u       u      u      0      0       1      0       0      0       0      0       0       1


Fields


       Field       Function
     31-10      Reserved
    —

       9        Enables clocks FRO_1MHz and FRO_12MHz for PLU deglitching.
 PLU_DEGLITC       0b - Clock is not enabled
  H_CLK_ENA        1b - Clock is enabled

       8-7       Reserved
    —

       6        Enables FRO_1MHz clock for clock muxing in clock gen
 FRO1MHZ_CL        0b - Clock is not enabled
    K_ENA           1b - Clock is enabled

       5        Enables clk_in clock for MICFIL, EMVSIM0/1, CAN0/1, I3C0/1, SAI0/1, SINC Filter (SINC), TSI, USBFS,
               SCT, uSDHC, clkout.
  CLKIN_ENA
                     0b - Clock is not enabled
                     1b - Clock is enabled

       4        Enables FRO HF clock for the Frequency Measure module
 FRO_HF_ENA        0b - Clock is not enabled
                     1b - Clock is enabled

       3        Enables the FRO_12MHz clock for the Flash, LPTMR0/1, and Frequency Measurement modules
 FRO12MHZ_E        0b - Clock is not enabled
     NA            1b - Clock is enabled

       2        Enables the FRO_1MHz clock for RTC module and for UTICK
                     0b - Clock is not enabled

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    612 / 3791

<!-- page 613 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

 FRO1MHZ_EN       1b - Clock is enabled
      A

       1        Enables the clk_in clock for the Frequency Measurement, USB HS and LPTMR0/1 modules.
 CLKIN_ENA_F        0b - Clock is not enabled
 M_USBH_LPT        1b - Clock is enabled

       0        Reserved
    —


17.4.1.109  I3C1 Functional Clock Selection (I3C1FCLKSEL)

Offset


 Register                   Offset

 I3C1FCLKSEL         B30h


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0       I3C1 clock select
     SEL            000b - No clock
                     001b - PLL0 clock

                                  Tablecontinuesonthenextpage...




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    613 / 3791

<!-- page 614 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     010b - CLKIN clock
                     011b - FRO_HF clock
                     100b - No clock
                     101b - PLL1_clk0 clock
                     110b - USB PLL clock
                     111b - No clock


17.4.1.110  Selects the I3C1 Time Control clock (I3C1FCLKSTCSEL)

Offset


 Register                   Offset

 I3C1FCLKSTCSEL      B34h


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0       I3C1 FCLK_STC clock select
     SEL            000b - I3C1 functional clock I3C1FCLK
                     001b - FRO_1M clock
                     010b - No clock

                                  Tablecontinuesonthenextpage...




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    614 / 3791

<!-- page 615 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     011b - No clock
                     100b - No clock
                     101b - No clock
                     110b - No clock
                     111b - No clock


17.4.1.111  I3C1 FCLK_STC Clock Divider (I3C1FCLKSTCDIV)

Offset


 Register                   Offset

 I3C1FCLKSTCDIV      B38h


Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

      29       Resets the divider counter

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    615 / 3791

<!-- page 616 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.112  I3C1 FCLK Slow clock Divider (I3C1FCLKSDIV)

Offset


 Register                   Offset

 I3C1FCLKSDIV        B3Ch


Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running
                     1b - Divider clock is stopped

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    616 / 3791

<!-- page 617 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.113  I3C1 Functional Clock FCLK Divider (I3C1FCLKDIV)

Offset


 Register                   Offset

 I3C1FCLKDIV          B40h


Diagram

 Bits       31      30      29     28      27     26     25     24      23     22     21     20      19     18     17     16

       UNST
    R        AB         RESE              HALT                                                       Reserved
                       T
   W

Reset     0       1       0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                      DIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Divider status flag
   UNSTAB          0b - Divider clock is stable
                     1b - Clock frequency is not stable

      30        Halts the divider counter
    HALT           0b - Divider clock is running

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    617 / 3791

<!-- page 618 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1b - Divider clock is stopped

      29       Resets the divider counter
    RESET          0b - Divider is not reset
                     1b - Divider is reset

      28-8      Reserved
    —

       7-0       Clock divider value
      DIV      The divider value = (DIV + 1)


17.4.1.114  I3C1 FCLK Slow Selection (I3C1FCLKSSEL)

Offset


 Register                   Offset

 I3C1FCLKSSEL        B44h


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                               SEL
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-3      Reserved
    —

       2-0       I3C1 FCLK Slow Clock Select
     SEL            000b - FRO_1M clock

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    618 / 3791

<!-- page 619 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     001b - No clock
                     010b - No clock
                     011b - No clock
                     100b - No clock
                     101b - No clock
                     110b - No clock
                     111b - No clock


17.4.1.115 ETB Counter Status Register (ETB_STATUS)

Offset


 Register                   Offset

 ETB_STATUS          B50h


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                          Reserved

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2       1       0

                                                                                 DBG_                  Reserv
    R                                           Reserved
                                                                                                                    HAL...                   ed                                                                                                NMI    IRQ
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0       0       0


Fields


       Field       Function
      31-4      Reserved
    —

       3       Debug halt request
 DBG_HALT_RE       0b - The debug halt request signal is not asserted
     Q             1b - The debug halt request signal is asserted when the ETB count expires

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    619 / 3791

<!-- page 620 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       2      ETB NMI
     NMI            0b - ETB NMI is not asserted
                     1b - ETB NMI is asserted. Write 1 to clear it.

       1      ETB Interrupt
     IRQ            0b - ETB interrupt is not asserted
                     1b - ETB interrupt is asserted when ETB count expires. Write 1 to clear it.

       0        Reserved
    —


17.4.1.116 ETB Counter Control Register (ETB_COUNTER_CTRL)

Offset


 Register                   Offset

 ETB_COUNTER_CTRL   B54h


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                          Reserved

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                           Reserved                                                                                                  CNTE
                                                                              RLRQ     RSPT
                                                                                             N   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-4      Reserved
    —

       3        Reload request
    RLRQ      Reloads the ETB packet counter with the ETB_COUNTER_RELOAD value. It is always zero when read.
                              If IRQ or NMI interrupts were enabled and an NMI or IRQ interrupt was generated on counter expiration,

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    620 / 3791

<!-- page 621 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                    setting this bit clears the pending NMI or IRQ interrupt request. If debug halt was enabled and a debug
                     halt request was asserted on counter expiration, setting this bit clears the debug halt request.
                     0b - No effect
                     1b - Clears pending debug halt, NMI, or IRQ interrupt requests

       2-1      Response Type
    RSPT           00b - No response when the ETB count expires
                     01b - Generates a normal interrupt when the ETB count expires
                     10b - Generates an NMI interrupt when the ETB count expires
                     11b - Generates a debug halt when the ETB count expires via CPU0 CTICHIN[2]

       0        Enables the ETB counter
   CNTEN          0b - ETB counter is disabled
                     1b - ETB counter is enabled


17.4.1.117 ETB Counter Reload Register (ETB_COUNTER_RELOAD)

Offset


 Register                   Offset

 ETB_COUNTER_RELO  B58h
 AD


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                       Reserved                                          RELOAD
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-11      Reserved

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    621 / 3791

<!-- page 622 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

    —

      10-0       Byte count reload value
   RELOAD     Indicates the 0-mod-4 value that the counter reloads to. Writing a non-0-mod-4 value to this field results
                     in a bus error.

17.4.1.118 ETB Counter Value Register (ETB_COUNTER_VALUE)

Offset


 Register                   Offset

 ETB_COUNTER_VALUE B5Ch


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                           COUNTER_VALUE
                       Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-11      Reserved
    —

      10-0       Byte count counter value
 COUNTER_VA  Indicates the current 0-mod-4 value of the counter
     LUE





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    622 / 3791

<!-- page 623 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

17.4.1.119  Gray to Binary Converter Gray code_gray[31:0] (GRAY_CODE_LSB)

Offset


 Register                   Offset

 GRAY_CODE_LSB      B60h


Function
The Gray Code LSB Input register (CODE_GRAY_LSB) contains the least-significant portion of the Gray code to be converted
back to binary.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                code_gray_31_0
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                                code_gray_31_0
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function

      31-0      Gray code [31:0]
 code_gray_31_ CODE_GRAY_LSB is the least-significant 32 bits of the 42-bit Gray code to be converted.
       0


17.4.1.120  Gray to Binary Converter Gray code_gray[41:32] (GRAY_CODE_MSB)

Offset


 Register                   Offset

 GRAY_CODE_MSB     B64h


Function
The Gray Code MSB Input register (CODE_GRAY_MSB) contains the most-significant portion of the Gray code to be
converted back to binary.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    623 / 3791

<!-- page 624 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                          Reserved

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                    Reserved
                                                                                       code_gray_41_32
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-10      Reserved
    —

       9-0       Gray code [41:32]
 code_gray_41_ CODE_GRAY_MSB is the most-significant 10 bits of the 42-bit Gray code to be converted.
      32


17.4.1.121  Gray to Binary Converter Binary Code [31:0] (BINARY_CODE_LSB)

Offset


 Register                   Offset

 BINARY_CODE_LSB    B68h


Function
The Binary Code LSB register (BINARY_CODE_LSB) contains the least-significant portion of the code converted from Gray to
binary coding.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                           code_bin_31_0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                           code_bin_31_0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    624 / 3791

<!-- page 625 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Fields


       Field       Function

      31-0       Binary code [31:0]
 code_bin_31_0  code_bin_31_0 is the least-significant 32 bits of the 42-bit converted code.


17.4.1.122  Gray to Binary Converter Binary Code [41:32] (BINARY_CODE_MSB)

Offset


 Register                   Offset

 BINARY_CODE_MSB    B6Ch


Function
The Binary Code MSB register (BINARY_CODE_MSB) contains the most-significant portion of the code converted from Gray
to binary coding.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                          Reserved

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                    Reserved                                                     code_bin_41_32

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-10      Reserved
    —

       9-0        Binary code [41:32]
 code_bin_41_32 code_bin_41_32 is the most-significant 10 bits of the 42-bit converted code.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    625 / 3791

<!-- page 626 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

17.4.1.123  Control Automatic Clock Gating (AUTOCLKGATEOVERRIDE)

Offset


 Register                   Offset

 AUTOCLKGATEOVERR  E04h
 IDE


Function
This register allows selectively disabling automatic clock gating for device SRAMs. By default, automatic clock gating turns off
clocks to each internal SRAM after 16 bus clocks with no activity. This saves power when the SRAMs are not used for a period
of time. When turned off due to automatic clock gating, there is a 1 clock delay for the next access to an SRAM. Automatic
clock gating may be disabled for time-critical code, which may typically give a 1 or 2% speed improvement.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6       5      4       3      2       1       0

    R                                        RAMH  RAMG  RAMF  RAME  RAMD  RAMC  RAMB  Reserv  Reserv
                              Reserved
   W                                                                   _CT...   _CT...   _CT...   _CT...   _CT...   _CT...   _CT...    ed      ed

Reset     1      1      1      1       1      1      1      1       1      1       1      1       1      1       1       1


Fields


       Field       Function
      31-9      Reserved
    —

       8         Controls automatic clock gating for the RAMG Controller
 RAMH_CTRL        0b - Automatic clock gating is not overridden
                     1b - Automatic clock gating is overridden (Automatic clock gating is disabled).

       7         Controls automatic clock gating for the RAMG Controller
 RAMG_CTRL        0b - Automatic clock gating is not overridden
                     1b - Automatic clock gating is overridden (Automatic clock gating is disabled).

       6         Controls automatic clock gating for the RAMF Controller
  RAMF_CTRL        0b - Automatic clock gating is not overridden
                     1b - Automatic clock gating is overridden (Automatic clock gating is disabled).

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    626 / 3791

<!-- page 627 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       5         Controls automatic clock gating for the RAMD Controller.
  RAME_CTRL        0b - Automatic clock gating is not overridden
                     1b - Automatic clock gating is overridden (Automatic clock gating is disabled).

       4         Controls automatic clock gating for the RAMD Controller
 RAMD_CTRL        0b - Automatic clock gating is not overridden
                     1b - Automatic clock gating is overridden (Automatic clock gating is disabled).

       3         Controls automatic clock gating for the RAMC Controller
 RAMC_CTRL        0b - Automatic clock gating is not overridden
                     1b - Automatic clock gating is overridden (Automatic clock gating is disabled).

       2         Controls automatic clock gating for the RAMB Controller
  RAMB_CTRL        0b - Automatic clock gating is not overridden
                     1b - Automatic clock gating is overridden (Automatic clock gating is disabled).

       1        Reserved Keep the default value.
    —

       0        Reserved
    —


17.4.1.124  Control Automatic Clock Gating C (AUTOCLKGATEOVERRIDEC)

Offset


 Register                   Offset

 AUTOCLKGATEOVERR  E2Ch
 IDEC


Function
This register allows selectively disabling automatic clock gating for device SRAMs. By default, automatic clock gating turns off
clocks to each internal SRAM after 16 bus clocks with no activity. This saves power when the SRAMs are not used for a period
of time. When turned off due to automatic clock gating, there is a 1 clock delay for the next access to an SRAM. Automatic
clock gating may be disabled for time-critical code, which may typically give a 1 or 2% speed improvement.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    627 / 3791

<!-- page 628 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits       31      30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
      RAMA  RAMX                                                   Reserved
   W

Reset     0       0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31        Controls automatic clock gating of the RAMA controller
    RAMA           0b - Automatic clock gating is not overridden
                     1b - Automatic clock gating is overridden (Automatic clock gating is disabled).

      30        Controls automatic clock gating of the RAMX controller
    RAMX           0b - Automatic clock gating is not overridden
                     1b - Automatic clock gating is overridden (Automatic clock gating is disabled).

      29-0      Reserved
    —


17.4.1.125 PWM0 Submodule Control (PWM0SUBCTL)

Offset


 Register                   Offset

 PWM0SUBCTL         E38h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    628 / 3791

<!-- page 629 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

   W                                                            0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15      14      13     12      11     10      9      8       7      6      5      4       3      2       1       0

    R      DMAV  DMAV  DMAV  DMAV                                                        CLK3_  CLK2_  CLK1_  CLK0_
       ALM3  ALM2  ALM1  ALM0                                                  EN    EN    EN    EN   W                                                            0

Reset     0       0       0      0       0      0      0      0       0      0      0      0       0      0       0       0


Fields


       Field       Function
     31-16      Reserved
    —

     15-12    PWM0 submodule n DMA compare value done mask
  DMAVALMn  When the mask is used, DMA should write the PWM's MCTRL register in the end to set the LDOK bits
                    of relevant submodules at the same time. Since the DMA done signal is blocked, the DMA request is not
                  cleared automatically by the hardware. After the DMA transfer is completed for the current DMA request,
                  the DMA request needs to be cleared manually by clearing SMxDMAEN[VALDE] followed by clearing
              SMxSTS[RF] with the software.
      11-4      Reserved
    —

       3-0       Enables PWM0 SUB Clockn
   CLKn_EN


17.4.1.126 PWM1 Submodule Control (PWM1SUBCTL)

Offset


 Register                   Offset

 PWM1SUBCTL        E3Ch





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    629 / 3791

<!-- page 630 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

   W                                                            0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15      14      13     12      11     10      9      8       7      6      5      4       3      2       1       0

    R      DMAV  DMAV  DMAV  DMAV                                                        CLK3_  CLK2_  CLK1_  CLK0_
       ALM3  ALM2  ALM1  ALM0                                                  EN    EN    EN    EN   W                                                            0

Reset     0       0       0      0       0      0      0      0       0      0      0      0       0      0       0       0


Fields


       Field       Function
     31-16      Reserved
    —

     15-12    PWM1 submodule n DMA compare value done mask
  DMAVALMn  When the mask is used, DMA should write the PWM's MCTRL register in the end to set the LDOK bits
                    of relevant submodules at the same time. Since the DMA done signal is blocked, the DMA request is not
                  cleared automatically by the hardware. After the DMA transfer is completed for the current DMA request,
                  the DMA request needs to be cleared manually by clearing SMxDMAEN[VALDE] followed by clearing
              SMxSTS[RF] with the software.
      11-4      Reserved
    —

       3-0       Enables PWM1 SUB Clockn
   CLKn_EN


17.4.1.127 CTIMER Global Start Enable (CTIMERGLOBALSTARTEN)

Offset


 Register                   Offset

 CTIMERGLOBALSTART E40h
 EN





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    630 / 3791

<!-- page 631 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2       1       0

    R                                                                          CTIME  CTIME  CTIME  CTIME  CTIME
                                             Reserved
   W                                                                                                              R4...     R3...     R2...     R1...     R0...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0       0       0


Fields


       Field       Function
      31-5      Reserved
    —

       4        Enables the CTIMER4 function clock
 CTIMER4_CLK       0b - Disable
     _EN            1b - Enable

       3        Enables the CTIMER3 function clock
 CTIMER3_CLK       0b - Disable
     _EN            1b - Enable

       2        Enables the CTIMER2 function clock
 CTIMER2_CLK       0b - Disable
     _EN            1b - Enable

       1        Enables the CTIMER1 function clock
 CTIMER1_CLK       0b - Disable
     _EN            1b - Enable

       0        Enables the CTIMER0 function clock
 CTIMER0_CLK       0b - Disable
     _EN            1b - Enable


17.4.1.128 RAM ECC Enable Control (ECC_ENABLE_CTRL)

Offset


 Register                   Offset

 ECC_ENABLE_CTRL    E44h



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    631 / 3791

<!-- page 632 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Function
This register is used to enable/disable RAM blocks ECC function. Only combinations 'b0000, 'b0001, 'b0011, 'b0111 and
'b1111 are allowed. For details of RAM blocks and ECC availability for a specific part number refer to the device data sheet .

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                          Reserved

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2       1       0

    R                                           Reserved                                                                            RAMF  RAMD  RAMB  RAMA
                                                                                                                       _RA...   _RA...   _RA...   _EC...   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0       1       1


Fields


       Field       Function
      31-4      Reserved
    —

       3      RAMF and RAME ECC enable
 RAMF_RAME_       0b - ECC is disabled
 ECC_ENABLE        1b - ECC is enabled

       2      RAMD and RAMC ECC enable
 RAMD_RAMC_       0b - ECC is disabled
 ECC_ENABLE        1b - ECC is enabled

       1      RAMB and RAMX ECC enable
 RAMB_RAMX_       0b - ECC is disabled
 ECC_ENABLE        1b - ECC is enabled

       0      RAMA ECC enable
 RAMA_ECC_E       0b - ECC is disabled
    NABLE           1b - ECC is enabled


17.4.1.129 JTAG Chip ID (JTAG_ID)

Offset


 Register                   Offset

 JTAG_ID              FF0h



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    632 / 3791

<!-- page 633 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                      JTAG_ID

   W

Reset     u      u      u      u       0      1      1      1       0      0      1      0       0      1      1      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                      JTAG_ID

   W

Reset     0      1      0      0       0      0      0      0       0      0      1      0       1      0      1      1


Fields


       Field       Function
      31-0        Indicates the device ID
   JTAG_ID


17.4.1.130  Device Type (DEVICE_TYPE)

Offset


 Register                   Offset

 DEVICE_TYPE         FF4h


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

                                                                                                            DEVIC
    R                     DEVICE_TYPE_PIN                         DEVICE_TYPE_PKG              Reserved
                                                                                                                                                                       E_...

   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                             DEVICE_TYPE_NUM

   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    633 / 3791

<!-- page 634 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Fields


       Field       Function
     31-24       Indicates the pin number. For example, 100-pin is 0x64.
 DEVICE_TYPE
     _PIN

     23-20       Indicates the device package type
 DEVICE_TYPE       0000b - HLQFP
    _PKG           0001b - HTQFP
                     0010b - BGA
                     0011b - MAXQFP (HDQFP)
                     0100b - QFN
                     0101b - CSP
                     0110b - LQFP

     19-17      Reserved
    —

      16         Indicates the device secure type
 DEVICE_TYPE       0b - Non-secure part
    _SEC           1b - Secure part

      15-0        Indicates the device part number
 DEVICE_TYPE  bit[15:12] is family number
    _NUM                           • 0001b: MCX N series
                           • 0010b: MCX A series
                           • 0011b: MCX L series
                           • other value: reserved
                      bit[11:0]: 3 digital of the part number. For example, the value for the N947 device is 0x947.


17.4.1.131  Device ID (DEVICE_ID0)

Offset


 Register                   Offset

 DEVICE_ID0           FF8h


Function
This register contains the device ID.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    634 / 3791

<!-- page 635 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R            Reserved             CUSTOMER_REVISION_ID         ROM_REV_MINOR                   Reserved

   W

Reset     0      0      0      0       u      u      u      u       u      u      u      u       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                            Reserved                                 FLASH_SIZE                  RAM_SIZE

   W

Reset     0      0      0      0       0      0      0      0       u      u      u      u       u      u      u      u


Fields


       Field       Function
     31-28      Reserved
    —

     27-24     CM33_SECURITY_EXTENSION field.
 CUSTOMER_R       1010b - Non secure version
  EVISION_ID             All other values - Any other value represents secure version

     23-20    ROM Patch Version.
 ROM_REV_MIN
    OR

      19-8      Reserved
    —

       7-4        Indicates Flash size of the device.
  FLASH_SIZE        0000b - 32 KB
                     0001b - 64 KB
                     0010b - 128 KB
                     0011b - 256 KB
                     0100b - 512 KB
                     0101b - 768 KB
                     0110b - 1 MB
                     0111b - 1.5 MB
                     1000b - 2 MB

       3-0        Indicates RAM size of the device.
  RAM_SIZE         0000b - 8 KB

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    635 / 3791

<!-- page 636 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0001b - 16 KB
                     0010b - 32 KB
                     0011b - 64 KB
                     0100b - 96 KB
                     0101b - 128 KB
                     0110b - 160 KB
                     0111b - 192 KB
                     1000b - 256 KB
                     1001b - 288 KB
                     1010b - 352 KB
                     1011b - 512 KB


17.4.1.132  Chip Revision ID and Number (DIEID)

Offset


 Register                   Offset

 DIEID               FFCh


Function
This register contains the chip number and revision.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R            Reserved                                          MCO_NUM_IN_DIE_ID

   W

Reset     u      u      u      u       u      u      u      u       0      1      0      1       0      0      1      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                   MCO_NUM_IN_DIE_ID                        MAJOR_REVISION             MINOR_REVISION

   W

Reset     0      0      0      0       1      0      0      1       1      0      1      0       u      u      u      u





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    636 / 3791

<!-- page 637 -->

NXP Semiconductors
                                                                                          System Controller (SYSCON)

Fields


       Field       Function
     31-28      Reserved
    —

      27-8       Chip number
 MCO_NUM_IN_
     DIE_ID

       7-4       Chip major revision
 MAJOR_REVIS
     ION

       3-0       Chip minor revision
 MINOR_REVISI
    ON





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    637 / 3791
