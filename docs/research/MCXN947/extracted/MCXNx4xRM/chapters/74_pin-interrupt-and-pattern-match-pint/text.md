# 74 Pin Interrupt and Pattern Match (PINT)

Source: `docs/mcxn947/MCXNx4xRM.pdf`
Pages: 3548-3569

<!-- page 3548 -->

NXP Semiconductors

Chapter 74
Pin Interrupt and Pattern Match (PINT)

74.1  Chip-specific PINT information

Table 711. Reference links to related information

         Topic             Related module                                Reference

      Full description            PINT                                   PINT

  System memory map                                           Memory map

        Clocking                                                        Clock distribution

   Signal multiplexing          Port control                                  Signal multiplexing
    Input multiplexing       INPUTMUX              See PINTSEL0 - PINTSEL7 registers in INPUTMUX

74.1.1 Module instances
This device has one instance of the PINT module, PINT0.

74.1.2  Initialization
   1. Select up to eight external interrupt pins from all available digital port pins on ports 0 and 1 in INPUTMUX. The pin
       selection process is the same for pin interrupts and the pattern match engine. The two features are mutually exclusive.
   2. Enable the clock to the PINT module via SYSCON AHBCLKCTRL0[PINT].
   3. Clear the PINT peripheral reset via SYSCON PRESETCTRL0[PINT_RST].

74.1.2.1  Pin interrupts and Pattern match initialization
In addition to the initialization steps described above, for Pin interrupts initialization and Pattern match initialization, ensure that
eight interrupt outputs from PINT are ORed together to one NVIC slot.

74.1.2.2  Configure pins as pin interrupts or as inputs to the pattern match engine
   1. Determine the pins that you want to serve as pin interrupts or pattern match inputs. See the data sheet for determining
      the GPIO port pin number associated with the package pin.
   2. For each pin selected, program the GPIO port pin number from ports 0 and 1 into one of the eight PINT_SEL registers
       in the INPUTMUX module.

                                           NOTE
            The port pin number serves to identify the pin to the PINT_SEL register. Any function, including GPIO, can be
             assigned to this pin via IOCON.

   3. Enable each pin interrupt in the NVIC.
Once the pin interrupts or pattern match inputs are configured, the pin interrupt detection levels or the pattern match boolean
expression can be set up.

                                           NOTE
            The inputs to the Pin interrupt select registers bypass the IOCON function selection. They do not have to
            be selected as GPIO in IOCON. Make sure that no analog function is selected on pins that are input to the
               pin interrupts.




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3548 / 3791

<!-- page 3549 -->

NXP Semiconductors
                                                                                               Pin Interrupt and Pattern Match (PINT)

74.2 Overview
The Pin Interrupt and Pattern Match (PINT) IP provides interrupt functionality for a selected set of GPIO pins. Per-pin edge- or
level-sensitive interrupts can be generated. This IP also includes a pattern match engine that uses the selected GPIO pins as
inputs to a Boolean expression. When the expression is satisfied, an interrupt is generated.

74.2.1  Block diagram
From all available GPIO pins, select up to 8 pins in the INPUTMUX module to serve as external interrupt pins (see the
"INPUTMUX" chapter for more details). Each external interrupt pins connect to eight individual interrupts in the NVIC. It is possible
to create external interrupt pins based either on rising or falling edges or on the input level.


                                    IN[i]                                   IENF[i]                                 PMCTRL[PMAT]
                                 (to pattern-                    (select high/low                                                (from pattern-
                       match logic)       for level-sensitive interrupt)                                          match logic)
     PIO0_0   00
     PIO0_1   01                   Synchronization
     PIO0_2   02
                                                                                 IENR[i]
                                                                                                                   1
                                                               (enable rising edge or                                                                                                1                          To NVIC
                                    Rising-edge detect        level-sensitive interrupt)                                            0
                               and synchronization                                                  0
                                               clear                                                                                             IST[i]
    PIO1_31   3F                                                                           RISE[i]                                                       (interrupt
                          Write 1 to RISE[i]                                                                                           status)
                                                                           FALL[i] PINSELa                                                                                              PMCTRL[SEL_PMATCH]
                                          Falling edge-detect                                                                                                                              (from pattern-match logic)
                               and synchronization
                                               clear


                          Write 1 to FALL[i]                                       IENF[i]                        ISEL[i]
                                                               (Enable falling-edge     (Edge- or level-sensitive
                                                                                         interrupt)            interrupt selection)

 Figure 438. Pin Interrupt diagram

74.2.2  Features
   • Pin interrupts
   — Select up to 8 pins from all GPIO pins on ports 0 and 1 as edge- or level-sensitive interrupt requests. Each request
         creates a separate interrupt in the NVIC.
   — Edge-sensitive interrupt pins interrupt on rising or falling edges or both.
   — Level-sensitive interrupt pins are high- or low-active.
   • Pattern-match engine
   — Select up to 8 pins from all digital pins on ports 0 and 1 to contribute to a Boolean expression. The Boolean
         expression consists of specified levels and/or transitions on various combinations of these pins.
   — Each bit slice minterm (product term) consisting of the specified Boolean expression can generate its own, dedicated
           interrupt request.
   — You can program any occurrence of a pattern match to generate a Receive Event (RXEV) notification to the CPU.
   — Use pattern match, in conjunction with software, to create complex state machines based on pin inputs.

