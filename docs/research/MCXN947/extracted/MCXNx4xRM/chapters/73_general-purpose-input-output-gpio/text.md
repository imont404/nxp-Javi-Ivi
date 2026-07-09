# 73 General Purpose Input/Output (GPIO)

Source: `docs/mcxn947/MCXNx4xRM.pdf`
Pages: 3521-3547

<!-- page 3521 -->

NXP Semiconductors

Chapter 73
General Purpose Input/Output (GPIO)

73.1  Chip-specific GPIO information

Table 704. Reference links to related information

         Topic             Related module                                Reference

      Full description           GPIO                                GPIO

        Clocking                                                        Clock distribution

  Power management                                           Power management

   Signal multiplexing          Port control                                  Signal multiplexing

                                           NOTE
           See Peripheral Reset Control 0 (PRESETCTRL0) to know how to reset GPIO. To enable GPIO clock, see AHB
              Clock Control 0 (AHBCLKCTRL0).

73.1.1 Module instances
This device has six instances of the GPIO module, GPIO0, GPIO1, GPIO2, GPIO3, GPIO4, and GPIO5.

73.1.2  Security considerations
The GPIO module implements Access protection that can be used to configure secure/non-secure and privileged/non-privileged
access on a per pin basis. The interrupt, DMA, or trigger functionality for each pin can also be configured for secure/non-secure
and privileged/non-privileged access.
Each GPIO module is also instantiated to use two module slots (for example, GPIO0 and GPIO0_alias). At the Secure AHB
controller, one slot can be configured for secure access while the other is configured for non-secure access. This allows each
GPIO module to be accessed by both secure and non-secure masters using the appropriate slot, while the Secure AHB controller’s
MISC_CTRL_REG[DISABLE_STRICT] option is configured for strict mode. Each GPIO module’s pin and interrupt, DMA, or trigger
secure/non-secure access protections are then used to restrict access according to the level used by that particular GPIO slot.

73.1.3  Interrupt, DMA request, and trigger outputs
GPIO modules can be used to trigger interrupts, DMA requests, or trigger outputs (see the INPUTMUX and WUU chapters
for details). See the table below for details on the chip-level interrupt, DMA request, and trigger output capability for each
GPIO instantiation:

Table 705. Interrupt, DMA request, and trigger outputs for GPIO

      GPIO module                      Interrupt            DMA request                  Trigger output

    GPIO0/GPIO0_alias1               Yes                     Yes                    No

    GPIO1/GPIO1_alias1               Yes                     Yes                     Yes

    GPIO2/GPIO2_alias1               Yes                     Yes                     Yes

    GPIO3/GPIO3_alias1               Yes                     Yes                     Yes

    GPIO4/GPIO4_alias1               Yes                     Yes                    No

    GPIO5/GPIO5_alias1               Yes                     Yes                     Yes




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3521 / 3791

<!-- page 3522 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)

73.1.4 Power domains/modes
The GPIO module can be implemented in different power domains which has implications for low power mode usage and
wake-up capability. See Table 334 and Table 336 in Power Management chapter for GPIO0/1/2/3/4/5 implementation in different
power modes.

73.1.5 GPIO wakeup via WAKEUP_b pin
The GPIO module can assert two interrupts into the VBAT module’s IRQ0_DET and IRQ1_DET. These interrupts can optionally
be enabled as source to assert the WAKEUP_b pin. When the device is operating in the VBAT power mode, you can transition the
device back to active mode by configuring an external circuitry to recognize the WAKEUP_b assertion and request for a system
power on. See the VBAT chapter for more details.

73.2 Overview
GPIO communicates to the processor core via a zero wait-state interface for maximum pin performance.

73.2.1  Block diagram



                                                          GPIO

                              GPIO0
                                                 GPIO pin
                                                              data control

                              GPIO31


                                                                                             Peripheral bus

                                                                                                     Interrupt, DMA
                                                                           Interrupt            request, or trigger
                                                                         control



 Figure 437. Block diagram

73.2.2  Features
   • Port Data Input (PDIR) displays the logic value on each pin when the pin is configured for any digital function, provided the
    corresponding Port Control and Interrupt modules for that pin are enabled.
   • Port Data Output (PDOR), with corresponding set, clear, and toggle registers, controls the output data of each pin when the
    pin is configured for the GPIO function.
   • Port Data Direction (PDDR) controls the direction of each pin when the pin is configured for the GPIO function.
   • Port Input Disable (PIDR) controls disabling of the input for each general-purpose pin.
   • Pin interrupts:
   — Interrupt flags and enable registers for each pin are functional in all digital pin muxing modes.
   — Support for interrupt, peripheral trigger, or DMA request is configured for each pin.
   — Support for edge-sensitive (rising, falling, or both) or level-sensitive (low, high) interrupts is configured for each pin.
   — Asynchronous wake-up in Low-Power mode.
   — GPIO generates a total of 2 interrupts, 2 output triggers, and 2 DMA requests.
   — Each pin can be used to generate a single interrupt, output trigger, or DMA request.



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3522 / 3791

<!-- page 3523 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)

   • Protection registers:
   — Each pin is configured for secure or nonsecure and privileged or nonprivileged access.
   — Each interrupt, trigger, and DMA request domain is configured for secure or nonsecure and privileged or
         nonprivileged access.

