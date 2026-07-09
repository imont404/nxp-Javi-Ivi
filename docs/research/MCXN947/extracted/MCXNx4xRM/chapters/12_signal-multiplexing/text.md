# 12 Signal Multiplexing

Source: `docs/mcxn947/MCXNx4xRM.pdf`
Pages: 285-315

<!-- page 285 -->

NXP Semiconductors

Chapter 12
Signal Multiplexing

12.1  Introduction


Pins have several functions available via signal multiplexing to optimize functionality in small packages. This chapter illustrates
which device's signals are multiplexed on which external pin.
See the pinout table (also in the attached spreadsheet) to know how the pins available on this device are configured. The attached
spreadsheet also provides the available device packages and pinout diagrams.

12.1.1  Signal multiplexing constraints
   • A given module signal must be assigned to a maximum of one package pin. Do not program the same function to more
    than one pin.
   • To ensure the best signal timing for a given peripheral's interface, choose the pins in closest proximity to each other.

12.2  Pinout
See the following table for pinout details, also in the attached MCXNx4x_Pinout.xlsx spreadsheet.

12.2.1 Pinmux view

Table 119. pinmux

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

 P1_8            A1               1                1               ALT0 - P1_8                IO Supply - VDD            ISP - UART_RXD

                                                                  ALT1 - TRACE_DATA0      Pad type - MED+I2C+I3C    ANALOG -
                                                                                                                   TSI0_CH17/ADC1_A8                                                                  ALT2 - FC4_P0               Default - DIS
                                                                                               VDD SYS -                                                                  ALT3 - FC5_P4
                                                                                                               WUU0_IN10/LPTMR1_ALT3
                                                                  ALT4 - CT_INP8

                                                                  ALT5 - SCT0_OUT2

                                                                  ALT6 - FLEXIO0_D16

                                                                  ALT7 - SmartDMA_PIO4

                                                                  ALT8 - PLU_OUT0

                                                                  ALT9 - ENET0_TXD2

                                                                  ALT10 - I3C1_SDA

 P1_9            B1               2                2               ALT0 - P1_9                IO Supply - VDD            ISP - UART_TXD

                                                                  ALT1 - TRACE_DATA1      Pad type - MED+I2C       ANALOG -
                                                                                                                   TSI0_CH18/ADC1_A9                                                                  ALT2 - FC4_P1               Default - DIS

                                                                  ALT3 - FC5_P5

                                                                  ALT4 - CT_INP9

                                                                  ALT5 - SCT0_OUT3

                                                                  ALT6 - FLEXIO0_D17

                                                                  ALT7 - SmartDMA_PIO5

                                                       Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    285 / 3791

<!-- page 286 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

                                                                  ALT8 - PLU_OUT1

                                                                  ALT9 - ENET0_TXD3

                                                                  ALT10 - I3C1_SCL

 P1_10           C3               3                3               ALT0 - P1_10               IO Supply - VDD            ISP - CAN_TXD

                                                                  ALT1 - TRACE_DATA2      Pad type - MED          ANALOG -
                                                                                                                    TSI0_CH19/ADC1_A10                                                                  ALT2 - FC4_P2               Default - DIS

                                                                  ALT3 - FC5_P6

                                                                  ALT4 - CT2_MAT0

                                                                  ALT5 - SCT0_IN2

                                                                  ALT6 - FLEXIO0_D18

                                                                  ALT7 - SmartDMA_PIO6

                                                                  ALT8 - PLU_IN0

                                                                  ALT9 - ENET0_TXER

                                                                  ALT11 - CAN0_TXD

 P1_11           D3               4                4               ALT0 - P1_11               IO Supply - VDD            ISP - CAN_RXD

                                                                  ALT1 - TRACE_DATA3      Pad type - MED          ANALOG -
                                                                                                                    TSI0_CH20/ADC1_A11                                                                  ALT2 - FC4_P3               Default - DIS
                                                                                               VDD SYS - WUU0_IN11                                                                  ALT4 - CT2_MAT1

                                                                  ALT5 - SCT0_IN3

                                                                  ALT6 - FLEXIO0_D19

                                                                  ALT7 - SmartDMA_PIO7

                                                                  ALT8 - PLU_IN1

                                                                  ALT9 - ENET0_RX_CLK

                                                                  ALT10 - I3C1_PUR

                                                                  ALT11 - CAN0_RXD

 P1_12           D2               5                5               ALT0 - P1_12               IO Supply - VDD         ANALOG -
                                                                                                                    TSI0_CH21/ADC1_A12                                                                  ALT1 - TRACE_CLK        Pad type - MED
                                                                                               VDD SYS - WUU0_IN12                                                                  ALT2 - FC4_P4               Default - DIS

                                                                  ALT3 - FC3_P0

                                                                  ALT4 - CT2_MAT2

                                                                  ALT5 - SCT0_OUT4

                                                                  ALT6 - FLEXIO0_D20

                                                                  ALT7 - SmartDMA_PIO8

                                                                  ALT8 - PLU_OUT2

                                                                  ALT9 - ENET0_RXER

                                                                  ALT11 - CAN1_RXD

                                                       Tablecontinuesonthenextpage...




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    286 / 3791

<!-- page 287 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

 P1_13           D1               6                6               ALT0 - P1_13               IO Supply - VDD         ANALOG -
                                                                                                                    TSI0_CH22/ADC1_A13                                                                  ALT1 - TRIG_IN3          Pad type - MED

                                                                  ALT2 - FC4_P5               Default - DIS

                                                                  ALT3 - FC3_P1

                                                                  ALT4 - CT2_MAT3

                                                                  ALT5 - SCT0_OUT5

                                                                  ALT6 - FLEXIO0_D21

                                                                  ALT7 - SmartDMA_PIO9

                                                                  ALT8 - PLU_OUT3

                                                                  ALT9 - ENET0_RXDV

                                                                  ALT11 - CAN1_TXD

 P1_14           D4               7                7               ALT0 - P1_14               IO Supply - VDD         ANALOG -
                                                                                                                    TSI0_CH23/ADC1_A14                                                                  ALT2 - FC4_P6            Pad type - MED

                                                                  ALT3 - FC3_P2               Default - DIS

                                                                  ALT4 - CT_INP10

                                                                  ALT5 - SCT0_IN4

                                                                  ALT6 - FLEXIO0_D22

                                                                  ALT7 - SmartDMA_PIO10

                                                                  ALT8 - PLU_IN2

                                                                  ALT9 - ENET0_RXD0

 P1_15           E4               8                8               ALT0 - P1_15               IO Supply - VDD         ANALOG -
                                                                                                                    TSI0_CH24/ADC1_A15                                                                  ALT3 - FC3_P3            Pad type - MED
                                                                                               VDD SYS - WUU0_IN13                                                                  ALT4 - CT_INP11             Default - DIS

                                                                  ALT5 - SCT0_IN5

                                                                  ALT6 - FLEXIO0_D23

                                                                  ALT7 - SmartDMA_PIO11

                                                                  ALT8 - PLU_IN3

                                                                  ALT9 - ENET0_RXD1

                                                                  ALT10 - I3C1_PUR

 VSS             P14                        --                           --                                           IO Supply - VDD

                                                                                         Pad type - VSSIO

 P1_16            F6                         --                           --               ALT0 - P1_16               IO Supply - VDD         ANALOG - ADC1_A16

                                                                  ALT2 - FC5_P0            Pad type - MED+I2C+I3C   VDD SYS - WUU0_IN14

                                                                  ALT3 - FC3_P4               Default - DIS

                                                                  ALT4 - CT_INP12

                                                                  ALT5 - SCT0_OUT6

                                                                  ALT6 - FLEXIO0_D24

                                                       Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    287 / 3791

<!-- page 288 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

                                                                  ALT7 - SmartDMA_PIO12

                                                                  ALT8 - PLU_OUT4

                                                                  ALT9 - ENET0_RXD2

                                                                  ALT10 - I3C1_SDA

 P1_17            F4                         --                           --               ALT0 - P1_17               IO Supply - VDD         ANALOG - ADC1_A17

                                                                  ALT2 - FC5_P1            Pad type - MED+I2C

                                                                  ALT3 - FC3_P5               Default - DIS

                                                                  ALT4 - CT_INP13

                                                                  ALT5 - SCT0_OUT7

                                                                  ALT6 - FLEXIO0_D25

                                                                  ALT7 - SmartDMA_PIO13

                                                                  ALT8 - PLU_OUT5

                                                                  ALT9 - ENET0_RXD3

                                                                  ALT10 - I3C1_SCL

 P1_18          G4                         --                           --               ALT0 - P1_18               IO Supply - VDD         ANALOG - ADC1_A18

                                                                  ALT1 - FREQME_CLK_IN0   Pad type - MED

                                                                  ALT2 - FC5_P2               Default - DIS

                                                                  ALT3 - FC3_P6

                                                                  ALT4 - CT3_MAT0

                                                                  ALT5 - SCT0_IN6

                                                                  ALT6 - FLEXIO0_D26

                                                                  ALT7 - SmartDMA_PIO14

                                                                  ALT8 - PLU_IN4

                                                                  ALT9 - ENET0_COL

                                                                  ALT11 - CAN0_TXD

 P1_19          G5                         --                           --               ALT0 - P1_19               IO Supply - VDD         ANALOG - ADC1_A19

                                                                  ALT1 - FREQME_CLK_IN1   Pad type - MED          VDD SYS - WUU0_IN15

                                                                  ALT2 - FC5_P3               Default - DIS

                                                                  ALT4 - CT3_MAT1

                                                                  ALT5 - SCT0_IN7

                                                                  ALT6 - FLEXIO0_D27

                                                                  ALT7 - SmartDMA_PIO15

                                                                  ALT8 - PLU_IN5

                                                                  ALT9 - ENET0_CRS

                                                                  ALT11 - CAN0_RXD

 P1_20           K5                         --                           --               ALT0 - P1_20               IO Supply - VDD         ANALOG -
                                                                                                                 ADC1_A20/CMP1_IN3                                                                  ALT1 - TRIG_IN2          Pad type - MED

                                                       Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    288 / 3791

