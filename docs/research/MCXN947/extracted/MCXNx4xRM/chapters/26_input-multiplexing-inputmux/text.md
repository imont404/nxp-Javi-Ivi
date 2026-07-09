# 26 Input Multiplexing (INPUTMUX)

Source: `docs/mcxn947/MCXNx4xRM.pdf`
Pages: 902-1122

<!-- page 902 -->

NXP Semiconductors

Chapter 26
Input Multiplexing (INPUTMUX)

26.1  Chip-specific INPUTMUX information

Table 308. Reference links to related information

         Topic             Related module                                Reference

      Full description        INPUTMUX                          INPUTMUX

       Peripheral                                      PBRG memory map
    memory map

Once set up, no clocks are required for the input multiplexer to function. The system clock is needed only to write to or read
from the INPUTMUX registers. Once the input multiplexer is configured, disable the clock to the INPUTMUX module in the
AHBCLKCTRL register.

26.1.1 Module instances
This device contains one instance of the INPUTMUX module, INPUTMUX0.

26.1.2  Security considerations
For secure applications, NXP recommends that this module is configured for secure and privileged access only to prevent
unintentional or malicious modification of the system operation by nonsecure software. The secure AHB controller controls the
security level for access to peripherals and does default to secure and privileged access for all peripherals.

26.2 Overview
The Input Multiplexing module (INPUTMUX) provides signal routing options for internal peripherals. Some peripheral inputs are
multiplexed to multiple input sources. The sources can be external pins, interrupts, output signals of other peripherals, or other
internal signals.

                                           NOTE
                           Depending on the package, not all inputs from external pins may be available.

26.2.1  Block Diagram
Figure 119 shows a generic input multiplexer arrangement with n inputs.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    902 / 3791

<!-- page 903 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)





                                   INPUT0

                                   INPUT1

                                   INPUT2

                                   INPUT3

                                   INPUT4
                                                                                          to specific
                                   INPUT5                          peripheral input

                                   INPUT6





                                   INPUTn



                                                                 Specific peripheral
                                                                 select register

 Figure 119. Generic input multiplexing

26.2.2  Features
   • Configures the inputs to the SCTIMER.
   • Configures the inputs to the asynchronous CTimers.
   • Configures the inputs to the Pin Interrupt and Pattern Match (PINT).
   • Configures the inputs to CMP, ADC, DAC, QDC, PWM, PWM EXT clock, AOI, TRIG_OUT pins.
   • Configures the inputs to the Frequency Measurement (FREQME).

26.3  Functional description
The INPUTMUX implements a number of input multiplexers that select one of many inputs to be routed to a specific input signal
for a given peripheral. This is used to allow user configuration of data paths between internal modules and/or external pins on the
device. For every module input (output from the INPUTMUX), there is a register that selects the input to use, where the register
name and description provide details on the module input controlled by each register. The input signal/pin options for each of the
muxes are configurable, and can vary from mux to mux. Refer to the register descriptions for the details on the input signal/pin
options used for each INPUTMUX output in Memory map and register definition.

26.4  External signals
The INPUTMUX has no dedicated pins. Multiplexer inputs from external pins work independently of any other function assigned
to the pin as long as no analog function is enabled.

26.5 Memory map and register definition
This section includes the INPUTMUX module memory map and detailed descriptions of all registers.

26.5.1 INPUTMUX register descriptions





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    903 / 3791

<!-- page 904 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

26.5.1.1 INPUTMUX memory map
INPUTMUX0 base address: 4000_6000h

     Offset     Register                                                        Width    Access   Reset value
                                                                                                     (In bits)

   0h - 1Ch    Inputmux Register for SCT0 Input (SCT0_INMUX0 - SCT0_INMUX7)    32    RW    0000_007Fh

     20h      Capture Select Register for CTIMER Inputs (CTIMER0CAP0)          32    RW    0000_007Fh

     24h      Capture Select Register for CTIMER Inputs (CTIMER0CAP1)          32    RW    0000_007Fh

     28h      Capture Select Register for CTIMER Inputs (CTIMER0CAP2)          32    RW    0000_007Fh

    2Ch      Capture Select Register for CTIMER Inputs (CTIMER0CAP3)          32    RW    0000_007Fh

     30h       Trigger Register for CTIMER (TIMER0TRIG)                        32    RW    0000_007Fh

     40h      Capture Select Register for CTIMER Inputs (CTIMER1CAP0)          32    RW    0000_007Fh

     44h      Capture Select Register for CTIMER Inputs (CTIMER1CAP1)          32    RW    0000_007Fh

     48h      Capture Select Register for CTIMER Inputs (CTIMER1CAP2)          32    RW    0000_007Fh

    4Ch      Capture Select Register for CTIMER Inputs (CTIMER1CAP3)          32    RW    0000_007Fh

     50h       Trigger Register for CTIMER (TIMER1TRIG)                        32    RW    0000_007Fh

     60h      Capture Select Register for CTIMER Inputs (CTIMER2CAP0)          32    RW    0000_007Fh

     64h      Capture Select Register for CTIMER Inputs (CTIMER2CAP1)          32    RW    0000_007Fh

     68h      Capture Select Register for CTIMER Inputs (CTIMER2CAP2)          32    RW    0000_007Fh

    6Ch      Capture Select Register for CTIMER Inputs (CTIMER2CAP3)          32    RW    0000_007Fh

     70h       Trigger Register for CTIMER (TIMER2TRIG)                        32    RW    0000_007Fh

  A0h - BCh   Inputmux Register for SMARTDMA Arch B Inputs                    32    RW    0000_007Fh
           (SMARTDMAARCHB_INMUX0 - SMARTDMAARCHB_INMUX7)

  C0h - DCh   Pin Interrupt Select (PINTSEL0 - PINTSEL7)                       32    RW    0000_007Fh

    180h      Selection for Frequency Measurement Reference Clock               32    RW    0000_003Fh
            (FREQMEAS_REF)

    184h      Selection for Frequency Measurement Target Clock                  32    RW    0000_003Fh
            (FREQMEAS_TAR)

    1A0h     Capture Select Register for CTIMER Inputs (CTIMER3CAP0)          32    RW    0000_007Fh

    1A4h     Capture Select Register for CTIMER Inputs (CTIMER3CAP1)          32    RW    0000_007Fh

    1A8h     Capture Select Register for CTIMER Inputs (CTIMER3CAP2)          32    RW    0000_007Fh

    1ACh     Capture Select Register for CTIMER Inputs (CTIMER3CAP3)          32    RW    0000_007Fh

    1B0h      Trigger Register for CTIMER (TIMER3TRIG)                        32    RW    0000_007Fh

    1C0h     Capture Select Register for CTIMER Inputs (CTIMER4CAP0)          32    RW    0000_007Fh

    1C4h     Capture Select Register for CTIMER Inputs (CTIMER4CAP1)          32    RW    0000_007Fh

    1C8h     Capture Select Register for CTIMER Inputs (CTIMER4CAP2)          32    RW    0000_007Fh

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    904 / 3791

<!-- page 905 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

     Offset     Register                                                        Width    Access   Reset value
                                                                                                     (In bits)

    1CCh     Capture Select Register for CTIMER Inputs (CTIMER4CAP3)          32    RW    0000_007Fh

    1D0h      Trigger Register for CTIMER (TIMER4TRIG)                        32    RW    0000_007Fh

    260h    CMP0 Input Connections (CMP0_TRIG)                           32    RW    0000_003Fh

 280h - 28Ch  ADC Trigger Input Connections (ADC0_TRIG0 - ADC0_TRIG3)         32    RW    0000_007Fh

 2C0h - 2CCh ADC Trigger Input Connections (ADC1_TRIG0 - ADC1_TRIG3)         32    RW    0000_007Fh

    300h    DAC0 Trigger Inputs (DAC0_TRIG)                               32    RW    0000_003Fh

    320h    DAC1 Trigger Inputs (DAC1_TRIG)                               32    RW    0000_003Fh

    340h    DAC2 Trigger Inputs (DAC2_TRIG)                               32    RW    0000_003Fh

    360h    QDC0 Trigger Input Connections (QDC0_TRIG)                     32    RW    0000_003Fh

    364h    QDC0 Input Connections (QDC0_HOME)                          32    RW    0000_003Fh

    368h    QDC0 Input Connections (QDC0_INDEX)                          32    RW    0000_003Fh

    36Ch    QDC0 Input Connections (QDC0_PHASEB)                        32    RW    0000_003Fh

    370h    QDC0 Input Connections (QDC0_PHASEA)                        32    RW    0000_003Fh

    380h    QDC1 Trigger Input Connections (QDC1_TRIG)                     32    RW    0000_003Fh

    384h    QDC1 Input Connections (QDC1_HOME)                          32    RW    0000_003Fh

    388h    QDC1 Input Connections (QDC1_INDEX)                          32    RW    0000_003Fh

    38Ch    QDC1 Input Connections (QDC1_PHASEB)                        32    RW    0000_003Fh

    390h    QDC1 Input Connections (QDC1_PHASEA)                        32    RW    0000_003Fh

 3A0h - 3ACh PWM0 External Synchronization (FlexPWM0_SM0_EXTSYNC -        32    RW    0000_003Fh
            FlexPWM0_SM3_EXTSYNC)

 3B0h - 3BCh PWM0 Input Trigger Connections (FlexPWM0_SM0_EXTA -           32    RW    0000_003Fh
            FlexPWM0_SM3_EXTA)

    3C0h    PWM0 External Force Trigger Connections                         32    RW    0000_003Fh
            (FlexPWM0_EXTFORCE)

 3C4h - 3D0h PWM0 Fault Input Trigger Connections (FlexPWM0_FAULT0 -         32    RW    0000_003Fh
             FlexPWM0_FAULT3)

 3E0h - 3ECh PWM1 External Synchronization (FlexPWM1_SM0_EXTSYNC -        32    RW    0000_003Fh
            FlexPWM1_SM3_EXTSYNC)

 3F0h - 3FCh PWM1 Input EXTA Connections (FlexPWM1_SM0_EXTA -            32    RW    0000_003Fh
            FlexPWM1_SM3_EXTA)

    400h    PWM1 External Force Trigger Connections                         32    RW    0000_003Fh
            (FlexPWM1_EXTFORCE)

                                  Tablecontinuesonthenextpage...




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    905 / 3791

<!-- page 906 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

     Offset     Register                                                        Width    Access   Reset value
                                                                                                     (In bits)

  404h - 410h  PWM1 Fault Input Trigger Connections (FlexPWM1_FAULT0 -         32    RW    0000_003Fh
             FlexPWM1_FAULT3)

    420h    PWM0 External Clock Trigger (PWM0_EXT_CLK)                   32    RW    0000_0007h

    424h    PWM1 External Clock Trigger (PWM1_EXT_CLK)                   32    RW    0000_0007h

 440h - 47Ch  EVTG Trigger Input Connections (EVTG_TRIG0 - EVTG_TRIG15)      32    RW    0000_003Fh

    480h     USB-FS Trigger Input Connections (USBFS_TRIG)                  32    RW    0000_000Fh

    4A0h     TSI Trigger Input Connections (TSI_TRIG)                         32    RW    0000_0003h

 4C0h - 4DCh EXT Trigger Connections (EXT_TRIG0 - EXT_TRIG7)                32    RW    0000_003Fh

    4E0h    CMP1 Input Connections (CMP1_TRIG)                           32    RW    0000_003Fh

    500h    CMP2 Input Connections (CMP2_TRIG)                           32    RW    0000_003Fh

  520h - 530h  SINC Filter Channel Trigger Input Connections (SINC_FILTER_CH0     32    RW    0000_003Fh
                      - SINC_FILTER_CH4)

  580h - 588h  OPAMP Trigger Input Connections (OPAMP0_TRIG -                32    RW    0000_003Fh
            OPAMP2_TRIG)

    5A0h    LP_FLEXCOMM0 Trigger Input Connections (FLEXCOMM0_TRIG)     32    RW    0000_003Fh

    5C0h    LP_FLEXCOMM1 Trigger Input Connections (FLEXCOMM1_TRIG)     32    RW    0000_003Fh

    5E0h    LP_FLEXCOMM2 Trigger Input Connections (FLEXCOMM2_TRIG)     32    RW    0000_003Fh

    600h    LP_FLEXCOMM3 Trigger Input Connections (FLEXCOMM3_TRIG)     32    RW    0000_003Fh

    620h    LP_FLEXCOMM4 Trigger Input Connections (FLEXCOMM4_TRIG)     32    RW    0000_003Fh

    640h    LP_FLEXCOMM5 Trigger Input Connections (FLEXCOMM5_TRIG)     32    RW    0000_003Fh

    660h    LP_FLEXCOMM6 Trigger Input Connections (FLEXCOMM6_TRIG)     32    RW    0000_003Fh

    680h    LP_FLEXCOMM7 Trigger Input Connections (FLEXCOMM7_TRIG)     32    RW    0000_003Fh

    6A0h    LP_FLEXCOMM8 Trigger Input Connections (FLEXCOMM8_TRIG)     32    RW    0000_003Fh

    6C0h    LP_FLEXCOMM9 Trigger Input Connections (FLEXCOMM9_TRIG)     32    RW    0000_003Fh

 6E0h - 6FCh  FlexIO Trigger Input Connections (FLEXIO_TRIG0 - FLEXIO_TRIG7)    32    RW    0000_007Fh

    700h    DMA0 Request Enable0 (DMA0_REQ_ENABLE0)                   32    RW    FFFF_FFFFh

    704h    DMA0 Request Enable0 (DMA0_REQ_ENABLE0_SET)               32    RW    0000_0000h

    708h    DMA0 Request Enable0 (DMA0_REQ_ENABLE0_CLR)               32    RW    0000_0000h

    70Ch    DMA0 Request Enable0 (DMA0_REQ_ENABLE0_TOG)              32    RW    0000_0000h

    710h    DMA0 Request Enable1 (DMA0_REQ_ENABLE1)                   32    RW    FFFF_FFFFh

    714h    DMA0 Request Enable1 (DMA0_REQ_ENABLE1_SET)               32    RW    0000_0000h

    718h    DMA0 Request Enable1 (DMA0_REQ_ENABLE1_CLR)               32    RW    0000_0000h

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    906 / 3791

<!-- page 907 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

     Offset     Register                                                        Width    Access   Reset value
                                                                                                     (In bits)

    71Ch    DMA0 Request Enable1 (DMA0_REQ_ENABLE1_TOG)              32    RW    0000_0000h

    720h    DMA0 Request Enable2 (DMA0_REQ_ENABLE2)                   32    RW    FFFF_FFFFh

    724h    DMA0 Request Enable2 (DMA0_REQ_ENABLE2_SET)               32    RW    0000_0000h

    728h    DMA0 Request Enable2 (DMA0_REQ_ENABLE2_CLR)               32    RW    0000_0000h

    72Ch    DMA0 Request Enable2 (DMA0_REQ_ENABLE2_TOG)              32    RW    0000_0000h

    730h    DMA0 Request Enable3 (DMA0_REQ_ENABLE3)                   32    RW    03FF_FFFFh

    734h    DMA0 Request Enable3 (DMA0_REQ_ENABLE3_SET)               32    RW    0000_0000h

    738h    DMA0 Request Enable3 (DMA0_REQ_ENABLE3_CLR)               32    RW    0000_0000h

    780h    DMA1 Request Enable0 (DMA1_REQ_ENABLE0)                   32    RW    FFFF_FFFFh

    784h    DMA1 Request Enable0 (DMA1_REQ_ENABLE0_SET)               32    RW    0000_0000h

    788h    DMA1 Request Enable0 (DMA1_REQ_ENABLE0_CLR)               32    RW    0000_0000h

    78Ch    DMA1 Request Enable0 (DMA1_REQ_ENABLE0_TOG)              32    RW    0000_0000h

    790h    DMA1 Request Enable1 (DMA1_REQ_ENABLE1)                   32    RW    FFFF_FFFFh

    794h    DMA1 Request Enable1 (DMA1_REQ_ENABLE1_SET)               32    RW    0000_0000h

    798h    DMA1 Request Enable1 (DMA1_REQ_ENABLE1_CLR)               32    RW    0000_0000h

    79Ch    DMA1 Request Enable1 (DMA1_REQ_ENABLE1_TOG)              32    RW    0000_0000h

    7A0h    DMA1 Request Enable2 (DMA1_REQ_ENABLE2)                   32    RW    FFFF_FFFFh

    7A4h    DMA1 Request Enable2 (DMA1_REQ_ENABLE2_SET)               32    W     0000_0000h

    7A8h    DMA1 Request Enable2 (DMA1_REQ_ENABLE2_CLR)               32    W     0000_0000h

    7ACh    DMA1 Request Enable2 (DMA1_REQ_ENABLE2_TOG)              32    W     0000_0000h

    7B0h    DMA1 Request Enable3 (DMA1_REQ_ENABLE3)                   32    RW    03FF_FFFFh

    7B4h    DMA1 Request Enable3 (DMA1_REQ_ENABLE3_SET)               32    RW    0000_0000h

    7B8h    DMA1 Request Enable3 (DMA1_REQ_ENABLE3_CLR)               32    RW    0000_0000h

26.5.1.2  Inputmux Register for SCT0 Input (SCT0_INMUX0 - SCT0_INMUX7)

Offset
For a = 0 to 7:


 Register                   Offset

 SCT0_INMUXa         0h + (a × 4h)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    907 / 3791

<!-- page 908 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Function
This register selects one input source for each SCT0 input from 24 external and internal sources. (An exception is SCT0 input
SCT0_INMUX7, which is directly connected to SCTASYNCCLK (output of SCT clock divider) PLL clock and not multiplexed
with any other signals).The output of SCT0 Input multiplexing register 0 selects the source for SCT0 input 0. The output of
SCT0 Input multiplexing register 1 selects the source for SCT0 input 1, and so forth up to SCT0 Input multiplexing register 6,
which selects the input for SCT0 input 6.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                      Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      1      1      1       1      1      1      1


Fields


       Field       Function
      31-7      Reserved
    —

       6-0        Input number to SCT0 inputs.
      INP            000_0000b - SCT0_IN0 input is selected
                     000_0001b - SCT0_IN1 input is selected
                     000_0010b - SCT0_IN2 input is selected
                     000_0011b - SCT0_IN3 input is selected
                     000_0100b - SCT0_IN4 input is selected
                     000_0101b - SCT0_IN5 input is selected
                     000_0110b - SCT0_IN6 input is selected
                     000_0111b - SCT0_IN7 input is selected
                     000_1000b - CTIMER0_MAT0 input is selected
                     000_1001b - CTIMER1_MAT0 input is selected
                     000_1010b - CTIMER2_MAT0 input is selected
                     000_1011b - CTIMER3_MAT0 input is selected
                     000_1100b - CTIMER4_MAT0 input is selected
                     000_1101b - ADC0 ADC0_IRQ input is selected
                     000_1110b - PINT GPIO_INT_BMAT input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    908 / 3791

<!-- page 909 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     000_1111b - usb0 start of frame input is selected
                     001_0000b - usb1 start of frame input is selected
                     001_0001b - SINC Filter CH0 Conversion Complete input is selected
                     001_0010b - SINC Filter CH1 Conversion Complete input is selected
                     001_0011b - SINC Filter CH2 Conversion Complete input is selected
                     001_0100b - SINC Filter CH3 Conversion Complete input is selected
                     001_0101b - SINC Filter CH4 Conversion Complete input is selected
                     001_0110b - Reserved
                     001_0111b - DEBUG_HALTED input is selected
                     001_1000b - ADC1_IRQ input is selected
                     001_1001b - ADC0_tcomp[0] input is selected
                     001_1010b - ADC0_tcomp[1] input is selected
                     001_1011b - ADC0_tcomp[2] input is selected
                     001_1100b - ADC0_tcomp[3] input is selected
                     001_1101b - ADC1_tcomp[0] input is selected
                     001_1110b - ADC1_tcomp[1] input is selected
                     001_1111b - ADC1_tcomp[2] input is selected
                     010_0000b - ADC1_tcomp[3] input is selected
                     010_0001b - CMP0_OUT input is selected
                     010_0010b - CMP1_OUT input is selected
                     010_0011b - CMP2_OUT input is selected
                     010_0100b - PWM0_SM0_MUX_TRIG0/PWM0_SM0_MUX_TRIG1 input is selected
                     010_0101b - PWM0_SM1_MUX_TRIG0/PWM0_SM1_MUX_TRIG1 input is selected
                     010_0110b - PWM0_SM2_MUX_TRIG0/PWM0_SM2_MUX_TRIG1 input is selected
                     010_0111b - PWM0_SM3_MUX_TRIG0/PWM0_SM3_MUX_TRIG1 input is selected
                     010_1000b - PWM1_SM0_MUX_TRIG0/PWM1_SM0_MUX_TRIG1 input is selected
                     010_1001b - PWM1_SM1_MUX_TRIG0/PWM1_SM1_MUX_TRIG1 input is selected
                     010_1010b - PWM1_SM2_MUX_TRIG0/PWM1_SM2_MUX_TRIG1 input is selected
                     010_1011b - PWM1_SM3_MUX_TRIG0/PWM1_SM3_MUX_TRIG1 input is selected
                     010_1100b - QDC0_CMP/POS_MATCH input is selected
                     010_1101b - QDC1_CMP/POS_MATCH input is selected
                     010_1110b - EVTG_OUT0A input is selected
                     010_1111b - EVTG_OUT0B input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    909 / 3791

<!-- page 910 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     011_0000b - EVTG_OUT1A input is selected
                     011_0001b - EVTG_OUT1B input is selected
                     011_0010b - EVTG_OUT2A input is selected
                     011_0011b - EVTG_OUT2B input is selected
                     011_0100b - EVTG_OUT3A input is selected
                     011_0101b - EVTG_OUT3B input is selected
                     011_0110b - FC3_P0 (SDO, SDA) input is selected
                     011_0111b - FC3_P1 (SCK, TXD, SCL) input is selected
                     011_1000b - FC3_P2 (RTS, SCLS, TXD) input is selected
                     011_1001b - FC3_P3 (PCS[0], CTS, SDAS) input is selected
                     011_1010b - Reserved
                     011_1011b - Reserved
                     011_1100b - LP_FLEXCOMM0 trig 0 (lpuart_trg_txword) input is selected
                     011_1101b - LP_FLEXCOMM0 trig 1 (lpuart_trg_rxword) input is selected
                     011_1110b - LP_FLEXCOMM0 trig 2 (lpuart_trg_rxidle) input is selected
                     011_1111b - LP_FLEXCOMM1 trig 0 input is selected
                     100_0000b - LP_FLEXCOMM1 trig 1 input is selected
                     100_0001b - LP_FLEXCOMM1 trig 2 input is selected
                     100_0010b - LP_FLEXCOMM2 trig 0 input is selected
                     100_0011b - LP_FLEXCOMM2 trig 1 input is selected
                     100_0100b - LP_FLEXCOMM2 trig 2 input is selected
                     100_0101b - LP_FLEXCOMM3 trig 0 input is selected
                     100_0110b - LP_FLEXCOMM3 trig 1 input is selected
                     100_0111b - LP_FLEXCOMM3 trig 2 input is selected
                     100_1000b - LP_FLEXCOMM3 trig 3 input is selected
                     100_1001b - SAI0 TX BCLK input is selected
                     100_1010b - SAI0 RX BCLK input is selected
                     100_1011b - SAI1 TX BCLK input is selected
                     100_1100b - SAI1 RX BCLK input is selected
                               All other values are reserved.


26.5.1.3  Capture Select Register for CTIMER Inputs (CTIMER0CAP0 - CTIMER2CAP3)

Offset
For a = 0 to 2; b = 0 to 3:



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    910 / 3791

<!-- page 911 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


 Register                   Offset

 CTIMERaCAPb         20h + (a × 20h) + (b × 4h)


Function
For each of the 5 standard timers, numbered i = 0 to 4 there are 4 CTIMERiCAPTj, with j = 0 to 3, each allowing selecting
between 25 external or internal input sources. The output of TIMER0CAPT0 Input multiplexing register 0 selects the source
for TIMER0 capture input 0. The output of TIMER0CAPT1 Input multiplexing register 1 selects the source for TIMER0 capture
input 1, and so forth up to TIMER4CAPT3. Input multiplexing register 3, which selects the input for TIMER4 capture input 3.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                      Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      1      1      1       1      1      1      1


Fields


       Field       Function
      31-7      Reserved
    —

       6-0        Input number for CTIMER
      INP            000_0000b - CT_INP0 input is selected
                     000_0001b - CT_INP1 input is selected
                     000_0010b - CT_INP2 input is selected
                     000_0011b - CT_INP3 input is selected
                     000_0100b - CT_INP4 input is selected
                     000_0101b - CT_INP5 input is selected
                     000_0110b - CT_INP6 input is selected
                     000_0111b - CT_INP7 input is selected
                     000_1000b - CT_INP8 input is selected
                     000_1001b - CT_INP9 input is selected
                     000_1010b - CT_INP10 input is selected
                     000_1011b - CT_INP11 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    911 / 3791

<!-- page 912 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     000_1100b - CT_INP12 input is selected
                     000_1101b - CT_INP13 input is selected
                     000_1110b - CT_INP14 input is selected
                     000_1111b - CT_INP15 input is selected
                     001_0000b - CT_INP16 input is selected
                     001_0001b - CT_INP17 input is selected
                     001_0010b - CT_INP18 input is selected
                     001_0011b - CT_INP19 input is selected
                     001_0100b - usb0 start of frame input is selected
                     001_0101b - usb1 start of frame input is selected
                     001_0110b - DCDC_BURST_ACTIVE input is selected
                     001_0111b - sai0_tx_sync_out1 input is selected
                     001_1000b - sai0_rx_sync_out2 input is selected
                     001_1001b - ADC0_IRQ input is selected
                     001_1010b - ADC1_IRQ input is selected
                     001_1011b - CMP0_OUT input is selected
                     001_1100b - CMP1_OUT input is selected
                     001_1101b - CMP2_OUT input is selected
                     001_1110b - PWM0_SM0_MUX_TRIG0/PWM0_SM0_MUX_TRIG1 input is selected
                     001_1111b - PWM0_SM1_MUX_TRIG0/PWM0_SM1_MUX_TRIG1 input is selected
                     010_0000b - PWM0_SM2_MUX_TRIG0/PWM0_SM2_MUX_TRIG1 input is selected
                     010_0001b - PWM0_SM3_MUX_TRIG0/PWM0_SM3_MUX_TRIG1 input is selected
                     010_0010b - PWM1_SM0_MUX_TRIG0/PWM1_SM0_MUX_TRIG1 input is selected
                     010_0011b - PWM1_SM1_MUX_TRIG0/PWM1_SM1_MUX_TRIG1 input is selected
                     010_0100b - PWM1_SM2_MUX_TRIG0/PWM1_SM2_MUX_TRIG1 input is selected
                     010_0101b - PWM1_SM3_MUX_TRIG0/PWM1_SM3_MUX_TRIG1 input is selected
                     010_0110b - QDC0_CMP/POS_MATCH input is selected
                     010_0111b - QDC1_CMP/POS_MATCH input is selected
                     010_1000b - EVTG_OUT0A input is selected
                     010_1001b - EVTG_OUT0B input is selected
                     010_1010b - EVTG_OUT1A input is selected
                     010_1011b - EVTG_OUT1B input is selected
                     010_1100b - EVTG_OUT2A input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    912 / 3791

<!-- page 913 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     010_1101b - EVTG_OUT2B input is selected
                     010_1110b - EVTG_OUT3A input is selected
                     010_1111b - EVTG_OUT3B input is selected
                     011_0000b - Reserved
                     011_0001b - Reserved
                     011_0010b - LP_FLEXCOMM0 trig 0 input is selected
                     011_0011b - LP_FLEXCOMM0 trig 1 input is selected
                     011_0100b - LP_FLEXCOMM0 trig 2 input is selected
                     011_0101b - LP_FLEXCOMM1 trig 0 input is selected
                     011_0110b - LP_FLEXCOMM1 trig 1 input is selected
                     011_0111b - LP_FLEXCOMM1 trig 2 input is selected
                     011_1000b - LP_FLEXCOMM2 trig 0 input is selected
                     011_1001b - LP_FLEXCOMM2 trig 1 input is selected
                     011_1010b - LP_FLEXCOMM2 trig 2 input is selected
                     011_1011b - LP_FLEXCOMM3 trig 0 input is selected
                     011_1100b - LP_FLEXCOMM3 trig 1 input is selected
                     011_1101b - LP_FLEXCOMM3 trig 2 input is selected
                     011_1110b - LP_FLEXCOMM3 trig 3 input is selected
                     011_1111b - sai1_tx_sync_out1 input is selected
                     100_0000b - sai1_rx_sync_out2 input is selected
                               All other values are reserved.

1.  sai_tx_sync_out is Transmit Frame Sync for multi-SAI synchronous operation.
2.  sai_rx_sync_out is Receive Frame Sync for multi-SAI synchronous operation.

26.5.1.4  Trigger Register for CTIMER (TIMER0TRIG - TIMER2TRIG)

Offset


 Register                   Offset

 TIMER0TRIG           30h

 TIMER1TRIG           50h

 TIMER2TRIG           70h


Function
This is the trigger register for CTIMER.




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    913 / 3791

<!-- page 914 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                      Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      1      1      1       1      1      1      1


Fields


       Field       Function
      31-7      Reserved
    —

       6-0        Input number for CTIMER
      INP            000_0000b - CT_INP0 input is selected
                     000_0001b - CT_INP1 input is selected
                     000_0010b - CT_INP2 input is selected
                     000_0011b - CT_INP3 input is selected
                     000_0100b - CT_INP4 input is selected
                     000_0101b - CT_INP5 input is selected
                     000_0110b - CT_INP6 input is selected
                     000_0111b - CT_INP7 input is selected
                     000_1000b - CT_INP8 input is selected
                     000_1001b - CT_INP9 input is selected
                     000_1010b - CT_INP10 input is selected
                     000_1011b - CT_INP11 input is selected
                     000_1100b - CT_INP12 input is selected
                     000_1101b - CT_INP13 input is selected
                     000_1110b - CT_INP14 input is selected
                     000_1111b - CT_INP15 input is selected
                     001_0000b - CT_INP16 input is selected
                     001_0001b - CT_INP17 input is selected
                     001_0010b - CT_INP18 input is selected
                     001_0011b - CT_INP19 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    914 / 3791

<!-- page 915 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     001_0100b - usb0 start of frame input is selected
                     001_0101b - usb1 start of frame input is selected
                     001_0110b - DCDC_BURST_ACTIVE input is selected
                     001_0111b - sai0_tx_sync_out1 input is selected
                     001_1000b - sai0_rx_sync_out2 input is selected
                     001_1001b - ADC0_IRQ input is selected
                     001_1010b - ADC1_IRQ input is selected
                     001_1011b - CMP0_OUT input is selected
                     001_1100b - CMP1_OUT input is selected
                     001_1101b - CMP2_OUT input is selected
                     001_1110b - PWM0_SM0_MUX_TRIG0/PWM0_SM0_MUX_TRIG1 input is selected
                     001_1111b - PWM0_SM1_MUX_TRIG0/PWM0_SM1_MUX_TRIG1 input is selected
                     010_0000b - PWM0_SM2_MUX_TRIG0/PWM0_SM2_MUX_TRIG1 input is selected
                     010_0001b - PWM0_SM3_MUX_TRIG0/PWM0_SM3_MUX_TRIG1 input is selected
                     010_0010b - PWM1_SM0_MUX_TRIG0/PWM1_SM0_MUX_TRIG1 input is selected
                     010_0011b - PWM1_SM1_MUX_TRIG0/PWM1_SM1_MUX_TRIG1 input is selected
                     010_0100b - PWM1_SM2_MUX_TRIG0/PWM1_SM2_MUX_TRIG1 input is selected
                     010_0101b - PWM1_SM3_MUX_TRIG0/PWM1_SM3_MUX_TRIG1 input is selected
                     010_0110b - QDC0_CMP/POS_MATCH input is selected
                     010_0111b - QDC1_CMP/POS_MATCH input is selected
                     010_1000b - EVTG_OUT0A input is selected
                     010_1001b - EVTG_OUT0B input is selected
                     010_1010b - EVTG_OUT1A input is selected
                     010_1011b - EVTG_OUT1B input is selected
                     010_1100b - EVTG_OUT2A input is selected
                     010_1101b - EVTG_OUT2B input is selected
                     010_1110b - EVTG_OUT3A input is selected
                     010_1111b - EVTG_OUT3B input is selected
                     011_0000b - Reserved
                     011_0001b - Reserved
                     011_0010b - LP_FLEXCOMM0 trig 0 input is selected
                     011_0011b - LP_FLEXCOMM0 trig 1 input is selected
                     011_0100b - LP_FLEXCOMM0 trig 2 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    915 / 3791

<!-- page 916 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     011_0101b - LP_FLEXCOMM1 trig 0 input is selected
                     011_0110b - LP_FLEXCOMM1 trig 1 input is selected
                     011_0111b - LP_FLEXCOMM1 trig 2 input is selected
                     011_1000b - LP_FLEXCOMM2 trig 0 input is selected
                     011_1001b - LP_FLEXCOMM2 trig 1 input is selected
                     011_1010b - LP_FLEXCOMM2 trig 2 input is selected
                     011_1011b - LP_FLEXCOMM3 trig 0 input is selected
                     011_1100b - LP_FLEXCOMM3 trig 1 input is selected
                     011_1101b - LP_FLEXCOMM3 trig 2 input is selected
                     011_1110b - LP_FLEXCOMM3 trig 3 input is selected
                     011_1111b - sai1_tx_sync_out1 input is selected
                     100_0000b - sai1_rx_sync_out2 input is selected
                               All other values are reserved.

1.  sai_tx_sync_out is Transmit Frame Sync for multi-SAI synchronous operation.
2.  sai_rx_sync_out is Receive Frame Sync for multi-SAI synchronous operation.

26.5.1.5  Inputmux Register for SMARTDMA Arch B Inputs (SMARTDMAARCHB_INMUX0 -
       SMARTDMAARCHB_INMUX7)

Offset
For a = 0 to 7:


 Register                   Offset

 SMARTDMAARCHB_IN  A0h + (a × 4h)
 MUXa


Function
This register is used to select SMARTDMA ArchB inputs.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    916 / 3791

<!-- page 917 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                      Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      1      1      1       1      1      1      1


Fields


       Field       Function
      31-7      Reserved
    —

       6-0        Input number select to SmartDMA ARCHB input
      INP            000_0000b - FlexIO interrupt is selected as input
                     000_0001b - GPIO P0_1 input is selected
                     000_0010b - GPIO P0_2 input is selected
                     000_0011b - GPIO P0_3 input is selected
                     000_0100b - GPIO P0_4 input is selected
                     000_0101b - GPIO P0_5 input is selected
                     000_0110b - GPIO P0_6 input is selected
                     000_0111b - GPIO P0_7 input is selected
                     000_1000b - GPIO P0_8 input is selected
                     000_1001b - GPIO P0_9 input is selected
                     000_1010b - GPIO P0_10 input is selected
                     000_1011b - GPIO P0_11 input is selected
                     000_1100b - GPIO P0_12 input is selected
                     000_1101b - GPIO P0_13 input is selected
                     000_1110b - GPIO P0_14 input is selected
                     000_1111b - GPIO P0_15 input is selected
                     001_0000b - SCT0 SCT_OUT8 input is selected
                     001_0001b - SCT0 SCT_OUT9 input is selected
                     001_0010b - Reserved
                     001_0011b - Reserved

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    917 / 3791