73.3  Functional description

73.3.1 Low-Power mode
You can configure GPIO to exit Low-Power mode via an asynchronous wake-up signal if an enabled interrupt is detected.

73.3.2 Debug mode
GPIO remains functional in Debug mode.

73.3.3  General-purpose input
The logic state of each pin is available via Port Data Input (PDIR) if:
   • The corresponding field in Port Input Disable (PIDR) is 0.
   • The pin is configured for a digital function.

73.3.4  General-purpose output
The logic state of each pin is controlled via Port Data Output (PDOR) and Port Data Direction (PDDR), provided the pin is
configured for the GPIO function. The following table depicts the conditions for a pin to be configured as input or output.

Table 706. General-purpose output

 If                                                Then

 A pin is configured for the GPIO function and the corresponding The pin is configured as an input.
 PDDR field is 0

 A pin is configured for the GPIO function and the corresponding The pin is configured as an output and the logic state of the pin
 PDDR field is 1                                                           is equal to the corresponding PDOR field.

For efficient bit manipulation on the general-purpose outputs, pin data set, pin data clear, and pin data toggle registers allow one or
more outputs within one port to be set, cleared, or toggled from a single register write, eliminating the need for a read-modify-write
operation to prevent changing pins accidentally.

73.3.5  Clocking
GPIO receives a single clock, which is used for register access and synchronization with external pin inputs. There are no
special considerations.

73.3.6 Reset
GPIO receives a single reset, which resets the peripheral. There are no special considerations.

73.3.7  External interrupts
The external interrupt capability of GPIO is available in all digital pin muxing modes.
GPIO generates a total of 2 interrupts, 2 output triggers, and 2 DMA requests. You can configure each pin individually for interrupt,
output trigger, or DMA requests:
   • Each output implements a separate interrupt status flag (ISF) register for that domain.


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3523 / 3791

<!-- page 3524 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)

   • Each output generates a single interrupt.
   • Each output generates a single DMA request.
   • Each output generates a single peripheral trigger output.
You can configure each pin individually for any of the external interrupt modes shown in the following table.

Table 707. Available pin configurations for external interrupts

 Signal conditions        Software polling using     Peripheral triggers            Interrupts        DMA requests
                                     flags

 Rising-edge                  Yes          —                 Yes                Yes

 Falling-edge                 Yes          —                 Yes                Yes

 Rising- and falling-             Yes          —                 Yes                Yes
 edge

 High-level           —                 Yes                 Yes          —

 Low-level            —                 Yes                 Yes          —

The interrupt status flag is set when the configured edge or level is detected on the pin. Unless GPIO is in Low-Power mode, the
input is first synchronized to the system clock to detect the configured level or edge transition.
GPIO generates a pin interrupt that asserts when the interrupt status flag is set for any enabled interrupt for that output. The
interrupt negates after the interrupt status flags for all enabled interrupts are cleared by writing a logic 1 to either ISFR0[ISFn]
or ICR0[ISF].
GPIO generates a DMA request that asserts when the interrupt status flag is set for any enabled DMA request in that output.
The DMA request negates after the DMA transfer is completed because that clears the interrupt status flags for all enabled
DMA requests.
In Low-Power mode, the interrupt status flag for any enabled interrupt is asynchronously set if the required level or edge is
detected. This also generates an asynchronous wake-up signal to exit Low-Power mode.
GPIO generates a peripheral trigger output that asserts if any pin configured for the active-high trigger is logic 1, or any pin
triggered for the active-low trigger is logic 0. The peripheral trigger output asynchronously updates from the value on the
configured pins.

73.3.8  Global interrupt control
The two global interrupt control registers (Global Interrupt Control Low (GICLR) and Global Interrupt Control High (GICHR)) allow
a single register write to update the upper 16 bits of Interrupt Control a (ICR0 - ICR31) for up to 16 pins, all with the same value.
These global interrupt control registers allow you to quickly configure multiple pins within the same port with the same
interrupt configuration.
The global interrupt control registers are write-only registers and always read 0.

73.3.9 DMA
GPIO generates 2 requests. For each pin, you can configure DMA requests to assert for either rising, falling, or both edge
detection. See External interrupts for more information.

73.3.10 Access protection
GPIO implements access protection registers (PCNS and PCNP) for each pin, as shown in Table 708.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3524 / 3791

<!-- page 3525 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)

Table 708. PCNS and PCNP register access protection implementation

 Access                        Description

 Secure                  You can write to or read the pins configured for secure access only in the Secure state.

 Nonsecure               You can write to or read the pins configured for nonsecure access only in the Nonsecure state.

 Privilege                 You can write to the pins configured for privilege access only in the Privilege state.

 Nonprivilege              You can write to the pins configured for nonprivilege access in both the Privilege and
                               Nonprivilege states.

GPIO implements access protection registers (ICNS and ICNP) for each interrupt, trigger output, and DMA request as shown in
Table 709.

Table 709. ICNS and ICNP register access protection implementation

 Access                        Description

 Secure                  You can configure the outputs programmed for secure access only in the Secure state.

 Nonsecure               You can configure the outputs programmed for nonsecure access only in the Nonsecure state.

 Privilege                 You can configure the outputs programmed for privilege access only in the Privilege state.

 Nonprivilege              You can configure the outputs programmed for nonprivilege access in both the Privilege and
                               Nonprivilege states.

