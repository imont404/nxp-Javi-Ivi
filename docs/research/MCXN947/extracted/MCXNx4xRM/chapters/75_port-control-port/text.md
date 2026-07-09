# 75 Port Control (PORT)

Source: `docs/mcxn947/MCXNx4xRM.pdf`
Pages: 3570-3696

<!-- page 3570 -->

NXP Semiconductors

Chapter 75
Port Control (PORT)

75.1  Chip-specific PORT information

Table 713. Reference links to related information

         Topic             Related module                                Reference

      Full description         PORT                            PORT

        Clocking                                                        Clock distribution

  Power management                                           Power management

   Signal multiplexing          Port control                                  Signal multiplexing

                                           NOTE
           See Peripheral Reset Control 0 (PRESETCTRL0) to know how to reset PORT. To enable PORT clock, see AHB
              Clock Control 0 (AHBCLKCTRL0).

75.1.1 Module instances
This device has six instances of the port module, PORT0, PORT1, PORT2, PORT3, PORT4, and PORT5.

                                           NOTE
                               For PORT5, the EFT detect function can only work when VBAT = 1.8 V.

75.1.2  Security considerations
For secure applications, NXP recommends that this module is configured for secure and privileged access only to prevent
unintentional or malicious modification of the system operation by nonsecure software. The secure AHB controller controls the
security level for access to peripherals and does default to secure and privileged access for all peripherals.

75.2 Overview
PORT provides support for pad control functions. You can configure most functions independently for each pin, in the 32-bit port,
and affect the pin regardless of its pin multiplexing state.
There exists a single instance of the PORT module for each port, and not all pins within each port are implemented on a
specific chip.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3570 / 3791

<!-- page 3571 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

75.2.1  Block diagram


                                                 PORT


                                                                 Multiplex                Pin                                                                                                            Port pin 31
                                                                    control                 control





                              Signals to and                   Multiplex                Pin                                                                                                            Port pin n
                           from peripherals                   control                 control





                                                                 Multiplex                Pin                                                                                                            Port pin 0
                                                                    control                 control




 Figure 444. Block diagram

75.2.2  Features
   • Individual pull control fields with pullup, pulldown, and pull-disable support
   • Individual PCRn[PFE] fields that enable and disable individual input passive filters
   • Individual PCRn[ODE] fields that enable and disable individual open drain outputs
   • Digital input inversion to optionally invert the digital input
   • Individual electrical fast transient (EFT) detect with an EFT detect flag and associated interrupt
   • Digital PCRn[IBE] fields to configure between analog or disabled functions and digital functions
   • Individual PCRn[MUX] fields supporting GPIO and up to 3 chip-specific digital functions

75.3  Functional description

75.3.1  Pin control
Each port pin has a corresponding Pin Control Register (PCR) associated with it that helps you configure the following functions
for each pin within the 32-bit port:
   • Pullup or pulldown enable
   • Open drain enable
   • Passive input filter enable
   • Digital input inversion
   • EFT detection
   • Software configuration lock
   • Pin multiplexing mode
These functions apply across all digital pin multiplexing modes, and individual peripherals do not override the configuration in
PCRnunless otherwise noted. For example, if an I2C function is enabled on a pin, it does not override the pullup or open drain
configuration for that pin.


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3571 / 3791

<!-- page 3572 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

PCRn[LK] allows the configuration for each pin to be locked until the next system reset. When locked, writes to the lower half of
that PCRnare ignored, although a bus error is not generated on an attempted write to a locked register.
The configuration of each PCRnis retained when the PORT module is disabled.
When you configure a pin in a digital pin multiplexing mode, the input buffer for that pin is enabled, allowing the pin state to be
read via the corresponding GPIO.PDIR or allowing a pin interrupt or DMA request to be generated. If a pin is always floating when
its input buffer is enabled, it can cause an increase in power consumption. This situation must be avoided. A pin can be floating
because of an input pin that is not connected or an output pin that is tristated (output buffer is disabled).
Enabling the internal pull resistor (or implementing an external pull resistor) ensures that a pin does not float when its input buffer
is enabled. The internal pull resistor is automatically disabled whenever the output buffer is enabled, allowing PCRn[PE] to remain
1. Configuring Pin Multiplexing mode to disabled or analog (PCRn[MUX] = 0) disables the pin’s input buffer and results in lowest
power consumption.

75.3.2  Global pin control
Global Pin Control Low (GPCLR) and Global Pin Control High (GPCHR) allow a single register write to update the lower 16 bits
of PCRnfor up to 16 pins, all with the same value. You cannot write to locked registers by using Global Pin Control Low (GPCLR)
and Global Pin Control High (GPCHR).
Global Pin Control Low (GPCLR) and Global Pin Control High (GPCHR) enable you to quickly configure multiple pins within the
same port and with the same peripheral function. These are write-only registers that always read as 0.

75.3.3 EFT detection
The EFT detect circuit is active whenever PORT is powered and EDCR[EDLC] and EDCR[EDHC] are 0.
The EFT high detect circuit for each pin is reset whenever EDCR[EDHC] = 1. Likewise, the EFT low detect circuit for each pin is
reset whenever EDCR[EDLC] = 1 and that pin's EFT high detect is not asserted.

75.3.4  Clocking
This module has no clocking considerations.

75.3.5  Interrupt
When EFT on a pin is detected, an interrupt request is generated to indicate the detection of EFT on that pin. This interrupt is
enabled when EDIER[EDIEn] = 1. Writing 0 to these fields clears the interrupt request but does not affect EFT detection on the
corresponding pin (EDFR[EDFn]).

75.3.6  Calibration
The combination of Calibration 0 (CALIB0) and Calibration 1 (CALIB1) controls the drive strength of a pin for the port and
PCRn[DSE] for the pin.
Calibration 0 (CALIB0) and Calibration 1 (CALIB1) represent two driver configurations. If PCRn[DSE] = 0, the configuration in
Calibration 0 (CALIB0) is used. Likewise, if PCRn[DSE] = 1, the configuration in Calibration 1 (CALIB1) is used.
The pulldown and pullup drivers have eight segments. The value of NCAL[5:3] represents the number of pulldown segments
that are enabled when driving low, and the value of PCAL[5:3] represents the number of pullup segments that turn on when
driving high. Writing 000b to these fields causes one segment to turn on, while writing 111b causes all eight segments to turn on.
Upon reset, Calibration 0 (CALIB0) becomes 1 to create approximately 50 Ω driver at 3.3 V and 25 °C (3 is a typical value) and
Calibration 1 (CALIB1) becomes 1 to create approximately 50 Ω driver at 1.8 V and 25 °C (7 is the typical value).
The three LSBs of CALIB0[NCAL], CALIB1[NCAL], CALIB0[PCAL], and CALIB1[PCAL] do not affect the driver's strength.
Instead, they are included to improve precision when calibrating to a different impedance value. This can be done by using the
following formula:
Calibration = (G × (CAL0 + 4) – 1024) >> 8, where G is a binary code representing the desired conductance and CAL0 is one of the
6-bit fields in Calibration 0 (CALIB0) and Calibration 1 (CALIB1). The value of G scales linearly with the intended conductance; G =


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3572 / 3791

<!-- page 3573 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

0 corresponds to 0 S (an open circuit), and G = 256 corresponds to 0.02 S (50 Ω). For example, the following operation configures
the driver to be approximately 0.03 S (33 Ω) at 3.3 V when PCRn[DSE] = 1:
The value of CALIB1[NCAL] = (384 × (the value of CALIB0[NCAL] + 4) – 1024) >> 8;
The value of CALIB1[PCAL] = (384 × (the value of CALIB0[PCAL] + 4) – 1024) >> 8;
Do not overflow or underflow NCAL[5:3] or PCAL[5:3]; otherwise, this operation may have an unintended result. You can configure
the drive strength for 50 Ω at 2.5 V and 25 °C by averaging the initial NCAL codes in Calibration 0 (CALIB0) and Calibration
1 (CALIB1), and averaging the initial PCAL codes in Calibration 0 (CALIB0) and Calibration 1 (CALIB1). However, the 2.5 V
configuration is not as accurate or precise as the original 1.8 V and 3.3 V configuration because the drive strength is not perfectly
linear with supply voltage.

                                           NOTE
              This calibration feature may not apply to all PORT pins. See the chip-specific PORT information for pins that
              support calibration.

75.4  Initialization
To initialize PORT, perform the following procedure:
   1.  Initialize the pin functions:
           •  Initialize single pin functions by writing appropriate values to PCRn.
           •  Initialize multiple pins (up to 16) with the same configuration by writing appropriate values to Global Pin Control Low
        (GPCLR) or Global Pin Control High (GPCHR).
   2. Lock the configuration for a given pin, by writing 1 to PCRn[LK], so that it cannot be changed until the next reset.

75.5  Application information

75.5.1  Determine polarity of an asserted EFT detection
You can determine the polarity of an asserted EFT detect by performing this procedure:
   1. Read EFT Detect Flag (EDFR).
   2. Write 1 and then write 0 to EDCR[EDLC].
   3. Read EFT Detect Flag (EDFR) again. Any field that has now become 0 indicates triggering of the EFT low detector only (no
     EFT high detector triggering).
   4. Write 1 and then write 0 to EDCR[EDHC].
   5. Read EFT Detect Flag (EDFR) again. Any field that has now become 0 indicates triggering of the EFT high detector. Any
        field that did not become 0 indicates triggering of both the EFT high and EFT low detectors.
   6. Write 1 and then write 0 to both EDCR[EDLC] and EDCR[EDHC] to ensure that all detectors are reset.

75.6 Memory map and register definition
Any read or write access to the PORT memory space, outside the valid memory map, results in a bus error. All register accesses
complete with zero wait states.

75.6.1 PORT register descriptions

75.6.1.1 PORT memory map
PORT0 base address: 4011_6000h
PORT1 base address: 4011_7000h



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3573 / 3791

<!-- page 3574 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

PORT2 base address: 4011_8000h
PORT3 base address: 4011_9000h
PORT4 base address: 4011_A000h
PORT5 base address: 4004_2000h

     Offset     Register                                                        Width    Access   Reset value
                                                                                                     (In bits)

     0h       Version ID (VERID)                                           32     R     0200_0000h

     10h      Global Pin Control Low (GPCLR)                                 32    W     0000_0000h

     14h      Global Pin Control High (GPCHR)                                32    W     0000_0000h

     20h       Configuration (CONFIG)                                        32    RW    0000_0000h

     40h     EFT Detect Flag (EDFR)                                       32     R     0000_0000h

     44h     EFT Detect Interrupt Enable (EDIER)                             32    RW    0000_0000h

     48h     EFT Detect Clear (EDCR)                                      32    RW    0000_0000h

     60h       Calibration 0 (CALIB0)                                         32    RW    See section

     64h       Calibration 1 (CALIB1)                                         32    RW    See section

     80h      Pin Control 0 (PCR0)                                          32    RW    See section

     84h      Pin Control 1 (PCR1)                                          32    RW    See section

     88h      Pin Control 2 (PCR2)                                          32    RW    See section

    8Ch      Pin Control 3 (PCR3)                                          32    RW    See section

   90h - 94h    Pin Control a (PCR4 - PCR5)                                    32    RW    0000_0000h

     98h      Pin Control 6 (PCR6)                                          32    RW    See section

  9Ch - A4h   Pin Control a (PCR7 - PCR9)                                    32    RW    See section

  A8h - BCh   Pin Control a (PCR10 - PCR15)                                  32    RW    See section

    C0h      Pin Control 16 (PCR16)                                        32    RW    0000_0000h

    C4h      Pin Control 17 (PCR17)                                        32    RW    0000_0000h

  C8h - CCh   Pin Control a (PCR18 - PCR19)                                  32    RW    0000_0000h

    D0h      Pin Control 20 (PCR20)                                        32    RW    0000_0000h

    D4h      Pin Control 21 (PCR21)                                        32    RW    0000_0000h

  D8h - FCh   Pin Control a (PCR22 - PCR31)                                  32    RW    See section

75.6.1.2  Version ID (VERID)

Offset


 Register                   Offset

 VERID                0h


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3574 / 3791

<!-- page 3575 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

Function
Specifies the version number and feature number of the chip.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                      MAJOR                                            MINOR

   W

Reset     0      0      0      0       0      0      1      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                               FEATURE

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function

     31-24      Major Version Number
   MAJOR      Indicates the major version number for the specification.

     23-16      Minor Version Number
    MINOR      Indicates the minor version number for the specification.

      15-0       Feature Specification Number
   FEATURE     Indicates the feature set number.
                     0000_0000_0000_0000b - Basic implementation


75.6.1.3  Global Pin Control Low (GPCLR)

Offset


 Register                   Offset

 GPCLR               10h


Function
Controls writes to the PCR15–PCR0 registers.

                                           NOTE
                                                This register supports only 32-bit writes.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3575 / 3791