74.3  Functional description

74.3.1  Pattern-match engine
With pattern matching, you can construct complex Boolean expressions using the same set of 8 GPIO pins you selected for
interrupts. Implement each term in the Boolean expression as one slice of the pattern-match engine. A slice consists of an input


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3549 / 3791

<!-- page 3550 -->

NXP Semiconductors
                                                                                               Pin Interrupt and Pattern Match (PINT)

selector and detection logic that continuously monitors the selected input and creates a HIGH output (true). If the input qualifies
as detected, several terms combine to a minterm. Assertion of a pin interrupt occurs when the minterm evaluates as true.
The detection logic of each slice can detect the following events on the selected input:
   • Edge with memory (sticky): A rising edge, a falling edge, or a rising or falling edge that is detected at any time after the
    edge-detection mechanism has been cleared. The input qualifies as detected (the detection logic output remains high)
     until the pattern-match engine detect logic is cleared again.
   • Event (nonsticky): Every time an edge (rising or falling) is detected, the detection logic output goes HIGH. After one clock
    cycle, writing 1 to this bit enables the detection logic to detect another edge.
   • Level: A high or low level on the selected input.
The following figure shows the details of the edge-detection logic for each slice.




                                                              1     0
                                                                       From previous
                                           Rising-edge                             PINT slice
              (from pin                      detect (sticky)                     1
             interrupt logic)                                                     PMCFG
                                             Falling-edge                                    PROD_ENDPTS[i]
                                                                     2                  IN[0]    0                 detect (sticky)
                  IN[1]    1
                  IN[2]    2                                              3
                  IN[3]    3                                                                                                 PMCTRL[PMAT]
                  IN[4]    4                                                                                                               (also to pin                                                                     4
                  IN[5]    5                                                                                                                    interrupt logic)
                  IN[6]    6
                  IN[7]    7                                              5


                                                              0     6
               PMSRC             Rising-edge
                        SRC[i]            detect (nonsticky)
                                                                     7
                                             Falling-edge
                                           detect (nonsticky)

                                                    PMCFG                To next
                                                                              CFG[i]               PINT slice

 Figure 439. Pattern-match bit slice

Combined sticky and nonsticky events form a pin interrupt whenever a rising or falling edge occurs after a qualifying edge event.
Combining a level detect with an event detect can create a time window during which rising or falling edges will create a pin
interrupt. See Pattern-match engine edge-detect examples for details.
The following figure shows connections between the pins and the pattern-match engine. The INPUTMUX block selects all pins
that are inputs to the pattern-match engine and can be GPIO port pins or other pin functions depending on the configuration.

                                           NOTE
            The pattern-match feature requires clocks in order to operate. It cannot generate an interrupt or wake the chip
               during reduced power modes below sleep mode.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3550 / 3791

<!-- page 3551 -->

NXP Semiconductors
                                                                                               Pin Interrupt and Pattern Match (PINT)



                                  To IN7
                                           slice n - 1
                                                             From slice
                                        To IN0                       n - 1
                                                   slice n - 1                       (tied high for slice 0)


                                                         Slice n                              Endpoint                    INPUT MUX
                                                                                        configured?
                                                        PMCFG bit n = 1
                                                               i                        IN0                       (PROD_ENDPTS)                All pins
                                                                                                  SCRn
                     PINSEL0                                        bits        Detect                                     Pin interrupt n
                                                                               logic
                                                               i                        IN7      PMSCR
                All pins


                     PINSEL7

                                                                      Slice n + 1                           Endpoint
                                                                                                    configured?
                                                               PMCFG bit n + 1 = 1
                                                          IN0 1                   (PROD_ENDPTS,
                              +                             tied high for slice 7)
                                                                                                                      SCRn                                                  Pin interrupt n + 1
                                                                               Detect
                                                                                                                      bits          logic


                                                          IN7                                                                                                                                                    PMSCR


                                  To IN7
                                           slice n + 2

                                        To IN0                                           To slice n + 2
                                                   slice n + 2

 Figure 440. Pattern-match engine connections

The pattern-match logic continuously monitors the 8 inputs and generates interrupts when any one or more minterms (product
terms) of the specified Boolean expression match. Each individual minterm generates a separate interrupt request.
It is also possible to enable the pattern-match module to generate RXEV output if any minterm in the Boolean expression matches.
The pattern-match function uses the same 8 interrupt request lines as the pin interrupts. Therefore, in terms of interrupt generation,
these two features are mutually exclusive. Control bit generates interrupt requests based on standard pin interrupts or pattern
matches. When selecting the pin interrupts, enable the RXEV request to the CPU for pattern matches.

                                           NOTE
           Do not use pattern matching to wake the part from deep-sleep mode. Pin interrupts must be selected in order to
             use the GPIO for wake-up.

8 bit-slice elements make up the pattern-match module. Each bit slice represents a single component in a minterm (product term)
within the Boolean expression. Every time the last bit slice for a particular minterm matches, an interrupt request is asserted.
Use the pattern-match capability to create complex software state machines. Each minterm (and its corresponding individual
interrupt) represents a different transition event to a new state. Then establish the new set of conditions (that is a new Boolean
expression) to make a transition out of the current state.