<!-- page 289 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

                                                                  ALT2 - FC5_P4               Default - DIS

                                                                  ALT3 - FC4_P0

                                                                  ALT4 - CT3_MAT2

                                                                  ALT5 - SCT0_OUT8

                                                                  ALT6 - FLEXIO0_D28

                                                                  ALT7 - SmartDMA_PIO16

                                                                  ALT8 - PLU_OUT6

                                                                  ALT9 - ENET0_MDC

                                                                  ALT11 - CAN1_TXD

 P1_21            L5                          --                           --               ALT0 - P1_21               IO Supply - VDD         ANALOG -
                                                                                                                 ADC1_A21/CMP2_IN3                                                                  ALT1 - TRIG_OUT2        Pad type - MED

                                                                  ALT2 - FC5_P5               Default - DIS

                                                                  ALT3 - FC4_P1

                                                                  ALT4 - CT3_MAT3

                                                                  ALT5 - SCT0_OUT9

                                                                  ALT6 - FLEXIO0_D29

                                                                  ALT7 - SmartDMA_PIO17

                                                                  ALT8 - PLU_OUT7

                                                                  ALT9 - ENET0_MDIO

                                                                  ALT10 - SAI1_MCLK

                                                                  ALT11 - CAN1_RXD

 P1_22            L4                          --                           --               ALT0 - P1_22               IO Supply - VDD         ANALOG - ADC1_A22

                                                                  ALT1 - TRIG_IN3          Pad type - MED

                                                                  ALT2 - FC5_P6               Default - DIS

                                                                  ALT3 - FC4_P2

                                                                  ALT4 - CT_INP14

                                                                  ALT5 - SCT0_OUT4

                                                                  ALT6 - FLEXIO0_D30

                                                                  ALT7 - SmartDMA_PIO18

 P1_23          M4                         --                           --               ALT0 - P1_23               IO Supply - VDD         ANALOG - ADC1_A23

                                                                  ALT3 - FC4_P3            Pad type - MED

                                                                  ALT4 - CT_INP15             Default - DIS

                                                                  ALT5 - SCT0_OUT5

                                                                  ALT6 - FLEXIO0_D31

                                                                  ALT7 - SmartDMA_PIO19

 RESET_B         F3               9                9                                           IO Supply - VDD

                                                                                         Pad type - RST

                                                       Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    289 / 3791

<!-- page 290 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

                                                                                                                     Default - RESET_B

 P1_30            F1               10               10              ALT0 - P1_30               IO Supply - VDD         ANALOG - XTAL48M

                                                                  ALT1 - TRIG_OUT3        Pad type - MED

                                                                  ALT4 - CT_INP16             Default - DIS

                                                                  ALT5 - SCT0_OUT8

                                                                  ALT10 - SAI0_MCLK

 P1_31            F2               11               11              ALT0 - P1_31               IO Supply - VDD         ANALOG - EXTAL48M

                                                                  ALT1 - TRIG_IN4          Pad type - MED

                                                                  ALT4 - CT_INP17             Default - DIS

                                                                  ALT5 - SCT0_OUT9

 VSS            D6                         --                           --                                           IO Supply - VDD

                                                                                         Pad type - VSSIO

 VDD_CORE       K10              12               12                                          IO Supply - VDD

                                                                                         Pad type - VDDINT

 VDD_LDO_CORE   K6               13               13                                          IO Supply - VDD         ANALOG
                                                                                                                                                                                                        - VDD_LDO_CORE                                                                                         Pad type - VDDINT_3V

 VDD            H8               13               13                                          IO Supply - VDD

                                                                                         Pad type - VDDIO

 VDD_P2         K8               13               13                                          IO Supply - VDD_P2

                                                                                         Pad type - VDDIO

 VSS            E5                         --                           --                                           IO Supply - VDD_P2

                                                                                         Pad type - VSSIO

 P2_0           H2               14               14              ALT0 - P2_0                IO Supply - VDD_P2

                                                                  ALT1 - TRIG_IN5          Pad type - FAST

                                                                  ALT2 - FC9_P6               Default - DIS

                                                                  ALT3 - uSDHC0_D5

                                                                  ALT4 - SCT0_IN0

                                                                  ALT5 - PWM1_A3

                                                                  ALT6 - FLEXIO0_D8

                                                                  ALT7 - SmartDMA_PIO20

                                                                  ALT8 - FLEXSPI0_B_SS1_b

                                                                  ALT10 - SAI0_RX_BCLK

 P2_1           H1               15               15              ALT0 - P2_1                IO Supply - VDD_P2

                                                                  ALT1 - TRACE_CLK        Pad type - FAST

                                                                  ALT3 - uSDHC0_D4           Default - DIS

                                                       Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    290 / 3791

<!-- page 291 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

                                                                  ALT4 - SCT0_IN1

                                                                  ALT5 - PWM1_B3

                                                                  ALT6 - FLEXIO0_D9

                                                                  ALT7 - SmartDMA_PIO21

                                                                  ALT8 - FLEXSPI0_B_DQS

                                                                  ALT9 - SINC0_MCLK_OUT0

                                                                  ALT10 - SAI0_RX_FS

 P2_2           H3               16               16              ALT0 - P2_2                IO Supply - VDD_P2       VDD SYS - WUU0_IN16

                                                                  ALT1 - CLKOUT           Pad type - FAST

                                                                  ALT2 - FC9_P3               Default - DIS

                                                                  ALT3 - uSDHC0_D1

                                                                  ALT4 - SCT0_OUT0

                                                                  ALT5 - PWM1_A2

                                                                  ALT6 - FLEXIO0_D10

                                                                  ALT7 - SmartDMA_PIO22

                                                                  ALT8 - FLEXSPI0_B_SS0_b

                                                                  ALT9 - SINC0_MCLK0

                                                                  ALT10 - SAI0_TXD0

 P2_3              J3               17               17              ALT0 - P2_3                IO Supply - VDD_P2

                                                                  ALT2 - FC9_P1            Pad type - FAST

                                                                  ALT3 - uSDHC0_D0           Default - DIS

                                                                  ALT4 - SCT0_OUT1

                                                                  ALT5 - PWM1_B2

                                                                  ALT6 - FLEXIO0_D11

                                                                  ALT7 - SmartDMA_PIO23

                                                                  ALT8 - FLEXSPI0_B_SCLK

                                                                  ALT9 - SINC0_MBIT0

                                                                  ALT10 - SAI0_RXD0

 P2_4            K3               18               18              ALT0 - P2_4                IO Supply - VDD_P2       VDD SYS - WUU0_IN17

                                                                  ALT2 - FC9_P0            Pad type - FAST

                                                                  ALT3 - uSDHC0_CLK         Default - DIS

                                                                  ALT4 - SCT0_OUT2

                                                                  ALT5 - PWM1_A1

                                                                  ALT6 - FLEXIO0_D12

                                                                  ALT7 - SmartDMA_PIO24

                                                                  ALT8 - FLEXSPI0_B_DATA0

                                                                  ALT9 - SINC0_MCLK1

                                                       Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    291 / 3791

<!-- page 292 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

                                                                  ALT10 - SAI0_RXD1

 P2_5            K1               19               19              ALT0 - P2_5                IO Supply - VDD_P2

                                                                  ALT1 - TRIG_OUT3        Pad type - FAST

                                                                  ALT2 - FC9_P2               Default - DIS

                                                                  ALT3 - uSDHC0_CMD

                                                                  ALT4 - SCT0_OUT3

                                                                  ALT5 - PWM1_B1

                                                                  ALT6 - FLEXIO0_D13

                                                                  ALT7 - SmartDMA_PIO25

                                                                  ALT8 - FLEXSPI0_B_DATA1

                                                                  ALT9 - SINC0_MBIT1

                                                                  ALT10 - SAI0_TXD1

 VSS           G2                         --                           --                                           IO Supply - VDD_P2

                                                                                         Pad type - VSSIO

 VDD_P2          L7                          --                           --                                           IO Supply - VDD_P2

                                                                                         Pad type - VDDIO

 P2_6            K2               20               20              ALT0 - P2_6                IO Supply - VDD_P2

                                                                  ALT1 - TRIG_IN4          Pad type - FAST

                                                                  ALT2 - FC9_P4               Default - DIS

                                                                  ALT3 - uSDHC0_D3

                                                                  ALT4 - SCT0_OUT4

                                                                  ALT5 - PWM1_A0

                                                                  ALT6 - FLEXIO0_D14

                                                                  ALT7 - SmartDMA_PIO26

                                                                  ALT8 - FLEXSPI0_B_DATA2

                                                                  ALT9 - SINC0_MCLK2

                                                                  ALT10 - SAI0_TX_BCLK

 P2_7             L2               21               21              ALT0 - P2_7                IO Supply - VDD_P2

                                                                  ALT1 - TRIG_IN5          Pad type - FAST

                                                                  ALT2 - FC9_P5               Default - DIS

                                                                  ALT3 - uSDHC0_D2

                                                                  ALT4 - SCT0_OUT5

                                                                  ALT5 - PWM1_B0

                                                                  ALT6 - FLEXIO0_D15

                                                                  ALT7 - SmartDMA_PIO27

                                                                  ALT8 - FLEXSPI0_B_DATA3

                                                       Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    292 / 3791