<!-- page 3576 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R    0      0      0      0      0      0      0      0      0      0      0      0      0      0      0      0

      GPWE GPWE GPWE GPWE GPWE GPWE GPWE GPWE GPWE GPWE GPWE GPWE GPWE GPWE GPWE GPWE
   W
          15     14     13     12     11     10      9      8      7      6      5      4      3      2      1      0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                            0

   W                                         GPWD

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function

     31-16      Global Pin Write Enable
   GPWEn     Configures the corresponding lower 16-bit field of PCRnto be updated with the value in the GPWD field.
                              If a selected PCR is locked, the write to that register is ignored.
                     0b - Not updated
                     1b - Updated

      15-0       Global Pin Write Data
   GPWD       Is written to PCRn[15:0] if GPWEn= 1.


75.6.1.4  Global Pin Control High (GPCHR)

Offset


 Register                   Offset

 GPCHR               14h


Function
Controls writes to the PCR31–PCR16 registers.

                                           NOTE
                                                This register supports only 32-bit writes.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3576 / 3791

<!-- page 3577 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R    0      0      0      0      0      0      0      0      0      0      0      0      0      0      0      0

      GPWE GPWE GPWE GPWE GPWE GPWE GPWE GPWE GPWE GPWE GPWE GPWE GPWE GPWE GPWE GPWE
   W
          31     30     29     28     27     26     25     24     23     22     21     20     19     18     17     16

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                            0

   W                                         GPWD

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function

     31-16      Global Pin Write Enable
   GPWEn     Configures the corresponding lower 16-bit field of PCRnto be updated with the value in the GPWD field.
                              If a selected PCR is locked, write to that register is ignored.
                     0b - Not updated
                     1b - Updated

      15-0       Global Pin Write Data
   GPWD       Is written to PCRn[15:0] if GPWEn= 1.


75.6.1.5  Configuration (CONFIG)

Offset


 Register                   Offset

 CONFIG               20h


Function
Configures the port voltage range.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3577 / 3791

<!-- page 3578 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                          Reserved

   W                                                            0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                      Reserved                                                                                            RANG
                                                                                                     E   W                                                        0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-1      Reserved
    —

       0         Port Voltage Range
   RANGE     Configures the port voltage range.
                     0b - 1.71 V–3.6 V
                     1b - 2.70 V–3.6 V


75.6.1.6 EFT Detect Flag (EDFR)

Offset


 Register                   Offset

 EDFR                 40h


Function
Specifies whether an EFT event is detected. The EFT detect logic is active in all pin multiplexing modes.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3578 / 3791

<!-- page 3579 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R  EDF31  EDF30  EDF29  EDF28  EDF27  EDF26  EDF25  EDF24  EDF23  EDF22  EDF21  EDF20  EDF19  EDF18  EDF17  EDF16

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R  EDF15  EDF14  EDF13  EDF12  EDF11  EDF10  EDF9  EDF8  EDF7  EDF6  EDF5  EDF4  EDF3  EDF2  EDF1  EDF0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function

      31      EFT Detect Flag
    EDF31      Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                        EDFR                 —

                PORT2                —                           EDFR

                PORT3                —                           EDFR

                PORT4                —                           EDFR

                PORT5                —                           EDFR

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

      30      EFT Detect Flag
    EDF30      Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3579 / 3791

<!-- page 3580 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                        EDFR                 —

                PORT2                —                           EDFR

                PORT3                —                           EDFR

                PORT4                —                           EDFR

                PORT5                —                           EDFR

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

      29      EFT Detect Flag
    EDF29      Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                —                           EDFR

                PORT2                —                           EDFR

                PORT3                —                           EDFR

                PORT4                —                           EDFR

                PORT5                —                           EDFR

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

      28      EFT Detect Flag
    EDF28      Indicates whether high or low EFT is detected on the corresponding pin.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3580 / 3791

<!-- page 3581 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                —                           EDFR

                PORT2                —                           EDFR

                PORT3                —                           EDFR

                PORT4                —                           EDFR

                PORT5                —                           EDFR

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

      27      EFT Detect Flag
    EDF27      Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                —                           EDFR

                PORT2                —                           EDFR

                PORT3                —                           EDFR

                PORT4                —                           EDFR

                PORT5                —                           EDFR

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3581 / 3791

<!-- page 3582 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

      26      EFT Detect Flag
    EDF26      Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                —                           EDFR

                PORT2                —                           EDFR

                PORT3                —                           EDFR

                PORT4                —                           EDFR

                PORT5                —                           EDFR

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

      25      EFT Detect Flag
    EDF25      Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                —                           EDFR

                PORT2                —                           EDFR

                PORT3                —                           EDFR

                PORT4                —                           EDFR

                PORT5                —                           EDFR


                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3582 / 3791

<!-- page 3583 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

      24      EFT Detect Flag
    EDF24      Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                —                           EDFR

                PORT2                —                           EDFR

                PORT3                —                           EDFR

                PORT4                —                           EDFR

                PORT5                —                           EDFR

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

      23      EFT Detect Flag
    EDF23      Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                        EDFR                 —

                PORT2                —                           EDFR

                PORT3                        EDFR                 —

                PORT4                        EDFR                 —


                                  Tablecontinuesonthenextpage...

                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3583 / 3791

<!-- page 3584 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                              Instance                       Field supported in                Field not supported in

                PORT5                —                           EDFR

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

      22      EFT Detect Flag
    EDF22      Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                        EDFR                 —

                PORT2                —                           EDFR

                PORT3                        EDFR                 —

                PORT4                        EDFR                 —

                PORT5                —                           EDFR

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

      21      EFT Detect Flag
    EDF21      Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                        EDFR                 —


                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3584 / 3791

<!-- page 3585 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                              Instance                       Field supported in                Field not supported in

                PORT2                —                           EDFR

                PORT3                        EDFR                 —

                PORT4                        EDFR                 —

                PORT5                —                           EDFR

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

      20      EFT Detect Flag
    EDF20      Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                        EDFR                 —

                PORT2                —                           EDFR

                PORT3                        EDFR                 —

                PORT4                        EDFR                 —

                PORT5                —                           EDFR

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

      19      EFT Detect Flag
    EDF19      Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3585 / 3791

<!-- page 3586 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                        EDFR                 —

                PORT2                —                           EDFR

                PORT3                        EDFR                 —

                PORT4                        EDFR                 —

                PORT5                —                           EDFR

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

      18      EFT Detect Flag
    EDF18      Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                        EDFR                 —

                PORT2                —                           EDFR

                PORT3                        EDFR                 —

                PORT4                        EDFR                 —

                PORT5                —                           EDFR

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

      17      EFT Detect Flag
    EDF17      Indicates whether high or low EFT is detected on the corresponding pin.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3586 / 3791

<!-- page 3587 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                        EDFR                 —

                PORT2                —                           EDFR

                PORT3                        EDFR                 —

                PORT4                        EDFR                 —

                PORT5                —                           EDFR

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

      16      EFT Detect Flag
    EDF16      Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                        EDFR                 —

                PORT2                —                           EDFR

                PORT3                        EDFR                 —

                PORT4                        EDFR                 —

                PORT5                —                           EDFR

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3587 / 3791

<!-- page 3588 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

      15      EFT Detect Flag
    EDF15      Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                        EDFR                 —

                PORT2                —                           EDFR

                PORT3                        EDFR                 —

                PORT4                        EDFR                 —

                PORT5                —                           EDFR

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

      14      EFT Detect Flag
    EDF14      Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                        EDFR                 —

                PORT2                —                           EDFR

                PORT3                        EDFR                 —

                PORT4                        EDFR                 —

                PORT5                —                           EDFR


                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3588 / 3791

<!-- page 3589 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

      13      EFT Detect Flag
    EDF13      Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                        EDFR                 —

                PORT2                —                           EDFR

                PORT3                        EDFR                 —

                PORT4                        EDFR                 —

                PORT5                —                           EDFR

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

      12      EFT Detect Flag
    EDF12      Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                        EDFR                 —

                PORT2                —                           EDFR

                PORT3                        EDFR                 —

                PORT4                        EDFR                 —


                                  Tablecontinuesonthenextpage...

                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3589 / 3791

<!-- page 3590 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                              Instance                       Field supported in                Field not supported in

                PORT5                —                           EDFR

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

      11      EFT Detect Flag
    EDF11      Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                        EDFR                 —

                PORT2                        EDFR                 —

                PORT3                        EDFR                 —

                PORT4                —                           EDFR

                PORT5                —                           EDFR

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

      10      EFT Detect Flag
    EDF10      Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                        EDFR                 —


                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3590 / 3791

<!-- page 3591 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                              Instance                       Field supported in                Field not supported in

                PORT2                        EDFR                 —

                PORT3                        EDFR                 —

                PORT4                —                           EDFR

                PORT5                —                           EDFR

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

       9      EFT Detect Flag
    EDF9       Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                        EDFR                 —

                PORT2                        EDFR                 —

                PORT3                        EDFR                 —

                PORT4                —                           EDFR

                PORT5                        EDFR                 —

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

       8      EFT Detect Flag
    EDF8       Indicates whether high or low EFT is detected on the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3591 / 3791

<!-- page 3592 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                              Instance                       Field supported in                Field not supported in

                PORT0                        EDFR                 —

                PORT1                        EDFR                 —

                PORT2                        EDFR                 —

                PORT3                        EDFR                 —

                PORT4                —                           EDFR

                PORT5                        EDFR                 —

                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

       7      EFT Detect Flag
    EDF7       Indicates whether high or low EFT is detected on the corresponding pin.
                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

       6      EFT Detect Flag
    EDF6       Indicates whether high or low EFT is detected on the corresponding pin.
                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

       5      EFT Detect Flag
    EDF5       Indicates whether high or low EFT is detected on the corresponding pin.
                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

       4      EFT Detect Flag
    EDF4       Indicates whether high or low EFT is detected on the corresponding pin.
                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

       3      EFT Detect Flag
    EDF3       Indicates whether high or low EFT is detected on the corresponding pin.
                     0b - No EFT event detected

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3592 / 3791

<!-- page 3593 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1b - High or/and low EFT event detected

       2      EFT Detect Flag
    EDF2       Indicates whether high or low EFT is detected on the corresponding pin.
                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

       1      EFT Detect Flag
    EDF1       Indicates whether high or low EFT is detected on the corresponding pin.
                     0b - No EFT event detected
                     1b - High or/and low EFT event detected

       0      EFT Detect Flag
    EDF0       Indicates whether high or low EFT is detected on the corresponding pin.
                     0b - No EFT event detected
                     1b - High or/and low EFT event detected


75.6.1.7 EFT Detect Interrupt Enable (EDIER)

Offset


 Register                   Offset

 EDIER                44h


Function
Configures whether to generate an interrupt when an EFT event is detected.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R  EDIE3  EDIE3  EDIE2  EDIE2  EDIE2  EDIE2  EDIE2  EDIE2  EDIE2  EDIE2  EDIE2  EDIE2  EDIE1  EDIE1  EDIE1  EDIE1
   W    1      0      9      8      7      6      5      4      3      2      1      0      9      8      7      6

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R  EDIE1  EDIE1  EDIE1  EDIE1  EDIE1  EDIE1
                                                   EDIE9  EDIE8  EDIE7  EDIE6  EDIE5  EDIE4  EDIE3  EDIE2  EDIE1  EDIE0
   W    5      4      3      2      1      0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3593 / 3791

<!-- page 3594 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

Fields


       Field       Function

      31      EFT Detect Interrupt Enable
    EDIE31     Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                           EDIER                 —

                PORT2                —                              EDIER

                PORT3                —                              EDIER

                PORT4                —                              EDIER

                PORT5                —                              EDIER

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

      30      EFT Detect Interrupt Enable
    EDIE30     Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                           EDIER                 —

                PORT2                —                              EDIER

                PORT3                —                              EDIER

                PORT4                —                              EDIER

                PORT5                —                              EDIER

                     0b - Interrupt not generated upon detection of the EFT event



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3594 / 3791

<!-- page 3595 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1b - Interrupt generated upon detection of the EFT event

      29      EFT Detect Interrupt Enable
    EDIE29     Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                —                              EDIER

                PORT2                —                              EDIER

                PORT3                —                              EDIER

                PORT4                —                              EDIER

                PORT5                —                              EDIER

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

      28      EFT Detect Interrupt Enable
    EDIE28     Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                —                              EDIER

                PORT2                —                              EDIER

                PORT3                —                              EDIER

                PORT4                —                              EDIER





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3595 / 3791

<!-- page 3596 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                              Instance                       Field supported in                Field not supported in

                PORT5                —                              EDIER

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

      27      EFT Detect Interrupt Enable
    EDIE27     Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                —                              EDIER

                PORT2                —                              EDIER

                PORT3                —                              EDIER

                PORT4                —                              EDIER

                PORT5                —                              EDIER

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

      26      EFT Detect Interrupt Enable
    EDIE26     Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                —                              EDIER


                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3596 / 3791

<!-- page 3597 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                              Instance                       Field supported in                Field not supported in

                PORT2                —                              EDIER

                PORT3                —                              EDIER

                PORT4                —                              EDIER

                PORT5                —                              EDIER

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

      25      EFT Detect Interrupt Enable
    EDIE25     Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                —                              EDIER

                PORT2                —                              EDIER

                PORT3                —                              EDIER

                PORT4                —                              EDIER

                PORT5                —                              EDIER

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

      24      EFT Detect Interrupt Enable
    EDIE24     Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3597 / 3791