<!-- page 918 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     001_0100b - MRT0 MRT_CH0_IRQ input is selected
                     001_0101b - MRT0 MRT_CH1_IRQ input is selected
                     001_0110b - CTIMER4_MAT3 input is selected
                     001_0111b - CTIMER4_MAT2 input is selected
                     001_1000b - CTIMER3_MAT3 input is selected
                     001_1001b - CTIMER3_MAT2 input is selected
                     001_1010b - CTIMER1_MAT3 input is selected
                     001_1011b - CTIMER1_MAT2 input is selected
                     001_1100b - UTICK0 UTICK_IRQ input is selected
                     001_1101b - WWDT0 WDT0_IRQ input is selected
                     001_1110b - ADC0 ADC0_IRQ input is selected
                     001_1111b - CMP0_IRQ input is selected
                     010_0000b - Reserved
                     010_0001b - LP_FLEXCOMM7_IRQ input is selected
                     010_0010b - LP_FLEXCOMM6_IRQ input is selected
                     010_0011b - LP_FLEXCOMM5_IRQ input is selected
                     010_0100b - LP_FLEXCOMM4_IRQ input is selected
                     010_0101b - LP_FLEXCOMM3_IRQ input is selected
                     010_0110b - LP_FLEXCOMM2_IRQ input is selected
                     010_0111b - LP_FLEXCOMM1_IRQ input is selected
                     010_1000b - LP_FLEXCOMM0_IRQ input is selected
                     010_1001b - DMA0_IRQ input is selected
                     010_1010b - DMA1_IRQ input is selected
                     010_1011b - SYS_IRQ1 input is selected
                     010_1100b - RTC_COMBO_IRQ input is selected
                     010_1101b - ARM_TXEV input is selected
                     010_1110b - PINT0 GPIO_INT_BMATCH input is selected
                     010_1111b - Reserved
                     011_0000b - Reserved
                     011_0001b - CMP0_OUT input is selected
                     011_0010b - usb0 start of frame input is selected
                     011_0011b - usb1 start of frame input is selected
                     011_0100b - OSTIMER0 OS_EVENT_TIMER_IRQ input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    918 / 3791

<!-- page 919 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     011_0101b - ADC1_IRQ input is selected
                     011_0110b - CMP0_IRQ/CMP1_IRQ/CMP2_IRQ input is selected
                     011_0111b - DAC0_IRQ input is selected
                     011_1000b - DAC1_IRQ/DAC2_IRQ input is selected
                     011_1001b - PWM0_IRQ input is selected
                     011_1010b - PWM1_IRQ input is selected
                     011_1011b - QDC0_IRQ input is selected
                     011_1100b - QDC1_IRQ input is selected
                     011_1101b - EVTG_OUT0A input is selected
                     011_1110b - EVTG_OUT1A input is selected
                     011_1111b - Reserved
                     100_0000b - Reserved
                     100_0001b - GPIO1_alias0 GPIO1 Pin Event Trig 0 input is selected
                     100_0010b - GPIO1_alias1 GPIO1 Pin Event Trig 1 input is selected
                     100_0011b - GPIO2_alias0 GPIO2 Pin Event Trig 0 input is selected
                     100_0100b - GPIO2_alias1 GPIO2 Pin Event Trig 1 input is selected
                     100_0101b - GPIO3_alias0 GPIO3 Pin Event Trig 0 input is selected
                     100_0110b - GPIO3_alias1 GPIO3 Pin Event Trig 1 input is selected
                               All other values are reserved.

1. SYS_IRQ combines the CDOG IRQ, WWDT IRQ, MBC secure violation IRQ, Secure AHB Matrix secure violation IRQ,
  GDET IRQ, ELS S50 error IRQ, PKC error IRQ, and VBAT IRQ using the logical OR operation.

26.5.1.6  Pin Interrupt Select (PINTSEL0 - PINTSEL7)

Offset
For a = 0 to 7:


 Register                   Offset

 PINTSELa            C0h + (a × 4h)


Function
Each of these eight registers selects one pin from among ports 0 and 1 as the source of a pin interrupt or as the input to
the pattern match engine. To select a pin for any of the 8 pin interrupts or pattern match engine inputs, write the GPIO port
pin number as 0 to 31 for pins P0_0 to P0_31 to the INP bits. Port 1 pins correspond to pin numbers 32 to 63. For example,
setting INP to 0x5 in PINTSEL0 selects pin P0_5 for pin interrupt 0. To determine the GPIO port pin number for a given device
package, see the pin description table in the data sheet.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    919 / 3791

<!-- page 920 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                      Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      1      1      1       1      1      1      1


Fields


       Field       Function
      31-7      Reserved
    —

       6-0        Pin number select for pin interrupt or pattern match engine input. For PIOx_y: INP = (x * 32) + y. PIO0_0
                    to PIO1_31 correspond to numbers 0 to 63.
      INP
                     000_0000b - GPIO P0_0 input is selected
                     000_0001b - GPIO P0_1 input is selected
                     000_0010b - GPIO P0_2 input is selected
                     000_0011b - GPIO P0_3 input is selected
                     000_0100b - GPIO P0_4 input is selected
                     000_0101b - GPIO P0_5 input is selected
                     000_0110b - GPIO P0_6 input is selected
                     000_0111b - GPIO P0_7 input is selected
                     000_1000b - GPIO P0_8 input is selected
                     000_1001b - GPIO P0_9 input is selected
                     000_1010b - GPIO P0_10 input is selected
                     000_1011b - GPIO P0_11 input is selected
                     000_1100b - GPIO P0_12 input is selected
                     000_1101b - GPIO P0_13 input is selected
                     000_1110b - GPIO P0_14 input is selected
                     000_1111b - GPIO P0_15 input is selected
                     001_0000b - GPIO P0_16 input is selected
                     001_0001b - GPIO P0_17 input is selected
                     001_0010b - GPIO P0_18 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    920 / 3791

<!-- page 921 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     001_0011b - GPIO P0_19 input is selected
                     001_0100b - GPIO P0_20 input is selected
                     001_0101b - GPIO P0_21 input is selected
                     001_0110b - GPIO P0_22 input is selected
                     001_0111b - GPIO P0_23 input is selected
                     001_1000b - GPIO P0_24 input is selected
                     001_1001b - GPIO P0_25 input is selected
                     001_1010b - GPIO P0_26 input is selected
                     001_1011b - GPIO P0_27 input is selected
                     001_1100b - GPIO P0_28 input is selected
                     001_1101b - GPIO P0_29 input is selected
                     001_1110b - GPIO P0_30 input is selected
                     001_1111b - GPIO P0_31 input is selected
                     010_0000b - GPIO P1_0 input is selected
                     010_0001b - GPIO P1_1 input is selected
                     010_0010b - GPIO P1_2 input is selected
                     010_0011b - GPIO P1_3 input is selected
                     010_0100b - GPIO P1_4 input is selected
                     010_0101b - GPIO P1_5 input is selected
                     010_0110b - GPIO P1_6 input is selected
                     010_0111b - GPIO P1_7 input is selected
                     010_1000b - GPIO P1_8 input is selected
                     010_1001b - GPIO P1_9 input is selected
                     010_1010b - GPIO P1_10 input is selected
                     010_1011b - GPIO P1_11 input is selected
                     010_1100b - GPIO P1_12 input is selected
                     010_1101b - GPIO P1_13 input is selected
                     010_1110b - GPIO P1_14 input is selected
                     010_1111b - GPIO P1_15 input is selected
                     011_0000b - GPIO P1_16 input is selected
                     011_0001b - GPIO P1_17 input is selected
                     011_0010b - GPIO P1_18 input is selected
                     011_0011b - GPIO P1_19 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    921 / 3791

<!-- page 922 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     011_0100b - GPIO P1_20 input is selected
                     011_0101b - GPIO P1_21 input is selected
                     011_0110b - GPIO P1_22 input is selected
                     011_0111b - GPIO P1_23 input is selected
                     011_1000b - Reserved
                     011_1001b - Reserved
                     011_1010b - Reserved
                     011_1011b - Reserved
                     011_1100b - Reserved
                     011_1101b - Reserved
                     011_1110b - GPIO P1_30 input is selected
                     011_1111b - GPIO P1_31 input is selected
                               All other values are reserved.


26.5.1.7  Selection for Frequency Measurement Reference Clock (FREQMEAS_REF)

Offset


 Register                   Offset

 FREQMEAS_REF       180h


Function
This register selects a clock for the reference clock of the frequency measure function. By default, no clock is selected.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    922 / 3791

<!-- page 923 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-6      Reserved
    —

       5-0       Clock source number (binary value) for frequency measure function reference clock.
      INP            00_0000b - clk_in (output of clk_in or XTAL mux in Clockgen) input is selected
                     00_0001b - FRO_12M input is selected
                     00_0010b - FRO_144M input is selected
                     00_0011b - Reserved
                     00_0100b - OSC_32K input is selected
                     00_0101b - CPU/system_clk input is selected
                     00_0110b - FREQME_CLK_IN0 input is selected
                     00_0111b - FREQME_CLK_IN1 input is selected
                     00_1000b - EVTG_OUT0A input is selected
                     00_1001b - EVTG_OUT1A input is selected
                               All other values are reserved.


26.5.1.8  Selection for Frequency Measurement Target Clock (FREQMEAS_TAR)

Offset


 Register                   Offset

 FREQMEAS_TAR       184h


Function
This register selects a clock for the target clock of the frequency measure function. By default, no clock is selected.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    923 / 3791

<!-- page 924 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-6      Reserved
    —

       5-0       Clock source number (binary value) for frequency measure function target clock.
      INP            00_0000b - clk_in (output of clk_in or XTAL mux in Clockgen) input is selected
                     00_0001b - FRO_12M input is selected
                     00_0010b - FRO_144M input is selected
                     00_0011b - Reserved
                     00_0100b - OSC_32K input is selected
                     00_0101b - CPU/system_clk input is selected
                     00_0110b - FREQME_CLK_IN0 input is selected
                     00_0111b - FREQME_CLK_IN1 input is selected
                     00_1000b - EVTG_OUT0A input is selected
                     00_1001b - EVTG_OUT1A input is selected
                               All other values are reserved.


26.5.1.9  Capture Select Register for CTIMER Inputs (CTIMER3CAP0 - CTIMER4CAP3)

Offset
For a = 3 to 4; b = 0 to 3:


 Register                   Offset

 CTIMERaCAPb         140h + (a × 20h) + (b × 4h)


Function
For each of the 5 standard timers, numbered i = 0 to 4 there are 4 CTIMERiCAPTj, with j = 0 to 3, each allowing selecting
between 25 external or internal input sources. The output of TIMER0CAPT0 Input multiplexing register 0 selects the source
for TIMER0 capture input 0. The output of TIMER0CAPT1 Input multiplexing register 1 selects the source for TIMER0 capture
input 1, and so forth up to TIMER4CAPT3 Input multiplexing register 3, which selects the input for TIMER4 capture input 3.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    924 / 3791

<!-- page 925 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                      Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      1      1      1       1      1      1      1


Fields


       Field       Function
      31-7      Reserved
    —

       6-0        Input number for CTIMER
      INP            000_0000b - CT_INP0 input is selected
                     000_0001b - CT_INP1 input is selected
                     000_0010b - CT_INP2 input is selected
                     000_0011b - CT_INP3 input is selected
                     000_0100b - CT_INP4 input is selected
                     000_0101b - CT_INP5 input is selected
                     000_0110b - CT_INP6 input is selected
                     000_0111b - CT_INP7 input is selected
                     000_1000b - CT_INP8 input is selected
                     000_1001b - CT_INP9 input is selected
                     000_1010b - CT_INP10 input is selected
                     000_1011b - CT_INP11 input is selected
                     000_1100b - CT_INP12 input is selected
                     000_1101b - CT_INP13 input is selected
                     000_1110b - CT_INP14 input is selected
                     000_1111b - CT_INP15 input is selected
                     001_0000b - CT_INP16 input is selected
                     001_0001b - CT_INP17 input is selected
                     001_0010b - CT_INP18 input is selected
                     001_0011b - CT_INP19 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    925 / 3791

<!-- page 926 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     001_0100b - usb0 start of frame input is selected
                     001_0101b - usb1 start of frame input is selected
                     001_0110b - DCDC_BURST_ACTIVE input is selected
                     001_0111b - sai0_tx_sync_out1 input is selected
                     001_1000b - sai0_rx_sync_out2 input is selected
                     001_1001b - ADC0 ADC0_IRQ input is selected
                     001_1010b - ADC0 ADC1_IRQ input is selected
                     001_1011b - CMP0_OUT input is selected
                     001_1100b - CMP1_OUT input is selected
                     001_1101b - CMP2_OUT input is selected
                     001_1110b - PWM0_SM0_MUX_TRIG0/PWM0_SM0_MUX_TRIG1 input is selected
                     001_1111b - PWM0_SM1_MUX_TRIG0/PWM0_SM1_MUX_TRIG1 input is selected
                     010_0000b - PWM0_SM2_MUX_TRIG0/PWM0_SM2_MUX_TRIG1 input is selected
                     010_0001b - PWM0_SM3_MUX_TRIG0/PWM0_SM3_MUX_TRIG1 input is selected
                     010_0010b - PWM1_SM0_MUX_TRIG0/PWM1_SM0_MUX_TRIG1 input is selected
                     010_0011b - PWM1_SM1_MUX_TRIG0/PWM1_SM1_MUX_TRIG1 input is selected
                     010_0100b - PWM1_SM2_MUX_TRIG0/PWM1_SM2_MUX_TRIG1 input is selected
                     010_0101b - PWM1_SM3_MUX_TRIG0/PWM1_SM3_MUX_TRIG1 input is selected
                     010_0110b - QDC0_CMP/POS_MATCH input is selected
                     010_0111b - QDC1_CMP/POS_MATCH input is selected
                     010_1000b - EVTG_OUT0A input is selected
                     010_1001b - EVTG_OUT0B input is selected
                     010_1010b - EVTG_OUT1A input is selected
                     010_1011b - EVTG_OUT1B input is selected
                     010_1100b - EVTG_OUT2A input is selected
                     010_1101b - EVTG_OUT2B input is selected
                     010_1110b - EVTG_OUT3A input is selected
                     010_1111b - EVTG_OUT3B input is selected
                     011_0000b - Reserved
                     011_0001b - Reserved
                     011_0010b - LP_FLEXCOMM0 trig 0 input is selected
                     011_0011b - LP_FLEXCOMM0 trig 1 input is selected
                     011_0100b - LP_FLEXCOMM0 trig 2 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    926 / 3791

<!-- page 927 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     011_0101b - LP_FLEXCOMM1 trig 0 input is selected
                     011_0110b - LP_FLEXCOMM1 trig 1 input is selected
                     011_0111b - LP_FLEXCOMM1 trig 2 input is selected
                     011_1000b - LP_FLEXCOMM2 trig 0 input is selected
                     011_1001b - LP_FLEXCOMM2 trig 1 input is selected
                     011_1010b - LP_FLEXCOMM2 trig 2 input is selected
                     011_1011b - LP_FLEXCOMM3 trig 0 input is selected
                     011_1100b - LP_FLEXCOMM3 trig 1 input is selected
                     011_1101b - LP_FLEXCOMM3 trig 2 input is selected
                     011_1110b - LP_FLEXCOMM3 trig 3 input is selected
                     011_1111b - sai1_tx_sync_out1 input is selected
                     100_0000b - sai1_rx_sync_out2 input is selected
                               All other values are reserved.

1.  sai_tx_sync_out is Transmit Frame Sync for multi-SAI synchronous operation.
2.  sai_rx_sync_out is Receive Frame Sync for multi-SAI synchronous operation.

26.5.1.10  Trigger Register for CTIMER (TIMER3TRIG - TIMER4TRIG)

Offset


 Register                   Offset

 TIMER3TRIG          1B0h

 TIMER4TRIG          1D0h


Function
This is the trigger register for CTIMER.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                      Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      1      1      1       1      1      1      1



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    927 / 3791

<!-- page 928 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-7      Reserved
    —

       6-0        Input number for CTIMER
      INP            000_0000b - CT_INP0 input is selected
                     000_0001b - CT_INP1 input is selected
                     000_0010b - CT_INP2 input is selected
                     000_0011b - CT_INP3 input is selected
                     000_0100b - CT_INP4 input is selected
                     000_0101b - CT_INP5 input is selected
                     000_0110b - CT_INP6 input is selected
                     000_0111b - CT_INP7 input is selected
                     000_1000b - CT_INP8 input is selected
                     000_1001b - CT_INP9 input is selected
                     000_1010b - CT_INP10 input is selected
                     000_1011b - CT_INP11 input is selected
                     000_1100b - CT_INP12 input is selected
                     000_1101b - CT_INP13 input is selected
                     000_1110b - CT_INP14 input is selected
                     000_1111b - CT_INP15 input is selected
                     001_0000b - CT_INP16 input is selected
                     001_0001b - CT_INP17 input is selected
                     001_0010b - CT_INP18 input is selected
                     001_0011b - CT_INP19 input is selected
                     001_0100b - usb0 start of frame input is selected
                     001_0101b - usb1 start of frame input is selected
                     001_0110b - DCDC_BURST_ACTIVE input is selected
                     001_0111b - sai0_tx_sync_out1 input is selected
                     001_1000b - sai0_rx_sync_out2 input is selected
                     001_1001b - ADC0 ADC0_IRQ input is selected
                     001_1010b - ADC0 ADC1_IRQ input is selected
                     001_1011b - CMP0_OUT input is selected
                     001_1100b - CMP1_OUT input is selected
                     001_1101b - CMP2_OUT input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    928 / 3791

<!-- page 929 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     001_1110b - PWM0_SM0_MUX_TRIG0/PWM0_SM0_MUX_TRIG1 input is selected
                     001_1111b - PWM0_SM1_MUX_TRIG0/PWM0_SM1_MUX_TRIG1 input is selected
                     010_0000b - PWM0_SM2_MUX_TRIG0/PWM0_SM2_MUX_TRIG1 input is selected
                     010_0001b - PWM0_SM3_MUX_TRIG0/PWM0_SM3_MUX_TRIG1 input is selected
                     010_0010b - PWM1_SM0_MUX_TRIG0/PWM1_SM0_MUX_TRIG1 input is selected
                     010_0011b - PWM1_SM1_MUX_TRIG0/PWM1_SM1_MUX_TRIG1 input is selected
                     010_0100b - PWM1_SM2_MUX_TRIG0/PWM1_SM2_MUX_TRIG1 input is selected
                     010_0101b - PWM1_SM3_MUX_TRIG0/PWM1_SM3_MUX_TRIG1 input is selected
                     010_0110b - QDC0_CMP/POS_MATCH input is selected
                     010_0111b - QDC1_CMP/POS_MATCH input is selected
                     010_1000b - EVTG_OUT0A input is selected
                     010_1001b - EVTG_OUT0B input is selected
                     010_1010b - EVTG_OUT1A input is selected
                     010_1011b - EVTG_OUT1B input is selected
                     010_1100b - EVTG_OUT2A input is selected
                     010_1101b - EVTG_OUT2B input is selected
                     010_1110b - EVTG_OUT3A input is selected
                     010_1111b - EVTG_OUT3B input is selected
                     011_0000b - Reserved
                     011_0001b - Reserved
                     011_0010b - LP_FLEXCOMM0 trig 0 input is selected
                     011_0011b - LP_FLEXCOMM0 trig 1 input is selected
                     011_0100b - LP_FLEXCOMM0 trig 2 input is selected
                     011_0101b - LP_FLEXCOMM1 trig 0 input is selected
                     011_0110b - LP_FLEXCOMM1 trig 1 input is selected
                     011_0111b - LP_FLEXCOMM1 trig 2 input is selected
                     011_1000b - LP_FLEXCOMM2 trig 0 input is selected
                     011_1001b - LP_FLEXCOMM2 trig 1 input is selected
                     011_1010b - LP_FLEXCOMM2 trig 2 input is selected
                     011_1011b - LP_FLEXCOMM3 trig 0 input is selected
                     011_1100b - LP_FLEXCOMM3 trig 1 input is selected
                     011_1101b - LP_FLEXCOMM3 trig 2 input is selected
                     011_1110b - LP_FLEXCOMM3 trig 3 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    929 / 3791

<!-- page 930 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     011_1111b - sai1_tx_sync_out1 input is selected
                     100_0000b - sai1_rx_sync_out2 input is selected
                               All other values are reserved.

1.  sai_tx_sync_out is Transmit Frame Sync for multi-SAI synchronous operation.
2.  sai_rx_sync_out is Receive Frame Sync for multi-SAI synchronous operation.

26.5.1.11 CMP0 Input Connections (CMP0_TRIG)

Offset


 Register                   Offset

 CMP0_TRIG           260h


Function
This register selects the CMP0 SAMPLE/WINDOW input.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                   TRIGIN
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


Fields


       Field       Function
      31-6      Reserved
    —

       5-0     CMP0 input trigger
    TRIGIN           00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT6 input is selected
                     00_0010b - SCT_OUT4 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    930 / 3791

<!-- page 931 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     00_0011b - SCT_OUT5 input is selected
                     00_0100b - SCT_OUT6 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER0_MAT0 input is selected
                     00_1001b - CTIMER4_MAT0 input is selected
                     00_1010b - Reserved
                     00_1011b - Reserved
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[0] input is selected
                     00_1110b - ADC1_tcomp[0] input is selected
                     00_1111b - Reserved
                     01_0000b - Reserved
                     01_0001b - PWM0_SM0_MUX_TRIG0/PWM0_SM0_MUX_TRIG1 input is selected
                     01_0010b - PWM0_SM1_MUX_TRIG0/PWM0_SM1_MUX_TRIG1 input is selected
                     01_0011b - PWM0_SM2_MUX_TRIG0/PWM0_SM2_MUX_TRIG1 input is selected
                     01_0100b - PWM0_SM3_MUX_TRIG0/PWM0_SM3_MUX_TRIG1 input is selected
                     01_0101b - PWM1_SM0_MUX_TRIG0/PWM1_SM0_MUX_TRIG1 input is selected
                     01_0110b - PWM1_SM1_MUX_TRIG0/PWM1_SM1_MUX_TRIG1 input is selected
                     01_0111b - PWM1_SM2_MUX_TRIG0/PWM1_SM2_MUX_TRIG1 input is selected
                     01_1000b - PWM1_SM3_MUX_TRIG0/PWM1_SM3_MUX_TRIG1 input is selected
                     01_1001b - QDC0_CMP/POS_MATCH input is selected
                     01_1010b - QDC1_CMP/POS_MATCH input is selected
                     01_1011b - EVTG_OUT0A input is selected
                     01_1100b - EVTG_OUT0B input is selected
                     01_1101b - EVTG_OUT1A input is selected
                     01_1110b - EVTG_OUT1B input is selected
                     01_1111b - EVTG_OUT2A input is selected
                     10_0000b - EVTG_OUT2B input is selected
                     10_0001b - EVTG_OUT3A input is selected
                     10_0010b - EVTG_OUT3B input is selected
                     10_0011b - LPTMR0 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    931 / 3791

<!-- page 932 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     10_0100b - LPTMR1 input is selected
                     10_0101b - GPIO2 Pin Event Trig 0 input is selected
                     10_0110b - GPIO2 Pin Event Trig 1 input is selected
                     10_0111b - GPIO3 Pin Event Trig 0 input is selected
                     10_1000b - GPIO3 Pin Event Trig 1 input is selected
                               All other values are reserved.


26.5.1.12 ADC Trigger Input Connections (ADC0_TRIG0 - ADC0_TRIG3)

Offset


 Register                   Offset

 ADC0_TRIG0           280h

 ADC0_TRIG1           284h

 ADC0_TRIG2           288h

 ADC0_TRIG3          28Ch


Function
This register selects the ADC0 trigger inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                   TRIGIN
   W

Reset     0      0      0      0       0      0      0      0       0      1      1      1       1      1      1      1


Fields


       Field       Function
      31-8      Reserved
    —

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    932 / 3791

<!-- page 933 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       7-0     ADC0 trigger inputs
    TRIGIN           0000_0000b - PINT PIN_INT0 input is selected
                     0000_0001b - PINT PIN_INT1 input is selected
                     0000_0010b - SCT0 SCT_OUT4 input is selected
                     0000_0011b - SCT0 SCT_OUT5 input is selected
                     0000_0100b - SCT0 SCT_OUT9 input is selected
                     0000_0101b - CTIMER0_MAT3 input is selected
                     0000_0110b - CTIMER1_MAT3 input is selected
                     0000_0111b - CTIMER2_MAT3 input is selected
                     0000_1000b - CTIMER3_MAT3 input is selected
                     0000_1001b - CTIMER4_MAT3 input is selected
                     0000_1010b - DCDC_Burst_Done_Trig input is selected
                     0000_1011b - Reserved
                     0000_1100b - PINT GPIO_INT_BMAT input is selected
                     0000_1101b - ADC0_tcomp[0] input is selected
                     0000_1110b - ADC0_tcomp[1] input is selected
                     0000_1111b - ADC0_tcomp[2] input is selected
                     0001_0000b - ADC0_tcomp[3] input is selected
                     0001_0001b - ADC1_tcomp[0] input is selected
                     0001_0010b - ADC1_tcomp[1] input is selected
                     0001_0011b - ADC1_tcomp[2] input is selected
                     0001_0100b - ADC1_tcomp[3] input is selected
                     0001_0101b - CMP0_OUT input is selected
                     0001_0110b - CMP1_OUT input is selected
                     0001_0111b - CMP2_OUT input is selected
                     0001_1000b - PWM0_SM0_MUX_TRIG0 input is selected
                     0001_1001b - PWM0_SM0_MUX_TRIG1 input is selected
                     0001_1010b - PWM0_SM1_MUX_TRIG0 input is selected
                     0001_1011b - PWM0_SM1_MUX_TRIG1 input is selected
                     0001_1100b - PWM0_SM2_MUX_TRIG0 input is selected
                     0001_1101b - PWM0_SM2_MUX_TRIG1 input is selected
                     0001_1110b - PWM0_SM3_MUX_TRIG0 input is selected
                     0001_1111b - PWM0_SM3_MUX_TRIG1 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    933 / 3791

<!-- page 934 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0010_0000b - PWM1_SM0_MUX_TRIG0 input is selected
                     0010_0001b - PWM1_SM0_MUX_TRIG1 input is selected
                     0010_0010b - PWM1_SM1_MUX_TRIG0 input is selected
                     0010_0011b - PWM1_SM1_MUX_TRIG1 input is selected
                     0010_0100b - PWM1_SM2_MUX_TRIG0 input is selected
                     0010_0101b - PWM1_SM2_MUX_TRIG1 input is selected
                     0010_0110b - PWM1_SM3_MUX_TRIG0 input is selected
                     0010_0111b - PWM1_SM3_MUX_TRIG1 input is selected
                     0010_1000b - QDC0_CMP/POS_MATCH input is selected
                     0010_1001b - QDC1_CMP/POS_MATCH input is selected
                     0010_1010b - EVTG_OUT0A input is selected
                     0010_1011b - EVTG_OUT0B input is selected
                     0010_1100b - EVTG_OUT1A input is selected
                     0010_1101b - EVTG_OUT1B input is selected
                     0010_1110b - EVTG_OUT2A input is selected
                     0010_1111b - EVTG_OUT2B input is selected
                     0011_0000b - EVTG_OUT3A input is selected
                     0011_0001b - EVTG_OUT3B input is selected
                     0011_0010b - LPTMR0 input is selected
                     0011_0011b - LPTMR1 input is selected
                     0011_0100b - FlexIO CH0 input is selected
                     0011_0101b - FlexIO CH1 input is selected
                     0011_0110b - FlexIO CH2 input is selected
                     0011_0111b - FlexIO CH3 input is selected
                     0011_1000b - SINC Filter CH0 Conversion Complete input is selected
                     0011_1001b - SINC Filter CH1 Conversion Complete input is selected
                     0011_1010b - SINC Filter CH2 Conversion Complete input is selected
                     0011_1011b - SINC Filter CH3 Conversion Complete input is selected
                     0011_1100b - SINC Filter CH4 Conversion Complete input is selected
                     0011_1101b - GPIO2 Pin Event Trig 0 input is selected
                     0011_1110b - GPIO2 Pin Event Trig 1 input is selected
                     0011_1111b - GPIO3 Pin Event Trig 0 input is selected
                     0100_0000b - GPIO3 Pin Event Trig 1 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    934 / 3791

<!-- page 935 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                               All other values are reserved.


26.5.1.13 ADC Trigger Input Connections (ADC1_TRIG0 - ADC1_TRIG3)

Offset


 Register                   Offset

 ADC1_TRIG0          2C0h

 ADC1_TRIG1          2C4h

 ADC1_TRIG2          2C8h

 ADC1_TRIG3         2CCh


Function
This register selects the ADC1 trigger inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                                   TRIGIN
   W

Reset     0      0      0      0       0      0      0      0       0      1      1      1       1      1      1      1


Fields


       Field       Function
      31-8      Reserved
    —

       7-0     ADC1 trigger inputs
    TRIGIN           0000_0000b - PINT PIN_INT0 input is selected
                     0000_0001b - PINT PIN_INT2 input is selected
                     0000_0010b - SCT0 SCT_OUT4 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    935 / 3791

<!-- page 936 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0000_0011b - SCT0 SCT_OUT5 input is selected
                     0000_0100b - SCT0 SCT_OUT3 input is selected
                     0000_0101b - CTIMER0_MAT3 input is selected
                     0000_0110b - CTIMER1_MAT3 input is selected
                     0000_0111b - CTIMER2_MAT3 input is selected
                     0000_1000b - CTIMER3_MAT2 input is selected
                     0000_1001b - CTIMER4_MAT1 input is selected
                     0000_1010b - DCDC_Burst_Done_Trig input is selected
                     0000_1011b - Reserved
                     0000_1100b - PINT GPIO_INT_BMAT input is selected
                     0000_1101b - ADC0_tcomp[0] input is selected
                     0000_1110b - ADC0_tcomp[1] input is selected
                     0000_1111b - ADC0_tcomp[2] input is selected
                     0001_0000b - ADC0_tcomp[3] input is selected
                     0001_0001b - ADC1_tcomp[0] input is selected
                     0001_0010b - ADC1_tcomp[1] input is selected
                     0001_0011b - ADC1_tcomp[2] input is selected
                     0001_0100b - ADC1_tcomp[3] input is selected
                     0001_0101b - CMP0_OUT input is selected
                     0001_0110b - CMP1_OUT input is selected
                     0001_0111b - CMP2_OUT input is selected
                     0001_1000b - PWM0_SM0_MUX_TRIG0 input is selected
                     0001_1001b - PWM0_SM0_MUX_TRIG1 input is selected
                     0001_1010b - PWM0_SM1_MUX_TRIG0 input is selected
                     0001_1011b - PWM0_SM1_MUX_TRIG1 input is selected
                     0001_1100b - PWM0_SM2_MUX_TRIG0 input is selected
                     0001_1101b - PWM0_SM2_MUX_TRIG1 input is selected
                     0001_1110b - PWM0_SM3_MUX_TRIG0 input is selected
                     0001_1111b - PWM0_SM3_MUX_TRIG1 input is selected
                     0010_0000b - PWM1_SM0_MUX_TRIG0 input is selected
                     0010_0001b - PWM1_SM0_MUX_TRIG1 input is selected
                     0010_0010b - PWM1_SM1_MUX_TRIG0 input is selected
                     0010_0011b - PWM1_SM1_MUX_TRIG1 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    936 / 3791

<!-- page 937 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0010_0100b - PWM1_SM2_MUX_TRIG0 input is selected
                     0010_0101b - PWM1_SM2_MUX_TRIG1 input is selected
                     0010_0110b - PWM1_SM3_MUX_TRIG0 input is selected
                     0010_0111b - PWM1_SM3_MUX_TRIG1 input is selected
                     0010_1000b - QDC0_CMP/POS_MATCH input is selected
                     0010_1001b - QDC1_CMP/POS_MATCH input is selected
                     0010_1010b - EVTG_OUT0A input is selected
                     0010_1011b - EVTG_OUT0B input is selected
                     0010_1100b - EVTG_OUT1A input is selected
                     0010_1101b - EVTG_OUT1B input is selected
                     0010_1110b - EVTG_OUT2A input is selected
                     0010_1111b - EVTG_OUT2B input is selected
                     0011_0000b - EVTG_OUT3A input is selected
                     0011_0001b - EVTG_OUT3B input is selected
                     0011_0010b - LPTMR0 input is selected
                     0011_0011b - LPTMR1 input is selected
                     0011_0100b - FlexIO CH0 input is selected
                     0011_0101b - FlexIO CH1 input is selected
                     0011_0110b - FlexIO CH2 input is selected
                     0011_0111b - FlexIO CH3 input is selected
                     0011_1000b - SINC Filter CH0 Conversion Complete input is selected
                     0011_1001b - SINC Filter CH1 Conversion Complete input is selected
                     0011_1010b - SINC Filter CH2 Conversion Complete input is selected
                     0011_1011b - SINC Filter CH3 Conversion Complete input is selected
                     0011_1100b - SINC Filter CH4 Conversion Complete input is selected
                     0011_1101b - GPIO2 Pin Event Trig 0 input is selected
                     0011_1110b - GPIO2 Pin Event Trig 1 input is selected
                     0011_1111b - GPIO3 Pin Event Trig 0 input is selected
                     0100_0000b - GPIO3 Pin Event Trig 1 input is selected
                               All other values are reserved.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    937 / 3791

<!-- page 938 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

26.5.1.14 DAC0 Trigger Inputs (DAC0_TRIG)

Offset


 Register                   Offset

 DAC0_TRIG            300h


Function
This register selects the DAC0 trigger inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                   TRIGIN
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


Fields


       Field       Function
      31-6      Reserved
    —

       5-0     DAC0 trigger input
    TRIGIN           00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT3 input is selected
                     00_0010b - SCT0 SCT_OUT4 input is selected
                     00_0011b - SCT0 SCT_OUT5 input is selected
                     00_0100b - SCT0 SCT_OUT0 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER2_MAT0 input is selected
                     00_1001b - CTIMER3_MAT0 input is selected
                     00_1010b - Reserved

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    938 / 3791

<!-- page 939 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     00_1011b - Reserved
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[0] input is selected
                     00_1110b - ADC1_tcomp[0] input is selected
                     00_1111b - CMP0_OUT input is selected
                     01_0000b - CMP1_OUT input is selected
                     01_0001b - CMP2_OUT input is selected
                     01_0010b - EVTG_OUT0A input is selected
                     01_0011b - EVTG_OUT0B input is selected
                     01_0100b - EVTG_OUT1A input is selected
                     01_0101b - EVTG_OUT1B input is selected
                     01_0110b - EVTG_OUT2A input is selected
                     01_0111b - EVTG_OUT2B input is selected
                     01_1000b - EVTG_OUT3A input is selected
                     01_1001b - EVTG_OUT3B input is selected
                     01_1010b - LPTMR0 input is selected
                     01_1011b - LPTMR1 input is selected
                     01_1100b - GPIO2 Pin Event Trig 0 input is selected
                     01_1101b - GPIO2 Pin Event Trig 1 input is selected
                     01_1110b - GPIO3 Pin Event Trig 0 input is selected
                     01_1111b - GPIO3 Pin Event Trig 1 input is selected
                               All other values are reserved.


26.5.1.15 DAC1 Trigger Inputs (DAC1_TRIG)

Offset


 Register                   Offset

 DAC1_TRIG            320h


Function
This register selects the DAC1 trigger inputs.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    939 / 3791

<!-- page 940 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                   TRIGIN
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


Fields


       Field       Function
      31-6      Reserved
    —

       5-0     DAC1 trigger input
    TRIGIN           00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT4 input is selected
                     00_0010b - SCT0 SCT_OUT4 input is selected
                     00_0011b - SCT0 SCT_OUT5 input is selected
                     00_0100b - SCT0 SCT_OUT1 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER2_MAT1 input is selected
                     00_1001b - CTIMER3_MAT1 input is selected
                     00_1010b - Reserved
                     00_1011b - Reserved
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[1] input is selected
                     00_1110b - ADC1_tcomp[1] input is selected
                     00_1111b - CMP0_OUT input is selected
                     01_0000b - CMP1_OUT input is selected
                     01_0001b - CMP2_OUT input is selected
                     01_0010b - EVTG_OUT0A input is selected
                     01_0011b - EVTG_OUT0B input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    940 / 3791

<!-- page 941 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_0100b - EVTG_OUT1A input is selected
                     01_0101b - EVTG_OUT1B input is selected
                     01_0110b - EVTG_OUT2A input is selected
                     01_0111b - EVTG_OUT2B input is selected
                     01_1000b - EVTG_OUT3A input is selected
                     01_1001b - EVTG_OUT3B input is selected
                     01_1010b - LPTMR0 input is selected
                     01_1011b - LPTMR1 input is selected
                     01_1100b - GPIO2 Pin Event Trig 0 input is selected
                     01_1101b - GPIO2 Pin Event Trig 1 input is selected
                     01_1110b - GPIO3 Pin Event Trig 0 input is selected
                     01_1111b - GPIO3 Pin Event Trig 1 input is selected
                               All other values are reserved.


26.5.1.16 DAC2 Trigger Inputs (DAC2_TRIG)

Offset


 Register                   Offset

 DAC2_TRIG            340h


Function
This register selects the DAC2 trigger inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                   TRIGIN
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    941 / 3791