<!-- page 293 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

                                                                  ALT9 - SINC0_MBIT2

                                                                  ALT10 - SAI0_TX_FS

 P2_8           M2                         --                           --               ALT0 - P2_8                IO Supply - VDD_P2

                                                                  ALT1 - TRACE_DATA0      Pad type - FAST

                                                                  ALT3 - uSDHC0_D7           Default - DIS

                                                                  ALT4 - SCT0_IN2

                                                                  ALT5 - PWM1_X0

                                                                  ALT6 - FLEXIO0_D16

                                                                  ALT7 - SmartDMA_PIO28

                                                                  ALT8 - FLEXSPI0_B_DATA4

                                                                  ALT9 - SINC0_MCLK3

                                                                  ALT10 - SAI1_TXD0

 P2_9           M1                         --                           --               ALT0 - P2_9                IO Supply - VDD_P2

                                                                  ALT1 - TRACE_DATA1      Pad type - FAST

                                                                  ALT3 - uSDHC0_D6           Default - DIS

                                                                  ALT4 - SCT0_IN3

                                                                  ALT5 - PWM1_X1

                                                                  ALT6 - FLEXIO0_D17

                                                                  ALT7 - SmartDMA_PIO29

                                                                  ALT8 - FLEXSPI0_B_DATA5

                                                                  ALT9 - SINC0_MBIT3

                                                                  ALT10 - SAI1_RXD0

 P2_10          M3                         --                           --               ALT0 - P2_10               IO Supply - VDD_P2

                                                                  ALT1 - TRACE_DATA2      Pad type - FAST

                                                                  ALT4 - SCT0_IN4             Default - DIS

                                                                  ALT5 - PWM1_X2

                                                                  ALT6 - FLEXIO0_D18

                                                                  ALT7 - SmartDMA_PIO31

                                                                  ALT8 - FLEXSPI0_B_DATA6

                                                                  ALT9 - SINC0_MCLK4

                                                                  ALT10 - SAI1_RXD1

 P2_11           N4                         --                           --               ALT0 - P2_11               IO Supply - VDD_P2

                                                                  ALT1 - TRACE_DATA3      Pad type - FAST

                                                                  ALT4 - SCT0_IN5             Default - DIS

                                                                  ALT5 - PWM1_X3

                                                                  ALT6 - FLEXIO0_D19

                                                                  ALT7 - SmartDMA_PIO30

                                                       Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    293 / 3791

<!-- page 294 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

                                                                  ALT8 - FLEXSPI0_B_DATA7

                                                                  ALT9 - SINC0_MBIT4

                                                                  ALT10 - SAI1_TXD1

 VSS            H5                         --                           --                                           IO Supply - VDD_P2

                                                                                         Pad type - VSSIO

 VDD_P2                 --                           --                           --                                           IO Supply - VDD_P2

                                                                                         Pad type - VDDIO

 VDD_P4         P4                         --                           --                                           IO Supply - VDD_P4

                                                                                         Pad type - VDDIO

 VSS_P4          P7                         --                           --                                           IO Supply - VDD_P4

                                                                                         Pad type - VSSIO

 P4_0            P1                         --                           --               ALT0 - P4_0                IO Supply - VDD_P4       VDD SYS - WUU0_IN18

                                                                  ALT1 - TRIG_IN6          Pad type - SLOW

                                                                  ALT2 - FC2_P0               Default - DIS

                                                                  ALT4 - CT_INP16

                                                                  ALT7 - SmartDMA_PIO24

                                                                  ALT8 - PLU_IN0

                                                                  ALT9 - SINC0_MCLK3

 ANA_0           P3                         --                           --                                           IO Supply - VDD_P4       ANALOG - ADC0_A0

                                                                                         Pad type - ANA

 P4_0/ANA_0            --                22               22              ALT0 - P4_0                IO Supply - VDD_P4       ANALOG - ADC0_A0

                                                                  ALT1 - TRIG_IN6          Pad type - SLOW         VDD SYS - WUU0_IN18

                                                                  ALT2 - FC2_P0               Default - DIS

                                                                  ALT4 - CT_INP16

                                                                  ALT7 - SmartDMA_PIO24

                                                                  ALT8 - PLU_IN0

                                                                  ALT9 - SINC0_MCLK3

 ANA_1          R3                         --                           --                                           IO Supply - VDD_P4       ANALOG - ADC0_B0

                                                                                         Pad type - ANA

 P4_1            P2                         --                           --               ALT0 - P4_1                IO Supply - VDD_P4

                                                                  ALT1 - TRIG_IN7          Pad type - SLOW

                                                                  ALT2 - FC2_P1               Default - DIS

                                                                  ALT4 - CT_INP17

                                                                  ALT7 - SmartDMA_PIO25

                                                                  ALT8 - PLU_IN1

                                                       Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    294 / 3791

<!-- page 295 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

 P4_1/ANA_1            --                23               23              ALT0 - P4_1                IO Supply - VDD_P4       ANALOG - ADC0_B0

                                                                  ALT1 - TRIG_IN7          Pad type - SLOW

                                                                  ALT2 - FC2_P1               Default - DIS

                                                                  ALT4 - CT_INP17

                                                                  ALT7 - SmartDMA_PIO25

                                                                  ALT8 - PLU_IN1

 P4_2             T1               24               24              ALT0 - P4_2                IO Supply - VDD_P4       ANALOG
                                                                                                                                                                                                        - DAC0_OUT/ADC0_A4/                                                                  ALT1 - TRIG_IN6          Pad type - SLOW
                                                                                                                  ADC1_A4/CMP0_IN4N/
                                                                  ALT2 - FC2_P2               Default - DIS
                                                                                                               CMP1_IN4N/CMP2_IN4N
                                                                  ALT4 - CT_INP12

                                                                  ALT7 - SmartDMA_PIO26

                                                                  ALT8 - PLU_IN2

                                                                  ALT9 - SINC0_MBIT3

 P4_3           U1               25               25              ALT0 - P4_3                IO Supply - VDD_P4       ANALOG
                                                                                                                                                                                                        - DAC1_OUT/ADC0_B4/                                                                  ALT1 - TRIG_IN7          Pad type - SLOW
                                                                                                                  ADC1_B4/CMP0_IN5N/
                                                                  ALT2 - FC2_P3               Default - DIS
                                                                                                               CMP1_IN5N/CMP2_IN5N
                                                                  ALT4 - CT_INP13
                                                                                               VDD SYS - WUU0_IN19
                                                                  ALT7 - SmartDMA_PIO27

                                                                  ALT8 - PLU_IN3

 P4_4           M6                         --                           --               ALT0 - P4_4                IO Supply - VDD_P4

                                                                  ALT2 - FC2_P4            Pad type - SLOW

                                                                  ALT4 - CT_INP14             Default - DIS

                                                                  ALT7 - SmartDMA_PIO28

                                                                  ALT8 - PLU_IN4

                                                                  ALT9 - SINC0_MCLK4

 ANA_4           T2                         --                           --                                           IO Supply - VDD_P4       ANALOG - ADC1_A0

                                                                                         Pad type - ANA

 P4_4/ANA_4            --                26               26              ALT0 - P4_4                IO Supply - VDD_P4       ANALOG - ADC1_A0

                                                                  ALT2 - FC2_P4            Pad type - SLOW

                                                                  ALT4 - CT_INP14             Default - DIS

                                                                  ALT7 - SmartDMA_PIO28

                                                                  ALT8 - PLU_IN4

                                                                  ALT9 - SINC0_MCLK4

 ANA_5           T3                         --                           --                                           IO Supply - VDD_P4       ANALOG - ADC1_B0

                                                                                         Pad type - ANA

 P4_5           M8                         --                           --               ALT0 - P4_5                IO Supply - VDD_P4

                                                       Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    295 / 3791

<!-- page 296 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

                                                                  ALT2 - FC2_P5            Pad type - SLOW

                                                                  ALT4 - CT_INP15             Default - DIS

                                                                  ALT7 - SmartDMA_PIO29

                                                                  ALT8 - PLU_IN5

                                                                  ALT9 - SINC0_MBIT4

 P4_5/ANA_5            --                27               27              ALT0 - P4_5                IO Supply - VDD_P4       ANALOG - ADC1_B0

                                                                  ALT2 - FC2_P5            Pad type - SLOW

                                                                  ALT4 - CT_INP15             Default - DIS

                                                                  ALT7 - SmartDMA_PIO29

                                                                  ALT8 - PLU_IN5

                                                                  ALT9 - SINC0_MBIT4

 ANA_6          U2                         --                           --                                           IO Supply - VDD_P4       ANALOG - DAC2_OUT/
                                                                                                              ADC0_A3/ADC1_A3                                                                                         Pad type - ANA

 P4_6           N7                         --                           --               ALT0 - P4_6                IO Supply - VDD_P4

                                                                  ALT1 - TRIG_OUT4        Pad type - SLOW

                                                                  ALT2 - FC2_P6               Default - DIS

                                                                  ALT4 - CT_INP18

                                                                  ALT7 - SmartDMA_PIO30

                                                                  ALT8 - PLU_CLK

 P4_6/ANA_6            --                28               28              ALT0 - P4_6                IO Supply - VDD_P4       ANALOG - DAC2_OUT/
                                                                                                              ADC0_A3/ADC1_A3                                                                  ALT1 - TRIG_OUT4        Pad type - SLOW

                                                                  ALT2 - FC2_P6               Default - DIS

                                                                  ALT4 - CT_INP18

                                                                  ALT7 - SmartDMA_PIO30

                                                                  ALT8 - PLU_CLK

 P4_7             T4                         --                           --               ALT0 - P4_7                IO Supply - VDD_P4

                                                                  ALT4 - CT_INP19          Pad type - SLOW

                                                                  ALT7 - SmartDMA_PIO31     Default - DIS

 ANA_7          U4                         --                           --                                           IO Supply - VDD_P4       ANALOG - VREFI/VREFO/
                                                                                                              ADC0_A7/ADC1_A7                                                                                         Pad type - ANA

 P4_7/ANA_7            --                29               29              ALT0 - P4_7                IO Supply - VDD_P4       ANALOG - VREFI/VREFO/
                                                                                                              ADC0_A7/ADC1_A7                                                                  ALT4 - CT_INP19          Pad type - SLOW

                                                                  ALT7 - SmartDMA_PIO31     Default - DIS

 VDD_ANA        R4               30               30                                          IO Supply - VDD_P4

                                                                                         Pad type - VDDINT_3V

                                                       Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    296 / 3791

