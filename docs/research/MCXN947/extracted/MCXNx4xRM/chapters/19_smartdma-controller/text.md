# 19 SmartDMA Controller

Source: `docs/mcxn947/MCXNx4xRM.pdf`
Pages: 694-699

<!-- page 694 -->

NXP Semiconductors

Chapter 19
SmartDMA Controller

19.1  Chip-specific SmartDMA Controller information

Table 284. Reference links to related information

         Topic             Related module                                Reference

      Full description     SmartDMA Controller                     SmartDMA Controller

  System memory map                                           Memory map

        Clocking                                                        Clock distribution

   Signal multiplexing          Port control                                  Signal multiplexing
    Input multiplexing       INPUTMUX        See SMARTDMAARCHB_INMUX0 - SMARTDMAARCHB_INMUX7
                                                                                registers in INPUTMUX

19.1.1  Clock
The SmartDMA clock can be enabled or disabled by setting the SmartDMA field in the AHBCLKCTRL1 register.

19.1.2 Reset
The SmartDMA module can be reset by setting the corresponding field in the PRESETCTRL1 register. Clear the SmartDMA
peripheral reset using the PRESETCTRLCLR1 register before using SmartDMA.

19.1.3  Interrupts
SmartDMA interrupt number is 53 (SmartDMA_IRQ).

19.1.4  Triggers
SmartDMA has 71 input sources in total in this chip. The following table lists the trigger sources of SmartDMA:

Table 285. SmartDMA trigger sources

 Input                                                      Function

 0                                            GPIO P0_0

 1                                            GPIO P0_1

 2                                            GPIO P0_2

 3                                            GPIO P0_3

 4                                            GPIO P0_4

 5                                            GPIO P0_5

 6                                            GPIO P0_6

 7                                            GPIO P0_7

 8                                            GPIO P0_8

 9                                            GPIO P0_9

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    694 / 3791

<!-- page 695 -->

NXP Semiconductors
                                                                                        SmartDMA Controller

Table 285. SmartDMA trigger sources (continued)

 Input                                                      Function

 10                                           GPIO P0_10

 11                                           GPIO P0_11

 12                                           GPIO P0_12

 13                                           GPIO P0_13

 14                                           GPIO P0_14

 15                                           GPIO P0_15

 16                                          SCT_OUT8

 17                                          SCT_OUT9

 18                                                  Reserved

 19                                                  Reserved

 20                                           MRT_CH0_IRQ

 21                                           MRT_CH1_IRQ

 22                                           CTIMER4_MAT3

 23                                           CTIMER4_MAT2

 24                                           CTIMER3_MAT3

 25                                           CTIMER3_MAT2

 26                                           CTIMER1_MAT3

 27                                           CTIMER1_MAT2

 28                                              UTICK_IRQ

 29                                          WDT0_IRQ

 30                                           ADC0_IRQ

 31                                           CMP0_IRQ

 32                                                  Reserved

 33                                          LP_FLEXCOMM7_IRQ

 34                                          LP_FLEXCOMM6_IRQ

 35                                          LP_FLEXCOMM5_IRQ

 36                                          LP_FLEXCOMM4_IRQ

 37                                          LP_FLEXCOMM3_IRQ

 38                                          LP_FLEXCOMM2_IRQ

 39                                          LP_FLEXCOMM1_IRQ

 40                                          LP_FLEXCOMM0_IRQ

 41                                           DMA0_IRQ

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    695 / 3791

<!-- page 696 -->

NXP Semiconductors
                                                                                        SmartDMA Controller

Table 285. SmartDMA trigger sources (continued)

 Input                                                      Function

 42                                           DMA1_IRQ
 43                                              SYS_IRQ1

 44                                         RTC_COMBO_IRQ

 45                                        ARM_TXEV

 46                                               PINT GPIO_INT_BMAT

 47                                                  Reserved

 48                                                  Reserved

 49                                        CMP0_OUT

 50                                         USB0 start of frame

 51                                         USB1 start of frame

 52                                           OS_EVENT_TIMER_IRQ

 53                                           ADC1_IRQ

 54                                            CMP0_IRQ/CMP1_IRQ/CMP2_IRQ

 55                                           DAC0_IRQ

 56                                             DAC1_IRQ/DAC2_IRQ

 57                                         PWM0_IRQ

 58                                         PWM1_IRQ

 59                                           QDC0_IRQ

 60                                           QDC1_IRQ

 61                                         EVTG_OUT0A

 62                                         EVTG_OUT1A

 63                                                  Reserved

 64                                                  Reserved

 65                                            GPIO1 Pin Event Trig 0

 66                                            GPIO1 Pin Event Trig 1

 67                                            GPIO2 Pin Event Trig 0

 68                                            GPIO2 Pin Event Trig 1

 69                                            GPIO3 Pin Event Trig 0

 70                                            GPIO3 Pin Event Trig 1