<!-- page 942 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-6      Reserved
    —

       5-0     DAC2 trigger input
    TRIGIN           00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT5 input is selected
                     00_0010b - SCT0 SCT_OUT4 input is selected
                     00_0011b - SCT0 SCT_OUT5 input is selected
                     00_0100b - SCT0 SCT_OUT2 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER2_MAT2 input is selected
                     00_1001b - CTIMER3_MAT2 input is selected
                     00_1010b - Reserved
                     00_1011b - Reserved
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[2] input is selected
                     00_1110b - ADC1_tcomp[2] input is selected
                     00_1111b - CMP0_OUT input is selected
                     01_0000b - CMP1_OUT input is selected
                     01_0001b - CMP2_OUT input is selected
                     01_0010b - EVTG_OUT0A input is selected
                     01_0011b - EVTG_OUT0B input is selected
                     01_0100b - EVTG_OUT1A input is selected
                     01_0101b - EVTG_OUT1B input is selected
                     01_0110b - EVTG_OUT2A input is selected
                     01_0111b - EVTG_OUT2B input is selected
                     01_1000b - EVTG_OUT3A input is selected
                     01_1001b - EVTG_OUT3B input is selected
                     01_1010b - LPTMR0 input is selected
                     01_1011b - LPTMR1 input is selected
                     01_1100b - GPIO2 Pin Event Trig 0 input is selected
                     01_1101b - GPIO2 Pin Event Trig 1 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    942 / 3791

<!-- page 943 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_1110b - GPIO3 Pin Event Trig 0 input is selected
                     01_1111b - GPIO3 Pin Event Trig 1 input is selected
                               All other values are reserved.


26.5.1.17 QDC0 Trigger Input Connections (QDC0_TRIG)

Offset


 Register                   Offset

 QDC0_TRIG           360h


Function
This register selects the QDC0 trigger inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


Fields


       Field       Function
      31-6      Reserved
    —

       5-0     QDC0 trigger input connections
      INP            00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT4 input is selected
                     00_0010b - SCT_OUT4 input is selected
                     00_0011b - SCT_OUT5 input is selected

                                  Tablecontinuesonthenextpage...




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    943 / 3791

<!-- page 944 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     00_0100b - SCT_OUT1 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER1_MAT0 input is selected
                     00_1001b - CTIMER3_MAT0 input is selected
                     00_1010b - Reserved
                     00_1011b - ARM_TXEV input is selected
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[0] input is selected
                     00_1110b - ADC0_tcomp[1] input is selected
                     00_1111b - ADC0_tcomp[2] input is selected
                     01_0000b - ADC0_tcomp[3] input is selected
                     01_0001b - ADC1_tcomp[0] input is selected
                     01_0010b - ADC1_tcomp[1] input is selected
                     01_0011b - ADC1_tcomp[2] input is selected
                     01_0100b - ADC1_tcomp[3] input is selected
                     01_0101b - CMP0_OUT input is selected
                     01_0110b - CMP1_OUT input is selected
                     01_0111b - CMP2_OUT input is selected
                     01_1000b - PWM1_SM0_MUX_TRIG0 input is selected
                     01_1001b - PWM1_SM0_MUX_TRIG1 input is selected
                     01_1010b - PWM1_SM1_MUX_TRIG0 input is selected
                     01_1011b - PWM1_SM1_MUX_TRIG1 input is selected
                     01_1100b - PWM1_SM2_MUX_TRIG0 input is selected
                     01_1101b - PWM1_SM2_MUX_TRIG1 input is selected
                     01_1110b - PWM1_SM3_MUX_TRIG0 input is selected
                     01_1111b - PWM1_SM3_MUX_TRIG1 input is selected
                     10_0000b - QDC0_CMP/POS_MATCH input is selected
                     10_0001b - QDC1_CMP/POS_MATCH input is selected
                     10_0010b - EVTG_OUT0A input is selected
                     10_0011b - EVTG_OUT0B input is selected
                     10_0100b - EVTG_OUT1A input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    944 / 3791

<!-- page 945 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     10_0101b - EVTG_OUT1B input is selected
                     10_0110b - EVTG_OUT2A input is selected
                     10_0111b - EVTG_OUT2B input is selected
                     10_1000b - EVTG_OUT3A input is selected
                     10_1001b - EVTG_OUT3B input is selected
                     10_1010b - TRIG_IN0 input is selected
                     10_1011b - TRIG_IN1 input is selected
                     10_1100b - TRIG_IN2 input is selected
                     10_1101b - TRIG_IN3 input is selected
                     10_1110b - TRIG_IN4 input is selected
                     10_1111b - TRIG_IN5 input is selected
                     11_0000b - TRIG_IN6 input is selected
                     11_0001b - TRIG_IN7 input is selected
                     11_0010b - TRIG_IN8 input is selected
                     11_0011b - TRIG_IN9 input is selected
                               All other values are reserved.


26.5.1.18 QDC0 Input Connections (QDC0_HOME)

Offset


 Register                   Offset

 QDC0_HOME          364h


Function
This register selects the QDC0 HOME inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    945 / 3791

<!-- page 946 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-6      Reserved
    —

       5-0     QDC0 HOME input connections
      INP            00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT4 input is selected
                     00_0010b - SCT0 SCT_OUT4 input is selected
                     00_0011b - SCT0 SCT_OUT5 input is selected
                     00_0100b - SCT0 SCT_OUT1 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER1_MAT0 input is selected
                     00_1001b - CTIMER3_MAT0 input is selected
                     00_1010b - Reserved
                     00_1011b - ARM_TXEV input is selected
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[0] input is selected
                     00_1110b - ADC0_tcomp[1] input is selected
                     00_1111b - ADC0_tcomp[2] input is selected
                     01_0000b - ADC0_tcomp[3] input is selected
                     01_0001b - ADC1_tcomp[0] input is selected
                     01_0010b - ADC1_tcomp[1] input is selected
                     01_0011b - ADC1_tcomp[2] input is selected
                     01_0100b - ADC1_tcomp[3] input is selected
                     01_0101b - CMP0_OUT input is selected
                     01_0110b - CMP1_OUT input is selected
                     01_0111b - CMP2_OUT input is selected
                     01_1000b - PWM1_SM0_MUX_TRIG0 input is selected
                     01_1001b - PWM1_SM0_MUX_TRIG1 input is selected
                     01_1010b - PWM1_SM1_MUX_TRIG0 input is selected
                     01_1011b - PWM1_SM1_MUX_TRIG1 input is selected
                     01_1100b - PWM1_SM2_MUX_TRIG0 input is selected
                     01_1101b - PWM1_SM2_MUX_TRIG1 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    946 / 3791

<!-- page 947 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_1110b - PWM1_SM3_MUX_TRIG0 input is selected
                     01_1111b - PWM1_SM3_MUX_TRIG1 input is selected
                     10_0000b - QDC0_CMP/POS_MATCH input is selected
                     10_0001b - QDC1_CMP/POS_MATCH input is selected
                     10_0010b - EVTG_OUT0A input is selected
                     10_0011b - EVTG_OUT0B input is selected
                     10_0100b - EVTG_OUT1A input is selected
                     10_0101b - EVTG_OUT1B input is selected
                     10_0110b - EVTG_OUT2A input is selected
                     10_0111b - EVTG_OUT2B input is selected
                     10_1000b - EVTG_OUT3A input is selected
                     10_1001b - EVTG_OUT3B input is selected
                     10_1010b - TRIG_IN0 input is selected
                     10_1011b - TRIG_IN1 input is selected
                     10_1100b - TRIG_IN2 input is selected
                     10_1101b - TRIG_IN3 input is selected
                     10_1110b - TRIG_IN4 input is selected
                     10_1111b - TRIG_IN5 input is selected
                     11_0000b - TRIG_IN6 input is selected
                     11_0001b - TRIG_IN7 input is selected
                     11_0010b - TRIG_IN8 input is selected
                     11_0011b - TRIG_IN9 input is selected
                               All other values are reserved.


26.5.1.19 QDC0 Input Connections (QDC0_INDEX)

Offset


 Register                   Offset

 QDC0_INDEX          368h


Function
This register selects the QDC0 INDEX inputs.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    947 / 3791

<!-- page 948 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


Fields


       Field       Function
      31-6      Reserved
    —

       5-0     QDC0 INDEX input connections
      INP            00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT4 input is selected
                     00_0010b - SCT_OUT4 input is selected
                     00_0011b - SCT_OUT5 input is selected
                     00_0100b - SCT_OUT1 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER1_MAT0 input is selected
                     00_1001b - CTIMER3_MAT0 input is selected
                     00_1010b - Reserved
                     00_1011b - ARM_TXEV input is selected
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[0] input is selected
                     00_1110b - ADC0_tcomp[1] input is selected
                     00_1111b - ADC0_tcomp[2] input is selected
                     01_0000b - ADC0_tcomp[3] input is selected
                     01_0001b - ADC1_tcomp[0] input is selected
                     01_0010b - ADC1_tcomp[1] input is selected
                     01_0011b - ADC1_tcomp[2] input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    948 / 3791

<!-- page 949 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_0100b - ADC1_tcomp[3] input is selected
                     01_0101b - CMP0_OUT input is selected
                     01_0110b - CMP1_OUT input is selected
                     01_0111b - CMP2_OUT input is selected
                     01_1000b - PWM1_SM0_MUX_TRIG0 input is selected
                     01_1001b - PWM1_SM0_MUX_TRIG1 input is selected
                     01_1010b - PWM1_SM1_MUX_TRIG0 input is selected
                     01_1011b - PWM1_SM1_MUX_TRIG1 input is selected
                     01_1100b - PWM1_SM2_MUX_TRIG0 input is selected
                     01_1101b - PWM1_SM2_MUX_TRIG1 input is selected
                     01_1110b - PWM1_SM3_MUX_TRIG0 input is selected
                     01_1111b - PWM1_SM3_MUX_TRIG1 input is selected
                     10_0000b - QDC0_CMP/POS_MATCH input is selected
                     10_0001b - QDC1_CMP/POS_MATCH input is selected
                     10_0010b - EVTG_OUT0A input is selected
                     10_0011b - EVTG_OUT0B input is selected
                     10_0100b - EVTG_OUT1A input is selected
                     10_0101b - EVTG_OUT1B input is selected
                     10_0110b - EVTG_OUT2A input is selected
                     10_0111b - EVTG_OUT2B input is selected
                     10_1000b - EVTG_OUT3A input is selected
                     10_1001b - EVTG_OUT3B input is selected
                     10_1010b - TRIG_IN0 input is selected
                     10_1011b - TRIG_IN1 input is selected
                     10_1100b - TRIG_IN2 input is selected
                     10_1101b - TRIG_IN3 input is selected
                     10_1110b - TRIG_IN4 input is selected
                     10_1111b - TRIG_IN5 input is selected
                     11_0000b - TRIG_IN6 input is selected
                     11_0001b - TRIG_IN7 input is selected
                     11_0010b - TRIG_IN8 input is selected
                     11_0011b - TRIG_IN9 input is selected
                               All other values are reserved.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    949 / 3791

<!-- page 950 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

26.5.1.20 QDC0 Input Connections (QDC0_PHASEB)

Offset


 Register                   Offset

 QDC0_PHASEB        36Ch


Function
This register selects the QDC0 PHASEB inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


Fields


       Field       Function
      31-6      Reserved
    —

       5-0     QDC0 PHASEB input connections
      INP            00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT4 input is selected
                     00_0010b - SCT_OUT4 input is selected
                     00_0011b - SCT_OUT5 input is selected
                     00_0100b - SCT_OUT1 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER1_MAT0 input is selected
                     00_1001b - CTIMER3_MAT0 input is selected
                     00_1010b - Reserved

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    950 / 3791

<!-- page 951 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     00_1011b - ARM_TXEV input is selected
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[0] input is selected
                     00_1110b - ADC0_tcomp[1] input is selected
                     00_1111b - ADC0_tcomp[2] input is selected
                     01_0000b - ADC0_tcomp[3] input is selected
                     01_0001b - ADC1_tcomp[0] input is selected
                     01_0010b - ADC1_tcomp[1] input is selected
                     01_0011b - ADC1_tcomp[2] input is selected
                     01_0100b - ADC1_tcomp[3] input is selected
                     01_0101b - CMP0_OUT input is selected
                     01_0110b - CMP1_OUT input is selected
                     01_0111b - CMP2_OUT input is selected
                     01_1000b - PWM1_SM0_MUX_TRIG0 input is selected
                     01_1001b - PWM1_SM0_MUX_TRIG1 input is selected
                     01_1010b - PWM1_SM1_MUX_TRIG0 input is selected
                     01_1011b - PWM1_SM1_MUX_TRIG1 input is selected
                     01_1100b - PWM1_SM2_MUX_TRIG0 input is selected
                     01_1101b - PWM1_SM2_MUX_TRIG1 input is selected
                     01_1110b - PWM1_SM3_MUX_TRIG0 input is selected
                     01_1111b - PWM1_SM3_MUX_TRIG1 input is selected
                     10_0000b - QDC0_CMP/POS_MATCH input is selected
                     10_0001b - QDC1_CMP/POS_MATCH input is selected
                     10_0010b - EVTG_OUT0A input is selected
                     10_0011b - EVTG_OUT0B input is selected
                     10_0100b - EVTG_OUT1A input is selected
                     10_0101b - EVTG_OUT1B input is selected
                     10_0110b - EVTG_OUT2A input is selected
                     10_0111b - EVTG_OUT2B input is selected
                     10_1000b - EVTG_OUT3A input is selected
                     10_1001b - EVTG_OUT3B input is selected
                     10_1010b - TRIG_IN0 input is selected
                     10_1011b - TRIG_IN1 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    951 / 3791

<!-- page 952 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     10_1100b - TRIG_IN2 input is selected
                     10_1101b - TRIG_IN3 input is selected
                     10_1110b - TRIG_IN4 input is selected
                     10_1111b - TRIG_IN5 input is selected
                     11_0000b - TRIG_IN6 input is selected
                     11_0001b - TRIG_IN7 input is selected
                     11_0010b - TRIG_IN8 input is selected
                     11_0011b - TRIG_IN9 input is selected
                               All other values are reserved.


26.5.1.21 QDC0 Input Connections (QDC0_PHASEA)

Offset


 Register                   Offset

 QDC0_PHASEA        370h


Function
This register selects the QDC0 PHASEA inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


Fields


       Field       Function
      31-6      Reserved
    —

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    952 / 3791

<!-- page 953 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       5-0     QDC0 PHASEA input connections
      INP            00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT4 input is selected
                     00_0010b - SCT_OUT4 input is selected
                     00_0011b - SCT_OUT5 input is selected
                     00_0100b - SCT_OUT1 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER1_MAT0 input is selected
                     00_1001b - CTIMER3_MAT0 input is selected
                     00_1010b - Reserved
                     00_1011b - ARM_TXEV input is selected
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[0] input is selected
                     00_1110b - ADC0_tcomp[1] input is selected
                     00_1111b - ADC0_tcomp[2] input is selected
                     01_0000b - ADC0_tcomp[3] input is selected
                     01_0001b - ADC1_tcomp[0] input is selected
                     01_0010b - ADC1_tcomp[1] input is selected
                     01_0011b - ADC1_tcomp[2] input is selected
                     01_0100b - ADC1_tcomp[3] input is selected
                     01_0101b - CMP0_OUT input is selected
                     01_0110b - CMP1_OUT input is selected
                     01_0111b - CMP2_OUT input is selected
                     01_1000b - PWM1_SM0_MUX_TRIG0 input is selected
                     01_1001b - PWM1_SM0_MUX_TRIG1 input is selected
                     01_1010b - PWM1_SM1_MUX_TRIG0 input is selected
                     01_1011b - PWM1_SM1_MUX_TRIG1 input is selected
                     01_1100b - PWM1_SM2_MUX_TRIG0 input is selected
                     01_1101b - PWM1_SM2_MUX_TRIG1 input is selected
                     01_1110b - PWM1_SM3_MUX_TRIG0 input is selected
                     01_1111b - PWM1_SM3_MUX_TRIG1 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    953 / 3791

<!-- page 954 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     10_0000b - QDC0_CMP/POS_MATCH input is selected
                     10_0001b - QDC1_CMP/POS_MATCH input is selected
                     10_0010b - EVTG_OUT0A input is selected
                     10_0011b - EVTG_OUT0B input is selected
                     10_0100b - EVTG_OUT1A input is selected
                     10_0101b - EVTG_OUT1B input is selected
                     10_0110b - EVTG_OUT2A input is selected
                     10_0111b - EVTG_OUT2B input is selected
                     10_1000b - EVTG_OUT3A input is selected
                     10_1001b - EVTG_OUT3B input is selected
                     10_1010b - TRIG_IN0 input is selected
                     10_1011b - TRIG_IN1 input is selected
                     10_1100b - TRIG_IN2 input is selected
                     10_1101b - TRIG_IN3 input is selected
                     10_1110b - TRIG_IN4 input is selected
                     10_1111b - TRIG_IN5 input is selected
                     11_0000b - TRIG_IN6 input is selected
                     11_0001b - TRIG_IN7 input is selected
                     11_0010b - TRIG_IN8 input is selected
                     11_0011b - TRIG_IN9 input is selected
                               All other values are reserved.


26.5.1.22 QDC1 Trigger Input Connections (QDC1_TRIG)

Offset


 Register                   Offset

 QDC1_TRIG           380h


Function
This register selects the QDC1 trigger inputs.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    954 / 3791

<!-- page 955 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


Fields


       Field       Function
      31-6      Reserved
    —

       5-0     QDC1 trigger input connections
      INP            00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT4 input is selected
                     00_0010b - SCT_OUT4 input is selected
                     00_0011b - SCT_OUT5 input is selected
                     00_0100b - SCT_OUT1 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER1_MAT0 input is selected
                     00_1001b - CTIMER3_MAT0 input is selected
                     00_1010b - Reserved
                     00_1011b - ARM_TXEV input is selected
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[0] input is selected
                     00_1110b - ADC0_tcomp[1] input is selected
                     00_1111b - ADC0_tcomp[2] input is selected
                     01_0000b - ADC0_tcomp[3] input is selected
                     01_0001b - ADC1_tcomp[0] input is selected
                     01_0010b - ADC1_tcomp[1] input is selected
                     01_0011b - ADC1_tcomp[2] input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    955 / 3791

<!-- page 956 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_0100b - ADC1_tcomp[3] input is selected
                     01_0101b - CMP0_OUT input is selected
                     01_0110b - CMP1_OUT input is selected
                     01_0111b - CMP2_OUT input is selected
                     01_1000b - PWM1_SM0_MUX_TRIG0 input is selected
                     01_1001b - PWM1_SM0_MUX_TRIG1 input is selected
                     01_1010b - PWM1_SM1_MUX_TRIG0 input is selected
                     01_1011b - PWM1_SM1_MUX_TRIG1 input is selected
                     01_1100b - PWM1_SM2_MUX_TRIG0 input is selected
                     01_1101b - PWM1_SM2_MUX_TRIG1 input is selected
                     01_1110b - PWM1_SM3_MUX_TRIG0 input is selected
                     01_1111b - PWM1_SM3_MUX_TRIG1 input is selected
                     10_0000b - QDC0_CMP/POS_MATCH input is selected
                     10_0001b - QDC1_CMP/POS_MATCH input is selected
                     10_0010b - EVTG_OUT0A input is selected
                     10_0011b - EVTG_OUT0B input is selected
                     10_0100b - EVTG_OUT1A input is selected
                     10_0101b - EVTG_OUT1B input is selected
                     10_0110b - EVTG_OUT2A input is selected
                     10_0111b - EVTG_OUT2B input is selected
                     10_1000b - EVTG_OUT3A input is selected
                     10_1001b - EVTG_OUT3B input is selected
                     10_1010b - TRIG_IN0 input is selected
                     10_1011b - TRIG_IN1 input is selected
                     10_1100b - TRIG_IN2 input is selected
                     10_1101b - TRIG_IN3 input is selected
                     10_1110b - TRIG_IN4 input is selected
                     10_1111b - TRIG_IN5 input is selected
                     11_0000b - TRIG_IN6 input is selected
                     11_0001b - TRIG_IN7 input is selected
                     11_0010b - TRIG_IN8 input is selected
                     11_0011b - TRIG_IN9 input is selected
                               All other values are reserved.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    956 / 3791

<!-- page 957 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

26.5.1.23 QDC1 Input Connections (QDC1_HOME)

Offset


 Register                   Offset

 QDC1_HOME          384h


Function
This register selects the QDC1 HOME inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


Fields


       Field       Function
      31-6      Reserved
    —

       5-0     QDC1 HOME input connections
      INP            00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT4 input is selected
                     00_0010b - SCT0 SCT_OUT4 input is selected
                     00_0011b - SCT0 SCT_OUT5 input is selected
                     00_0100b - SCT0 SCT_OUT1 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER1_MAT0 input is selected
                     00_1001b - CTIMER3_MAT0 input is selected
                     00_1010b - Reserved

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    957 / 3791

<!-- page 958 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     00_1011b - ARM_TXEV input is selected
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[0] input is selected
                     00_1110b - ADC0_tcomp[1] input is selected
                     00_1111b - ADC0_tcomp[2] input is selected
                     01_0000b - ADC0_tcomp[3] input is selected
                     01_0001b - ADC1_tcomp[0] input is selected
                     01_0010b - ADC1_tcomp[1] input is selected
                     01_0011b - ADC1_tcomp[2] input is selected
                     01_0100b - ADC1_tcomp[3] input is selected
                     01_0101b - CMP0_OUT input is selected
                     01_0110b - CMP1_OUT input is selected
                     01_0111b - CMP2_OUT input is selected
                     01_1000b - PWM1_SM0_MUX_TRIG0 input is selected
                     01_1001b - PWM1_SM0_MUX_TRIG1 input is selected
                     01_1010b - PWM1_SM1_MUX_TRIG0 input is selected
                     01_1011b - PWM1_SM1_MUX_TRIG1 input is selected
                     01_1100b - PWM1_SM2_MUX_TRIG0 input is selected
                     01_1101b - PWM1_SM2_MUX_TRIG1 input is selected
                     01_1110b - PWM1_SM3_MUX_TRIG0 input is selected
                     01_1111b - PWM1_SM3_MUX_TRIG1 input is selected
                     10_0000b - QDC0_CMP/POS_MATCH input is selected
                     10_0001b - QDC1_CMP/POS_MATCH input is selected
                     10_0010b - EVTG_OUT0A input is selected
                     10_0011b - EVTG_OUT0B input is selected
                     10_0100b - EVTG_OUT1A input is selected
                     10_0101b - EVTG_OUT1B input is selected
                     10_0110b - EVTG_OUT2A input is selected
                     10_0111b - EVTG_OUT2B input is selected
                     10_1000b - EVTG_OUT3A input is selected
                     10_1001b - EVTG_OUT3B input is selected
                     10_1010b - TRIG_IN0 input is selected
                     10_1011b - TRIG_IN1 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    958 / 3791

<!-- page 959 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     10_1100b - TRIG_IN2 input is selected
                     10_1101b - TRIG_IN3 input is selected
                     10_1110b - TRIG_IN4 input is selected
                     10_1111b - TRIG_IN5 input is selected
                     11_0000b - TRIG_IN6 input is selected
                     11_0001b - TRIG_IN7 input is selected
                     11_0010b - TRIG_IN8 input is selected
                     11_0011b - TRIG_IN9 input is selected
                               All other values are reserved.


26.5.1.24 QDC1 Input Connections (QDC1_INDEX)

Offset


 Register                   Offset

 QDC1_INDEX          388h


Function
This register selects the QDC1 INDEX inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


Fields


       Field       Function
      31-6      Reserved
    —

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    959 / 3791

<!-- page 960 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       5-0     QDC1 INDEX input connections
      INP            00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT4 input is selected
                     00_0010b - SCT_OUT4 input is selected
                     00_0011b - SCT_OUT5 input is selected
                     00_0100b - SCT_OUT1 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER1_MAT0 input is selected
                     00_1001b - CTIMER3_MAT0 input is selected
                     00_1010b - Reserved
                     00_1011b - ARM_TXEV input is selected
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[0] input is selected
                     00_1110b - ADC0_tcomp[1] input is selected
                     00_1111b - ADC0_tcomp[2] input is selected
                     01_0000b - ADC0_tcomp[3] input is selected
                     01_0001b - ADC1_tcomp[0] input is selected
                     01_0010b - ADC1_tcomp[1] input is selected
                     01_0011b - ADC1_tcomp[2] input is selected
                     01_0100b - ADC1_tcomp[3] input is selected
                     01_0101b - CMP0_OUT input is selected
                     01_0110b - CMP1_OUT input is selected
                     01_0111b - CMP2_OUT input is selected
                     01_1000b - PWM1_SM0_MUX_TRIG0 input is selected
                     01_1001b - PWM1_SM0_MUX_TRIG1 input is selected
                     01_1010b - PWM1_SM1_MUX_TRIG0 input is selected
                     01_1011b - PWM1_SM1_MUX_TRIG1 input is selected
                     01_1100b - PWM1_SM2_MUX_TRIG0 input is selected
                     01_1101b - PWM1_SM2_MUX_TRIG1 input is selected
                     01_1110b - PWM1_SM3_MUX_TRIG0 input is selected
                     01_1111b - PWM1_SM3_MUX_TRIG1 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    960 / 3791

<!-- page 961 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     10_0000b - QDC0_CMP/POS_MATCH input is selected
                     10_0001b - QDC1_CMP/POS_MATCH input is selected
                     10_0010b - EVTG_OUT0A input is selected
                     10_0011b - EVTG_OUT0B input is selected
                     10_0100b - EVTG_OUT1A input is selected
                     10_0101b - EVTG_OUT1B input is selected
                     10_0110b - EVTG_OUT2A input is selected
                     10_0111b - EVTG_OUT2B input is selected
                     10_1000b - EVTG_OUT3A input is selected
                     10_1001b - EVTG_OUT3B input is selected
                     10_1010b - TRIG_IN0 input is selected
                     10_1011b - TRIG_IN1 input is selected
                     10_1100b - TRIG_IN2 input is selected
                     10_1101b - TRIG_IN3 input is selected
                     10_1110b - TRIG_IN4 input is selected
                     10_1111b - TRIG_IN5 input is selected
                     11_0000b - TRIG_IN6 input is selected
                     11_0001b - TRIG_IN7 input is selected
                     11_0010b - TRIG_IN8 input is selected
                     11_0011b - TRIG_IN9 input is selected
                               All other values are reserved.


26.5.1.25 QDC1 Input Connections (QDC1_PHASEB)

Offset


 Register                   Offset

 QDC1_PHASEB        38Ch


Function
This register selects the QDC1 PHASEB inputs.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    961 / 3791

<!-- page 962 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


Fields


       Field       Function
      31-6      Reserved
    —

       5-0     QDC1 PHASEB input connections
      INP            00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT4 input is selected
                     00_0010b - SCT_OUT4 input is selected
                     00_0011b - SCT_OUT5 input is selected
                     00_0100b - SCT_OUT1 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER1_MAT0 input is selected
                     00_1001b - CTIMER3_MAT0 input is selected
                     00_1010b - Reserved
                     00_1011b - ARM_TXEV input is selected
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[0] input is selected
                     00_1110b - ADC0_tcomp[1] input is selected
                     00_1111b - ADC0_tcomp[2] input is selected
                     01_0000b - ADC0_tcomp[3] input is selected
                     01_0001b - ADC1_tcomp[0] input is selected
                     01_0010b - ADC1_tcomp[1] input is selected
                     01_0011b - ADC1_tcomp[2] input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    962 / 3791

<!-- page 963 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_0100b - ADC1_tcomp[3] input is selected
                     01_0101b - CMP0_OUT input is selected
                     01_0110b - CMP1_OUT input is selected
                     01_0111b - CMP2_OUT input is selected
                     01_1000b - PWM1_SM0_MUX_TRIG0 input is selected
                     01_1001b - PWM1_SM0_MUX_TRIG1 input is selected
                     01_1010b - PWM1_SM1_MUX_TRIG0 input is selected
                     01_1011b - PWM1_SM1_MUX_TRIG1 input is selected
                     01_1100b - PWM1_SM2_MUX_TRIG0 input is selected
                     01_1101b - PWM1_SM2_MUX_TRIG1 input is selected
                     01_1110b - PWM1_SM3_MUX_TRIG0 input is selected
                     01_1111b - PWM1_SM3_MUX_TRIG1 input is selected
                     10_0000b - QDC0_CMP/POS_MATCH input is selected
                     10_0001b - QDC1_CMP/POS_MATCH input is selected
                     10_0010b - EVTG_OUT0A input is selected
                     10_0011b - EVTG_OUT0B input is selected
                     10_0100b - EVTG_OUT1A input is selected
                     10_0101b - EVTG_OUT1B input is selected
                     10_0110b - EVTG_OUT2A input is selected
                     10_0111b - EVTG_OUT2B input is selected
                     10_1000b - EVTG_OUT3A input is selected
                     10_1001b - EVTG_OUT3B input is selected
                     10_1010b - TRIG_IN0 input is selected
                     10_1011b - TRIG_IN1 input is selected
                     10_1100b - TRIG_IN2 input is selected
                     10_1101b - TRIG_IN3 input is selected
                     10_1110b - TRIG_IN4 input is selected
                     10_1111b - TRIG_IN5 input is selected
                     11_0000b - TRIG_IN6 input is selected
                     11_0001b - TRIG_IN7 input is selected
                     11_0010b - TRIG_IN8 input is selected
                     11_0011b - TRIG_IN9 input is selected
                               All other values are reserved.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    963 / 3791

<!-- page 964 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

26.5.1.26 QDC1 Input Connections (QDC1_PHASEA)

Offset


 Register                   Offset

 QDC1_PHASEA        390h


Function
This register selects the QDC1 PHASEA inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


Fields


       Field       Function
      31-6      Reserved
    —

       5-0     QDC1 PHASEA input connections
      INP            00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT4 input is selected
                     00_0010b - SCT_OUT4 input is selected
                     00_0011b - SCT_OUT5 input is selected
                     00_0100b - SCT_OUT1 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER1_MAT0 input is selected
                     00_1001b - CTIMER3_MAT0 input is selected
                     00_1010b - Reserved

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    964 / 3791

<!-- page 965 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     00_1011b - ARM_TXEV input is selected
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[0] input is selected
                     00_1110b - ADC0_tcomp[1] input is selected
                     00_1111b - ADC0_tcomp[2] input is selected
                     01_0000b - ADC0_tcomp[3] input is selected
                     01_0001b - ADC1_tcomp[0] input is selected
                     01_0010b - ADC1_tcomp[1] input is selected
                     01_0011b - ADC1_tcomp[2] input is selected
                     01_0100b - ADC1_tcomp[3] input is selected
                     01_0101b - CMP0_OUT input is selected
                     01_0110b - CMP1_OUT input is selected
                     01_0111b - CMP2_OUT input is selected
                     01_1000b - PWM1_SM0_MUX_TRIG0 input is selected
                     01_1001b - PWM1_SM0_MUX_TRIG1 input is selected
                     01_1010b - PWM1_SM1_MUX_TRIG0 input is selected
                     01_1011b - PWM1_SM1_MUX_TRIG1 input is selected
                     01_1100b - PWM1_SM2_MUX_TRIG0 input is selected
                     01_1101b - PWM1_SM2_MUX_TRIG1 input is selected
                     01_1110b - PWM1_SM3_MUX_TRIG0 input is selected
                     01_1111b - PWM1_SM3_MUX_TRIG1 input is selected
                     10_0000b - QDC0_CMP/POS_MATCH input is selected
                     10_0001b - QDC1_CMP/POS_MATCH input is selected
                     10_0010b - EVTG_OUT0A input is selected
                     10_0011b - EVTG_OUT0B input is selected
                     10_0100b - EVTG_OUT1A input is selected
                     10_0101b - EVTG_OUT1B input is selected
                     10_0110b - EVTG_OUT2A input is selected
                     10_0111b - EVTG_OUT2B input is selected
                     10_1000b - EVTG_OUT3A input is selected
                     10_1001b - EVTG_OUT3B input is selected
                     10_1010b - TRIG_IN0 input is selected
                     10_1011b - TRIG_IN1 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    965 / 3791

<!-- page 966 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     10_1100b - TRIG_IN2 input is selected
                     10_1101b - TRIG_IN3 input is selected
                     10_1110b - TRIG_IN4 input is selected
                     10_1111b - TRIG_IN5 input is selected
                     11_0000b - TRIG_IN6 input is selected
                     11_0001b - TRIG_IN7 input is selected
                     11_0010b - TRIG_IN8 input is selected
                     11_0011b - TRIG_IN9 input is selected
                               All other values are reserved.


26.5.1.27 PWM0 External Synchronization (FlexPWM0_SM0_EXTSYNC - FlexPWM0_SM3_EXTSYNC)

Offset


 Register                   Offset

 FlexPWM0_SM0_EXTSY 3A0h
 NC

 FlexPWM0_SM1_EXTSY 3A4h
 NC

 FlexPWM0_SM2_EXTSY 3A8h
 NC

 FlexPWM0_SM3_EXTSY 3ACh
 NC


Function
This register selects the PWM0 EXTSYNC inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                   TRIGIN
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    966 / 3791

<!-- page 967 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-6      Reserved
    —

       5-0     EXTSYNC input connections for PWM0
    TRIGIN           00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT5 input is selected
                     00_0010b - SCT_OUT4 input is selected
                     00_0011b - SCT_OUT5 input is selected
                     00_0100b - SCT_OUT2 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER2_MAT0 input is selected
                     00_1001b - CTIMER4_MAT0 input is selected
                     00_1010b - Reserved
                     00_1011b - ARM_TXEV input is selected
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[0] input is selected
                     00_1110b - ADC0_tcomp[1] input is selected
                     00_1111b - ADC0_tcomp[2] input is selected
                     01_0000b - ADC0_tcomp[3] input is selected
                     01_0001b - ADC1_tcomp[0] input is selected
                     01_0010b - ADC1_tcomp[1] input is selected
                     01_0011b - ADC1_tcomp[2] input is selected
                     01_0100b - ADC1_tcomp[3] input is selected
                     01_0101b - CMP0_OUT input is selected
                     01_0110b - CMP1_OUT input is selected
                     01_0111b - CMP2_OUT input is selected
                     01_1000b - PWM1_SM0_MUX_TRIG0 input is selected
                     01_1001b - PWM1_SM0_MUX_TRIG1 input is selected
                     01_1010b - PWM1_SM1_MUX_TRIG0 input is selected
                     01_1011b - PWM1_SM1_MUX_TRIG1 input is selected
                     01_1100b - PWM1_SM2_MUX_TRIG0 input is selected
                     01_1101b - PWM1_SM2_MUX_TRIG1 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    967 / 3791

<!-- page 968 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_1110b - PWM1_SM3_MUX_TRIG0 input is selected
                     01_1111b - PWM1_SM3_MUX_TRIG1 input is selected
                     10_0000b - QDC0_CMP/POS_MATCH input is selected
                     10_0001b - QDC1_CMP/POS_MATCH input is selected
                     10_0010b - EVTG_OUT0A input is selected
                     10_0011b - EVTG_OUT0B input is selected
                     10_0100b - EVTG_OUT1A input is selected
                     10_0101b - EVTG_OUT1B input is selected
                     10_0110b - EVTG_OUT2A input is selected
                     10_0111b - EVTG_OUT2B input is selected
                     10_1000b - EVTG_OUT3A input is selected
                     10_1001b - EVTG_OUT3B input is selected
                     10_1010b - TRIG_IN0 input is selected
                     10_1011b - TRIG_IN1 input is selected
                     10_1100b - TRIG_IN2 input is selected
                     10_1101b - TRIG_IN3 input is selected
                     10_1110b - TRIG_IN4 input is selected
                     10_1111b - TRIG_IN5 input is selected
                     11_0000b - TRIG_IN6 input is selected
                     11_0001b - TRIG_IN7 input is selected
                     11_0010b - TRIG_IN8 input is selected
                     11_0011b - TRIG_IN9 input is selected
                     11_0100b - SINC Filter CH0 sync Break input is selected
                     11_0101b - SINC Filter CH1 sync Break input is selected
                     11_0110b - SINC Filter CH2 sync Break input is selected
                     11_0111b - SINC Filter CH3 sync Break input is selected
                     11_1000b - SINC Filter CH4 sync Break input is selected
                     11_1001b - GPIO2 Pin Event Trig 0 input is selected
                     11_1010b - GPIO2 Pin Event Trig 1 input is selected
                     11_1011b - GPIO3 Pin Event Trig 0 input is selected
                     11_1100b - GPIO3 Pin Event Trig 1 input is selected
                               All other values are reserved.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    968 / 3791