<!-- page 297 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

 VREFH          R5               31               31                                          IO Supply - VDD_P4       ANALOG - VREFH

                                                                                         Pad type - ANA

 VREFL          R6               32               32                                          IO Supply - VDD_P4       ANALOG - VREFL

                                                                                         Pad type - VSSINT

 VSS_P4          P6               33               33                                          IO Supply - VDD_P4

                                                                                         Pad type - VSSIO

 VDD_P4         N5               34               34                                          IO Supply - VDD_P4

                                                                                         Pad type - VDDIO

 P4_12            T6               35               35              ALT0 - P4_12               IO Supply - VDD_P4         ISP - USB0_VBUS_DET

                                                                  ALT1 - USB0_VBUS_DET    Pad type - SLOW         ANALOG - OPAMP0_INP0/
                                                                                                              ADC0_A5/ADC1_A5                                                                  ALT2 - FC2_P0               Default - DIS
                                                                                               VDD SYS - WUU0_IN20                                                                  ALT4 - CT4_MAT0

                                                                  ALT6 - FLEXIO0_D20

                                                                  ALT8 - PLU_OUT0

                                                                  ALT9 - SINC0_MCLK0

                                                                  ALT11 - CAN0_RXD

 P4_13            T7                         --                           --               ALT0 - P4_13               IO Supply - VDD_P4       ANALOG - OPAMP0_INP1/
                                                                                                              ADC0_B5/ADC1_B5                                                                  ALT1 - TRIG_IN8          Pad type - SLOW

                                                                  ALT2 - FC2_P1               Default - DIS

                                                                  ALT3 - USB1_OTGn_ID

                                                                  ALT4 - CT4_MAT1

                                                                  ALT6 - FLEXIO0_D21

                                                                  ALT8 - PLU_OUT1

                                                                  ALT9 - SINC0_MBIT0

                                                                  ALT11 - CAN0_TXD

 ANA_14         U6                         --                           --                                           IO Supply - VDD_P4       ANALOG - OPAMP0_INN

                                                                                         Pad type - LOLK

 P4_13/ANA_14         --                36               36              ALT0 - P4_13               IO Supply - VDD_P4       ANALOG
                                                                                                                                                                                                        - OPAMP0_INP1/ADC0_B5/                                                                  ALT1 - TRIG_IN8          Pad type - SLOW
                                                                                                             ADC1_B5/OPAMP0_INN
                                                                  ALT2 - FC2_P1               Default - DIS

                                                                  ALT3 - USB1_OTGn_ID

                                                                  ALT4 - CT4_MAT1

                                                                  ALT6 - FLEXIO0_D21

                                                                  ALT8 - PLU_OUT1

                                                                  ALT9 - SINC0_MBIT0

                                                                  ALT11 - CAN0_TXD

                                                       Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    297 / 3791

<!-- page 298 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

 P4_14           N8                         --                           --               ALT0 - P4_14               IO Supply - VDD_P4

                                                                  ALT4 - CT4_MAT2         Pad type - SLOW

                                                                  ALT6 - FLEXIO0_D22         Default - DIS

                                                                  ALT8 - PLU_OUT2

 P4_15            T8               37               37              ALT0 - P4_15               IO Supply - VDD_P4       ANALOG - OPAMP0_OUT/
                                                                                                                ADC0_A1/CMP0_IN4P                                                                  ALT1 - TRIG_OUT4        Pad type - SLOW
                                                                                               VDD SYS - WUU0_IN21                                                                  ALT3                         Default - DIS
                                                                                                                      - USB1_VBUSVALID_EXT

                                                                  ALT4 - CT4_MAT3

                                                                  ALT6 - FLEXIO0_D23

                                                                  ALT8 - PLU_OUT3

                                                                  ALT9 - SINC0_MCLK_OUT0

                                                                  ALT11 - CAN1_RXD

 P4_16           R8               38               38              ALT0 - P4_16               IO Supply - VDD_P4       ANALOG -
                                                                                                              OPAMP1_INP0/ADC0_A6                                                                  ALT2 - FC2_P2            Pad type - SLOW

                                                                  ALT3 - USB1_OTGn_PWR    Default - DIS

                                                                  ALT4 - CT3_MAT0

                                                                  ALT6 - FLEXIO0_D24

                                                                  ALT8 - PLU_OUT4

                                                                  ALT9 - SINC0_MCLK1

                                                                  ALT11 - CAN1_TXD

 P4_17           R9                         --                           --               ALT0 - P4_17               IO Supply - VDD_P4       ANALOG -
                                                                                                              OPAMP1_INP1/ADC0_B6                                                                  ALT1 - TRIG_IN9          Pad type - SLOW

                                                                  ALT2 - FC2_P3               Default - DIS

                                                                  ALT3 - USB1_OTGn_OC

                                                                  ALT4 - CT3_MAT1

                                                                  ALT6 - FLEXIO0_D25

                                                                  ALT8 - PLU_OUT5

                                                                  ALT9 - SINC0_MBIT1

 ANA_18         U8                         --                           --                                           IO Supply - VDD_P4       ANALOG - OPAMP1_INN

                                                                                         Pad type - LOLK

 P4_17/ANA_18         --                39               39              ALT0 - P4_17               IO Supply - VDD_P4       ANALOG - OPAMP1_INP1/
                                                                                                             ADC0_B6/OPAMP1_INN                                                                  ALT1 - TRIG_IN9          Pad type - SLOW

                                                                  ALT2 - FC2_P3               Default - DIS

                                                                  ALT3 - USB1_OTGn_OC

                                                                  ALT4 - CT3_MAT1

                                                                  ALT6 - FLEXIO0_D25

                                                       Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    298 / 3791

<!-- page 299 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

                                                                  ALT8 - PLU_OUT5

                                                                  ALT9 - SINC0_MBIT1

 P4_18           N10                       --                           --               ALT0 - P4_18               IO Supply - VDD_P4

                                                                  ALT4 - CT3_MAT2         Pad type - SLOW

                                                                  ALT6 - FLEXIO0_D26         Default - DIS

                                                                  ALT8 - PLU_OUT6

 P4_19           R10              40                          --               ALT0 - P4_19               IO Supply - VDD_P4       ANALOG - OPAMP1_OUT/
                                                                                                                ADC0_B1/CMP1_IN4P                                                                  ALT1 - TRIG_OUT5        Pad type - SLOW

                                                                  ALT4 - CT3_MAT3            Default - DIS

                                                                  ALT6 - FLEXIO0_D27

                                                                  ALT8 - PLU_OUT7

                                                                  ALT9 - SINC0_MCLK_OUT1

 P4_20            T10              41                          --               ALT0 - P4_20               IO Supply - VDD_P4       ANALOG -
                                                                                                              OPAMP2_INP0/ADC1_A6                                                                  ALT1 - TRIG_IN8          Pad type - SLOW

                                                                  ALT2 - FC2_P4               Default - DIS

                                                                  ALT4 - CT2_MAT0

                                                                  ALT6 - FLEXIO0_D28

                                                                  ALT9 - SINC0_MCLK2

 P4_21            T11                        --                           --               ALT0 - P4_21               IO Supply - VDD_P4       ANALOG -
                                                                                                              OPAMP2_INP1/ADC1_B6                                                                  ALT1 - TRIG_IN9          Pad type - SLOW

                                                                  ALT2 - FC2_P5               Default - DIS

                                                                  ALT4 - CT2_MAT1

                                                                  ALT6 - FLEXIO0_D29

                                                                  ALT9 - SINC0_MBIT2

 ANA_22          U10                       --                           --                                           IO Supply - VDD_P4       ANALOG - OPAMP2_INN

                                                                                         Pad type - LOLK

                               --                42                          --               ALT0 - P4_21               IO Supply - VDD_P4       ANALOG - OPAMP2_INP1/
                                                                                                             ADC1_B6/OPAMP2_INN                                                                  ALT1 - TRIG_IN9          Pad type - SLOW

                                                                  ALT2 - FC2_P5               Default - DIS

                                                                  ALT4 - CT2_MAT1

                                                                  ALT6 - FLEXIO0_D29

                                                                  ALT9 - SINC0_MBIT2

 P4_22            T12                        --                           --               ALT0 - P4_22               IO Supply - VDD_P4

                                                                  ALT4 - CT2_MAT2         Pad type - SLOW

                                                                  ALT6 - FLEXIO0_D30         Default - DIS

                                                       Tablecontinuesonthenextpage...




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    299 / 3791