74.3.2  Clocking
The PINT module has a single clock input, the APB PCLK.
This clock is used for its APB interface, as well as all other peripheral functionality. It must be enabled by the system for register
accesses and pattern matching.



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3551 / 3791

<!-- page 3552 -->

NXP Semiconductors
                                                                                               Pin Interrupt and Pattern Match (PINT)

74.3.3  Interrupts
The PINT module has 8 interrupt channels, which can be configured by its registers. Each channel has its own interrupt output.
Depending on how this IP is integrated, the interrupt outputs can be ORed together to use a single system-level interrupt.

74.4  External signals
PINSELa determines the inputs to the pin interrupt and pattern-match and engines. See the "INPUTMUX" chapter for more details.

74.5  Initialization
For initialization steps, see chip-specific section.

74.6  Application information

Pattern-Match Interrupt Bit-Slice Source (PMSRC) and Pattern-Match Interrupt Bit Slice Configuration (PMCFG) specify the
following expression:



 IN0 AND NOT IN1 AND IN3 rising edge OR IN1 AND IN2 OR IN0 AND NOT IN3 AND NOT IN4

Each term in the Boolean expression, IN0, NOT IN1, IN3 rising edge, and so on, represents one bit slice of the pattern-
match engine.
   • In the first AND function IN0 AND NOT IN1 AND IN3 rising edge, bit slice 0 monitors for a high level on input IN0, bit slice
   1 monitors for a low level on input IN1, and bit slice 2 monitors for a rising edge on input IN3. Detecting this combination of
    features (all three terms are true) asserts the interrupt associated with bit slice 2.
   • In the second AND function IN1 AND IN2, bit slice 3 monitors input IN1 for a high level and bit slice 4 monitors input IN2
     for a high level. Detecting this combination asserts the interrupt associated with bit slice 4.
   • In the third AND function IN0 AND NOT IN3 AND NOT IN4, bit slice 5 monitors input IN0 for a high level, bit slice 6
    monitors input IN3 for a low level, and bit slice 7 monitors input IN4 for a low level. Detecting this combination asserts the
     interrupt associated with bit slice 7.
   • The ORed result of all three AND functions asserts the RXEV request to the CPU. That is, any of the three terms that are
    true will assert the output.

74.6.1  Pattern-match engine example
Suppose you want to match the Boolean pattern
(IN1) + (IN1 * IN2) + (~IN2 * ~IN3 * IN6fe) + (IN5 * IN7ev)
with:
IN6fe = (sticky) falling edge on input 6
IN7ev = (nonsticky) event (rising or falling edge) on input 7
Each individual term in the above expression is controlled by one bit slice. To specify this expression, program the pattern-match
bit slice source and configuration register fields as follows:
   • Pattern-Match Interrupt Bit-Slice Source (PMSRC)
   — Because bit slice 5 is used to detect a sticky event on input 6, you can write 1 to SRC5 to clear any preexisting edge
         detects on bit slice 5.
   — SRC0: 001 - select input 1 for bit slice 0
   — SRC1: 001 - select input 1 for bit slice 1
   — SRC2: 010 - select input 2 for bit slice 2



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3552 / 3791

<!-- page 3553 -->

NXP Semiconductors
                                                                                               Pin Interrupt and Pattern Match (PINT)

   — SRC3: 010 - select input 2 for bit slice 3
   — SRC4: 011 - select input 3 for bit slice 4
   — SRC5: 110 - select input 6 for bit slice 5
   — SRC6: 101 - select input 5 for bit slice 6
   — SRC7: 111 - select input 7 for bit slice 7
   • Pattern-Match Interrupt Bit Slice Configuration (PMCFG)
   — PROD_ENDPTS0 = 1
   — PROD_ENDPTS2 = 1
   — PROD_ENDPTS5 = 1
   — All other slices are not product term endpoints and their PMCFG[PROD_ENDPTSn] are 0. Slice 7 is always a
         product term endpoint and does not have a register bit associated with it.
   — PROD_ENDPTS= 0100101 - bit slices 0, 2, 5, and 7 are product-term endpoints. (Bit slice 7 is an endpoint by default
             - no associated register bit).
   — CFG0: 000 - high level on the selected input (input 1) for bit slice 0
   — CFG1: 000 - high level on the selected input (input 1) for bit slice 1
   — CFG2: 000 - high level on the selected input (input 2) for bit slice 2
   — CFG3: 101 - low level on the selected input (input 2) for bit slice 3
   — CFG4: 101 - low level on the selected input (input 3) for bit slice 4
   — CFG5: 010 - (sticky) falling edge on the selected input (input 6) for bit slice 5
   — CFG6: 000 - high level on the selected input (input 5) for bit slice 6
   — CFG7: 111 - event (any edge, nonsticky) on the selected input (input 7) for bit slice 7
   • Pattern-Match Interrupt Control (PMCTRL)
   — Bit0: Writing this bit selects pattern matches to generate the pin interrupts in place of the standard pin interrupt
        mechanism.
         For this example, detecting a match on the first product term asserts pin interrupt 0 (which, in this case, is just a high
           level on input 1).
         Pin interrupt 2 is asserted in response to a match on the second product term.
         Pin interrupt 5 is asserted when there is a match on the third product term.
         Pin interrupt 7 is asserted on a match on the last term.
   — Bit1: Writing this bit causes the RXEV signal to the CPU getting asserted whenever a match occurs on any of the
         product terms in the expression. Otherwise, the RXEV line does not get used.
   — Bit31:24: At any given time, bits 0, 2, 5, and 7 may be high if the corresponding product terms are currently
         matching.
   — The remaining bits are low.