<!-- page 3598 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                —                              EDIER

                PORT2                —                              EDIER

                PORT3                —                              EDIER

                PORT4                —                              EDIER

                PORT5                —                              EDIER

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

      23      EFT Detect Interrupt Enable
    EDIE23     Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                           EDIER                 —

                PORT2                —                              EDIER

                PORT3                           EDIER                 —

                PORT4                           EDIER                 —

                PORT5                —                              EDIER

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

      22      EFT Detect Interrupt Enable
    EDIE22     Configures the EFT detect interrupt for the corresponding pin.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3598 / 3791

<!-- page 3599 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                           EDIER                 —

                PORT2                —                              EDIER

                PORT3                           EDIER                 —

                PORT4                           EDIER                 —

                PORT5                —                              EDIER

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

      21      EFT Detect Interrupt Enable
    EDIE21     Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                           EDIER                 —

                PORT2                —                              EDIER

                PORT3                           EDIER                 —

                PORT4                           EDIER                 —

                PORT5                —                              EDIER

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3599 / 3791

<!-- page 3600 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

      20      EFT Detect Interrupt Enable
    EDIE20     Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                           EDIER                 —

                PORT2                —                              EDIER

                PORT3                           EDIER                 —

                PORT4                           EDIER                 —

                PORT5                —                              EDIER

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

      19      EFT Detect Interrupt Enable
    EDIE19     Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                           EDIER                 —

                PORT2                —                              EDIER

                PORT3                           EDIER                 —

                PORT4                           EDIER                 —

                PORT5                —                              EDIER


                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3600 / 3791

<!-- page 3601 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

      18      EFT Detect Interrupt Enable
    EDIE18     Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                           EDIER                 —

                PORT2                —                              EDIER

                PORT3                           EDIER                 —

                PORT4                           EDIER                 —

                PORT5                —                              EDIER

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

      17      EFT Detect Interrupt Enable
    EDIE17     Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                           EDIER                 —

                PORT2                —                              EDIER

                PORT3                           EDIER                 —

                PORT4                           EDIER                 —


                                  Tablecontinuesonthenextpage...

                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3601 / 3791

<!-- page 3602 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                              Instance                       Field supported in                Field not supported in

                PORT5                —                              EDIER

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

      16      EFT Detect Interrupt Enable
    EDIE16     Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                           EDIER                 —

                PORT2                —                              EDIER

                PORT3                           EDIER                 —

                PORT4                           EDIER                 —

                PORT5                —                              EDIER

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

      15      EFT Detect Interrupt Enable
    EDIE15     Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                           EDIER                 —


                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3602 / 3791

<!-- page 3603 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                              Instance                       Field supported in                Field not supported in

                PORT2                —                              EDIER

                PORT3                           EDIER                 —

                PORT4                           EDIER                 —

                PORT5                —                              EDIER

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

      14      EFT Detect Interrupt Enable
    EDIE14     Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                           EDIER                 —

                PORT2                —                              EDIER

                PORT3                           EDIER                 —

                PORT4                           EDIER                 —

                PORT5                —                              EDIER

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

      13      EFT Detect Interrupt Enable
    EDIE13     Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3603 / 3791

<!-- page 3604 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                           EDIER                 —

                PORT2                —                              EDIER

                PORT3                           EDIER                 —

                PORT4                           EDIER                 —

                PORT5                —                              EDIER

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

      12      EFT Detect Interrupt Enable
    EDIE12     Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                           EDIER                 —

                PORT2                —                              EDIER

                PORT3                           EDIER                 —

                PORT4                           EDIER                 —

                PORT5                —                              EDIER

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

      11      EFT Detect Interrupt Enable
    EDIE11     Configures the EFT detect interrupt for the corresponding pin.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3604 / 3791

<!-- page 3605 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                           EDIER                 —

                PORT2                           EDIER                 —

                PORT3                           EDIER                 —

                PORT4                —                              EDIER

                PORT5                —                              EDIER

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

      10      EFT Detect Interrupt Enable
    EDIE10     Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                           EDIER                 —

                PORT2                           EDIER                 —

                PORT3                           EDIER                 —

                PORT4                —                              EDIER

                PORT5                —                              EDIER

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3605 / 3791

<!-- page 3606 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

       9      EFT Detect Interrupt Enable
    EDIE9      Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                           EDIER                 —

                PORT2                           EDIER                 —

                PORT3                           EDIER                 —

                PORT4                —                              EDIER

                PORT5                           EDIER                 —

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

       8      EFT Detect Interrupt Enable
    EDIE8      Configures the EFT detect interrupt for the corresponding pin.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                           EDIER                 —

                PORT1                           EDIER                 —

                PORT2                           EDIER                 —

                PORT3                           EDIER                 —

                PORT4                —                              EDIER

                PORT5                           EDIER                 —


                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3606 / 3791

<!-- page 3607 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

       7      EFT Detect Interrupt Enable
    EDIE7      Configures the EFT detect interrupt for the corresponding pin.
                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

       6      EFT Detect Interrupt Enable
    EDIE6      Configures the EFT detect interrupt for the corresponding pin.
                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

       5      EFT Detect Interrupt Enable
    EDIE5      Configures the EFT detect interrupt for the corresponding pin.
                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

       4      EFT Detect Interrupt Enable
    EDIE4      Configures the EFT detect interrupt for the corresponding pin.
                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

       3      EFT Detect Interrupt Enable
    EDIE3      Configures the EFT detect interrupt for the corresponding pin.
                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

       2      EFT Detect Interrupt Enable
    EDIE2      Configures the EFT detect interrupt for the corresponding pin.
                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

       1      EFT Detect Interrupt Enable
    EDIE1      Configures the EFT detect interrupt for the corresponding pin.
                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event

       0      EFT Detect Interrupt Enable

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3607 / 3791

<!-- page 3608 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
    EDIE0      Configures the EFT detect interrupt for the corresponding pin.
                     0b - Interrupt not generated upon detection of the EFT event
                     1b - Interrupt generated upon detection of the EFT event


75.6.1.8 EFT Detect Clear (EDCR)

Offset


 Register                   Offset

 EDCR                48h


Function
Clears EFT detectors. The EFT detect logic is cleared per port.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1       0

    R                                                    0
                                                                                             EDLC  EDHC
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0       0


Fields


       Field       Function
      31-2      Reserved
    —

       1      EFT Detect Low Clear
    EDLC      Clears low EFT detectors. If this field = 1, all low EFT detectors for which the corresponding high EFT
                  detectors are not asserted are cleared.
                     0b - Does not clear
                     1b - Clears

       0      EFT Detect High Clear

                                  Tablecontinuesonthenextpage...

                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3608 / 3791

<!-- page 3609 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
    EDHC      Clears high EFT detectors.
                     0b - Does not clear
                     1b - Clears


75.6.1.9  Calibration 0 (CALIB0)

Offset


 Register                   Offset

 CALIB0               60h


Function
Stores calibration values for the PMOS and NMOS output drivers when PCRn[DSE] = 0.

                                           NOTE
                               Each module instance supports a different number of registers.


                Instance                           Register supported                    Register not supported

 PORT0                                   CALIB0                    —

 PORT1                                   CALIB0                    —

 PORT2                                   CALIB0                    —

 PORT3                                   CALIB0                    —

 PORT4                    —                                       CALIB0

 PORT5                    —                                       CALIB0


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                     0
                                                                                      PCAL
   W

Reset     0      0      0      0       0      0      0      0       0      0       u1      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                     0
                                                                                    NCAL
   W

Reset     0      0      0      0       0      0      0      0       0      0       u1      u       u      u      u      u



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3609 / 3791

<!-- page 3610 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

1. Reset values are loaded out of IFR.

Fields


       Field       Function
     31-22      Reserved
    —

     21-16       Calibration of PMOS Output Driver
    PCAL

      15-6      Reserved
    —

       5-0        Calibration of NMOS Output Driver
    NCAL


75.6.1.10  Calibration 1 (CALIB1)

Offset


 Register                   Offset

 CALIB1               64h


Function
Stores calibration values for the PMOS and NMOS output drivers when PCRn[DSE] = 1.

                                           NOTE
                               Each module instance supports a different number of registers.


                Instance                           Register supported                    Register not supported

 PORT0                                   CALIB1                    —

 PORT1                                   CALIB1                    —

 PORT2                                   CALIB1                    —

 PORT3                                   CALIB1                    —

 PORT4                    —                                       CALIB1

 PORT5                    —                                       CALIB1





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3610 / 3791

<!-- page 3611 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                     0
                                                                                      PCAL
   W

Reset     0      0      0      0       0      0      0      0       0      0       u1      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                     0
                                                                                    NCAL
   W

Reset     0      0      0      0       0      0      0      0       0      0       u1      u       u      u      u      u

1. Reset values are loaded out of IFR.

Fields


       Field       Function
     31-22      Reserved
    —

     21-16       Calibration of PMOS Output Driver
    PCAL

      15-6      Reserved
    —

       5-0        Calibration of NMOS Output Driver
    NCAL


75.6.1.11  Pin Control 0 (PCR0)

Offset


 Register                   Offset

 PCR0                 80h


Function
Configures pin control features on each pin.

                                           NOTE
           Do not modify pin configuration registers associated with pins that are unavailable in your selected package. All
            unbonded pins unavailable in your package default to the Disabled state for lowest power consumption.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3611 / 3791

<!-- page 3612 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset                                                See Register reset values.

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R            0                                                  0
         LK            INV     IBE           MUX                  DSE   ODE   PFE   SRE    PV    PE    PS
   W

Reset                                               See Register reset values.


Register reset values


 Register                                              Reset value

 PCR0                                                    PORT0: 0000_1143h
                                                  PORT1–PORT5: 0000_0000h


Fields


       Field       Function
     31-16      Reserved
    —

      15       Lock Register
      LK       Locks this PCR.
             When a PCRnis locked, its fields cannot be updated until the next reset.
                     0b - Does not lock
                     1b - Locks

      14       Reserved
    —

      13         Invert Input
      INV        Inverts the digital input.
                     0b - Does not invert
                     1b - Inverts

      12         Input Buffer Enable
      IBE       Enables digital input buffer. When disabled, the digital input is required for analog functions.
                     0b - Disables

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3612 / 3791

<!-- page 3613 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1b - Enables

      11-8       Pin Multiplex Control
    MUX       Configures the multiplexing slots on each pin.
                Not all pins support all pin multiplexing slots. Unimplemented pin multiplexing slots are reserved.
                Unimplemented pin multiplexing slots can result in different behaviors, if the unimplemented slot is
               phantomed (because the module is phantomed on that die) versus unimplemented on the die.
               The corresponding pin is configured according to the following pin multiplexing slots:

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        PCR0                 —

                PORT1                        PCR0                 —

                PORT2                        PCR0                 —

                PORT3                        PCR0                 —

                PORT4                        PCR0                 —

                PORT5                              PCR0[9–8]                           PCR0[11–10]


                                                 NOTE
                                      The descriptions of the field settings vary by module instance.


                         Instance                                    Field value and description

                PORT0                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3613 / 3791

<!-- page 3614 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


       Field       Function


                         Instance                                    Field value and description

                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT1                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT2                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3614 / 3791

<!-- page 3615 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


       Field       Function


                         Instance                                    Field value and description

                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT3                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT4                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3615 / 3791

<!-- page 3616 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                         Instance                                    Field value and description

                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT5                      00b - Alternative 0 (GPIO)
                                          01b - Alternative 1 (chip-specific)
                                          10b - Alternative 2 (chip-specific)
                                          11b - Alternative 3 (chip-specific)

       7        Reserved
    —

       6         Drive Strength Enable
    DSE       Configures drive strength, low or high, on each pin.
               The drive strength configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, low drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.
                           • When this field = 1, high drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        PCR0                 —

                PORT1                        PCR0                 —

                PORT2                        PCR0                 —

                PORT3                        PCR0                 —

                PORT4                        PCR0                 —

                PORT5                —                            PCR0

                     0b - Low
                     1b - High

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3616 / 3791

<!-- page 3617 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

       5       Open Drain Enable
    ODE      Enables open drain output on each pin.
               The open drain configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the open drain output is disabled on the corresponding pin.
                           • When this field = 1, the open drain output is enabled on the corresponding pin, if the pin is
                      configured as a digital output.
                     0b - Disables
                     1b - Enables

       4        Passive Filter Enable
     PFE       Enables passive input filter on each pin.
               The passive filter configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the passive input filter is disabled on the corresponding pin.
                           • When this field = 1, the passive input filter is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
              See the chip's data sheet for filter characteristics.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                —                            PCR0

                PORT1                        PCR0                 —

                PORT2                —                            PCR0

                PORT3                —                            PCR0

                PORT4                —                            PCR0

                PORT5                        PCR0                 —

                     0b - Disables
                     1b - Enables

       3        Slew Rate Enable
    SRE       Configures the slew rate feature, fast or slow, on each corresponding pin.
               The slew rate configuration is valid for all digital pin multiplexing modes.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3617 / 3791