<!-- page 300 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

 P4_23           U12              43                          --               ALT0 - P4_23               IO Supply - VDD_P4       ANALOG - OPAMP2_OUT/
                                                                                                                  ADC0_A2/ADC0_B2/                                                                  ALT1 - TRIG_OUT5        Pad type - SLOW
                                                                                                                ADC1_B3/CMP2_IN4P
                                                                  ALT2 - FC2_P6               Default - DIS

                                                                  ALT4 - CT2_MAT3

                                                                  ALT6 - FLEXIO0_D31

                                                                  ALT9 - SINC0_MCLK_OUT2

 VSS_P4          P9                         --                           --                                           IO Supply - VDD_P4

                                                                                         Pad type - VSSIO

 VDD_P4                 --                           --                           --                                           IO Supply - VDD_P4

                                                                                         Pad type - VDDIO

 VSS              J4                          --                           --                                           IO Supply - VDD_USB

                                                                                         Pad type - VSSIO

 USB1_DP        R13                       --                40                                          IO Supply - VDD_USB      ANALOG - USB1_DP

                                                                                         Pad type - ANA

 USB1_DM        R14                       --                41                                          IO Supply - VDD_USB      ANALOG - USB1_DM

                                                                                         Pad type - ANA

 USB1_ID         P11                        --                           --                                           IO Supply - VDD_USB      ANALOG - USB1_ID

                                                                                         Pad type - ANA

 USB1_VBUS      U14                       --                42                                          IO Supply - VDD_USB      ANALOG - USB1_VBUS

                                                                                         Pad type - VDDINT_5V

 VSS              J8                          --                43                                          IO Supply - VDD_USB

                                                                                         Pad type - VSSIO

 VDD_USB        R12              44               44                                          IO Supply - VDD_USB

                                                                                         Pad type - VDDIO

 USB0_DM         T14              45               45                                          IO Supply - VDD_USB      ANALOG - USB0_DM

                                                                                         Pad type - ANA          VDD SYS - WUU0_IN28

 USB0_DP         T15              46               46                                          IO Supply - VDD_USB      ANALOG - USB0_DP

                                                                                         Pad type - ANA          VDD SYS - WUU0_IN29

 VSS             T16                        --                           --                                           IO Supply - VDD_BAT

                                                                                         Pad type - VSSIO

 VDD_BAT         T17              47               47                                          IO Supply - VDD_BAT

                                                                                         Pad type - VDDIO

 P5_0            U16              48               48              ALT0 - P5_0                IO Supply - VDD_BAT      ANALOG -
                                                                                                               EXTAL32K/ADC1_B8                                                                  ALT1 - TRIG_IN10         Pad type - AON

                                                       Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    300 / 3791

<!-- page 301 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

                                                                  ALT2 - LPTMR0_ALT2        Default - DIS

 P5_1            U17              49               49              ALT0 - P5_1                IO Supply - VDD_BAT      ANALOG -
                                                                                                                XTAL32K/ADC1_B9                                                                  ALT1 - TRIG_OUT6        Pad type - AON

                                                                  ALT2 - LPTMR1_ALT2        Default - DIS

 P5_2           M10              50               50              ALT0 - P5_2                IO Supply - VDD_BAT      ANALOG - ADC1_B10

                                                                  ALT1 - VBAT_WAKEUP_b   Pad type - RST

                                                                  ALT2 - SPC_LPREQ          Default - ALT1

                                                                  ALT3 - TAMPER0

 P5_3            N11              51               51              ALT0 - P5_3                IO Supply - VDD_BAT      ANALOG - ADC1_B11

                                                                  ALT1 - TRIG_IN11         Pad type - AON

                                                                  ALT2 - RTC_CLKOUT         Default - DIS

                                                                  ALT3 - TAMPER1

 P5_4           M12                       --                           --               ALT0 - P5_4                IO Supply - VDD_BAT      ANALOG - ADC1_B12

                                                                  ALT1 - TRIG_OUT7        Pad type - AON

                                                                  ALT2 - SPC_LPREQ          Default - DIS

                                                                  ALT3 - TAMPER2

 P5_5            K12                        --                           --               ALT0 - P5_5                IO Supply - VDD_BAT      ANALOG - ADC1_B13

                                                                  ALT1 - TRIG_IN10         Pad type - AON

                                                                  ALT2 - LPTMR0_ALT2        Default - DIS

                                                                  ALT3 - TAMPER3

 P5_6            K13                        --                           --               ALT0 - P5_6                IO Supply - VDD_BAT      ANALOG - ADC1_B14

                                                                  ALT1 - TRIG_OUT6        Pad type - AON

                                                                  ALT2 - LPTMR1_ALT2        Default - DIS

                                                                  ALT3 - TAMPER4

 P5_7             L13                        --                           --               ALT0 - P5_7                IO Supply - VDD_BAT      ANALOG - ADC1_B15

                                                                  ALT1 - TRIG_IN11         Pad type - AON

                                                                  ALT3 - TAMPER5             Default - DIS

 P5_8             L14                        --                           --               ALT0 - P5_8                IO Supply - VDD_BAT      ANALOG - ADC1_B16

                                                                  ALT1 - TRIG_OUT7        Pad type - AON

                                                                  ALT3 - TAMPER6             Default - DIS

 P5_9           M14                       --                           --               ALT0 - P5_9                IO Supply - VDD_BAT      ANALOG - ADC1_B17

                                                                  ALT3 - TAMPER7          Pad type - AON

                                                                                                                     Default - DIS

 VSS             E13                        --                           --                                           IO Supply - VDD_BAT

                                                                                         Pad type - VSSIO

                                                       Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    301 / 3791

<!-- page 302 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

 VSS_DCDC       P16              52               52                                          IO Supply - VDD_DCDC

                                                                                         Pad type - VSSIO

 DCDC_LX        P17              53               53                                          IO Supply - VDD_DCDC    ANALOG - DCDC_LX

                                                                                         Pad type - ANA

 VDD_DCDC       R15              54               54                                          IO Supply - VDD_DCDC

                                                                                         Pad type - VDDIO

 VDD_LDO_SYS    P15              54               54                                          IO Supply - VDD_P3

                                                                                         Pad type - VDDIO

 VDD_SYS        N14              55               55                                          IO Supply - VDD_P3

                                                                                         Pad type - VDDINT

 VSS              J14                        --                           --                                           IO Supply - VDD_P3

                                                                                         Pad type - VSSIO

 P3_23          M15                       --                           --               ALT0 - P3_23               IO Supply - VDD_P3

                                                                  ALT3 - FC6_P3            Pad type - FAST

                                                                  ALT4 - CT_INP11             Default - DIS

                                                                  ALT5 - PWM1_X3

                                                                  ALT6 - FLEXIO0_D31

                                                                  ALT7 - SmartDMA_PIO23

                                                                  ALT10 - SAI1_TXD1

 P3_22          M16                       --                           --               ALT0 - P3_22               IO Supply - VDD_P3

                                                                  ALT2 - FC8_P6            Pad type - FAST

                                                                  ALT3 - FC6_P2               Default - DIS

                                                                  ALT4 - CT_INP10

                                                                  ALT5 - PWM1_X2

                                                                  ALT6 - FLEXIO0_D30

                                                                  ALT7 - SmartDMA_PIO22

                                                                  ALT9 - SIM0_VCCEN

                                                                  ALT10 - SAI1_RXD1

 P3_21            L16              56               56              ALT0 - P3_21               IO Supply - VDD_P3

                                                                  ALT1 - TRIG_OUT1        Pad type - FAST

                                                                  ALT2 - FC8_P5               Default - DIS

                                                                  ALT3 - FC6_P1

                                                                  ALT4 - CT2_MAT3

                                                                  ALT5 - PWM1_B3

                                                                  ALT6 - FLEXIO0_D29

                                                                  ALT7 - SmartDMA_PIO21

                                                       Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    302 / 3791

<!-- page 303 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

                                                                  ALT9 - SIM0_RST

                                                                  ALT10 - SAI1_RXD0

 P3_20          M17              57               57              ALT0 - P3_20               IO Supply - VDD_P3       VDD SYS - WUU0_IN27

                                                                  ALT1 - TRIG_OUT0        Pad type - FAST

                                                                  ALT2 - FC8_P4               Default - DIS

                                                                  ALT3 - FC6_P0

                                                                  ALT4 - CT2_MAT2

                                                                  ALT5 - PWM1_A3

                                                                  ALT6 - FLEXIO0_D28

                                                                  ALT7 - SmartDMA_PIO20

                                                                  ALT9 - SIM0_PD

                                                                  ALT10 - SAI1_TXD0

 P3_19           K17                        --                           --               ALT0 - P3_19               IO Supply - VDD_P3

                                                                  ALT2 - FC7_P6            Pad type - FAST

                                                                  ALT4 - CT2_MAT1            Default - DIS

                                                                  ALT5 - PWM1_X1

                                                                  ALT6 - FLEXIO0_D27

                                                                  ALT7 - SmartDMA_PIO19

                                                                  ALT10 - SAI1_RX_FS

 VSS            K9                         --                           --                                           IO Supply - VDD_P3

                                                                                         Pad type - VSSIO

 VDD_CORE       L11              58               58                                          IO Supply - VDD_P3

                                                                                         Pad type - VDDINT

 VDD_P3         G11              59               59                                          IO Supply - VDD_P3

                                                                                         Pad type - VDDIO

 P3_18           K16                        --                           --               ALT0 - P3_18               IO Supply - VDD_P3

                                                                  ALT3 - FC6_P6            Pad type - FAST

                                                                  ALT4 - CT2_MAT0            Default - DIS

                                                                  ALT5 - PWM1_X0

                                                                  ALT6 - FLEXIO0_D26

                                                                  ALT7 - SmartDMA_PIO18

                                                                  ALT10 - SAI1_RX_BCLK

 P3_17           K15              60               60              ALT0 - P3_17               IO Supply - VDD_P3       VDD SYS - WUU0_IN26

                                                                  ALT2 - FC8_P3            Pad type - FAST

                                                                  ALT4 - CT_INP9              Default - DIS

                                                                  ALT5 - PWM1_B2

                                                       Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    303 / 3791