You can write to the access protection registers (PCNS, PCNP, ICNS, and ICNP) only in the Secure-Privilege state; you can
optionally lock these registers until the next reset.
Configuring a pin interrupt, trigger output, or DMA request requires the following access permissions:
   • Access permission to configure the pin.
   • Access permission to either configure to the desired interrupt, trigger output, or DMA request, or writing 0 to ICRn[IRQC].
   • Access permission to either configure with the existing interrupt, trigger output, or DMA request, or if the current value of
    ICRn[IRQC] is 0.
   • The interrupt configuration is not locked.

73.4  External signals

Table 710. External signals

        Signal                                               Description                                          Direction
   GPIO31–GPIO0                               General-purpose input/output                                   I/O
                          State meaning     Asserted: The pin is logic 1.
                                          Deasserted: The pin is logic 0.

                            Timing         Assertion: When output, this signal occurs on the rising-edge of the
                                       system clock. For input, it may occur at any time and input may be
                                           asserted asynchronously to the system clock.
                                            Deassertion: When output, this signal occurs on the rising-edge of
                                             the system clock. For input, it may occur at any time and input may
                                     be asserted asynchronously to the system clock.




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3525 / 3791

<!-- page 3526 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)

                                           NOTE
             Not all pins within each GPIO are implemented on each chip. See the "Signal Multiplexing" chapter for the number
                of GPIO pins within each port available on this chip.

73.5  Initialization
To initialize GPIO:
   1.  Initialize the GPIO pins for the output function:
          a. Configure the output logic value for each pin by using Port Data Output (PDOR).
          b. Configure the direction for each pin by using Port Data Direction (PDDR).
   2.  Initialize the interrupt function by writing to Interrupt Control a (ICR0 - ICR31) for the corresponding pins and desired
       configuration. If the pin is previously used for a different function, first write 0100_0000h to Interrupt Control a (ICR0 - ICR31)
       to disable the previous function and clear the flag.

73.6  Application information
GPIO includes the following applications:
   • Reading the state of a single pin by performing a byte read of Pin Data (P0DR - P31DR).
   • Updating the state of a single pin by performing a byte write to Pin Data (P0DR - P31DR).
   • Reading the state of multiple pins by reading Port Data Input (PDIR).
   • Updating the state of multiple pins by using the following ways:
   — Writing to Port Data Output (PDOR).
   — Writing to Port Set Output (PSOR) to write 1 to Port Data Output (PDOR).
   — Writing to Port Clear Output (PCOR) to write 0 to Port Data Output (PDOR).
   — Writing to Port Toggle Output (PTOR) to toggle Port Data Output (PDOR).

73.7 Memory map and register definition
The GPIO registers support 8-bit, 16-bit, or 32-bit accesses. Any read or write access to the GPIO memory space that is outside
the valid memory map results in a bus error.

                                           NOTE
              For simplicity, each GPIO port's register appears with the same width of 32 bits, corresponding to 32 pins. The
               actual number of pins per port (and therefore the number of usable control bits per port register) is chip-specific.
           See the "Signal Multiplexing" chapter for the exact control bits of each port.

73.7.1 GPIO register descriptions

73.7.1.1 GPIO memory map
GPIO0 base address: 4009_6000h
GPIO0_alias1 base address: 4009_7000h
GPIO1 base address: 4009_8000h
GPIO1_alias1 base address: 4009_9000h
GPIO2 base address: 4009_A000h
GPIO2_alias1 base address: 4009_B000h
GPIO3 base address: 4009_C000h


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3526 / 3791

<!-- page 3527 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)

GPIO3_alias1 base address: 4009_D000h
GPIO4 base address: 4009_E000h
GPIO4_alias1 base address: 4009_F000h
GPIO5 base address: 4004_0000h
GPIO5_alias1 base address: 4004_1000h

     Offset     Register                                                        Width    Access   Reset value
                                                                                                     (In bits)

     0h       Version ID (VERID)                                           32     R     0201_0001h

     4h      Parameter (PARAM)                                           32     R     0000_0002h

     Ch      Lock (LOCK)                                                 32    RW    0000_0000h

     10h      Pin Control Nonsecure (PCNS)                                  32    RW    0000_0000h

     14h       Interrupt Control Nonsecure (ICNS)                               32    RW    0000_0000h

     18h      Pin Control Nonprivilege (PCNP)                                 32    RW    0000_0000h

    1Ch       Interrupt Control Nonprivilege (ICNP)                              32    RW    0000_0000h

     40h       Port Data Output (PDOR)                                       32    RW    0000_0000h

     44h       Port Set Output (PSOR)                                        32    W     0000_0000h

     48h       Port Clear Output (PCOR)                                      32    W     0000_0000h

    4Ch      Port Toggle Output (PTOR)                                     32    W     0000_0000h

     50h       Port Data Input (PDIR)                                         32     R     0000_0000h

     54h       Port Data Direction (PDDR)                                     32    RW    0000_0000h

     58h       Port Input Disable (PIDR)                                       32    RW    0000_0000h

   60h - 7Fh    Pin Data (P0DR - P31DR)                                       8    RW        00h

  80h - FCh    Interrupt Control a (ICR0 - ICR31)                                32    RW    0000_0000h

    100h      Global Interrupt Control Low (GICLR)                             32    W     0000_0000h

    104h      Global Interrupt Control High (GICHR)                             32    W     0000_0000h

  120h - 124h   Interrupt Status Flag (ISFR0 - ISFR1)                             32    RW    0000_0000h