<!-- page 969 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

26.5.1.28 PWM0 Input Trigger Connections (FlexPWM0_SM0_EXTA - FlexPWM0_SM3_EXTA)

Offset


 Register                   Offset

 FlexPWM0_SM0_EXTA  3B0h

 FlexPWM0_SM1_EXTA  3B4h

 FlexPWM0_SM2_EXTA  3B8h

 FlexPWM0_SM3_EXTA  3BCh


Function
This register selects the PWM0 EXTA inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                   TRIGIN
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


Fields


       Field       Function
      31-6      Reserved
    —

       5-0      EXTA input connections for PWM0
    TRIGIN           00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT5 input is selected
                     00_0010b - SCT_OUT4 input is selected
                     00_0011b - SCT_OUT5 input is selected
                     00_0100b - SCT_OUT2 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    969 / 3791

<!-- page 970 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     00_1000b - CTIMER2_MAT0 input is selected
                     00_1001b - CTIMER4_MAT0 input is selected
                     00_1010b - Reserved
                     00_1011b - ARM_TXEV input is selected
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[0] input is selected
                     00_1110b - ADC0_tcomp[1] input is selected
                     00_1111b - ADC0_tcomp[2] input is selected
                     01_0000b - ADC0_tcomp[3] input is selected
                     01_0001b - ADC1_tcomp[0] input is selected
                     01_0010b - ADC1_tcomp[1] input is selected
                     01_0011b - ADC1_tcomp[2] input is selected
                     01_0100b - ADC1_tcomp[3] input is selected
                     01_0101b - CMP0_OUT input is selected
                     01_0110b - CMP1_OUT input is selected
                     01_0111b - CMP2_OUT input is selected
                     01_1000b - PWM1_SM0_MUX_TRIG0 input is selected
                     01_1001b - PWM1_SM0_MUX_TRIG1 input is selected
                     01_1010b - PWM1_SM1_MUX_TRIG0 input is selected
                     01_1011b - PWM1_SM1_MUX_TRIG1 input is selected
                     01_1100b - PWM1_SM2_MUX_TRIG0 input is selected
                     01_1101b - PWM1_SM2_MUX_TRIG1 input is selected
                     01_1110b - PWM1_SM3_MUX_TRIG0 input is selected
                     01_1111b - PWM1_SM3_MUX_TRIG1 input is selected
                     10_0000b - QDC0_CMP/POS_MATCH input is selected
                     10_0001b - QDC1_CMP/POS_MATCH input is selected
                     10_0010b - EVTG_OUT0A input is selected
                     10_0011b - EVTG_OUT0B input is selected
                     10_0100b - EVTG_OUT1A input is selected
                     10_0101b - EVTG_OUT1B input is selected
                     10_0110b - EVTG_OUT2A input is selected
                     10_0111b - EVTG_OUT2B input is selected
                     10_1000b - EVTG_OUT3A input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    970 / 3791

<!-- page 971 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     10_1001b - EVTG_OUT3B input is selected
                     10_1010b - TRIG_IN0 input is selected
                     10_1011b - TRIG_IN1 input is selected
                     10_1100b - TRIG_IN2 input is selected
                     10_1101b - TRIG_IN3 input is selected
                     10_1110b - TRIG_IN4 input is selected
                     10_1111b - TRIG_IN5 input is selected
                     11_0000b - TRIG_IN6 input is selected
                     11_0001b - TRIG_IN7 input is selected
                     11_0010b - TRIG_IN8 input is selected
                     11_0011b - TRIG_IN9 input is selected
                     11_0100b - SINC Filter CH0 sync Break input is selected
                     11_0101b - SINC Filter CH1 sync Break input is selected
                     11_0110b - SINC Filter CH2 sync Break input is selected
                     11_0111b - SINC Filter CH3 sync Break input is selected
                     11_1000b - SINC Filter CH4 sync Break input is selected
                     11_1001b - GPIO2 Pin Event Trig 0 input is selected
                     11_1010b - GPIO2 Pin Event Trig 1 input is selected
                     11_1011b - GPIO3 Pin Event Trig 0 input is selected
                     11_1100b - GPIO3 Pin Event Trig 1 input is selected
                               All other values are reserved.


26.5.1.29 PWM0 External Force Trigger Connections (FlexPWM0_EXTFORCE)

Offset


 Register                   Offset

 FlexPWM0_EXTFORCE  3C0h


Function
This register selects the PWM0 EXTFORCE inputs.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    971 / 3791

<!-- page 972 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                   TRIGIN
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


Fields


       Field       Function
      31-6      Reserved
    —

       5-0     EXTFORCE input connections for PWM0
    TRIGIN           00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT5 input is selected
                     00_0010b - SCT_OUT4 input is selected
                     00_0011b - SCT_OUT5 input is selected
                     00_0100b - SCT_OUT2 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER2_MAT0 input is selected
                     00_1001b - CTIMER4_MAT0 input is selected
                     00_1010b - Reserved
                     00_1011b - ARM_TXEV input is selected
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[0] input is selected
                     00_1110b - ADC0_tcomp[1] input is selected
                     00_1111b - ADC0_tcomp[2] input is selected
                     01_0000b - ADC0_tcomp[3] input is selected
                     01_0001b - ADC1_tcomp[0] input is selected
                     01_0010b - ADC1_tcomp[1] input is selected
                     01_0011b - ADC1_tcomp[2] input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    972 / 3791

<!-- page 973 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_0100b - ADC1_tcomp[3] input is selected
                     01_0101b - CMP0_OUT input is selected
                     01_0110b - CMP1_OUT input is selected
                     01_0111b - CMP2_OUT input is selected
                     01_1000b - PWM1_SM0_MUX_TRIG0 input is selected
                     01_1001b - PWM1_SM0_MUX_TRIG1 input is selected
                     01_1010b - PWM1_SM1_MUX_TRIG0 input is selected
                     01_1011b - PWM1_SM1_MUX_TRIG1 input is selected
                     01_1100b - PWM1_SM2_MUX_TRIG0 input is selected
                     01_1101b - PWM1_SM2_MUX_TRIG1 input is selected
                     01_1110b - PWM1_SM3_MUX_TRIG0 input is selected
                     01_1111b - PWM1_SM3_MUX_TRIG1 input is selected
                     10_0000b - QDC0_CMP/POS_MATCH input is selected
                     10_0001b - QDC1_CMP/POS_MATCH input is selected
                     10_0010b - EVTG_OUT0A input is selected
                     10_0011b - EVTG_OUT0B input is selected
                     10_0100b - EVTG_OUT1A input is selected
                     10_0101b - EVTG_OUT1B input is selected
                     10_0110b - EVTG_OUT2A input is selected
                     10_0111b - EVTG_OUT2B input is selected
                     10_1000b - EVTG_OUT3A input is selected
                     10_1001b - EVTG_OUT3B input is selected
                     10_1010b - TRIG_IN0 input is selected
                     10_1011b - TRIG_IN1 input is selected
                     10_1100b - TRIG_IN2 input is selected
                     10_1101b - TRIG_IN3 input is selected
                     10_1110b - TRIG_IN4 input is selected
                     10_1111b - TRIG_IN5 input is selected
                     11_0000b - TRIG_IN6 input is selected
                     11_0001b - TRIG_IN7 input is selected
                     11_0010b - TRIG_IN8 input is selected
                     11_0011b - TRIG_IN9 input is selected
                     11_0100b - SINC Filter CH0 sync Break input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    973 / 3791

<!-- page 974 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     11_0101b - SINC Filter CH1 sync Break input is selected
                     11_0110b - SINC Filter CH2 sync Break input is selected
                     11_0111b - SINC Filter CH3 sync Break input is selected
                     11_1000b - SINC Filter CH4 sync Break input is selected
                     11_1001b - GPIO2 Pin Event Trig 0 input is selected
                     11_1010b - GPIO2 Pin Event Trig 1 input is selected
                     11_1011b - GPIO3 Pin Event Trig 0 input is selected
                     11_1100b - GPIO3 Pin Event Trig 1 input is selected
                               All other values are reserved.


26.5.1.30 PWM0 Fault Input Trigger Connections (FlexPWM0_FAULT0 - FlexPWM0_FAULT3)

Offset


 Register                   Offset

 FlexPWM0_FAULT0     3C4h

 FlexPWM0_FAULT1     3C8h

 FlexPWM0_FAULT2    3CCh

 FlexPWM0_FAULT3     3D0h


Function
This register selects the PWM0 FAULT inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                   TRIGIN
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    974 / 3791

<!-- page 975 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-6      Reserved
    —

       5-0      FAULT input connections for PWM0
    TRIGIN           00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT5 input is selected
                     00_0010b - SCT_OUT4 input is selected
                     00_0011b - SCT_OUT5 input is selected
                     00_0100b - SCT_OUT2 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER2_MAT0 input is selected
                     00_1001b - CTIMER4_MAT0 input is selected
                     00_1010b - Reserved
                     00_1011b - ARM_TXEV input is selected
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[0] input is selected
                     00_1110b - ADC0_tcomp[1] input is selected
                     00_1111b - ADC0_tcomp[2] input is selected
                     01_0000b - ADC0_tcomp[3] input is selected
                     01_0001b - ADC1_tcomp[0] input is selected
                     01_0010b - ADC1_tcomp[1] input is selected
                     01_0011b - ADC1_tcomp[2] input is selected
                     01_0100b - ADC1_tcomp[3] input is selected
                     01_0101b - CMP0_OUT input is selected
                     01_0110b - CMP1_OUT input is selected
                     01_0111b - CMP2_OUT input is selected
                     01_1000b - PWM1_SM0_MUX_TRIG0 input is selected
                     01_1001b - PWM1_SM0_MUX_TRIG1 input is selected
                     01_1010b - PWM1_SM1_MUX_TRIG0 input is selected
                     01_1011b - PWM1_SM1_MUX_TRIG1 input is selected
                     01_1100b - PWM1_SM2_MUX_TRIG0 input is selected
                     01_1101b - PWM1_SM2_MUX_TRIG1 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    975 / 3791

<!-- page 976 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_1110b - PWM1_SM3_MUX_TRIG0 input is selected
                     01_1111b - PWM1_SM3_MUX_TRIG1 input is selected
                     10_0000b - QDC0_CMP/POS_MATCH input is selected
                     10_0001b - QDC1_CMP/POS_MATCH input is selected
                     10_0010b - EVTG_OUT0A input is selected
                     10_0011b - EVTG_OUT0B input is selected
                     10_0100b - EVTG_OUT1A input is selected
                     10_0101b - EVTG_OUT1B input is selected
                     10_0110b - EVTG_OUT2A input is selected
                     10_0111b - EVTG_OUT2B input is selected
                     10_1000b - EVTG_OUT3A input is selected
                     10_1001b - EVTG_OUT3B input is selected
                     10_1010b - TRIG_IN0 input is selected
                     10_1011b - TRIG_IN1 input is selected
                     10_1100b - TRIG_IN2 input is selected
                     10_1101b - TRIG_IN3 input is selected
                     10_1110b - TRIG_IN4 input is selected
                     10_1111b - TRIG_IN5 input is selected
                     11_0000b - TRIG_IN6 input is selected
                     11_0001b - TRIG_IN7 input is selected
                     11_0010b - TRIG_IN8 input is selected
                     11_0011b - TRIG_IN9 input is selected
                     11_0100b - SINC Filter CH0 sync Break input is selected
                     11_0101b - SINC Filter CH1 sync Break input is selected
                     11_0110b - SINC Filter CH2 sync Break input is selected
                     11_0111b - SINC Filter CH3 sync Break input is selected
                     11_1000b - SINC Filter CH4 sync Break input is selected
                     11_1001b - GPIO2 Pin Event Trig 0 input is selected
                     11_1010b - GPIO2 Pin Event Trig 1 input is selected
                     11_1011b - GPIO3 Pin Event Trig 0 input is selected
                     11_1100b - GPIO3 Pin Event Trig 1 input is selected
                               All other values are reserved.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    976 / 3791

<!-- page 977 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

26.5.1.31 PWM1 External Synchronization (FlexPWM1_SM0_EXTSYNC - FlexPWM1_SM3_EXTSYNC)

Offset


 Register                   Offset

 FlexPWM1_SM0_EXTSY 3E0h
 NC

 FlexPWM1_SM1_EXTSY 3E4h
 NC

 FlexPWM1_SM2_EXTSY 3E8h
 NC

 FlexPWM1_SM3_EXTSY 3ECh
 NC


Function
This register selects the PWM1 EXTSYNC inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                   TRIGIN
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


Fields


       Field       Function
      31-6      Reserved
    —

       5-0     EXTSYNC input connections for PWM1
    TRIGIN           00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT2 input is selected
                     00_0010b - SCT_OUT4 input is selected
                     00_0011b - SCT_OUT5 input is selected
                     00_0100b - SCT_OUT3 input is selected

                                  Tablecontinuesonthenextpage...




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    977 / 3791

<!-- page 978 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER2_MAT1 input is selected
                     00_1001b - CTIMER4_MAT1 input is selected
                     00_1010b - Reserved
                     00_1011b - ARM_TXEV input is selected
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[0] input is selected
                     00_1110b - ADC0_tcomp[1] input is selected
                     00_1111b - ADC0_tcomp[2] input is selected
                     01_0000b - ADC0_tcomp[3] input is selected
                     01_0001b - ADC1_tcomp[0] input is selected
                     01_0010b - ADC1_tcomp[1] input is selected
                     01_0011b - ADC1_tcomp[2] input is selected
                     01_0100b - ADC1_tcomp[3] input is selected
                     01_0101b - CMP0_OUT input is selected
                     01_0110b - CMP1_OUT input is selected
                     01_0111b - CMP2_OUT input is selected
                     01_1000b - PWM0_SM0_MUX_TRIG0 input is selected
                     01_1001b - PWM0_SM0_MUX_TRIG1 input is selected
                     01_1010b - PWM0_SM1_MUX_TRIG0 input is selected
                     01_1011b - PWM0_SM1_MUX_TRIG1 input is selected
                     01_1100b - PWM0_SM2_MUX_TRIG0 input is selected
                     01_1101b - PWM0_SM2_MUX_TRIG1 input is selected
                     01_1110b - PWM0_SM3_MUX_TRIG0 input is selected
                     01_1111b - PWM0_SM3_MUX_TRIG1 input is selected
                     10_0000b - QDC0_CMP/POS_MATCH input is selected
                     10_0001b - QDC1_CMP/POS_MATCH input is selected
                     10_0010b - EVTG_OUT0A input is selected
                     10_0011b - EVTG_OUT0B input is selected
                     10_0100b - EVTG_OUT1A input is selected
                     10_0101b - EVTG_OUT1B input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    978 / 3791

<!-- page 979 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     10_0110b - EVTG_OUT2A input is selected
                     10_0111b - EVTG_OUT2B input is selected
                     10_1000b - EVTG_OUT3A input is selected
                     10_1001b - EVTG_OUT3B input is selected
                     10_1010b - TRIG_IN0 input is selected
                     10_1011b - TRIG_IN1 input is selected
                     10_1100b - TRIG_IN2 input is selected
                     10_1101b - TRIG_IN3 input is selected
                     10_1110b - TRIG_IN4 input is selected
                     10_1111b - TRIG_IN5 input is selected
                     11_0000b - TRIG_IN6 input is selected
                     11_0001b - TRIG_IN7 input is selected
                     11_0010b - TRIG_IN8 input is selected
                     11_0011b - TRIG_IN9 input is selected
                     11_0100b - SINC Filter CH0 sync Break input is selected
                     11_0101b - SINC Filter CH1 sync Break input is selected
                     11_0110b - SINC Filter CH2 sync Break input is selected
                     11_0111b - SINC Filter CH3 sync Break input is selected
                     11_1000b - SINC Filter CH4 sync Break input is selected
                     11_1001b - GPIO2 Pin Event Trig 0 input is selected
                     11_1010b - GPIO2 Pin Event Trig 1 input is selected
                     11_1011b - GPIO3 Pin Event Trig 0 input is selected
                     11_1100b - GPIO3 Pin Event Trig 1 input is selected
                               All other values are reserved.


26.5.1.32 PWM1 Input EXTA Connections (FlexPWM1_SM0_EXTA - FlexPWM1_SM3_EXTA)

Offset


 Register                   Offset

 FlexPWM1_SM0_EXTA  3F0h

 FlexPWM1_SM1_EXTA  3F4h

 FlexPWM1_SM2_EXTA  3F8h

 FlexPWM1_SM3_EXTA  3FCh




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    979 / 3791

<!-- page 980 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Function
This register selects the PWM1 EXTA inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                   TRIGIN
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


Fields


       Field       Function
      31-6      Reserved
    —

       5-0      EXTA input connections for PWM1
    TRIGIN           00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT2 input is selected
                     00_0010b - SCT_OUT4 input is selected
                     00_0011b - SCT_OUT5 input is selected
                     00_0100b - SCT_OUT3 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER2_MAT1 input is selected
                     00_1001b - CTIMER4_MAT1 input is selected
                     00_1010b - Reserved
                     00_1011b - ARM_TXEV input is selected
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[0] input is selected
                     00_1110b - ADC0_tcomp[1] input is selected
                     00_1111b - ADC0_tcomp[2] input is selected
                     01_0000b - ADC0_tcomp[3] input is selected

                                  Tablecontinuesonthenextpage...




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    980 / 3791

<!-- page 981 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_0001b - ADC1_tcomp[0] input is selected
                     01_0010b - ADC1_tcomp[1] input is selected
                     01_0011b - ADC1_tcomp[2] input is selected
                     01_0100b - ADC1_tcomp[3] input is selected
                     01_0101b - CMP0_OUT input is selected
                     01_0110b - CMP1_OUT input is selected
                     01_0111b - CMP2_OUT input is selected
                     01_1000b - PWM0_SM0_MUX_TRIG0 input is selected
                     01_1001b - PWM0_SM0_MUX_TRIG1 input is selected
                     01_1010b - PWM0_SM1_MUX_TRIG0 input is selected
                     01_1011b - PWM0_SM1_MUX_TRIG1 input is selected
                     01_1100b - PWM0_SM2_MUX_TRIG0 input is selected
                     01_1101b - PWM0_SM2_MUX_TRIG1 input is selected
                     01_1110b - PWM0_SM3_MUX_TRIG0 input is selected
                     01_1111b - PWM0_SM3_MUX_TRIG1 input is selected
                     10_0000b - QDC0_CMP/POS_MATCH input is selected
                     10_0001b - QDC1_CMP/POS_MATCH input is selected
                     10_0010b - EVTG_OUT0A input is selected
                     10_0011b - EVTG_OUT0B input is selected
                     10_0100b - EVTG_OUT1A input is selected
                     10_0101b - EVTG_OUT1B input is selected
                     10_0110b - EVTG_OUT2A input is selected
                     10_0111b - EVTG_OUT2B input is selected
                     10_1000b - EVTG_OUT3A input is selected
                     10_1001b - EVTG_OUT3B input is selected
                     10_1010b - TRIG_IN0 input is selected
                     10_1011b - TRIG_IN1 input is selected
                     10_1100b - TRIG_IN2 input is selected
                     10_1101b - TRIG_IN3 input is selected
                     10_1110b - TRIG_IN4 input is selected
                     10_1111b - TRIG_IN5 input is selected
                     11_0000b - TRIG_IN6 input is selected
                     11_0001b - TRIG_IN7 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    981 / 3791

<!-- page 982 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     11_0010b - TRIG_IN8 input is selected
                     11_0011b - TRIG_IN9 input is selected
                     11_0100b - SINC Filter CH0 sync Break input is selected
                     11_0101b - SINC Filter CH1 sync Break input is selected
                     11_0110b - SINC Filter CH2 sync Break input is selected
                     11_0111b - SINC Filter CH3 sync Break input is selected
                     11_1000b - SINC Filter CH4 sync Break input is selected
                     11_1001b - GPIO2 Pin Event Trig 0 input is selected
                     11_1010b - GPIO2 Pin Event Trig 1 input is selected
                     11_1011b - GPIO3 Pin Event Trig 0 input is selected
                     11_1100b - GPIO3 Pin Event Trig 1 input is selected
                               All other values are reserved.


26.5.1.33 PWM1 External Force Trigger Connections (FlexPWM1_EXTFORCE)

Offset


 Register                   Offset

 FlexPWM1_EXTFORCE  400h


Function
This register selects the PWM1 EXTFORCE inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                   TRIGIN
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    982 / 3791

<!-- page 983 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-6      Reserved
    —

       5-0     EXTFORCE input connections for PWM1
    TRIGIN           00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT2 input is selected
                     00_0010b - SCT_OUT4 input is selected
                     00_0011b - SCT_OUT5 input is selected
                     00_0100b - SCT_OUT3 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER2_MAT1 input is selected
                     00_1001b - CTIMER4_MAT1 input is selected
                     00_1010b - Reserved
                     00_1011b - ARM_TXEV input is selected
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[0] input is selected
                     00_1110b - ADC0_tcomp[1] input is selected
                     00_1111b - ADC0_tcomp[2] input is selected
                     01_0000b - ADC0_tcomp[3] input is selected
                     01_0001b - ADC1_tcomp[0] input is selected
                     01_0010b - ADC1_tcomp[1] input is selected
                     01_0011b - ADC1_tcomp[2] input is selected
                     01_0100b - ADC1_tcomp[3] input is selected
                     01_0101b - CMP0_OUT input is selected
                     01_0110b - CMP1_OUT input is selected
                     01_0111b - CMP2_OUT input is selected
                     01_1000b - PWM0_SM0_MUX_TRIG0 input is selected
                     01_1001b - PWM0_SM0_MUX_TRIG1 input is selected
                     01_1010b - PWM0_SM1_MUX_TRIG0 input is selected
                     01_1011b - PWM0_SM1_MUX_TRIG1 input is selected
                     01_1100b - PWM0_SM2_MUX_TRIG0 input is selected
                     01_1101b - PWM0_SM2_MUX_TRIG1 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    983 / 3791

<!-- page 984 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_1110b - PWM0_SM3_MUX_TRIG0 input is selected
                     01_1111b - PWM0_SM3_MUX_TRIG1 input is selected
                     10_0000b - QDC0_CMP/POS_MATCH input is selected
                     10_0001b - QDC1_CMP/POS_MATCH input is selected
                     10_0010b - EVTG_OUT0A input is selected
                     10_0011b - EVTG_OUT0B input is selected
                     10_0100b - EVTG_OUT1A input is selected
                     10_0101b - EVTG_OUT1B input is selected
                     10_0110b - EVTG_OUT2A input is selected
                     10_0111b - EVTG_OUT2B input is selected
                     10_1000b - EVTG_OUT3A input is selected
                     10_1001b - EVTG_OUT3B input is selected
                     10_1010b - TRIG_IN0 input is selected
                     10_1011b - TRIG_IN1 input is selected
                     10_1100b - TRIG_IN2 input is selected
                     10_1101b - TRIG_IN3 input is selected
                     10_1110b - TRIG_IN4 input is selected
                     10_1111b - TRIG_IN5 input is selected
                     11_0000b - TRIG_IN6 input is selected
                     11_0001b - TRIG_IN7 input is selected
                     11_0010b - TRIG_IN8 input is selected
                     11_0011b - TRIG_IN9 input is selected
                     11_0100b - SINC Filter CH0 sync Break input is selected
                     11_0101b - SINC Filter CH1 sync Break input is selected
                     11_0110b - SINC Filter CH2 sync Break input is selected
                     11_0111b - SINC Filter CH3 sync Break input is selected
                     11_1000b - SINC Filter CH4 sync Break input is selected
                     11_1001b - GPIO2 Pin Event Trig 0 input is selected
                     11_1010b - GPIO2 Pin Event Trig 1 input is selected
                     11_1011b - GPIO3 Pin Event Trig 0 input is selected
                     11_1100b - GPIO3 Pin Event Trig 1 input is selected
                               All other values are reserved.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    984 / 3791

<!-- page 985 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

26.5.1.34 PWM1 Fault Input Trigger Connections (FlexPWM1_FAULT0 - FlexPWM1_FAULT3)

Offset


 Register                   Offset

 FlexPWM1_FAULT0     404h

 FlexPWM1_FAULT1     408h

 FlexPWM1_FAULT2     40Ch

 FlexPWM1_FAULT3     410h


Function
This register selects the PWM1 FAULT inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                   TRIGIN
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


Fields


       Field       Function
      31-6      Reserved
    —

       5-0      FAULT input connections for PWM1
    TRIGIN           00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT2 input is selected
                     00_0010b - SCT_OUT4 input is selected
                     00_0011b - SCT_OUT5 input is selected
                     00_0100b - SCT_OUT3 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    985 / 3791

<!-- page 986 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     00_1000b - CTIMER2_MAT1 input is selected
                     00_1001b - CTIMER4_MAT1 input is selected
                     00_1010b - Reserved
                     00_1011b - ARM_TXEV input is selected
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[0] input is selected
                     00_1110b - ADC0_tcomp[1] input is selected
                     00_1111b - ADC0_tcomp[2] input is selected
                     01_0000b - ADC0_tcomp[3] input is selected
                     01_0001b - ADC1_tcomp[0] input is selected
                     01_0010b - ADC1_tcomp[1] input is selected
                     01_0011b - ADC1_tcomp[2] input is selected
                     01_0100b - ADC1_tcomp[3] input is selected
                     01_0101b - CMP0_OUT input is selected
                     01_0110b - CMP1_OUT input is selected
                     01_0111b - CMP2_OUT input is selected
                     01_1000b - PWM0_SM0_MUX_TRIG0 input is selected
                     01_1001b - PWM0_SM0_MUX_TRIG1 input is selected
                     01_1010b - PWM0_SM1_MUX_TRIG0 input is selected
                     01_1011b - PWM0_SM1_MUX_TRIG1 input is selected
                     01_1100b - PWM0_SM2_MUX_TRIG0 input is selected
                     01_1101b - PWM0_SM2_MUX_TRIG1 input is selected
                     01_1110b - PWM0_SM3_MUX_TRIG0 input is selected
                     01_1111b - PWM0_SM3_MUX_TRIG1 input is selected
                     10_0000b - QDC0_CMP/POS_MATCH input is selected
                     10_0001b - QDC1_CMP/POS_MATCH input is selected
                     10_0010b - EVTG_OUT0A input is selected
                     10_0011b - EVTG_OUT0B input is selected
                     10_0100b - EVTG_OUT1A input is selected
                     10_0101b - EVTG_OUT1B input is selected
                     10_0110b - EVTG_OUT2A input is selected
                     10_0111b - EVTG_OUT2B input is selected
                     10_1000b - EVTG_OUT3A input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    986 / 3791

<!-- page 987 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     10_1001b - EVTG_OUT3B input is selected
                     10_1010b - TRIG_IN0 input is selected
                     10_1011b - TRIG_IN1 input is selected
                     10_1100b - TRIG_IN2 input is selected
                     10_1101b - TRIG_IN3 input is selected
                     10_1110b - TRIG_IN4 input is selected
                     10_1111b - TRIG_IN5 input is selected
                     11_0000b - TRIG_IN6 input is selected
                     11_0001b - TRIG_IN7 input is selected
                     11_0010b - TRIG_IN8 input is selected
                     11_0011b - TRIG_IN9 input is selected
                     11_0100b - SINC Filter CH0 sync Break input is selected
                     11_0101b - SINC Filter CH1 sync Break input is selected
                     11_0110b - SINC Filter CH2 sync Break input is selected
                     11_0111b - SINC Filter CH3 sync Break input is selected
                     11_1000b - SINC Filter CH4 sync Break input is selected
                     11_1001b - GPIO2 Pin Event Trig 0 input is selected
                     11_1010b - GPIO2 Pin Event Trig 1 input is selected
                     11_1011b - GPIO3 Pin Event Trig 0 input is selected
                     11_1100b - GPIO3 Pin Event Trig 1 input is selected
                               All other values are reserved.


26.5.1.35 PWM0 External Clock Trigger (PWM0_EXT_CLK)

Offset


 Register                   Offset

 PWM0_EXT_CLK       420h


Function
PWM0 external clock trigger connections





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    987 / 3791

<!-- page 988 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      Reserved                                                   TRIGIN
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-3      Reserved
    —

       2-0      EXT_CLK input connections for PWM0
    TRIGIN          000b - FRO16K input is selected
                     001b - OSC_32k input is selected
                     010b - EVTG_OUT0A input is selected
                     011b - EVTG_OUT1A input is selected
                     100b - TRIG_IN0 input is selected
                     101b - TRIG_IN7 input is selected
                               All other values are reserved.


26.5.1.36 PWM1 External Clock Trigger (PWM1_EXT_CLK)

Offset


 Register                   Offset

 PWM1_EXT_CLK       424h


Function
PWM1 external clock trigger connections





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    988 / 3791

<!-- page 989 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                  Reserved                                                   TRIGIN
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
      31-4      Reserved
    —

       3-0      EXT_CLK input connections for PWM1
    TRIGIN          0000b - FRO16K input is selected
                     0001b - OSC_32k input is selected
                     0010b - EVTG_OUT0A input is selected
                     0011b - EVTG_OUT1A input is selected
                     0100b - TRIG_IN0 input is selected
                     0101b - TRIG_IN7 input is selected
                               All other values are reserved.


26.5.1.37 EVTG Trigger Input Connections (EVTG_TRIG0 - EVTG_TRIG15)

Offset
For a = 0 to 15:


 Register                   Offset

 EVTG_TRIGa          440h + (a × 4h)


Function
This register is used to select the EVTG trigger inputs.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    989 / 3791

<!-- page 990 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


Fields


       Field       Function
      31-6      Reserved
    —

       5-0     EVTG trigger input connections
      INP            00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT1 input is selected
                     00_0010b - SCT_OUT0 input is selected
                     00_0011b - SCT_OUT1 input is selected
                     00_0100b - SCT_OUT2 input is selected
                     00_0101b - SCT_OUT3 input is selected
                     00_0110b - CTIMER0_MAT3 input is selected
                     00_0111b - CTIMER1_MAT3 input is selected
                     00_1000b - CTIMER2_MAT3 input is selected
                     00_1001b - CTIMER2_MAT2 input is selected
                     00_1010b - CTIMER3_MAT2 input is selected
                     00_1011b - CTIMER4_MAT2 input is selected
                     00_1100b - Reserved
                     00_1101b - PINT GPIO_INT_BMAT input is selected
                     00_1110b - ADC0_IRQ input is selected
                     00_1111b - ADC1_IRQ input is selected
                     01_0000b - ADC0_tcomp[0] input is selected
                     01_0001b - ADC0_tcomp[1] input is selected
                     01_0010b - ADC0_tcomp[2] input is selected
                     01_0011b - ADC0_tcomp[3] input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    990 / 3791

<!-- page 991 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_0100b - ADC1_tcomp[0] input is selected
                     01_0101b - ADC1_tcomp[1] input is selected
                     01_0110b - ADC1_tcomp[2] input is selected
                     01_0111b - ADC1_tcomp[3] input is selected
                     01_1000b - CMP0_OUT input is selected
                     01_1001b - CMP1_OUT input is selected
                     01_1010b - CMP2_OUT input is selected
                     01_1011b - PWM0_SM0_MUX_TRIG0 input is selected
                     01_1100b - PWM0_SM0_MUX_TRIG1 input is selected
                     01_1101b - PWM0_SM1_MUX_TRIG0 input is selected
                     01_1110b - PWM0_SM1_MUX_TRIG1 input is selected
                     01_1111b - PWM0_SM2_MUX_TRIG0 input is selected
                     10_0000b - PWM0_SM2_MUX_TRIG1 input is selected
                     10_0001b - PWM0_SM3_MUX_TRIG0 input is selected
                     10_0010b - PWM0_SM3_MUX_TRIG1 input is selected
                     10_0011b - PWM1_SM0_MUX_TRIG0 input is selected
                     10_0100b - PWM1_SM0_MUX_TRIG1 input is selected
                     10_0101b - PWM1_SM1_MUX_TRIG0 input is selected
                     10_0110b - PWM1_SM1_MUX_TRIG1 input is selected
                     10_0111b - PWM1_SM2_MUX_TRIG0 input is selected
                     10_1000b - PWM1_SM2_MUX_TRIG1 input is selected
                     10_1001b - PWM1_SM3_MUX_TRIG0 input is selected
                     10_1010b - PWM1_SM3_MUX_TRIG1 input is selected
                     10_1011b - QDC0_CMP/POS_MATCH input is selected
                     10_1100b - QDC1_CMP/POS_MATCH input is selected
                     10_1101b - TRIG_IN0 input is selected
                     10_1110b - TRIG_IN1 input is selected
                     10_1111b - TRIG_IN2 input is selected
                     11_0000b - TRIG_IN3 input is selected
                     11_0001b - LPTMR0 input is selected
                     11_0010b - LPTMR1 input is selected
                     11_0011b - SINC Filter CH0 Break input is selected
                     11_0100b - SINC Filter CH1 Break input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    991 / 3791

<!-- page 992 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     11_0101b - SINC Filter CH2 Break input is selected
                     11_0110b - SINC Filter CH3 Break input is selected
                     11_0111b - SINC Filter CH4 Break input is selected
                     11_1000b - Reserved
                     11_1001b - Reserved
                               All other values are reserved.


26.5.1.38 USB-FS Trigger Input Connections (USBFS_TRIG)

Offset


 Register                   Offset

 USBFS_TRIG          480h


Function
This register is used to select the USB-FS trigger inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                  Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       1      1      1      1


Fields


       Field       Function
      31-4      Reserved
    —

       3-0      USB-FS trigger input connections. The trigger output of LP_FLEXCOMM is an input of peripheral
              INPUTMUX.
      INP
                     0000b - LP_FLEXCOMM 0 trigger out [3] input is selected

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    992 / 3791

<!-- page 993 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0001b - LP_FLEXCOMM 1 trigger out [3] input is selected
                     0010b - LP_FLEXCOMM 2 trigger out [3] input is selected
                     0011b - LP_FLEXCOMM 3 trigger out [3] input is selected
                     0100b - LP_FLEXCOMM 4 trigger out [3] input is selected
                     0101b - LP_FLEXCOMM 5 trigger out [3] input is selected
                     0110b - LP_FLEXCOMM 6 trigger out [3] input is selected
                     0111b - LP_FLEXCOMM 7 trigger out [3] input is selected
                     1000b - LP_FLEXCOMM 8 trigger out [3] input is selected
                     1001b - LP_FLEXCOMM 9 trigger out [3] input is selected
                               All other values are reserved.


26.5.1.39  TSI Trigger Input Connections (TSI_TRIG)

Offset


 Register                   Offset

 TSI_TRIG             4A0h


Function
This register is used to select the TSI trigger inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                         Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      1      1


Fields


       Field       Function
      31-2      Reserved

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    993 / 3791

<!-- page 994 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

    —

       1-0       TSI trigger input connections
      INP            00b - LPTMR0 input is selected
                     01b - LPTMR1 input is selected
                               All other values are reserved.


26.5.1.40 EXT Trigger Connections (EXT_TRIG0 - EXT_TRIG7)

Offset
For a = 0 to 7:


 Register                   Offset

 EXT_TRIGa           4C0h + (a × 4h)


Function
The EXT_TRIGa regsiters select the source of TRIG_OUTa pins.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


Fields


       Field       Function
      31-6      Reserved
    —

       5-0      TRIG_OUTa pin input connections
      INP            00_0000b - PINT PIN_INT0 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    994 / 3791