74.6.2  Pattern-match engine edge-detect examples
The following figures show only pattern-match functionality in which accurate timing is not implied. Inputs (INn) are shown
synchronized to the system clock for simplicity.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3553 / 3791

<!-- page 3554 -->

NXP Semiconductors
                                                                                               Pin Interrupt and Pattern Match (PINT)





                    System clock


                          Slice 0 (IN0re)

                             IN0


                  SRC0 = 0, CFG0 = 3h, PROD_ENDPTS0 = 0b (sticky rising-edge detection)
                                                                                                   minterm (IN0re)(1N1eV)
                                                                                                                  pin interrupt raised on
                          Slice 1 (IN1ev)                                                                                         falling edge of Input1
                                                                                               any time after IN0 has
                             IN1                                                             gone high

                   NVIC pin interrupt 1 and
                  GPIO_INT_BMAT output

                  SRC1 = 1, CFG1 = 7h, PROD_ENDPTS1 = 1b (nonsticky edge detection)

 Figure 441. Pattern-match engine examples: sticky edge detect




                   System clock


                         Slice 0 (IN0)

                             IN0


                  SRC0 = 0, CFG0 = 4h, PROD_ENDPTS0 = 0b (high-level detection)
                                                                                                  minterm (IN0)(1N1eV)
                         Slice 1 (IN1ev)                                                                           pin interrupt raised on
                                                                                                                               rising edge of 1N1 during
                                                                                                             the high level of IN0
                             IN1

                  NVIC pin interrupt 1 and
                  GPIO_INT_BMAT output

                  SRC1 = 1, CFG1 = 7h, PROD_ENDPTS1 = 1b (nonsticky edge detection)

 Figure 442. Pattern-match engine examples: Windowed nonsticky edge-detect evaluates as true


                    System clock


                           Slice 0 (IN0)

                              IN0


                   SRC0 = 0, CFG0 = 4h, PROD_ENDPTS0 = 0b (high-level detection)
                                                                                                    minterm (IN0)(1N1eV)
                           Slice 1 (IN1ev)                                                            no pin interrupt raised
                                                                                    1N1 does not change
                                                                                      when IN0 level is high                              IN1

                   NVIC pin interrupt 1 and
                   GPIO_INT_BMAT output

                   SRC1 = 1, CFG1 = 7h, PROD_ENDPTS1 = 1b (nonsticky edge detection)

 Figure 443. Pattern-match engine examples: Windowed nonsticky edge-detect evaluates as false





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3554 / 3791

<!-- page 3555 -->

NXP Semiconductors
                                                                                               Pin Interrupt and Pattern Match (PINT)

74.7 Memory map and register definition
This section includes the PINT memory map and detailed descriptions of all registers.
The chip includes up to 8 pins identified as interrupt sources by Pin Interrupt Select (PINSELa). The registers have 8 fields that
correspond to the pins called out by PINSEL.

Table 712. Pin interrupt registers for edge- and level-sensitive pins

 Name         Edge-sensitive function                                Level-sensitive function

 IENR        Enables rising-edge interrupts.                     Enables level interrupts.

 SIENR        Write to enable rising-edge interrupts.                 Write to enable level interrupts.

 CIENR        Write to disable rising-edge interrupts.                 Write to disable level interrupts.

 IENF         Enables falling-edge interrupts.                       Selects active level.

 SIENF        Write to enable falling-edge interrupts.                 Write to select high active.

 CIENF        Write to disable falling-edge interrupts.                Write to select low active.

74.7.1  Pin Interrupts and Pattern Match register descriptions

74.7.1.1 PINT memory map
PINT0 base address: 4000_4000h

     Offset     Register                                                        Width    Access   Reset value
                                                                                                     (In bits)

     0h       Pin Interrupt Mode (ISEL)                                       32    RW    See section

     4h       Pin Interrupt Level or Rising-Edge Interrupt Enable (IENR)             32    RW    See section

     8h       Pin Interrupt Level or Rising-Edge Interrupt Set (SIENR)              32    RW    See section

     Ch       Pin Interrupt Level (Rising-Edge Interrupt) Clear (CIENR)              32    RW    See section

     10h      Pin Interrupt Active Level or Falling-Edge Interrupt Enable (IENF)       32    RW    See section

     14h      Pin Interrupt Active Level or Falling-Edge Interrupt Set (SIENF)         32    RW    See section

     18h      Pin Interrupt Active Level or Falling-Edge Interrupt Clear (CIENF)       32    RW    See section

    1Ch      Pin Interrupt Rising Edge (RISE)                                 32    RW    See section

     20h      Pin Interrupt Falling Edge (FALL)                                 32    RW    See section

     24h      Pin Interrupt Status (IST)                                       32    RW    See section

     28h      Pattern-Match Interrupt Control (PMCTRL)                         32    RW    See section

    2Ch      Pattern-Match Interrupt Bit-Slice Source (PMSRC)                   32    RW    See section

     30h      Pattern-Match Interrupt Bit Slice Configuration (PMCFG)              32    RW    See section





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3555 / 3791