73.7.1.2  Version ID (VERID)

Offset


 Register                   Offset

 VERID                0h


Function
Indicates the version ID number of each GPIO.




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3527 / 3791

<!-- page 3528 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                      MAJOR                                            MINOR

   W

Reset     0      0      0      0       0      0      1      0       0      0      0      0       0      0      0      1

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                               FEATURE

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      1


Fields


       Field       Function

     31-24      Major Version Number
   MAJOR      Indicates the major version number for the specification.

     23-16      Minor Version Number
    MINOR      Indicates the minor version number for the specification.

      15-0       Feature Specification Number
   FEATURE     Indicates the feature set number.
                     0000_0000_0000_0000b - Basic implementation
                     0000_0000_0000_0001b - Protection registers implemented


73.7.1.3  Parameter (PARAM)

Offset


 Register                   Offset

 PARAM               4h


Function
Indicates the interrupt number of each GPIO.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3528 / 3791

<!-- page 3529 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                             0                                             IRQNUM

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      1      0


Fields


       Field       Function
      31-4      Reserved
    —

       3-0         Interrupt Number
   IRQNUM     Indicates the number of interrupt, trigger, or DMA request domains.


73.7.1.4  Lock (LOCK)

Offset


 Register                   Offset

 LOCK              Ch


Function
Locks the nonsecure and nonprivilege access protection registers. You can write to this register only in the Secure-Privilege
state.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2       1       0

    R                                            0
                                                                                          ICNP  PCNP   ICNS  PCNS
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0       0       0



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3529 / 3791

<!-- page 3530 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)

Fields


       Field       Function
      31-4      Reserved
    —

       3        Lock ICNP
     ICNP      Locks Interrupt Control Nonprivilege (ICNP). If this field is 0, you can write to ICNP in the Secure-Privilege
                     state. If this field is 1, you cannot write to ICNP until the next reset.
                     0b - Writable in Secure-Privilege state
                     1b - Not writable until the next reset

       2        Lock PCNP
    PCNP      Locks Pin Control Nonprivilege (PCNP). If this field is 0, you can write to PCNP in the Secure-Privilege state.
                              If this field is 1, you cannot write to PCNP until the next reset.
                     0b - Writable in Secure-Privilege state
                     1b - Not writable until the next reset

       1        Lock ICNS
     ICNS      Locks Interrupt Control Nonsecure (ICNS). If this field is 0, you can write to ICNS in the Secure-Privilege
                     state. If this field is 1, you cannot write to ICNS until the next reset.
                     0b - Writable in Secure-Privilege state
                     1b - Not writable until the next reset

       0        Lock PCNS
    PCNS      Locks Pin Control Nonsecure (PCNS). If this field is 0, you can write to PCNS in the Secure-Privilege state.
                              If this field is 1, you cannot write to PCNS until the next reset.
                     0b - Writable in Secure-Privilege state
                     1b - Not writable until the next reset


73.7.1.5  Pin Control Nonsecure (PCNS)

Offset


 Register                   Offset

 PCNS                 10h


Function
Configures secure or nonsecure access protection for each pin. You can write to this register only in the Secure-Privilege state
if it is not locked (LOCK[PCNS] = 0).





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3530 / 3791

<!-- page 3531 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
       NSE31  NSE30  NSE29  NSE28  NSE27  NSE26  NSE25  NSE24  NSE23  NSE22  NSE21  NSE20  NSE19  NSE18  NSE17  NSE16
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
       NSE15  NSE14  NSE13  NSE12  NSE11  NSE10  NSE9  NSE8  NSE7  NSE6  NSE5  NSE4  NSE3  NSE2  NSE1  NSE0
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function

      31-0      Nonsecure Enable
    NSEn      Configures secure or nonsecure access protection for each pin. If this field is 0, the pin is configured
                     for secure access. You can read or write to the corresponding pin's registers and fields only in the
                Secure state. When you access the corresponding pin's registers in the Nonsecure state, all fields in
                  the registers related to that pin are read zero (with writes ignored). If this field is 1, the pin is configured
                     for nonsecure access. You can read or write to the corresponding pin's registers and fields only in the
                Nonsecure state. When you access the corresponding pin's registers in the Secure state, all fields in the
                    registers related to that pin are read zero (with writes ignored).
                     0b - Secure access
                     1b - Nonsecure access


73.7.1.6  Interrupt Control Nonsecure (ICNS)

Offset


 Register                   Offset

 ICNS                 14h


Function
Configures secure and nonsecure access protection for each interrupt, output trigger, or DMA request. You can update this
register only in the Secure-Privilege state if it is not locked (LOCK[ICNS] = 0).





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3531 / 3791

<!-- page 3532 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1       0

    R                                                    0
                                                                                               NSE1  NSE0
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0       0