<!-- page 304 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

                                                                  ALT6 - FLEXIO0_D25

                                                                  ALT7 - SmartDMA_PIO17

                                                                  ALT9 - SIM0_IO

                                                                  ALT10 - SAI1_TX_FS

 P3_16             J15              61               61              ALT0 - P3_16               IO Supply - VDD_P3

                                                                  ALT2 - FC8_P2            Pad type - FAST

                                                                  ALT4 - CT_INP8              Default - DIS

                                                                  ALT5 - PWM1_A2

                                                                  ALT6 - FLEXIO0_D24

                                                                  ALT7 - SmartDMA_PIO16

                                                                  ALT9 - SIM0_CLK

                                                                  ALT10 - SAI1_TX_BCLK

 P3_15           H15              62               62              ALT0 - P3_15               IO Supply - VDD_P3

                                                                  ALT2 - FC8_P1            Pad type - FAST

                                                                  ALT4 - CT_INP7              Default - DIS

                                                                  ALT5 - PWM1_B1

                                                                  ALT6 - FLEXIO0_D23

                                                                  ALT7 - SmartDMA_PIO15

                                                                  ALT8 - FLEXSPI0_A_DATA7

                                                                  ALT10 - SAI0_RX_FS

 P3_14           H17              63               63              ALT0 - P3_14               IO Supply - VDD_P3       VDD SYS - WUU0_IN25

                                                                  ALT2 - FC8_P0            Pad type - FAST

                                                                  ALT4 - CT_INP6              Default - DIS

                                                                  ALT5 - PWM1_A1

                                                                  ALT6 - FLEXIO0_D22

                                                                  ALT7 - SmartDMA_PIO14

                                                                  ALT8 - FLEXSPI0_A_DATA6

                                                                  ALT10 - SAI0_RX_BCLK

 P3_13           H16              64               64              ALT0 - P3_13               IO Supply - VDD_P3

                                                                  ALT2 - FC7_P5            Pad type - FAST

                                                                  ALT3 - FC6_P5               Default - DIS

                                                                  ALT4 - CT1_MAT3

                                                                  ALT5 - PWM1_B0

                                                                  ALT6 - FLEXIO0_D21

                                                                  ALT7 - SmartDMA_PIO13

                                                                  ALT8 - FLEXSPI0_A_DATA5

                                                                  ALT10 - SAI0_TXD1

                                                       Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    304 / 3791

<!-- page 305 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

 P3_12           G16              65               65              ALT0 - P3_12               IO Supply - VDD_P3

                                                                  ALT2 - FC7_P4            Pad type - FAST

                                                                  ALT3 - FC6_P4               Default - DIS

                                                                  ALT4 - CT1_MAT2

                                                                  ALT5 - PWM1_A0

                                                                  ALT6 - FLEXIO0_D20

                                                                  ALT7 - SmartDMA_PIO12

                                                                  ALT8 - FLEXSPI0_A_DATA4

                                                                  ALT10 - SAI0_RXD1

 VSS            N13                       --                           --                                           IO Supply - VDD_P3

                                                                                         Pad type - VSSIO

 VDD_P3         H10              66               66                                          IO Supply - VDD_P3

                                                                                         Pad type - VDDIO

 P3_11            F16              67               67              ALT0 - P3_11               IO Supply - VDD_P3       VDD SYS - WUU0_IN24

                                                                  ALT2 - FC6_P3            Pad type - FAST

                                                                  ALT3 - FC7_P5               Default - DIS

                                                                  ALT4 - CT1_MAT1

                                                                  ALT5 - PWM0_B3

                                                                  ALT6 - FLEXIO0_D19

                                                                  ALT7 - SmartDMA_PIO11

                                                                  ALT8 - FLEXSPI0_A_DATA3

                                                                  ALT9 - SIM0_IO

                                                                  ALT10 - SAI0_RXD0

 P3_10            F17              68               68              ALT0 - P3_10               IO Supply - VDD_P3

                                                                  ALT2 - FC6_P2            Pad type - FAST

                                                                  ALT3 - FC7_P4               Default - DIS

                                                                  ALT4 - CT1_MAT0

                                                                  ALT5 - PWM0_A3

                                                                  ALT6 - FLEXIO0_D18

                                                                  ALT7 - SmartDMA_PIO10

                                                                  ALT8 - FLEXSPI0_A_DATA2

                                                                  ALT9 - SIM0_CLK

                                                                  ALT10 - SAI0_TXD0

 P3_9             F15              69               69              ALT0 - P3_9                IO Supply - VDD_P3

                                                                  ALT2 - FC6_P5            Pad type - FAST

                                                                  ALT3 - FC7_P2               Default - DIS

                                                       Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    305 / 3791

<!-- page 306 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

                                                                  ALT4 - CT_INP5

                                                                  ALT5 - PWM0_B2

                                                                  ALT6 - FLEXIO0_D17

                                                                  ALT7 - SmartDMA_PIO9

                                                                  ALT8 - FLEXSPI0_A_DATA1

                                                                  ALT9 - SIM0_RST

                                                                  ALT10 - SAI0_TX_FS

 P3_8            E14              70               70              ALT0 - P3_8                IO Supply - VDD_P3       VDD SYS - WUU0_IN23

                                                                  ALT2 - FC6_P4            Pad type - FAST

                                                                  ALT3 - FC7_P0               Default - DIS

                                                                  ALT4 - CT_INP4

                                                                  ALT5 - PWM0_A2

                                                                  ALT6 - FLEXIO0_D16

                                                                  ALT7 - SmartDMA_PIO8

                                                                  ALT8 - FLEXSPI0_A_DATA0

                                                                  ALT9 - SIM0_PD

                                                                  ALT10 - SAI0_TX_BCLK

 P3_7            D14              71               71              ALT0 - P3_7                IO Supply - VDD_P3

                                                                  ALT2 - FC6_P6            Pad type - FAST

                                                                  ALT3 - FC7_P1               Default - DIS

                                                                  ALT4 - CT4_MAT3

                                                                  ALT5 - PWM0_B1

                                                                  ALT6 - FLEXIO0_D15

                                                                  ALT7 - SmartDMA_PIO7

                                                                  ALT8 - FLEXSPI0_A_SCLK

                                                                  ALT9 - SIM0_VCCEN

                                                                  ALT10 - SAI0_MCLK

 P3_6            D17              72               72              ALT0 - P3_6                IO Supply - VDD_P3

                                                                  ALT1 - CLKOUT           Pad type - FAST

                                                                  ALT2 - FC6_P1               Default - DIS

                                                                  ALT4 - CT4_MAT2

                                                                  ALT5 - PWM0_A1

                                                                  ALT6 - FLEXIO0_D14

                                                                  ALT7 - SmartDMA_PIO6

                                                                  ALT8 - FLEXSPI0_A_DQS

                                                                  ALT9 - SIM1_VCCEN

                                                                  ALT10 - SAI1_MCLK

                                                       Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    306 / 3791

<!-- page 307 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

 VSS             P12                        --                           --                                           IO Supply - VDD_P3

                                                                                         Pad type - VSSIO

 VDD_P3         H12              73               73                                          IO Supply - VDD_P3

                                                                                         Pad type - VDDIO

 P3_5            G14                       --                           --               ALT0 - P3_5                IO Supply - VDD_P3

                                                                  ALT2 - FC7_P3            Pad type - FAST

                                                                  ALT4 - CT_INP19             Default - DIS

                                                                  ALT5 - PWM0_X3

                                                                  ALT6 - FLEXIO0_D13

                                                                  ALT7 - SmartDMA_PIO5

                                                                  ALT9 - SIM1_IO

 P3_4             F14                        --                           --               ALT0 - P3_4                IO Supply - VDD_P3

                                                                  ALT2 - FC7_P2            Pad type - FAST

                                                                  ALT4 - CT_INP18             Default - DIS

                                                                  ALT5 - PWM0_X2

                                                                  ALT6 - FLEXIO0_D12

                                                                  ALT7 - SmartDMA_PIO4

                                                                  ALT9 - SIM1_CLK

 P3_3            D16                       --                           --               ALT0 - P3_3                IO Supply - VDD_P3

                                                                  ALT2 - FC7_P1            Pad type - FAST

                                                                  ALT4 - CT4_MAT1            Default - DIS

                                                                  ALT5 - PWM0_X1

                                                                  ALT6 - FLEXIO0_D11

                                                                  ALT7 - SmartDMA_PIO3

                                                                  ALT9 - SIM1_RST

 P3_2            D15                       --                           --               ALT0 - P3_2                IO Supply - VDD_P3

                                                                  ALT2 - FC7_P0            Pad type - FAST

                                                                  ALT4 - CT4_MAT0            Default - DIS

                                                                  ALT5 - PWM0_X0

                                                                  ALT6 - FLEXIO0_D10

                                                                  ALT7 - SmartDMA_PIO2

                                                                  ALT9 - SIM1_PD

 P3_1            C15              74               74              ALT0 - P3_1                IO Supply - VDD_P3

                                                                  ALT1 - TRIG_IN1          Pad type - FAST

                                                                  ALT2 - FC6_P0               Default - DIS

                                                                  ALT3 - FC7_P6

                                                       Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    307 / 3791