<!-- page 3556 -->

NXP Semiconductors
                                                                                               Pin Interrupt and Pattern Match (PINT)

74.7.1.2  Pin Interrupt Mode (ISEL)

Offset


 Register                   Offset

 ISEL                  0h


Function
Determines whether an interrupt is edge- or level-sensitive.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                       PMODE
   W

Reset     u      u      u      u       u      u      u      u       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-8      Read value is undefined; write only 0.
    —

       7-0         Interrupt mode
   PMODE     Selects the interrupt mode for each pin interrupt. Bit nconfigures the pin interrupt selected in PINSEL in
              INPUTMUX.
                     0000_0000b - In bit n configures the interrupt to be edge-sensitive
                     0000_0001b - In bit n configures the interrupt to be level-sensitive


74.7.1.3  Pin Interrupt Level or Rising-Edge Interrupt Enable (IENR)

Offset


 Register                   Offset

 IENR                 4h





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3556 / 3791

<!-- page 3557 -->

NXP Semiconductors
                                                                                               Pin Interrupt and Pattern Match (PINT)

Function
Enables the interrupt by each field in Pin Interrupt Level or Rising-Edge Interrupt Enable (IENR), depending on the ISEL[PMODE]
configured in Pin Interrupt Mode (ISEL).
   •  If the PMODE is edge sensitive (PMODE = 0), the rising-edge interrupt is enabled.
   •  If the PMODE is level sensitive (PMODE = 1), the level interrupt is enabled. Pin Interrupt Active Level or Falling-Edge
     Interrupt Enable (IENF) configures the active level (HIGH or LOW) for this interrupt.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                            ENRL
   W

Reset     u      u      u      u       u      u      u      u       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-8      Read value is undefined; write only 0.
    —

       7-0       Enables Interrupt
    ENRL      Enables the rising edge or level interrupt for each pin interrupt corresponding to the bit index. Bit n
                  configures the pin interrupt selected in PINSELa.
                     0000_0000b - In bit n disables the corresponding interrupt
                     0000_0001b - In bit n enables the corresponding interrupt


74.7.1.4  Pin Interrupt Level or Rising-Edge Interrupt Set (SIENR)

Offset


 Register                   Offset

 SIENR                8h


Function
Configures the corresponding field in Pin Interrupt Level or Rising-Edge Interrupt Enable (IENR) by each field in Pin Interrupt
Level or Rising-Edge Interrupt Set (SIENR), depending on the ISEL[PMODE] configured in Pin Interrupt Mode (ISEL).





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3557 / 3791

<!-- page 3558 -->

NXP Semiconductors
                                                                                               Pin Interrupt and Pattern Match (PINT)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved
   W                                                                        SETENRL

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function
      31-8      Reserved
    —

       7-0       Configures IENR
   SETENRL    Configures IENR by writing 1s to this address. Bit nconfigures the corresponding bit in IENR.
                     0000_0000b - No operation for interrupt n
                     0000_0001b - Enable rising edge or level interrupt for interrupt n


74.7.1.5  Pin Interrupt Level (Rising-Edge Interrupt) Clear (CIENR)

Offset


 Register                   Offset

 CIENR              Ch


Function
Writing 0 to each bit in Pin Interrupt Level (Rising-Edge Interrupt) Clear (CIENR) adjusts the corresponding bit in Pin Interrupt
Level or Rising-Edge Interrupt Enable (IENR) depending on the ISEL[PMODE] configured in Pin Interrupt Mode (ISEL). Writing
0 to this address configures bits in Pin Interrupt Level or Rising-Edge Interrupt Enable (IENR), effectively disabling interrupts.
Writing 0 to bit aconfigures bit nin Pin Interrupt Level or Rising-Edge Interrupt Enable (IENR).





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3558 / 3791

<!-- page 3559 -->

NXP Semiconductors
                                                                                               Pin Interrupt and Pattern Match (PINT)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                                       CENRL
                                  Reserved
   W                                                                  W1C

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function
      31-8      Reserved
    —

       7-0        Clear bits in IENR
   CENRL           0000_0000b - No operation
                     0000_0001b - Disable rising edge or level interrupt


74.7.1.6  Pin Interrupt Active Level or Falling-Edge Interrupt Enable (IENF)

Offset


 Register                   Offset

 IENF                 10h


Function
Enables the falling-edge interrupt or configures the level sensitivity depending on the ISEL[PMODE] configured in Pin Interrupt
Mode (ISEL).
   •  If the PMODE is edge sensitive (PMODE = 0), the falling-edge interrupt is enabled.
   •  If the PMODE is level sensitive (PMODE = 1), the active level of the level interrupt (HIGH or LOW) is configured.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3559 / 3791

<!-- page 3560 -->

NXP Semiconductors
                                                                                               Pin Interrupt and Pattern Match (PINT)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                            ENAF
   W