Fields


       Field       Function
      31-2      Reserved
    —

       1-0       Nonsecure Enable
    NSEn      Configures secure or nonsecure access protection for each interrupt, output trigger, or DMA request.
                              If this field is 0, the interrupt, output trigger, or DMA request is configured for secure access. You can
                  configure a pin to use the corresponding interrupt, output trigger, or DMA request, or reconfigure a pin
                    that is already configured to use the corresponding interrupt, output trigger, or DMA request only in the
                Secure state. If this field is 1, the interrupt, output trigger, or DMA request is configured for nonsecure
                 access. You can configure a pin to use the corresponding interrupt, output trigger, or DMA request, or
                  reconfigure a pin that is already configured to use the corresponding interrupt, output trigger, or DMA
                  request only in the Nonsecure state.

                                                 NOTE
                           See the GPIO chip-specific information to determine which GPIO instances support
                                        interrupt, DMA request, or trigger capabilities.

                     0b - Secure access
                     1b - Nonsecure access


73.7.1.7  Pin Control Nonprivilege (PCNP)

Offset


 Register                   Offset

 PCNP                 18h


Function
Configures access protection on each pin either for privilege or for both privilege and nonprivilege accesses. You can update
this register only in the Secure-Privilege state if it is not locked (LOCK[PCNP] = 0).




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3532 / 3791

<!-- page 3533 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
       NPE31  NPE30  NPE29  NPE28  NPE27  NPE26  NPE25  NPE24  NPE23  NPE22  NPE21  NPE20  NPE19  NPE18  NPE17  NPE16
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
       NPE15  NPE14  NPE13  NPE12  NPE11  NPE10  NPE9  NPE8  NPE7  NPE6  NPE5  NPE4  NPE3  NPE2  NPE1  NPE0
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function

      31-0       Nonprivilege Enable
    NPEn      Configures privilege or nonprivilege access protection for each pin. If this field is 0, the pin is configured
                     for privilege access. Write access to the corresponding pin's registers and fields is allowed only in the
                    Privilege state. When you access the corresponding pin's registers and fields in the Nonprivilege state,
                         all fields related to that pin in this GPIO are readable (with writes ignored). If this field is 1, the pin is
                  configured for nonprivilege access; read or write access to the corresponding pin's registers is allowed in
                 both Privilege and Nonprivilege states.
                     0b - Privilege access
                     1b - Nonprivilege access


73.7.1.8  Interrupt Control Nonprivilege (ICNP)

Offset


 Register                   Offset

 ICNP                1Ch


Function
Configures privilege and nonprivilege access protection for each interrupt, trigger output, or DMA request. You can update this
register only in the Secure-Privilege state if it is not locked (LOCK[ICNP] = 0).





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3533 / 3791

<!-- page 3534 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1       0

    R                                                    0
                                                                                               NPE1  NPE0
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0       0


Fields


       Field       Function
      31-2      Reserved
    —

       1-0        Nonprivilege Enable
    NPEn      Configures privilege or nonprivilege access protection for each interrupt, trigger output, or DMA request.
                              If this field is 0, the pin is configured for privilege access. You can configure a pin to use the
                 corresponding interrupt, trigger output, or DMA request, or reconfigure a pin that is already configured to
                use the corresponding interrupt, trigger output, or DMA request only in the Privilege state. If this field is 1,
                  the pin is configured for nonprivilege access. In either Privilege or Nonprivilege state, you can configure
               a pin to use the corresponding interrupt, trigger output, or DMA request, or reconfigure a pin that is
                  already configured to use the corresponding interrupt, trigger output, or DMA request.

                                                 NOTE
                           See the GPIO chip-specific information to determine which GPIO instances support
                                        interrupt, DMA request, or trigger capabilities.

                     0b - Privilege access
                     1b - Nonprivilege access


73.7.1.9  Port Data Output (PDOR)

Offset


 Register                   Offset

 PDOR                40h


Function
Configures the logic levels that are driven on each general-purpose output pin.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3534 / 3791

<!-- page 3535 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)

                                           NOTE
           Do not modify the pin configuration registers associated with pins that are not available in your selected package.
            By default, these unbonded pins are set to the Disable state for lowest power consumption.


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R  PDO3  PDO3  PDO2  PDO2  PDO2  PDO2  PDO2  PDO2  PDO2  PDO2  PDO2  PDO2  PDO1  PDO1  PDO1  PDO1
   W    1      0      9      8      7      6      5      4      3      2      1      0      9      8      7      6

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R  PDO1  PDO1  PDO1  PDO1  PDO1  PDO1
                                           PDO9  PDO8  PDO7  PDO6  PDO5  PDO4  PDO3  PDO2  PDO1  PDO0
   W    5      4      3      2      1      0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function

      31-0       Port Data Output
    PDOn      Configures the logic level on the pin if it is configured for general-purpose output. If this field is 0, logic
                    level 0 is driven on the pin, if the pin is configured for general-purpose output. If this field is 1, logic level
               1 is driven on the pin, if the pin is configured for general-purpose output.

                                                 NOTE
                                        Reading the fields for unbonded pins returns an undefined value.

                     0b - Logic level 0
                     1b - Logic level 1


73.7.1.10  Port Set Output (PSOR)

Offset


 Register                   Offset

 PSOR                44h


Function
Updates the corresponding fields of Port Data Output (PDOR) to become 1.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3535 / 3791