1. SYS_IRQ combines the CDOG IRQ, WWDT IRQ, MBC secure violation IRQ, Secure AHB Matrix secure violation IRQ, GDET
   IRQ, ELS S50 error IRQ, PKC error IRQ, and VBAT IRQ using the logical OR operation.

19.1.5 Memory map
The priority of AHB bus masters can be adjusted by using the SYSCON[AHBMATPRIO] register.



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    696 / 3791

<!-- page 697 -->

NXP Semiconductors
                                                                                        SmartDMA Controller

19.2 Overview
SmartDMA is a core that supports unique, reduced instruction sets. It works in a similar way to the Arm core. Being the controller
of AHB matrix, SmartDMA can access:
   •  All its targets.
   • The GPIO peripheral control and data registers.

The purpose of SmartDMA is to perform event- and I/O-driven handling to offload the Arm processor in the system.

19.2.1  Block diagram




                                Pattern detector
             8-bit input bus
  (all eight bits go to each           Bit - Slice 0
           of the bit slices)                                                                          Heartbeat rhythm
                                         Bit - Slice 1

                                                                                                        GPIO interface
                                         Bit - Slice 2

                                         Bit - Slice 3                                        ALU
                                                         Logical combiner
                                         Bit - Slice 4                                                                    Registers


                                         Bit - Slice 5
                                                                                 Decode
                                                                                                         SmartDMA
                                         Bit - Slice 6                                                                                                                                              breakpoint

                                         Bit - Slice 7                                          IFETCH    DFETCH





                                                                                               Instruction                                  Interrupt
                                      Vector detector and address calculator                           Data - AHB
                                                                                                                           - AHB                                     to Arm


 Figure 100. Block diagram

19.2.2  Features
SmartDMA is designed to deliver an optimal performance for the following purposes:
   • State machine control
   • Boolean detection
   • Tedious or repetitive simple tasks
   • DMA function
   • Interrupt load reduction on Arm
   • Streaming GPIO
   • General I/O handling
   • Protocol emulation over I/O, such as parallel camera and LCD interfaces
   • Shift-based algorithm calculation
   • Large-scale data manipulation





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    697 / 3791

<!-- page 698 -->

NXP Semiconductors
                                                                                        SmartDMA Controller

19.3  Functional description

19.3.1  Clocking
SmartDMA uses the same system clock as the Arm core does. You can enable or disable the SmartDMA clock by configuring
the appropriate register field (see the chip-specific section for details). However, you must enable the clock for SmartDMA before
setting the SmartDMA controller and SmartDMA program execution.

19.3.2 Reset
You can reset the SmartDMA module by configuring the corresponding register field. See the chip-specific section for details.

19.3.3  Interrupts
You can enable the Arm interrupt for SmartDMA via nested vectored interrupt controller (NVIC). See the chip-specific section
for details.

19.3.4  Pin description
SmartDMA can access and manipulate a maximum of 32 I/O pins that are selected as “SmartDMA function”. You can configure
“SmartDMA function” by using signal multiplexing.
SmartDMA includes dedicated GPIO control registers through which it can quickly access I/O ports. See the chip-specific section
for details.

19.3.5 Requests and triggers
In general, SmartDMA requests set the pace of execution to match what the peripheral (including its FIFO if it has one) could do.
For example,
   • USART issues a transmit SmartDMA request when its transmit FIFO is not full, and a receive SmartDMA request when its
    receive FIFO is not empty.
   • The GPIO pin issues a data storage SmartDMA request when its rising edge is detected.
   • The timer event issues a PWM output SmartDMA request when the counter reaches zero.
   • ADC issues a result reading SmartDMA request after the conversion is complete.
SmartDMA includes a number of channels connected to external input sources. See the chip-specific section for details.

19.3.6  Setting and power consumption in different modes
In Sleep mode, SmartDMA:
   •  Is active.
   • Can operate and access all enabled peripherals and memories.
   • Can wake up the Arm core from Sleep mode.
In Power-Down and Deep Power-Down modes, SmartDMA:
   •  Is inactive.
   •  Is configurable for low-power consumption by:
   — Disabling the SmartDMA clock when not in use.
   — Placing SmartDMA into the reset state.
   — Holding the SmartDMA core by using some opcodes.
   — Slowing down the frequency of SmartDMA execution.



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    698 / 3791

<!-- page 699 -->

NXP Semiconductors
                                                                                        SmartDMA Controller

19.4 Memory map
SmartDMA includes two AHB bus master interfaces. You can adjust the priority of AHB bus masters to achieve the desired system
performance. See the chip-specific section for details.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    699 / 3791