Reset     u      u      u      u       u      u      u      u       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-8      Reserved
    —

       7-0       Enables Interrupt
    ENAF      Enables the falling edge or configures the active-level interrupt for each pin interrupt. Bit aconfigures the
                   pin interrupt selected in PINSELa.
                     0000_0000b - Disable (set active interrupt level LOW)
                     0000_0001b - Enable (set active interrupt level HIGH)


74.7.1.7  Pin Interrupt Active Level or Falling-Edge Interrupt Set (SIENF)

Offset


 Register                   Offset

 SIENF                14h


Function
Sets the corresponding bit in Pin Interrupt Active Level or Falling-Edge Interrupt Enable (IENF) (by each bit of SIENF),
depending on the PMODE configured in Pin Interrupt Mode (ISEL).
   •  If the PMODE is edge sensitive (PMODE = 0), the falling-edge interrupt becomes 1.
   •  If the PMODE is level sensitive (PMODE = 1), the HIGH-active interrupt is selected.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3560 / 3791

<!-- page 3561 -->

NXP Semiconductors
                                                                                               Pin Interrupt and Pattern Match (PINT)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved
   W                                                                        SETENAF

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function
      31-8      Reserved
    —

       7-0        Write 1 to this address to clear Pin Interrupt Active Level or Falling-Edge Interrupt Enable (IENF) to
                  disable interrupts. Bit asets bit nin IENF.
   SETENAF
                     0000_0000b - Writes 0 to IENF. No operation
                     0000_0001b - Select HIGH-active interrupt or enable falling-edge interrupt


74.7.1.8  Pin Interrupt Active Level or Falling-Edge Interrupt Clear (CIENF)

Offset


 Register                   Offset

 CIENF                18h