<!-- page 3618 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        PCR0                 —

                PORT1                        PCR0                 —

                PORT2                        PCR0                 —

                PORT3                        PCR0                 —

                PORT4                        PCR0                 —

                PORT5                —                            PCR0

                     0b - Fast
                     1b - Slow

       2          Pull Value
     PV        Selects high or low internal pull resistor value.
               The pull value configuration is valid for all digital pin multiplexing modes.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                —                            PCR0

                PORT1                —                            PCR0

                PORT2                —                            PCR0

                PORT3                —                            PCR0

                PORT4                —                            PCR0

                PORT5                        PCR0                 —

                     0b - Low

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3618 / 3791

<!-- page 3619 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1b - High

       1          Pull Enable
     PE       Enables the internal pull resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pull resistor is not enabled on the corresponding pin.
                           • When this field = 1, the internal pull resistor is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
                     0b - Disables
                     1b - Enables

       0          Pull Select
     PS       Enables the internal pullup or pulldown resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pulldown resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                           • When this field = 1, the internal pullup resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                     0b - Enables internal pulldown resistor
                     1b - Enables internal pullup resistor


75.6.1.12  Pin Control 1 (PCR1)

Offset


 Register                   Offset

 PCR1                 84h


Function
Configures pin control features on each pin.

                                           NOTE
           Do not modify pin configuration registers associated with pins that are unavailable in your selected package. All
            unbonded pins unavailable in your package default to the Disabled state for lowest power consumption.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3619 / 3791

<!-- page 3620 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset                                                See Register reset values.

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R            0                                                  0
         LK            INV     IBE           MUX                  DSE   ODE   PFE   SRE    PV    PE    PS
   W

Reset                                               See Register reset values.


Register reset values


 Register                                              Reset value

 PCR1                                                    PORT0: 0000_1102h
                                                  PORT1–PORT5: 0000_0000h


Fields


       Field       Function
     31-16      Reserved
    —

      15       Lock Register
      LK       Locks this PCR.
             When a PCRnis locked, its fields cannot be updated until the next reset.
                     0b - Does not lock
                     1b - Locks

      14       Reserved
    —

      13         Invert Input
      INV        Inverts the digital input.
                     0b - Does not invert
                     1b - Inverts

      12         Input Buffer Enable
      IBE       Enables digital input buffer. When disabled, the digital input is required for analog functions.
                     0b - Disables

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3620 / 3791

<!-- page 3621 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1b - Enables

      11-8       Pin Multiplex Control
    MUX       Configures the multiplexing slots on each pin.
                Not all pins support all pin multiplexing slots. Unimplemented pin multiplexing slots are reserved.
                Unimplemented pin multiplexing slots can result in different behaviors, if the unimplemented slot is
               phantomed (because the module is phantomed on that die) versus unimplemented on the die.
               The corresponding pin is configured according to the following pin multiplexing slots:

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        PCR1                 —

                PORT1                        PCR1                 —

                PORT2                        PCR1                 —

                PORT3                        PCR1                 —

                PORT4                        PCR1                 —

                PORT5                              PCR1[9–8]                           PCR1[11–10]


                                                 NOTE
                                      The descriptions of the field settings vary by module instance.


                         Instance                                    Field value and description

                PORT0                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3621 / 3791

<!-- page 3622 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


       Field       Function


                         Instance                                    Field value and description

                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT1                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT2                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3622 / 3791

<!-- page 3623 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


       Field       Function


                         Instance                                    Field value and description

                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT3                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT4                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3623 / 3791

<!-- page 3624 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                         Instance                                    Field value and description

                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT5                      00b - Alternative 0 (GPIO)
                                          01b - Alternative 1 (chip-specific)
                                          10b - Alternative 2 (chip-specific)
                                          11b - Alternative 3 (chip-specific)

       7        Reserved
    —

       6         Drive Strength Enable
    DSE       Configures drive strength, low or high, on each pin.
               The drive strength configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, low drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.
                           • When this field = 1, high drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        PCR1                 —

                PORT1                        PCR1                 —

                PORT2                        PCR1                 —

                PORT3                        PCR1                 —

                PORT4                        PCR1                 —

                PORT5                —                            PCR1

                     0b - Low
                     1b - High

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3624 / 3791

<!-- page 3625 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

       5       Open Drain Enable
    ODE      Enables open drain output on each pin.
               The open drain configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the open drain output is disabled on the corresponding pin.
                           • When this field = 1, the open drain output is enabled on the corresponding pin, if the pin is
                      configured as a digital output.
                     0b - Disables
                     1b - Enables

       4        Passive Filter Enable
     PFE       Enables passive input filter on each pin.
               The passive filter configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the passive input filter is disabled on the corresponding pin.
                           • When this field = 1, the passive input filter is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
              See the chip's data sheet for filter characteristics.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                —                            PCR1

                PORT1                        PCR1                 —

                PORT2                —                            PCR1

                PORT3                —                            PCR1

                PORT4                —                            PCR1

                PORT5                        PCR1                 —

                     0b - Disables
                     1b - Enables

       3        Slew Rate Enable
    SRE       Configures the slew rate feature, fast or slow, on each corresponding pin.
               The slew rate configuration is valid for all digital pin multiplexing modes.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3625 / 3791

<!-- page 3626 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        PCR1                 —

                PORT1                        PCR1                 —

                PORT2                        PCR1                 —

                PORT3                        PCR1                 —

                PORT4                        PCR1                 —

                PORT5                —                            PCR1

                     0b - Fast
                     1b - Slow

       2          Pull Value
     PV        Selects high or low internal pull resistor value.
               The pull value configuration is valid for all digital pin multiplexing modes.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                —                            PCR1

                PORT1                —                            PCR1

                PORT2                —                            PCR1

                PORT3                —                            PCR1

                PORT4                —                            PCR1

                PORT5                        PCR1                 —

                     0b - Low

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3626 / 3791

<!-- page 3627 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1b - High

       1          Pull Enable
     PE       Enables the internal pull resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pull resistor is not enabled on the corresponding pin.
                           • When this field = 1, the internal pull resistor is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
                     0b - Disables
                     1b - Enables

       0          Pull Select
     PS       Enables the internal pullup or pulldown resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pulldown resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                           • When this field = 1, the internal pullup resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                     0b - Enables internal pulldown resistor
                     1b - Enables internal pullup resistor


75.6.1.13  Pin Control 2 (PCR2)

Offset


 Register                   Offset

 PCR2                 88h


Function
Configures pin control features on each pin.

                                           NOTE
           Do not modify pin configuration registers associated with pins that are unavailable in your selected package. All
            unbonded pins unavailable in your package default to the Disabled state for lowest power consumption.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3627 / 3791

<!-- page 3628 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset                                                See Register reset values.

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R            0                                                  0
         LK            INV     IBE           MUX                  DSE   ODE   PFE   SRE    PV    PE    PS
   W

Reset                                               See Register reset values.


Register reset values


 Register                                              Reset value

 PCR2                                                    PORT0: 0000_0140h
                                                  PORT1–PORT4: 0000_0000h
                                                    PORT5: 0000_0100h


Fields


       Field       Function
     31-16      Reserved
    —

      15       Lock Register
      LK       Locks this PCR.
             When a PCRnis locked, its fields cannot be updated until the next reset.
                     0b - Does not lock
                     1b - Locks

      14       Reserved
    —

      13         Invert Input
      INV        Inverts the digital input.
                     0b - Does not invert
                     1b - Inverts

      12         Input Buffer Enable
      IBE       Enables digital input buffer. When disabled, the digital input is required for analog functions.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3628 / 3791

<!-- page 3629 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Disables
                     1b - Enables

      11-8       Pin Multiplex Control
    MUX       Configures the multiplexing slots on each pin.
                Not all pins support all pin multiplexing slots. Unimplemented pin multiplexing slots are reserved.
                Unimplemented pin multiplexing slots can result in different behaviors, if the unimplemented slot is
               phantomed (because the module is phantomed on that die) versus unimplemented on the die.
               The corresponding pin is configured according to the following pin multiplexing slots:

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        PCR2                 —

                PORT1                        PCR2                 —

                PORT2                        PCR2                 —

                PORT3                        PCR2                 —

                PORT4                        PCR2                 —

                PORT5                              PCR2[9–8]                           PCR2[11–10]


                                                 NOTE
                                      The descriptions of the field settings vary by module instance.


                         Instance                                    Field value and description

                PORT0                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3629 / 3791

<!-- page 3630 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


       Field       Function


                         Instance                                    Field value and description

                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT1                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT2                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3630 / 3791

<!-- page 3631 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


       Field       Function


                         Instance                                    Field value and description

                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT3                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT4                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3631 / 3791

<!-- page 3632 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                         Instance                                    Field value and description

                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT5                      00b - Alternative 0 (GPIO)
                                          01b - Alternative 1 (chip-specific)
                                          10b - Alternative 2 (chip-specific)
                                          11b - Alternative 3 (chip-specific)

       7        Reserved
    —

       6         Drive Strength Enable
    DSE       Configures drive strength, low or high, on each pin.
               The drive strength configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, low drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.
                           • When this field = 1, high drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        PCR2                 —

                PORT1                        PCR2                 —

                PORT2                        PCR2                 —

                PORT3                        PCR2                 —

                PORT4                        PCR2                 —

                PORT5                —                            PCR2


                                  Tablecontinuesonthenextpage...




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3632 / 3791

<!-- page 3633 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Low
                     1b - High

       5       Open Drain Enable
    ODE      Enables open drain output on each pin.
               The open drain configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the open drain output is disabled on the corresponding pin.
                           • When this field = 1, the open drain output is enabled on the corresponding pin, if the pin is
                      configured as a digital output.
                     0b - Disables
                     1b - Enables

       4        Passive Filter Enable
     PFE       Enables passive input filter on each pin.
               The passive filter configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the passive input filter is disabled on the corresponding pin.
                           • When this field = 1, the passive input filter is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
              See the chip's data sheet for filter characteristics.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                —                            PCR2

                PORT1                —                            PCR2

                PORT2                —                            PCR2

                PORT3                —                            PCR2

                PORT4                —                            PCR2

                PORT5                        PCR2                 —

                     0b - Disables
                     1b - Enables

       3        Slew Rate Enable

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3633 / 3791

<!-- page 3634 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

    SRE       Configures the slew rate feature, fast or slow, on each corresponding pin.
               The slew rate configuration is valid for all digital pin multiplexing modes.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        PCR2                 —

                PORT1                        PCR2                 —

                PORT2                        PCR2                 —

                PORT3                        PCR2                 —

                PORT4                        PCR2                 —

                PORT5                —                            PCR2

                     0b - Fast
                     1b - Slow

       2          Pull Value
     PV        Selects high or low internal pull resistor value.
               The pull value configuration is valid for all digital pin multiplexing modes.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                —                            PCR2

                PORT1                —                            PCR2

                PORT2                —                            PCR2

                PORT3                —                            PCR2

                PORT4                —                            PCR2

                PORT5                        PCR2                 —


                                  Tablecontinuesonthenextpage...

                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3634 / 3791

<!-- page 3635 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                              Instance                       Field supported in                Field not supported in

                     0b - Low
                     1b - High

       1          Pull Enable
     PE       Enables the internal pull resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pull resistor is not enabled on the corresponding pin.
                           • When this field = 1, the internal pull resistor is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
                     0b - Disables
                     1b - Enables

       0          Pull Select
     PS       Enables the internal pullup or pulldown resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pulldown resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                           • When this field = 1, the internal pullup resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                     0b - Enables internal pulldown resistor
                     1b - Enables internal pullup resistor


75.6.1.14  Pin Control 3 (PCR3)

Offset


 Register                   Offset

 PCR3               8Ch


Function
Configures pin control features on each pin.

                                           NOTE
           Do not modify pin configuration registers associated with pins that are unavailable in your selected package. All
            unbonded pins unavailable in your package default to the Disabled state for lowest power consumption.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3635 / 3791

<!-- page 3636 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset                                                See Register reset values.

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R            0                                                  0
         LK            INV     IBE           MUX                  DSE   ODE   PFE   SRE    PV    PE    PS
   W

Reset                                               See Register reset values.


Register reset values


 Register                                              Reset value

 PCR3                                                    PORT0: 0000_1103h
                                                  PORT1–PORT5: 0000_0000h


Fields


       Field       Function
     31-16      Reserved
    —

      15       Lock Register
      LK       Locks this PCR.
             When a PCRnis locked, its fields cannot be updated until the next reset.
                     0b - Does not lock
                     1b - Locks

      14       Reserved
    —

      13         Invert Input
      INV        Inverts the digital input.
                     0b - Does not invert
                     1b - Inverts

      12         Input Buffer Enable
      IBE       Enables digital input buffer. When disabled, the digital input is required for analog functions.
                     0b - Disables

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3636 / 3791

<!-- page 3637 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1b - Enables

      11-8       Pin Multiplex Control
    MUX       Configures the multiplexing slots on each pin.
                Not all pins support all pin multiplexing slots. Unimplemented pin multiplexing slots are reserved.
                Unimplemented pin multiplexing slots can result in different behaviors, if the unimplemented slot is
               phantomed (because the module is phantomed on that die) versus unimplemented on the die.
               The corresponding pin is configured according to the following pin multiplexing slots:

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        PCR3                 —

                PORT1                        PCR3                 —

                PORT2                        PCR3                 —

                PORT3                        PCR3                 —

                PORT4                        PCR3                 —

                PORT5                              PCR3[9–8]                           PCR3[11–10]


                                                 NOTE
                                      The descriptions of the field settings vary by module instance.


                         Instance                                    Field value and description

                PORT0                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3637 / 3791