<!-- page 3536 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R    0      0      0      0      0      0      0      0      0      0      0      0      0      0      0      0

       PTSO  PTSO  PTSO  PTSO  PTSO  PTSO  PTSO  PTSO  PTSO  PTSO  PTSO  PTSO  PTSO  PTSO  PTSO  PTSO
   W
          31     30     29     28     27     26     25     24     23     22     21     20     19     18     17     16

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R    0      0      0      0      0      0      0      0      0      0      0      0      0      0      0      0

       PTSO  PTSO  PTSO  PTSO  PTSO  PTSO  PTSO  PTSO  PTSO  PTSO  PTSO  PTSO  PTSO  PTSO  PTSO  PTSO
   W
          15     14     13     12     11     10      9      8      7      6      5      4      3      2      1      0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function

      31-0       Port Set Output
    PTSOn     Updates the content of the corresponding field in Port Data Output (PDOR). If this field is 0, the
                 corresponding PDOR field does not change. If this field is 1, the corresponding PDOR field becomes 1.
                     0b - No change
                     1b - Corresponding field in PDOR becomes 1


73.7.1.11  Port Clear Output (PCOR)

Offset


 Register                   Offset

 PCOR                48h


Function
Updates the corresponding fields of Port Data Output (PDOR) to become 0.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3536 / 3791

<!-- page 3537 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R    0      0      0      0      0      0      0      0      0      0      0      0      0      0      0      0

       PTCO  PTCO  PTCO  PTCO  PTCO  PTCO  PTCO  PTCO  PTCO  PTCO  PTCO  PTCO  PTCO  PTCO  PTCO  PTCO
   W
          31     30     29     28     27     26     25     24     23     22     21     20     19     18     17     16

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R    0      0      0      0      0      0      0      0      0      0      0      0      0      0      0      0

       PTCO  PTCO  PTCO  PTCO  PTCO  PTCO  PTCO  PTCO  PTCO  PTCO  PTCO  PTCO  PTCO  PTCO  PTCO  PTCO
   W
          15     14     13     12     11     10      9      8      7      6      5      4      3      2      1      0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function

      31-0       Port Clear Output
    PTCOn     Updates the content of the corresponding field in Port Data Output (PDOR). If this field is 0, the
                 corresponding PDOR field does not change. If this field is 1, the corresponding PDOR field becomes 0.
                     0b - No change
                     1b - Corresponding field in PDOR becomes 0


73.7.1.12  Port Toggle Output (PTOR)

Offset


 Register                   Offset

 PTOR               4Ch


Function
Updates the corresponding fields of Port Data Output (PDOR) to set to the inverse of their current logic states.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3537 / 3791

<!-- page 3538 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R    0      0      0      0      0      0      0      0      0      0      0      0      0      0      0      0

       PTTO  PTTO  PTTO  PTTO  PTTO  PTTO  PTTO  PTTO  PTTO  PTTO  PTTO  PTTO  PTTO  PTTO  PTTO  PTTO
   W
          31     30     29     28     27     26     25     24     23     22     21     20     19     18     17     16

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R    0      0      0      0      0      0      0      0      0      0      0      0      0      0      0      0

       PTTO  PTTO  PTTO  PTTO  PTTO  PTTO  PTTO  PTTO  PTTO  PTTO  PTTO  PTTO  PTTO  PTTO  PTTO  PTTO
   W
          15     14     13     12     11     10      9      8      7      6      5      4      3      2      1      0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function

      31-0       Port Toggle Output
    PTTOn     Updates the content of the corresponding field in Port Data Output (PDOR). If this field is 0, the
                 corresponding PDOR field does not change. If this field is 1, the corresponding PDOR field is set to the
                  inverse of its current logic state.
                     0b - No change
                     1b - Set to the inverse of its current logic state


73.7.1.13  Port Data Input (PDIR)

Offset


 Register                   Offset

 PDIR                 50h


Function
Captures the logic levels of each general-purpose input pin.

                                           NOTE
           Do not modify the pin configuration registers associated with the pins that are not available in your selected
             package. By default, these unbonded pins are set to the Disable state for lowest power consumption.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3538 / 3791

<!-- page 3539 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R  PDI31  PDI30  PDI29  PDI28  PDI27  PDI26  PDI25  PDI24  PDI23  PDI22  PDI21  PDI20  PDI19  PDI18  PDI17  PDI16

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R  PDI15  PDI14  PDI13  PDI12  PDI11  PDI10   PDI9   PDI8   PDI7   PDI6   PDI5   PDI4   PDI3   PDI2   PDI1   PDI0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function

      31-0       Port Data Input
     PDIn       Indicates the logic level of the pin that is configured for use by a digital function. If this field is 0, the pin
                    logic level is logic 0 or is not configured or implemented for use by a digital function. If this field is 1, the
                   pin logic level is logic 1.
                     0b - Logic 0
                     1b - Logic 1


73.7.1.14  Port Data Direction (PDDR)

Offset


 Register                   Offset

 PDDR                54h


Function
Configures the individual port pins for input or output.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R  PDD3  PDD3  PDD2  PDD2  PDD2  PDD2  PDD2  PDD2  PDD2  PDD2  PDD2  PDD2  PDD1  PDD1  PDD1  PDD1
   W    1      0      9      8      7      6      5      4      3      2      1      0      9      8      7      6

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R  PDD1  PDD1  PDD1  PDD1  PDD1  PDD1
                                            PDD9  PDD8  PDD7  PDD6  PDD5  PDD4  PDD3  PDD2  PDD1  PDD0
   W    5      4      3      2      1      0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3539 / 3791