<!-- page 995 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     00_0001b - PINT PIN_INT1 input is selected
                     00_0010b - ADC0_IRQ input is selected
                     00_0011b - ADC1_IRQ input is selected
                     00_0100b - ADC0_tcomp[0] input is selected
                     00_0101b - ADC1_tcomp[0] input is selected
                     00_0110b - PWM0_SM0_MUX_TRIG0/PWM0_SM0_MUX_TRIG1 input is selected
                     00_0111b - PWM0_SM1_MUX_TRIG0/PWM0_SM1_MUX_TRIG1 input is selected
                     00_1000b - PWM0_SM2_MUX_TRIG0/PWM0_SM2_MUX_TRIG1 input is selected
                     00_1001b - PWM0_SM3_MUX_TRIG0/PWM0_SM3_MUX_TRIG1 input is selected
                     00_1010b - PWM1_SM0_MUX_TRIG0/PWM1_SM0_MUX_TRIG1 input is selected
                     00_1011b - PWM1_SM1_MUX_TRIG0/PWM1_SM1_MUX_TRIG1 input is selected
                     00_1100b - PWM1_SM2_MUX_TRIG0/PWM1_SM2_MUX_TRIG1 input is selected
                     00_1101b - PWM1_SM3_MUX_TRIG0/PWM1_SM3_MUX_TRIG1 input is selected
                     00_1110b - QDC0_CMP/POS_MATCH input is selected
                     00_1111b - QDC1_CMP/POS_MATCH input is selected
                     01_0000b - EVTG_OUT0A input is selected
                     01_0001b - EVTG_OUT0B input is selected
                     01_0010b - EVTG_OUT1A input is selected
                     01_0011b - EVTG_OUT1B input is selected
                     01_0100b - EVTG_OUT2A input is selected
                     01_0101b - EVTG_OUT2B input is selected
                     01_0110b - EVTG_OUT3A input is selected
                     01_0111b - EVTG_OUT3B input is selected
                     01_1000b - Reserved
                     01_1001b - Reserved
                     01_1010b - LPTMR0 input is selected
                     01_1011b - LPTMR1 input is selected
                     01_1100b - SCT Out0 input is selected
                     01_1101b - SCT Out1 input is selected
                     01_1110b - SCT Out2 input is selected
                     01_1111b - SCT Out3 input is selected
                     10_0000b - SCT Out4 input is selected
                     10_0001b - SCT Out5 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    995 / 3791

<!-- page 996 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     10_0010b - LP_FLEXCOMM0 trigger output 3 input is selected
                     10_0011b - LP_FLEXCOMM1 trigger output 3 input is selected
                     10_0100b - LP_FLEXCOMM2 trigger output 3 input is selected
                     10_0101b - LP_FLEXCOMM3 trigger output 3 input is selected
                     10_0110b - LP_FLEXCOMM4 trigger output 3 input is selected
                     10_0111b - LP_FLEXCOMM5 trigger output 3 input is selected
                     10_1000b - LP_FLEXCOMM6 trigger output 3 input is selected
                     10_1001b - LP_FLEXCOMM7 trigger output 3 input is selected
                     10_1010b - LP_FLEXCOMM8 trigger output 3 input is selected
                     10_1011b - LP_FLEXCOMM9 trigger output 3 input is selected
                     10_1100b - CMP0_OUT input is selected
                     10_1101b - CMP1_OUT input is selected
                     10_1110b - CMP2_OUT input is selected
                     10_1111b - ENET_PPS_OUT_0 input is selected
                               All other values are reserved.


26.5.1.41 CMP1 Input Connections (CMP1_TRIG)

Offset


 Register                   Offset

 CMP1_TRIG           4E0h


Function
This register selects the CMP1 SAMPLE/WINDOW input.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                   TRIGIN
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    996 / 3791

<!-- page 997 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-6      Reserved
    —

       5-0     CMP1 input trigger
    TRIGIN           00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT7 input is selected
                     00_0010b - SCT0 SCT_OUT4 input is selected
                     00_0011b - SCT0 SCT_OUT5 input is selected
                     00_0100b - SCT0 SCT_OUT7 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER3_MAT1 input is selected
                     00_1001b - CTIMER4_MAT1 input is selected
                     00_1010b - Reserved
                     00_1011b - Reserved
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[1] input is selected
                     00_1110b - ADC1_tcomp[1] input is selected
                     00_1111b - Reserved
                     01_0000b - Reserved
                     01_0001b - PWM0_SM0_MUX_TRIG0/PWM0_SM0_MUX_TRIG1 input is selected
                     01_0010b - PWM0_SM1_MUX_TRIG0/PWM0_SM1_MUX_TRIG1 input is selected
                     01_0011b - PWM0_SM2_MUX_TRIG0/PWM0_SM2_MUX_TRIG1 input is selected
                     01_0100b - PWM0_SM3_MUX_TRIG0/PWM0_SM3_MUX_TRIG1 input is selected
                     01_0101b - PWM1_SM0_MUX_TRIG0/PWM1_SM0_MUX_TRIG1 input is selected
                     01_0110b - PWM1_SM1_MUX_TRIG0/PWM1_SM1_MUX_TRIG1 input is selected
                     01_0111b - PWM1_SM2_MUX_TRIG0/PWM1_SM2_MUX_TRIG1 input is selected
                     01_1000b - PWM1_SM3_MUX_TRIG0/PWM1_SM3_MUX_TRIG1 input is selected
                     01_1001b - QDC0_CMP/POS_MATCH input is selected
                     01_1010b - QDC1_CMP/POS_MATCH input is selected
                     01_1011b - EVTG_OUT0A input is selected
                     01_1100b - EVTG_OUT0B input is selected
                     01_1101b - EVTG_OUT1A input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    997 / 3791

<!-- page 998 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_1110b - EVTG_OUT1B input is selected
                     01_1111b - EVTG_OUT2A input is selected
                     10_0000b - EVTG_OUT2B input is selected
                     10_0001b - EVTG_OUT3A input is selected
                     10_0010b - EVTG_OUT3B input is selected
                     10_0011b - LPTMR0 input is selected
                     10_0100b - LPTMR1 input is selected
                     10_0101b - GPIO2 Pin Event Trig 0 input is selected
                     10_0110b - GPIO2 Pin Event Trig 1 input is selected
                     10_0111b - GPIO3 Pin Event Trig 0 input is selected
                     10_1000b - GPIO3 Pin Event Trig 1 input is selected
                               All other values are reserved.


26.5.1.42 CMP2 Input Connections (CMP2_TRIG)

Offset


 Register                   Offset

 CMP2_TRIG           500h


Function
This register selects the CMP2 SAMPLE/WINDOW input.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                   TRIGIN
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    998 / 3791

<!-- page 999 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-6      Reserved
    —

       5-0     CMP2 input trigger
    TRIGIN           00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT4 input is selected
                     00_0010b - SCT0 SCT_OUT4 input is selected
                     00_0011b - SCT0 SCT_OUT5 input is selected
                     00_0100b - SCT0 SCT_OUT8 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER3_MAT2 input is selected
                     00_1001b - CTIMER4_MAT2 input is selected
                     00_1010b - Reserved
                     00_1011b - Reserved
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[2] input is selected
                     00_1110b - ADC1_tcomp[2] input is selected
                     00_1111b - Reserved
                     01_0000b - Reserved
                     01_0001b - PWM0_SM0_MUX_TRIG0/PWM0_SM0_MUX_TRIG1 input is selected
                     01_0010b - PWM0_SM1_MUX_TRIG0/PWM0_SM1_MUX_TRIG1 input is selected
                     01_0011b - PWM0_SM2_MUX_TRIG0/PWM0_SM2_MUX_TRIG1 input is selected
                     01_0100b - PWM0_SM3_MUX_TRIG0/PWM0_SM3_MUX_TRIG1 input is selected
                     01_0101b - PWM1_SM0_MUX_TRIG0/PWM1_SM0_MUX_TRIG1 input is selected
                     01_0110b - PWM1_SM1_MUX_TRIG0/PWM1_SM1_MUX_TRIG1 input is selected
                     01_0111b - PWM1_SM2_MUX_TRIG0/PWM1_SM2_MUX_TRIG1 input is selected
                     01_1000b - PWM1_SM3_MUX_TRIG0/PWM1_SM3_MUX_TRIG1 input is selected
                     01_1001b - QDC0_CMP/POS_MATCH input is selected
                     01_1010b - QDC1_CMP/POS_MATCH input is selected
                     01_1011b - EVTG_OUT0A input is selected
                     01_1100b - EVTG_OUT0B input is selected
                     01_1101b - EVTG_OUT1A input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    999 / 3791

<!-- page 1000 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_1110b - EVTG_OUT1B input is selected
                     01_1111b - EVTG_OUT2A input is selected
                     10_0000b - EVTG_OUT2B input is selected
                     10_0001b - EVTG_OUT3A input is selected
                     10_0010b - EVTG_OUT3B input is selected
                     10_0011b - LPTMR0 input is selected
                     10_0100b - LPTMR1 input is selected
                     10_0101b - GPIO2 Pin Event Trig 0 input is selected
                     10_0110b - GPIO2 Pin Event Trig 1 input is selected
                     10_0111b - GPIO3 Pin Event Trig 0 input is selected
                     10_1000b - GPIO3 Pin Event Trig 1 input is selected
                               All other values are reserved.


26.5.1.43 SINC Filter Channel Trigger Input Connections (SINC_FILTER_CH0 - SINC_FILTER_CH4)

Offset


 Register                   Offset

 SINC_FILTER_CH0      520h

 SINC_FILTER_CH1      524h

 SINC_FILTER_CH2      528h

 SINC_FILTER_CH3     52Ch

 SINC_FILTER_CH4      530h


Function
This register is used to select the SINC Filter channel trigger inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1000 / 3791

<!-- page 1001 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-6      Reserved
    —

       5-0      SINC FILTER trigger input connections
      INP            00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT1 input is selected
                     00_0010b - SCT_OUT4 input is selected
                     00_0011b - SCT_OUT5 input is selected
                     00_0100b - SCT_OUT9 input is selected
                     00_0101b - CTIMER0_MAT3 input is selected
                     00_0110b - CTIMER1_MAT3 input is selected
                     00_0111b - CTIMER2_MAT3 input is selected
                     00_1000b - CTIMER3_MAT3 input is selected
                     00_1001b - CTIMER4_MAT3 input is selected
                     00_1010b - Reserved
                     00_1011b - Reserved
                     00_1100b - PINT GPIO_INT_BMAT input is selected
                     00_1101b - ADC0_tcomp[0] input is selected
                     00_1110b - ADC0_tcomp[1] input is selected
                     00_1111b - ADC0_tcomp[2] input is selected
                     01_0000b - ADC0_tcomp[3] input is selected
                     01_0001b - ADC1_tcomp[0] input is selected
                     01_0010b - ADC1_tcomp[1] input is selected
                     01_0011b - ADC1_tcomp[2] input is selected
                     01_0100b - ADC1_tcomp[3] input is selected
                     01_0101b - CMP0_OUT input is selected
                     01_0110b - CMP1_OUT input is selected
                     01_0111b - CMP2_OUT input is selected
                     01_1000b - PWM0_SM0_MUX_TRIG0 input is selected
                     01_1001b - PWM0_SM0_MUX_TRIG1 input is selected
                     01_1010b - PWM0_SM1_MUX_TRIG0 input is selected
                     01_1011b - PWM0_SM1_MUX_TRIG1 input is selected
                     01_1100b - PWM0_SM2_MUX_TRIG0 input is selected
                     01_1101b - PWM0_SM2_MUX_TRIG1 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1001 / 3791

<!-- page 1002 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_1110b - PWM0_SM3_MUX_TRIG0 input is selected
                     01_1111b - PWM0_SM3_MUX_TRIG1 input is selected
                     10_0000b - PWM1_SM0_MUX_TRIG0 input is selected
                     10_0001b - PWM1_SM0_MUX_TRIG1 input is selected
                     10_0010b - PWM1_SM1_MUX_TRIG0 input is selected
                     10_0011b - PWM1_SM1_MUX_TRIG1 input is selected
                     10_0100b - PWM1_SM2_MUX_TRIG0 input is selected
                     10_0101b - PWM1_SM2_MUX_TRIG1 input is selected
                     10_0110b - PWM1_SM3_MUX_TRIG0 input is selected
                     10_0111b - PWM1_SM3_MUX_TRIG1 input is selected
                     10_1000b - QDC0_CMP/POS_MATCH input is selected
                     10_1001b - QDC1_CMP/POS_MATCH input is selected
                     10_1010b - EVTG_OUT0A input is selected
                     10_1011b - EVTG_OUT0B input is selected
                     10_1100b - EVTG_OUT1A input is selected
                     10_1101b - EVTG_OUT1B input is selected
                     10_1110b - EVTG_OUT2A input is selected
                     10_1111b - EVTG_OUT2B input is selected
                     11_0000b - EVTG_OUT3A input is selected
                     11_0001b - EVTG_OUT3B input is selected
                     11_0010b - LPTMR0 input is selected
                     11_0011b - LPTMR1 input is selected
                     11_0100b - FlexIO CH0 input is selected
                     11_0101b - FlexIO CH1 input is selected
                     11_0110b - FlexIO CH2 input is selected
                     11_0111b - FlexIO CH3 input is selected
                     11_1000b - WUU input is selected
                               All other values are reserved.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1002 / 3791

<!-- page 1003 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

26.5.1.44 OPAMP Trigger Input Connections (OPAMP0_TRIG - OPAMP2_TRIG)

Offset


 Register                   Offset

 OPAMP0_TRIG         580h

 OPAMP1_TRIG         584h

 OPAMP2_TRIG         588h


Function
This register is used to select the OPAMP trigger inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1


Fields


       Field       Function
      31-6      Reserved
    —

       5-0     OPAMP trigger input connections
      INP            00_0000b - PINT PIN_INT0 input is selected
                     00_0001b - PINT PIN_INT1 input is selected
                     00_0010b - PINT PIN_INT2 input is selected
                     00_0011b - PINT PIN_INT3 input is selected
                     00_0100b - SCT_OUT4 input is selected
                     00_0101b - SCT_OUT5 input is selected
                     00_0110b - SCT_OUT6 input is selected
                     00_0111b - SCT_OUT7 input is selected
                     00_1000b - SCT_OUT8 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1003 / 3791

<!-- page 1004 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     00_1001b - CTIMER0_MAT3 input is selected
                     00_1010b - CTIMER1_MAT3 input is selected
                     00_1011b - CTIMER2_MAT3 input is selected
                     00_1100b - CTIMER3_MAT3 input is selected
                     00_1101b - CTIMER4_MAT3 input is selected
                     00_1110b - PINT GPIO_INT_BMAT input is selected
                     00_1111b - ADC0_tcomp[0] input is selected
                     01_0000b - ADC0_tcomp[1] input is selected
                     01_0001b - ADC0_tcomp[2] input is selected
                     01_0010b - ADC0_tcomp[3] input is selected
                     01_0011b - ADC1_tcomp[0] input is selected
                     01_0100b - ADC1_tcomp[1] input is selected
                     01_0101b - ADC1_tcomp[2] input is selected
                     01_0110b - ADC1_tcomp[3] input is selected
                     01_0111b - PWM0_SM0_MUX_TRIG0 input is selected
                     01_1000b - PWM0_SM0_MUX_TRIG1 input is selected
                     01_1001b - PWM0_SM1_MUX_TRIG0 input is selected
                     01_1010b - PWM0_SM1_MUX_TRIG1 input is selected
                     01_1011b - PWM0_SM2_MUX_TRIG0 input is selected
                     01_1100b - PWM0_SM2_MUX_TRIG1 input is selected
                     01_1101b - PWM0_SM3_MUX_TRIG0 input is selected
                     01_1110b - PWM0_SM3_MUX_TRIG1 input is selected
                     01_1111b - PWM1_SM0_MUX_TRIG0 input is selected
                     10_0000b - PWM1_SM0_MUX_TRIG1 input is selected
                     10_0001b - PWM1_SM1_MUX_TRIG0 input is selected
                     10_0010b - PWM1_SM1_MUX_TRIG1 input is selected
                     10_0011b - PWM1_SM2_MUX_TRIG0 input is selected
                     10_0100b - PWM1_SM2_MUX_TRIG1 input is selected
                     10_0101b - PWM1_SM3_MUX_TRIG0 input is selected
                     10_0110b - PWM1_SM3_MUX_TRIG1 input is selected
                     10_0111b - EVTG_OUT0A input is selected
                     10_1000b - EVTG_OUT0B input is selected
                     10_1001b - EVTG_OUT1A input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1004 / 3791

<!-- page 1005 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     10_1010b - EVTG_OUT1B input is selected
                     10_1011b - EVTG_OUT2A input is selected
                     10_1100b - EVTG_OUT2B input is selected
                     10_1101b - EVTG_OUT3A input is selected
                     10_1110b - EVTG_OUT3B input is selected
                     10_1111b - TRIG_IN0 input is selected
                     11_0000b - TRIG_IN1 input is selected
                     11_0001b - TRIG_IN2 input is selected
                     11_0010b - TRIG_IN3 input is selected
                     11_0011b - FlexIO CH4 input is selected
                     11_0100b - FlexIO CH5 input is selected
                     11_0101b - FlexIO CH6 input is selected
                     11_0110b - FlexIO CH7 input is selected
                               All other values are reserved.


26.5.1.45 LP_FLEXCOMM0 Trigger Input Connections (FLEXCOMM0_TRIG)

Offset


 Register                   Offset

 FLEXCOMM0_TRIG     5A0h


Function
This register is used to select the LP_FLEXCOMM0 trigger inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1005 / 3791

<!-- page 1006 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-6      Reserved
    —

       5-0     LP_FLEXCOMM0 trigger input connections
      INP            00_0000b - PINT PIN_INT4 input is selected
                     00_0001b - PINT PIN_INT5 input is selected
                     00_0010b - PINT PIN_INT6 input is selected
                     00_0011b - SCT_OUT5 input is selected
                     00_0100b - SCT_OUT6 input is selected
                     00_0101b - SCT_OUT7 input is selected
                     00_0110b - CTIMER0_MAT1 input is selected
                     00_0111b - CTIMER1_MAT1 input is selected
                     00_1000b - CTIMER2_MAT0 input is selected
                     00_1001b - CTIMER3_MAT0 input is selected
                     00_1010b - CTIMER4_MAT0 input is selected
                     00_1011b - LPTMR0 input is selected
                     00_1100b - LPTMR1 input is selected
                     00_1101b - Reserved
                     00_1110b - PINT GPIO_INT_BMAT input is selected
                     00_1111b - CMP0_OUT input is selected
                     01_0000b - CMP1_OUT input is selected
                     01_0001b - CMP2_OUT input is selected
                     01_0010b - EVTG_OUT0A input is selected
                     01_0011b - EVTG_OUT0B input is selected
                     01_0100b - EVTG_OUT1A input is selected
                     01_0101b - EVTG_OUT1B input is selected
                     01_0110b - EVTG_OUT2A input is selected
                     01_0111b - EVTG_OUT2B input is selected
                     01_1000b - EVTG_OUT3A input is selected
                     01_1001b - EVTG_OUT3B input is selected
                     01_1010b - TRIG_IN0 input is selected
                     01_1011b - TRIG_IN1 input is selected
                     01_1100b - TRIG_IN2 input is selected
                     01_1101b - TRIG_IN3 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1006 / 3791

<!-- page 1007 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_1110b - TRIG_IN4 input is selected
                     01_1111b - TRIG_IN10 input is selected
                     10_0000b - TRIG_IN11 input is selected
                     10_0001b - FlexIO CH4 input is selected
                     10_0010b - FlexIO CH5 input is selected
                     10_0011b - FlexIO CH6 input is selected
                     10_0100b - FlexIO CH7 input is selected
                     10_0101b - USB0 ipp_ind_uart_rxd_usbmux input is selected
                     10_0110b - GPIO2 Pin Event Trig 0 input is selected
                     10_0111b - GPIO2 Pin Event Trig 1 input is selected
                     10_1000b - GPIO3 Pin Event Trig 0 input is selected
                     10_1001b - GPIO3 Pin Event Trig 1 input is selected
                     10_1010b - WUU input is selected
                               All other values are reserved.


26.5.1.46 LP_FLEXCOMM1 Trigger Input Connections (FLEXCOMM1_TRIG)

Offset


 Register                   Offset

 FLEXCOMM1_TRIG     5C0h


Function
This register is used to select the LP_FLEXCOMM1 trigger inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1007 / 3791

<!-- page 1008 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-6      Reserved
    —

       5-0     LP_FLEXCOMM1 trigger input connections
      INP            00_0000b - PINT PIN_INT4 input is selected
                     00_0001b - PINT PIN_INT5 input is selected
                     00_0010b - PINT PIN_INT6 input is selected
                     00_0011b - SCT_OUT5 input is selected
                     00_0100b - SCT_OUT6 input is selected
                     00_0101b - SCT_OUT7 input is selected
                     00_0110b - CTIMER0_MAT1 input is selected
                     00_0111b - CTIMER1_MAT1 input is selected
                     00_1000b - CTIMER2_MAT0 input is selected
                     00_1001b - CTIMER3_MAT0 input is selected
                     00_1010b - CTIMER4_MAT0 input is selected
                     00_1011b - LPTMR0 input is selected
                     00_1100b - LPTMR1 input is selected
                     00_1101b - Reserved
                     00_1110b - PINT GPIO_INT_BMAT input is selected
                     00_1111b - CMP0_OUT input is selected
                     01_0000b - CMP1_OUT input is selected
                     01_0001b - CMP2_OUT input is selected
                     01_0010b - EVTG_OUT0A input is selected
                     01_0011b - EVTG_OUT0B input is selected
                     01_0100b - EVTG_OUT1A input is selected
                     01_0101b - EVTG_OUT1B input is selected
                     01_0110b - EVTG_OUT2A input is selected
                     01_0111b - EVTG_OUT2B input is selected
                     01_1000b - EVTG_OUT3A input is selected
                     01_1001b - EVTG_OUT3B input is selected
                     01_1010b - TRIG_IN0 input is selected
                     01_1011b - TRIG_IN1 input is selected
                     01_1100b - TRIG_IN2 input is selected
                     01_1101b - TRIG_IN3 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1008 / 3791

<!-- page 1009 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_1110b - TRIG_IN4 input is selected
                     01_1111b - TRIG_IN10 input is selected
                     10_0000b - TRIG_IN11 input is selected
                     10_0001b - FlexIO CH4 input is selected
                     10_0010b - FlexIO CH5 input is selected
                     10_0011b - FlexIO CH6 input is selected
                     10_0100b - FlexIO CH7 input is selected
                     10_0101b - USB0 ipp_ind_uart_rxd_usbmux input is selected
                     10_0110b - GPIO2 Pin Event Trig 0 input is selected
                     10_0111b - GPIO2 Pin Event Trig 1 input is selected
                     10_1000b - GPIO3 Pin Event Trig 0 input is selected
                     10_1001b - GPIO3 Pin Event Trig 1 input is selected
                     10_1010b - WUU input is selected
                               All other values are reserved.


26.5.1.47 LP_FLEXCOMM2 Trigger Input Connections (FLEXCOMM2_TRIG)

Offset


 Register                   Offset

 FLEXCOMM2_TRIG     5E0h


Function
This register is used to select the LP_FLEXCOMM2 trigger inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1009 / 3791

<!-- page 1010 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-6      Reserved
    —

       5-0     LP_FLEXCOMM2 trigger input connections
      INP            00_0000b - PINT PIN_INT4 input is selected
                     00_0001b - PINT PIN_INT6 input is selected
                     00_0010b - PINT PIN_INT7 input is selected
                     00_0011b - SCT_OUT5 input is selected
                     00_0100b - SCT_OUT8 input is selected
                     00_0101b - SCT_OUT9 input is selected
                     00_0110b - CTIMER0_MAT1 input is selected
                     00_0111b - CTIMER1_MAT1 input is selected
                     00_1000b - CTIMER2_MAT1 input is selected
                     00_1001b - CTIMER3_MAT1 input is selected
                     00_1010b - CTIMER4_MAT1 input is selected
                     00_1011b - LPTMR0 input is selected
                     00_1100b - LPTMR1 input is selected
                     00_1101b - Reserved
                     00_1110b - PINT GPIO_INT_BMAT input is selected
                     00_1111b - CMP0_OUT input is selected
                     01_0000b - CMP1_OUT input is selected
                     01_0001b - CMP2_OUT input is selected
                     01_0010b - EVTG_OUT0A input is selected
                     01_0011b - EVTG_OUT0B input is selected
                     01_0100b - EVTG_OUT1A input is selected
                     01_0101b - EVTG_OUT1B input is selected
                     01_0110b - EVTG_OUT2A input is selected
                     01_0111b - EVTG_OUT2B input is selected
                     01_1000b - EVTG_OUT3A input is selected
                     01_1001b - EVTG_OUT3B input is selected
                     01_1010b - TRIG_IN0 input is selected
                     01_1011b - TRIG_IN1 input is selected
                     01_1100b - TRIG_IN2 input is selected
                     01_1101b - TRIG_IN3 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1010 / 3791

<!-- page 1011 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_1110b - TRIG_IN4 input is selected
                     01_1111b - TRIG_IN10 input is selected
                     10_0000b - TRIG_IN11 input is selected
                     10_0001b - FlexIO CH4 input is selected
                     10_0010b - FlexIO CH5 input is selected
                     10_0011b - FlexIO CH6 input is selected
                     10_0100b - FlexIO CH7 input is selected
                     10_0101b - USB0 ipp_ind_uart_rxd_usbmux input is selected
                     10_0110b - GPIO2 Pin Event Trig 0 input is selected
                     10_0111b - GPIO2 Pin Event Trig 1 input is selected
                     10_1000b - GPIO3 Pin Event Trig 0 input is selected
                     10_1001b - GPIO3 Pin Event Trig 1 input is selected
                     10_1010b - WUU input is selected
                               All other values are reserved.


26.5.1.48 LP_FLEXCOMM3 Trigger Input Connections (FLEXCOMM3_TRIG)

Offset


 Register                   Offset

 FLEXCOMM3_TRIG     600h


Function
This register is used to select the LP_FLEXCOMM3 trigger inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1011 / 3791

<!-- page 1012 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-6      Reserved
    —

       5-0     LP_FLEXCOMM3 trigger input connections
      INP            00_0000b - PINT PIN_INT4 input is selected
                     00_0001b - PINT PIN_INT5 input is selected
                     00_0010b - PINT PIN_INT7 input is selected
                     00_0011b - SCT_OUT5 input is selected
                     00_0100b - SCT_OUT8 input is selected
                     00_0101b - SCT_OUT9 input is selected
                     00_0110b - CTIMER0_MAT1 input is selected
                     00_0111b - CTIMER1_MAT1 input is selected
                     00_1000b - CTIMER2_MAT1 input is selected
                     00_1001b - CTIMER3_MAT1 input is selected
                     00_1010b - CTIMER4_MAT1 input is selected
                     00_1011b - LPTMR0 input is selected
                     00_1100b - LPTMR1 input is selected
                     00_1101b - Reserved
                     00_1110b - PINT GPIO_INT_BMAT input is selected
                     00_1111b - CMP0_OUT input is selected
                     01_0000b - CMP1_OUT input is selected
                     01_0001b - CMP2_OUT input is selected
                     01_0010b - EVTG_OUT0A input is selected
                     01_0011b - EVTG_OUT0B input is selected
                     01_0100b - EVTG_OUT1A input is selected
                     01_0101b - EVTG_OUT1B input is selected
                     01_0110b - EVTG_OUT2A input is selected
                     01_0111b - EVTG_OUT2B input is selected
                     01_1000b - EVTG_OUT3A input is selected
                     01_1001b - EVTG_OUT3B input is selected
                     01_1010b - TRIG_IN0 input is selected
                     01_1011b - TRIG_IN1 input is selected
                     01_1100b - TRIG_IN2 input is selected
                     01_1101b - TRIG_IN3 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1012 / 3791

<!-- page 1013 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_1110b - TRIG_IN4 input is selected
                     01_1111b - TRIG_IN10 input is selected
                     10_0000b - TRIG_IN11 input is selected
                     10_0001b - FlexIO CH4 input is selected
                     10_0010b - FlexIO CH5 input is selected
                     10_0011b - FlexIO CH6 input is selected
                     10_0100b - FlexIO CH7 input is selected
                     10_0101b - USB0 ipp_ind_uart_rxd_usbmux input is selected
                     10_0110b - GPIO2 Pin Event Trig 0 input is selected
                     10_0111b - GPIO2 Pin Event Trig 1 input is selected
                     10_1000b - GPIO3 Pin Event Trig 0 input is selected
                     10_1001b - GPIO3 Pin Event Trig 1 input is selected
                     10_1010b - WUU input is selected
                               All other values are reserved.


26.5.1.49 LP_FLEXCOMM4 Trigger Input Connections (FLEXCOMM4_TRIG)

Offset


 Register                   Offset

 FLEXCOMM4_TRIG     620h


Function
This register is used to select the LP_FLEXCOMM4 trigger inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1013 / 3791

<!-- page 1014 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-6      Reserved
    —

       5-0     LP_FLEXCOMM4 trigger input connections
      INP            00_0000b - PINT PIN_INT4 input is selected
                     00_0001b - PINT PIN_INT5 input is selected
                     00_0010b - PINT PIN_INT7 input is selected
                     00_0011b - SCT_OUT0 input is selected
                     00_0100b - SCT_OUT1 input is selected
                     00_0101b - SCT_OUT2 input is selected
                     00_0110b - CTIMER0_MAT1 input is selected
                     00_0111b - CTIMER1_MAT1 input is selected
                     00_1000b - CTIMER2_MAT2 input is selected
                     00_1001b - CTIMER3_MAT2 input is selected
                     00_1010b - CTIMER4_MAT2 input is selected
                     00_1011b - LPTMR0 input is selected
                     00_1100b - LPTMR1 input is selected
                     00_1101b - Reserved
                     00_1110b - PINT GPIO_INT_BMAT input is selected
                     00_1111b - CMP0_OUT input is selected
                     01_0000b - CMP1_OUT input is selected
                     01_0001b - CMP2_OUT input is selected
                     01_0010b - EVTG_OUT0A input is selected
                     01_0011b - EVTG_OUT0B input is selected
                     01_0100b - EVTG_OUT1A input is selected
                     01_0101b - EVTG_OUT1B input is selected
                     01_0110b - EVTG_OUT2A input is selected
                     01_0111b - EVTG_OUT2B input is selected
                     01_1000b - EVTG_OUT3A input is selected
                     01_1001b - EVTG_OUT3B input is selected
                     01_1010b - TRIG_IN0 input is selected
                     01_1011b - TRIG_IN1 input is selected
                     01_1100b - TRIG_IN2 input is selected
                     01_1101b - TRIG_IN3 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1014 / 3791

<!-- page 1015 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_1110b - TRIG_IN4 input is selected
                     01_1111b - TRIG_IN10 input is selected
                     10_0000b - TRIG_IN11 input is selected
                     10_0001b - FlexIO CH4 input is selected
                     10_0010b - FlexIO CH5 input is selected
                     10_0011b - FlexIO CH6 input is selected
                     10_0100b - FlexIO CH7 input is selected
                     10_0101b - USB0 ipp_ind_uart_rxd_usbmux input is selected
                     10_0110b - GPIO2 Pin Event Trig 0 input is selected
                     10_0111b - GPIO2 Pin Event Trig 1 input is selected
                     10_1000b - GPIO3 Pin Event Trig 0 input is selected
                     10_1001b - GPIO3 Pin Event Trig 1 input is selected
                     10_1010b - WUU input is selected
                               All other values are reserved.


26.5.1.50 LP_FLEXCOMM5 Trigger Input Connections (FLEXCOMM5_TRIG)

Offset


 Register                   Offset

 FLEXCOMM5_TRIG     640h


Function
This register is used to select the LP_FLEXCOMM5 trigger inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1015 / 3791

<!-- page 1016 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-6      Reserved
    —

       5-0     LP_FLEXCOMM5 trigger input connections
      INP            00_0000b - PINT PIN_INT4 input is selected
                     00_0001b - PINT PIN_INT5 input is selected
                     00_0010b - PINT PIN_INT7 input is selected
                     00_0011b - SCT_OUT0 input is selected
                     00_0100b - SCT_OUT1 input is selected
                     00_0101b - SCT_OUT2 input is selected
                     00_0110b - CTIMER0_MAT1 input is selected
                     00_0111b - CTIMER1_MAT1 input is selected
                     00_1000b - CTIMER2_MAT2 input is selected
                     00_1001b - CTIMER3_MAT2 input is selected
                     00_1010b - CTIMER4_MAT2 input is selected
                     00_1011b - LPTMR0 input is selected
                     00_1100b - LPTMR1 input is selected
                     00_1101b - Reserved
                     00_1110b - PINT GPIO_INT_BMAT input is selected
                     00_1111b - CMP0_OUT input is selected
                     01_0000b - CMP1_OUT input is selected
                     01_0001b - CMP2_OUT input is selected
                     01_0010b - EVTG_OUT0A input is selected
                     01_0011b - EVTG_OUT0B input is selected
                     01_0100b - EVTG_OUT1A input is selected
                     01_0101b - EVTG_OUT1B input is selected
                     01_0110b - EVTG_OUT2A input is selected
                     01_0111b - EVTG_OUT2B input is selected
                     01_1000b - EVTG_OUT3A input is selected
                     01_1001b - EVTG_OUT3B input is selected
                     01_1010b - TRIG_IN0 input is selected
                     01_1011b - TRIG_IN1 input is selected
                     01_1100b - TRIG_IN2 input is selected
                     01_1101b - TRIG_IN3 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1016 / 3791

<!-- page 1017 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_1110b - TRIG_IN4 input is selected
                     01_1111b - TRIG_IN10 input is selected
                     10_0000b - TRIG_IN11 input is selected
                     10_0001b - FlexIO CH4 input is selected
                     10_0010b - FlexIO CH5 input is selected
                     10_0011b - FlexIO CH6 input is selected
                     10_0100b - FlexIO CH7 input is selected
                     10_0101b - USB0 ipp_ind_uart_rxd_usbmux input is selected
                     10_0110b - GPIO2 Pin Event Trig 0 input is selected
                     10_0111b - GPIO2 Pin Event Trig 1 input is selected
                     10_1000b - GPIO3 Pin Event Trig 0 input is selected
                     10_1001b - GPIO3 Pin Event Trig 1 input is selected
                     10_1010b - WUU input is selected
                               All other values are reserved.


26.5.1.51 LP_FLEXCOMM6 Trigger Input Connections (FLEXCOMM6_TRIG)

Offset


 Register                   Offset

 FLEXCOMM6_TRIG     660h


Function
This register is used to select the LP_FLEXCOMM6 trigger inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1017 / 3791

<!-- page 1018 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-6      Reserved
    —

       5-0     LP_FLEXCOMM6 trigger input connections
      INP            00_0000b - PINT PIN_INT4 input is selected
                     00_0001b - PINT PIN_INT5 input is selected
                     00_0010b - PINT PIN_INT7 input is selected
                     00_0011b - SCT_OUT0 input is selected
                     00_0100b - SCT_OUT3 input is selected
                     00_0101b - SCT_OUT4 input is selected
                     00_0110b - CTIMER0_MAT1 input is selected
                     00_0111b - CTIMER1_MAT1 input is selected
                     00_1000b - CTIMER2_MAT3 input is selected
                     00_1001b - CTIMER3_MAT3 input is selected
                     00_1010b - CTIMER4_MAT3 input is selected
                     00_1011b - LPTMR0 input is selected
                     00_1100b - LPTMR1 input is selected
                     00_1101b - Reserved
                     00_1110b - PINT GPIO_INT_BMAT input is selected
                     00_1111b - CMP0_OUT input is selected
                     01_0000b - CMP1_OUT input is selected
                     01_0001b - CMP2_OUT input is selected
                     01_0010b - EVTG_OUT0A input is selected
                     01_0011b - EVTG_OUT0B input is selected
                     01_0100b - EVTG_OUT1A input is selected
                     01_0101b - EVTG_OUT1B input is selected
                     01_0110b - EVTG_OUT2A input is selected
                     01_0111b - EVTG_OUT2B input is selected
                     01_1000b - EVTG_OUT3A input is selected
                     01_1001b - EVTG_OUT3B input is selected
                     01_1010b - TRIG_IN0 input is selected
                     01_1011b - TRIG_IN1 input is selected
                     01_1100b - TRIG_IN2 input is selected
                     01_1101b - TRIG_IN3 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1018 / 3791

<!-- page 1019 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_1110b - TRIG_IN4 input is selected
                     01_1111b - TRIG_IN10 input is selected
                     10_0000b - TRIG_IN11 input is selected
                     10_0001b - FlexIO CH4 input is selected
                     10_0010b - FlexIO CH5 input is selected
                     10_0011b - FlexIO CH6 input is selected
                     10_0100b - FlexIO CH7 input is selected
                     10_0101b - USB0 ipp_ind_uart_rxd_usbmux input is selected
                     10_0110b - GPIO2 Pin Event Trig 0 input is selected
                     10_0111b - GPIO2 Pin Event Trig 1 input is selected
                     10_1000b - GPIO3 Pin Event Trig 0 input is selected
                     10_1001b - GPIO3 Pin Event Trig 1 input is selected
                     10_1010b - WUU input is selected
                               All other values are reserved.


26.5.1.52 LP_FLEXCOMM7 Trigger Input Connections (FLEXCOMM7_TRIG)

Offset


 Register                   Offset

 FLEXCOMM7_TRIG     680h


Function
This register is used to select the LP_FLEXCOMM7 trigger inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1019 / 3791