<!-- page 3638 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


       Field       Function


                         Instance                                    Field value and description

                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT1                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT2                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3638 / 3791

<!-- page 3639 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


       Field       Function


                         Instance                                    Field value and description

                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT3                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT4                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3639 / 3791

<!-- page 3640 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                         Instance                                    Field value and description

                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT5                      00b - Alternative 0 (GPIO)
                                          01b - Alternative 1 (chip-specific)
                                          10b - Alternative 2 (chip-specific)
                                          11b - Alternative 3 (chip-specific)

       7        Reserved
    —

       6         Drive Strength Enable
    DSE       Configures drive strength, low or high, on each pin.
               The drive strength configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, low drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.
                           • When this field = 1, high drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        PCR3                 —

                PORT1                        PCR3                 —

                PORT2                        PCR3                 —

                PORT3                        PCR3                 —

                PORT4                        PCR3                 —

                PORT5                —                            PCR3

                     0b - Low
                     1b - High

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3640 / 3791

<!-- page 3641 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

       5       Open Drain Enable
    ODE      Enables open drain output on each pin.
               The open drain configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the open drain output is disabled on the corresponding pin.
                           • When this field = 1, the open drain output is enabled on the corresponding pin, if the pin is
                      configured as a digital output.
                     0b - Disables
                     1b - Enables

       4        Passive Filter Enable
     PFE       Enables passive input filter on each pin.
               The passive filter configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the passive input filter is disabled on the corresponding pin.
                           • When this field = 1, the passive input filter is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
              See the chip's data sheet for filter characteristics.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                —                            PCR3

                PORT1                —                            PCR3

                PORT2                —                            PCR3

                PORT3                —                            PCR3

                PORT4                —                            PCR3

                PORT5                        PCR3                 —

                     0b - Disables
                     1b - Enables

       3        Slew Rate Enable
    SRE       Configures the slew rate feature, fast or slow, on each corresponding pin.
               The slew rate configuration is valid for all digital pin multiplexing modes.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3641 / 3791

<!-- page 3642 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        PCR3                 —

                PORT1                        PCR3                 —

                PORT2                        PCR3                 —

                PORT3                        PCR3                 —

                PORT4                        PCR3                 —

                PORT5                —                            PCR3

                     0b - Fast
                     1b - Slow

       2          Pull Value
     PV        Selects high or low internal pull resistor value.
               The pull value configuration is valid for all digital pin multiplexing modes.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                —                            PCR3

                PORT1                —                            PCR3

                PORT2                —                            PCR3

                PORT3                —                            PCR3

                PORT4                —                            PCR3

                PORT5                        PCR3                 —

                     0b - Low

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3642 / 3791

<!-- page 3643 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1b - High

       1          Pull Enable
     PE       Enables the internal pull resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pull resistor is not enabled on the corresponding pin.
                           • When this field = 1, the internal pull resistor is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
                     0b - Disables
                     1b - Enables

       0          Pull Select
     PS       Enables the internal pullup or pulldown resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pulldown resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                           • When this field = 1, the internal pullup resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                     0b - Enables internal pulldown resistor
                     1b - Enables internal pullup resistor


75.6.1.15  Pin Control a (PCR4 - PCR5)

Offset


 Register                   Offset

 PCR4                 90h

 PCR5                 94h


Function
Configures pin control features on each pin.

                                           NOTE
           Do not modify pin configuration registers associated with pins that are unavailable in your selected package. All
            unbonded pins unavailable in your package default to the Disabled state for lowest power consumption.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3643 / 3791

<!-- page 3644 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R            0                                                  0
         LK            INV     IBE           MUX                  DSE   ODE   PFE   SRE    PV    PE    PS
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-16      Reserved
    —

      15       Lock Register
      LK       Locks this PCR.
             When a PCRnis locked, its fields cannot be updated until the next reset.
                     0b - Does not lock
                     1b - Locks

      14       Reserved
    —

      13         Invert Input
      INV        Inverts the digital input.
                     0b - Does not invert
                     1b - Inverts

      12         Input Buffer Enable
      IBE       Enables digital input buffer. When disabled, the digital input is required for analog functions.
                     0b - Disables
                     1b - Enables

      11-8       Pin Multiplex Control
    MUX       Configures the multiplexing slots on each pin.
                Not all pins support all pin multiplexing slots. Unimplemented pin multiplexing slots are reserved.
                Unimplemented pin multiplexing slots can result in different behaviors, if the unimplemented slot is
               phantomed (because the module is phantomed on that die) versus unimplemented on the die.




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3644 / 3791

<!-- page 3645 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


       Field       Function

               The corresponding pin is configured according to the following pin multiplexing slots:

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                         PCR4–PCR5              —

                PORT1                         PCR4–PCR5              —

                PORT2                         PCR4–PCR5              —

                PORT3                         PCR4–PCR5              —

                PORT4                         PCR4–PCR5              —

                PORT5                            PCR4–PCR5[9–8]                    PCR4–PCR5[11–10]


                                                 NOTE
                                      The descriptions of the field settings vary by module instance.


                         Instance                                    Field value and description

                PORT0                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3645 / 3791

<!-- page 3646 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


       Field       Function


                         Instance                                    Field value and description

                PORT1                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT2                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT3                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3646 / 3791

<!-- page 3647 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                         Instance                                    Field value and description

                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT4                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT5                      00b - Alternative 0 (GPIO)
                                          01b - Alternative 1 (chip-specific)
                                          10b - Alternative 2 (chip-specific)
                                          11b - Alternative 3 (chip-specific)


                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3647 / 3791

<!-- page 3648 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       7        Reserved
    —

       6         Drive Strength Enable
    DSE       Configures drive strength, low or high, on each pin.
               The drive strength configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, low drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.
                           • When this field = 1, high drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                         PCR4–PCR5              —

                PORT1                         PCR4–PCR5              —

                PORT2                         PCR4–PCR5              —

                PORT3                         PCR4–PCR5              —

                PORT4                         PCR4–PCR5              —

                PORT5                —                             PCR4–PCR5

                     0b - Low
                     1b - High

       5       Open Drain Enable
    ODE      Enables open drain output on each pin.
               The open drain configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the open drain output is disabled on the corresponding pin.
                           • When this field = 1, the open drain output is enabled on the corresponding pin, if the pin is
                      configured as a digital output.
                     0b - Disables
                     1b - Enables

       4        Passive Filter Enable

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3648 / 3791

<!-- page 3649 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

     PFE       Enables passive input filter on each pin.
               The passive filter configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the passive input filter is disabled on the corresponding pin.
                           • When this field = 1, the passive input filter is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
              See the chip's data sheet for filter characteristics.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                         PCR4–PCR5              —

                PORT1                —                             PCR4–PCR5

                PORT2                —                             PCR4–PCR5

                PORT3                —                             PCR4–PCR5

                PORT4                —                             PCR4–PCR5

                PORT5                         PCR4–PCR5              —

                     0b - Disables
                     1b - Enables

       3        Slew Rate Enable
    SRE       Configures the slew rate feature, fast or slow, on each corresponding pin.
               The slew rate configuration is valid for all digital pin multiplexing modes.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                         PCR4–PCR5              —

                PORT1                         PCR4–PCR5              —

                PORT2                         PCR4–PCR5              —


                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3649 / 3791

<!-- page 3650 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                              Instance                       Field supported in                Field not supported in

                PORT3                         PCR4–PCR5              —

                PORT4                         PCR4–PCR5              —

                PORT5                —                             PCR4–PCR5

                     0b - Fast
                     1b - Slow

       2          Pull Value
     PV        Selects high or low internal pull resistor value.
               The pull value configuration is valid for all digital pin multiplexing modes.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                —                             PCR4–PCR5

                PORT1                —                             PCR4–PCR5

                PORT2                —                             PCR4–PCR5

                PORT3                —                             PCR4–PCR5

                PORT4                —                             PCR4–PCR5

                PORT5                         PCR4–PCR5              —

                     0b - Low
                     1b - High

       1          Pull Enable
     PE       Enables the internal pull resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pull resistor is not enabled on the corresponding pin.
                           • When this field = 1, the internal pull resistor is enabled on the corresponding pin, if the pin is
                      configured as a digital input.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3650 / 3791

<!-- page 3651 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Disables
                     1b - Enables

       0          Pull Select
     PS       Enables the internal pullup or pulldown resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pulldown resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                           • When this field = 1, the internal pullup resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                     0b - Enables internal pulldown resistor
                     1b - Enables internal pullup resistor


75.6.1.16  Pin Control 6 (PCR6)

Offset


 Register                   Offset

 PCR6                 98h


Function
Configures pin control features on each pin.

                                           NOTE
           Do not modify pin configuration registers associated with pins that are unavailable in your selected package. All
            unbonded pins unavailable in your package default to the Disabled state for lowest power consumption.


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset                                                See Register reset values.

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R            0                                                  0
         LK            INV     IBE           MUX                  DSE   ODE   PFE   SRE    PV    PE    PS
   W

Reset                                               See Register reset values.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3651 / 3791

<!-- page 3652 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

Register reset values


 Register                                              Reset value

 PCR6                                                    PORT0: 0000_1103h
                                                  PORT1–PORT5: 0000_0000h


Fields


       Field       Function
     31-16      Reserved
    —

      15       Lock Register
      LK       Locks this PCR.
             When a PCRnis locked, its fields cannot be updated until the next reset.
                     0b - Does not lock
                     1b - Locks

      14       Reserved
    —

      13         Invert Input
      INV        Inverts the digital input.
                     0b - Does not invert
                     1b - Inverts

      12         Input Buffer Enable
      IBE       Enables digital input buffer. When disabled, the digital input is required for analog functions.
                     0b - Disables
                     1b - Enables

      11-8       Pin Multiplex Control
    MUX       Configures the multiplexing slots on each pin.
                Not all pins support all pin multiplexing slots. Unimplemented pin multiplexing slots are reserved.
                Unimplemented pin multiplexing slots can result in different behaviors, if the unimplemented slot is
               phantomed (because the module is phantomed on that die) versus unimplemented on the die.
               The corresponding pin is configured according to the following pin multiplexing slots:

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3652 / 3791

<!-- page 3653 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


       Field       Function


                              Instance                       Field supported in                Field not supported in

                PORT0                        PCR6                 —

                PORT1                        PCR6                 —

                PORT2                        PCR6                 —

                PORT3                        PCR6                 —

                PORT4                        PCR6                 —

                PORT5                              PCR6[9–8]                           PCR6[11–10]


                                                 NOTE
                                      The descriptions of the field settings vary by module instance.


                         Instance                                    Field value and description

                PORT0                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT1                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3653 / 3791

<!-- page 3654 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


       Field       Function


                         Instance                                    Field value and description

                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT2                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT3                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3654 / 3791

<!-- page 3655 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                         Instance                                    Field value and description

                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT4                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT5                      00b - Alternative 0 (GPIO)
                                          01b - Alternative 1 (chip-specific)
                                          10b - Alternative 2 (chip-specific)
                                          11b - Alternative 3 (chip-specific)

       7        Reserved
    —

       6         Drive Strength Enable
                 Configures drive strength, low or high, on each pin.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3655 / 3791

<!-- page 3656 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

    DSE      The drive strength configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, low drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.
                           • When this field = 1, high drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        PCR6                 —

                PORT1                        PCR6                 —

                PORT2                        PCR6                 —

                PORT3                        PCR6                 —

                PORT4                        PCR6                 —

                PORT5                —                            PCR6

                     0b - Low
                     1b - High

       5       Open Drain Enable
    ODE      Enables open drain output on each pin.
               The open drain configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the open drain output is disabled on the corresponding pin.
                           • When this field = 1, the open drain output is enabled on the corresponding pin, if the pin is
                      configured as a digital output.
                     0b - Disables
                     1b - Enables

       4        Passive Filter Enable
     PFE       Enables passive input filter on each pin.
               The passive filter configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the passive input filter is disabled on the corresponding pin.
                           • When this field = 1, the passive input filter is enabled on the corresponding pin, if the pin is
                      configured as a digital input.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3656 / 3791

<!-- page 3657 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

              See the chip's data sheet for filter characteristics.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                —                            PCR6

                PORT1                —                            PCR6

                PORT2                —                            PCR6

                PORT3                —                            PCR6

                PORT4                —                            PCR6

                PORT5                        PCR6                 —

                     0b - Disables
                     1b - Enables

       3        Slew Rate Enable
    SRE       Configures the slew rate feature, fast or slow, on each corresponding pin.
               The slew rate configuration is valid for all digital pin multiplexing modes.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                        PCR6                 —

                PORT1                        PCR6                 —

                PORT2                        PCR6                 —

                PORT3                        PCR6                 —

                PORT4                        PCR6                 —

                PORT5                —                            PCR6


                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3657 / 3791