<!-- page 308 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

                                                                  ALT4 - CT_INP17

                                                                  ALT5 - PWM0_B0

                                                                  ALT6 - FLEXIO0_D9

                                                                  ALT7 - SmartDMA_PIO1

                                                                  ALT8 - FLEXSPI0_A_SS1_b

 P3_0            B17              75               75              ALT0 - P3_0                IO Supply - VDD_P3       VDD SYS - WUU0_IN22

                                                                  ALT1 - TRIG_IN0          Pad type - FAST

                                                                  ALT3 - FC7_P3               Default - DIS

                                                                  ALT4 - CT_INP16

                                                                  ALT5 - PWM0_A0

                                                                  ALT6 - FLEXIO0_D8

                                                                  ALT7 - SmartDMA_PIO0

                                                                  ALT8 - FLEXSPI0_A_SS0_b

 VSS            H9                         --                           --                                           IO Supply - VDD_P3

                                                                                         Pad type - VSSIO

 VDD_P3                 --                           --                           --                                           IO Supply - VDD_P3

                                                                                         Pad type - VDDIO

 VDD                       --                           --                           --                                           IO Supply - VDD

                                                                                         Pad type - VDDIO

 VSS              J10                        --                           --                                           IO Supply - VDD

                                                                                         Pad type - VSSIO

 P0_0            A17              76               76              ALT0 - P0_0                IO Supply - VDD

                                                                  ALT1 - TMS/SWDIO        Pad type - MED

                                                                  ALT2 - FC1_P0               Default - ALT1

                                                                  ALT4 - CT_INP0

 P0_1            A16              77               77              ALT0 - P0_1                IO Supply - VDD

                                                                  ALT1 - TCLK/SWCLK       Pad type - MED

                                                                  ALT2 - FC1_P1               Default - ALT1

                                                                  ALT4 - CT_INP1

 P0_2            B16              78               78              ALT0 - P0_2                IO Supply - VDD

                                                                  ALT1 - TDO/SWO          Pad type - MED

                                                                  ALT2 - FC1_P2               Default - ALT1

                                                                  ALT4 - CT0_MAT0

                                                                  ALT5 - UTICK_CAP0

                                                                  ALT10 - I3C0_PUR

                                                       Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    308 / 3791

<!-- page 309 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

 P0_3            B15              79               79              ALT0 - P0_3                IO Supply - VDD         ANALOG - CMP1_IN1

                                                                  ALT1 - TDI               Pad type - MED

                                                                  ALT2 - FC1_P3               Default - ALT1

                                                                  ALT4 - CT0_MAT1

                                                                  ALT5 - UTICK_CAP1

                                                                  ALT8 - HSCMP0_OUT

 P0_4            B14              80               80              ALT0 - P0_4                IO Supply - VDD         ANALOG - TSI0_CH8

                                                                  ALT1 - EWM0_IN          Pad type - MED+I2C       VDD SYS - WUU0_IN0

                                                                  ALT2 - FC0_P0               Default - DIS

                                                                  ALT3 - FC1_P4

                                                                  ALT4 - CT0_MAT2

                                                                  ALT5 - UTICK_CAP2

                                                                  ALT8 - HSCMP1_OUT

                                                                  ALT9 - PDM0_CLK

 P0_5            A14              81               81              ALT0 - P0_5                IO Supply - VDD         ANALOG - TSI0_CH9

                                                                  ALT1 - EWM0_OUT_b      Pad type - MED+I2C

                                                                  ALT2 - FC0_P1               Default - DIS

                                                                  ALT3 - FC1_P5

                                                                  ALT4 - CT0_MAT3

                                                                  ALT5 - UTICK_CAP3

                                                                  ALT9 - PDM0_DATA0

 P0_6            C14              82               82              ALT0 - P0_6                IO Supply - VDD            ISP - ISPMODE_N

                                                                  ALT1 - ISPMODE_N        Pad type - MED          ANALOG - TSI0_CH10

                                                                  ALT2 - FC0_P2               Default - ALT1

                                                                  ALT3 - FC1_P6

                                                                  ALT4 - CT_INP2

                                                                  ALT8 - HSCMP2_OUT

                                                                  ALT9 - PDM0_DATA1

 P0_7            C13                       --                           --               ALT0 - P0_7                IO Supply - VDD         ANALOG - CMP2_IN1

                                                                  ALT2 - FC0_P3            Pad type - MED          VDD SYS - WUU0_IN1

                                                                  ALT4 - CT_INP3              Default - DIS

 P0_8            C12                       --                           --               ALT0 - P0_8                IO Supply - VDD         ANALOG - ADC0_B8

                                                                  ALT2 - FC0_P4            Pad type - MED

                                                                  ALT4 - CT_INP0              Default - DIS

                                                                  ALT6 - FLEXIO0_D0

 P0_9            A12                        --                           --               ALT0 - P0_9                IO Supply - VDD         ANALOG - ADC0_B9

                                                       Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    309 / 3791

<!-- page 310 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

                                                                  ALT2 - FC0_P5            Pad type - MED

                                                                  ALT4 - CT_INP1              Default - DIS

                                                                  ALT6 - FLEXIO0_D1

 P0_10           B12                        --                           --               ALT0 - P0_10               IO Supply - VDD         ANALOG - ADC0_B10

                                                                  ALT2 - FC0_P6            Pad type - MED

                                                                  ALT4 - CT0_MAT0            Default - DIS

                                                                  ALT6 - FLEXIO0_D2

 P0_11           B11                        --                           --               ALT0 - P0_11               IO Supply - VDD         ANALOG - ADC0_B11

                                                                  ALT4 - CT0_MAT1         Pad type - MED

                                                                  ALT6 - FLEXIO0_D3          Default - DIS

                                                                  ALT8 - HSCMP2_OUT

 VDD           G7               83               83                                          IO Supply - VDD

                                                                                         Pad type - VDDIO

 VSS            D9                         --                           --                                           IO Supply - VDD

                                                                                         Pad type - VSSIO

 P0_12           D11                       --                           --               ALT0 - P0_12               IO Supply - VDD         ANALOG - ADC0_B12

                                                                  ALT2 - FC1_P4            Pad type - MED

                                                                  ALT3 - FC0_P0               Default - DIS

                                                                  ALT4 - CT0_MAT2

                                                                  ALT6 - FLEXIO0_D4

 P0_13            F12                        --                           --               ALT0 - P0_13               IO Supply - VDD         ANALOG - ADC0_B13

                                                                  ALT2 - FC1_P5            Pad type - MED

                                                                  ALT3 - FC0_P1               Default - DIS

                                                                  ALT4 - CT0_MAT3

                                                                  ALT6 - FLEXIO0_D5

 P0_14           E11                        --                           --               ALT0 - P0_14               IO Supply - VDD         ANALOG - ADC0_B14

                                                                  ALT2 - FC1_P6            Pad type - MED

                                                                  ALT3 - FC0_P2               Default - DIS

                                                                  ALT4 - CT_INP2

                                                                  ALT5 - UTICK_CAP0

                                                                  ALT6 - FLEXIO0_D6

 P0_15           G13                       --                           --               ALT0 - P0_15               IO Supply - VDD         ANALOG - ADC0_B15

                                                                  ALT3 - FC0_P3            Pad type - MED

                                                                  ALT4 - CT_INP3              Default - DIS

                                                                  ALT5 - UTICK_CAP1

                                                                  ALT6 - FLEXIO0_D7

                                                       Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    310 / 3791

<!-- page 311 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

 P0_16           B10              84               84              ALT0 - P0_16               IO Supply - VDD            ISP - I2C_SDA

                                                                  ALT2 - FC0_P0            Pad type - MED+I2C+I3C    ANALOG -
                                                                                                                   TSI0_CH11/ADC0_A8                                                                  ALT4 - CT0_MAT0            Default - DIS
                                                                                               VDD SYS - WUU0_IN2                                                                  ALT5 - UTICK_CAP2

                                                                  ALT6 - FLEXIO0_D0

                                                                  ALT9 - PDM0_CLK

                                                                  ALT10 - I3C0_SDA

 P0_17           A10              85               85              ALT0 - P0_17               IO Supply - VDD            ISP - I2C_SCL

                                                                  ALT2 - FC0_P1            Pad type - MED+I2C       ANALOG -
                                                                                                                   TSI0_CH12/ADC0_A9                                                                  ALT4 - CT0_MAT1            Default - DIS

                                                                  ALT5 - UTICK_CAP3

                                                                  ALT6 - FLEXIO0_D1

                                                                  ALT9 - PDM0_DATA0

                                                                  ALT10 - I3C0_SCL

 P0_18           C10              86               86              ALT0 - P0_18               IO Supply - VDD         ANALOG -
                                                                                                                    TSI0_CH13/ADC0_A10                                                                  ALT1 - EWM0_IN          Pad type - MED

                                                                  ALT2 - FC0_P2               Default - DIS

                                                                  ALT4 - CT0_MAT2

                                                                  ALT6 - FLEXIO0_D2

                                                                  ALT8 - HSCMP0_OUT

                                                                  ALT9 - PDM0_DATA1

 P0_19           C9               87               87              ALT0 - P0_19               IO Supply - VDD         ANALOG -
                                                                                                                    TSI0_CH14/ADC0_A11                                                                  ALT1 - EWM0_OUT_b      Pad type - MED
                                                                                               VDD SYS - WUU0_IN3                                                                  ALT2 - FC0_P3               Default - DIS

                                                                  ALT4 - CT0_MAT3

                                                                  ALT6 - FLEXIO0_D3

                                                                  ALT8 - HSCMP1_OUT

 P0_20           C8               88               88              ALT0 - P0_20               IO Supply - VDD         ANALOG -
                                                                                                                    TSI0_CH15/ADC0_A12                                                                  ALT2 - FC0_P4            Pad type - MED+I2C+I3C
                                                                                               VDD SYS - WUU0_IN4                                                                  ALT3 - FC1_P0               Default - DIS

                                                                  ALT4 - CT_INP0

                                                                  ALT6 - FLEXIO0_D4

                                                                  ALT10 - I3C0_SDA

 P0_21           A8               89               89              ALT0 - P0_21               IO Supply - VDD         ANALOG -
                                                                                                                    TSI0_CH16/ADC0_A13                                                                  ALT2 - FC0_P5            Pad type - MED+I2C

                                                                  ALT3 - FC1_P1               Default - DIS

                                                       Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    311 / 3791