<!-- page 1020 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-6      Reserved
    —

       5-0     LP_FLEXCOMM7 trigger input connections
      INP            00_0000b - PINT PIN_INT4 input is selected
                     00_0001b - PINT PIN_INT5 input is selected
                     00_0010b - PINT PIN_INT7 input is selected
                     00_0011b - SCT_OUT0 input is selected
                     00_0100b - SCT_OUT3 input is selected
                     00_0101b - SCT_OUT4 input is selected
                     00_0110b - CTIMER0_MAT1 input is selected
                     00_0111b - CTIMER1_MAT1 input is selected
                     00_1000b - CTIMER2_MAT3 input is selected
                     00_1001b - CTIMER3_MAT3 input is selected
                     00_1010b - CTIMER4_MAT3 input is selected
                     00_1011b - LPTMR0 input is selected
                     00_1100b - LPTMR1 input is selected
                     00_1101b - Reserved
                     00_1110b - PINT GPIO_INT_BMAT input is selected
                     00_1111b - CMP0_OUT input is selected
                     01_0000b - CMP1_OUT input is selected
                     01_0001b - CMP2_OUT input is selected
                     01_0010b - EVTG_OUT0A input is selected
                     01_0011b - EVTG_OUT0B input is selected
                     01_0100b - EVTG_OUT1A input is selected
                     01_0101b - EVTG_OUT1B input is selected
                     01_0110b - EVTG_OUT2A input is selected
                     01_0111b - EVTG_OUT2B input is selected
                     01_1000b - EVTG_OUT3A input is selected
                     01_1001b - EVTG_OUT3B input is selected
                     01_1010b - TRIG_IN0 input is selected
                     01_1011b - TRIG_IN1 input is selected
                     01_1100b - TRIG_IN2 input is selected
                     01_1101b - TRIG_IN3 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1020 / 3791

<!-- page 1021 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_1110b - TRIG_IN4 input is selected
                     01_1111b - TRIG_IN10 input is selected
                     10_0000b - TRIG_IN11 input is selected
                     10_0001b - FlexIO CH4 input is selected
                     10_0010b - FlexIO CH5 input is selected
                     10_0011b - FlexIO CH6 input is selected
                     10_0100b - FlexIO CH7 input is selected
                     10_0101b - USB0 ipp_ind_uart_rxd_usbmux input is selected
                     10_0110b - GPIO2 Pin Event Trig 0 input is selected
                     10_0111b - GPIO2 Pin Event Trig 1 input is selected
                     10_1000b - GPIO3 Pin Event Trig 0 input is selected
                     10_1001b - GPIO3 Pin Event Trig 1 input is selected
                     10_1010b - WUU input is selected
                               All other values are reserved.


26.5.1.53 LP_FLEXCOMM8 Trigger Input Connections (FLEXCOMM8_TRIG)

Offset


 Register                   Offset

 FLEXCOMM8_TRIG     6A0h


Function
This register is used to select the LP_FLEXCOMM8 trigger inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1021 / 3791

<!-- page 1022 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-6      Reserved
    —

       5-0     LP_FLEXCOMM8 trigger input connections
      INP            00_0000b - PINT PIN_INT4 input is selected
                     00_0001b - PINT PIN_INT5 input is selected
                     00_0010b - PINT PIN_INT7 input is selected
                     00_0011b - SCT_OUT0 input is selected
                     00_0100b - SCT_OUT3 input is selected
                     00_0101b - SCT_OUT4 input is selected
                     00_0110b - CTIMER0_MAT1 input is selected
                     00_0111b - CTIMER1_MAT1 input is selected
                     00_1000b - CTIMER2_MAT3 input is selected
                     00_1001b - CTIMER3_MAT3 input is selected
                     00_1010b - CTIMER4_MAT3 input is selected
                     00_1011b - LPTMR0 input is selected
                     00_1100b - LPTMR1 input is selected
                     00_1101b - Reserved
                     00_1110b - PINT GPIO_INT_BMAT input is selected
                     00_1111b - CMP0_OUT input is selected
                     01_0000b - CMP1_OUT input is selected
                     01_0001b - CMP2_OUT input is selected
                     01_0010b - EVTG_OUT0A input is selected
                     01_0011b - EVTG_OUT0B input is selected
                     01_0100b - EVTG_OUT1A input is selected
                     01_0101b - EVTG_OUT1B input is selected
                     01_0110b - EVTG_OUT2A input is selected
                     01_0111b - EVTG_OUT2B input is selected
                     01_1000b - EVTG_OUT3A input is selected
                     01_1001b - EVTG_OUT3B input is selected
                     01_1010b - TRIG_IN0 input is selected
                     01_1011b - TRIG_IN1 input is selected
                     01_1100b - TRIG_IN2 input is selected
                     01_1101b - TRIG_IN3 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1022 / 3791

<!-- page 1023 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_1110b - TRIG_IN4 input is selected
                     01_1111b - TRIG_IN10 input is selected
                     10_0000b - TRIG_IN11 input is selected
                     10_0001b - FlexIO CH4 input is selected
                     10_0010b - FlexIO CH5 input is selected
                     10_0011b - FlexIO CH6 input is selected
                     10_0100b - FlexIO CH7 input is selected
                     10_0101b - USB0 ipp_ind_uart_rxd_usbmux input is selected
                     10_0110b - GPIO2 Pin Event Trig 0 input is selected
                     10_0111b - GPIO2 Pin Event Trig 1 input is selected
                     10_1000b - GPIO3 Pin Event Trig 0 input is selected
                     10_1001b - GPIO3 Pin Event Trig 1 input is selected
                     10_1010b - WUU input is selected
                               All other values are reserved.


26.5.1.54 LP_FLEXCOMM9 Trigger Input Connections (FLEXCOMM9_TRIG)

Offset


 Register                   Offset

 FLEXCOMM9_TRIG     6C0h


Function
This register is used to select the LP_FLEXCOMM9 trigger inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                          Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       1      1      1      1





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1023 / 3791

<!-- page 1024 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-6      Reserved
    —

       5-0     LP_FLEXCOMM9 trigger input connections
      INP            00_0000b - PINT PIN_INT4 input is selected
                     00_0001b - PINT PIN_INT5 input is selected
                     00_0010b - PINT PIN_INT7 input is selected
                     00_0011b - SCT_OUT0 input is selected
                     00_0100b - SCT_OUT3 input is selected
                     00_0101b - SCT_OUT4 input is selected
                     00_0110b - CTIMER0_MAT1 input is selected
                     00_0111b - CTIMER1_MAT1 input is selected
                     00_1000b - CTIMER2_MAT0 input is selected
                     00_1001b - CTIMER3_MAT0 input is selected
                     00_1010b - CTIMER4_MAT0 input is selected
                     00_1011b - LPTMR0 input is selected
                     00_1100b - LPTMR1 input is selected
                     00_1101b - Reserved
                     00_1110b - PINT GPIO_INT_BMAT input is selected
                     00_1111b - CMP0_OUT input is selected
                     01_0000b - CMP1_OUT input is selected
                     01_0001b - CMP2_OUT input is selected
                     01_0010b - EVTG_OUT0A input is selected
                     01_0011b - EVTG_OUT0B input is selected
                     01_0100b - EVTG_OUT1A input is selected
                     01_0101b - EVTG_OUT1B input is selected
                     01_0110b - EVTG_OUT2A input is selected
                     01_0111b - EVTG_OUT2B input is selected
                     01_1000b - EVTG_OUT3A input is selected
                     01_1001b - EVTG_OUT3B input is selected
                     01_1010b - TRIG_IN0 input is selected
                     01_1011b - TRIG_IN1 input is selected
                     01_1100b - TRIG_IN2 input is selected
                     01_1101b - TRIG_IN3 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1024 / 3791

<!-- page 1025 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     01_1110b - TRIG_IN4 input is selected
                     01_1111b - TRIG_IN10 input is selected
                     10_0000b - TRIG_IN11 input is selected
                     10_0001b - FlexIO CH4 input is selected
                     10_0010b - FlexIO CH5 input is selected
                     10_0011b - FlexIO CH6 input is selected
                     10_0100b - FlexIO CH7 input is selected
                     10_0101b - USB0 ipp_ind_uart_rxd_usbmux input is selected
                     10_0110b - GPIO2 Pin Event Trig 0 input is selected
                     10_0111b - GPIO2 Pin Event Trig 1 input is selected
                     10_1000b - GPIO3 Pin Event Trig 0 input is selected
                     10_1001b - GPIO3 Pin Event Trig 1 input is selected
                     10_1010b - WUU input is selected
                               All other values are reserved.


26.5.1.55  FlexIO Trigger Input Connections (FLEXIO_TRIG0 - FLEXIO_TRIG7)

Offset
For a = 0 to 7:


 Register                   Offset

 FLEXIO_TRIGa         6E0h + (a × 4h)


Function
This register is used to select the FlexIO trigger inputs.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                      Reserved                                                      INP
   W

Reset     0      0      0      0       0      0      0      0       0      1      1      1       1      1      1      1




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1025 / 3791

<!-- page 1026 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31-7      Reserved
    —

       6-0        Input number for FlexIO0.
      INP            000_0000b - PINT PIN_INT4 input is selected
                     000_0001b - PINT PIN_INT5 input is selected
                     000_0010b - PINT PIN_INT6 input is selected
                     000_0011b - PINT PIN_INT7 input is selected
                     000_0100b - SCT_OUT5 input is selected
                     000_0101b - SCT_OUT6 input is selected
                     000_0110b - SCT_OUT7 input is selected
                     000_0111b - SCT_OUT8 input is selected
                     000_1000b - SCT_OUT9 input is selected
                     000_1001b - T0_MAT1 input is selected
                     000_1010b - T1_MAT1 input is selected
                     000_1011b - T2_MAT1 input is selected
                     000_1100b - T3_MAT1 input is selected
                     000_1101b - T4_MAT1 input is selected
                     000_1110b - LPTMR0 input is selected
                     000_1111b - LPTMR1 input is selected
                     001_0000b - Reserved
                     001_0001b - PINT GPIO_INT_BMAT input is selected
                     001_0010b - ADC0_tcomp[0] input is selected
                     001_0011b - ADC0_tcomp[1] input is selected
                     001_0100b - ADC0_tcomp[2] input is selected
                     001_0101b - ADC0_tcomp[3] input is selected
                     001_0110b - ADC1_tcomp[0] input is selected
                     001_0111b - ADC1_tcomp[1] input is selected
                     001_1000b - ADC1_tcomp[2] input is selected
                     001_1001b - ADC1_tcomp[3] input is selected
                     001_1010b - CMP0_OUT input is selected
                     001_1011b - CMP1_OUT input is selected
                     001_1100b - CMP2_OUT input is selected
                     001_1101b - PWM0_SM0_MUX_TRIG0 input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1026 / 3791

<!-- page 1027 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     001_1110b - PWM0_SM0_MUX_TRIG1 input is selected
                     001_1111b - PWM0_SM1_MUX_TRIG0 input is selected
                     010_0000b - PWM0_SM1_MUX_TRIG1 input is selected
                     010_0001b - PWM0_SM2_MUX_TRIG0 input is selected
                     010_0010b - PWM0_SM2_MUX_TRIG1 input is selected
                     010_0011b - PWM0_SM3_MUX_TRIG0 input is selected
                     010_0100b - PWM0_SM3_MUX_TRIG1 input is selected
                     010_0101b - PWM1_SM0_MUX_TRIG0 input is selected
                     010_0110b - PWM1_SM0_MUX_TRIG1 input is selected
                     010_0111b - PWM1_SM1_MUX_TRIG0 input is selected
                     010_1000b - PWM1_SM1_MUX_TRIG1 input is selected
                     010_1001b - PWM1_SM2_MUX_TRIG0 input is selected
                     010_1010b - PWM1_SM2_MUX_TRIG1 input is selected
                     010_1011b - PWM1_SM3_MUX_TRIG0 input is selected
                     010_1100b - PWM1_SM3_MUX_TRIG1 input is selected
                     010_1101b - EVTG_OUT0A input is selected
                     010_1110b - EVTG_OUT0B input is selected
                     010_1111b - EVTG_OUT1A input is selected
                     011_0000b - EVTG_OUT1B input is selected
                     011_0001b - EVTG_OUT2A input is selected
                     011_0010b - EVTG_OUT2B input is selected
                     011_0011b - EVTG_OUT3A input is selected
                     011_0100b - EVTG_OUT3B input is selected
                     011_0101b - TRIG_IN0 input is selected
                     011_0110b - TRIG_IN1 input is selected
                     011_0111b - TRIG_IN2 input is selected
                     011_1000b - TRIG_IN3 input is selected
                     011_1001b - TRIG_IN4 input is selected
                     011_1010b - SINC Filter CH0 Conversion Complete input is selected
                     011_1011b - SINC Filter CH1 Conversion Complete input is selected
                     011_1100b - SINC Filter CH2 Conversion Complete input is selected
                     011_1101b - SINC Filter CH3 Conversion Complete input is selected
                     011_1110b - SINC Filter CH4 Conversion Complete input is selected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1027 / 3791

<!-- page 1028 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     011_1111b - LP_FLEXCOMM0 trig 0 (lpuart_trg_txword) input is selected
                     100_0000b - LP_FLEXCOMM0 trig 1 (lpuart_trg_rxword) input is selected
                     100_0001b - LP_FLEXCOMM0 trig 2 (lpuart_trg_rxidle) input is selected
                     100_0010b - LP_FLEXCOMM1 trig 0 input is selected
                     100_0011b - LP_FLEXCOMM1 trig 1 input is selected
                     100_0100b - LP_FLEXCOMM1 trig 2 input is selected
                     100_0101b - LP_FLEXCOMM2 trig 0 input is selected
                     100_0110b - LP_FLEXCOMM2 trig 1 input is selected
                     100_0111b - LP_FLEXCOMM2 trig 2 input is selected
                     100_1000b - LP_FLEXCOMM3 trig 0 input is selected
                     100_1001b - LP_FLEXCOMM3 trig 1 input is selected
                     100_1010b - LP_FLEXCOMM3 trig 2 input is selected
                     100_1011b - LP_FLEXCOMM3 trig 3 input is selected
                     100_1100b - WUU input is selected
                               All other values are reserved.


26.5.1.56 DMA0 Request Enable0 (DMA0_REQ_ENABLE0)

Offset


 Register                   Offset

 DMA0_REQ_ENABLE0   700h


Function
DMA request 0-31 enable for DMA0. One bit per request. 0: DMA request to DMA0 and response from DMA0 are blocked.
1:DMA request and response are enabled for DMA0.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R  REQ3  REQ3  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ1  REQ1  REQ1  REQ1
   W   1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...

Reset     1      1      1      1       1      1      1      1       1      1      1      1       1      1      1      1

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                                                                                   0       REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ9  REQ8  REQ7  REQ6  REQ5  REQ4  REQ3  REQ2  REQ1
           5_E...    4_E...    3_E...    2_E...    1_E...    0_E...   _EN0   _EN0   _EN0   _EN0   _EN0   _EN0   _EN0   _EN0   _EN0   W

Reset     1      1      1      1       1      1      1      1       1      1      1      1       1      1      1      1



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1028 / 3791

<!-- page 1029 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31        This register is used to enable and disable EVTG0 OUT0A request.
  REQ31_EN0        0b - Disable
                     1b - Enable

      30        This register is used to enable and disable CMP2 DMA_request.
  REQ30_EN0        0b - Disable
                     1b - Enable

      29        This register is used to enable and disable CMP1 DMA_request.
  REQ29_EN0        0b - Disable
                     1b - Enable

      28        This register is used to enable and disable CMP0 DMA_request.
  REQ28_EN0        0b - Disable
                     1b - Enable

      27        This register is used to enable and disable DAC2 FIFO_request.
  REQ27_EN0        0b - Disable
                     1b - Enable

      26        This register is used to enable and disable DAC1 FIFO_request.
  REQ26_EN0        0b - Disable
                     1b - Enable

      25        This register is used to enable and disable DAC0 FIFO_request.
  REQ25_EN0        0b - Disable
                     1b - Enable

      24        This register is used to enable and disable ADC1 FIFO B request.
  REQ24_EN0        0b - Disable
                     1b - Enable

      23        This register is used to enable and disable ADC1 FIFO A request.
  REQ23_EN0        0b - Disable
                     1b - Enable

      22        This register is used to enable and disable ADC0 FIFO B request.
  REQ22_EN0        0b - Disable
                     1b - Enable

      21        This register is used to enable and disable ADC0 FIFO A request.
  REQ21_EN0

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1029 / 3791

<!-- page 1030 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Disable
                     1b - Enable

      20        This register is used to enable and disable SCT0 DMA1 request.
  REQ20_EN0        0b - Disable
                     1b - Enable

      19        This register is used to enable and disable SCT0 DMA0 request.
  REQ19_EN0        0b - Disable
                     1b - Enable

      18        This register is used to enable and disable MICFIL0 FIFO_request.
  REQ18_EN0        0b - Disable
                     1b - Enable

      17        This register is used to enable and disable WUU0 wake up event request.
  REQ17_EN0        0b - Disable
                     1b - Enable

      16        This register is used to enable and disable CTIMER4 DMAREQ_M1 request.
  REQ16_EN0        0b - Disable
                     1b - Enable

      15        This register is used to enable and disable CTIMER4 DMAREQ_M0 request.
  REQ15_EN0        0b - Disable
                     1b - Enable

      14        This register is used to enable and disable CTIMER3 DMAREQ_M1 request.
  REQ14_EN0        0b - Disable
                     1b - Enable

      13        This register is used to enable and disable CTIMER3 DMAREQ_M0 request.
  REQ13_EN0        0b - Disable
                     1b - Enable

      12        This register is used to enable and disable CTIMER2 DMAREQ_M1 request.
  REQ12_EN0        0b - Disable
                     1b - Enable

      11        This register is used to enable and disable CTIMER2 DMAREQ_M0 request.
  REQ11_EN0        0b - Disable
                     1b - Enable

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1030 / 3791

<!-- page 1031 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      10        This register is used to enable and disable CTIMER1 DMAREQ_M1 request.
  REQ10_EN0        0b - Disable
                     1b - Enable

       9         This register is used to enable and disable CTIMER1 DMAREQ_M0 request.
  REQ9_EN0         0b - Disable
                     1b - Enable

       8         This register is used to enable and disable CTIMER0 DMAREQ_M1 request.
  REQ8_EN0         0b - Disable
                     1b - Enable

       7         This register is used to enable and disable CTIMER0 DMAREQ_M0 request.
  REQ7_EN0         0b - Disable
                     1b - Enable

       6         This register is used to enable and disable PINT0 INT3 request.
  REQ6_EN0         0b - Disable
                     1b - Enable

       5         This register is used to enable and disable PINT0 INT2 request.
  REQ5_EN0         0b - Disable
                     1b - Enable

       4         This register is used to enable and disable PINT0 INT1 request.
  REQ4_EN0         0b - Disable
                     1b - Enable

       3         This register is used to enable and disable PINT0 INT0 request.
  REQ3_EN0         0b - Disable
                     1b - Enable

       2         This register is used to enable and disable FLEXSPI0 transmit event request.
  REQ2_EN0         0b - Disable
                     1b - Enable

       1         This register is used to enable and disable FLEXSPI0 receive event request.
  REQ1_EN0         0b - Disable
                     1b - Enable

       0        Reserved

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1031 / 3791

<!-- page 1032 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

    —


26.5.1.57 DMA0 Request Enable0 (DMA0_REQ_ENABLE0_SET)

Offset


 Register                   Offset

 DMA0_REQ_ENABLE0_  704h
 SET


Function
Writing a 1 to a bit in this register sets the corresponding bit in DMA0_REQ_ENABLE0.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

       REQ3  REQ3  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ1  REQ1  REQ1  REQ1
   W
           1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                                                                                   0

       REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ9  REQ8  REQ7  REQ6  REQ5  REQ4  REQ3  REQ2  REQ1
   W
           5_E...    4_E...    3_E...    2_E...    1_E...    0_E...   _EN0   _EN0   _EN0   _EN0   _EN0   _EN0   _EN0   _EN0   _EN0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Writing a 1 to REQ31_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ31_EN0

      30         Writing a 1 to REQ30_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ30_EN0

      29         Writing a 1 to REQ29_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ29_EN0

      28         Writing a 1 to REQ28_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1032 / 3791

<!-- page 1033 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ28_EN0

      27         Writing a 1 to REQ27_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ27_EN0

      26         Writing a 1 to REQ26_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ26_EN0

      25         Writing a 1 to REQ25_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ25_EN0

      24         Writing a 1 to REQ24_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ24_EN0

      23         Writing a 1 to REQ23_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ23_EN0

      22         Writing a 1 to REQ22_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ22_EN0

      21         Writing a 1 to REQ21_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ21_EN0

      20         Writing a 1 to REQ20_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ20_EN0

      19         Writing a 1 to REQ19_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ19_EN0

      18         Writing a 1 to REQ18_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ18_EN0

      17         Writing a 1 to REQ17_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ17_EN0

      16         Writing a 1 to REQ16_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ16_EN0

      15         Writing a 1 to REQ15_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ15_EN0

      14         Writing a 1 to REQ14_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ14_EN0

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1033 / 3791

<!-- page 1034 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      13         Writing a 1 to REQ13_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ13_EN0

      12         Writing a 1 to REQ12_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ12_EN0

      11         Writing a 1 to REQ11_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ11_EN0

      10         Writing a 1 to REQ10_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ10_EN0

       9         Writing a 1 to REQ9_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ9_EN0

       8         Writing a 1 to REQ8_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ8_EN0

       7         Writing a 1 to REQ7_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ7_EN0

       6         Writing a 1 to REQ6_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ6_EN0

       5         Writing a 1 to REQ5_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ5_EN0

       4         Writing a 1 to REQ4_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ4_EN0

       3         Writing a 1 to REQ3_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ3_EN0

       2         Writing a 1 to REQ2_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ2_EN0

       1         Writing a 1 to REQ1_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE0.
  REQ1_EN0

       0        Reserved
    —





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1034 / 3791

<!-- page 1035 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

26.5.1.58 DMA0 Request Enable0 (DMA0_REQ_ENABLE0_CLR)

Offset


 Register                   Offset

 DMA0_REQ_ENABLE0_  708h
 CLR


Function
Writing a 1 to a bit in this register clears the corresponding bit in DMA0_REQ_ENABLE0.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

       REQ3  REQ3  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ1  REQ1  REQ1  REQ1
   W
           1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                                                                                   0

       REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ9  REQ8  REQ7  REQ6  REQ5  REQ4  REQ3  REQ2  REQ1
   W
           5_E...    4_E...    3_E...    2_E...    1_E...    0_E...   _EN0   _EN0   _EN0   _EN0   _EN0   _EN0   _EN0   _EN0   _EN0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Writing a 1 to REQ31_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ31_EN0

      30         Writing a 1 to REQ30_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ30_EN0

      29         Writing a 1 to REQ29_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ29_EN0

      28         Writing a 1 to REQ28_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ28_EN0

      27         Writing a 1 to REQ27_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ27_EN0

      26         Writing a 1 to REQ26_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1035 / 3791

<!-- page 1036 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ26_EN0

      25         Writing a 1 to REQ25_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ25_EN0

      24         Writing a 1 to REQ24_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ24_EN0

      23         Writing a 1 to REQ23_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ23_EN0

      22         Writing a 1 to REQ22_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ22_EN0

      21         Writing a 1 to REQ21_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ21_EN0

      20         Writing a 1 to REQ20_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ20_EN0

      19         Writing a 1 to REQ19_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ19_EN0

      18         Writing a 1 to REQ18_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ18_EN0

      17         Writing a 1 to REQ17_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ17_EN0

      16         Writing a 1 to REQ16_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ16_EN0

      15         Writing a 1 to REQ15_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ15_EN0

      14         Writing a 1 to REQ14_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ14_EN0

      13         Writing a 1 to REQ13_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ13_EN0

      12         Writing a 1 to REQ12_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ12_EN0

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1036 / 3791

<!-- page 1037 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      11         Writing a 1 to REQ11_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ11_EN0

      10         Writing a 1 to REQ10_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ10_EN0

       9         Writing a 1 to REQ9_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ9_EN0

       8         Writing a 1 to REQ8_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ8_EN0

       7         Writing a 1 to REQ7_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ7_EN0

       6         Writing a 1 to REQ6_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ6_EN0

       5         Writing a 1 to REQ5_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ5_EN0

       4         Writing a 1 to REQ4_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ4_EN0

       3         Writing a 1 to REQ3_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ3_EN0

       2         Writing a 1 to REQ2_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ2_EN0

       1         Writing a 1 to REQ1_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE0.
  REQ1_EN0

       0        Reserved
    —


26.5.1.59 DMA0 Request Enable0 (DMA0_REQ_ENABLE0_TOG)

Offset


 Register                   Offset

 DMA0_REQ_ENABLE0_  70Ch
 TOG


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1037 / 3791

<!-- page 1038 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Function
Writing a 1 to a bit in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

       REQ3  REQ3  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ1  REQ1  REQ1  REQ1
   W
           1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                                                                                   0

       REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ9  REQ8  REQ7  REQ6  REQ5  REQ4  REQ3  REQ2  REQ1
   W
           5_E...    4_E...    3_E...    2_E...    1_E...    0_E...   _EN0   _EN0   _EN0   _EN0   _EN0   _EN0   _EN0   _EN0   _EN0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Writing a 1 to REQ31_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ31_EN0

      30         Writing a 1 to REQ30_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ30_EN0

      29         Writing a 1 to REQ29_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ29_EN0

      28         Writing a 1 to REQ28_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ28_EN0

      27         Writing a 1 to REQ27_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ27_EN0

      26         Writing a 1 to REQ26_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ26_EN0

      25         Writing a 1 to REQ25_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ25_EN0

      24         Writing a 1 to REQ24_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ24_EN0

      23         Writing a 1 to REQ23_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1038 / 3791

<!-- page 1039 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ23_EN0

      22         Writing a 1 to REQ22_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ22_EN0

      21         Writing a 1 to REQ21_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ21_EN0

      20         Writing a 1 to REQ20_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ20_EN0

      19         Writing a 1 to REQ19_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ19_EN0

      18         Writing a 1 to REQ18_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ18_EN0

      17         Writing a 1 to REQ17_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ17_EN0

      16         Writing a 1 to REQ16_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ16_EN0

      15         Writing a 1 to REQ15_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ15_EN0

      14         Writing a 1 to REQ14_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ14_EN0

      13         Writing a 1 to REQ13_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ13_EN0

      12         Writing a 1 to REQ12_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ12_EN0

      11         Writing a 1 to REQ11_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ11_EN0

      10         Writing a 1 to REQ10_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ10_EN0

       9         Writing a 1 to RE9_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ9_EN0

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1039 / 3791

<!-- page 1040 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       8         Writing a 1 to REQ8_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ8_EN0

       7         Writing a 1 to REQ7_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ7_EN0

       6         Writing a 1 to REQ6_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ6_EN0

       5         Writing a 1 to REQ5_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ5_EN0

       4         Writing a 1 to REQ4_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ4_EN0

       3         Writing a 1 to REQ3_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ3_EN0

       2         Writing a 1 to REQ2_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ2_EN0

       1         Writing a 1 to REQ1_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE0.
  REQ1_EN0

       0        Reserved
    —


26.5.1.60 DMA0 Request Enable1 (DMA0_REQ_ENABLE1)

Offset


 Register                   Offset

 DMA0_REQ_ENABLE1   710h


Function
DMA request 32-63 enable for DMA0. One bit per request. 0: DMA request to DMA0 and response from DMA0 are blocked.
1:DMA request and response are enabled for DMA0.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1040 / 3791

<!-- page 1041 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                        0      0       REQ6  REQ6  REQ6  REQ6  REQ5  REQ5  REQ5              REQ5  REQ5  REQ5  REQ5  REQ5  REQ4  REQ4
           3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...                       4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...   W

Reset     1      1      1      1       1      1      1      1       1      1      1      1       1      1      1      1

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R  REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3
   W   7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...

Reset     1      1      1      1       1      1      1      1       1      1      1      1       1      1      1      1


Fields


       Field       Function
      31        This register is used to enable and disable FlexIO0 Shifter2 Status DMA request OR Timer2 Status DMA
                   request.
  REQ63_EN0
                     0b - Disable
                     1b - Enable

      30        This register is used to enable and disable FlexIO0 Shifter1 Status DMA request OR Timer1 Status DMA
                   request.
  REQ62_EN0
                     0b - Disable
                     1b - Enable

      29        This register is used to enable and disable FlexIO0 Shifter0 Status DMA request OR Timer0 Status DMA
                   request.
  REQ61_EN0
                     0b - Disable
                     1b - Enable

      28        This register is used to enable and disable CAN1 DMA request.
  REQ60_EN0        0b - Disable
                     1b - Enable

      27        This register is used to enable and disable CAN0 DMA request.
  REQ59_EN0        0b - Disable
                     1b - Enable

      26        This register is used to enable and disable LPTMR1 counter match event request.
  REQ58_EN0        0b - Disable
                     1b - Enable

      25        This register is used to enable and disable LPTMR0 counter match event request.
  REQ57_EN0        0b - Disable

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1041 / 3791

<!-- page 1042 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1b - Enable

      24       Reserved
    —

      23       Reserved
    —

      22        This register is used to enable and disable PWM1 Req_val3 request.
  REQ54_EN0        0b - Disable
                     1b - Enable

      21        This register is used to enable and disable PWM1 Req_val2 request.
  REQ53_EN0        0b - Disable
                     1b - Enable

      20        This register is used to enable and disable PWM1 Req_val1 request.
  REQ52_EN0        0b - Disable
                     1b - Enable

      19        This register is used to enable and disable PWM1 Req_val0 request.
  REQ51_EN0        0b - Disable
                     1b - Enable

      18        This register is used to enable and disable PWM1 Req_capt3 request.
  REQ50_EN0        0b - Disable
                     1b - Enable

      17        This register is used to enable and disable PWM1 Req_capt2 request.
  REQ49_EN0        0b - Disable
                     1b - Enable

      16        This register is used to enable and disable PWM1 Req_capt1 request.
  REQ48_EN0        0b - Disable
                     1b - Enable

      15        This register is used to enable and disable PWM1 Req_capt0 request.
  REQ47_EN0        0b - Disable
                     1b - Enable

      14        This register is used to enable and disable PWM0 Req_val3 request.
  REQ46_EN0        0b - Disable

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1042 / 3791

<!-- page 1043 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1b - Enable

      13        This register is used to enable and disable PWM0 Req_val2 request.
  REQ45_EN0        0b - Disable
                     1b - Enable

      12        This register is used to enable and disable PWM0 Req_val1 request.
  REQ44_EN0        0b - Disable
                     1b - Enable

      11        This register is used to enable and disable PWM0 Req_val0 request.
  REQ43_EN0        0b - Disable
                     1b - Enable

      10        This register is used to enable and disable PWM0 Req_capt3 request.
  REQ42_EN0        0b - Disable
                     1b - Enable

       9         This register is used to enable and disable PWM0 Req_capt2 request.
  REQ41_EN0        0b - Disable
                     1b - Enable

       8         This register is used to enable and disable PWM0 Req_capt1 request.
  REQ40_EN0        0b - Disable
                     1b - Enable

       7         This register is used to enable and disable PWM0 Req_capt0 request.
  REQ39_EN0        0b - Disable
                     1b - Enable

       6         This register is used to enable and disable EVTG0 OUT3B request.
  REQ38_EN0        0b - Disable
                     1b - Enable

       5         This register is used to enable and disable EVTG0 OUT3A request.
  REQ37_EN0        0b - Disable
                     1b - Enable

       4         This register is used to enable and disable EVTG0 OUT2B request.
  REQ36_EN0        0b - Disable
                     1b - Enable

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1043 / 3791

<!-- page 1044 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       3         This register is used to enable and disable EVTG0 OUT2A request.
  REQ35_EN0        0b - Disable
                     1b - Enable

       2         This register is used to enable and disable EVTG0 OUT1B request.
  REQ34_EN0        0b - Disable
                     1b - Enable

       1         This register is used to enable and disable EVTG0 OUT1A request.
  REQ33_EN0        0b - Disable
                     1b - Enable

       0         This register is used to enable and disable EVTG0 OUT0B request.
  REQ32_EN0        0b - Disable
                     1b - Enable


26.5.1.61 DMA0 Request Enable1 (DMA0_REQ_ENABLE1_SET)

Offset


 Register                   Offset

 DMA0_REQ_ENABLE1_  714h
 SET


Function
Writing a 1 to REQ_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                        0      0

       REQ6  REQ6  REQ6  REQ6  REQ5  REQ5  REQ5              REQ5  REQ5  REQ5  REQ5  REQ5  REQ4  REQ4
   W
           3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...                       4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R

       REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3
   W
           7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1044 / 3791

<!-- page 1045 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31         Writing a 1 to REQ63_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ63_EN0

      30         Writing a 1 to REQ62_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ62_EN0

      29         Writing a 1 to REQ61_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ61_EN0

      28         Writing a 1 to REQ60_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ60_EN0

      27         Writing a 1 to REQ59_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ59_EN0

      26         Writing a 1 to REQ58_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ58_EN0

      25         Writing a 1 to REQ57_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ57_EN0

      24       Reserved
    —

      23       Reserved
    —

      22         Writing a 1 to REQ54_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ54_EN0

      21         Writing a 1 to REQ53_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ53_EN0

      20         Writing a 1 to REQ52_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ52_EN0

      19         Writing a 1 to REQ51_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ51_EN0

      18         Writing a 1 to REQ50_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ50_EN0

      17         Writing a 1 to REQ49_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1045 / 3791

<!-- page 1046 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ49_EN0

      16         Writing a 1 to REQ48_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ48_EN0

      15         Writing a 1 to REQ47_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ47_EN0

      14         Writing a 1 to REQ46_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ46_EN0

      13         Writing a 1 to REQ45_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ45_EN0

      12         Writing a 1 to REQ44_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ44_EN0

      11         Writing a 1 to REQ43_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ43_EN0

      10         Writing a 1 to REQ42_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ42_EN0

       9         Writing a 1 to REQ41_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ41_EN0

       8         Writing a 1 to REQ40_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ40_EN0

       7         Writing a 1 to REQ39_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ39_EN0

       6         Writing a 1 to REQ38_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ38_EN0

       5         Writing a 1 to REQ37_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ37_EN0

       4         Writing a 1 to REQ36_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ36_EN0

       3         Writing a 1 to REQ35_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ35_EN0

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1046 / 3791

<!-- page 1047 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       2         Writing a 1 to REQ34_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ34_EN0

       1         Writing a 1 to REQ33_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ33_EN0

       0         Writing a 1 to REQ32_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE1.
  REQ32_EN0


26.5.1.62 DMA0 Request Enable1 (DMA0_REQ_ENABLE1_CLR)

Offset


 Register                   Offset

 DMA0_REQ_ENABLE1_  718h
 CLR


Function
Writing a 1 to a bit in this register clears the corresponding bit in DMA0_REQ_ENABLE1.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                        0      0

       REQ6  REQ6  REQ6  REQ6  REQ5  REQ5  REQ5              REQ5  REQ5  REQ5  REQ5  REQ5  REQ4  REQ4
   W
           3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...                       4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R

       REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3
   W
           7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Writing a 1 to REQ63_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ63_EN0

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1047 / 3791

<!-- page 1048 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      30         Writing a 1 to REQ62_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ62_EN0

      29         Writing a 1 to REQ61_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ61_EN0

      28         Writing a 1 to REQ60_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ60_EN0

      27         Writing a 1 to REQ59_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ59_EN0

      26         Writing a 1 to REQ58_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ58_EN0

      25         Writing a 1 to REQ57_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ57_EN0

      24       Reserved
    —

      23       Reserved
    —

      22         Writing a 1 to REQ54_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ54_EN0

      21         Writing a 1 to REQ53_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ53_EN0

      20         Writing a 1 to REQ52_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ52_EN0

      19         Writing a 1 to REQ51_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ51_EN0

      18         Writing a 1 to REQ50_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ50_EN0

      17         Writing a 1 to REQ49_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ49_EN0

      16         Writing a 1 to REQ48_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1048 / 3791

<!-- page 1049 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ48_EN0

      15         Writing a 1 to REQ47_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ47_EN0

      14         Writing a 1 to REQ46_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ46_EN0

      13         Writing a 1 to REQ45_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ45_EN0

      12         Writing a 1 to REQ44_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ44_EN0

      11         Writing a 1 to REQ43_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ43_EN0

      10         Writing a 1 to REQ42_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ42_EN0

       9         Writing a 1 to REQ41_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ41_EN0

       8         Writing a 1 to REQ40_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ40_EN0

       7         Writing a 1 to REQ39_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ39_EN0

       6         Writing a 1 to REQ38_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ38_EN0

       5         Writing a 1 to REQ37_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ37_EN0

       4         Writing a 1 to REQ36_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ36_EN0

       3         Writing a 1 to REQ35_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ35_EN0

       2         Writing a 1 to REQ34_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ34_EN0

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1049 / 3791