<!-- page 3658 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Fast
                     1b - Slow

       2          Pull Value
     PV        Selects high or low internal pull resistor value.
               The pull value configuration is valid for all digital pin multiplexing modes.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                —                            PCR6

                PORT1                —                            PCR6

                PORT2                —                            PCR6

                PORT3                —                            PCR6

                PORT4                —                            PCR6

                PORT5                        PCR6                 —

                     0b - Low
                     1b - High

       1          Pull Enable
     PE       Enables the internal pull resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pull resistor is not enabled on the corresponding pin.
                           • When this field = 1, the internal pull resistor is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
                     0b - Disables
                     1b - Enables

       0          Pull Select
     PS       Enables the internal pullup or pulldown resistor.
                  This configuration is valid for all digital pin multiplexing modes.

                                  Tablecontinuesonthenextpage...




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3658 / 3791

<!-- page 3659 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                           • When this field = 0, the internal pulldown resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                           • When this field = 1, the internal pullup resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                     0b - Enables internal pulldown resistor
                     1b - Enables internal pullup resistor


75.6.1.17  Pin Control a (PCR7 - PCR9)

Offset


 Register                   Offset

 PCR7               9Ch

 PCR8                A0h

 PCR9                A4h


Function
Configures pin control features on each pin.

                                           NOTE
           Do not modify pin configuration registers associated with pins that are unavailable in your selected package. All
            unbonded pins unavailable in your package default to the Disabled state for lowest power consumption.

                                           NOTE
                               Each module instance supports a different number of registers.


                Instance                           Register supported                    Register not supported

 PORT0                               PCR7–PCR9                 —

 PORT1                               PCR7–PCR9                 —

 PORT2                               PCR7–PCR9                 —

 PORT3                               PCR7–PCR9                 —

 PORT4                              PCR7                                PCR8–PCR9

 PORT5                               PCR7–PCR9                 —





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3659 / 3791

<!-- page 3660 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset                                                See Register reset values.

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R            0                                                  0
         LK            INV     IBE           MUX                  DSE   ODE   PFE   SRE    PV    PE    PS
   W

Reset                                               See Register reset values.


Register reset values


 Register                                              Reset value

 PCR7                                        PORT0–PORT5: 0000_0000h

 PCR8–PCR9                                                  PORT0–PORT3: 0000_0000h
                                                    PORT4: Register not supported
                                                    PORT5: 0000_0000h


Fields


       Field       Function
     31-16      Reserved
    —

      15       Lock Register
      LK       Locks this PCR.
             When a PCRnis locked, its fields cannot be updated until the next reset.
                     0b - Does not lock
                     1b - Locks

      14       Reserved
    —

      13         Invert Input
      INV        Inverts the digital input.
                     0b - Does not invert
                     1b - Inverts

      12         Input Buffer Enable

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3660 / 3791

<!-- page 3661 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
      IBE       Enables digital input buffer. When disabled, the digital input is required for analog functions.
                     0b - Disables
                     1b - Enables

      11-8       Pin Multiplex Control
    MUX       Configures the multiplexing slots on each pin.
                Not all pins support all pin multiplexing slots. Unimplemented pin multiplexing slots are reserved.
                Unimplemented pin multiplexing slots can result in different behaviors, if the unimplemented slot is
               phantomed (because the module is phantomed on that die) versus unimplemented on the die.
               The corresponding pin is configured according to the following pin multiplexing slots:

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                         PCR7–PCR9              —

                PORT1                         PCR7–PCR9              —

                PORT2                         PCR7–PCR9              —

                PORT3                         PCR7–PCR9              —

                PORT4                        PCR7                 —

                PORT5                            PCR7–PCR9[9–8]                    PCR7–PCR9[11–10]


                                                 NOTE
                                      The descriptions of the field settings vary by module instance.


                         Instance                                    Field value and description

                PORT0                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3661 / 3791

<!-- page 3662 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


       Field       Function


                         Instance                                    Field value and description

                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT1                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT2                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3662 / 3791

<!-- page 3663 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


       Field       Function


                         Instance                                    Field value and description

                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT3                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)
                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT4                      0000b - Alternative 0 (GPIO)
                                          0001b - Alternative 1 (chip-specific)
                                          0010b - Alternative 2 (chip-specific)
                                          0011b - Alternative 3 (chip-specific)
                                          0100b - Alternative 4 (chip-specific)
                                          0101b - Alternative 5 (chip-specific)
                                          0110b - Alternative 6 (chip-specific)
                                          0111b - Alternative 7 (chip-specific)
                                          1000b - Alternative 8 (chip-specific)
                                          1001b - Alternative 9 (chip-specific)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3663 / 3791

<!-- page 3664 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                         Instance                                    Field value and description

                                          1010b - Alternative 10 (chip-specific)
                                          1011b - Alternative 11 (chip-specific)
                                          1100b - Alternative 12 (chip-specific)
                                          1101b - Alternative 13 (chip-specific)

                PORT5                      00b - Alternative 0 (GPIO)
                                          01b - Alternative 1 (chip-specific)
                                          10b - Alternative 2 (chip-specific)
                                          11b - Alternative 3 (chip-specific)

       7        Reserved
    —

       6         Drive Strength Enable
    DSE       Configures drive strength, low or high, on each pin.
               The drive strength configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, low drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.
                           • When this field = 1, high drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                         PCR7–PCR9              —

                PORT1                         PCR7–PCR9              —

                PORT2                         PCR7–PCR9              —

                PORT3                         PCR7–PCR9              —

                PORT4                        PCR7                 —

                PORT5                —                             PCR7–PCR9


                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3664 / 3791

<!-- page 3665 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                              Instance                       Field supported in                Field not supported in

                     0b - Low
                     1b - High

       5       Open Drain Enable
    ODE      Enables open drain output on each pin.
               The open drain configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the open drain output is disabled on the corresponding pin.
                           • When this field = 1, the open drain output is enabled on the corresponding pin, if the pin is
                      configured as a digital output.
                     0b - Disables
                     1b - Enables

       4        Passive Filter Enable
     PFE       Enables passive input filter on each pin.
               The passive filter configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the passive input filter is disabled on the corresponding pin.
                           • When this field = 1, the passive input filter is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
              See the chip's data sheet for filter characteristics.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                —                             PCR7–PCR9

                PORT1                         PCR8–PCR9                     PCR7

                PORT2                —                             PCR7–PCR9

                PORT3                —                             PCR7–PCR9

                PORT4                —                            PCR7

                PORT5                         PCR7–PCR9              —


                                  Tablecontinuesonthenextpage...




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3665 / 3791

<!-- page 3666 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Disables
                     1b - Enables

       3        Slew Rate Enable
    SRE       Configures the slew rate feature, fast or slow, on each corresponding pin.
               The slew rate configuration is valid for all digital pin multiplexing modes.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                         PCR7–PCR9              —

                PORT1                         PCR7–PCR9              —

                PORT2                         PCR7–PCR9              —

                PORT3                         PCR7–PCR9              —

                PORT4                        PCR7                 —

                PORT5                —                             PCR7–PCR9

                     0b - Fast
                     1b - Slow

       2          Pull Value
     PV        Selects high or low internal pull resistor value.
               The pull value configuration is valid for all digital pin multiplexing modes.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                —                             PCR7–PCR9

                PORT1                        PCR8                         PCR7
                                                                           PCR9

                PORT2                —                             PCR7–PCR9


                                  Tablecontinuesonthenextpage...

                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3666 / 3791

<!-- page 3667 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                              Instance                       Field supported in                Field not supported in

                PORT3                —                             PCR7–PCR9

                PORT4                —                            PCR7

                PORT5                         PCR7–PCR9              —

                     0b - Low
                     1b - High

       1          Pull Enable
     PE       Enables the internal pull resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pull resistor is not enabled on the corresponding pin.
                           • When this field = 1, the internal pull resistor is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
                     0b - Disables
                     1b - Enables

       0          Pull Select
     PS       Enables the internal pullup or pulldown resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pulldown resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                           • When this field = 1, the internal pullup resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                     0b - Enables internal pulldown resistor
                     1b - Enables internal pullup resistor


75.6.1.18  Pin Control a (PCR10 - PCR15)

Offset


 Register                   Offset

 PCR10               A8h

 PCR11             ACh

 PCR12               B0h

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3667 / 3791

<!-- page 3668 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

 Register                   Offset

 PCR13               B4h

 PCR14               B8h

 PCR15             BCh


Function
Configures pin control features on each pin.

                                           NOTE
           Do not modify pin configuration registers associated with pins that are unavailable in your selected package. All
            unbonded pins unavailable in your package default to the Disabled state for lowest power consumption.

                                           NOTE
                               Each module instance supports a different number of registers.


                Instance                           Register supported                    Register not supported

 PORT0                                PCR10–PCR15                —

 PORT1                                PCR10–PCR15                —

 PORT2                                PCR10–PCR11                          PCR12–PCR15

 PORT3                                PCR10–PCR15                —

 PORT4                                PCR12–PCR15                          PCR10–PCR11

 PORT5                    —                                   PCR10–PCR15


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset                                                See Register reset values.

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R            0                                                  0                     0              0
         LK            INV     IBE           MUX                  DSE   ODE         SRE          PE    PS
   W

Reset                                               See Register reset values.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3668 / 3791

<!-- page 3669 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

Register reset values


 Register                                              Reset value

 PCR10–PCR11                                                  PORT0–PORT3: 0000_0000h
                                                    PORT4,PORT5: Register not supported

 PCR12–PCR15                                                    PORT0,PORT1: 0000_0000h
                                                    PORT2: Register not supported
                                                    PORT3,PORT4: 0000_0000h
                                                    PORT5: Register not supported


Fields


       Field       Function
     31-16      Reserved
    —

      15       Lock Register
      LK       Locks this PCR.
             When a PCRnis locked, its fields cannot be updated until the next reset.
                     0b - Does not lock
                     1b - Locks

      14       Reserved
    —

      13         Invert Input
      INV        Inverts the digital input.
                     0b - Does not invert
                     1b - Inverts

      12         Input Buffer Enable
      IBE       Enables digital input buffer. When disabled, the digital input is required for analog functions.
                     0b - Disables
                     1b - Enables

      11-8       Pin Multiplex Control
    MUX       Configures the multiplexing slots on each pin.
                Not all pins support all pin multiplexing slots. Unimplemented pin multiplexing slots are reserved.
                Unimplemented pin multiplexing slots can result in different behaviors, if the unimplemented slot is
               phantomed (because the module is phantomed on that die) versus unimplemented on the die.
               The corresponding pin is configured according to the following pin multiplexing slots:

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3669 / 3791

<!-- page 3670 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0000b - Alternative 0 (GPIO)
                     0001b - Alternative 1 (chip-specific)
                     0010b - Alternative 2 (chip-specific)
                     0011b - Alternative 3 (chip-specific)
                     0100b - Alternative 4 (chip-specific)
                     0101b - Alternative 5 (chip-specific)
                     0110b - Alternative 6 (chip-specific)
                     0111b - Alternative 7 (chip-specific)
                     1000b - Alternative 8 (chip-specific)
                     1001b - Alternative 9 (chip-specific)
                     1010b - Alternative 10 (chip-specific)
                     1011b - Alternative 11 (chip-specific)
                     1100b - Alternative 12 (chip-specific)
                     1101b - Alternative 13 (chip-specific)

       7        Reserved
    —

       6         Drive Strength Enable
    DSE       Configures drive strength, low or high, on each pin.
               The drive strength configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, low drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.
                           • When this field = 1, high drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.
                     0b - Low
                     1b - High

       5       Open Drain Enable
    ODE      Enables open drain output on each pin.
               The open drain configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the open drain output is disabled on the corresponding pin.
                           • When this field = 1, the open drain output is enabled on the corresponding pin, if the pin is
                      configured as a digital output.
                     0b - Disables
                     1b - Enables

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3670 / 3791

<!-- page 3671 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       4        Reserved
    —

       3        Slew Rate Enable
    SRE       Configures the slew rate feature, fast or slow, on each corresponding pin.
               The slew rate configuration is valid for all digital pin multiplexing modes.
                     0b - Fast
                     1b - Slow

       2        Reserved
    —

       1          Pull Enable
     PE       Enables the internal pull resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pull resistor is not enabled on the corresponding pin.
                           • When this field = 1, the internal pull resistor is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
                     0b - Disables
                     1b - Enables

       0          Pull Select
     PS       Enables the internal pullup or pulldown resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pulldown resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                           • When this field = 1, the internal pullup resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                     0b - Enables internal pulldown resistor
                     1b - Enables internal pullup resistor


75.6.1.19  Pin Control 16 (PCR16)

Offset


 Register                   Offset

 PCR16              C0h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3671 / 3791

<!-- page 3672 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