<!-- page 312 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

                                                                  ALT4 - CT_INP1

                                                                  ALT6 - FLEXIO0_D5

                                                                  ALT10 - I3C0_SCL

 P0_22           B8               90               90              ALT0 - P0_22               IO Supply - VDD         ANALOG -
                                                                                                                 ADC0_A14/CMP1_IN2                                                                  ALT1 - EWM0_IN          Pad type - MED

                                                                  ALT2 - FC0_P6               Default - DIS

                                                                  ALT3 - FC1_P2

                                                                  ALT4 - CT_INP2

                                                                  ALT6 - FLEXIO0_D6

                                                                  ALT10 - I3C0_PUR

 P0_23           B7               91               91              ALT0 - P0_23               IO Supply - VDD         ANALOG -
                                                                                                                 ADC0_A15/CMP2_IN2                                                                  ALT1 - EWM0_OUT_b      Pad type - MED
                                                                                               VDD SYS - WUU0_IN5                                                                  ALT3 - FC1_P3               Default - DIS

                                                                  ALT4 - CT_INP3

                                                                  ALT6 - FLEXIO0_D7

 VSS            H13                       --                           --                                           IO Supply - VDD

                                                                                         Pad type - VSSIO

 P0_24           B6                         --                           --               ALT0 - P0_24               IO Supply - VDD         ANALOG - ADC0_B16

                                                                  ALT2 - FC1_P0            Pad type - MED

                                                                  ALT4 - CT0_MAT0            Default - DIS

 P0_25           A6                         --                           --               ALT0 - P0_25               IO Supply - VDD         ANALOG - ADC0_B17

                                                                  ALT2 - FC1_P1            Pad type - MED

                                                                  ALT4 - CT0_MAT1            Default - DIS

 P0_26            F10                        --                           --               ALT0 - P0_26               IO Supply - VDD         ANALOG - ADC0_B18

                                                                  ALT2 - FC1_P2            Pad type - MED

                                                                  ALT4 - CT0_MAT2            Default - DIS

 P0_27           E10                        --                           --               ALT0 - P0_27               IO Supply - VDD         ANALOG - ADC0_B19

                                                                  ALT2 - FC1_P3            Pad type - MED

                                                                  ALT4 - CT0_MAT3            Default - DIS

 P0_28           E8                         --                           --               ALT0 - P0_28               IO Supply - VDD         ANALOG - ADC0_B20

                                                                  ALT2 - FC1_P4            Pad type - MED

                                                                  ALT3 - FC0_P4               Default - DIS

                                                                  ALT4 - CT_INP0

 P0_29            F8                         --                           --               ALT0 - P0_29               IO Supply - VDD         ANALOG - ADC0_B21

                                                                  ALT2 - FC1_P5            Pad type - MED

                                                       Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    312 / 3791

<!-- page 313 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

                                                                  ALT3 - FC0_P5               Default - DIS

                                                                  ALT4 - CT_INP1

 P0_30           E7                         --                           --               ALT0 - P0_30               IO Supply - VDD         ANALOG - ADC0_B22

                                                                  ALT2 - FC1_P6            Pad type - MED

                                                                  ALT3 - FC0_P6               Default - DIS

                                                                  ALT4 - CT_INP2

 P0_31           D7                         --                           --               ALT0 - P0_31               IO Supply - VDD         ANALOG - ADC0_B23

                                                                  ALT4 - CT_INP3           Pad type - MED

                                                                                                                     Default - DIS

 P1_0           C6               92               92              ALT0 - P1_0                IO Supply - VDD            ISP - SPI_SDO

                                                                  ALT1 - TRIG_IN0          Pad type - MED+I2C       ANALOG - TSI0_CH0/
                                                                                                                 ADC0_A16/CMP0_IN0                                                                  ALT2 - FC3_P0               Default - DIS
                                                                                               VDD SYS -                                                                  ALT3 - FC4_P4
                                                                                                              WUU0_IN6/LPTMR0_ALT3
                                                                  ALT4 - CT_INP4

                                                                  ALT5 - SCT0_OUT6

                                                                  ALT6 - FLEXIO0_D8

                                                                  ALT10 - SAI1_TX_BCLK

 P1_1           C5               93               93              ALT0 - P1_1                IO Supply - VDD            ISP - SPI_SCK

                                                                  ALT1 - TRIG_IN1          Pad type - MED+I2C       ANALOG - TSI0_CH1/
                                                                                                                 ADC0_A17/CMP1_IN0                                                                  ALT2 - FC3_P1               Default - DIS

                                                                  ALT3 - FC4_P5

                                                                  ALT4 - CT_INP5

                                                                  ALT5 - SCT0_OUT7

                                                                  ALT6 - FLEXIO0_D9

                                                                  ALT10 - SAI1_TX_FS

 P1_2           C4               94               94              ALT0 - P1_2                IO Supply - VDD            ISP - SPI_SDI

                                                                  ALT1 - TRIG_OUT0        Pad type - MED          ANALOG - TSI0_CH2/
                                                                                                                 ADC0_A18/CMP2_IN0                                                                  ALT2 - FC3_P2               Default - DIS

                                                                  ALT3 - FC4_P6

                                                                  ALT4 - CT1_MAT0

                                                                  ALT5 - SCT0_IN6

                                                                  ALT6 - FLEXIO0_D10

                                                                  ALT9 - ENET0_MDC

                                                                  ALT10 - SAI1_TXD0

                                                                  ALT11 - CAN0_TXD

 P1_3            B4               95               95              ALT0 - P1_3                IO Supply - VDD            ISP - SPI_PCS

                                                       Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    313 / 3791

<!-- page 314 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

                                                                  ALT1 - TRIG_OUT1        Pad type - MED          ANALOG - TSI0_CH3/
                                                                                                                 ADC0_A19/CMP0_IN1                                                                  ALT2 - FC3_P3               Default - DIS
                                                                                               VDD SYS - WUU0_IN7                                                                  ALT4 - CT1_MAT1

                                                                  ALT5 - SCT0_IN7

                                                                  ALT6 - FLEXIO0_D11

                                                                  ALT9 - ENET0_MDIO

                                                                  ALT10 - SAI1_RXD0

                                                                  ALT11 - CAN0_RXD

 VDD            H6               96               96                                          IO Supply - VDD

                                                                                         Pad type - VDDIO

 VSS            D12                       --                           --                                           IO Supply - VDD

                                                                                         Pad type - VSSIO

 P1_4            A4               97               97              ALT0 - P1_4                IO Supply - VDD         ANALOG - TSI0_CH4/
                                                                                                                 ADC0_A20/CMP0_IN2                                                                  ALT1 - FREQME_CLK_IN0   Pad type - MED
                                                                                               VDD SYS - WUU0_IN8                                                                  ALT2 - FC3_P4               Default - DIS

                                                                  ALT3 - FC5_P0

                                                                  ALT4 - CT1_MAT2

                                                                  ALT5 - SCT0_OUT0

                                                                  ALT6 - FLEXIO0_D12

                                                                  ALT7 - SmartDMA_PIO0

                                                                  ALT9 - ENET0_TX_CLK

                                                                  ALT10 - SAI0_TXD1

 P1_5            B3               98               98              ALT0 - P1_5                IO Supply - VDD         ANALOG - TSI0_CH5/
                                                                                                                 ADC0_A21/CMP0_IN3                                                                  ALT1 - FREQME_CLK_IN1   Pad type - MED

                                                                  ALT2 - FC3_P5               Default - DIS

                                                                  ALT3 - FC5_P1

                                                                  ALT4 - CT1_MAT3

                                                                  ALT5 - SCT0_OUT1

                                                                  ALT6 - FLEXIO0_D13

                                                                  ALT7 - SmartDMA_PIO1

                                                                  ALT9 - ENET0_TXEN

                                                                  ALT10 - SAI0_RXD1

 P1_6            B2               99               99              ALT0 - P1_6                IO Supply - VDD         ANALOG -
                                                                                                                   TSI0_CH6/ADC0_A22                                                                  ALT1 - TRIG_IN2          Pad type - MED

                                                                  ALT2 - FC3_P6               Default - DIS

                                                                  ALT3 - FC5_P2

                                                       Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    314 / 3791

<!-- page 315 -->

NXP Semiconductors
                                                                                                                      Signal Multiplexing


Table 119. pinmux (continued)

      Pin Name       184BGA ALL     100HLQFP N94X   100HLQFP N54X      Pinmux Assignment           Pad Settings               Alternate Functions

                                                                  ALT4 - CT_INP6

                                                                  ALT5 - SCT0_IN0

                                                                  ALT6 - FLEXIO0_D14

                                                                  ALT7 - SmartDMA_PIO2

                                                                  ALT9 - ENET0_TXD0

                                                                  ALT10 - SAI1_RX_BCLK

                                                                  ALT11 - CAN1_TXD

 P1_7            A2               100              100             ALT0 - P1_7                IO Supply - VDD         ANALOG -
                                                                                                                   TSI0_CH7/ADC0_A23                                                                  ALT1 - TRIG_OUT2        Pad type - MED
                                                                                               VDD SYS - WUU0_IN9                                                                  ALT3 - FC5_P3               Default - DIS

                                                                  ALT4 - CT_INP7

                                                                  ALT5 - SCT0_IN1

                                                                  ALT6 - FLEXIO0_D15

                                                                  ALT7 - SmartDMA_PIO3

                                                                  ALT8 - PLU_CLK

                                                                  ALT9 - ENET0_TXD1

                                                                  ALT10 - SAI1_RX_FS

                                                                  ALT11 - CAN1_RXD

Note:
1. For BGA package, all balls with same name are shorted together on BGA package.
2. VSS_ANA and VSS_P4 are shorted together on package.
3. +I3C in Pad Type means strong pull up resistor is implemented on the pin. PV bit is implemented in the Pin
Control register of the pin.
4. +I2C in Pad Type represents I2C filter is implemented on the pin. PFE bit is implemented in the Pin Control
register of the pin.
5. DIS in default column means the pin's input buffer is disabled by default.
6. AON and RST pads support passive filter. PFE bit is implemented in the Pin Control register of the pin.
7. PE, PS, SRE, and DSE are supported in the Pin Control register of all types of IO.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    315 / 3791