<!-- page 1050 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       1         Writing a 1 to REQ33_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ33_EN0

       0         Writing a 1 to REQ32_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE1.
  REQ32_EN0


26.5.1.63 DMA0 Request Enable1 (DMA0_REQ_ENABLE1_TOG)

Offset


 Register                   Offset

 DMA0_REQ_ENABLE1_  71Ch
 TOG


Function
Writing a 1 to REQ_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                        0      0

       REQ6  REQ6  REQ6  REQ6  REQ5  REQ5  REQ5              REQ5  REQ5  REQ5  REQ5  REQ5  REQ4  REQ4
   W
           3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...                       4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R

       REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3
   W
           7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Writing a 1 to REQ63_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ63_EN0

      30         Writing a 1 to REQ62_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ62_EN0

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1050 / 3791

<!-- page 1051 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      29         Writing a 1 to REQ61_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ61_EN0

      28         Writing a 1 to REQ60_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ60_EN0

      27         Writing a 1 to REQ59_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ59_EN0

      26         Writing a 1 to REQ58_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ58_EN0

      25         Writing a 1 to REQ57_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ57_EN0

      24       Reserved
    —

      23       Reserved
    —

      22         Writing a 1 to REQ54_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ54_EN0

      21         Writing a 1 to REQ53_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ53_EN0

      20         Writing a 1 to REQ52_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ52_EN0

      19         Writing a 1 to REQ51_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ51_EN0

      18         Writing a 1 to REQ50_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ50_EN0

      17         Writing a 1 to REQ49_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ49_EN0

      16         Writing a 1 to REQ48_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ48_EN0

      15         Writing a 1 to REQ47_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1051 / 3791

<!-- page 1052 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ47_EN0

      14         Writing a 1 to REQ46_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ46_EN0

      13         Writing a 1 to REQ55_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ45_EN0

      12         Writing a 1 to REQ44_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ44_EN0

      11         Writing a 1 to REQ43_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ43_EN0

      10         Writing a 1 to REQ42_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ42_EN0

       9         Writing a 1 to REQ41_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ41_EN0

       8         Writing a 1 to REQ40_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ40_EN0

       7         Writing a 1 to REQ39_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ39_EN0

       6         Writing a 1 to REQ38_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ38_EN0

       5         Writing a 1 to REQ37_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ37_EN0

       4         Writing a 1 to REQ36_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ36_EN0

       3         Writing a 1 to REQ35_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ35_EN0

       2         Writing a 1 to REQ34_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ34_EN0

       1         Writing a 1 to REQ33_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ33_EN0

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1052 / 3791

<!-- page 1053 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       0         Writing a 1 to REQ32_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE1.
  REQ32_EN0


26.5.1.64 DMA0 Request Enable2 (DMA0_REQ_ENABLE2)

Offset


 Register                   Offset

 DMA0_REQ_ENABLE2   720h


Function
DMA request 64-95 enable for DMA0. One bit per request. 0: DMA request to DMA0 and response from DMA0 are blocked.
1:DMA request and response are enabled for DMA0.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                         0      0       REQ9  REQ9  REQ9  REQ9  REQ9              REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8
           5_E...    4_E...    3_E...    2_E...    1_E...                       8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...   W

Reset     1      1      1      1       1      1      1      1       1      1      1      1       1      1      1      1

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ6  REQ6  REQ6  REQ6  REQ6  REQ6
   W   9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...

Reset     1      1      1      1       1      1      1      1       1      1      1      1       1      1      1      1


Fields


       Field       Function
      31        This register is used to enable and disable I3C0 receive request.
  REQ95_EN0        0b - Disable
                     1b - Enable

      30        This register is used to enable and disable EMVSIM1 transmit request.
  REQ94_EN0        0b - Disable
                     1b - Enable

      29        This register is used to enable and disable EMVSIM1 receive request.
  REQ93_EN0

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1053 / 3791

<!-- page 1054 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Disable
                     1b - Enable

      28        This register is used to enable and disable EMVSIM0 transmit request.
  REQ92_EN0        0b - Disable
                     1b - Enable

      27        This register is used to enable and disable EMVSIM0 receive request.
  REQ91_EN0        0b - Disable
                     1b - Enable

      26       Reserved
    —

      25       Reserved
    —

      24        This register is used to enable and disable LP_FLEXCOMM9 transmit request.
  REQ88_EN0        0b - Disable
                     1b - Enable

      23        This register is used to enable and disable LP_FLEXCOMM9 receive request.
  REQ87_EN0        0b - Disable
                     1b - Enable

      22        This register is used to enable and disable LP_FLEXCOMM8 transmit request.
  REQ86_EN0        0b - Disable
                     1b - Enable

      21        This register is used to enable and disable LP_FLEXCOMM8 receive request.
  REQ85_EN0        0b - Disable
                     1b - Enable

      20        This register is used to enable and disable LP_FLEXCOMM7 transmit request.
  REQ84_EN0        0b - Disable
                     1b - Enable

      19        This register is used to enable and disable LP_FLEXCOMM7 receive request.
  REQ83_EN0        0b - Disable
                     1b - Enable

      18        This register is used to enable and disable LP_FLEXCOMM6 transmit request.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1054 / 3791

<!-- page 1055 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ82_EN0        0b - Disable
                     1b - Enable

      17        This register is used to enable and disable LP_FLEXCOMM6 receive request.
  REQ81_EN0        0b - Disable
                     1b - Enable

      16        This register is used to enable and disable LP_FLEXCOMM5 transmit request.
  REQ80_EN0        0b - Disable
                     1b - Enable

      15        This register is used to enable and disable LP_FLEXCOMM5 receive request.
  REQ79_EN0        0b - Disable
                     1b - Enable

      14        This register is used to enable and disable LP_FLEXCOMM4 transmit request.
  REQ78_EN0        0b - Disable
                     1b - Enable

      13        This register is used to enable and disable LP_FLEXCOMM4 receive request.
  REQ77_EN0        0b - Disable
                     1b - Enable

      12        This register is used to enable and disable LP_FLEXCOMM3 transmit request.
  REQ76_EN0        0b - Disable
                     1b - Enable

      11        This register is used to enable and disable LP_FLEXCOMM3 receive request.
  REQ75_EN0        0b - Disable
                     1b - Enable

      10        This register is used to enable and disable LP_FLEXCOMM2 transmit request.
  REQ74_EN0        0b - Disable
                     1b - Enable

       9         This register is used to enable and disable LP_FLEXCOMM2 receive request.
  REQ73_EN0        0b - Disable
                     1b - Enable

       8         This register is used to enable and disable LP_FLEXCOMM1 transmit request.
  REQ72_EN0        0b - Disable
                     1b - Enable

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1055 / 3791

<!-- page 1056 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       7         This register is used to enable and disable LP_FLEXCOMM1 receive request.
  REQ71_EN0        0b - Disable
                     1b - Enable

       6         This register is used to enable and disable LP_FLEXCOMM0 transmit request.
  REQ70_EN0        0b - Disable
                     1b - Enable

       5         This register is used to enable and disable LP_FLEXCOMM0 receive request.
  REQ69_EN0        0b - Disable
                     1b - Enable

       4         This register is used to enable and disable FlexIO0 shift register 7 request.
  REQ68_EN0        0b - Disable
                     1b - Enable

       3         This register is used to enable and disable FlexIO0 shift register 6 request.
  REQ67_EN0        0b - Disable
                     1b - Enable

       2         This register is used to enable and disable FlexIO0 shift register 5 request.
  REQ66_EN0        0b - Disable
                     1b - Enable

       1         This register is used to enable and disable FlexIO0 shift register 4 request.
  REQ65_EN0        0b - Disable
                     1b - Enable

       0         This register is used to enable and disable FlexIO0 shift register 3 request.
  REQ64_EN0        0b - Disable
                     1b - Enable


26.5.1.65 DMA0 Request Enable2 (DMA0_REQ_ENABLE2_SET)

Offset


 Register                   Offset

 DMA0_REQ_ENABLE2_  724h
 SET


Function
Writing a 1 to a bit in this register sets the corresponding bit in DMA0_REQ_ENABLE2.


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1056 / 3791

<!-- page 1057 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                         0      0

       REQ9  REQ9  REQ9  REQ9  REQ9              REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8
   W
           5_E...    4_E...    3_E...    2_E...    1_E...                       8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R

       REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ6  REQ6  REQ6  REQ6  REQ6  REQ6
   W
           9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Writing a 1 to REQ95_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ95_EN0

      30         Writing a 1 to REQ94_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ94_EN0

      29         Writing a 1 to REQ93_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ93_EN0

      28         Writing a 1 to REQ92_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ92_EN0

      27         Writing a 1 to REQ91_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ91_EN0

      26       Reserved
    —

      25       Reserved
    —

      24         Writing a 1 to REQ88_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ88_EN0

      23         Writing a 1 to REQ87_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ87_EN0

      22         Writing a 1 to REQ86_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1057 / 3791

<!-- page 1058 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ86_EN0

      21         Writing a 1 to REQ85_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ85_EN0

      20         Writing a 1 to REQ84_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ84_EN0

      19         Writing a 1 to REQ83_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ83_EN0

      18         Writing a 1 to REQ82_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ82_EN0

      17         Writing a 1 to REQ81_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ81_EN0

      16         Writing a 1 to REQ80_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ80_EN0

      15         Writing a 1 to REQ79_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ79_EN0

      14         Writing a 1 to REQ78_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ78_EN0

      13         Writing a 1 to REQ77_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ77_EN0

      12         Writing a 1 to REQ876_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ76_EN0

      11         Writing a 1 to REQ75_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ75_EN0

      10         Writing a 1 to REQ74_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ74_EN0

       9         Writing a 1 to REQ73_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ73_EN0

       8         Writing a 1 to REQ72_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ72_EN0

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1058 / 3791

<!-- page 1059 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       7         Writing a 1 to REQ71_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ71_EN0

       6         Writing a 1 to REQ70_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ70_EN0

       5         Writing a 1 to REQ69_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ69_EN0

       4         Writing a 1 to REQ68_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ68_EN0

       3         Writing a 1 to REQ67_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ67_EN0

       2         Writing a 1 to REQ66_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ66_EN0

       1         Writing a 1 to REQ65_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ65_EN0

       0         Writing a 1 to REQ64_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE2.
  REQ64_EN0


26.5.1.66 DMA0 Request Enable2 (DMA0_REQ_ENABLE2_CLR)

Offset


 Register                   Offset

 DMA0_REQ_ENABLE2_  728h
 CLR


Function
Writing a 1 to a bit in this register clears the corresponding bit in DMA0_REQ_ENABLE2.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1059 / 3791

<!-- page 1060 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                         0      0

       REQ9  REQ9  REQ9  REQ9  REQ9              REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8
   W
           5_E...    4_E...    3_E...    2_E...    1_E...                       8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R

       REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ6  REQ6  REQ6  REQ6  REQ6  REQ6
   W
           9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Writing a 1 to REQ95_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ95_EN0

      30         Writing a 1 to REQ94_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ94_EN0

      29         Writing a 1 to REQ93_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ93_EN0

      28         Writing a 1 to REQ92_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ92_EN0

      27         Writing a 1 to REQ91_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ91_EN0

      26       Reserved
    —

      25       Reserved
    —

      24         Writing a 1 to REQ88_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ88_EN0

      23         Writing a 1 to REQ87_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ87_EN0

      22         Writing a 1 to REQ86_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1060 / 3791

<!-- page 1061 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ86_EN0

      21         Writing a 1 to REQ85_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ85_EN0

      20         Writing a 1 to REQ84_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ84_EN0

      19         Writing a 1 to REQ83_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ83_EN0

      18         Writing a 1 to REQ82_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ82_EN0

      17         Writing a 1 to REQ81_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ81_EN0

      16         Writing a 1 to REQ80_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ80_EN0

      15         Writing a 1 to REQ79_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ79_EN0

      14         Writing a 1 to REQ78_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ78_EN0

      13         Writing a 1 to REQ77_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ77_EN0

      12         Writing a 1 to REQ76_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ76_EN0

      11         Writing a 1 to REQ75_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ75_EN0

      10         Writing a 1 to REQ74_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ74_EN0

       9         Writing a 1 to REQ73_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ73_EN0

       8         Writing a 1 to REQ72_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ72_EN0

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1061 / 3791

<!-- page 1062 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       7         Writing a 1 to REQ71_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ71_EN0

       6         Writing a 1 to REQ70_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ70_EN0

       5         Writing a 1 to REQ69_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ69_EN0

       4         Writing a 1 to REQ68_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ68_EN0

       3         Writing a 1 to REQ67_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ67_EN0

       2         Writing a 1 to REQ66_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ66_EN0

       1         Writing a 1 to REQ65_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ65_EN0

       0         Writing a 1 to REQ64_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE2.
  REQ64_EN0


26.5.1.67 DMA0 Request Enable2 (DMA0_REQ_ENABLE2_TOG)

Offset


 Register                   Offset

 DMA0_REQ_ENABLE2_  72Ch
 TOG


Function
Writing a 1 to a bit in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1062 / 3791

<!-- page 1063 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                         0      0

       REQ9  REQ9  REQ9  REQ9  REQ9              REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8
   W
           5_E...    4_E...    3_E...    2_E...    1_E...                       8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R

       REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ6  REQ6  REQ6  REQ6  REQ6  REQ6
   W
           9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Writing a 1 to REQ95_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ95_EN0

      30         Writing a 1 to REQ94_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ94_EN0

      29         Writing a 1 to REQ93_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ93_EN0

      28         Writing a 1 to REQ92_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ92_EN0

      27         Writing a 1 to REQ91_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ91_EN0

      26       Reserved
    —

      25       Reserved
    —

      24         Writing a 1 to REQ88_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ88_EN0

      23         Writing a 1 to REQ87_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ87_EN0

      22         Writing a 1 to REQ86_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1063 / 3791

<!-- page 1064 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ86_EN0

      21         Writing a 1 to REQ85_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ85_EN0

      20         Writing a 1 to REQ84_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ84_EN0

      19         Writing a 1 to REQ83_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ83_EN0

      18         Writing a 1 to REQ82_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ82_EN0

      17         Writing a 1 to REQ81_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ81_EN0

      16         Writing a 1 to REQ80_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ80_EN0

      15         Writing a 1 to REQ79_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ79_EN0

      14         Writing a 1 to REQ78_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ78_EN0

      13         Writing a 1 to REQ77_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ77_EN0

      12         Writing a 1 to REQ76_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ76_EN0

      11         Writing a 1 to REQ75_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ75_EN0

      10         Writing a 1 to REQ74_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ74_EN0

       9         Writing a 1 to REQ73_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ73_EN0

       8         Writing a 1 to REQ72_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ72_EN0

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1064 / 3791

<!-- page 1065 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       7         Writing a 1 to REQ71_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ71_EN0

       6         Writing a 1 to REQ70_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ70_EN0

       5         Writing a 1 to REQ69_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ69_EN0

       4         Writing a 1 to REQ68_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ68_EN0

       3         Writing a 1 to REQ67_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ67_EN0

       2         Writing a 1 to REQ66_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ66_EN0

       1         Writing a 1 to REQ65_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ65_EN0

       0         Writing a 1 to REQ64_EN0 in this register toggles the corresponding bit in DMA0_REQ_ENABLE2.
  REQ64_EN0


26.5.1.68 DMA0 Request Enable3 (DMA0_REQ_ENABLE3)

Offset


 Register                   Offset

 DMA0_REQ_ENABLE3   730h


Function
DMA request 96-121 enable for DMA0. One bit per request. 0: DMA request to DMA0 and response from DMA0 are blocked.
1:DMA request and response are enabled for DMA0.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1065 / 3791

<!-- page 1066 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R    0      0      0      0      0      0                                           REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1
                                                                        21_...    20_...    19_...    18_...    17_...    16_...    15_...    14_...    13_...    12_...   W

Reset     0      0      0      0       0      0      1      1       1      1      1      1       1      1      1      1

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ9  REQ9  REQ9  REQ9
   W   11_...    10_...    09_...    08_...    07_...    06_...    05_...    04_...    03_...    02_...    01_...    00_...    9_E...    8_E...    7_E...    6_E...

Reset     1      1      1      1       1      1      1      1       1      1      1      1       1      1      1      1


Fields


       Field       Function
      31       Reserved
    —

      30       Reserved
    —

      29       Reserved
    —

      28       Reserved
    —

      27       Reserved
    —

      26       Reserved
    —

      25        This register is used to enable and disable TSI0 out of range request.
 REQ121_EN0        0b - Disable
                     1b - Enable

      24        This register is used to enable and disable TSI0 end of scan request.
 REQ120_EN0        0b - Disable
                     1b - Enable

      23        This register is used to enable and disable GPIO5 pin event request 1.
 REQ119_EN0        0b - Disable
                     1b - Enable

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1066 / 3791

<!-- page 1067 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      22        This register is used to enable and disable GPIO5 pin event request 0.
 REQ118_EN0        0b - Disable
                     1b - Enable

      21        This register is used to enable and disable GPIO4 pin event request 1.
 REQ117_EN0        0b - Disable
                     1b - Enable

      20        This register is used to enable and disable GPIO4 pin event request 0.
 REQ116_EN0        0b - Disable
                     1b - Enable

      19        This register is used to enable and disable GPIO3 pin event request 1.
 REQ115_EN0        0b - Disable
                     1b - Enable

      18        This register is used to enable and disable GPIO3 pin event request 0.
 REQ114_EN0        0b - Disable
                     1b - Enable

      17        This register is used to enable and disable GPIO2 pin event request 1.
 REQ113_EN0        0b - Disable
                     1b - Enable

      16        This register is used to enable and disable GPIO2 pin event request 0.
 REQ112_EN0        0b - Disable
                     1b - Enable

      15        This register is used to enable and disable GPIO1 pin event request 1.
 REQ111_EN0        0b - Disable
                     1b - Enable

      14        This register is used to enable and disable GPIO1 pin event request 0.
 REQ110_EN0        0b - Disable
                     1b - Enable

      13        This register is used to enable and disable GPIO0 pin event request 1.
 REQ109_EN0        0b - Disable
                     1b - Enable

      12        This register is used to enable and disable GPIO0 pin event request 0.
 REQ108_EN0

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1067 / 3791

<!-- page 1068 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Disable
                     1b - Enable

      11        This register is used to enable and disable SINC0 ipd_req_sinc[4] or ipd_req_alt [4] request.
 REQ107_EN0        0b - Disable
                     1b - Enable

      10        This register is used to enable and disable SINC0 ipd_req_sinc[3] or ipd_req_alt [3] request.
 REQ106_EN0        0b - Disable
                     1b - Enable

       9         This register is used to enable and disable SINC0 ipd_req_sinc[2] or ipd_req_alt [2] request.
 REQ105_EN0        0b - Disable
                     1b - Enable

       8         This register is used to enable and disable SINC0 ipd_req_sinc[1] or ipd_req_alt [1] request.
 REQ104_EN0        0b - Disable
                     1b - Enable

       7         This register is used to enable and disable SINC0 ipd_req_sinc[0] or ipd_req_alt [0] request.
 REQ103_EN0        0b - Disable
                     1b - Enable

       6         This register is used to enable and disable SAI1 transmit request.
 REQ102_EN0        0b - Disable
                     1b - Enable

       5         This register is used to enable and disable SAI1 receive request.
 REQ101_EN0        0b - Disable
                     1b - Enable

       4         This register is used to enable and disable SAI0 transmit request.
 REQ100_EN0        0b - Disable
                     1b - Enable

       3         This register is used to enable and disable SAI0 receive request.
  REQ99_EN0        0b - Disable
                     1b - Enable

       2         This register is used to enable and disable I3C1 transmit request.
  REQ98_EN0        0b - Disable
                     1b - Enable

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1068 / 3791

<!-- page 1069 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       1         This register is used to enable and disable I3C1 receive request.
  REQ97_EN0        0b - Disable
                     1b - Enable

       0         This register is used to enable and disable I3C0 transmit request.
  REQ96_EN0        0b - Disable
                     1b - Enable


26.5.1.69 DMA0 Request Enable3 (DMA0_REQ_ENABLE3_SET)

Offset


 Register                   Offset

 DMA0_REQ_ENABLE3_  734h
 SET


Function
Writing a 1 to a bit in this register sets the corresponding bit in DMA0_REQ_ENABLE3

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R    0      0      0      0      0      0

                                           REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1
   W
                                                                        21_...    20_...    19_...    18_...    17_...    16_...    15_...    14_...    13_...    12_...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R

       REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ9  REQ9  REQ9  REQ9
   W
            11_...    10_...    09_...    08_...    07_...    06_...    05_...    04_...    03_...    02_...    01_...    00_...    9_E...    8_E...    7_E...    6_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31       Reserved
    —

      30       Reserved

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1069 / 3791

<!-- page 1070 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

    —

      29       Reserved
    —

      28       Reserved
    —

      27       Reserved
    —

      26       Reserved
    —

      25         Writing a 1 to REQ121_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ121_EN0

      24         Writing a 1 to REQ120_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ120_EN0

      23         Writing a 1 to REQ119_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ119_EN0

      22         Writing a 1 to REQ118_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ118_EN0

      21         Writing a 1 to REQ117_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ117_EN0

      20         Writing a 1 to REQ116_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ116_EN0

      19         Writing a 1 to REQ115_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ115_EN0

      18         Writing a 1 to REQ114_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ114_EN0

      17         Writing a 1 to REQ113_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ113_EN0

      16         Writing a 1 to REQ112_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ112_EN0

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1070 / 3791

<!-- page 1071 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      15         Writing a 1 to REQ111_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ111_EN0

      14         Writing a 1 to REQ110_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ110_EN0

      13         Writing a 1 to REQ109_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ109_EN0

      12         Writing a 1 to REQ108_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ108_EN0

      11         Writing a 1 to REQ107_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ107_EN0

      10         Writing a 1 to REQ106_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ106_EN0

       9         Writing a 1 to REQ105_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ105_EN0

       8         Writing a 1 to REQ104_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ104_EN0

       7         Writing a 1 to REQ103_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ103_EN0

       6         Writing a 1 to REQ102_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ102_EN0

       5         Writing a 1 to REQ101_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ101_EN0

       4         Writing a 1 to REQ100_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ100_EN0

       3         Writing a 1 to REQ99_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
  REQ99_EN0

       2         Writing a 1 to REQ98_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
  REQ98_EN0

       1         Writing a 1 to REQ97_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1071 / 3791

<!-- page 1072 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ97_EN0

       0         Writing a 1 to REQ96_EN0 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
  REQ96_EN0


26.5.1.70 DMA0 Request Enable3 (DMA0_REQ_ENABLE3_CLR)

Offset


 Register                   Offset

 DMA0_REQ_ENABLE3_  738h
 CLR


Function
Writing a 1 to a bit in this register clears the corresponding bit in DMA0_REQ_ENABLE3

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R    0      0      0      0      0      0

                                           REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1
   W
                                                                        21_...    20_...    19_...    18_...    17_...    16_...    15_...    14_...    13_...    12_...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R

       REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ9  REQ9  REQ9  REQ9
   W
            11_...    10_...    09_...    08_...    07_...    06_...    05_...    04_...    03_...    02_...    01_...    00_...    9_E...    8_E...    7_E...    6_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31       Reserved
    —

      30       Reserved
    —

      29       Reserved

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1072 / 3791

<!-- page 1073 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

    —

      28       Reserved
    —

      27       Reserved
    —

      26       Reserved
    —

      25         Writing a 1 to REQ121_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ121_EN0

      24         Writing a 1 to REQ120_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ120_EN0

      23         Writing a 1 to REQ119_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ119_EN0

      22         Writing a 1 to REQ118_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ118_EN0

      21         Writing a 1 to REQ117_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ117_EN0

      20         Writing a 1 to REQ116_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ116_EN0

      19         Writing a 1 to REQ115_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ115_EN0

      18         Writing a 1 to REQ114_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ114_EN0

      17         Writing a 1 to REQ113_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ113_EN0

      16         Writing a 1 to REQ112_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ112_EN0

      15         Writing a 1 to REQ111_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ111_EN0

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1073 / 3791

<!-- page 1074 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      14         Writing a 1 to REQ110_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ110_EN0

      13         Writing a 1 to REQ109_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ109_EN0

      12         Writing a 1 to REQ108_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ108_EN0

      11         Writing a 1 to REQ107_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ107_EN0

      10         Writing a 1 to REQ106_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ106_EN0

       9         Writing a 1 to REQ105_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ105_EN0

       8         Writing a 1 to REQ104_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ104_EN0

       7         Writing a 1 to REQ103_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ103_EN0

       6         Writing a 1 to REQ102_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ102_EN0

       5         Writing a 1 to REQ101_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ101_EN0

       4         Writing a 1 to REQ100_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ100_EN0

       3         Writing a 1 to REQ99_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
  REQ99_EN0

       2         Writing a 1 to REQ98_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
  REQ98_EN0

       1         Writing a 1 to REQ97_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
  REQ97_EN0

       0         Writing a 1 to REQ96_EN0 in this register clears the corresponding bit in DMA0_REQ_ENABLE3

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1074 / 3791

<!-- page 1075 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ96_EN0


26.5.1.71 DMA1 Request Enable0 (DMA1_REQ_ENABLE0)

Offset


 Register                   Offset

 DMA1_REQ_ENABLE0   780h


Function
DMA request 0-31 enable for DMA1. One bit per request. 0: DMA request to DMA1 and response from DMA1 are blocked.
1:DMA request and response are enabled for DMA1.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R  REQ3  REQ3  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ1  REQ1  REQ1  REQ1
   W   1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...

Reset     1      1      1      1       1      1      1      1       1      1      1      1       1      1      1      1

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                                                                                   0       REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ9  REQ8  REQ7  REQ6  REQ5  REQ4  REQ3  REQ2  REQ1
           5_E...    4_E...    3_E...    2_E...    1_E...    0_E...   _EN1   _EN1   _EN1   _EN1   _EN1   _EN1   _EN1   _EN1   _EN1   W

Reset     1      1      1      1       1      1      1      1       1      1      1      1       1      1      1      1


Fields


       Field       Function
      31        This register is used to enable and disable EVTG0 OUT0A request.
  REQ31_EN1        0b - Disable
                     1b - Enable

      30        This register is used to enable and disable CMP2 DMA_request.
  REQ30_EN1        0b - Disable
                     1b - Enable

      29        This register is used to enable and disable CMP1 DMA_request.
  REQ29_EN1        0b - Disable
                     1b - Enable

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1075 / 3791

<!-- page 1076 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      28        This register is used to enable and disable CMP0 DMA_request.
  REQ28_EN1        0b - Disable
                     1b - Enable

      27        This register is used to enable and disable DAC2 FIFO_request.
  REQ27_EN1        0b - Disable
                     1b - Enable

      26        This register is used to enable and disable DAC1 FIFO_request.
  REQ26_EN1        0b - Disable
                     1b - Enable

      25        This register is used to enable and disable DAC0 FIFO_request.
  REQ25_EN1        0b - Disable
                     1b - Enable

      24        This register is used to enable and disable ADC1 FIFO B request.
  REQ24_EN1        0b - Disable
                     1b - Enable

      23        This register is used to enable and disable ADC1 FIFO A request.
  REQ23_EN1        0b - Disable
                     1b - Enable

      22        This register is used to enable and disable ADC0 FIFO B request.
  REQ22_EN1        0b - Disable
                     1b - Enable

      21        This register is used to enable and disable ADC0 FIFO A request.
  REQ21_EN1        0b - Disable
                     1b - Enable

      20        This register is used to enable and disable SCT0 DMA1 request.
  REQ20_EN1        0b - Disable
                     1b - Enable

      19        This register is used to enable and disable SCT0 DMA0 request.
  REQ19_EN1        0b - Disable
                     1b - Enable

      18        This register is used to enable and disable MICFIL0 FIFO_request.
  REQ18_EN1

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1076 / 3791

<!-- page 1077 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Disable
                     1b - Enable

      17        This register is used to enable and disable WUU0 wake up event request.
  REQ17_EN1        0b - Disable
                     1b - Enable

      16        This register is used to enable and disable CTIMER4 DMAREQ_M1 request.
  REQ16_EN1        0b - Disable
                     1b - Enable

      15        This register is used to enable and disable CTIMER4 DMAREQ_M0 request.
  REQ15_EN1        0b - Disable
                     1b - Enable

      14        This register is used to enable and disable CTIMER3 DMAREQ_M1 request.
  REQ14_EN1        0b - Disable
                     1b - Enable

      13        This register is used to enable and disable CTIMER3 DMAREQ_M0 request.
  REQ13_EN1        0b - Disable
                     1b - Enable

      12        This register is used to enable and disable CTIMER2 DMAREQ_M1 request.
  REQ12_EN1        0b - Disable
                     1b - Enable

      11        This register is used to enable and disable CTIMER2 DMAREQ_M0 request.
  REQ11_EN1        0b - Disable
                     1b - Enable

      10        This register is used to enable and disable CTIMER1 DMAREQ_M1 request.
  REQ10_EN1        0b - Disable
                     1b - Enable

       9         This register is used to enable and disable CTIMER1 DMAREQ_M0 request.
  REQ9_EN1         0b - Disable
                     1b - Enable

       8         This register is used to enable and disable CTIMER0 DMAREQ_M1 request.
  REQ8_EN1         0b - Disable
                     1b - Enable

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1077 / 3791

<!-- page 1078 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       7         This register is used to enable and disable CTIMER0 DMAREQ_M0 request.
  REQ7_EN1         0b - Disable
                     1b - Enable

       6         This register is used to enable and disable PINT0 INT3 request.
  REQ6_EN1         0b - Disable
                     1b - Enable

       5         This register is used to enable and disable PINT0 INT2 request.
  REQ5_EN1         0b - Disable
                     1b - Enable

       4         This register is used to enable and disable PINT0 INT1 request.
  REQ4_EN1         0b - Disable
                     1b - Enable

       3         This register is used to enable and disable PINT0 INT0 request.
  REQ3_EN1         0b - Disable
                     1b - Enable

       2         This register is used to enable and disable FLEXSPI0 transmit event request.
  REQ2_EN1         0b - Disable
                     1b - Enable

       1         This register is used to enable and disable FLEXSPI0 receive event request.
  REQ1_EN1         0b - Disable
                     1b - Enable

       0        Reserved
    —


26.5.1.72 DMA1 Request Enable0 (DMA1_REQ_ENABLE0_SET)

Offset


 Register                   Offset

 DMA1_REQ_ENABLE0_  784h
 SET


Function
Writing a 1 to a bit in this register sets the corresponding bit in DMA1_REQ_ENABLE0.




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1078 / 3791

<!-- page 1079 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

       REQ3  REQ3  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ1  REQ1  REQ1  REQ1
   W
           1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                                                                                   0

       REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ9  REQ8  REQ7  REQ6  REQ5  REQ4  REQ3  REQ2  REQ1
   W
           5_E...    4_E...    3_E...    2_E...    1_E...    0_E...   _EN1   _EN1   _EN1   _EN1   _EN1   _EN1   _EN1   _EN1   _EN1

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Writing a 1 to REQ31_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ31_EN1

      30         Writing a 1 to REQ30_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ30_EN1

      29         Writing a 1 to REQ29_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ29_EN1

      28         Writing a 1 to REQ28_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ28_EN1

      27         Writing a 1 to REQ27_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ27_EN1

      26         Writing a 1 to REQ26_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ26_EN1

      25         Writing a 1 to REQ25_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ25_EN1

      24         Writing a 1 to REQ24_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ24_EN1

      23         Writing a 1 to REQ23_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ23_EN1

      22         Writing a 1 to REQ22_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1079 / 3791

<!-- page 1080 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ22_EN1

      21         Writing a 1 to REQ21_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ21_EN1

      20         Writing a 1 to REQ20_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ20_EN1

      19         Writing a 1 to REQ19_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ19_EN1

      18         Writing a 1 to REQ18_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ18_EN1

      17         Writing a 1 to REQ17_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ17_EN1

      16         Writing a 1 to REQ16_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ16_EN1

      15         Writing a 1 to REQ15_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ15_EN1

      14         Writing a 1 to REQ14_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ14_EN1

      13         Writing a 1 to REQ13_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ13_EN1

      12         Writing a 1 to REQ12_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ12_EN1

      11         Writing a 1 to REQ11_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ11_EN1

      10         Writing a 1 to REQ10_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ10_EN1

       9         Writing a 1 to REQ9_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ9_EN1

       8         Writing a 1 to REQ8_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ8_EN1

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1080 / 3791

<!-- page 1081 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       7         Writing a 1 to REQ7_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ7_EN1

       6         Writing a 1 to REQ6_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ6_EN1

       5         Writing a 1 to REQ5_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ5_EN1

       4         Writing a 1 to REQ4_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ4_EN1

       3         Writing a 1 to REQ3_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ3_EN1

       2         Writing a 1 to REQ2_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ2_EN1

       1         Writing a 1 to REQ1_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE0.
  REQ1_EN1

       0        Reserved
    —


26.5.1.73 DMA1 Request Enable0 (DMA1_REQ_ENABLE0_CLR)

Offset


 Register                   Offset

 DMA1_REQ_ENABLE0_  788h
 CLR


Function
Writing a 1 to a bit in this register clears the corresponding bit in DMA1_REQ_ENABLE0.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1081 / 3791

<!-- page 1082 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

       REQ3  REQ3  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ1  REQ1  REQ1  REQ1
   W
           1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                                                                                   0

       REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ9  REQ8  REQ7  REQ6  REQ5  REQ4  REQ3  REQ2  REQ1
   W
           5_E...    4_E...    3_E...    2_E...    1_E...    0_E...   _EN1   _EN1   _EN1   _EN1   _EN1   _EN1   _EN1   _EN1   _EN1

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Writing a 1 to REQ31_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ31_EN1

      30         Writing a 1 to REQ30_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ30_EN1

      29         Writing a 1 to REQ29_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ29_EN1

      28         Writing a 1 to REQ28_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ28_EN1

      27         Writing a 1 to REQ27_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ27_EN1

      26         Writing a 1 to REQ26_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ26_EN1

      25         Writing a 1 to REQ25_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ25_EN1

      24         Writing a 1 to REQ24_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ24_EN1

      23         Writing a 1 to REQ23_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ23_EN1

      22         Writing a 1 to REQ22_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1082 / 3791

<!-- page 1083 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ22_EN1

      21         Writing a 1 to REQ21_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ21_EN1

      20         Writing a 1 to REQ20_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ20_EN1

      19         Writing a 1 to REQ19_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ19_EN1

      18         Writing a 1 to REQ18_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ18_EN1

      17         Writing a 1 to REQ17_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ17_EN1

      16         Writing a 1 to REQ16_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ16_EN1

      15         Writing a 1 to REQ15_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ15_EN1

      14         Writing a 1 to REQ14_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ14_EN1

      13         Writing a 1 to REQ13_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ13_EN1

      12         Writing a 1 to REQ12_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ12_EN1

      11         Writing a 1 to REQ11_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ11_EN1

      10         Writing a 1 to REQ10_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ10_EN1

       9         Writing a 1 to REQ9_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ9_EN1

       8         Writing a 1 to REQ8_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ8_EN1

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1083 / 3791

<!-- page 1084 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       7         Writing a 1 to REQ7_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ7_EN1

       6         Writing a 1 to REQ6_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ6_EN1

       5         Writing a 1 to REQ5_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ5_EN1

       4         Writing a 1 to REQ4_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ4_EN1

       3         Writing a 1 to REQ3_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ3_EN1

       2         Writing a 1 to REQ2_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ2_EN1

       1         Writing a 1 to REQ1_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE0.
  REQ1_EN1

       0        Reserved
    —


26.5.1.74 DMA1 Request Enable0 (DMA1_REQ_ENABLE0_TOG)

Offset


 Register                   Offset

 DMA1_REQ_ENABLE0_  78Ch
 TOG


Function
Writing a 1 to a bit in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1084 / 3791

<!-- page 1085 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

       REQ3  REQ3  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ2  REQ1  REQ1  REQ1  REQ1
   W
           1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                                                                                   0

       REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ9  REQ8  REQ7  REQ6  REQ5  REQ4  REQ3  REQ2  REQ1
   W
           5_E...    4_E...    3_E...    2_E...    1_E...    0_E...   _EN1   _EN1   _EN1   _EN1   _EN1   _EN1   _EN1   _EN1   _EN1

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Writing a 1 to REQ31_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ31_EN1

      30         Writing a 1 to REQ30_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ30_EN1

      29         Writing a 1 to REQ29_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ29_EN1

      28         Writing a 1 to REQ28_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ28_EN1

      27         Writing a 1 to REQ27_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ27_EN1

      26         Writing a 1 to REQ26_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ26_EN1

      25         Writing a 1 to REQ25_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ25_EN1

      24         Writing a 1 to REQ24_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ24_EN1

      23         Writing a 1 to REQ23_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ23_EN1

      22         Writing a 1 to REQ22_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1085 / 3791