Function
Configures pin control features on each pin.

                                           NOTE
           Do not modify pin configuration registers associated with pins that are unavailable in your selected package. All
            unbonded pins unavailable in your package default to the Disabled state for lowest power consumption.

                                           NOTE
                               Each module instance supports a different number of registers.


                Instance                           Register supported                    Register not supported

 PORT0                               PCR16                    —

 PORT1                               PCR16                    —

 PORT2                    —                                   PCR16

 PORT3                               PCR16                    —

 PORT4                               PCR16                    —

 PORT5                    —                                   PCR16


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R            0                                                  0
         LK            INV     IBE           MUX                  DSE   ODE   PFE   SRE    PV    PE    PS
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-16      Reserved
    —

      15       Lock Register
      LK       Locks this PCR.
             When a PCRnis locked, its fields cannot be updated until the next reset.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3672 / 3791

<!-- page 3673 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Does not lock
                     1b - Locks

      14       Reserved
    —

      13         Invert Input
      INV        Inverts the digital input.
                     0b - Does not invert
                     1b - Inverts

      12         Input Buffer Enable
      IBE       Enables digital input buffer. When disabled, the digital input is required for analog functions.
                     0b - Disables
                     1b - Enables

      11-8       Pin Multiplex Control
    MUX       Configures the multiplexing slots on each pin.
                Not all pins support all pin multiplexing slots. Unimplemented pin multiplexing slots are reserved.
                Unimplemented pin multiplexing slots can result in different behaviors, if the unimplemented slot is
               phantomed (because the module is phantomed on that die) versus unimplemented on the die.
               The corresponding pin is configured according to the following pin multiplexing slots:
                     0000b - Alternative 0 (GPIO)
                     0001b - Alternative 1 (chip-specific)
                     0010b - Alternative 2 (chip-specific)
                     0011b - Alternative 3 (chip-specific)
                     0100b - Alternative 4 (chip-specific)
                     0101b - Alternative 5 (chip-specific)
                     0110b - Alternative 6 (chip-specific)
                     0111b - Alternative 7 (chip-specific)
                     1000b - Alternative 8 (chip-specific)
                     1001b - Alternative 9 (chip-specific)
                     1010b - Alternative 10 (chip-specific)
                     1011b - Alternative 11 (chip-specific)
                     1100b - Alternative 12 (chip-specific)
                     1101b - Alternative 13 (chip-specific)

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3673 / 3791

<!-- page 3674 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       7        Reserved
    —

       6         Drive Strength Enable
    DSE       Configures drive strength, low or high, on each pin.
               The drive strength configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, low drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.
                           • When this field = 1, high drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.
                     0b - Low
                     1b - High

       5       Open Drain Enable
    ODE      Enables open drain output on each pin.
               The open drain configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the open drain output is disabled on the corresponding pin.
                           • When this field = 1, the open drain output is enabled on the corresponding pin, if the pin is
                      configured as a digital output.
                     0b - Disables
                     1b - Enables

       4        Passive Filter Enable
     PFE       Enables passive input filter on each pin.
               The passive filter configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the passive input filter is disabled on the corresponding pin.
                           • When this field = 1, the passive input filter is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
              See the chip's data sheet for filter characteristics.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                         PCR16                —

                PORT1                         PCR16                —


                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3674 / 3791

<!-- page 3675 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                              Instance                       Field supported in                Field not supported in

                PORT3                —                             PCR16

                PORT4                —                             PCR16

                     0b - Disables
                     1b - Enables

       3        Slew Rate Enable
    SRE       Configures the slew rate feature, fast or slow, on each corresponding pin.
               The slew rate configuration is valid for all digital pin multiplexing modes.
                     0b - Fast
                     1b - Slow

       2          Pull Value
     PV        Selects high or low internal pull resistor value.
               The pull value configuration is valid for all digital pin multiplexing modes.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                         PCR16                —

                PORT1                         PCR16                —

                PORT3                —                             PCR16

                PORT4                —                             PCR16

                     0b - Low
                     1b - High

       1          Pull Enable
     PE       Enables the internal pull resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pull resistor is not enabled on the corresponding pin.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3675 / 3791

<!-- page 3676 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                           • When this field = 1, the internal pull resistor is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
                     0b - Disables
                     1b - Enables

       0          Pull Select
     PS       Enables the internal pullup or pulldown resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pulldown resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                           • When this field = 1, the internal pullup resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                     0b - Enables internal pulldown resistor
                     1b - Enables internal pullup resistor


75.6.1.20  Pin Control 17 (PCR17)

Offset


 Register                   Offset

 PCR17              C4h


Function
Configures pin control features on each pin.

                                           NOTE
           Do not modify pin configuration registers associated with pins that are unavailable in your selected package. All
            unbonded pins unavailable in your package default to the Disabled state for lowest power consumption.

                                           NOTE
                               Each module instance supports a different number of registers.


                Instance                           Register supported                    Register not supported

 PORT0                               PCR17                    —

 PORT1                               PCR17                    —

 PORT2                    —                                   PCR17

 PORT3                               PCR17                    —

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3676 / 3791

<!-- page 3677 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                                Tablecontinuedfromthepreviouspage...

                Instance                           Register supported                    Register not supported

 PORT4                               PCR17                    —

 PORT5                    —                                   PCR17


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R            0                                                  0                                    0
         LK            INV     IBE           MUX                  DSE   ODE   PFE   SRE          PE    PS
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-16      Reserved
    —

      15       Lock Register
      LK       Locks this PCR.
             When a PCRnis locked, its fields cannot be updated until the next reset.
                     0b - Does not lock
                     1b - Locks

      14       Reserved
    —

      13         Invert Input
      INV        Inverts the digital input.
                     0b - Does not invert
                     1b - Inverts

      12         Input Buffer Enable
      IBE       Enables digital input buffer. When disabled, the digital input is required for analog functions.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3677 / 3791

<!-- page 3678 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Disables
                     1b - Enables

      11-8       Pin Multiplex Control
    MUX       Configures the multiplexing slots on each pin.
                Not all pins support all pin multiplexing slots. Unimplemented pin multiplexing slots are reserved.
                Unimplemented pin multiplexing slots can result in different behaviors, if the unimplemented slot is
               phantomed (because the module is phantomed on that die) versus unimplemented on the die.
               The corresponding pin is configured according to the following pin multiplexing slots:
                     0000b - Alternative 0 (GPIO)
                     0001b - Alternative 1 (chip-specific)
                     0010b - Alternative 2 (chip-specific)
                     0011b - Alternative 3 (chip-specific)
                     0100b - Alternative 4 (chip-specific)
                     0101b - Alternative 5 (chip-specific)
                     0110b - Alternative 6 (chip-specific)
                     0111b - Alternative 7 (chip-specific)
                     1000b - Alternative 8 (chip-specific)
                     1001b - Alternative 9 (chip-specific)
                     1010b - Alternative 10 (chip-specific)
                     1011b - Alternative 11 (chip-specific)
                     1100b - Alternative 12 (chip-specific)
                     1101b - Alternative 13 (chip-specific)

       7        Reserved
    —

       6         Drive Strength Enable
    DSE       Configures drive strength, low or high, on each pin.
               The drive strength configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, low drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.
                           • When this field = 1, high drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.
                     0b - Low
                     1b - High

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3678 / 3791

<!-- page 3679 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

       5       Open Drain Enable
    ODE      Enables open drain output on each pin.
               The open drain configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the open drain output is disabled on the corresponding pin.
                           • When this field = 1, the open drain output is enabled on the corresponding pin, if the pin is
                      configured as a digital output.
                     0b - Disables
                     1b - Enables

       4        Passive Filter Enable
     PFE       Enables passive input filter on each pin.
               The passive filter configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the passive input filter is disabled on the corresponding pin.
                           • When this field = 1, the passive input filter is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
              See the chip's data sheet for filter characteristics.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                         PCR17                —

                PORT1                         PCR17                —

                PORT3                —                             PCR17

                PORT4                —                             PCR17

                     0b - Disables
                     1b - Enables

       3        Slew Rate Enable
    SRE       Configures the slew rate feature, fast or slow, on each corresponding pin.
               The slew rate configuration is valid for all digital pin multiplexing modes.
                     0b - Fast
                     1b - Slow

                                  Tablecontinuesonthenextpage...




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3679 / 3791

<!-- page 3680 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       2        Reserved
    —

       1          Pull Enable
     PE       Enables the internal pull resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pull resistor is not enabled on the corresponding pin.
                           • When this field = 1, the internal pull resistor is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
                     0b - Disables
                     1b - Enables

       0          Pull Select
     PS       Enables the internal pullup or pulldown resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pulldown resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                           • When this field = 1, the internal pullup resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                     0b - Enables internal pulldown resistor
                     1b - Enables internal pullup resistor


75.6.1.21  Pin Control a (PCR18 - PCR19)

Offset


 Register                   Offset

 PCR18              C8h

 PCR19             CCh


Function
Configures pin control features on each pin.

                                           NOTE
           Do not modify pin configuration registers associated with pins that are unavailable in your selected package. All
            unbonded pins unavailable in your package default to the Disabled state for lowest power consumption.

                                           NOTE
                               Each module instance supports a different number of registers.




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3680 / 3791

<!-- page 3681 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                Instance                           Register supported                    Register not supported

 PORT0                                PCR18–PCR19                —

 PORT1                                PCR18–PCR19                —

 PORT2                    —                                   PCR18–PCR19

 PORT3                                PCR18–PCR19                —

 PORT4                                PCR18–PCR19                —

 PORT5                    —                                   PCR18–PCR19


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R            0                                                  0                     0              0
         LK            INV     IBE           MUX                  DSE   ODE         SRE          PE    PS
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-16      Reserved
    —

      15       Lock Register
      LK       Locks this PCR.
             When a PCRnis locked, its fields cannot be updated until the next reset.
                     0b - Does not lock
                     1b - Locks

      14       Reserved
    —

      13         Invert Input
      INV        Inverts the digital input.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3681 / 3791

<!-- page 3682 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Does not invert
                     1b - Inverts

      12         Input Buffer Enable
      IBE       Enables digital input buffer. When disabled, the digital input is required for analog functions.
                     0b - Disables
                     1b - Enables

      11-8       Pin Multiplex Control
    MUX       Configures the multiplexing slots on each pin.
                Not all pins support all pin multiplexing slots. Unimplemented pin multiplexing slots are reserved.
                Unimplemented pin multiplexing slots can result in different behaviors, if the unimplemented slot is
               phantomed (because the module is phantomed on that die) versus unimplemented on the die.
               The corresponding pin is configured according to the following pin multiplexing slots:
                     0000b - Alternative 0 (GPIO)
                     0001b - Alternative 1 (chip-specific)
                     0010b - Alternative 2 (chip-specific)
                     0011b - Alternative 3 (chip-specific)
                     0100b - Alternative 4 (chip-specific)
                     0101b - Alternative 5 (chip-specific)
                     0110b - Alternative 6 (chip-specific)
                     0111b - Alternative 7 (chip-specific)
                     1000b - Alternative 8 (chip-specific)
                     1001b - Alternative 9 (chip-specific)
                     1010b - Alternative 10 (chip-specific)
                     1011b - Alternative 11 (chip-specific)
                     1100b - Alternative 12 (chip-specific)
                     1101b - Alternative 13 (chip-specific)

       7        Reserved
    —

       6         Drive Strength Enable
    DSE       Configures drive strength, low or high, on each pin.
               The drive strength configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, low drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3682 / 3791

<!-- page 3683 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                           • When this field = 1, high drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.
                     0b - Low
                     1b - High

       5       Open Drain Enable
    ODE      Enables open drain output on each pin.
               The open drain configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the open drain output is disabled on the corresponding pin.
                           • When this field = 1, the open drain output is enabled on the corresponding pin, if the pin is
                      configured as a digital output.
                     0b - Disables
                     1b - Enables

       4        Reserved
    —

       3        Slew Rate Enable
    SRE       Configures the slew rate feature, fast or slow, on each corresponding pin.
               The slew rate configuration is valid for all digital pin multiplexing modes.
                     0b - Fast
                     1b - Slow

       2        Reserved
    —

       1          Pull Enable
     PE       Enables the internal pull resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pull resistor is not enabled on the corresponding pin.
                           • When this field = 1, the internal pull resistor is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
                     0b - Disables
                     1b - Enables

       0          Pull Select
     PS       Enables the internal pullup or pulldown resistor.
                  This configuration is valid for all digital pin multiplexing modes.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3683 / 3791

<!-- page 3684 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                           • When this field = 0, the internal pulldown resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                           • When this field = 1, the internal pullup resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                     0b - Enables internal pulldown resistor
                     1b - Enables internal pullup resistor


75.6.1.22  Pin Control 20 (PCR20)

Offset


 Register                   Offset

 PCR20              D0h


Function
Configures pin control features on each pin.

                                           NOTE
           Do not modify pin configuration registers associated with pins that are unavailable in your selected package. All
            unbonded pins unavailable in your package default to the Disabled state for lowest power consumption.

                                           NOTE
                               Each module instance supports a different number of registers.


                Instance                           Register supported                    Register not supported

 PORT0                               PCR20                    —

 PORT1                               PCR20                    —

 PORT2                    —                                   PCR20

 PORT3                               PCR20                    —

 PORT4                               PCR20                    —

 PORT5                    —                                   PCR20





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3684 / 3791