<!-- page 3540 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)

Fields


       Field       Function

      31-0       Port Data Direction
    PDDn      Configures individual port pins for input or output. If this field is 0, the pin is configured as general-
                purpose input for the GPIO function. If this field is 1, the pin is configured as general-purpose output for
                  the GPIO function.
                     0b - Input
                     1b - Output


73.7.1.15  Port Input Disable (PIDR)

Offset


 Register                   Offset

 PIDR                 58h


Function
Disables the input for each general-purpose pin, which prevents the value from being reported in Port Data Input (PDIR).

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
        PID31  PID30  PID29  PID28  PID27  PID26  PID25  PID24  PID23  PID22  PID21  PID20  PID19  PID18  PID17  PID16
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
        PID15  PID14  PID13  PID12  PID11  PID10   PID9   PID8   PID7   PID6   PID5   PID4   PID3   PID2   PID1   PID0
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function

      31-0       Port Input Disable
     PIDn       Disables a pin for general-purpose input. If this field is 0, the pin is configured for general-purpose
                     input, provided the pin is configured for a digital function. If this field is 1, the pin is disabled for
                 general-purpose input.
                     0b - Configured for general-purpose input
                     1b - Disabled for general-purpose input





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3540 / 3791

<!-- page 3541 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)

73.7.1.16  Pin Data (P0DR - P31DR)

Offset
For a = 0 to 31:


 Register                   Offset

 PaDR                 60h + (a × 1h)


Function
Configures the data feature of a pin. Pins that are unimplemented or not configured for a digital function read zero.

                                           NOTE
           You must not modify Pin Data (P0DR - P31DR) associated with the pins that are not available in your selected
             package. These unbonded pins are, by default, set to the Disable state for lowest power consumption.


Diagram

 Bits                 7             6             5             4             3             2             1             0

         R                                                 0
                                                                                              PD
       W

Reset               0             0             0             0             0             0             0             0


Fields


       Field       Function
       7-1       Reserved
    —

       0        Pin Data (I/O)
     PD        Specifies the pin logic level. This field updates the corresponding field in Port Data Output (PDOR);
                  reading this field returns the value in the corresponding field of Port Data Input (PDIR). If this field is 0,
                  the pin logic level is logic zero or not configured for use by a digital function. If this field is 1, the pin logic
                    level is logic one.
                     0b - Logic zero
                     1b - Logic one


73.7.1.17  Interrupt Control a (ICR0 - ICR31)

Offset
For a = 0 to 31:


 Register                   Offset

 ICRa                 80h + (a × 4h)



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3541 / 3791

<!-- page 3542 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)

Function
Configures interrupt features on each pin.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                          0                              ISF                 0
                                                               LK                 IRQS             IRQC
   W                                         W1C

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-25      Reserved
    —

      24         Interrupt Status Flag
      ISF        Indicates whether the configured interrupt is detected. The pin interrupt configuration is valid in all digital pin
                muxing modes.
               The fields in Interrupt Status Flag (ISFR0 - ISFR1) have the same function. ISF can be cleared with either
                    register field.
                              If the pin is configured to generate a DMA request, then the corresponding flag is cleared automatically at
                  the completion of the requested DMA transfer. Otherwise, the flag remains set until a logic 1 is written to the
                      flag. If the pin is configured for a level-sensitive interrupt and the pin remains asserted, then the flag is set
                 again immediately after it is cleared.

                                                 NOTE
                                                This field behaves differently for register reads and writes.

             When reading
                     0b - Not detected
                     1b - Detected
             When writing
                     0b - No effect
                     1b - Clear the flag

      23       Lock
      LK

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3542 / 3791

<!-- page 3543 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                Locks ICR[23:0], and the locked field cannot be updated until the next system reset. If this field is
                    0, interrupt configuration by ICR[23:0] is not locked and can be updated. If this field is 1, interrupt
                   configuration by ICR[23:0] is locked and cannot be updated until the next system reset.
                     0b - Lock
                     1b - Do not lock

     22-21      Reserved
    —

      20         Interrupt Select
     IRQS      Configures the selected interrupt, trigger output, or DMA request.

                                                 NOTE
                           See the GPIO chip-specific information to determine which GPIO instances support
                                        interrupt, DMA request, or trigger capabilities.

                     0b - Interrupt, trigger output, or DMA request 0
                     1b - Interrupt, trigger output, or DMA request 1

     19-16       Interrupt Configuration
    IRQC       Specifies the ISF and DMA request configuration. The pin interrupt configuration is valid in all digital pin
                muxing modes. When changing the interrupt configuration, it is recommended to first disable ISF and
                 then write the new configuration. The corresponding pin is configured to generate interrupt, trigger, or
           DMA request.

                                                 NOTE
                           See the GPIO chip-specific information to determine the GPIO instances that support
                                        interrupt, DMA request, or trigger capabilities.

                     0000b - ISF is disabled
                     0001b - ISF and DMA request on rising edge
                     0010b - ISF and DMA request on falling edge
                     0011b - ISF and DMA request on either edge
                     0100b - Reserved
                     0101b - ISF sets on rising edge
                     0110b - ISF sets on falling edge
                     0111b - ISF sets on either edge
                     1000b - ISF and interrupt when logic 0
                     1001b - ISF and interrupt on rising edge
                     1010b - ISF and interrupt on falling edge
                     1011b - ISF and Interrupt on either edge

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3543 / 3791