<!-- page 1086 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ22_EN1

      21         Writing a 1 to REQ21_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ21_EN1

      20         Writing a 1 to REQ20_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ20_EN1

      19         Writing a 1 to REQ19_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ19_EN1

      18         Writing a 1 to REQ18_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ18_EN1

      17         Writing a 1 to REQ17_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ17_EN1

      16         Writing a 1 to REQ16_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ16_EN1

      15         Writing a 1 to REQ15_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ15_EN1

      14         Writing a 1 to REQ14_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ14_EN1

      13         Writing a 1 to REQ13_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ13_EN1

      12         Writing a 1 to REQ12_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ12_EN1

      11         Writing a 1 to REQ11_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ11_EN1

      10         Writing a 1 to REQ10_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ10_EN1

       9         Writing a 1 to RE9_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ9_EN1

       8         Writing a 1 to REQ8_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ8_EN1

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1086 / 3791

<!-- page 1087 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       7         Writing a 1 to REQ7_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ7_EN1

       6         Writing a 1 to REQ6_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ6_EN1

       5         Writing a 1 to REQ5_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ5_EN1

       4         Writing a 1 to REQ4_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ4_EN1

       3         Writing a 1 to REQ3_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ3_EN1

       2         Writing a 1 to REQ2_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ2_EN1

       1         Writing a 1 to REQ1_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE0.
  REQ1_EN1

       0        Reserved
    —


26.5.1.75 DMA1 Request Enable1 (DMA1_REQ_ENABLE1)

Offset


 Register                   Offset

 DMA1_REQ_ENABLE1   790h


Function
DMA request 32-63 enable for DMA1. One bit per request. 0: DMA request to DMA1 and response from DMA1 are blocked.
1:DMA request and response are enabled for DMA1.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1087 / 3791

<!-- page 1088 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                        0      0       REQ6  REQ6  REQ6  REQ6  REQ5  REQ5  REQ5              REQ5  REQ5  REQ5  REQ5  REQ5  REQ4  REQ4
           3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...                       4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...   W

Reset     1      1      1      1       1      1      1      1       1      1      1      1       1      1      1      1

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R  REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3
   W   7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...

Reset     1      1      1      1       1      1      1      1       1      1      1      1       1      1      1      1


Fields


       Field       Function
      31        This register is used to enable and disable FlexIO0 Shifter2 Status DMA request OR Timer2 Status DMA
                   request.
  REQ63_EN1
                     0b - Disable
                     1b - Enable

      30        This register is used to enable and disable FlexIO0 Shifter1 Status DMA request OR Timer1 Status DMA
                   request.
  REQ62_EN1
                     0b - Disable
                     1b - Enable

      29        This register is used to enable and disable FlexIO0 Shifter0 Status DMA request OR Timer0 Status DMA
                   request.
  REQ61_EN1
                     0b - Disable
                     1b - Enable

      28        This register is used to enable and disable CAN1 DMA request.
  REQ60_EN1        0b - Disable
                     1b - Enable

      27        This register is used to enable and disable CAN0 DMA request.
  REQ59_EN1        0b - Disable
                     1b - Enable

      26        This register is used to enable and disable LPTMR1 counter match event request.
  REQ58_EN1        0b - Disable
                     1b - Enable

      25        This register is used to enable and disable LPTMR0 counter match event request.
  REQ57_EN1        0b - Disable

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1088 / 3791

<!-- page 1089 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1b - Enable

      24       Reserved
    —

      23       Reserved
    —

      22        This register is used to enable and disable PWM1 Req_val3 request.
  REQ54_EN1        0b - Disable
                     1b - Enable

      21        This register is used to enable and disable PWM1 Req_val2 request.
  REQ53_EN1        0b - Disable
                     1b - Enable

      20        This register is used to enable and disable PWM1 Req_val1 request.
  REQ52_EN1        0b - Disable
                     1b - Enable

      19        This register is used to enable and disable PWM1 Req_val0 request.
  REQ51_EN1        0b - Disable
                     1b - Enable

      18        This register is used to enable and disable PWM1 Req_capt3 request.
  REQ50_EN1        0b - Disable
                     1b - Enable

      17        This register is used to enable and disable PWM1 Req_capt2 request.
  REQ49_EN1        0b - Disable
                     1b - Enable

      16        This register is used to enable and disable PWM1 Req_capt1 request.
  REQ48_EN1        0b - Disable
                     1b - Enable

      15        This register is used to enable and disable PWM1 Req_capt0 request.
  REQ47_EN1        0b - Disable
                     1b - Enable

      14        This register is used to enable and disable PWM0 Req_val3 request.
  REQ46_EN1        0b - Disable

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1089 / 3791

<!-- page 1090 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1b - Enable

      13        This register is used to enable and disable PWM0 Req_val2 request.
  REQ45_EN1        0b - Disable
                     1b - Enable

      12        This register is used to enable and disable PWM0 Req_val1 request.
  REQ44_EN1        0b - Disable
                     1b - Enable

      11        This register is used to enable and disable PWM0 Req_val0 request.
  REQ43_EN1        0b - Disable
                     1b - Enable

      10        This register is used to enable and disable PWM0 Req_capt3 request.
  REQ42_EN1        0b - Disable
                     1b - Enable

       9         This register is used to enable and disable PWM0 Req_capt2 request.
  REQ41_EN1        0b - Disable
                     1b - Enable

       8         This register is used to enable and disable PWM0 Req_capt1 request.
  REQ40_EN1        0b - Disable
                     1b - Enable

       7         This register is used to enable and disable PWM0 Req_capt0 request.
  REQ39_EN1        0b - Disable
                     1b - Enable

       6         This register is used to enable and disable EVTG0 OUT3B request.
  REQ38_EN1        0b - Disable
                     1b - Enable

       5         This register is used to enable and disable EVTG0 OUT3A request.
  REQ37_EN1        0b - Disable
                     1b - Enable

       4         This register is used to enable and disable EVTG0 OUT2B request.
  REQ36_EN1        0b - Disable
                     1b - Enable

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1090 / 3791

<!-- page 1091 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       3         This register is used to enable and disable EVTG0 OUT2A request.
  REQ35_EN1        0b - Disable
                     1b - Enable

       2         This register is used to enable and disable EVTG0 OUT1B request.
  REQ34_EN1        0b - Disable
                     1b - Enable

       1         This register is used to enable and disable EVTG0 OUT1A request.
  REQ33_EN1        0b - Disable
                     1b - Enable

       0         This register is used to enable and disable EVTG0 OUT0B request.
  REQ32_EN1        0b - Disable
                     1b - Enable


26.5.1.76 DMA1 Request Enable1 (DMA1_REQ_ENABLE1_SET)

Offset


 Register                   Offset

 DMA1_REQ_ENABLE1_  794h
 SET


Function
Writing a 1 to REQ_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                        0      0

       REQ6  REQ6  REQ6  REQ6  REQ5  REQ5  REQ5              REQ5  REQ5  REQ5  REQ5  REQ5  REQ4  REQ4
   W
           3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...                       4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R

       REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3
   W
           7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1091 / 3791

<!-- page 1092 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Fields


       Field       Function
      31         Writing a 1 to REQ63_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ63_EN1

      30         Writing a 1 to REQ62_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ62_EN1

      29         Writing a 1 to REQ61_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ61_EN1

      28         Writing a 1 to REQ60_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ60_EN1

      27         Writing a 1 to REQ59_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ59_EN1

      26         Writing a 1 to REQ58_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ58_EN1

      25         Writing a 1 to REQ57_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ57_EN1

      24       Reserved
    —

      23       Reserved
    —

      22         Writing a 1 to REQ54_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ54_EN1

      21         Writing a 1 to REQ53_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ53_EN1

      20         Writing a 1 to REQ52_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ52_EN1

      19         Writing a 1 to REQ51_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ51_EN1

      18         Writing a 1 to REQ50_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ50_EN1

      17         Writing a 1 to REQ49_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1092 / 3791

<!-- page 1093 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ49_EN1

      16         Writing a 1 to REQ48_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ48_EN1

      15         Writing a 1 to REQ47_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ47_EN1

      14         Writing a 1 to REQ46_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ46_EN1

      13         Writing a 1 to REQ45_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ45_EN1

      12         Writing a 1 to REQ44_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ44_EN1

      11         Writing a 1 to REQ43_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ43_EN1

      10         Writing a 1 to REQ42_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ42_EN1

       9         Writing a 1 to REQ41_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ41_EN1

       8         Writing a 1 to REQ40_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ40_EN1

       7         Writing a 1 to REQ39_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ39_EN1

       6         Writing a 1 to REQ38_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ38_EN1

       5         Writing a 1 to REQ37_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ37_EN1

       4         Writing a 1 to REQ36_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ36_EN1

       3         Writing a 1 to REQ35_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ35_EN1

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1093 / 3791

<!-- page 1094 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       2         Writing a 1 to REQ34_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ34_EN1

       1         Writing a 1 to REQ33_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ33_EN1

       0         Writing a 1 to REQ32_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE1.
  REQ32_EN1


26.5.1.77 DMA1 Request Enable1 (DMA1_REQ_ENABLE1_CLR)

Offset


 Register                   Offset

 DMA1_REQ_ENABLE1_  798h
 CLR


Function
Writing a 1 to a bit in this register clears the corresponding bit in DMA1_REQ_ENABLE1.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                        0      0

       REQ6  REQ6  REQ6  REQ6  REQ5  REQ5  REQ5              REQ5  REQ5  REQ5  REQ5  REQ5  REQ4  REQ4
   W
           3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...                       4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R

       REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3
   W
           7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Writing a 1 to REQ63_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ63_EN1

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1094 / 3791

<!-- page 1095 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      30         Writing a 1 to REQ62_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ62_EN1

      29         Writing a 1 to REQ61_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ61_EN1

      28         Writing a 1 to REQ60_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ60_EN1

      27         Writing a 1 to REQ59_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ59_EN1

      26         Writing a 1 to REQ58_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ58_EN1

      25         Writing a 1 to REQ57_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ57_EN1

      24       Reserved
    —

      23       Reserved
    —

      22         Writing a 1 to REQ54_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ54_EN1

      21         Writing a 1 to REQ53_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ53_EN1

      20         Writing a 1 to REQ52_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ52_EN1

      19         Writing a 1 to REQ51_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ51_EN1

      18         Writing a 1 to REQ50_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ50_EN1

      17         Writing a 1 to REQ49_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ49_EN1

      16         Writing a 1 to REQ48_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1095 / 3791

<!-- page 1096 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ48_EN1

      15         Writing a 1 to REQ47_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ47_EN1

      14         Writing a 1 to REQ46_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ46_EN1

      13         Writing a 1 to REQ45_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ45_EN1

      12         Writing a 1 to REQ44_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ44_EN1

      11         Writing a 1 to REQ43_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ43_EN1

      10         Writing a 1 to REQ42_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ42_EN1

       9         Writing a 1 to REQ41_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ41_EN1

       8         Writing a 1 to REQ40_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ40_EN1

       7         Writing a 1 to REQ39_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ39_EN1

       6         Writing a 1 to REQ38_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ38_EN1

       5         Writing a 1 to REQ37_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ37_EN1

       4         Writing a 1 to REQ36_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ36_EN1

       3         Writing a 1 to REQ35_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ35_EN1

       2         Writing a 1 to REQ34_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ34_EN1

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1096 / 3791

<!-- page 1097 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       1         Writing a 1 to REQ33_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ33_EN1

       0         Writing a 1 to REQ32_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE1.
  REQ32_EN1


26.5.1.78 DMA1 Request Enable1 (DMA1_REQ_ENABLE1_TOG)

Offset


 Register                   Offset

 DMA1_REQ_ENABLE1_  79Ch
 TOG


Function
Writing a 1 to REQ_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                        0      0

       REQ6  REQ6  REQ6  REQ6  REQ5  REQ5  REQ5              REQ5  REQ5  REQ5  REQ5  REQ5  REQ4  REQ4
   W
           3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...                       4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R

       REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ4  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3  REQ3
   W
           7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Writing a 1 to REQ63_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ63_EN1

      30         Writing a 1 to REQ62_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ62_EN1

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1097 / 3791

<!-- page 1098 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      29         Writing a 1 to REQ61_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ61_EN1

      28         Writing a 1 to REQ60_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ60_EN1

      27         Writing a 1 to REQ59_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ59_EN1

      26         Writing a 1 to REQ58_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ58_EN1

      25         Writing a 1 to REQ57_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ57_EN1

      24       Reserved
    —

      23       Reserved
    —

      22         Writing a 1 to REQ54_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ54_EN1

      21         Writing a 1 to REQ53_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ53_EN1

      20         Writing a 1 to REQ52_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ52_EN1

      19         Writing a 1 to REQ51_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ51_EN1

      18         Writing a 1 to REQ50_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ50_EN1

      17         Writing a 1 to REQ49_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ49_EN1

      16         Writing a 1 to REQ48_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ48_EN1

      15         Writing a 1 to REQ47_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1098 / 3791

<!-- page 1099 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ47_EN1

      14         Writing a 1 to REQ46_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ46_EN1

      13         Writing a 1 to REQ55_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ45_EN1

      12         Writing a 1 to REQ44_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ44_EN1

      11         Writing a 1 to REQ43_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ43_EN1

      10         Writing a 1 to REQ42_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ42_EN1

       9         Writing a 1 to REQ41_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ41_EN1

       8         Writing a 1 to REQ40_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ40_EN1

       7         Writing a 1 to REQ39_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ39_EN1

       6         Writing a 1 to REQ38_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ38_EN1

       5         Writing a 1 to REQ37_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ37_EN1

       4         Writing a 1 to REQ36_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ36_EN1

       3         Writing a 1 to REQ35_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ35_EN1

       2         Writing a 1 to REQ34_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ34_EN1

       1         Writing a 1 to REQ33_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ33_EN1

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1099 / 3791

<!-- page 1100 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       0         Writing a 1 to REQ32_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE1.
  REQ32_EN1


26.5.1.79 DMA1 Request Enable2 (DMA1_REQ_ENABLE2)

Offset


 Register                   Offset

 DMA1_REQ_ENABLE2   7A0h


Function
DMA request 64-95 enable for DMA1. One bit per request. 0: DMA request to DMA1 and response from DMA1 are blocked.
1:DMA request and response are enabled for DMA1.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                         0      0       REQ9  REQ9  REQ9  REQ9  REQ9              REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8
           5_E...    4_E...    3_E...    2_E...    1_E...                       8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...   W

Reset     1      1      1      1       1      1      1      1       1      1      1      1       1      1      1      1

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ6  REQ6  REQ6  REQ6  REQ6  REQ6
   W   9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...

Reset     1      1      1      1       1      1      1      1       1      1      1      1       1      1      1      1


Fields


       Field       Function
      31        This register is used to enable and disable I3C0 receive request.
  REQ95_EN1        0b - Disable
                     1b - Enable

      30        This register is used to enable and disable EMVSIM1 transmit request.
  REQ94_EN1        0b - Disable
                     1b - Enable

      29        This register is used to enable and disable EMVSIM1 receive request.
  REQ93_EN1

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1100 / 3791

<!-- page 1101 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Disable
                     1b - Enable

      28        This register is used to enable and disable EMVSIM0 transmit request.
  REQ92_EN1        0b - Disable
                     1b - Enable

      27        This register is used to enable and disable EMVSIM0 receive request.
  REQ91_EN1        0b - Disable
                     1b - Enable

      26       Reserved
    —

      25       Reserved
    —

      24        This register is used to enable and disable LP_FLEXCOMM9 transmit request.
  REQ88_EN1        0b - Disable
                     1b - Enable

      23        This register is used to enable and disable LP_FLEXCOMM9 receive request.
  REQ87_EN1        0b - Disable
                     1b - Enable

      22        This register is used to enable and disable LP_FLEXCOMM8 transmit request.
  REQ86_EN1        0b - Disable
                     1b - Enable

      21        This register is used to enable and disable LP_FLEXCOMM8 receive request.
  REQ85_EN1        0b - Disable
                     1b - Enable

      20        This register is used to enable and disable LP_FLEXCOMM7 transmit request.
  REQ84_EN1        0b - Disable
                     1b - Enable

      19        This register is used to enable and disable LP_FLEXCOMM7 receive request.
  REQ83_EN1        0b - Disable
                     1b - Enable

      18        This register is used to enable and disable LP_FLEXCOMM6 transmit request.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1101 / 3791

<!-- page 1102 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ82_EN1        0b - Disable
                     1b - Enable

      17        This register is used to enable and disable LP_FLEXCOMM6 receive request.
  REQ81_EN1        0b - Disable
                     1b - Enable

      16        This register is used to enable and disable LP_FLEXCOMM5 transmit request.
  REQ80_EN1        0b - Disable
                     1b - Enable

      15        This register is used to enable and disable LP_FLEXCOMM5 receive request.
  REQ79_EN1        0b - Disable
                     1b - Enable

      14        This register is used to enable and disable LP_FLEXCOMM4 transmit request.
  REQ78_EN1        0b - Disable
                     1b - Enable

      13        This register is used to enable and disable LP_FLEXCOMM4 receive request.
  REQ77_EN1        0b - Disable
                     1b - Enable

      12        This register is used to enable and disable LP_FLEXCOMM3 transmit request.
  REQ76_EN1        0b - Disable
                     1b - Enable

      11        This register is used to enable and disable LP_FLEXCOMM3 receive request.
  REQ75_EN1        0b - Disable
                     1b - Enable

      10        This register is used to enable and disable LP_FLEXCOMM2 transmit request.
  REQ74_EN1        0b - Disable
                     1b - Enable

       9         This register is used to enable and disable LP_FLEXCOMM2 receive request.
  REQ73_EN1        0b - Disable
                     1b - Enable

       8         This register is used to enable and disable LP_FLEXCOMM1 transmit request.
  REQ72_EN1        0b - Disable
                     1b - Enable

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1102 / 3791

<!-- page 1103 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       7         This register is used to enable and disable LP_FLEXCOMM1 receive request.
  REQ71_EN1        0b - Disable
                     1b - Enable

       6         This register is used to enable and disable LP_FLEXCOMM0 transmit request.
  REQ70_EN1        0b - Disable
                     1b - Enable

       5         This register is used to enable and disable LP_FLEXCOMM0 receive request.
  REQ69_EN1        0b - Disable
                     1b - Enable

       4         This register is used to enable and disable FlexIO0 Shifter7 Status DMA request OR Timer7 Status DMA
                   request.
  REQ68_EN1
                     0b - Disable
                     1b - Enable

       3         This register is used to enable and disable FlexIO0 Shifter6 Status DMA request OR Timer6 Status DMA
                   request.
  REQ67_EN1
                     0b - Disable
                     1b - Enable

       2         This register is used to enable and disable FlexIO0 Shifter5 Status DMA request OR Timer5 Status DMA
                   request.
  REQ66_EN1
                     0b - Disable
                     1b - Enable

       1         This register is used to enable and disable FlexIO0 Shifter4 Status DMA request OR Timer4 Status DMA
                   request.
  REQ65_EN1
                     0b - Disable
                     1b - Enable

       0         This register is used to enable and disable FlexIO0 Shifter3 Status DMA request OR Timer3 Status DMA
                   request.
  REQ64_EN1
                     0b - Disable
                     1b - Enable





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1103 / 3791

<!-- page 1104 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

26.5.1.80 DMA1 Request Enable2 (DMA1_REQ_ENABLE2_SET)

Offset


 Register                   Offset

 DMA1_REQ_ENABLE2_  7A4h
 SET


Function
Writing a 1 to a bit in this register sets the corresponding bit in DMA1_REQ_ENABLE2.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

       REQ9  REQ9  REQ9  REQ9  REQ9  REQ9  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8
   W
           5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R

       REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ6  REQ6  REQ6  REQ6  REQ6  REQ6
   W
           9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Writing a 1 to REQ95_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ95_EN1

      30         Writing a 1 to REQ94_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ94_EN1

      29         Writing a 1 to REQ93_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ93_EN1

      28         Writing a 1 to REQ92_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ92_EN1

      27         Writing a 1 to REQ91_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ91_EN1

      26         Writing a 1 to REQ90_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1104 / 3791

<!-- page 1105 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ90_EN1

      25         Writing a 1 to REQ89_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ89_EN1

      24         Writing a 1 to REQ88_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ88_EN1

      23         Writing a 1 to REQ87_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ87_EN1

      22         Writing a 1 to REQ86_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ86_EN1

      21         Writing a 1 to REQ85_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ85_EN1

      20         Writing a 1 to REQ84_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ84_EN1

      19         Writing a 1 to REQ83_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ83_EN1

      18         Writing a 1 to REQ82_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ82_EN1

      17         Writing a 1 to REQ81_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ81_EN1

      16         Writing a 1 to REQ80_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ80_EN1

      15         Writing a 1 to REQ79_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ79_EN1

      14         Writing a 1 to REQ78_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ78_EN1

      13         Writing a 1 to REQ77_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ77_EN1

      12         Writing a 1 to REQ876_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ76_EN1

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1105 / 3791

<!-- page 1106 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      11         Writing a 1 to REQ75_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ75_EN1

      10         Writing a 1 to REQ74_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ74_EN1

       9         Writing a 1 to REQ73_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ73_EN1

       8         Writing a 1 to REQ72_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ72_EN1

       7         Writing a 1 to REQ71_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ71_EN1

       6         Writing a 1 to REQ70_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ70_EN1

       5         Writing a 1 to REQ69_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ69_EN1

       4         Writing a 1 to REQ68_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ68_EN1

       3         Writing a 1 to REQ67_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ67_EN1

       2         Writing a 1 to REQ66_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ66_EN1

       1         Writing a 1 to REQ65_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ65_EN1

       0         Writing a 1 to REQ64_EN1 in this register sets the corresponding bit in DMA1_REQ_ENABLE2.
  REQ64_EN1


26.5.1.81 DMA1 Request Enable2 (DMA1_REQ_ENABLE2_CLR)

Offset


 Register                   Offset

 DMA1_REQ_ENABLE2_  7A8h
 CLR


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1106 / 3791

<!-- page 1107 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Function
Writing a 1 to a bit in this register clears the corresponding bit in DMA1_REQ_ENABLE2.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

       REQ9  REQ9  REQ9  REQ9  REQ9  REQ9  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8
   W
           5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R

       REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ6  REQ6  REQ6  REQ6  REQ6  REQ6
   W
           9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Writing a 1 to REQ95_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ95_EN1

      30         Writing a 1 to REQ94_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ94_EN1

      29         Writing a 1 to REQ93_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ93_EN1

      28         Writing a 1 to REQ92_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ92_EN1

      27         Writing a 1 to REQ91_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ91_EN1

      26         Writing a 1 to REQ90_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ90_EN1

      25         Writing a 1 to REQ89_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ89_EN1

      24         Writing a 1 to REQ88_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ88_EN1

      23         Writing a 1 to REQ87_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1107 / 3791

<!-- page 1108 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ87_EN1

      22         Writing a 1 to REQ86_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ86_EN1

      21         Writing a 1 to REQ85_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ85_EN1

      20         Writing a 1 to REQ84_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ84_EN1

      19         Writing a 1 to REQ83_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ83_EN1

      18         Writing a 1 to REQ82_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ82_EN1

      17         Writing a 1 to REQ81_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ81_EN1

      16         Writing a 1 to REQ80_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ80_EN1

      15         Writing a 1 to REQ79_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ79_EN1

      14         Writing a 1 to REQ78_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ78_EN1

      13         Writing a 1 to REQ77_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ77_EN1

      12         Writing a 1 to REQ76_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ76_EN1

      11         Writing a 1 to REQ75_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ75_EN1

      10         Writing a 1 to REQ74_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ74_EN1

       9         Writing a 1 to REQ73_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ73_EN1

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1108 / 3791

<!-- page 1109 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       8         Writing a 1 to REQ72_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ72_EN1

       7         Writing a 1 to REQ71_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ71_EN1

       6         Writing a 1 to REQ70_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ70_EN1

       5         Writing a 1 to REQ69_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ69_EN1

       4         Writing a 1 to REQ68_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ68_EN1

       3         Writing a 1 to REQ67_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ67_EN1

       2         Writing a 1 to REQ66_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ66_EN1

       1         Writing a 1 to REQ65_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ65_EN1

       0         Writing a 1 to REQ64_EN1 in this register clears the corresponding bit in DMA1_REQ_ENABLE2.
  REQ64_EN1


26.5.1.82 DMA1 Request Enable2 (DMA1_REQ_ENABLE2_TOG)

Offset


 Register                   Offset

 DMA1_REQ_ENABLE2_  7ACh
 TOG


Function
Writing a 1 to a bit in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1109 / 3791

<!-- page 1110 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R

       REQ9  REQ9  REQ9  REQ9  REQ9  REQ9  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8  REQ8
   W
           5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R

       REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ7  REQ6  REQ6  REQ6  REQ6  REQ6  REQ6
   W
           9_E...    8_E...    7_E...    6_E...    5_E...    4_E...    3_E...    2_E...    1_E...    0_E...    9_E...    8_E...    7_E...    6_E...    5_E...    4_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31         Writing a 1 to REQ95_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ95_EN1

      30         Writing a 1 to REQ94_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ94_EN1

      29         Writing a 1 to REQ93_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ93_EN1

      28         Writing a 1 to REQ92_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ92_EN1

      27         Writing a 1 to REQ91_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ91_EN1

      26         Writing a 1 to REQ90_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ90_EN1

      25         Writing a 1 to REQ89_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ89_EN1

      24         Writing a 1 to REQ88_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ88_EN1

      23         Writing a 1 to REQ87_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ87_EN1

      22         Writing a 1 to REQ86_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1110 / 3791

<!-- page 1111 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ86_EN1

      21         Writing a 1 to REQ85_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ85_EN1

      20         Writing a 1 to REQ84_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ84_EN1

      19         Writing a 1 to REQ83_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ83_EN1

      18         Writing a 1 to REQ82_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ82_EN1

      17         Writing a 1 to REQ81_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ81_EN1

      16         Writing a 1 to REQ80_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ80_EN1

      15         Writing a 1 to REQ79_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ79_EN1

      14         Writing a 1 to REQ78_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ78_EN1

      13         Writing a 1 to REQ77_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ77_EN1

      12         Writing a 1 to REQ76_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ76_EN1

      11         Writing a 1 to REQ75_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ75_EN1

      10         Writing a 1 to REQ74_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ74_EN1

       9         Writing a 1 to REQ73_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ73_EN1

       8         Writing a 1 to REQ72_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ72_EN1

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1111 / 3791

<!-- page 1112 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       7         Writing a 1 to REQ71_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ71_EN1

       6         Writing a 1 to REQ70_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ70_EN1

       5         Writing a 1 to REQ69_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ69_EN1

       4         Writing a 1 to REQ68_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ68_EN1

       3         Writing a 1 to REQ67_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ67_EN1

       2         Writing a 1 to REQ66_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ66_EN1

       1         Writing a 1 to REQ65_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ65_EN1

       0         Writing a 1 to REQ64_EN1 in this register toggles the corresponding bit in DMA1_REQ_ENABLE2.
  REQ64_EN1


26.5.1.83 DMA1 Request Enable3 (DMA1_REQ_ENABLE3)

Offset


 Register                   Offset

 DMA1_REQ_ENABLE3   7B0h


Function
DMA request 96-121 enable for DMA1. One bit per request. 0: DMA request to DMA1 and response from DMA1 are blocked.
1:DMA request and response are enabled for DMA1.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1112 / 3791

<!-- page 1113 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R    0      0      0      0      0      0                                           REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1
                                                                        21_...    20_...    19_...    18_...    17_...    16_...    15_...    14_...    13_...    12_...   W

Reset     0      0      0      0       0      0      1      1       1      1      1      1       1      1      1      1

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ9  REQ9  REQ9  REQ9
   W   11_...    10_...    09_...    08_...    07_...    06_...    05_...    04_...    03_...    02_...    01_...    00_...    9_E...    8_E...    7_E...    6_E...

Reset     1      1      1      1       1      1      1      1       1      1      1      1       1      1      1      1


Fields


       Field       Function
      31       Reserved
    —

      30       Reserved
    —

      29       Reserved
    —

      28       Reserved
    —

      27       Reserved
    —

      26       Reserved
    —

      25        This register is used to enable and disable TSI0 out of range request.
 REQ121_EN1        0b - Disable
                     1b - Enable

      24        This register is used to enable and disable TSI0 end of scan request.
 REQ120_EN1        0b - Disable
                     1b - Enable

      23        This register is used to enable and disable GPIO5 pin event request 1.
 REQ119_EN1        0b - Disable
                     1b - Enable

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1113 / 3791

<!-- page 1114 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      22        This register is used to enable and disable GPIO5 pin event request 0.
 REQ118_EN1        0b - Disable
                     1b - Enable

      21        This register is used to enable and disable GPIO4 pin event request 1.
 REQ117_EN1        0b - Disable
                     1b - Enable

      20        This register is used to enable and disable GPIO4 pin event request 0.
 REQ116_EN1        0b - Disable
                     1b - Enable

      19        This register is used to enable and disable GPIO3 pin event request 1.
 REQ115_EN1        0b - Disable
                     1b - Enable

      18        This register is used to enable and disable GPIO3 pin event request 0.
 REQ114_EN1        0b - Disable
                     1b - Enable

      17        This register is used to enable and disable GPIO2 pin event request 1.
 REQ113_EN1        0b - Disable
                     1b - Enable

      16        This register is used to enable and disable GPIO2 pin event request 0.
 REQ112_EN1        0b - Disable
                     1b - Enable

      15        This register is used to enable and disable GPIO1 pin event request 1.
 REQ111_EN1        0b - Disable
                     1b - Enable

      14        This register is used to enable and disable GPIO1 pin event request 0.
 REQ110_EN1        0b - Disable
                     1b - Enable

      13        This register is used to enable and disable GPIO0 pin event request 1.
 REQ109_EN1        0b - Disable
                     1b - Enable

      12        This register is used to enable and disable GPIO0 pin event request 0.
 REQ108_EN1

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1114 / 3791

<!-- page 1115 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Disable
                     1b - Enable

      11        This register is used to enable and disable SINC0 ipd_req_sinc[4] or ipd_req_alt [4] request.
 REQ107_EN1        0b - Disable
                     1b - Enable

      10        This register is used to enable and disable SINC0 ipd_req_sinc[3] or ipd_req_alt [3] request.
 REQ106_EN1        0b - Disable
                     1b - Enable

       9         This register is used to enable and disable SINC0 ipd_req_sinc[2] or ipd_req_alt [2] request.
 REQ105_EN1        0b - Disable
                     1b - Enable

       8         This register is used to enable and disable SINC0 ipd_req_sinc[1] or ipd_req_alt [1] request.
 REQ104_EN1        0b - Disable
                     1b - Enable

       7         This register is used to enable and disable SINC0 ipd_req_sinc[0] or ipd_req_alt [0] request.
 REQ103_EN1        0b - Disable
                     1b - Enable

       6         This register is used to enable and disable SAI1 transmit request.
 REQ102_EN1        0b - Disable
                     1b - Enable

       5         This register is used to enable and disable SAI1 receive request.
 REQ101_EN1        0b - Disable
                     1b - Enable

       4         This register is used to enable and disable SAI0 transmit request.
 REQ100_EN1        0b - Disable
                     1b - Enable

       3         This register is used to enable and disable SAI0 receive request.
  REQ99_EN1        0b - Disable
                     1b - Enable

       2         This register is used to enable and disable I3C1 transmit request.
  REQ98_EN1        0b - Disable
                     1b - Enable

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1115 / 3791

<!-- page 1116 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       1         This register is used to enable and disable I3C1 receive request.
  REQ97_EN1        0b - Disable
                     1b - Enable

       0         This register is used to enable and disable I3C0 transmit request.
  REQ96_EN1        0b - Disable
                     1b - Enable


26.5.1.84 DMA1 Request Enable3 (DMA1_REQ_ENABLE3_SET)

Offset


 Register                   Offset

 DMA1_REQ_ENABLE3_  7B4h
 SET


Function
Writing a 1 to a bit in this register sets the corresponding bit in DMA1_REQ_ENABLE3

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R    0      0      0      0      0      0

                                           REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1
   W
                                                                        21_...    20_...    19_...    18_...    17_...    16_...    15_...    14_...    13_...    12_...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R

       REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ9  REQ9  REQ9  REQ9
   W
            11_...    10_...    09_...    08_...    07_...    06_...    05_...    04_...    03_...    02_...    01_...    00_...    9_E...    8_E...    7_E...    6_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31       Reserved
    —

      30       Reserved

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1116 / 3791

<!-- page 1117 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

    —

      29       Reserved
    —

      28       Reserved
    —

      27       Reserved
    —

      26       Reserved
    —

      25         Writing a 1 to REQ121_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ121_EN1

      24         Writing a 1 to REQ120_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ120_EN1

      23         Writing a 1 to REQ119_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ119_EN1

      22         Writing a 1 to REQ118_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ118_EN1

      21         Writing a 1 to REQ117_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ117_EN1

      20         Writing a 1 to REQ116_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ116_EN1

      19         Writing a 1 to REQ115_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ115_EN1

      18         Writing a 1 to REQ114_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ114_EN1

      17         Writing a 1 to REQ113_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ113_EN1

      16         Writing a 1 to REQ112_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ112_EN1

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1117 / 3791

<!-- page 1118 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      15         Writing a 1 to REQ111_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ111_EN1

      14         Writing a 1 to REQ110_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ110_EN1

      13         Writing a 1 to REQ109_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ109_EN1

      12         Writing a 1 to REQ108_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ108_EN1

      11         Writing a 1 to REQ107_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ107_EN1

      10         Writing a 1 to REQ106_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ106_EN1

       9         Writing a 1 to REQ105_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ105_EN1

       8         Writing a 1 to REQ104_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ104_EN1

       7         Writing a 1 to REQ103_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ103_EN1

       6         Writing a 1 to REQ102_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ102_EN1

       5         Writing a 1 to REQ101_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ101_EN1

       4         Writing a 1 to REQ100_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
 REQ100_EN1

       3         Writing a 1 to REQ99_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
  REQ99_EN1

       2         Writing a 1 to REQ98_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
  REQ98_EN1

       1         Writing a 1 to REQ97_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1118 / 3791

<!-- page 1119 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ97_EN1

       0         Writing a 1 to REQ96_EN1 in this register sets the corresponding bit in DMA0_REQ_ENABLE3
  REQ96_EN1


26.5.1.85 DMA1 Request Enable3 (DMA1_REQ_ENABLE3_CLR)

Offset


 Register                   Offset

 DMA1_REQ_ENABLE3_  7B8h
 CLR


Function
Writing a 1 to a bit in this register clears the corresponding bit in DMA1_REQ_ENABLE3

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R    0      0      0      0      0      0

                                           REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1
   W
                                                                        21_...    20_...    19_...    18_...    17_...    16_...    15_...    14_...    13_...    12_...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R

       REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ1  REQ9  REQ9  REQ9  REQ9
   W
            11_...    10_...    09_...    08_...    07_...    06_...    05_...    04_...    03_...    02_...    01_...    00_...    9_E...    8_E...    7_E...    6_E...

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31       Reserved
    —

      30       Reserved
    —

      29       Reserved

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1119 / 3791

<!-- page 1120 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

    —

      28       Reserved
    —

      27       Reserved
    —

      26       Reserved
    —

      25         Writing a 1 to REQ121_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3.
 REQ121_EN1

      24         Writing a 1 to REQ120_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ120_EN1

      23         Writing a 1 to REQ119_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ119_EN1

      22         Writing a 1 to REQ118_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ118_EN1

      21         Writing a 1 to REQ117_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ117_EN1

      20         Writing a 1 to REQ116_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ116_EN1

      19         Writing a 1 to REQ115_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ115_EN1

      18         Writing a 1 to REQ114_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ114_EN1

      17         Writing a 1 to REQ113_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ113_EN1

      16         Writing a 1 to REQ112_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ112_EN1

      15         Writing a 1 to REQ111_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ111_EN1

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1120 / 3791

<!-- page 1121 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      14         Writing a 1 to REQ110_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ110_EN1

      13         Writing a 1 to REQ109_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ109_EN1

      12         Writing a 1 to REQ108_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ108_EN1

      11         Writing a 1 to REQ107_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ107_EN1

      10         Writing a 1 to REQ106_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ106_EN1

       9         Writing a 1 to REQ105_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ105_EN1

       8         Writing a 1 to REQ104_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ104_EN1

       7         Writing a 1 to REQ103_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ103_EN1

       6         Writing a 1 to REQ102_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ102_EN1

       5         Writing a 1 to REQ101_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ101_EN1

       4         Writing a 1 to REQ100_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
 REQ100_EN1

       3         Writing a 1 to REQ99_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
  REQ99_EN1

       2         Writing a 1 to REQ98_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
  REQ98_EN1

       1         Writing a 1 to REQ97_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3
  REQ97_EN1

       0         Writing a 1 to REQ96_EN1 in this register clears the corresponding bit in DMA0_REQ_ENABLE3

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1121 / 3791

<!-- page 1122 -->

NXP Semiconductors
                                                                                                              Input Multiplexing (INPUTMUX)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

  REQ96_EN1





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1122 / 3791