<!-- page 3685 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R            0                                                  0
         LK            INV     IBE           MUX                  DSE   ODE   PFE   SRE    PV    PE    PS
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-16      Reserved
    —

      15       Lock Register
      LK       Locks this PCR.
             When a PCRnis locked, its fields cannot be updated until the next reset.
                     0b - Does not lock
                     1b - Locks

      14       Reserved
    —

      13         Invert Input
      INV        Inverts the digital input.
                     0b - Does not invert
                     1b - Inverts

      12         Input Buffer Enable
      IBE       Enables digital input buffer. When disabled, the digital input is required for analog functions.
                     0b - Disables
                     1b - Enables

      11-8       Pin Multiplex Control
    MUX       Configures the multiplexing slots on each pin.
                Not all pins support all pin multiplexing slots. Unimplemented pin multiplexing slots are reserved.
                Unimplemented pin multiplexing slots can result in different behaviors, if the unimplemented slot is
               phantomed (because the module is phantomed on that die) versus unimplemented on the die.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3685 / 3791

<!-- page 3686 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

               The corresponding pin is configured according to the following pin multiplexing slots:
                     0000b - Alternative 0 (GPIO)
                     0001b - Alternative 1 (chip-specific)
                     0010b - Alternative 2 (chip-specific)
                     0011b - Alternative 3 (chip-specific)
                     0100b - Alternative 4 (chip-specific)
                     0101b - Alternative 5 (chip-specific)
                     0110b - Alternative 6 (chip-specific)
                     0111b - Alternative 7 (chip-specific)
                     1000b - Alternative 8 (chip-specific)
                     1001b - Alternative 9 (chip-specific)
                     1010b - Alternative 10 (chip-specific)
                     1011b - Alternative 11 (chip-specific)
                     1100b - Alternative 12 (chip-specific)
                     1101b - Alternative 13 (chip-specific)

       7        Reserved
    —

       6         Drive Strength Enable
    DSE       Configures drive strength, low or high, on each pin.
               The drive strength configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, low drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.
                           • When this field = 1, high drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.
                     0b - Low
                     1b - High

       5       Open Drain Enable
    ODE      Enables open drain output on each pin.
               The open drain configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the open drain output is disabled on the corresponding pin.
                           • When this field = 1, the open drain output is enabled on the corresponding pin, if the pin is
                      configured as a digital output.
                     0b - Disables

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3686 / 3791

<!-- page 3687 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1b - Enables

       4        Passive Filter Enable
     PFE       Enables passive input filter on each pin.
               The passive filter configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the passive input filter is disabled on the corresponding pin.
                           • When this field = 1, the passive input filter is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
              See the chip's data sheet for filter characteristics.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                         PCR20                —

                PORT1                —                             PCR20

                PORT3                —                             PCR20

                PORT4                —                             PCR20

                     0b - Disables
                     1b - Enables

       3        Slew Rate Enable
    SRE       Configures the slew rate feature, fast or slow, on each corresponding pin.
               The slew rate configuration is valid for all digital pin multiplexing modes.
                     0b - Fast
                     1b - Slow

       2          Pull Value
     PV        Selects high or low internal pull resistor value.
               The pull value configuration is valid for all digital pin multiplexing modes.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.

                                  Tablecontinuesonthenextpage...




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3687 / 3791

<!-- page 3688 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                              Instance                       Field supported in                Field not supported in

                PORT0                         PCR20                —

                PORT1                —                             PCR20

                PORT3                —                             PCR20

                PORT4                —                             PCR20

                     0b - Low
                     1b - High

       1          Pull Enable
     PE       Enables the internal pull resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pull resistor is not enabled on the corresponding pin.
                           • When this field = 1, the internal pull resistor is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
                     0b - Disables
                     1b - Enables

       0          Pull Select
     PS       Enables the internal pullup or pulldown resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pulldown resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                           • When this field = 1, the internal pullup resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                     0b - Enables internal pulldown resistor
                     1b - Enables internal pullup resistor


75.6.1.23  Pin Control 21 (PCR21)

Offset


 Register                   Offset

 PCR21              D4h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3688 / 3791

<!-- page 3689 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

Function
Configures pin control features on each pin.

                                           NOTE
           Do not modify pin configuration registers associated with pins that are unavailable in your selected package. All
            unbonded pins unavailable in your package default to the Disabled state for lowest power consumption.

                                           NOTE
                               Each module instance supports a different number of registers.


                Instance                           Register supported                    Register not supported

 PORT0                               PCR21                    —

 PORT1                               PCR21                    —

 PORT2                    —                                   PCR21

 PORT3                               PCR21                    —

 PORT4                               PCR21                    —

 PORT5                    —                                   PCR21


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R            0                                                  0                                    0
         LK            INV     IBE           MUX                  DSE   ODE   PFE   SRE          PE    PS
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-16      Reserved
    —

      15       Lock Register
      LK       Locks this PCR.
             When a PCRnis locked, its fields cannot be updated until the next reset.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3689 / 3791

<!-- page 3690 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Does not lock
                     1b - Locks

      14       Reserved
    —

      13         Invert Input
      INV        Inverts the digital input.
                     0b - Does not invert
                     1b - Inverts

      12         Input Buffer Enable
      IBE       Enables digital input buffer. When disabled, the digital input is required for analog functions.
                     0b - Disables
                     1b - Enables

      11-8       Pin Multiplex Control
    MUX       Configures the multiplexing slots on each pin.
                Not all pins support all pin multiplexing slots. Unimplemented pin multiplexing slots are reserved.
                Unimplemented pin multiplexing slots can result in different behaviors, if the unimplemented slot is
               phantomed (because the module is phantomed on that die) versus unimplemented on the die.
               The corresponding pin is configured according to the following pin multiplexing slots:
                     0000b - Alternative 0 (GPIO)
                     0001b - Alternative 1 (chip-specific)
                     0010b - Alternative 2 (chip-specific)
                     0011b - Alternative 3 (chip-specific)
                     0100b - Alternative 4 (chip-specific)
                     0101b - Alternative 5 (chip-specific)
                     0110b - Alternative 6 (chip-specific)
                     0111b - Alternative 7 (chip-specific)
                     1000b - Alternative 8 (chip-specific)
                     1001b - Alternative 9 (chip-specific)
                     1010b - Alternative 10 (chip-specific)
                     1011b - Alternative 11 (chip-specific)
                     1100b - Alternative 12 (chip-specific)
                     1101b - Alternative 13 (chip-specific)

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3690 / 3791

<!-- page 3691 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       7        Reserved
    —

       6         Drive Strength Enable
    DSE       Configures drive strength, low or high, on each pin.
               The drive strength configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, low drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.
                           • When this field = 1, high drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.
                     0b - Low
                     1b - High

       5       Open Drain Enable
    ODE      Enables open drain output on each pin.
               The open drain configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the open drain output is disabled on the corresponding pin.
                           • When this field = 1, the open drain output is enabled on the corresponding pin, if the pin is
                      configured as a digital output.
                     0b - Disables
                     1b - Enables

       4        Passive Filter Enable
     PFE       Enables passive input filter on each pin.
               The passive filter configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the passive input filter is disabled on the corresponding pin.
                           • When this field = 1, the passive input filter is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
              See the chip's data sheet for filter characteristics.

                                                 NOTE
                                 This field is not supported in every instance. The following table includes only
                                supported registers.


                              Instance                       Field supported in                Field not supported in

                PORT0                         PCR21                —

                PORT1                —                             PCR21


                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3691 / 3791

<!-- page 3692 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function


                              Instance                       Field supported in                Field not supported in

                PORT3                —                             PCR21

                PORT4                —                             PCR21

                     0b - Disables
                     1b - Enables

       3        Slew Rate Enable
    SRE       Configures the slew rate feature, fast or slow, on each corresponding pin.
               The slew rate configuration is valid for all digital pin multiplexing modes.
                     0b - Fast
                     1b - Slow

       2        Reserved
    —

       1          Pull Enable
     PE       Enables the internal pull resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pull resistor is not enabled on the corresponding pin.
                           • When this field = 1, the internal pull resistor is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
                     0b - Disables
                     1b - Enables

       0          Pull Select
     PS       Enables the internal pullup or pulldown resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pulldown resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                           • When this field = 1, the internal pullup resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                     0b - Enables internal pulldown resistor
                     1b - Enables internal pullup resistor





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3692 / 3791

<!-- page 3693 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

75.6.1.24  Pin Control a (PCR22 - PCR31)

Offset
For a = 22 to 31:


 Register                   Offset

 PCRa                 80h + (a × 4h)


Function
Configures pin control features on each pin.

                                           NOTE
           Do not modify pin configuration registers associated with pins that are unavailable in your selected package. All
            unbonded pins unavailable in your package default to the Disabled state for lowest power consumption.

                                           NOTE
                               Each module instance supports a different number of registers.


                Instance                           Register supported                    Register not supported

 PORT0                                PCR22–PCR31                —

 PORT1                                PCR22–PCR23                          PCR24–PCR29
                                      PCR30–PCR31

 PORT2                    —                                   PCR22–PCR31

 PORT3                                PCR22–PCR23                          PCR24–PCR31

 PORT4                                PCR22–PCR23                          PCR24–PCR31

 PORT5                    —                                   PCR22–PCR31


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset                                                See Register reset values.

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R            0                                                  0                     0              0
         LK            INV     IBE           MUX                  DSE   ODE         SRE          PE    PS
   W

Reset                                               See Register reset values.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3693 / 3791

<!-- page 3694 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)

Register reset values


 Register                                              Reset value

 PCR22–PCR23                                                    PORT0,PORT1: 0000_0000h
                                                    PORT2: Register not supported
                                                    PORT3,PORT4: 0000_0000h
                                                    PORT5: Register not supported

 PCR24–PCR29                                     0000_0000h

 PCR30–PCR31                                                    PORT0,PORT1: 0000_0000h
                                                  PORT2–PORT5: Register not supported


Fields


       Field       Function
     31-16      Reserved
    —

      15       Lock Register
      LK       Locks this PCR.
             When a PCRnis locked, its fields cannot be updated until the next reset.
                     0b - Does not lock
                     1b - Locks

      14       Reserved
    —

      13         Invert Input
      INV        Inverts the digital input.
                     0b - Does not invert
                     1b - Inverts

      12         Input Buffer Enable
      IBE       Enables digital input buffer. When disabled, the digital input is required for analog functions.
                     0b - Disables
                     1b - Enables

      11-8       Pin Multiplex Control
    MUX       Configures the multiplexing slots on each pin.
                Not all pins support all pin multiplexing slots. Unimplemented pin multiplexing slots are reserved.
                Unimplemented pin multiplexing slots can result in different behaviors, if the unimplemented slot is
               phantomed (because the module is phantomed on that die) versus unimplemented on the die.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3694 / 3791

<!-- page 3695 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

               The corresponding pin is configured according to the following pin multiplexing slots:
                     0000b - Alternative 0 (GPIO)
                     0001b - Alternative 1 (chip-specific)
                     0010b - Alternative 2 (chip-specific)
                     0011b - Alternative 3 (chip-specific)
                     0100b - Alternative 4 (chip-specific)
                     0101b - Alternative 5 (chip-specific)
                     0110b - Alternative 6 (chip-specific)
                     0111b - Alternative 7 (chip-specific)
                     1000b - Alternative 8 (chip-specific)
                     1001b - Alternative 9 (chip-specific)
                     1010b - Alternative 10 (chip-specific)
                     1011b - Alternative 11 (chip-specific)
                     1100b - Alternative 12 (chip-specific)
                     1101b - Alternative 13 (chip-specific)

       7        Reserved
    —

       6         Drive Strength Enable
    DSE       Configures drive strength, low or high, on each pin.
               The drive strength configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, low drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.
                           • When this field = 1, high drive strength is configured on the corresponding pin, if the pin is configured
                   as a digital output.
                     0b - Low
                     1b - High

       5       Open Drain Enable
    ODE      Enables open drain output on each pin.
               The open drain configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the open drain output is disabled on the corresponding pin.
                           • When this field = 1, the open drain output is enabled on the corresponding pin, if the pin is
                      configured as a digital output.
                     0b - Disables

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3695 / 3791

<!-- page 3696 -->

NXP Semiconductors
                                                                                                                      Port Control (PORT)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1b - Enables

       4        Reserved
    —

       3        Slew Rate Enable
    SRE       Configures the slew rate feature, fast or slow, on each corresponding pin.
               The slew rate configuration is valid for all digital pin multiplexing modes.
                     0b - Fast
                     1b - Slow

       2        Reserved
    —

       1          Pull Enable
     PE       Enables the internal pull resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pull resistor is not enabled on the corresponding pin.
                           • When this field = 1, the internal pull resistor is enabled on the corresponding pin, if the pin is
                      configured as a digital input.
                     0b - Disables
                     1b - Enables

       0          Pull Select
     PS       Enables the internal pullup or pulldown resistor.
                  This configuration is valid for all digital pin multiplexing modes.
                           • When this field = 0, the internal pulldown resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                           • When this field = 1, the internal pullup resistor is enabled on the corresponding pin, if the
                     corresponding PCRn.PE field = 1.
                     0b - Enables internal pulldown resistor
                     1b - Enables internal pullup resistor





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3696 / 3791