<!-- page 3544 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1100b - ISF and interrupt when logic 1
                     1101b - Enable active-high trigger output; ISF on rising edge (pin state is ORed with other
                       enabled triggers to generate the output trigger for use by other peripherals)
                     1110b - Enable active-low trigger output; ISF on falling edge (pin state is inverted and ORed with
                         other enabled triggers to generate the output trigger for use by other peripherals)
                     1111b - Reserved

      15-0      Reserved
    —


73.7.1.18  Global Interrupt Control Low (GICLR)

Offset


 Register                   Offset

 GICLR                100h


Function
Updates any combination of the lower 16 Interrupt Control registers with the same value. This register supports only 32-bit
writes and ignores any 16-bit or 8-bit writes.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W                                               GIWD

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R    0      0      0      0      0      0      0      0      0      0      0      0      0      0      0      0

       GIWE  GIWE  GIWE  GIWE  GIWE  GIWE  GIWE  GIWE  GIWE  GIWE  GIWE  GIWE  GIWE  GIWE  GIWE  GIWE
   W
          15     14     13     12     11     10      9      8      7      6      5      4      3      2      1      0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function

     31-16      Global Interrupt Write Data
    GIWD

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3544 / 3791

<!-- page 3545 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                   Indicates the write value that is written to the upper 16 bits of Interrupt Control a (ICR0 - ICR31), selected
                by GIWE.

      15-0       Global Interrupt Write Enable
    GIWEn      Indicates whether the upper 16 bits of the corresponding Interrupt Control a (ICR0 - ICR31) are updated
                   with the value in GIWD.
                     0b - Not updated
                     1b - Updated


73.7.1.19  Global Interrupt Control High (GICHR)

Offset


 Register                   Offset

 GICHR                104h


Function
Updates any combination of the upper 16 Interrupt Control registers with the same value. This register supports only 32-bit
writes and ignores any 16-bit or 8-bit writes.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W                                               GIWD

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R    0      0      0      0      0      0      0      0      0      0      0      0      0      0      0      0

       GIWE  GIWE  GIWE  GIWE  GIWE  GIWE  GIWE  GIWE  GIWE  GIWE  GIWE  GIWE  GIWE  GIWE  GIWE  GIWE
   W
          31     30     29     28     27     26     25     24     23     22     21     20     19     18     17     16

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function

     31-16      Global Interrupt Write Data
    GIWD       Indicates the write value that is written to the upper 16 bits of Interrupt Control a (ICR0 - ICR31), selected
                by GIWE.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3545 / 3791

<!-- page 3546 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

      15-0       Global Interrupt Write Enable
    GIWEn      Indicates whether the upper 16 bits of the corresponding Interrupt Control a (ICR0 - ICR31) are updated
                   with the value in GIWD.
                     0b - Not updated.
                     1b - Updated


73.7.1.20  Interrupt Status Flag (ISFR0 - ISFR1)

Offset


 Register                   Offset

 ISFR0                120h

 ISFR1                124h


Function
Indicates whether the related configured interrupt is detected on each pin. The pin interrupt configuration is valid in all digital pin
muxing modes. The ISF for each pin is also visible in the corresponding Interrupt Control register, and each flag can be cleared
in either location.
There is a separate ISF register for each interrupt, trigger, or DMA request domain. Each status flag is only visible in the register
that corresponds to that flag's domain, as configured in ICRn[IRQS].

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R  ISF31   ISF30   ISF29   ISF28   ISF27   ISF26   ISF25   ISF24   ISF23   ISF22   ISF21   ISF20   ISF19   ISF18   ISF17   ISF16

   W  W1C   W1C   W1C   W1C   W1C   W1C   W1C   W1C   W1C   W1C   W1C   W1C   W1C   W1C   W1C   W1C

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R  ISF15   ISF14   ISF13   ISF12   ISF11   ISF10   ISF9    ISF8    ISF7    ISF6    ISF5    ISF4    ISF3    ISF2    ISF1    ISF0

   W  W1C   W1C   W1C   W1C   W1C   W1C   W1C   W1C   W1C   W1C   W1C   W1C   W1C   W1C   W1C   W1C

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function

      31-0        Interrupt Status Flag
      ISFn

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3546 / 3791

<!-- page 3547 -->

NXP Semiconductors
                                                                                           General Purpose Input/Output (GPIO)


       Field       Function

                   Indicates the detection of the configured interrupt on each pin of the same number. If this field is 0, the
                  configured interrupt is not detected on the pin of the same number. If this field is 1, the configured interrupt
                       is detected on the pin of the same number. If the pin is configured to generate a DMA request, then the
                 corresponding flag is cleared automatically at the completion of the requested DMA transfer. Otherwise, the
                     flag remains set until a logic 1 is written to the flag. If the pin is configured for a level-sensitive interrupt and
                  the pin remains asserted, then the flag is set again immediately after it is cleared.

                                                 NOTE
                                                This field behaves differently for register reads and writes.

             When reading
                     0b - Not detected
                     1b - Detected
             When writing
                     0b - No effect
                     1b - Clear the flag





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   3547 / 3791