Function
Sets the corresponding bit in Pin Interrupt Active Level or Falling-Edge Interrupt Enable (IENF) (by each bit of Pin Interrupt
Active Level or Falling-Edge Interrupt Clear (CIENF), depending on the ISEL[PMODE] configured in Pin Interrupt Mode (ISEL).
   •  If the PMODE is edge sensitive (PMODE = 0), the falling-edge interrupt is cleared.
   •  If the PMODE is level sensitive (PMODE = 1), the LOW-active interrupt is selected.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3561 / 3791

<!-- page 3562 -->

NXP Semiconductors
                                                                                               Pin Interrupt and Pattern Match (PINT)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved
   W                                                                       CENAF

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function
      31-8      Reserved
    —

       7-0        Writes 0 to IENF
   CENAF      Specifies that if you change Pin Interrupt Active Level or Falling-Edge Interrupt Enable (IENF) to 0 by
                    writing 1 to it,this will disable interrupts. Writing 0 to bit aconfigures bit nin Pin Interrupt Active Level or
                  Falling-Edge Interrupt Enable (IENF).
                     0000_0000b - No operation
                     0000_0001b - LOW-active interrupt selected or falling-edge interrupt disabled


74.7.1.9  Pin Interrupt Rising Edge (RISE)

Offset


 Register                   Offset

 RISE                1Ch


Function
Contains 1s for pin interrupts selected in PINSELa (see the "INPUTMUX" chapter for more details) on which a rising edge has
been detected. Writing 1s to this register clears rising-edge detection. Ones in this register assert an interrupt request for pins
that are enabled for rising-edge interrupts. All edges are detected for all pins selected by PINSEL, regardless of whether they
are interrupt-enabled.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3562 / 3791

<!-- page 3563 -->

NXP Semiconductors
                                                                                               Pin Interrupt and Pattern Match (PINT)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                           RDET
   W

Reset     u      u      u      u       u      u      u      u       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-8      Reserved
    —

       7-0       Rising-Edge Detect
    RDET        Bit adetects the rising edge of the pin selected in PINSELa.
                     0000_0000b - Read 0- No rising edge (since Reset or you wrote a 1 to this field last time), Write
                          0- No operation
                     0000_0001b - Read 1- Rising edge (since Reset or you wrote a 1 to this field last time), Write 1-
                        Clear rising-edge detection for this pin


74.7.1.10  Pin Interrupt Falling Edge (FALL)

Offset


 Register                   Offset

 FALL                 20h


Function
Contains 1s for pin interrupts selected in PINSELa on which a falling edge has been detected. Writing 1s to this register clears
falling-edge detection. The values (1s) in this register assert interrupt requests for pins with falling-edge interrupt capabilities.
PINSELa detects all edges for all pins, regardless of whether they are interrupt-enabled.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3563 / 3791

<!-- page 3564 -->

NXP Semiconductors
                                                                                               Pin Interrupt and Pattern Match (PINT)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                             FDET
   W

Reset     u      u      u      u       u      u      u      u       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-8      Reserved
    —

       7-0        Falling-Edge Detect
    FDET        Bit adetects the falling edge of the pin selected in PINSELa.
                     0000_0000b - Read 0- No falling edge (since Reset or you wrote a 1 to this field last time), Write
                          0- No operation
                     0000_0001b - Read 1- Falling edge (since Reset or you wrote a 1 to this field last time), Write 1-
                        Clear falling-edge detection for this bit


74.7.1.11  Pin Interrupt Status (IST)

Offset


 Register                   Offset

 IST                   24h


Function
Returns 1s for pin interrupts that are currently requesting an interrupt. For edge-sensitive pins in Interrupt Select, writing
1s to this register clears both rising- and falling-edge detection for the pin. For level-sensitive pins, writing 1s inverts the
corresponding bit in the active-level register, thus switching the active level on the pin.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3564 / 3791

<!-- page 3565 -->

NXP Semiconductors
                                                                                               Pin Interrupt and Pattern Match (PINT)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                                 Reserved
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                  Reserved                                             PSTAT
   W

Reset     u      u      u      u       u      u      u      u       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-8      Reserved
    —

       7-0        Pin Interrupt Status
    PSTAT       Bit areturns the status, clears the edge interrupt, or inverts the active level of the pin selected in
               PINSELa.
                     0000_0000b - Read 0- Interrupt is not requested, Write 0- No operation
                     0000_0001b - Read 1- Interrupt is requested, Write 1 (edge-sensitive)- clear rising- and falling-
                     edge detection for this pin, Write 1 (level-sensitive)- switch the active level for this pin in Pin
                            Interrupt Active Level or Falling-Edge Interrupt Enable (IENF)


74.7.1.12  Pattern-Match Interrupt Control (PMCTRL)

Offset


 Register                   Offset

 PMCTRL              28h


Function
Contains a field to select pattern-match interrupt generation (as opposed to pin interrupts which share the same interrupt
request lines), and another to enable the RXEV output to the CPU. Read the current state of the pattern match in
this register. If the pattern-match feature is not used (either for interrupt generation or for RXEV assertion), write 0 to
PMCTRL[SEL_PMATCH] and PMCTRL[ENA_RXEV] of this register to conserve power.

                                           NOTE
             Set up the pattern-match configuration in Pattern-Match Interrupt Bit-Slice Source (PMSRC) and Pattern-Match
                Interrupt Bit Slice Configuration (PMCFG) before writing to this register to enable (or reenable) the pattern-match
                 functionality. This prevents spurious interrupts from occurring during the feature"s activation.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3565 / 3791

<!-- page 3566 -->

NXP Semiconductors
                                                                                               Pin Interrupt and Pattern Match (PINT)

                                           NOTE
            The pattern-match feature requires clocks to operate and thus not generate an interrupt or wake the chip during
             reduced power modes below sleep mode.


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                      PMAT

   W                                                                                         Reserved

Reset     0      0      0      0       0      0      0      0       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1       0

    R                                                                                                 ENA_  SEL_P
                                                                                           RXEV    MA...   W                                                   Reserved

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      0       0


Fields


       Field       Function

     31-24       Pattern Matches
    PMAT      Displays the current state of pattern matches.
                     0000_0001b - The corresponding product term is matched by the current state of the appropriate
                          inputs

      23-2       Reserved, will read 0"s
    —

       1        Enables the RXEV output to the CPU and/or to a GPIO output, when the specified Boolean expression
                  evaluates to true. If this value is 0b, RXEV output to the CPU is disabled. If this value is 1b, RXEV output
  ENA_RXEV                    to the CPU is enabled.
                     0b - Disabled
                     1b - Enabled

       0         Specifies whether the pin interrupts are controlled by the pin interrupt function or by the pattern-match
                    function. If this value is 0b, interrupts are driven in response to the standard pin interrupt function. If this
 SEL_PMATCH                 value is 1b, interrupts are driven in response to pattern matches.
                     0b - Pin interrupt
                     1b - Pattern match





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3566 / 3791

<!-- page 3567 -->

NXP Semiconductors
                                                                                               Pin Interrupt and Pattern Match (PINT)

74.7.1.13  Pattern-Match Interrupt Bit-Slice Source (PMSRC)

Offset


 Register                   Offset

 PMSRC              2Ch


Function
Specifies the input source for each of the pattern-match bit slices. Each of the possible 8 inputs is selected in the pin interrupt
select registers in INPUTMUX. Input 0 corresponds to the pin selected in PINSEL0, input 1 corresponds to the pin selected in
PINSEL1, and so on.

                                           NOTE
               Writing any value to either Pattern-Match Interrupt Bit Slice Configuration (PMCFG) or Pattern-Match Interrupt Bit-
                Slice Source (PMSRC), or disabling the pattern-match feature by clearing both the PMCTRL[SEL_PMATCH] and
           PMCTRL[ENA_RXEV] bits in Pattern-Match Interrupt Control (PMCTRL) to zeros will erase all edge-detect history.


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
             SRC7              SRC6               SRC5               SRC4              SRC3        SRC2
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
          SRC2           SRC1               SRC0
   W                                                                                         Reserved

Reset     0      0      0      0       0      0      0      0       u      u      u      u       u      u      u      u


Fields


       Field       Function
  31-29: SRC7   Selects the input source for bit slice n
  28-26: SRC6        000b - Input 0 (selects the pin identified in PINSEL0)
  25-23: SRC5        001b - Input 1 (selects the pin identified in PINSEL1)
  22-20: SRC4        010b - Input 2 (selects the pin identified in PINSEL2)
  19-17: SRC3        011b - Input 3 (selects the pin identified in PINSEL3)
  16-14: SRC2        100b - Input 4 (selects the pin identified in PINSEL4)
  13-11: SRC1        101b - Input 5 (selects the pin identified in PINSEL5)
   10-8: SRC0         110b - Input 6 (selects the pin identified in PINSEL6)
                     111b - Input 7 (selects the pin identified in PINSEL7)

       7-0       Reserved, will read 0's
    —


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3567 / 3791

<!-- page 3568 -->

NXP Semiconductors
                                                                                               Pin Interrupt and Pattern Match (PINT)

74.7.1.14  Pattern-Match Interrupt Bit Slice Configuration (PMCFG)

Offset


 Register                   Offset

 PMCFG               30h


Function
Configures the detect logic and contains bits to select from among eight alternative conditions for each bit slice that cause that bit
slice to contribute to a pattern match. The seven LSBs of this register specify which bit slices are the end-points of product terms
in the Boolean expression (where OR terms are to be inserted in the expression).
Two types of edge detection on each input are possible:
   • Sticky: A rising edge, a falling edge, or a rising or falling edge that is detected at any time after the edge-detection
   mechanism becomes 0. The input qualifies as detected (the detect logic output remains HIGH) until the pattern-match
    engine detect logic becomes 0 again.
   • Nonsticky: In the event of an edge (rise or fall), the detect logic output for this pin goes HIGH. One clock cycle clears this
      bit, allowing the edge detector to detect a second edge.
To clear the pattern-match engine detect logic, write any value to either Pattern-Match Interrupt Bit Slice Configuration
(PMCFG) or Pattern-Match Interrupt Bit-Slice Source (PMSRC), or disable the pattern-match feature by changing both
PMCTRL[SEL_PMATCH] and PMCTRL[ENA_RXEV] to 0 by writing 1 to Pattern-Match Interrupt Control (PMCTRL). This
will erase all edge-detect history.
To select whether a slice marks the final component in a minterm of the Boolean expression, write a 1 in the corresponding
PROD_ENDPTSa bit. Setting a term as the final component has two effects:
   1. Anytime a match to that product term occurs, this leads to an assertion of interrupt request associated with this bit slice.
   2. The next bit slice will start a new, independent product term in the Boolean expression. The Boolean expression will
      include an OR following the element controlled by this bit slice.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
             CFG7              CFG6               CFG5               CFG4              CFG3        CFG2
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6       5      4       3      2       1       0

    R
                                                         PROD  PROD  PROD  PROD  PROD  PROD  PROD
          CFG2           CFG1              CFG0                                                                      Reserv   _EN...   _EN...   _EN...   _EN...   _EN...   _EN...   _EN...   W
                                                                    ed

Reset     0      0      0      0       0      0      0      0       u      0       0      0       0      0       0       0





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3568 / 3791

<!-- page 3569 -->

NXP Semiconductors
                                                                                               Pin Interrupt and Pattern Match (PINT)

Fields


       Field       Function

  31-29: CFG7   Match Configuration
  28-26: CFG6   Specifies the match contribution condition for bit slice n:
  25-23: CFG5       •  If the value is 000b, this bit slice always contributes to a product term match.
  22-20: CFG4       •  If the value is 001b, edge detection for this bit slice is clear when there has been a rising edge
                  on the specified input since last clearing. In order to clear this match condition, you must write to  19-17: CFG3
                     Pattern-Match Interrupt Bit Slice Configuration (PMCFG) or Pattern-Match Interrupt Bit-Slice Source
  16-14: CFG2      (PMSRC).
  13-11: CFG1       •  If the value is 010b, edge detection for this bit slice is clear when there has been a falling edge
                  on the specified input since last clearing. In order to clear this match condition, you must write to   10-8: CFG0
                     Pattern-Match Interrupt Bit Slice Configuration (PMCFG) or Pattern-Match Interrupt Bit-Slice Source
                  (PMSRC).
                           •  If the value is 011b, edge detection for this bit slice is clear when there has been either a rising or a
                           falling edge on the specified input since last clearing. In order to clear this match condition, you must
                        write to Pattern-Match Interrupt Bit Slice Configuration (PMCFG) or Pattern-Match Interrupt Bit-Slice
                   Source (PMSRC).
                           •  If the value is 100b, match for this bit slice occurs when there is a high level on the input specified for
                          this bit slice in Pattern-Match Interrupt Bit-Slice Source (PMSRC).
                           •  If the value is 101b, match occurs when there is a low level on the specified input.
                           •  If the value is 110b, it never contributes to a match and must be used to disable any unused bit
                          slices.
                           •  If the value is 111b, a match occurs when the specified input first detects a rising or falling edge.
                      This is a nonsticky version of value 0 x 3. Write 0 to this field after one clock cycle.
                     000b - Constant HIGH
                     001b - Sticky rising edge
                     010b - Sticky falling edge
                     011b - Sticky rising or falling edge
                     100b - High level
                     101b - Low level
                     110b - Constant 0
                     111b - Event (Nonsticky rising or falling edge)

       7          Bit slice 7 is automatically considered a product endpoint.
    —

       6-0       Determines whether slice n is an endpoint. Slice n is not an endpoint. Slice n is the endpoint of a product
                 term (minterm). Pin interrupt n in the NVIC is raised if the minterm evaluates as true.
 PROD_ENDPT
      Sn            0b - No effect
                     1b - Endpoint





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3569 / 3791
