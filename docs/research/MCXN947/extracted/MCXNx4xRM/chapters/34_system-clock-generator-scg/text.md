# 34 System Clock Generator (SCG)

Source: `docs/mcxn947/MCXNx4xRM.pdf`
Pages: 1216-1307

<!-- page 1216 -->

NXP Semiconductors

Chapter 34
System Clock Generator (SCG)

34.1  Chip-specific SCG information

Table 322. Reference links to related information

         Topic             Related module                                Reference

      Full description         SCG                            SCG

  System memory map                                           Memory map

        Clocking                                                        Clock distribution

   Signal multiplexing          Port control                                  Signal multiplexing

34.1.1 Module instances
This device contains one instance of the SCG module, SCG0.

34.1.2  Security considerations
For secure applications, NXP recommends that this module is configured for secure and privileged access only to prevent
unintentional or malicious modification of the system operation by nonsecure software. The secure AHB controller controls the
security level for access to peripherals and does default to secure and privileged access for all peripherals.

34.1.3  Crystal oscillator clock sources
In this device, the ROSC is sourced from Crystal Oscillator 32.768 kHz clock in VBAT block, and the SOSC is sourced from System
Crystal Oscillator Clock in the SCG block.

34.1.4 SIRCCSR[SIRCSTEN] power domain
SIRCCSR[SIRCSTEN] is enabled when the WAKE domain is in Deep Sleep mode. When the WAKE domain is in Deep Sleep
mode, PMCTRLWAKE[LPMODE]=0x1.

34.1.5  Clock name decoder ring
The table below shows translation of clock name sources used in the SCG chapter to the names used throughout the document.

Table 323. Clock name decoder ring

             Clock source specified in chapter                            Decoded clock name

                     APLL                                         PLL0_CLK

                     SPLL                                         PLL1_CLK

                  SOSC                                           CLK_IN

                     SIRC                                      FRO_12M

                      FIRC                                      FRO_HF

                 ROSC                                        XTAL32K

                    UPLL                                      USB_PLL_CLK




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1216 / 3791

<!-- page 1217 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

34.2 Overview
The System Clock Generator (SCG) provides the main clock and additional peripheral clocks for the MCU. SCG takes clock inputs
from a variety of sources and generates the clocks that the MCU requires.


                                    Chip




                                               Clock sources        SCG           Chip clocks





 Figure 163. System Clock Generator (SCG) in MCU





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1217 / 3791

<!-- page 1218 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

34.2.1  Block diagram



                                     SIRC                                   sirc_1mhz
                                                                 1/12
                                            auto trim
                                                                   SIRC_ CLK_PERIPH_EN

                                                                                                      Clock       sirc_12mhz
                                     SIRC
                                                                                                                 gating

                                                               FIRC_FCLK_PERIPH_EN

                                                                                                      Clock      firc_144mhz
                                                                                                                 gating

                                                               FIRC_SCLK_PERIPH_EN

                                                                                                      Clock       firc_48mhz
                              FIRC                                                                     gating
                                   auto trim
                                            RANGE
                                                                  12 MHz             SCS

                                                    1
                              FIRC       144 MHz                                           2
                                                                              mux                                                                                             3                  firc clock
                                         48 MHz             Range
                                                    0

             ROSC                                                                       4

                                               ROSC Feq. monitor
                  XTAL                                                                     1             sosc_clk
                          SOSC
                 EXTAL

                                                SOSC Feq. monitor

                                                                                                     usb_pll_clk
               UPLL                                                                       7          main clock
                                                           0                                                   mux
                                                                                         MUX                                                SCS
                                                                                         CLK                               pll0_clk  5                pll0_clk                                                                        APLL(PLL0)
                                                                                         REF


                                                      firc_48mhz
                                                           1


                                                         APLL source

                                                           0
                                                                                         MUX
                                                                                         CLK          SPLL(PLL1)      pll1_clk  6                pll1_clk
                                                                                         REF


                                                      firc_48mhz
                                                           1


                                                         SPLL source



                            LDO
                                                                       SCG


 Figure 164. Block diagram



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1218 / 3791

<!-- page 1219 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

34.2.2  Features
Key features of SCG are:
   • Auxiliary phase-locked loop (APLL) clock and system phase-locked loop (SPLL) clock. See PLL features and benefits and
    Functional features for details.
   — Can be selected as the clock source for system clocks
   • Two internal reference clock (IRC) generators with trim capability: FIRC and SIRC
   — FIRC can output 144 MHz or 48 MHz clock by range select and can be used as a source for the APLL and SPLL.
   — SIRC can output 12 MHz clock.
   — Either of the IRC generators can be selected as the clock source for the system clocks.
   • System crystal oscillator clock (SOSC)
   — Can be used as a source for the APLL and SPLL.
   — Can be selected as the clock source for system clocks.
   • Real-time oscillator clock (ROSC)
   — Can be selected as the clock source for the device. To ascertain which signal the ROSC input maps to, see the
          chip-specific information.
   • Clock monitor with reset and interrupt request capability for SOSC, ROSC, APLL, SPLL, and UPLL clocks
   • Low dropout regulator (LDO) used as the supply for the PLL and SOSC
   — Configurable output voltage (1 V to 1.25 V)
   — Wide supply range (1.71 V up to 3.63 V)
   — Wide load current range (up to 8 mA)
   — Low noise, high Power Supply Rejection Ratio (PSRR)
   — Bypass mode

34.3  Functional description

34.3.1 SCG clock mode transitions
The system boots up from the FIRC source. The system clock can be switched between the following clock sources: FIRC, SIRC,
SOSC, ROSC, APLL, SPLL, and UPLL.
SCG restricts programming into invalid clock modes, and ignores writes to the System Clock Source (SCS) fields. When a
transition between Run modes or a transition from Run mode into Wait mode occurs, SCG switches to the corresponding clock
mode as defined in the Run Clock Control Register (RCCR). After the switch is complete, the system switches to the selected Run
or Wait mode.
The following table shows the SCG modes of operation.

Table 324. SCG modes of operation

       Mode                                                     Description

 SOSC            SCG enters SOSC mode when the following occurs:
                                    • The device is in Run mode and 0001 is written to RCCR[SCS]
                                    • SOSCCSR[SOSCEN] = 1b

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1219 / 3791

<!-- page 1220 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Table 324. SCG modes of operation (continued)

       Mode                                                     Description

                                    • SOSCCSR[SOSCVLD] = 1b
                            In SOSC mode, the system clocks are derived from the external SOSC.

 SIRC            SCG enters SIRC mode when the following occurs:
                                    • The device is in Run mode and 0010 is written to RCCR[SCS]
                                    • SIRCCSR[SIRCVLD] = 1b
                            In SIRC mode, the system clocks are derived from SIRC.

 FIRC               FIRC mode is the default clock mode of operation. SCG enters FIRC mode when the following occurs:
                                    • The device is in Run mode and 0011 is written to RCCR[SCS]
                                    • FIRCCSR[FIRCEN] = 1b
                                    • FIRCCSR[FIRCVLD] = 1b
                            In FIRC mode, the system clocks are derived from FIRC. Two frequency range settings are available
                              for FIRC clock, as described in FIRCCFG[RANGE]. Changes to FIRC range settings are ignored when
                         the FIRC clock is enabled.

 ROSC           SCG enters ROSC mode when the following occurs:
                                    • The device is in Run mode and 0100 is written to RCCR[SCS]
                                    • ROSCCSR[ROSCVLD] = 1b
                                    • LDOCSR[LDOEN] = 1b and LDOCSR[VOUT_OK] = 1b
                            In ROSC mode, the system clocks are derived from the external ROSC.

 APLL            SCG enters APLL mode when the following occurs:
                                    • The device is in Run mode and 0101 is written to RCCR[SCS]
                                    • APLLCSR[APLLCLKEN] = 1b
                                    • APLLCSR[APLLPWREN] = 1b
                                    • APLLCSR[APLL_LOCK] = 1b
                                    • LDOCSR[LDOEN] = 1b and LDOCSR[VOUT_OK] = 1b
                            In APLL mode, the system clocks are derived from the output of the PLL which is controlled by either
                         the FIRC or SOSC. The selected PLL clock frequency locks to a multiplication factor (as specified by
                                   its corresponding MDIV) times the selected PLL reference frequency. The programmable reference
                           divider of the PLL must be configured to produce a valid PLL reference clock. This divide value is
                         defined by the NDIV bits.

 SPLL            SCG enters SPLL mode when the following occurs:
                                    • The device is in Run mode and 0110 is written to RCCR[SCS]
                                    • SPLLCSR[SPLLCLKEN] = 1b
                                    • SPLLCSR[SPLLPWREN] = 1b
                                    • SPLLCSR[SPLL_LOCK] = 1b

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1220 / 3791

<!-- page 1221 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Table 324. SCG modes of operation (continued)

       Mode                                                     Description

                                    • LDOCSR[LDOEN] = 1b and LDOCSR[VOUT_OK] = 1b
                            In SPLL mode, the system clocks are derived from the output of PLL which is controlled by either
                         the FIRC or SOSC. The selected PLL clock frequency locks to a multiplication factor (as specified by
                                   its corresponding MDIV) times the selected PLL reference frequency. The programmable reference
                           divider of the PLL must be configured to produce a valid PLL reference clock. This divide value is
                         defined by the NDIV bits.

 UPLL            SCG enters UPLL mode when the following occurs:
                                    • The device is in Run mode and 0111 is written to RCCR[SCS]
                                    • UPLLCSR[UPLLVLD] = 1b
                            In UPLL mode, the system clocks are derived from the output of UPLL.

 Stop             SCG enters Stop mode whenever the CORE domain enters a low power, state retention, or power
                   down state. Power modes are chip-specific. For power mode assignments, see Power Management
                        chapter that describes module operation in low-power modes. All SCG clock signals are static when
                          entering Stop mode, including SCG system clocks (except clocks explicitly configured to continue
                        running and are able to continue running in the target power mode).
                      There are some exceptions. The following clocks can continue to run and stay enabled when all the
                         respective conditions listed below are true.
                                    • SIRC is available in Deep Sleep mode when all the following conditions are true:
              — SIRCCSR[SIRCSTEN] = 1b
                                    • FIRC is available in Deep Sleep mode when all the following conditions are true:
              — FIRCCSR[FIRCEN] = 1b
              — FIRCCSR[FIRCSTEN] = 1b
                                    • SOSC is available in Deep Sleep mode when all the following conditions are true:
              — SOSCCSR[SOSCEN] = 1b
              — SOSCCSR[SOSCSTEN] = 1b
              — LDOCSR[LDOEN] = 1b and LDOCSR[VOUT_OK] = 1b
                                    • APLL is available in Deep Sleep mode when all the following conditions are true:
              — APLLCSR[APLLCLKEN] = 1b
              — APLLCSR[APLLPWREN] = 1b
              — APLLCSR[APLLSTEN] = 1b
              — LDOCSR[LDOEN] = 1b and LDOCSR[VOUT_OK] = 1b
                                    • SPLL is available in Deep Sleep mode when all the following conditions are true:
              — SPLLCSR[SPLLCLKEN] = 1b
              — SPLLCSR[SPLLPWREN] = 1b
              — SPLLCSR[SPLLSTEN] = 1b
              — LDOCSR[LDOEN] = 1b and LDOCSR[VOUT_OK] = 1b




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1221 / 3791

<!-- page 1222 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

34.3.2 SCG SOSC operation
The following table lists the SOSC configurations and SOSC output clock.

Table 325. SOSC mode operation

          SOSCEN                      EREFS                    SOSC clock

 0                                 0                                         Off

 1                                 0                             EXTAL Pad Clock

 1                                 1                                         Crystal Oscillator Clock

34.3.3 SCG LOC operation
SCG supports the loss of clock (LOC) monitoring for the external reference clocks, including SOSC and ROSC. To enable the
LOC monitor, the CMRE field for each of these clocks must be programmed to 1. To monitor these external reference clocks, the
SCG uses an internal 31.25KHz reference clock derived by dividing the SIRC internal clock. You must program the SIRC to be
enabled prior to enabling the LOC for either of these external reference clocks.

                                           NOTE
                                                                 Fint = SIRC divided down to 31.25 KHz

When enabled, the LOC monitors generate a system reset or system interrupt request depending on the CMRE field settings (see
the CMRE description for each of the clocks for further information). The frequency conditions for the external reference clocks
that trigger a LOC event are listed below:
   • ROSC loss of external clock minimum frequency
   — When ROSC clock frequency is above (3/5) Fint, the chip is never reset and will not generate an interrupt.
   — When ROSC clock frequency is between (2/5) Fint and (3/5) Fint, the chip might reset or generate an interrupt, depending
        on the phase dependency between the ROSC clock and the reference clock.
   — When ROSC clock frequency is below (2/5) Fint, the chip always resets or generates an interrupt.
   • SOSC loss of external clock minimum frequency
   — When SOSC clock frequency is above (16/5) Fint, the chip is never reset and will not generate an interrupt.
   — When SOSC clock frequency is between (15/5) Fint and (16/5) Fint, the chip might reset or generate an interrupt,
        depending on the phase dependency between the SOSC clock and the reference clock.
   — When SOSC clock frequency is below (15/5) Fint, the chip always resets or generates an interrupt.

34.3.4 APLL and SPLL operation
APLL and SPLL have similar functions and registers.
PLL is typically used to create a frequency that is higher than other on-chip clock sources and to operate both the CPU and
other on-chip functions. You can also use it to obtain a specific clock that is otherwise not available. For example, a source clock
with a frequency of any integer MHz (such as the 12 MHz FIRC) can be divided down to 1 MHz, then multiplied up to any other
integer MHz (such as 13, 14, and 15).
PLL can be set up by calling an API supplied by NXP Semiconductors. See APLL and SPLL registers in SCG register descriptions
for more information.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1222 / 3791

<!-- page 1223 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

                                           NOTE
           APLL and SPLL have similar registers. Any references to the register or field names of these registers, without
                 prefix "A" or "S", apply for both registers. For example, PLL_LOCK refers to both APLLCSR[APLL_LOCK]
            and SPLLCSR[SPLL_LOCK].

34.3.4.1  General description
PLL is a multipurpose phase-locked loop (PLL). It generates a CMOS-compatible clock and can be used in many applications,
including as a frequency synthesizer and clock for digital circuits, and analog-to-digital and digital-to-analog converters.

PLL includes several normal modes, spread spectrum mode, fractional mode, and a power-down mode. These are summarized
in Table 326 and detailed in the following sections.

Table 326. PLL operating mode summary
 Mode           PLLCSR[PLLP PLLSSCG1[SS_ PLLCTRL[LIM  PLLSSCG1[SEL  PLLSSCG1[M M comes from
              WREN]       PD]          UPOFF]      _SS_MDIV]        R[2:0]]

 Normal          1            1              0            0              0              MDIV[15:0]

 Fractional        1            0              1            1              0             SS_MDIV[32:0]

 Spread spectrum  1            0              1              x              >0            MDIV[15:0] or
                                                                                           SS_MDIV[32:0]
 Power-down      0            1               x              x                x              x


34.3.4.2  Abbreviations

Table 327. Abbreviations and notations

 Acronym        Description

 PLL           Phase-locked loop

 PFD          Phase frequency detector

 CCO           Current control oscillator

 PSR          Power supply rejection

 EMI            Electromagnetic interferences

 SSCG         Spread spectrum clock generation

 clkin            Input clock to the PLL
 Fin            Frequency of clkin
 clkout          Output clock of the PLL
 Fout           Frequency of clkout, Fout = Fcco / (2 * P)
                         • Fout = Fcco / (2 * P) (The divide-by-2 divider in the postdivider is not bypassed.)
                         • Fout = Fcco / P (Bypass divide-by-2 divider in the postdivider.)

 clkref         PLL reference clock. The input clock to the PFD.
 Fref            Frequency of clkref, Fref = Fin / N

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1223 / 3791

<!-- page 1224 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Table 327. Abbreviations and notations (continued)

 Acronym        Description
 Fcco           Frequency of output clock of the CCO, Fcco = M * Fref
 N               Predivider value

M            Feedback divider value

 P               Postdivider value
 tpon          PLL start-up time

34.3.4.3 PLL block diagram





                                                                                                                                     &         clkout
                                                                                                            Q
                       clkin                N-divider                 &       PFD          Filter     CCO               P-divider        &        ÷2           D

                                             1





                                                          &
        PLLPWREN                                                                          M-divider





          LIMUPOFF

       BANDDIRECT


                                                                                                                          PL550M
                                                                                                                                                   NDIV[7:0]     NREQNDIVACK                                              BYPASSPREDIV       clkrefo                               clkfbo              SELR[3:0]  SELI[5:0]  SELP[4:0] MREQ  MACK                MDIV[15:0]                                                                       PDIV[4:0]     PREQPACK                                                                                                                                                                         BYPASSPOSTDIV2 BYPASSPOSTDIV     PLLCLKEN
                                                                                                                                                        clk

                                                                              SSCG wrapper
                                               ~SEL_SS_MDIV                     1   0


                                                                            1   0      ΣΔ modulator

                                                                                 1



                                                  SS_PD

                                                                                                                                                          MREQ       DITHER           MDIV[15:0]                 SS_MDIV[32:0]    MF[2:0]    MR[2:0]   MC[1:0]                    SS_MDIV_REQ       SS_MDIV_ACK

 Figure 165. PLL block diagram


34.3.4.4 PLL normal mode

34.3.4.4.1 PLL features and benefits
   • Integrated PLL with no external components for clock generation


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1224 / 3791

<!-- page 1225 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

   • Large input range at the phase detector (Fref)
   • Programmable:
   — Predivider N (N, 1 to 28-1)
   — Feedback-divider M (M, 1 to 216-1)
   — Postdivider P * 2 (P, 1 to 25-1)
   • Programmable bandwidth (integrating action, proportional action, high frequency pole)
   • On-the-fly adjustment of the clock dividers with handshake control
   • Positive edge clocking
   • Frequency limiter to avoid PLL hang up
   • Power down mode
   • Support for bypassing the predivider and the postdivider
   • Support for bypassing the divide-by-2 in the postdivider
   • Support for disabling the output clock

34.3.4.4.2 PLL functional description

34.3.4.4.2.1  Basic functions
PLL is designed to generate a CMOS compatible clock. PLL can also be used to filter clocks or to make spread spectrum and
fractional clocks. The following sections detail these features.

34.3.4.4.2.2  Main blocks
The PLL block diagram is shown in Figure 165. The clock input must be fed to clkin; clkout is the PLL clock output. The analog
part of the PLL consists of a phase frequency detector (PFD), a filter, and a current controlled oscillator (CCO). The PFD has two
inputs: a reference input from the (divided) external clock and one input from the divided CCO output clock. The PFD compares
the phase and frequency of these input signals and generates a control signal if they do not match. This control signal is fed to a
filter which drives the CCO.
PLL contains three programmable dividers: predivider (N), feedback-divider (M), and postdivider (P). Every divider contains a bus
(XDIV[n:0] in which X is N, M, or P) to load a divider ratio. The dividers also possess the handshake signals, XREQ, and XACK,
to select a new divider ratio. See Selecting divider ratios for more information on selecting the divider ratios.
PLL is stable after startup time tpon. There is a hardware timer in the reference clock domain in SCG to count this time. You must
configure the LOCK_TIME register to (500 μs/Tref + 300). When the number of reference clocks reaches LOCK_TIME after power
on or reconfiguration, the flag PLL_LOCK is set. If the interrupt enable bit PLL_LOCK_IE is set, a PLL_LOCK interrupt is generated.

                                           NOTE
            The PLL clock is gated off when PLL_LOCK is low, regardless of the state of the PLLCLKEN field. At start, set
          PLLPWREN and PLLCLKEN to 1 to enable the PLL. After tpon, the PLL_LOCK is valid and the PLL clock is
               automatically ungated.

To avoid frequency hang-up, the PLL contains a frequency limiter. This feature is built in to prevent the CCO from running too fast.
This can occur if, for example, a wrong feedback-divider (M) ratio is applied to the PLL.
There are additional dividers in the clocking system to bring the PLL output frequency down to what is needed for the CPU
and peripherals.

34.3.4.4.3 PLL application information
For more information on the selectable bandwidth, input clocks, and divider ratios of the PLL, see the following sections:


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1225 / 3791

<!-- page 1226 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

   • Selecting bandwidth
   • Input clock
   • Selecting divider ratios
PLL has eight modes:
   • Mode 1: Normal operating mode
   • Mode 2: Reserved
   • Mode 3: Power down mode (PLLPWREN)
   • Mode 4: Reserved
   • Mode 5: Reserved
   • Mode 6: Reserved
   • Mode 7: Reserved
   • Mode 8: Enable mode (PLLCLKEN).

34.3.4.4.3.1  Selecting bandwidth
For some applications, such as filtering of the input signal, it can be advantageous to change the bandwidth of the PLL.
In normal applications, you must calculate the bandwidth manually by using the feedback divider M (ranging from 1 to 216-1),
Equation 1, and Equation 2. The PLL is automatically stable in such case. In normal applications, APLLCTRL[BANDDIRECT] and
SPLLCTRL[BANDDIRECT] must be 0; in this case, the bandwidth changes as a function of M.


                                                    A = floor (M ÷ 4) + 1

                                                           A   if A < 31
                                                        SELP =                                                                         { 31  if A ≥ 31

 Equation 1. SELP


                                                                    1               if M ≥ 8000
                                             A =    floor (8000 ÷ M)    if 8000 > M ≥ 122                                                    {2 × floor (M ÷ 4)+ 3 if 122 > M ≥ 1

                                                  A if A < 63
                                                     SELI =                                                             { 63 if A ≥ 63

 Equation 2. SELI

SELR must be 0.

                                           NOTE
                In some applications, you may prefer to change the bandwidth directly on the PLL. In such an application, you must
                write 1 to APLLCTRL[BANDDIRECT] or SPLLCTRL[BANDDIRECT].


34.3.4.4.3.2  Input clock
A good-quality input clock is necessary for good PLL performance. The input clock on clkin, optionally divided by the predivider
ratio N, gives the reference frequency Fref for the PLL loop (Fref = Fin / N).
Use SOURCE[1:0] to select one of two inputs to the PLL: SOSC and FIRC. The input clock must have an accurate frequency
before powering up the PLL. You can confirm a valid clock for SOSC and FIRC by using SOSCCSR[SOSCVLD] and
FIRCCSR[FIRCACC], respectively.
For an optimal performance, use a reference frequency within the frequency range 5 MHz - 50 MHz. The total possible reference
frequency range is specified in the data sheet.



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1226 / 3791

<!-- page 1227 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

34.3.4.4.3.3  Selecting divider ratios
The PLL contains three dividers: predivider (N), feedback-divider (M), and postdivider (P). The divider ratios can be selected in
different ways.
Start up
For a correct start up, a low to high transition of PLLPWREN is required after the supplies are stable. The divider ratio data
XDIV[n:0] (in which X is N, M, or P) must also be stable (see Figure 166) before PLLPWREN is made high.



        XDIV[n:0], SCG register, X=M,N,P             NEW


                                                               Transparent if PLLPWREN is low


                   XDIV[n:0], PLL internal                    NEW





                   PLLPWREN


 Figure 166. Loading of the divider ratios at power down

Changing divider ratio after starting up
There are two ways to change the divider ratio after the PLL start up: you can reset the PLL or you can read the divider
ratio asynchronous.
When changing divider ratios, you must avoid forbidden divider ratio combinations at all times in order to keep the CCO frequency
Fcco within the specified range. See the product data sheet for the range information. If you need to change more than one divider
ratio, it is preferable to use the reset method.
Resetting the PLL
You can select the new divider ratios by making PLLPWREN low. After power down, the new divider ratios at the input buses
XDIV[n:0] (in which X is N, M, or P) are loaded into the dividers (Figure 166). You must ensure that the divider ratio data XDIV[n:0]
is stable before the PLLPWREN is made high (PLLPWREN=1). This can be done by resetting the PLL (PLLPWREN=0) until the
divider ratio data XDIV[n:0] is stable.
Reading the divider ratio asynchronous
You can select the divider ratio of the different dividers (M, N, P) on-the-fly with the help of a handshake protocol (Figure 167).
First, a new divider ratio has to be put at the divider input bus (XDIV), and after that a request (XREQ=1) must be given. As soon
as the divider gives acknowledgment (XACK=1), the new divider ratio is loaded and the request can be made low (XREQ=0).
For a fractional application, write 1 to SEL_SS_MDIV. Use SS_MDIV_REQ and SS_MDIV[32:0].

                                           NOTE
                                        If the N divider is changed, you should also update the LOCK_TIME according to the new N.

                                           NOTE
             Updating divider ratio will clear PLL_LOCK until tpon (defined in LOCK_TIME) is passed. Since PLL_LOCK=0 will
              gate off the PLL clock before on-the-fly update, you should switch the system to another clock source.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1227 / 3791

<!-- page 1228 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)



                                                                                               tpon



                      PLL_LOCK





        XDIV[n:0], SCG register, X=M,N,P             NEW





                       XREQ





                        XACK




                   XDIV[n:0], PLL internal                            NEW




                  PLLPWREN=1

 Figure 167. Handshake protocol for asynchronous update divider ratio


34.3.4.4.3.4 Mode 1: Normal operating mode
Typical operation of the PLL includes the following sequence to produce the PLL output:
   1. An optional predivider of the PLL input.
   2. A frequency multiplication.
   3. An optional postdivider.
You can select the predivider and postdivider (see Table 328) to produce:
   • Mode 1a: Normal operating mode without postdivider and without predivider
   • Mode 1b: Normal operating mode with postdivider and without predivider
   • Mode 1c: Normal operating mode without postdivider and with predivider
   • Mode 1d: Normal operating mode with postdivider and with predivider
To produce the best phase-noise and jitter performance at the output of the PLL, you must use the highest possible reference clock
(clkref) at the PFD. Therefore, NXP recommends modes 1a and 1b, if you can make the right output frequency without a predivider.
By using the postdivider and the divide-by-2 divider, the divider ratio of the PLL clock output (clkout) is even when
APLLCTRL[BYPASSPOSTDIV2] or SPLLCTRL[BYPASSPOSTDIV2] is 0. For uneven divider ratios, the divide-by-2 divider
can be bypassed when the BYPASSPOSTDIV2 field is 1.

                                           NOTE
                                                            If BYPASSPOSTDIV2 is 1, the duty cycle of clkout cannot be 50%.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1228 / 3791

<!-- page 1229 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Table 328. Modes within the normal operating mode

 Mode 1                        BYPASSPREDIV                 BYPASSPOSTDIV

 Mode 1a                           1                                 1

 Mode 1b                           1                                 0

 Mode 1c                           0                                 1

 Mode 1d                           0                                 0

Table 329 shows the terms that the equations in this section use.

Table 329. Mode 1nequation terms

 Term                                    Description                           Allowed values
M                                Feedback divider                    1 to 216-1
 N                                        Predivider                         1 to 28-1
 P                                        Postdivider                         1 to 25-1

Mode 1a: Normal operating mode without postdivider and without predivider
Mode 1a bypasses the postdivider and predivider. Equation 3 shows the operating frequency:


                                                                    Fout = FCCO = M × Fin


 Equation 3. Operating frequency in mode 1a

Mode 1b: Normal operating mode with postdivider and without predivider
Mode 1b bypasses the predivider. Equation 4 shows the operating frequency:

                                                                           Fcco  M
                                                                         Fout =    =       Fin                                                         2×P  2×P×
 Equation 4. Operating frequency in mode 1b

Mode 1c: Normal operating mode without postdivider and with predivider
Mode 1c bypasses the postdivider with divide-by-2. Equation 5 shows the operating frequency:


                                             M
                                                                          Fout = Fcco =  × Fin                                                        N
 Equation 5. Operating frequency in mode 1c

Mode 1d: Normal operating mode with postdivider and with predivider
Mode 1d does not bypass either divider. Equation 6 shows the operating frequency:

                                                                          Fcco   M
                                                                       Fout =    =          Fin                                                        2×P  N×2×P×
 Equation 6. Operating frequency in mode 1d





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1229 / 3791

<!-- page 1230 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

34.3.4.4.3.5 Mode 3: Power down mode (PLLPWREN)
If the PLL is not used, or if it is turned off in a running application, power can be saved by putting the PLL in power down mode
(PLLPWREN=0). Before this is done, the CPU and any peripherals that are not meant to be stopped must be running from a
different clock source.
In power down mode, the oscillator stops and the output clkout will be low.
While in PLL power down mode, the PLL_LOCK will be low to indicate that the PLL is not in lock. When the PLL power down mode
is terminated by setting the PLLPWREN field to one, the PLL will resume its normal operation and will make the PLL_LOCK high
after the start up time.

34.3.4.4.3.6 Mode 8: Enable mode (PLLCLKEN)
In Enable mode, the output clock (clkout) of the PLL is enabled. If PLLCLKEN=0, the output of the PLL is low (clkout=0). Ensure
that no spikes occur at the output of the PLL (clkout) by switching into and out of the Enable mode.
Going out of Enable mode can be used for safely switching to other modes, such as power down mode and direct output
(BYPASSPOSTDIV). This method ensures that no spikes can occur at the output.

34.3.4.5 PLL spread spectrum and fractional modes
The SSCG wrapper is a ΣΔ modulator for the multi-purpose PLL and makes it a fractional-N PLL.
By controlling the feedback divider of the PLL, the wrapper can also generate a spread spectrum clock. SSCG decreases
electromagnetic interferences (EMI).

34.3.4.5.1  Functional features
   • Handshake circuit to read the asynchronous SS_MDIV[32:0] at the wrapper
   • Twenty-five fractional bits
   • Preferred reference frequency Fref > 3 MHz
   • Center-spread modulation (default)
   • Modulation waveform control
   • Modulation waveform dithering
   • Power-down mode (SS_PD)
   • Triangular-wave modulation frequency:

                                                                                       Fref
                                                              fm =
                                                                           Nss
                                                                      with Nss ranging from 16 to 512

     Equation 7. Triangular-wave modulation frequency

   • Frequency-modulation depth:




     Equation 8. Frequency-modulation depth


34.3.4.5.2  Functional description





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1230 / 3791

<!-- page 1231 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

34.3.4.5.2.1  Basic functions
The SSCG WRAPPER is a second order ΣΔ modulator for the multipurpose PLL and makes it a fractional-N PLL. By controlling
the feedback divider of the PLL, the wrapper can also generate a spread spectrum clock (Spread Spectrum Clock Generator).
The main block is the ΣΔ modulator which generates the appropriate MDIV[15:0] values from the input word SS_MDIV[32:0] to
have at the output (clkout) of the PLL the correct frequency.

34.3.4.5.2.2  Main blocks
The SSCG WRAPPER consist of:
   • A handshake circuit to read a new input word on the buses: SS_MDIV[32:0], MR[2:0], MF[2:0], MC[2:0]
   • A waveform generator
   • A second order ΣΔmodulator
   • A multiplexer to bypass the wrapper
The SSCG WRAPPER will generate by default a center spread triangular waveform. It is also possible to generate a down spread
triangular waveform which is explained Center spread versus down spread.

34.3.4.5.2.3  Handshake circuit
A new (asynchronous) input word (SS_MDIV[32:0], MF[2:0], MR[2:0], and MC[1:0]) can be read with the help of a handshake
protocol (Figure 168).
First the new word must be put on the bus. Then a request (SS_MDIV_REQ=1) must be given. As soon as the acknowledge pin
goes high (SS_MDIV_ACK=1), the new input word is loaded and the request can be made low (SS_MDIV_REQ=0).


                                                                                                              tpon



                                  PLL_LOCK





    SS_MDIV[32:0]/MF[2:0]/MR[2:0]/MC[1:0], SCG register             NEW





                              SS_MDIV_REQ





                              SS_MDIV_ACK





   SS_MDIV[32:0]/MF[2:0]/MR[2:0]/MC[1:0], SSCG internal                             NEW




                              PLLPWREN=1

 Figure 168. Handshake protocol of the input words SS_MDIV[32:0], MF[2:0], MR[2:0] and MC[1:0]


                                           NOTE
             Updating the divider ratio will clear the PLL_LOCK until tpon (defined in LOCK_TIME) is passed. Because
           PLL_LOCK=0 will gate off the PLL clock, system should be switched to another clock source before on-the-
                    fly updates.



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1231 / 3791

<!-- page 1232 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

34.3.4.5.2.4  Center spread versus down spread
By default, the wrapper generates a triangular waveform with center spread. This means that the average CCO frequency
between MR[2:0] = 0 (no spread spectrum) and MR[2:0] > 0 is the same. see Equation 9.


                                                                Maverage, center spread = Maverage


 Equation 9. Divider equivalence

Some applications require a triangular waveform with down spread. For down spread, instead of the average CCO frequency
being the same, the maximum frequency must be the same in spread spectrum mode (MR[2:0]>0). For down spread, the average
feedback divider ratio must be decreased depending on the modulation depth. See Equation 10 and Equation 11.


                                                          Maverage, down spread = Maverage – 0.25 × kss


 Equation 10. Average divider, down spread


          M average = 64.5, kss = 4
                    Maverage,center spread = Maverage =64.5 (SSMDIV[32:0] = 0_1000_0001_0000_0000_0000_0000_0000_0000b= 0_8100_0000h)
                  Maverage,down spread = Maverage – 1=63.5 (SSMDIV[32:0] = 0_0111_1111_1000_0000_0000_0000_0000_0000b= 0_7F80_0000h)


 Equation 11. Average divider example


34.3.4.5.2.5  Triangular wave modulation
You can calculate the clock cycle displacement and spectral tone reduction ΔP for the center spread triangular waveform
modulation with a modulation frequency depth δfmodpk-pk and a modulation frequency fm.
Equation 12 shows the theoretical maximum clock-cycle displacement (peak-to-peak).

                                                    Nss × kss
                                                                                                     if PLLCTRL[BYPASSPOSTDIV] = 1
                                                 16
                                      ∆nmax;theoretical =
                                                    Nss × kss
                                                                                                     if PLLCTRL[BYPASSPOSTDIV] = 0 and PLLCTRL[PDIV] = 1
                                              32 × PPLL

 Equation 12. Theoretical maximum clock-cycle displacement

In practice the clock cycle displacement could be larger. So, for safety reasons (buffer overflow) use Equation 13:

                                                    Nss × kss
                                                                                                    if PLLCTRL[BYPASSPOSTDIV] = 1
                                                 8
                                     ∆nmax;practical =
                                                    Nss × kss
                                                                                                    if PLLCTRL[BYPASSPOSTDIV] = 0 and PLLCTRL[PDIV] = 1
                                             16 × PPLL

 Equation 13. Practical maximum clock-cycle displacement

Equation 14 shows the spectral tone reduction / EMI reduction ΔP at Fout.

                                                   Nss × kss
                                                                                                  if PLLCTRL[BYPASSPOSTDIV] = 1                                    10 × log (   2  )
                         ∆P ≈
                                                   Nss × kss                 {10 × log (      )  if PLLCTRL[BYPASSPOSTDIV] = 0 and PLLCTRL[PDIV] = 1                                                4
 Equation 14. ΔP at Fout



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1232 / 3791

<!-- page 1233 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Table 330 shows the spectral tone reduction and clock cycle displacement for BYPASSPOSTDIV=0 and PPLL=1.

Table 330. Values for different settings, BYPASSPOSTDIV = 0, PPLL = 1

 Table values are:
                         512          384          256          128       64       32       24       16 ΔP Δnmax
        =   ≈   =   =   =   =   ≈   =                                                                           MF[2:0]=000 NSS                    MF[2:0]=001 NSS                    MF[2:0]=010 NSS                    MF[2:0]=011 NSS                    MF[2:0]=100 NSS                    MF[2:0]=101 NSS                    MF[2:0]=110 NSS                    MF[2:0]=111 NSS

 MR[2:0]=000,     0 dB 0     0 dB 0     0 dB 0     0 dB 0     0 dB 0     0 dB 0     0 dB 0     0 dB 0
 Kss≈0

 MR[2:0]=001,     21 dB 32   20 dB 24   18 dB 16   15 dB 8    12 dB 4    9 dB 2     8 dB 1.5    6 dB 1
 Kss≈0.5

 MR[2:0]=010,     23 dB 48   22 dB 32   20 dB 24   17 dB 12   14 dB 6    11 dB 3    10 dB 2.2   8 dB 1.5
 Kss≈0.75

 MR[2:0]=011,     24 dB 64   23 dB 48   21 dB 32   18 dB 16   15 dB 8    12 dB 4    11 dB 3    9 dB 2
 Kss≈1

 MR[2:0]=100,     26 dB 96   25 dB 64   25 dB 48   20 dB 24   17 dB 12   14 dB 6    13 dB 4.5   12 dB 4
 Kss≈1.5

 MR[2:0]=101,     27 dB 128  26 dB 96   24 dB 64   21 dB 32   18 dB 16   15 dB 8    14 dB 6    12 dB 4
 Kss≈2

 MR[2:0]=110,     28 dB 192  28 dB 128  26 dB 96   23 dB 48   20 dB 24   17 dB 12   16 dB 9    14 dB 6
 Kss≈3

 MR[2:0]=111,     30 dB 256  29 dB 192  27 dB 128  24 dB 64   21 dB 32   18 dB 16   17 dB 12   15 dB 8
 Kss≈4

34.3.4.5.2.6  Mdivider value, stepsize, and output frequency
Table 331 shows examples of the average feedback-divider ratio Maverage (which defines) and stepsize.


                      Maverage = SS_MDIV [32:25] dec + 2-25 × SS_MDIV[24:0] dec + DITHER × 2-26 = 2-25 × SS_MDIVdec + DITHER × 2-26


 Equation 15. Average feedback-divider ratio

If the SSCG WRAPPER is in power down (SS_PD = 1), then the input word SS_MDIV[32:0] is not modulated. Examples of the
feedback-divider ratio M and stepsize for SS_PD = 1 can be found in Table 332.

Table 331. Examples of Maverage and stepsize if SS_PD = 0
 SS_MDIV[32:0]              Maverage(DITHER=1)          Maverage(DITHER=0)          Stepsize
 0 1000 0000 0000 0000      64 + 2-26                  64                       1 ÷ (225*Maverage) ≈ 0.48ppb
 0000 0000 0000 0000bin =
 080000000hex = 64 *225

                                  Tablecontinuesonthenextpage...




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1233 / 3791

<!-- page 1234 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Table 331. Examples of Maverage and stepsize if SS_PD = 0 (continued)
 SS_MDIV[32:0]              Maverage(DITHER=1)          Maverage(DITHER=0)          Stepsize
 0 1000 0010 0000 0000      65 + 2-26                  65                       1 ÷ (225*Maverage) ≈ 0.46ppb
 0000 0000 0000 0000bin =
 082000000hex = 65 *225
 0 1000 0010 0000 0000      65 + 2-25 + 2-26             65 + 2-25                  1 ÷ (225*Maverage) ≈ 0.46ppb
 0000 0000 0000 0001bin =
 082000001hex = 65 *225+1
 0 1000 0011 0000 0000      65 + 2-1 + 2-26              65 + 2-1                   1 ÷ (225*Maverage) ≈ 0.46ppb
 0000 0000 0000 0000bin =
 083000000hex = 65 * 225+224
 0 1011 1000 0000 0000      92 + 2-26                  92                       1 ÷ (225*Maverage) ≈ 0.33ppb1
 0000 0000 0000 0000bin =
 0B8000000hex = 92 *225
 0 0011 0110 0000 0000      27 + 2-26                  27                       1 ÷ (225*Maverage) ≈ 1.11ppb2
 0000 0000 0000 0000bin =
 036000000hex = 27 *225

1. maximum Maverage, minimum stepsize
2. minimum Maverage, maximum stepsize

Table 332. Examples M and stepsize if SS_PD = 1
 SS_MDIV[32:0]              M 1(SS_PD = 1)                       stepsize2(SS_PD = 1)

 0 1000 0000 0000 0000 0000 0000 0000 64                                1 ÷ M ≈ 1.56%
 0000bin = 080000000hex = 64 * 225

 0 1000 0010 0000 0000 0000 0000 0000 65                                1 ÷ M ≈ 1.54%
 0000bin = 082000000hex = 65 *225

 0 1000 0010 0000 0000 0000 0000 0000 65                                1 ÷ M ≈ 1.54%
 0001bin = 082000001hex = 65 *225 + 1

 0 1000 0011 0000 0000 0000 0000 0000 65                                1 ÷ M ≈ 1.54%
 0000bin = 083000000hex = 65 * 225 + 224

 0 1011 1000 0000 0000 0000 0000 0000 92                                1 ÷ M ≈ 1.09%
 0000bin = 0B8000000hex = 92 *225

 0 0011 0110 0000 0000 0000 0000 0000 27                                1 ÷ M ≈ 3.70%
 0000bin = 036000000hex = 27 *225

1. M = SS_MDIV[32:25]dec
2.  stepsize = 1 ÷ M

34.3.4.5.3  Application information
In Figure 165 of the SSCG, WRAPPER and PLL are shown.
The spread spectrum clock can be disabled by setting MR[2:0] to zero. If the fractional part of SS_MDIV[32:0] is set to zero
(SS_MDIV[24:0]=0) then the feedback divider ratio will have a constant value (no fractional behavior).

                                           NOTE
                        If the spread spectrum mode is enabled, choose N to ensure Fin/N > 3 MHz. The spread spectrum mode cannot
            be used when Fin = 32 kHz.


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1234 / 3791

<!-- page 1235 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

34.3.4.5.3.1 PLL bandwidth settings
For spread spectrum and fractional application, it is important to set the PLL bandwidth in a preferred setting. The preferred
settings for the PLL bandwidth at 3 MHz < Fref < 5 MHz are:
   • SELR[3:0]=b0100
   • SELI[5:0]=b000100
   • SELP[4:0]=b00011

34.3.4.5.3.2  Input clock
For Spread spectrum and fractional application, the preferred reference frequency is Fref > 3 MHz.

34.3.4.6  Procedure for determining PLL settings
In general, the PLL configuration values are as follows:
   1.  Identify a desired PLL output frequency. This may depend on a specific interface frequency needed or on expected
    CPU performance requirements, and may be limited by system power availability.
   2. Determine which clock source to use as the PLL input. This is based on the required power or accuracy, or by the
       potential to obtain the desired PLL output frequency.
   3.  Identify PLL settings to obtain the desired output from the selected input. The Fcco frequency must be either the actual
      desired output frequency, or the desired output frequency times 2 x P, where P is from 1 to 31. The Fcco frequency must
      also be a multiple of the PLL reference frequency, which is either the PLL input or the PLL input divided by N (where N
        is from 1 to 255).
   4. There are several ways to obtain the same PLL output frequency. PLL power depends on Fcco (a lower frequency uses
       less power) and the divider used. Bypassing the input or output divider, or bypassing both, saves power.
   5. Ensure that the selected settings meet all the PLL requirements:
           • Fin is in the range of 32 kHz to 150 MHz.
           • Fcco is in the range of 275 MHz to 550 MHz.
           • Fout is in the range of 4.3 MHz to 2x Max CPU frequency.
           • The predivider is either bypassed, or N is in the range of 1 to 255.
           • The postdivider is either bypassed, or P is in the range of 1 to 31.
           • M is in the range of 1 to 65,535.

                                           NOTE
              Also note that the PLL startup time becomes longer as Fref drops. PLL accuracy and jitter is better with higher values
                of Fref.

34.3.4.7 PLL setup sequence
The sequence of initializing and connecting the PLL is as follows:
   1. Enable PLL LDO.
   2. Ensure that the PLL output is disconnected from any downstream functions.
   3. Select a PLL input clock source. See SOURCE field in APLL Control Register (APLLCTRL) and SPLL Control Register
     (SPLLCTRL).
   4. Set up the PLL dividers and mode settings. See APLL and SPLL registers in Memory map and register definition.
   5. Wait for the input clock source to be valid. It is indicated by SOSCCSR[SOSCVLD] for SOSC and FIRCCSR[FIRCACC]
       for FIRC.


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1235 / 3791

<!-- page 1236 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

   6. Power up PLL and write 1 to PLLPWREN and PLLCLKEN. The PLL clock is gated until PLL_LOCK=1.
   7. Wait for the PLL output to stabilize, when PLL_LOCK=1. The start-up time is tpll_lock.
   8.  If the PLL is used to clock the CPU, you can change the CPU Clock Divider setting for the operation with the PLL, if
      needed. This must be complete before connecting the PLL.
   9. Connect the PLL to whichever downstream function you are using it with. See the chip's Clocking chapter for more
      information on the clock divider structure.

34.3.5  Clocks
SCG has the following input clocks:
   • Bus clock for register access
   • External USB PLL (UPLL) and Real-Time oscillator (ROSC) clocks as one of the system clock sources
SCG has the following output clocks:
   • Main clock
   • System PLL clock (SPLL)
   • Auxiliary PLL clock (APLL)
   • Fast internal reference clock (FIRC)
   — 144 MHz and 48 MHz
   • Slow internal reference clock (SIRC)
   — 12 MHz and 1 MHz, divided from SIRC
   • System oscillator clock (SOSC)

34.3.6 Resets
These are the SCG resets:
   • The global reset is fed into this module. The entire module is reset by the global reset.
   • The clock monitor generates a reset in the following cases:
   — SOSCCSR[SOSCCM] = 1 & SOSCCSR[SOSCCMRE] = 1 & SOSCCSR[SOSCERR] = 1
   — ROSCCSR[ROSCCM] = 1 & ROSCCSR[ROSCCMRE] = 1 & ROSCCSR[ROSCERR] = 1
   — APLLCSR[APLLCM] = 1 & APLLCSR[APLLCMRE] = 1 & APLLCSR[APLLERR] = 1
   — SPLLCSR[SPLLCM] = 1 & SPLLCSR[SPLLCMRE] = 1 & SPLLCSR[SPLLERR] = 1
   — UPLLCSR[UPLLCM] = 1 & UPLLCSR[UPLLCMRE] = 1 & UPLLCSR[UPLLERR] = 1
On the assertion of any reset source, the FIRC 48 MHz clock is enabled or starts up if not already running.

34.3.7  Interrupts
   • Clock monitor generates an interrupt when any of the following occurs:
   — SOSCCSR[SOSCCM] = 1 & SOSCCSR[SOSCCMRE] = 0 & SOSCCSR[SOSCERR] = 1
   — ROSCCSR[ROSCCM] = 1 & ROSCCSR[ROSCCMRE] = 0 & ROSCCSR[ROSCERR] = 1
   — APLLCSR[APLLCM] = 1 & APLLCSR[APLLCMRE] = 0 & APLLCSR[APLLERR] = 1
   — SPLLCSR[SPLLCM] = 1 & SPLLCSR[SPLLCMRE] = 0 & SPLLCSR[SPLLERR] = 1
   — UPLLCSR[UPLLCM] = 1 & UPLLCSR[UPLLCMRE] = 0 & UPLLCSR[UPLLERR] = 1
   • Interrupt can also be generated when any of the following occurs:


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1236 / 3791

<!-- page 1237 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

   — SOSC clock source is not valid. SOSCCSR[SOSCVLD_IE] = 1 & SOSCCSR[SOSCVLD] = 1
   — FIRC clock source is not accurate. FIRCCSR[FIRCACC_IE] = 1 & FIRCCSR[FIRCACC] = 1
   — FIRC trimming error detected. FIRCCSR[FIRCERR_IE] = 1 & FIRCCSR[FIRCERR] = 1
   — SIRC trimming error detected. SIRCCSR[SIRCERR_IE] = 1 & SIRCCSR[SIRCERR] = 1
   — APLL is not powered or locked. APLLCSR[APLL_LOCK_IE] = 1 & APLLCSR[APLL_LOCK] = 1
   — SPLL is not powered or locked. SPLLCSR[SPLL_LOCK_IE] = 1 & SPLLCSR[SPLL_LOCK] = 1

34.4  External signals

Table 333. External signal descriptions
         Signal                               Description                 Mode                        I/O

       XTAL      SOSC Crystal output pin                     SOSC              O

      EXTAL      SOSC Crystal input pin                      SOSC                                       I

     CLKOUT     SCG clock output pin                      SCG               O

34.5  Initialization
Out of reset, the following events occur:
   • FIRC is default enabled and FIRCCFG[RANGE] is reset to zero. The FIRC 48 MHz clock is selected as the system
    clock source.
   • SIRC is default enabled.
   •  All other clock sources are disabled.

34.6  Application information

34.6.1 SOSC configuration example
   1. Write 1 to LDOCSR[LDOEN] to enable LDO
   2. Write 1 to SOSCCFG[EREFS] to select SOSC (internal crystal oscillator) source
   3. Write 0 to SOSCCFG[RANGE] to configure SOSC range
   4. Write 0 to SOSCCSR[LK] to unlock SOSCCSR
   5. Write 1 to SOSCCSR[SOSCCM] to enable SOSC clock monitor
   6. Write 1 to SOSCCSR[SOSCSTEN] to enable SOSC in Deep Sleep mode, if needed
   7. Write 1 to SOSCCSR[SOSCEN] to enable SOSC
   8. Read SOSCCSR[SOSCVLD] until it returns 1, indicating SOSC is valid
   9. Read SOSCCSR[SOSCERR] to ensure it returns 0, indicating there is no error
   10. Write 1 to RCCR[SCS] to switch main clock to SOSC
   11. Read CSR[SCS] until it returns 1, indicating the switch is complete

34.6.2 SIRC configuration example

34.6.2.1 SIRC normal configuration example
   1. Write 0 to SIRCCSR[LK] to unlock SIRCCSR


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1237 / 3791

<!-- page 1238 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

   2. Write 1 to SIRCCSR[SIRCSTEN] to enable SIRC in Deep Sleep mode, if needed
   3. Write 1 to SIRCCSR[SIRC_CLK_PERIPH_EN] to enable SIRC clock for peripheral use
   4. Read SIRCCSR[SIRCVLD] until it returns 1, indicating SIRC is valid
   5. Read SIRCCSR[SIRCERR] to ensure it returns 0
   6. Write 2 to RCCR[SCS] to switch main clock to SIRC
   7. Read CSR[SCS] until it returns 2, indicating the switch is complete

34.6.2.2 SIRC auto trim example
   1. Follow steps 1 through 8 in SOSC configuration example
   2. Write 2 to SIRCTCFG[TRIMSRC] to select SOSC as auto trim clock source
   3. Write 39 to SIRCTCFG[TRIMDIV] to divide SOSC to 1 MHz
   4. Write 0 to SIRCCSR[LK] to unlock SIRCCSR
   5. Write 1 to SIRCCSR[SIRCTREN] to enable auto trim
   6. Write 1 to SIRCCSR[SIRCTRUP] to enable update
   7. Read SIRCCSR[SIRCVLD] until it returns 1, indicating SIRC is valid
   8. Read SIRCCSR[SIRCERR] to ensure it returns 0
   9. Read SIRCCSR]TRIM_LOCK] until it returns 1.
The auto trim process continues until it is disabled. The SIRCSTAT register shows the locked trim values. If there is an unlock after
lock, SIRCCSR[SIRCERR] is set.

34.6.3 FIRC configuration example

34.6.3.1 FIRC normal configuration example
   1. Write 1 to FIRCCFG[RANGE] to select 144 MHz
   2. Write 0 to FIRCCSR[LK] to unlock FIRCCSR
   3. Write 1 to FIRCCSR[FIRC_FCLK_PERIPH_EN] to enable FIRC 144 MHz clock for peripheral use, if needed
   4. Write 1 to FIRCCSR[FIRC_SCLK_PERIPH_EN] to enable FIRC 48 MHz clock for peripheral use, if needed
   5. Write 1 to FIRCCSR[FIRCSTEN] to enable FIRC clock source in Deep Sleep mode, if needed
   6. Write 1 to FIRCCSR[FIRCEN] to enable FIRC
   7. Read FIRCCSR[FIRCVLD] until it returns 1, indicating FIRC is valid
   8. Read FIRCCSR[FIRCERR] to ensure it returns 0
   9. Write 3 to RCCR[SCS] to switch main clock to FIRC
   10. Read CSR[SCS] until it returns 3, indicating the switch is complete

34.6.3.2 FIRC auto trim example
   1. sosc_clk_enable (); // Enable SOSC clock
   2. wait_sosc_valid (); // Wait SOSC to be valid
   3. Write 2 to FIRCTCFG[TRIMSRC] to select SOSC as auto trim clock source
   4. Write 39 to FIRCTCFG[TRIMDIV] to divide SOSC to 1 MHz
   5. Write 0 to FIRCCSR[LK] to unlock FIRCCSR


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1238 / 3791

<!-- page 1239 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

   6. Write 1 to FIRCCSR[FIRCTREN] to enable auto trim
   7. Write 1 to FIRCCSR[FIRCTRUP] to enable update
   8. Read FIRCCSR[FIRCVLD] until it returns 1, indicating FIRC is valid
   9. Read FIRCCSR[FIRCERR] to ensure it returns 0
   10. Read FIRCCSR[TRIM_LOCK] until it returns 1
The auto trim process continues until it is disabled. The FIRCSTAT register shows the locked trim values. If there is an unlock after
lock, FIRCCSR[FIRCERR] is set.

34.6.4 ROSC configuration example
   1. Write 0 to ROSCCSR[LK] to unlock ROSCCSR
   2. Write 1 to ROSCCSR[ROSCCM] to enable ROSC clock monitor, if needed
   3. Read ROSCCSR[ROSCVLD] until it returns 1, indicating ROSC is valid
   4. Read ROSCCSR[ROSCERR] to ensure it returns 0, indicating there is no error
   5. Write 4 to RCCR[SCS] to switch main clock to ROSC
   6. Read CSR[SCS] until it returns 4, indicating the switch is complete

34.6.5 APLL configuration example (SPLL is similar as APLL)

34.6.5.1 APLL outputs 200 MHz clock with normal function

   1. Write 1 to LDOCSR[LDOEN] to enable LDO
   2. Configure APLL reference clock source: SOSC
          a. Write 1 to SOSCCFG[EREFS] to select SOSC source
          b. Write 2 to SOSCCFG[RANGE] to configure SOSC range
           c. Write 0 to SOSCCSR[LK] to unlock SOSCCSR
          d. Write 1 to SOSCCSR[SOSCCM] to enable SOSC clock monitor
          e. Write 1 to SOSCCSR[SOSCEN] to enable SOSC
                f. Read SOSCCSR[SOSCVLD] returns 1, indicating SOSC is valid
          g. Read SOSCCSR[SOSCERR] to ensure it returns 0, indicating there is no error
   3. Configure APLL:

                                           NOTE
                                                 clk_out = clk_in/N*M/P (bypass post divider)

          a. Write 0 to APLLCTRL[SOURCE] to select SOSC as clock source
          b. Write 6 to APLLCTRL[SELP]. The value should refer to register SELP
           c. Write 13 to APLLCTRL[SELI]. The value should refer to register SELI
          d. Write 0 to APLLCTRL[SELR]. The value should refer to register SELR
          e. Write 2 to APLLNDIV[NDIV] to configure N divider
                f.  Write 2 to APLLPDIV[PDIV] to configure P divider
          g. Write 20 to APLLMDIV[MDIV] to configure M divider
          h. Write 0 to APLLCTRL[BYPASSPREDIV] to use the N divider



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1239 / 3791

<!-- page 1240 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

                 i.  Write 0 to APLLCTRL[BYPASSPOSTDIV] to use the P divider
                 j.  Write 1 to APLLCTRL[BYPASSPOSTDIV2] to bypass P divider_2
           k. Write 0 to APLLCSR[LK] to unlock APLLCSR
                 l.  Write 1 to APLLCSR[APLLCM] to enable clock monitor
        m. Write 5A5A0001h to the TRIM_LOCK register to unlock the APLLLOCK_CNFG register
          n. Write 10300 to APLLLOCK_CNFG[LOCK_TIME] to configure lock time of APLL stable. 10300 equals 500 µs/50 ns
          + 300, where 50 ns is the period of clk_ref (clk_in/N).
          o. Write 3 to APLLCSR[APLLCLKEN] and APLLCSR[APLLPWREN] to enable and power on the APLL clock
          p. Either read APLLCSR[APLL_LOCK] until it returns 1, indicating APLL is locked, or enable
          APLLCSR[APLL_LOCK_IE] to use the interrupt
          q. Read APLLCSR[APLLERR] to ensure it returns 0
   4. Write 5 to RCCR[SCS] to switch main clock to APLL
   5. Read CSR[SCS] until it returns 5, indicating the switch is complete

34.6.5.2 APLL outputs 150 MHz clock with spread-spectrum function from FIRC 48 MHz reference clock

   1. Write 1 to LDOCSR[LDOEN] to enable LDO
   2. Configure APLL reference clock source: FIRC 48 MHz
          a. Write 1 to FIRCCSR_FIRC_SCLK_PERIPH_EN] to select 48 MHz
          b. Write 0 to FIRCCSR[LK] to unlock FIRCCSR
           c. Write 1 to FIRCCSR[FIRCEN] to enable FIRC
          d. Read FIRCCSR[FIRCVLD] until it returns 1, indicating FIRC is valid
          e. Read FIRCCSR[FIRCERR] to ensure it returns 0
   3. Configure APLL:

                                           NOTE
                                                 clk_out = clk_in/N*M/P (bypass post divider)

          a. Write 1 to APLLCTRL[LIMUPOFF] to set to spectrum and fractional applications
          b. Write 1 to APLLCTRL[SOURCE] to select FIRC as clock source
           c. Write 2 to APLLCTRL[SELP]. The value should refer to register SELP.
          d. Write 5 to APLLCTRL[SELI]. The value should refer to register SELI.
          e. Write 0 to APLLCTRL[SELR]. The value should refer to register SELR.
                f.  Write 1 to APLLNDIV[NDIV] to configure N divider
          g. Write 2 to APLLPDIV[PDIV] to configure P divider
          h. Write 0C800000h to APLLSSCG0[SS_MDIV_LSB] to configure M divider (M = 6.25)
                 i.  Write 0 to APLLSSCG1[SS_MDIV_MSB] to configure M divider (M = 6.25)
                 j.  Write 1 to APLLSSCG1[SEL_SS_MDIV]
           k. Write 0 to APLLSSCG1[DITHER]
                 l.  Write 2 to APLLSSCG1[MC]
        m. Write 0 to APLLSSCG1[MR]



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1240 / 3791

<!-- page 1241 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

          n. Write 0 to APLLSSCG1[MF]
          o. Write 0 to APLLCTRL[BYPASSPREDIV] to use the N divider
          p. Write 0 to APLLCTRL[BYPASSPOSTDIV] to use the P divider
          q. Write 1 to APLLCTRL[BYPASSPOSTDIV2] to bypass P divider_2
              r.  Write 0 to APLLCSR[LK] to unlock APLLCSR
           s. Write 1 to APLLCSR[APLLCM] to enable clock monitor
                t.  Write 5A5A0001h to the TRIM_LOCK register to unlock the APLLLOCK_CNFG register
          u. Write 24300 to APLLLOCK_CNFG[LOCK_TIME] to configure lock time of APLL stable. 24300 equals 500 µs/
            20.8 ns+300, where 20.8 ns is the period of clk_ref (clk_in/N).
           v. Write 0 to APLLSSCG1[SS_PD] to power up SSCG
         w. Write 3 to APLLCSR[APLLCLKEN] and APLLCSR[APLLPWREN] to enable and power on PLL clock
           x.  Either read APLLCSR[APLL_LOCK] until it returns 1, indicating APLL is locked, or enable
          APLLCSR_APLL_LOCK_IE to use the interrupt
           y. Read APLLCSR[APLLERR] to ensure it returns 0
   4. Write 5 to RCCR[SCS] to switch main clock to APLL
   5. Read CSR[SCS until it returns 5, indicating the switch is complete

34.6.6 UPLL configuration example
   1. Write 0 to UPLLCSR[LK] to unlock UPLLCSR
   2. Write 1 to UPLLCSR[UPLLCM] to enable clock monitor
   3. Read UPLLCSR[UPLLVLD] until it returns 1, indicating UPLL is valid
   4. Read UPLLCSR[UPLLERR] to ensure it returns 0
   5. Write 7 to RCCR[SCS] to switch main clock to UPLL
   6. Read CSR[SCS] until it returns 7, indicating the switch is complete
   7. Write 1 to UPLLCSR[LK] to lock UPLLCSR

34.7 Memory map and register definition
This section includes information about the memory map and register definitions:
   • Accesses on non-implemented registers that are outside the SCG maximum illegal address range generate a transfer error.
   • Read accesses on non-implemented registers that are inside the SCG maximum illegal address range do not generate a
    transfer error.
   • Write accesses on non-implemented registers that are inside the SCG maximum illegal address range generate a
    transfer error.
   • Write accesses on read-only registers generate a transfer error.
   • Only 32-bit writes are allowed for any writable SCG registers. Writes of 8 or 16 bits result in transfer errors.

34.7.1 SCG register descriptions

34.7.1.1 SCG memory map
SCG0 base address: 4004_4000h




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1241 / 3791

<!-- page 1242 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


     Offset     Register                                                        Width    Access   Reset value
                                                                                                     (In bits)

     0h       Version ID Register (VERID)                                    32     R     0000_0000h

     4h      Parameter Register (PARAM)                                   32     R     0000_01FEh

     8h       Trim Lock register (TRIM_LOCK)                                 32    RW    0000_0000h

     10h      Clock Status Register (CSR)                                    32     R     0300_0000h

     14h     Run Clock Control Register (RCCR)                              32    RW    0300_0000h

    100h    SOSC Control Status Register (SOSCCSR)                        32    RW    0000_0000h

    108h    SOSC Configuration Register (SOSCCFG)                         32    RW    0000_0000h

    200h     SIRC Control Status Register (SIRCCSR)                          32    RW    0100_0020h

    20Ch    SIRC Trim Configuration Register (SIRCTCFG)                     32    RW    0000_0000h

    210h     SIRC Trim Register (SIRCTRIM)                                 32    RW    See section

    218h     SIRC Auto-trimming Status Register (SIRCSTAT)                    32    RW    See section

    300h     FIRC Control Status Register (FIRCCSR)                          32    RW    See section

    308h     FIRC Configuration Register (FIRCCFG)                           32    RW    0000_0000h

    30Ch     FIRC Trim Configuration Register (FIRCTCFG)                      32    RW    0000_0000h

    310h     FIRC Trim Register (FIRCTRIM)                                 32    RW    See section

    318h     FIRC Auto-trimming Status Register (FIRCSTAT)                    32    RW    See section

    400h    ROSC Control Status Register (ROSCCSR)                        32    RW    0000_0000h

    500h     APLL Control Status Register (APLLCSR)                          32    RW    0000_0000h

    504h     APLL Control Register (APLLCTRL)                              32    RW    0000_0000h

    508h     APLL Status Register (APLLSTAT)                               32     R     0000_0000h

    50Ch    APLL N Divider Register (APLLNDIV)                             32    RW    0000_0001h

    510h     APLL M Divider Register (APLLMDIV)                             32    RW    0000_0001h

    514h     APLL P Divider Register (APLLPDIV)                             32    RW    0000_0001h

    518h     APLL LOCK Configuration Register (APLLLOCK_CNFG)              32    RW    0000_4F4Ch

    520h     APLL SSCG Status Register (APLLSSCGSTAT)                    32     R     0000_0000h

    524h     APLL Spread Spectrum Control 0 Register (APLLSSCG0)             32    RW    0000_0000h

    528h     APLL Spread Spectrum Control 1 Register (APLLSSCG1)             32    RW    8000_0000h

    5F4h    APLL Override Register (APLL_OVRD)                            32    RW    0000_0000h

    600h     SPLL Control Status Register (SPLLCSR)                          32    RW    0000_0000h

    604h     SPLL Control Register (SPLLCTRL)                              32    RW    0000_0000h

    608h     SPLL Status Register (SPLLSTAT)                               32     R     0000_0000h

    60Ch    SPLL N Divider Register (SPLLNDIV)                             32    RW    0000_0001h

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1242 / 3791

<!-- page 1243 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

     Offset     Register                                                        Width    Access   Reset value
                                                                                                     (In bits)

    610h     SPLL M Divider Register (SPLLMDIV)                             32    RW    0000_0001h

    614h     SPLL P Divider Register (SPLLPDIV)                             32    RW    0000_0001h

    618h     SPLL LOCK Configuration Register (SPLLLOCK_CNFG)              32    RW    0000_4F4Ch

    620h     SPLL SSCG Status Register (SPLLSSCGSTAT)                    32     R     0000_0000h

    624h     SPLL Spread Spectrum Control 0 Register (SPLLSSCG0)             32    RW    0000_0000h

    628h     SPLL Spread Spectrum Control 1 Register (SPLLSSCG1)             32    RW    8000_0000h

    6F4h    SPLL Override Register (SPLL_OVRD)                            32    RW    0000_0000h

    700h    UPLL Control Status Register (UPLLCSR)                         32    RW    0000_0000h

    800h    LDO Control and Status Register (LDOCSR)                        32    RW    0000_0008h

34.7.1.2  Version ID Register (VERID)

Offset


 Register                   Offset

 VERID                0h


Function
Indicates the version integrated for this instance on the device.

                                           NOTE
                                               Writing to this register results in a transfer error.


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                VERSION

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                VERSION

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1243 / 3791

<!-- page 1244 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Fields


       Field       Function

      31-0     SCG Version Number
   VERSION    This read-only field returns the SCG module version number.


34.7.1.3  Parameter Register (PARAM)

Offset


 Register                   Offset

 PARAM               4h


Function
Indicates the feature parameters for this instance on the device.

                                           NOTE
                                               Writing to this register results in a transfer error.


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6       5      4       3      2       1       0

                                                         UPLLC  SPLLC  APLLC  ROSC  FIRCC  SIRCC  SOSC
    R                          0                            1                                                           0
                                                                                           LK...     LK...     LK...    CLK...    LK...     LK...    CLK...

   W

Reset     0      0      0      0       0      0      0      1       1      1       1      1       1      1       1       0


Fields


       Field       Function
      31-9      Reserved
    —

       8        Reserved
    —

       7       UPLL Clock Present
                   Indicates that the UPLL clock source is present.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1244 / 3791

<!-- page 1245 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

 UPLLCLKPRES       0b - UPLL clock source is not present
                     1b - UPLL clock source is present

       6       SPLL Clock Present
 SPLLCLKPRES  Indicates that the SPLL clock source is present.
                     0b - SPLL clock source is not present
                     1b - SPLL clock source is present

       5       APLL Clock Present
 APLLCLKPRES  Indicates that the APLL clock source is present.
                     0b - APLL clock source is not present
                     1b - APLL clock source is present

       4      ROSC Clock Present
 ROSCCLKPRE  Indicates that the ROSC clock source is present.
      S                     0b - ROSC clock source is not present
                     1b - ROSC clock source is present

       3       FIRC Clock Present
 FIRCCLKPRES  Indicates that the FIRC clock source is present.
                     0b - FIRC clock source is not present
                     1b - FIRC clock source is present

       2       SIRC Clock Present
 SIRCCLKPRES  Indicates that the SIRC clock source is present.
                     0b - SIRC clock source is not present
                     1b - SIRC clock source is present

       1      SOSC Clock Present
 SOSCCLKPRE  Indicates that the SOSC clock source is present.
      S                     0b - SOSC clock source is not present
                     1b - SOSC clock source is present

       0        Reserved
    —





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1245 / 3791

<!-- page 1246 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

34.7.1.4  Trim Lock register (TRIM_LOCK)

Offset


 Register                   Offset

 TRIM_LOCK           8h


Function
Contains the TRIM_UNLOCK and IFR_DISABLE fields.

                                           NOTE
                                     These fields reset solely on a POR or LVD event.


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W                                               TRIM_LOCK_KEY

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1       0

    R                                                    0                                                                                                                        IFR_DI  TRIM_
                                                                                                                                                                       S...     UN...   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0       0


Fields


       Field       Function

     31-16     TRIM_LOCK_KEY
 TRIM_LOCK_K  Write value that is used to unlock writes to the IFR_DISABLE and TRIM_UNLOCK fields. Write 5A5Ah to
     EY        allow writes to IFR_DISABLE and TRIM_UNLOCK to take effect.

                                                 NOTE
                                                      This field is writable but reads always return 0.

      15-2      Reserved
    —

       1       IFR_DISABLE
 IFR_DISABLE  Locks IFR write access to SCG trim registers. When set, this field prevents SCG trim registers from
                  loading IFR during warm reset.

                                  Tablecontinuesonthenextpage...





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1246 / 3791

<!-- page 1247 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                                                 NOTE
                           IFR_DISABLE is only writable during write access to TRIM_LOCK when the
                             TRIM_LOCK[31:16] write value equals 16'h5a5a.

                     0b - IFR write access to SCG trim registers not disabled. The SCG Trim registers are
                     reprogrammed with the IFR values after any system reset.
                     1b - IFR write access to SCG trim registers during system reset is blocked.

       0      TRIM_UNLOCK
 TRIM_UNLOCK  Locks user write access to SCG Trim and PLL LOCK Configuration registers.

                                                 NOTE
                        TRIM_UNLOCK is only writable during write access to TRIM_LOCK when the
                             TRIM_LOCK[31:16] write value equals 16'h5a5a.

                     0b - SCG Trim registers are locked and not writable.
                     1b - SCG Trim registers are unlocked and writable.


34.7.1.5  Clock Status Register (CSR)

Offset


 Register                   Offset

 CSR                  10h


Function
Shows the currently configured clock source that is generating the system clock.

                                           NOTE
                                     Writing to the Clock Status Register (CSR) results in a transfer error.


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R               0                      SCS                                          0

   W

Reset     0      0      0      0       0      0      1      1       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1247 / 3791

<!-- page 1248 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Fields


       Field       Function
     31-28      Reserved
    —

     27-24      System Clock Source
    SCS       Returns the currently configured clock source generating the system clock. Reports the configuration set
                by the RCCR.
                     0000b - Reserved
                     0001b - SOSC
                     0010b - SIRC
                     0011b - FIRC
                     0100b - ROSC
                     0101b - APLL
                     0110b - SPLL
                     0111b - UPLL
                     1000b-1111b - Reserved

      23-0      Reserved
    —


34.7.1.6 Run Clock Control Register (RCCR)

Offset


 Register                   Offset

 RCCR                14h


Function
Selects the clock source generating the system clock.

                                           NOTE
              Switching to a new system clock source must be done after previous RCCR changes are complete and the Clock
              Status Register has been updated to match the present RCCR setting.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1248 / 3791

<!-- page 1249 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R               0                                                                         0
                                        SCS
   W

Reset     0      0      0      0       0      0      1      1       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-28      Reserved
    —

     27-24      System Clock Source
    SCS       Selects the clock source generating the system clock in Run mode. Attempts to select a clock that is not valid
                 are ignored.
                    In Run mode, the clock source must be enabled and valid before system clocks can switch to a different
                  clock source.

                                                 NOTE
                           You must wait for the previous SCS clock switch to complete before programming SCS to
                            a different value.

                     0000b - Reserved
                     0001b - SOSC
                     0010b - SIRC
                     0011b - FIRC
                     0100b - ROSC
                     0101b - APLL
                     0110b - SPLL
                     0111b - UPLL
                     1000b-1111b - Reserved

      23-0      Reserved
    —





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1249 / 3791

<!-- page 1250 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

34.7.1.7 SOSC Control Status Register (SOSCCSR)

Offset


 Register                   Offset

 SOSCCSR             100h


Function
Contains control and status fields for the SOSC clock source.

Diagram

 Bits      31      30     29     28      27     26      25     24      23     22     21     20      19     18     17      16

                                    SOSC  SOSC  SOSC
    R    0                     0                                                          0            SOSC                    ERR   SEL   VLD                                     SOSC  SOSC                                                               LK
                   VLD...                                                                     CMRE  CM
   W                              W1C

Reset     0       0      0      0       0      0       0      0       0      0      0      0       0      0      0       0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2       1       0

    R                                                0                                                   0                                                                                        SOSC  SOSC
                                                                                             STEN   EN   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0       0       0


Fields


       Field       Function
      31       Reserved
    —

      30     SOSC Valid Interrupt Enable
 SOSCVLD_IE  Generates an interrupt when SOSCVLD is asserted.
                     0b - SOSCVLD interrupt is not enabled
                     1b - SOSCVLD interrupt is enabled

     29-27      Reserved
    —

      26     SOSC Clock Error
  SOSCERR    This flag is reset on Chip POR only. You can also clear this flag by writing 1.
                     0b - SOSC Clock Monitor is disabled or has not detected an error
                     1b - SOSC Clock Monitor is enabled and detected an error

      25     SOSC Selected

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1250 / 3791

<!-- page 1251 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
  SOSCSEL    Indicates if the SOSC clock source is selected as the system clock source.
                     0b - SOSC is not the system clock source
                     1b - SOSC is the system clock source

      24     SOSC Valid
  SOSCVLD    Indicates if the SOSC clock source is valid.
                     0b - SOSC is not enabled or clock is not valid
                     1b - SOSC is enabled and output clock is valid

      23       Lock Register
      LK       Locks this register so that it cannot be written to.

                                                 NOTE
                                                       This field can be cleared or set at any time.

                     0b - This Control Status Register can be written
                     1b - This Control Status Register cannot be written

     22-18      Reserved
    —

      17     SOSC Clock Monitor Reset Enable
  SOSCCMRE   Enables the SOSCERR generate reset.
                     0b - Clock monitor generates an interrupt when an error is detected
                     1b - Clock monitor generates a reset when an error is detected

      16     SOSC Clock Monitor Enable
  SOSCCM    Enables the clock monitor when SOSCVLD is set.
            SOSC clock monitor remains enabled in Deep Sleep mode only if SOSCCM and SOSCSTEN are enabled.
                              If the clock is disabled in Deep Sleep mode, then SOSCCM must be cleared to prevent unexpected SOSC
                   loss of clock. The clock monitor is always disabled in Power Down and Deep Power Down modes. When the
                  clock monitor is disabled in a low power mode, it remains disabled until the clock valid flag is set, following
                     exit from that mode.

                                                 NOTE
                           The reference clock used to monitor the SOSC is the SIRC. This clock must be enabled in
                                  order to monitor the SOSC. SIRC is automatically disabled in Power Down and Deep Power
                         Down modes and the SOSC clock monitor is disabled.

                     0b - SOSC Clock Monitor is disabled
                     1b - SOSC Clock Monitor is enabled

      15-3      Reserved

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1251 / 3791

<!-- page 1252 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

    —

       2        Reserved
    —

       1      SOSC Stop Enable
  SOSCSTEN   Enables the SOSC clock source in Deep Sleep mode if SOSCEN is set.
                     0b - SOSC is disabled in Deep Sleep mode
                     1b - SOSC is enabled in Deep Sleep mode only if SOSCEN is set

       0      SOSC Enable
   SOSCEN    Enables the SOSC clock source.
                     0b - SOSC is disabled
                     1b - SOSC is enabled


34.7.1.8 SOSC Configuration Register (SOSCCFG)

Offset


 Register                   Offset

 SOSCCFG             108h


Function
Contains the clock frequency range select and external reference select for the SOSC clock source.

                                           NOTE
           You cannot change the SOSCCFG register when the SOSC is enabled. When the SOSC is enabled, writes to this
                register are ignored and there is no transfer error.


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3       2      1      0

    R               0                             0                     0                         0                  0                                                                                       EREF
                                                                   RANGE
                                                                                         S   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0       0      0      0




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1252 / 3791

<!-- page 1253 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Fields


       Field       Function
     31-12      Reserved
    —

      11-8      Reserved
    —

       7-6       Reserved
    —

       5-4     SOSC Range Select
   RANGE     Selects the frequency range for the system crystal oscillator (OSC).
                     00b - Frequency range select of 16-20 MHz.
                     01b - Frequency range select of 20-30 MHz.
                     10b - Frequency range select of 30-50 MHz.
                     11b - Frequency range select of 50-66 MHz.

       3        Reserved
    —

       2         External Reference Select
    EREFS      Selects the source for the external reference clock. This field selects which clock is output from the
            SOSC into the SCG: either from the crystal oscillator or from an external clock input.
                     0b - External reference clock selected. LDO can be disabled in this case.
                     1b - Internal crystal oscillator of OSC selected.

       1-0       Reserved
    —


34.7.1.9 SIRC Control Status Register (SIRCCSR)

Offset


 Register                   Offset

 SIRCCSR             200h


Function
Contains control and status bits for the SIRC clock source.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1253 / 3791

<!-- page 1254 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Diagram

 Bits      31     30     29     28      27     26      25     24      23     22     21     20      19     18     17     16

                                         SIRCE  SIRCS  SIRCV
    R               0                                                                             0                                  SIRCE   RR     EL     LD                                                               LK
                                               RR...
   W                              W1C

Reset     0      0      0      0       0      0       0      1       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1       0

                                          TRIM_
    R               0                                                   0                         0                     0                             COAR    LO...   SIRCT  SIRCT                SIRC_                      SIRCS
                                               SE_...        RUP   REN                        CL...                      TEN
   W

Reset     0      0      0      0       0      0       0      0       0      0      1      0       0      0      0       0


Fields


       Field       Function
     31-28      Reserved
    —

      27      SIRC Clock Error Interrupt Enable
  SIRCERR_IE   Generates an interrupt when SIRCERR is asserted.
                     0b - SIRCERR interrupt is not enabled
                     1b - SIRCERR interrupt is enabled

      26      SIRC Clock Error
   SIRCERR    This flag is reset on chip POR only. You can also clear this flag by writing 1.
                     0b - Error not detected with the SIRC trimming
                     1b - Error detected with the SIRC trimming

      25      SIRC Selected
   SIRCSEL     Indicates if the SIRC clock source is selected as the system clock source.
                     0b - SIRC is not the system clock source
                     1b - SIRC is the system clock source

      24      SIRC Valid
   SIRCVLD     Indicates if the SIRC clock source is valid.
                     0b - SIRC is not enabled or clock is not valid
                     1b - SIRC is enabled and output clock is valid

      23       Lock Register
      LK        This field can be cleared or set at any time. Use this field to prevent runaway code from changing SIRC
                  clock configurations.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1254 / 3791

<!-- page 1255 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Control Status Register can be written
                     1b - Control Status Register cannot be written

     22-12      Reserved
    —

      11       Coarse Auto Trim Bypass
 COARSE_TRIM Use this bit is to bypass the coarse trim step when auto-trimming.
   _BYPASS                     0b - SIRC coarse auto-trim is not bypassed
                     1b - SIRC coarse auto-trim is bypassed

      10      SIRC TRIM LOCK
  TRIM_LOCK  When SIRCTREN and SIRCTRUP are enabled, the TRIM_LOCK register indicates when auto trimming
                       is complete and output SIRC frequency has locked to target SIRC range.

                                                 NOTE
                                                •  This field is automatically cleared if SIRCTREN and SIRCTRUP are not set.

                                                • TRIM_LOCK is not set when SIRCTCFG[TRIMSRC] = 2'b00, i.e., using full speed USB
                                  as the trim source.

                     0b - SIRC auto trim not locked to target frequency range
                     1b - SIRC auto trim locked to target frequency range

       9       SIRC Trim Update
  SIRCTRUP    Allows the SIRCSTAT to be updated by auto-trimming hardware.
                     0b - Disables SIRC trimming updates
                     1b - Enables SIRC trimming updates

       8       SIRC 12 MHz Trim Enable (SIRCCFG[RANGE]=1)
  SIRCTREN   Enables the auto trim of SIRC 12 MHz by an external clock source.
                     0b - Disables trimming SIRC to an external clock source
                     1b - Enables trimming SIRC to an external clock source

       7-6       Reserved
    —

       5       SIRC Clock to Peripherals Enable
 SIRC_CLK_PE  Enables the SIRC clock for peripheral use.
   RIPH_EN                     0b - SIRC clock to peripherals is disabled
                     1b - SIRC clock to peripherals is enabled

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1255 / 3791

<!-- page 1256 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       4-2       Reserved
    —

       1       SIRC Stop Enable
  SIRCSTEN    Enables the SIRC clock source in specific power modes. See the chip-specific section for information on
                what conditions apply when SIRCSTEN is used.
                     0b - SIRC is disabled in Deep Sleep mode
                     1b - SIRC is enabled in Deep Sleep mode

       0        Reserved
    —


34.7.1.10 SIRC Trim Configuration Register (SIRCTCFG)

Offset


 Register                   Offset

 SIRCTCFG           20Ch


Function
Contains the auto trim clock source select and trim clock divider control for the SIRC clock source.

                                           NOTE
            The SIRCTCFG register cannot be changed when SIRC tuning is enabled. When the SIRC tuning is enabled, writes
                to this register are ignored and there is no transfer error.


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                  0
                                                                                           TRIMDIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                   0                             0                                0
                                                                                                  TRIMSRC
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1256 / 3791

<!-- page 1257 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Fields


       Field       Function
     31-23      Reserved
    —

     22-16     SIRC Trim Predivider
   TRIMDIV     Divider of SOSC for SIRC trimming.
             When selecting SOSC as the SIRC trimming source, the TRIMDIV register must be set to correct div ratio
                    to generate 1 MHz output reference trimming clock.

                                                 NOTE
                           TRIMDIV is an N-Divider supporting a div ratio of 1 (TRIMDIV=00h) to a div ratio of
                            128 (TRIMDIV=7Fh).

     15-11      Reserved
    —

      10-8      Reserved
    —

       7-2       Reserved
    —

       1-0       Trim Source
   TRIMSRC    Configures the external clock source to tune the SIRC. TRIMSRC must be configured before
                programming SIRCSTAT register for trim update.
                     00b - Reserved
                     01b - Reserved
                     10b - SOSC. This option requires that SOSC be divided using the TRIMDIV field to get a
                        frequency of 1 MHz.
                     11b - ROSC (32.768 kHz)


34.7.1.11 SIRC Trim Register (SIRCTRIM)

Offset


 Register                   Offset

 SIRCTRIM             210h


Function
Holds the trim values for the SIRC clock source. This register is loaded from IFR during reset.

                                           NOTE
                                        Writes to this register are protected by the TRIM_LOCK.



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1257 / 3791

<!-- page 1258 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R           0                                                          0
                                     FVCHTRIM                                             TCTRIM
   W

Reset     0      0      0       u1      u      u      u      u       0      0      0       u1      u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R        0                                                          0
                                     CLTRIM                                           CCOTRIM
   W

Reset     0      0       u1      u       u      u      u      u       0      0       u1      u       u      u      u      u

1. Reset values are loaded out of IFR.

Fields


       Field       Function
     31-29      Reserved
    —

     28-24       Calibrates the replica voltage in FSU for CCO to get well frequency at initial period
  FVCHTRIM

     23-21      Reserved
    —

     20-16      Trim Temp
   TCTRIM     Trim bus to calibrate the relationship between freq and temperature.

     15-14      Reserved
    —

      13-8     CL Trim
   CLTRIM     Trims bus to calibrate the freq of CCO in close loop mode to within approximately ±0.6% of the target
               12 MHz frequency.
       7-6       Reserved
    —

       5-0     CCO Trim
   CCOTRIM    Trims bus to calibrate the freq of CCO in open loop mode to within approximately ±2.5% of the target
               12 MHz frequency.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1258 / 3791

<!-- page 1259 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

34.7.1.12 SIRC Auto-trimming Status Register (SIRCSTAT)

Offset


 Register                   Offset

 SIRCSTAT             218h


Function
This register is loaded from IFR during reset.
This register is uploaded with the trim values generated by SIRC auto-trimming. SIRC auto-trimming is enabled when
SIRCTREN=1 and SIRCTRUP=1. When SIRCTREN=1 and SIRCTRUP=0 (write 10 or 11 to TRIMSRC), writes to this register are
allowed and values written to this register are used to trim SIRC clock.

                                           NOTE
           You must program TRIMSRC to 10 or 11. Writes to this register are allowed and values written to this register are
            used to trim the SIRC clock.


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R        0                                                          0
                                     CLTRIM                                           CCOTRIM
   W

Reset     0      0       u1      u       u      u      u      u       0      0       u1      u       u      u      u      u

1. Reset values are loaded out of IFR.

Fields


       Field       Function
     31-14      Reserved
    —

      13-8     CL Trim
   CLTRIM    Use CLTRIM to calibrate the frequency of CCO in close loop mode and to trim the SIRC clock to within
                 approximately ±0.6% of the target 12 MHz frequency.
       7-6       Reserved
    —

       5-0     CCO Trim

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1259 / 3791

<!-- page 1260 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
   CCOTRIM   Use CCOTRIM to calibrate the frequency of CCO in open loop mode and to coarsely trim the SIRC clock
                    to within approximately ±2.5% of the target 12 MHz frequency.

34.7.1.13 FIRC Control Status Register (FIRCCSR)

Offset


 Register                   Offset

 FIRCCSR              300h


Function
Contains control and status fields for the FIRC clock source.

Diagram

 Bits      31      30     29     28      27     26      25     24      23     22     21     20      19     18     17     16

        FIRCA                              FIRCE  FIRCS  FIRCV
    R                       0                                                                      0       CC   FIRCA                FIRCE   RR     EL     LD                                                               LK
                    CC...                      RR...
   W                              W1C

Reset     u       0      0      0       0      0       1      1       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

                                          TRIM_
    R        0              0                                            0                         0      0                             COAR    LO...   FIRCT  FIRCT                 FIRC_  FIRC_                FIRCS  FIRCE
                                               SE_...        RUP   REN                       FC...    SC...                TEN    N
   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       0      0      0      1


Fields


       Field       Function

      31       FIRC Frequency Accurate
   FIRCACC     Indicates if the FIRC clock source is accurate.
                     0b - FIRC is not enabled or clock is not accurate.
                     1b - FIRC is enabled and output clock is accurate. The clock is accurate after 4096 clock cycles of
                     144 MHz (RANGE=1) or 1365 clock cycles of 48 MHz(RANGE=0) from the FIRC analog.

      30       FIRC Accurate Interrupt Enable
  FIRCACC_IE   Generates an interrupt when FIRCACC is asserted.
                     0b - FIRCACC interrupt is not enabled

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1260 / 3791

<!-- page 1261 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1b - FIRCACC interrupt is enabled

     29-28      Reserved
    —

      27       FIRC Clock Error Interrupt Enable
  FIRCERR_IE   Generates an interrupt when FIRCERR is asserted.
                     0b - FIRCERR interrupt is not enabled
                     1b - FIRCERR interrupt is enabled

      26       FIRC Clock Error
   FIRCERR     This flag is reset on Chip POR only. You can also clear this flag by writing 1.
                     0b - Error not detected with the FIRC trimming
                     1b - Error detected with the FIRC trimming

      25       FIRC Selected
   FIRCSEL     Indicates if the FIRC clock source is selected as the system clock source.
                     0b - FIRC is not the system clock source
                     1b - FIRC is the system clock source

      24       FIRC Valid status
   FIRCVLD     Indicates if the FIRC clock source is valid.
                     0b - FIRC is not enabled or clock is not valid.
                     1b - FIRC is enabled and output clock is valid. The clock is valid after there is an output clock
                       from the FIRC analog.

      23       Lock Register
      LK      You can clear or set this field at any time. Use this field to prevent runaway code from changing FIRC
                  clock configurations.
                     0b - Control Status Register can be written
                     1b - Control Status Register cannot be written

     22-14      Reserved
    —

     13-12      Reserved
    —

      11       Coarse Auto Trim Bypass
              Use this bit is to bypass the coarse trim step when auto-trimming.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1261 / 3791

<!-- page 1262 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

 COARSE_TRIM       0b - FIRC coarse auto trim is not bypassed
   _BYPASS                     1b - FIRC coarse auto trim is bypassed

      10       FIRC TRIM LOCK
  TRIM_LOCK  When FIRCTREN and FIRCTRUP are enabled, the TRIM_LOCK register indicates when auto trimming
                       is complete and output FIRC frequency has locked to target FIRC range.

                                                 NOTE
                                                •  This field is automatically cleared if FIRCTREN and FIRCTRUP are not set.

                                                • TRIM_LOCK is not set when FIRCTCFG[TRIMSRC] = 2'b00, i.e., using full speed USB
                                  as the trim source.

                     0b - FIRC auto trim not locked to target frequency range
                     1b - FIRC auto trim locked to target frequency range

       9       FIRC Trim Update
  FIRCTRUP    Allows the FIRCSTAT to be updated by the auto-trimming hardware.
                     0b - Disables FIRC trimming updates
                     1b - Enables FIRC trimming updates

       8       FIRC 144 MHz Trim Enable (FIRCCFG[RANGE]=1)
  FIRCTREN    Enables the auto tirm of FIRC 144 MHz by an external clock source.
                     0b - Disables trimming FIRC to an external clock source
                     1b - Enables trimming FIRC to an external clock source

       7-6       Reserved
    —

       5       FIRC 144 MHz Clock to peripherals Enable
 FIRC_FCLK_PE Enables the FIRC 144 MHz clock for peripheral use.
   RIPH_EN                     0b - FIRC 144 MHz to peripherals is disabled
                     1b - FIRC 144 MHz to peripherals is enabled

       4       FIRC 48 MHz Clock to peripherals Enable
 FIRC_SCLK_P  Enables the FIRC 48 MHz clock for peripheral use.
  ERIPH_EN                     0b - FIRC 48 MHz to peripherals is disabled
                     1b - FIRC 48 MHz to peripherals is enabled

       3        Reserved
    —

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1262 / 3791

<!-- page 1263 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       2        Reserved
    —

       1       FIRC Stop Enable
  FIRCSTEN    Enables the FIRC clock source in Deep Sleep mode if FIRCEN is set.
                     0b - FIRC is disabled in Deep Sleep mode
                     1b - FIRC is enabled in Deep Sleep mode

       0       FIRC Enable
   FIRCEN     Enables the FIRC clock source.
                     0b - FIRC is disabled
                     1b - FIRC is enabled


34.7.1.14 FIRC Configuration Register (FIRCCFG)

Offset


 Register                   Offset

 FIRCCFG              308h


Function
Controls the clock frequency range select for the FIRC clock source.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                        0                                                                                            RANG
                                                                                                     E   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1263 / 3791

<!-- page 1264 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Fields


       Field       Function
      31-1      Reserved
    —

       0        Frequency Range
   RANGE    Use this field to select the 48 MHz or 144 MHz FIRC clock when used as system clock.

                                                 NOTE
                                                  This field can be changed while FIRC clock is enabled.

                     0b - 48 MHz FIRC clock selected
                     1b - 144 MHz FIRC clock selected


34.7.1.15 FIRC Trim Configuration Register (FIRCTCFG)

Offset


 Register                   Offset

 FIRCTCFG            30Ch


Function
Contains the auto trim clock source select and trim clock divider control for the FIRC clock source.

                                           NOTE
           You cannot change this register when FIRC tuning is enabled. When the FIRC tuning is enabled, writes to this
                register are ignored and there is no transfer error.


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                  0
                                                                                           TRIMDIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                   0                             0                                0
                                                                                                  TRIMSRC
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1264 / 3791

<!-- page 1265 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Fields


       Field       Function
     31-23      Reserved
    —

     22-16     FIRC Trim Predivider
   TRIMDIV     Divider of SOSC for FIRC trimming.

                                                 NOTE
                        When selecting SOSC as the FIRC trimming source, the TRIMDIV register must be set to
                                    correct div ratio to generate 1 MHz output reference trimming clock.

                                                 NOTE
                           TRIMDIV is an N-Divider supporting a div ratio of 1 (TRIMDIV=00h) to a div ratio of
                            128 (TRIMDIV=7Fh).

     15-11      Reserved
    —

      10-8      Reserved
    —

       7-2       Reserved
    —

       1-0       Trim Source
   TRIMSRC    Configures the external clock source to tune the FIRC. This field must be configured before programming
                  the FIRCSTAT register for trim update.
                     00b - USB0 Start of Frame (1 kHz). This option does not use TRIMDIV
                     01b - Reserved
                     10b - SOSC. This option requires that SOSC be divided using the TRIMDIV field to get a
                        frequency of 1 MHz.
                     11b - ROSC. 32.768 kHz


34.7.1.16 FIRC Trim Register (FIRCTRIM)

Offset


 Register                   Offset

 FIRCTRIM             310h


Function
The FIRC Trim Register holds the trim values for the FIRC clock source. This register is loaded from IFR during reset. These
values are used for trimming the two range (RANGE=0 and RANGE=1) frequencies of FIRC.



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1265 / 3791

<!-- page 1266 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

                                           NOTE
                                       Writes to this register are protected by TRIM LOCK register.


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R        0                                                                         0
                                  TRIMSTART                                                       TRIMTEMP
   W

Reset     0      0       u1      u       u      u      u      u       0      0      0      0       0      0       u1      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R        0
                                 TRIMCOAR                                        TRIMFINE
   W

Reset     0      0       u1      u       u      u      u      u       u1      u      u      u       u      u      u      u

1. Reset values are loaded out of IFR.

Fields


       Field       Function
     31-30      Reserved
    —

     29-24      Trim Start
  TRIMSTART   Current DAC adjustment of the replica cco current of start-up circuit.

     23-18      Reserved
    —

     17-16      Trim Temperature
  TRIMTEMP   Temperature coefficient compensation.

     15-14      Reserved
    —

      13-8       Trim Coarse
  TRIMCOAR   TRIMCOAR bits are used to coarsely trim the FIRC Clock to within approximately ±3.2% of the target
                  frequency.

       7-0       Trim Fine
   TRIMFINE    Current DAC adjustment of the CCO current. TRIMFINE bits are used to fine trim the FIRC Clock to
                    within approximately ±0.25% of the target frequency.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1266 / 3791

<!-- page 1267 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

34.7.1.17 FIRC Auto-trimming Status Register (FIRCSTAT)

Offset


 Register                   Offset

 FIRCSTAT             318h


Function
This register is loaded from IFR during reset.
Trim values are uploaded to this register by FIRC auto-trimming. FIRC auto-trimming is enabled when FIRCTREN=1 and
FIRCTRUP=1. When FIRCTREN=1 and FIRCTRUP=0 (write 10 or 11 to TRIMSRC), writes to this register are allowed and values
written to this register are used to trim FIRC clock.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R        0
                                 TRIMCOAR                                        TRIMFINE
   W

Reset     0      0       u1      u       u      u      u      u       u1      u      u      u       u      u      u      u

1. Reset values are loaded out of IFR.

Fields


       Field       Function
     31-16      Reserved
    —

     15-14      Reserved
    —

      13-8       Trim Coarse
  TRIMCOAR   Use this field for the current DAC adjustment of the base current and to coarsely trim the FIRC Clock to
                    within approximately ±3.2% of the target 144 MHz frequency.

       7-0       Trim Fine
   TRIMFINE   Use this field for the current DAC adjustment of the CCO current and to trim the FIRC Clock to within
                 approximately ±0.25% of the target 144 MHz frequency.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1267 / 3791

<!-- page 1268 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

34.7.1.18 ROSC Control Status Register (ROSCCSR)

Offset


 Register                   Offset

 ROSCCSR             400h


Function
Contains control and status bits for the ROSC clock source.

Diagram

 Bits      31     30     29     28      27     26      25     24      23     22     21     20      19     18     17      16

                                   ROSC  ROSC  ROSC
    R                   0                                                                  0                                    ERR   SEL    VLD                                    ROSC  ROSC                                                               LK
                                                                                    CMRE  CM
   W                             W1C

Reset     0      0      0      0       0      0       0      0       0      0      0      0       0      0      0       0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-27      Reserved
    —

      26     ROSC Clock Error
  ROSCERR    This flag is reset on Chip POR only. You can also clear this flag by writing 1.
                     0b - ROSC Clock Monitor is disabled or has not detected an error
                     1b - ROSC Clock Monitor is enabled and detected an RTC loss of clock error

      25     ROSC Selected
  ROSCSEL    Indicates if the ROSC clock source is selected as the system clock source.
                     0b - ROSC is not the system clock source
                     1b - ROSC is the system clock source

      24     ROSC Valid
  ROSCVLD    Indicates if the ROSC clock source is valid.
                     0b - ROSC is not enabled or clock is not valid

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1268 / 3791

<!-- page 1269 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1b - ROSC is enabled and output clock is valid

      23       Lock Register
      LK       Locks this register so that it cannot be written to.

                                                 NOTE
                                                       This field can be cleared or set at any time.

                     0b - Control Status Register can be written
                     1b - Control Status Register cannot be written

     22-18      Reserved
    —

      17     ROSC Clock Monitor Reset Enable
  ROSCCMRE   Enables the ROSCERR to generate a reset.
                     0b - Clock monitor generates an interrupt when an error is detected
                     1b - Clock monitor generates a reset when an error is detected

      16     ROSC Clock Monitor
  ROSCCM    Enables the clock monitor when ROSCVLD is set.
             When the clock monitor is disabled in a low power mode, it remains disabled until the clock valid flag is set
                   following exit from the low power mode.

                                                 NOTE
                          SIRC is the reference clock used to monitor the ROSC. You must program the SIRC clock
                                      to be enabled in order to monitor the ROSC. SIRC is automatically disabled in Power Down
                            and Deep Power Down modes, and the clock monitor of the ROSC is disabled.

                     0b - ROSC clock monitor is disabled
                     1b - ROSC clock monitor is enabled

      15-0      Reserved
    —


34.7.1.19 APLL Control Status Register (APLLCSR)

Offset


 Register                   Offset

 APLLCSR             500h


Function
Contains control and status fields for the APLL clock source.


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1269 / 3791

<!-- page 1270 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

                                           NOTE
                After the PLL is powered on, do not change the configuration registers except for fields that can be modified on-
                  the-fly.


Diagram

 Bits      31      30     29     28      27     26      25     24      23     22     21     20      19     18     17      16

                                        APLLE  APLLS  APLL_
    R    0                     0                                                          0               APLL_                    RR     EL      LO...                                         APLLC  APLLC                                                               LK
                     LO...                                                                     MRE   M
   W                              W1C

Reset     0       0      0      0       0      0       0      0       0      0      0      0       0      0      0       0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2       1       0

    R                                            0                                                                                                     Reserv  APLLS  APLLC  APLLP
                                                                                                  ed    TEN     LK...    WR...   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0       0       0


Fields


       Field       Function
      31       Reserved
    —

      30      APLL LOCK Interrupt Enable
 APLL_LOCK_IE  Generates an interrupt when APLL_LOCK is asserted.
                     0b - APLL_LOCK interrupt is not enabled
                     1b - APLL_LOCK interrupt is enabled

     29-27      Reserved
    —

      26      APLL Clock Error
   APLLERR    This flag is reset on-chip POR only. You can also clear this flag by writing 1.
                     0b - APLL Clock Monitor is disabled or has not detected an error
                     1b - APLL Clock Monitor is enabled and detected an error

      25      APLL Selected
   APLLSEL     Indicates if the APLL clock source is selected as the system clock source.
                     0b - APLL is not the system clock source
                     1b - APLL is the system clock source

      24      APLL LOCK
  APLL_LOCK

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1270 / 3791

<!-- page 1271 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                   Indicates when the APLL clock is locked. This flag is set when the number of reference clocks reaches
             LOCK_TIME after power on.
             APLL_LOCK clears with any of these conditions:
                           • APLL power down
                           • SOSC error when selected as the reference clock
                           • Writes to APLLCTRL, APLLNDIV, APLLMDIV, APLLSSCG0, or APLLSSCG1
                     0b - APLL is not powered on or not locked
                     1b - APLL is locked

      23       Lock Register
      LK       Locks this register so that it cannot be written to.

                                                 NOTE
                                             You can clear or set this field at any time.

                     0b - Control Status Register can be written
                     1b - Control Status Register cannot be written

     22-18      Reserved
    —

      17      APLL Clock Monitor Reset Enable
  APLLCMRE   Enables the APLLERR generate reset.
                     0b - Clock monitor generates an interrupt when an error is detected
                     1b - Clock monitor generates a reset when an error is detected

      16      APLL Clock Monitor
   APLLCM     Enables the clock monitor. If the clock source is disabled in a low power mode, then the clock monitor is
                  also disabled in that mode. The clock monitor is always disabled in Power Down and Deep Power Down
               modes. When the clock monitor is disabled in a low power mode, it remains disabled until the PLL_LOCK
                       is set following exit from the low power mode.
                     0b - APLL Clock Monitor is disabled
                     1b - APLL Clock Monitor is enabled

      15-4      Reserved
    —

       3        Reserved
    —

       2       APLL Stop Enable

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1271 / 3791

<!-- page 1272 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
  APLLSTEN   Enables the APLL clock source in Deep Sleep mode if APLLCLKEN and APLLPWREN are set.
                     0b - APLL is disabled in Deep Sleep mode
                     1b - APLL is enabled in Deep Sleep mode

       1       APLL Clock Enable
  APLLCLKEN   Enables the APLL clock source.
              Use this field to enable the analog PLL to send out the clock. You can configure APLLCLKEN and
            APLLPWREN at the same time because hardware only enables the clock when PLL_LOCK=1.

                                                 NOTE
                                            Ensure APLLCLKEN is enabled before using PLL clock.

                     0b - APLL clock is disabled
                     1b - APLL clock is enabled

       0       APLL Power Enable
 APLLPWREN  Powers up the APLL clock source.
                     0b - APLL clock is powered off
                     1b - APLL clock is powered on


34.7.1.20 APLL Control Register (APLLCTRL)

Offset


 Register                   Offset

 APLLCTRL            504h


Function
Contains control fields for the analog PLL.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22      21     20      19     18      17      16

                                                              Reserv
    R                   0                                           0              0                                                             ed            Reserv        BYPA  BYPA  BAND  LIMUP  BYPA                                     SOURCE
                                                                            ed             SSP...   SSP...   DIR...   OFF    SSP...
   W

Reset     0      0      0      0       0      0      0      0       0      0       0      0       0      0       0       0

 Bits       15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R    0
                           SELP                                     SELI                            SELR
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1272 / 3791

<!-- page 1273 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Fields


       Field       Function
     31-27      Reserved
    —

     26-25      Clock Source
   SOURCE     Configures the input clock source (clkin) for the APLL.

                                                 NOTE
                        A good quality input clock is necessary for good PLL performance. The input clock on
                                          clkin, optionally divided by the predivider ratio N, gives the reference frequency Fref for the
                                PLL-loop (Fref = Fin / N). For optimal performance, use a reference frequency within the
                                frequency range 5 MHz to 50 MHz.

                     00b - SOSC
                     01b - FIRC 48 MHz clock. FIRC_SCLK_PERIPH_EN must be set to use FIRC 48 MHz clock.
                     10b - Reserved
                     11b - No clock

      24       Reserved
    —

      23       Reserved
    —

      22       Reserved
    —

      21       Reserved
    —

      20       Bypass of the postdivider
 BYPASSPOST  Enables the bypass of the postdivider.
      DIV
                                                 NOTE
                                 Bypass the postdivider also bypasses the divide-by-2 divider in the postdivider.

                     0b - Use the postdivider.
                     1b - Bypass of the postdivider

      19       Bypass of the predivider
 BYPASSPREDI  Enables the bypass of the predivider.
      V                     0b - Use the predivider.
                     1b - Bypass of the predivider.

                                  Tablecontinuesonthenextpage...




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1273 / 3791

<!-- page 1274 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

      18        Control of the bandwidth of the PLL.
 BANDDIRECT  Enables the direct control of the bandwidth.
                     0b - The bandwidth is changed synchronously with the feedback-divider
                     1b - Modifies the bandwidth of the PLL directly

      17      Up Limiter
  LIMUPOFF    In spread spectrum and fractional applications, LIMUPOFF must be set to 1. In other applications,
             LIMUPOFF = 0.
                     0b - Application set to non-Spectrum and Fractional applications.
                     1b - Application set to Spectrum and Fractional applications.

      16       Bypass of Divide-by-2 Divider
 BYPASSPOST  Bypass of the divide-by-2 divider in the postdivider.
     DIV2                     0b - Use the divide-by-2 divider in the postdivider
                     1b - Bypass of the divide-by-2 divider in the postdivider

      15       Reserved
    —

     14-10      Bandwidth select P (proportional) value.
    SELP     See related content in Selecting bandwidth and PLL bandwidth settings.

       9-4       Bandwidth select I (integration) value.
     SELI      See related content in Selecting bandwidth and PLL bandwidth settings.

       3-0       Bandwidth select R (resistor) value.
    SELR     See related content in Selecting bandwidth and PLL bandwidth settings.


34.7.1.21 APLL Status Register (APLLSTAT)

Offset


 Register                   Offset

 APLLSTAT             508h


Function
Contains status fields for the analog PLL.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1274 / 3791

<!-- page 1275 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2       1       0

                                                                                              Reserv  PDIVA  MDIVA  NDIVA  Reserv
    R                                         0
                                                                                           ed    CK    CK    CK     ed

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0       0       0


Fields


       Field       Function
      31-5      Reserved
    —

       4        Reserved
    —

       3         Postdivider(P) ratio change acknowledge.
   PDIVACK   PDIVACK is response to the PREQ that the postdivider (P) ratio change is accepted by the analog PLL.
                     0b - The postdivider (P) ratio change is not accepted by the analog PLL
                     1b - The postdivider (P) ratio change is accepted by the analog PLL

       2        Feedback(M) divider ratio change acknowledge.
   MDIVACK   MDIVACK is response to the MREQ that the feedback (M) ratio change is accepted by the analog PLL.
                     0b - The feedback (M) ratio change is not accepted by the analog PLL
                     1b - The feedback (M) ratio change is accepted by the analog PLL

       1         Predivider(N) ratio change acknowledge.
   NDIVACK   NDIVACK is response to the NREQ that the predivider (N) ratio change is accepted by the analog PLL.
                     0b - The predivider (N) ratio change is not accepted by the analog PLL
                     1b - The predivider (N) ratio change is accepted by the analog PLL

       0        Reserved
    —





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1275 / 3791

<!-- page 1276 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

34.7.1.22 APLL N Divider Register (APLLNDIV)

Offset


 Register                   Offset

 APLLNDIV            50Ch


Function
Contains predivider ratio and change request fields for on-the-fly update.

Diagram

 Bits       31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                               0
      NREQ
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                              0
                                                                                       NDIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      1


Fields


       Field       Function

      31        Predivider ratio change request.
    NREQ      Normally, you must program the divider ratio of the different dividers (MDIV, NDIV, PDIV) when PLL is in
                   reset state (APLLPWREN = 0). However, you can also select the divider ratio on-the-fly with the help of a
               handshake protocol. Normally, you must program the divider ratio for MDIV, NDIV, and PDIV when the PLL
                       is in a reset state (APLLWREN=0). However, you can also select the divider ratio on-the-fly with the help of
               a handshake protocol:
                        1. Write to NDIV to select a new divider ratio
                        2. Write 1 to NREQ to request the ratio change
                        3. APLLSTAT[NDIVACK] returns 1 to confirm the ratio change is accepted
                        4. Write 0 to NREQ to end the ratio change request

                                                 NOTE
                                  Use the reset method if more than one divider ratio must be changed.

                                                 NOTE
                                During the on-the-fly divider ratio update process, the PLL clock must be switched off from
                                  the system clock. APLL_LOCK bit is cleared in this case. Every time Fref changes, a new
                                 value LOCK_TIME must be programmed. Before using the PLL clock, you must wait for
                         APLL_LOCK to set again to ensure the PLL is locked.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1276 / 3791

<!-- page 1277 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Predivider ratio change is not requested
                     1b - Predivider ratio change is requested

      30-8      Reserved
    —

       7-0        Predivider divider ratio (N-divider).
     NDIV       N-divider supports a divider ratio of 1 (NDIV=01h) to a divider ratio of 255 (NDIV=FFh).

                                                 NOTE
                                               Do not write 0 to this field.


34.7.1.23 APLL M Divider Register (APLLMDIV)

Offset


 Register                   Offset

 APLLMDIV             510h


Function
Contains feedback divider ratio and change request fields for on-the-fly update.

Diagram

 Bits       31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                               0
      MREQ
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                         MDIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      1


Fields


       Field       Function

      31       Feedback ratio change request.
   MREQ

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1277 / 3791

<!-- page 1278 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                  Normally, you must program the divider ratio for MDIV, NDIV, and PDIV when the PLL is in a reset
                   state (APLLWREN=0). However, you can also select the divider ratio on-the-fly with the help of a
               handshake protocol:
                        1. Write to MDIV to select a new divider ratio
                        2. Write 1 to MREQ to request the ratio change
                        3. APLLSTAT[MDIVACK] returns 1 to confirm the ratio change is accepted
                        4. Write 0 to MREQ to end the ratio change request

                                                 NOTE
                                  Use the reset method if more than one divider ratio must be changed.

                                                 NOTE
                                During the on-the-fly divider ratio update process, the PLL clock must be switched off from
                                  the system clock. APLL_LOCK bit is cleared in this case. Every time Fref changes, a new
                                 value LOCK_TIME must be programmed. Before using the PLL clock, you must wait for
                         APLL_LOCK to set again to ensure the PLL is locked.

                     0b - Feedback ratio change is not requested
                     1b - Feedback ratio change is requested

     30-16      Reserved
    —

      15-0      Feedback divider divider ratio (M-divider).
    MDIV       M-divider supports a divider ratio of 1 (MDIV=0001h) to a divider ratio of 65535 (MDIV=FFFFh).

                                                 NOTE
                                               Do not write 0 to this field.


34.7.1.24 APLL P Divider Register (APLLPDIV)

Offset


 Register                   Offset

 APLLPDIV             514h


Function
Contains post divider ratio and change request fields for on-the-fly update.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1278 / 3791

<!-- page 1279 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Diagram

 Bits       31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                               0
       PREQ
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                         0
                                                                                                    PDIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      1


Fields


       Field       Function

      31        Postdivider ratio change request
    PREQ      Normally, you must program the divider ratio for MDIV, NDIV, and PDIV when the PLL is in a reset
                   state (APLLWREN=0). However, you can also select the divider ratio on-the-fly with the help of a
               handshake protocol:
                        1. Write to PDIV to select a new divider ratio
                        2. Write 1 to PREQ to request the ratio change
                        3. APLLSTAT[PDIVACK] returns 1 to confirm the ratio change is accepted
                        4. Write 0 to PREQ to end the ratio change request

                                                 NOTE
                                  Use the reset method if more than one divider ratio must be changed.

                                                 NOTE
                                During the on-the-fly divider ratio update process, the PLL clock must be switched off from
                                  the system clock. APLL_LOCK bit is cleared in this case. Every time Fref changes, a new
                                 value LOCK_TIME must be programmed. Before using the PLL clock, you must wait for
                         APLL_LOCK to set again to ensure the PLL is locked.

                     0b - Postdivider ratio change is not requested
                     1b - Postdivider ratio change is requested

      30-5      Reserved
    —

       4-0        Postdivider divider ratio (P-divider)
     PDIV       P-divider supports a divider ratio of PDIV*2, where PDIV is from 1 (PDIV=1h) to 31 (PDIV=1Fh).
             BYPASSPOSTDIV2 can be set to bypass the divide-by-2 in the postdivider.

                                                 NOTE
                                               Do not write 0 to this field.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1279 / 3791

<!-- page 1280 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

34.7.1.25 APLL LOCK Configuration Register (APLLLOCK_CNFG)

Offset


 Register                   Offset

 APLLLOCK_CNFG      518h


Function
Contains the number of reference clocks to count before APLL is considered locked and valid.

                                           NOTE
                                 Writes to this register are protected by the TRIM_LOCK register register.


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                        0                                                                                                       LOCK_
                                                                                                                                                                                                  TI...   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                       LOCK_TIME
   W

Reset     0      1      0      0       1      1      1      1       0      1      0      0       1      1      0      0


Fields


       Field       Function
     31-17      Reserved
    —

      16-0       Configures the number of reference clocks to count before APLL is considered locked.
  LOCK_TIME                                                 NOTE
                           The lock time programmed in this register must be equal to the PLL 500 µs lock time plus
                                  the 300 refclk count startup: LOCK_TIME = 500 µs/Tref + 300, Fref = Fin/N (input frequency
                                   divided by predivider ratio).

                                                 NOTE
                                       Every time Fref changes, you must program a new LOCK_TIME value.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1280 / 3791

<!-- page 1281 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

34.7.1.26 APLL SSCG Status Register (APLLSSCGSTAT)

Offset


 Register                   Offset

 APLLSSCGSTAT       520h


Function
Contains status field for the SSCG block.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

                                                                                                    SS_M
    R                                                        0
                                                                                                                                                               DIV...

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-1      Reserved
    —

       0       SS_MDIV change acknowledge
 SS_MDIV_ACK SS_MDIV_ACK responses to the SS_MDIV_REQ. Indicates that the SS_MDIV, MF, MR, and MC change
                       is accepted by the analog PLL.
                     0b - The SS_MDIV, MF, MR, and MC ratio change is not accepted by the analog PLL
                     1b - The SS_MDIV, MF, MR, and MC ratio change is accepted by the analog PLL


34.7.1.27 APLL Spread Spectrum Control 0 Register (APLLSSCG0)

Offset


 Register                   Offset

 APLLSSCG0           524h


Function
Contains SS_MDIV.


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1281 / 3791

<!-- page 1282 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                       SS_MDIV_LSB
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                       SS_MDIV_LSB
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function

      31-0     SS_MDIV
 SS_MDIV_LSB  Represents the average feedback divider ratio Maverage.
                SS_MDIV[32:25] is the integer part of the feedback divider ratio.
                SS_MDIV[24:0] is the fractional part of the feedback divider ratio.
                Average feedback-divider ratio Maverage:

                                Maverage = SS_MDIV [32:25] dec + 2-25 × SS_MDIV[24:0] dec + DITHER × 2-26 = 2-25 × SS_MDIVdec + DITHER × 2-26


                  stepsize = 1/(225 * M average)

34.7.1.28 APLL Spread Spectrum Control 1 Register (APLLSSCG1)

Offset


 Register                   Offset

 APLLSSCG1           528h


Function
Contains SS_MDIV[32] and other control fields for the SSCG.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1282 / 3791

<!-- page 1283 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Diagram

 Bits       31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                               0
       SS_PD
   W

Reset     1      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1       0

    R               0                                  SEL_S  DITHE                                                   SS_M  SS_M
                                         MC           MR               MF
                                                     S_...    R                                                                                     DIV...    DIV...   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0       0


Fields


       Field       Function

      31     SSCG Power Down
    SS_PD     Puts SSCG into power down.

                                                 NOTE
                           You must configure DITHER, MC, MF, MR, and SS_MDIV[32:0] before SS_PD is cleared.

                     0b - SSCG is powered on
                     1b - SSCG is powered off

     30-12      Reserved
    —

      11      SS_MDIV select.
 SEL_SS_MDIV  Selects the SS_MDIV value.
                     0b - Feedback divider ratio is MDIV[15:0]
                     1b - Feedback divider ratio is SS_MDIV[32:0]

      10         Dither Enable
   DITHER     Enables dithering between two modulation frequencies in a random way. This decreases the probability that
                  the modulated waveform occurs with the same phase on a particular point on the screen.
                     0b - Dither is not enabled
                     1b - Dither is enabled

       9-8       Modulation Waveform Control
    MC       Compensation for the low-pass filtering of the PLL in order to get a triangular modulation at the output of the
              PLL and a flat frequency spectrum.

                                                 NOTE
                               The recommended setting for the modulation waveform control is MC[1:0] = 10.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1283 / 3791

<!-- page 1284 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     00b - MC[1:0] no compensation
                     11b - MC[1:0] maximum compensation

       7-5       Modulation Depth Control
    MR        Controls the programmable frequency modulation depth according to the following equation.
                Average feedback-divider ratio:

                                                                                         Fref × kss            kss
                                                                   fmodpk-pk =       =                                                                                  Fsso    SS_MDIV[32:25]dec

                           • MR[2:0] = 000 - k SS = 0 (no spread spectrum)
                           • MR[2:0] = 001 - k SS ≈ 0.5
                           • MR[2:0] = 010 - k SS ≈ 0.75
                           • MR[2:0] = 011 - k SS ≈ 1
                           • MR[2:0] = 100 - k SS ≈ 1.5
                           • MR[2:0] = 101 - k SS ≈ 2
                           • MR[2:0] = 110 - k SS ≈ 3
                           • MR[2:0] = 111 - k SS ≈ 4

       4-2       Modulation Frequency Control
     MF        Controls the programmable modulation frequency. The below modulation frequencies are examples based
               on a 4 MHz reference frequency (F ref).
               fm = F ref /N SS
                           • MF[2:0] = 000 - N SS = 512 (fm ≈ 7.8 kHz)
                           • MF[2:0] = 001 - N SS ≈ 384 (fm ≈ 10.4 kHz)
                           • MF[2:0] = 010 - N SS = 256 (fm ≈ 15.6 kHz)
                           • MF[2:0] = 011 - N SS = 128 (fm ≈ 31.3 kHz)
                           • MF[2:0] = 100 - N SS = 64 (fm ≈ 62.5 kHz)
                           • MF[2:0] = 101 - N SS = 32 (fm ≈ 125 kHz)
                           • MF[2:0] = 110 - N SS ≈ 24 (fm ≈ 166.6 kHz)
                           • MF[2:0] = 111 - N SS = 16 (fm ≈ 250 kHz)

       1        SS_MDIV[32:0] change request.
 SS_MDIV_REQ Change request for SS_MDIV, MF, MR, and MC.
                  Normally, you must program the divider ratio for SS_MDIV, MF, MR, and MC when the PLL is in a
                   reset state (APLLWREN=0). However, you can also select the divider ratio on-the-fly with the help of a
               handshake protocol:
                        1. Write to SS_MDIV, MF, MR, and MC to select a new divider ratio

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1284 / 3791

<!-- page 1285 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                        2. Write 1 to SS_MDIV_REQ to request the ratio change
                        3. SS_MDIV_ACK returns 1 to confirm the ratio change is accepted
                        4. Write 0 to SS_MDIV_REQ to end the ratio change request

                                                 NOTE
                                  Use the reset method if more than one divider ratio must be changed.

                                                 NOTE
                                During the on-the-fly divider ratio update process, the PLL clock must be switched off from
                                  the system clock. APLL_LOCK bit is cleared in this case. Every time Fref changes, a new
                                 value LOCK_TIME must be programmed. Before using the PLL clock, you must wait for
                         APLL_LOCK to set again to ensure the PLL is locked.

                     0b - SS_MDIV change is not requested
                     1b - SS_MDIV change is requested

       0       SS_MDIV[32]
 SS_MDIV_MSB See APLLSSCG0[SS_MDIV_LSB].


34.7.1.29 APLL Override Register (APLL_OVRD)

Offset


 Register                   Offset

 APLL_OVRD           5F4h


Function
Contains override fields for the analog PLL.

Diagram

 Bits       31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                               0        APLL_
           OV...   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1       0

    R                                                    0                                                                                                  APLLC  APLLP
                                                                                                                                                        LK...    WR...   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0       0





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1285 / 3791

<!-- page 1286 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Fields


       Field       Function

      31      APLL Override Enable
 APLL_OVRD_E  Enables the override control for APLL
     N                     0b - APLL override is disabled
                     1b - APLL override is enabled

      30-2      Reserved
    —

       1       APLL Clock Enable Override if APLL_OVRD_EN=1
 APLLCLKEN_O  Enables the APLL clock source.
    VRD                     0b - APLL clock is disabled
                     1b - APLL clock is enabled

       0       APLL Power Enable Override if APLL_OVRD_EN=1
 APLLPWREN_  Powers up the APLL clock source.
    OVRD
                                                 NOTE
                                                  Write 0 to SS_PD to use the Spread Spectrum function.

                     0b - APLL clock is powered off
                     1b - APLL clock is powered on


34.7.1.30 SPLL Control Status Register (SPLLCSR)

Offset


 Register                   Offset

 SPLLCSR             600h


Function
Contains control and status fields for SPLL clock source.

                                           NOTE
                After PLL is powered on, do not change the configuration registers except for fields that can be modified on-the-fly.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1286 / 3791

<!-- page 1287 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Diagram

 Bits      31      30     29     28      27     26      25     24      23     22     21     20      19     18     17      16

                                        SPLLE  SPLLS  SPLL_
    R    0                     0                                                          0               SPLL_                    RR     EL      LO...                                         SPLLC  SPLLC                                                               LK
                     LO...                                                                     MRE   M
   W                              W1C

Reset     0       0      0      0       0      0       0      0       0      0      0      0       0      0      0       0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2       1       0

    R                                            0                                                                                                     Reserv  SPLLS  SPLLC  SPLLP
                                                                                                  ed    TEN     LK...    WR...   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0       0       0


Fields


       Field       Function
      31       Reserved
    —

      30      SPLL LOCK Interrupt Enable
 SPLL_LOCK_IE  Generates an interrupt when SPLL_LOCK is asserted.
                     0b - SPLL_LOCK interrupt is not enabled
                     1b - SPLL_LOCK interrupt is enabled

     29-27      Reserved
    —

      26      SPLL Clock Error
   SPLLERR    This flag is reset on Chip POR only. You can also clear this flag by writing 1.
                     0b - SPLL Clock Monitor is disabled or has not detected an error
                     1b - SPLL Clock Monitor is enabled and detected an error

      25      SPLL Selected
   SPLLSEL     This flag indicates if the SPLL clock source is selected as the system clock source.
                     0b - SPLL is not the system clock source
                     1b - SPLL is the system clock source

      24      SPLL LOCK
  SPLL_LOCK   Indicates when the SPLL clock is locked. This flag is set when the number of reference clocks reaches
             LOCK_TIME after power on.
             SPLL_LOCK clears with any of these conditions:
                           • SPLL power down

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1287 / 3791

<!-- page 1288 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                           • SOSC error when selected as the reference clock
                           • Writes to SPLLCTRL, SPLLNDIV, SPLLMDIV, SPLLSSCG0, or SPLLSSCG1

                                                 NOTE
                           Use the SPLL_LOCK field to verify that the SPLL is locked after power-on. This
                                           field is cleared when you modify the SPLLCTRL, SPLLNDIV, SPLLMDIV, SPLLPDIV,
                          SPLLSSCG0, or SPLLSSCG1 configurations on-the-fly. In this case, you must wait for
                         SPLL_LOCK to set again to ensure PLL is locked before using the PLL clock.

                                                 NOTE
                           To monitor the SPLL clock, use the SPLLCM field to ensure that the SPLL Clock Monitor
                                            is enabled.

                     0b - SPLL is not powered on or not locked
                     1b - SPLL is locked

      23       Lock Register
      LK       Locks this register so that it cannot be written to.

                                                 NOTE
                                             You can clear or set this field at any time.

                     0b - Control Status Register can be written
                     1b - Control Status Register cannot be written

     22-18      Reserved
    —

      17      SPLL Clock Monitor Reset Enable
  SPLLCMRE   Enables the SPLLERR generate reset.
                     0b - Clock monitor generates an interrupt when an error is detected
                     1b - Clock monitor generates a reset when an error is detected

      16      SPLL Clock Monitor
   SPLLCM     Enables the clock monitor. If the clock source is disabled in a low power mode, then the clock monitor is
                  also disabled in that mode. The clock monitor is always disabled in Power Down and Deep Power Down
               modes. When the clock monitor is disabled in a low power mode, it remains disabled until the PLL_LOCK
                       is set following exit from that mode.
                     0b - SPLL Clock Monitor is disabled
                     1b - SPLL Clock Monitor is enabled

      15-4      Reserved
    —

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1288 / 3791

<!-- page 1289 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
       3        Reserved
    —

       2       SPLL Stop Enable
  SPLLSTEN   Enables the SPLL clock source in Deep Sleep mode if SPLLCLKEN and SPLLPWREN are set.
                     0b - SPLL is disabled in Deep Sleep mode
                     1b - SPLL is enabled in Deep Sleep mode

       1       SPLL Clock Enable
  SPLLCLKEN   Enables the SPLL clock source.
              Use this field to enable the analog PLL to send out the clock. You can configure SPLLCLKEN and
            SPLLPWREN at the same time because hardware only enables the clock when PLL_LOCK=1.

                                                 NOTE
                                            Ensure SPLLCLKEN is enabled before using PLL clock.

                     0b - SPLL clock is disabled
                     1b - SPLL clock is enabled

       0       SPLL Power Enable
 SPLLPWREN  Powers up the SPLL clock source.

                                                 NOTE
                                           Write 0 to SS_PD to use the spread spectrum and fractional functions.

                     0b - SPLL clock is powered off
                     1b - SPLL clock is powered on


34.7.1.31 SPLL Control Register (SPLLCTRL)

Offset


 Register                   Offset

 SPLLCTRL            604h


Function
Contains control fields for the analog PLL.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1289 / 3791

<!-- page 1290 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22      21     20      19     18      17      16

                                                              Reserv
    R                   0                                           0              0                                                             ed            Reserv        BYPA  BYPA  BAND  LIMUP  BYPA                                     SOURCE
                                                                            ed             SSP...   SSP...   DIR...   OFF    SSP...
   W

Reset     0      0      0      0       0      0      0      0       0      0       0      0       0      0       0       0

 Bits       15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R    0
                           SELP                                     SELI                            SELR
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-27      Reserved
    —

     26-25      Clock Source
   SOURCE     Configures the input clock source (clkin) for the SPLL.

                                                 NOTE
                        A good-quality input clock is necessary for good PLL performance. The input clock on
                                          clkin, optionally divided by the predivider ratio N, gives the reference frequency Fref for the
                                PLL-loop (Fref = Fin / N). For optimal performance, use a reference frequency within the
                                frequency range 5 MHz to 50 MHz.

                     00b - SOSC
                     01b - FIRC 48 MHz clock. FIRC_SCLK_PERIPH_EN must be set to use FIRC 48 MHz clock.
                     10b - Reserved
                     11b - No clock

      24       Reserved
    —

      23       Reserved
    —

      22       Reserved
    —

      21       Reserved
    —

      20       Bypass of the postdivider.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1290 / 3791

<!-- page 1291 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
 BYPASSPOST  Enables the bypass of the postdivider.
      DIV            0b - Use the postdivider
                     1b - Bypass of the postdivider

      19       Bypass of the predivider.
 BYPASSPREDI  Enables the bypass of the predivider.
      V                     0b - Use the predivider
                     1b - Bypass of the predivider

      18        Control of the bandwidth of the PLL.
 BANDDIRECT  Enables the direct control of the bandwidth.
                     0b - The bandwidth is changed synchronously with the feedback-divider
                     1b - Modifies the bandwidth of the PLL directly

      17      Up Limiter.
  LIMUPOFF    In spread spectrum and fractional applications, you must set LIMUPOFF to 1. In other applications,
             LIMUPOFF = 0.
                     0b - Application set to non-Spectrum and Fractional applications.
                     1b - Application set to Spectrum and Fractional applications.

      16       Bypass of Divide-by-2 Divider
 BYPASSPOST  Bypass of the divide-by-2 divider in the postdivider.
     DIV2                     0b - Use the divide-by-2 divider in the postdivider.
                     1b - Bypass of the divide-by-2 divider in the postdivider

      15       Reserved
    —

     14-10      Bandwidth select P (proportional) value.
    SELP     See related content in Selecting bandwidth and PLL bandwidth settings.

       9-4       Bandwidth select I (integration) value.
     SELI      See related content in Selecting bandwidth and PLL bandwidth settings.

       3-0       Bandwidth select R (resistor) value.
    SELR     See related content in Selecting bandwidth and PLL bandwidth settings.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1291 / 3791

<!-- page 1292 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

34.7.1.32 SPLL Status Register (SPLLSTAT)

Offset


 Register                   Offset

 SPLLSTAT             608h


Function
Contains status fields for the analog PLL.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2       1       0

                                                                                              Reserv  PDIVA  MDIVA  NDIVA  Reserv
    R                                         0
                                                                                           ed    CK    CK    CK     ed

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0       0       0


Fields


       Field       Function
      31-5      Reserved
    —

       4        Reserved
    —

       3         Postdivider (P) ratio change acknowledge
   PDIVACK   PDIVACK is response to the PREQ that the postdivider (P) ratio change is accepted by the analog PLL.
                     0b - The postdivider (P) ratio change is not accepted by the analog PLL
                     1b - The postdivider (P) ratio change is accepted by the analog PLL

       2       Feedback (M) divider ratio change acknowledge
   MDIVACK     Indicates to the MREQ that the feedback (M) ratio change is accepted by the analog PLL.
                     0b - The feedback (M) ratio change is not accepted by the analog PLL.
                     1b - The feedback (M) ratio change is accepted by the analog PLL.

       1         Predivider (N) ratio change acknowledge

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1292 / 3791

<!-- page 1293 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function
   NDIVACK     Indicates to the NREQ that the predivider (N) ratio change is accepted by the analog PLL.
                     0b - The predivider (N) ratio change is not accepted by the analog PLL.
                     1b - The predivider (N) ratio change is accepted by the analog PLL.

       0        Reserved
    —


34.7.1.33 SPLL N Divider Register (SPLLNDIV)

Offset


 Register                   Offset

 SPLLNDIV            60Ch


Function
Contains predivider ratio and change request fields for on-the-fly update.

Diagram

 Bits       31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                               0
      NREQ
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                              0
                                                                                       NDIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      1


Fields


       Field       Function

      31        Predivider ratio change request.
    NREQ      Normally, you must program the divider ratio of the different dividers (MDIV, NDIV, PDIV) when PLL is in
                   reset state (SPLLPWREN = 0). However, you can also select the divider ratio on-the-fly with the help of a
               handshake protocol. Normally, you must program the divider ratio for MDIV, NDIV, and PDIV when the PLL
                       is in a reset state (SPLLWREN=0). However, you can also select the divider ratio on-the-fly with the help of
               a handshake protocol:

                                  Tablecontinuesonthenextpage...




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1293 / 3791

<!-- page 1294 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                        1. Write to NDIV to select a new divider ratio
                        2. Write 1 to NREQ to request the ratio change
                        3. SPLLSTAT[NDIVACK] returns 1 to confirm the ratio change is accepted
                        4. Write 0 to NREQ to end the ratio change request

                                                 NOTE
                                  Use the reset method if more than one divider ratio must be changed.

                                                 NOTE
                                During the on-the-fly divider ratio update process, the PLL clock must be switched off from
                                  the system clock. SPLL_LOCK bit is cleared in this case. Every time Fref changes, a new
                                 value LOCK_TIME must be programmed. Before using the PLL clock, you must wait for
                         SPLL_LOCK to set again to ensure the PLL is locked.

                     0b - Predivider ratio change is not requested
                     1b - Predivider ratio change is requested

      30-8      Reserved
    —

       7-0        Predivider divider ratio (N-divider).
     NDIV       N-divider supports a divider ratio of 1 (NDIV=01h) to a divider ratio of 255 (NDIV=FFh).

                                                 NOTE
                                               Do not write 0 to this field.


34.7.1.34 SPLL M Divider Register (SPLLMDIV)

Offset


 Register                   Offset

 SPLLMDIV             610h


Function
Contains feedback divider ratio and change request fields for on-the-fly update.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1294 / 3791

<!-- page 1295 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Diagram

 Bits       31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                               0
      MREQ
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                         MDIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      1


Fields


       Field       Function

      31       Feedback ratio change request.
   MREQ      Normally, you must program the divider ratio for MDIV, NDIV, and PDIV when the PLL is in a reset
                   state (SPLLWREN=0). However, you can also select the divider ratio on-the-fly with the help of a
               handshake protocol:
                        1. Write to MDIV to select a new divider ratio
                        2. Write 1 to MREQ to request the ratio change
                        3. SPLLSTAT[MDIVACK] returns 1 to confirm the ratio change is accepted
                        4. Write 0 to MREQ to end the ratio change request

                                                 NOTE
                                  Use the reset method if more than one divider ratio must be changed.

                                                 NOTE
                                During the on-the-fly divider ratio update process, the PLL clock must be switched off from
                                  the system clock. SPLL_LOCK bit is cleared in this case. Every time Fref changes, a new
                                 value LOCK_TIME must be programmed. Before using the PLL clock, you must wait for
                         SPLL_LOCK to set again to ensure the PLL is locked.

                     0b - Feedback ratio change is not requested
                     1b - Feedback ratio change is requested

     30-16      Reserved
    —

      15-0      Feedback divider divider ratio (M-divider).
    MDIV       M-divider supports a divider ratio of 1 (MDIV=0001h) to a divider ratio of 65535 (MDIV=FFFFh).

                                                 NOTE
                                               Do not write 0 to this field.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1295 / 3791

<!-- page 1296 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

34.7.1.35 SPLL P Divider Register (SPLLPDIV)

Offset


 Register                   Offset

 SPLLPDIV             614h


Function
Contains postdivider ratio and change request fields for an on-the-fly update.

Diagram

 Bits       31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                               0
       PREQ
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                         0
                                                                                                    PDIV
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      1


Fields


       Field       Function

      31        Postdivider ratio change request
    PREQ      Normally, you must program the divider ratio for MDIV, NDIV, and PDIV when the PLL is in a reset
                   state (SPLLWREN=0). However, you can also select the divider ratio on-the-fly with the help of a
               handshake protocol:
                        1. Write to PDIV to select a new divider ratio
                        2. Write 1 to PREQ to request the ratio change
                        3. SPLLSTAT[PDIVACK] returns 1 to confirm the ratio change is accepted
                        4. Write 0 to PREQ to end the ratio change request

                                                 NOTE
                                  Use the reset method if more than one divider ratio must be changed.

                                                 NOTE
                                During the on-the-fly divider ratio update process, the PLL clock must be switched off from
                                  the system clock. SPLL_LOCK bit is cleared in this case. Every time Fref changes, a new
                                 value LOCK_TIME must be programmed. Before using the PLL clock, you must wait for
                         SPLL_LOCK to set again to ensure the PLL is locked.

                                  Tablecontinuesonthenextpage...




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1296 / 3791

<!-- page 1297 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     0b - Postdivider ratio change is not requested
                     1b - Postdivider ratio change is requested

      30-5      Reserved
    —

       4-0        Postdivider divider ratio (P-divider)
     PDIV       P-divider supports a divider ratio of PDIV*2, where PDIV is from 1 (PDIV=1h) to 31 (PDIV=1Fh).
             BYPASSPOSTDIV2 can be set to bypass the divide by 2 in the postdivider.

                                                 NOTE
                                               Do not write 0 to this field.


34.7.1.36 SPLL LOCK Configuration Register (SPLLLOCK_CNFG)

Offset


 Register                   Offset

 SPLLLOCK_CNFG      618h


Function
Configures the number of reference clocks to count before SPLL is considered locked and valid.

                                           NOTE
                                     Writes to this register are protected by the TRIM_LOCK register.


Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                        0                                                                                                       LOCK_
                                                                                                                                                                                                  TI...   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                       LOCK_TIME
   W

Reset     0      1      0      0       1      1      1      1       0      1      0      0       1      1      0      0





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1297 / 3791

<!-- page 1298 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)

Fields


       Field       Function
     31-17      Reserved
    —

      16-0       Configures the number of reference clocks to count before SPLL is considered locked.
  LOCK_TIME                                                 NOTE
                           The lock time programmed in this register must be equal to meet the PLL 500 µs lock
                                 time plus the 300 refclk count startup, LOCK_TIME = 500 µs/Tref + 300, Fref = Fin/N (input
                                frequency divided by predivider ratio).

                                                 NOTE
                                      Every time Fref changes, a new value LOCK_TIME must be programmed.


34.7.1.37 SPLL SSCG Status Register (SPLLSSCGSTAT)

Offset


 Register                   Offset

 SPLLSSCGSTAT       620h


Function
Contains status field for the SSCG block.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

                                                                                                    SS_M
    R                                                        0
                                                                                                                                                               DIV...

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-1      Reserved

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1298 / 3791

<!-- page 1299 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

    —

       0       SS_MDIV change acknowledge
 SS_MDIV_ACK  Indicates to the SS_MDIV_REQ that the SS_MDIV, MF, MR, and MC change is accepted by the analog
                PLL.
                     0b - The SS_MDIV, MF, MR, and MC ratio change is not accepted by the analog PLL
                     1b - The SS_MDIV, MF, MR, and MC ratio change is accepted by the analog PLL


34.7.1.38 SPLL Spread Spectrum Control 0 Register (SPLLSSCG0)

Offset


 Register                   Offset

 SPLLSSCG0           624h


Function
Contains SS_MDIV[31:0].

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                       SS_MDIV_LSB
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                       SS_MDIV_LSB
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function

      31-0      SS_MDIV[31:0]
 SS_MDIV_LSB  Represents the average feedback divider ratio Maverage.
                SS_MDIV[32:25] is the integer part of the feedback divider ratio.
                SS_MDIV[24:0] is the fractional part of the feedback divider ratio.

                                  Tablecontinuesonthenextpage...




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1299 / 3791

<!-- page 1300 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


       Field       Function

                                                 NOTE
                              SS_MDIV[32:0] must not be programmed to value 0 for spread spectrum and
                                       fractional function.

                Average feedback-divider ratio M average:

                                Maverage = SS_MDIV [32:25] dec + 2-25 × SS_MDIV[24:0] dec + DITHER × 2-26 = 2-25 × SS_MDIVdec + DITHER × 2-26


                  stepsize = 1/(225 * M average)

34.7.1.39 SPLL Spread Spectrum Control 1 Register (SPLLSSCG1)

Offset


 Register                   Offset

 SPLLSSCG1           628h


Function
Contains SS_MDIV[32] and other control fields for SSCG.

Diagram

 Bits       31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                               0
       SS_PD
   W

Reset     1      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1       0

    R               0                                  SEL_S  DITHE                                                   SS_M  SS_M
                                         MC           MR               MF
                                                     S_...    R                                                                                     DIV...    DIV...   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0       0


Fields


       Field       Function

      31     SSCG Power Down
    SS_PD     Puts SSCG into power down.

                                  Tablecontinuesonthenextpage...





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1300 / 3791

<!-- page 1301 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                                                 NOTE
                           The configuration of DITHER, MC, MF, MR, and SS_MDIV[32:0] must be written before
                        SS_PD is cleared.

                     0b - SSCG is powered on
                     1b - SSCG is powered off

     30-12      Reserved
    —

      11      SS_MDIV select.
 SEL_SS_MDIV  Selects the SS_MDIV value.
                     0b - Feedback divider ratio is MDIV[15:0]
                     1b - Feedback divider ratio is SS_MDIV[32:0]

      10         Dither Enable
   DITHER     Enables dithering between two modulation frequencies in a random way. This decreases the probability of
                  the modulated waveform occurring with the same phase on a particular point on the screen.
                     0b - Dither is not enabled
                     1b - Dither is enabled

       9-8       Modulation Waveform Control
    MC       Compensation for the low-pass filtering of the PLL in order to get a triangular modulation at the output of the
              PLL and a flat frequency spectrum.

                                                 NOTE
                               The recommended setting for the modulation waveform control is MC[1:0] = 10.

                     00b - MC[1:0] no compensation
                     11b - MC[1:0] maximum compensation

       7-5       Modulation Depth Control
    MR        Controls the programmable frequency modulation depth according to the following equation.
                Average feedback-divider ratio:

                                                                                         Fref × kss            kss
                                                                   fmodpk-pk =       =                                                                                  Fsso    SS_MDIV[32:25]dec

                           • MR[2:0] = 000 - k SS = 0 (no spread spectrum)
                           • MR[2:0] = 001 - k SS ≈ 0.5
                           • MR[2:0] = 010 - k SS ≈ 0.75
                           • MR[2:0] = 011 - k SS ≈ 1

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1301 / 3791

<!-- page 1302 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                           • MR[2:0] = 100 - k SS ≈ 1.5
                           • MR[2:0] = 101 - k SS ≈ 2
                           • MR[2:0] = 110 - k SS ≈ 3
                           • MR[2:0] = 111 - k SS ≈ 4

       4-2       Modulation Frequency Control
     MF        Controls the programmable modulation frequency. The below modulation frequencies are examples based
               on a 4 MHz reference frequency (F ref).
               fm = F ref /N SS
                           • MF[2:0] = 000 - N SS = 512 (fm ≈ 7.8 kHz)
                           • MF[2:0] = 001 - N SS ≈ 384 (fm ≈ 10.4 kHz)
                           • MF[2:0] = 010 - N SS = 256 (fm ≈ 15.6 kHz)
                           • MF[2:0] = 011 - N SS = 128 (fm ≈ 31.3 kHz)
                           • MF[2:0] = 100 - N SS = 64 (fm ≈ 62.5 kHz)
                           • MF[2:0] = 101 - N SS = 32 (fm ≈ 125 kHz)
                           • MF[2:0] = 110 - N SS ≈ 24 (fm ≈ 166.6 kHz)
                           • MF[2:0] = 111 - N SS = 16 (fm ≈ 250 kHz)

       1        SS_MDIV[32:0] change request.
 SS_MDIV_REQ Change request for SS_MDIV, MF, MR, and MC.
                  Normally, you must program the divider ratio for SS_MDIV, MF, MR, and MC when the PLL is in a
                   reset state (SPLLWREN=0). However, you can also select the divider ratio on-the-fly with the help of a
               handshake protocol:
                        1. Write to SS_MDIV, MF, MR, and MC to select a new divider ratio
                        2. Write 1 to SS_MDIV_REQ to request the ratio change
                        3. SS_MDIV_ACK returns 1 to confirm the ratio change is accepted
                        4. Write 0 to SS_MDIV_REQ to end the ratio change request

                                                 NOTE
                                  Use the reset method if more than one divider ratio must be changed.

                                                 NOTE
                                During the on-the-fly divider ratio update process, the PLL clock must be switched off from
                                  the system clock. SPLL_LOCK bit is cleared in this case. Every time Fref changes, a new
                                 value LOCK_TIME must be programmed. Before using the PLL clock, you must wait for
                         SPLL_LOCK to set again to ensure the PLL is locked.

                     0b - SS_MDIV change is not requested
                     1b - SS_MDIV change is requested

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1302 / 3791

<!-- page 1303 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

       0       SS_MDIV[32]
 SS_MDIV_MSB See SS_MDIV_LSB.


34.7.1.40 SPLL Override Register (SPLL_OVRD)

Offset


 Register                   Offset

 SPLL_OVRD           6F4h


Function
Contains override fields for the analog PLL.

Diagram

 Bits       31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                               0        SPLL_
           OV...   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1       0

    R                                                    0                                                                                                  SPLLC  SPLLP
                                                                                                                                                        LK...    WR...   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0       0


Fields


       Field       Function

      31      SPLL Override Enable
 SPLL_OVRD_E  Enable the override control for SPLL
     N                     0b - SPLL override is disabled
                     1b - SPLL override is enabled

      30-2      Reserved
    —

       1       SPLL Clock Enable Override if SPLL_OVRD_EN=1
                Enable the SPLL clock source.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1303 / 3791

<!-- page 1304 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

 SPLLCLKEN_O       0b - SPLL clock is disabled
    VRD                     1b - SPLL clock is enabled

       0       SPLL Power Enable Override if SPLL_OVRD_EN=1
 SPLLPWREN_  Power up the SPLL clock source.
    OVRD
                                                 NOTE
                                                  Write 0 to SS_PD to use the Spread Spectrum function.

                     0b - SPLL clock is powered off
                     1b - SPLL clock is powered on


34.7.1.41 UPLL Control Status Register (UPLLCSR)

Offset


 Register                   Offset

 UPLLCSR             700h


Function
Contains control and status bits for UPLL clock source.

Diagram

 Bits      31     30     29     28      27     26      25     24      23     22     21     20      19     18     17      16

                                       UPLLE  UPLLS  UPLLV
    R                   0                                                                  0                                    RR     EL     LD                                         UPLLC UPLLC                                                               LK
                                                                                    MRE   M
   W                             W1C

Reset     0      0      0      0       0      0       0      0       0      0      0      0       0      0      0       0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-27      Reserved
    —

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1304 / 3791

<!-- page 1305 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

      26      UPLL Clock Error
   UPLLERR    This flag is reset on Chip POR only.
                     0b - UPLL Clock Monitor is disabled or has not detected an error
                     1b - UPLL Clock Monitor is enabled and detected an error

      25      UPLL Selected
   UPLLSEL     Indicates whether the UPLL clock source is selected as the system clock source.
                     0b - UPLL is not the system clock source
                     1b - UPLL is the system clock source

      24      UPLL Valid
   UPLLVLD     Indicates when the UPLL clock is valid.
                     0b - UPLL is not enabled or clock is not valid
                     1b - UPLL is enabled and output clock is valid

      23       Lock Register
      LK       Locks this register so that it cannot be written to.

                                                 NOTE
                                             You can clear or set this field at any time.

                     0b - Control Status Register can be written
                     1b - Control Status Register cannot be written

     22-18      Reserved
    —

      17      UPLL Clock Monitor Reset Enable
  UPLLCMRE   Enables the UPLLERR generate reset.
                     0b - Clock monitor generates an interrupt when an error is detected
                     1b - Clock monitor generates a reset when an error is detected

      16      UPLL Clock Monitor
   UPLLCM    Enables the clock monitor. If the clock source is disabled in a low power mode, then the clock monitor is
                  also disabled in that mode. The clock monitor is always disabled in Power Down and Deep Power Down
               modes. When the clock monitor is disabled in a low power mode, it remains disabled until the clock valid
                     flag is set following exit from that mode.
                     0b - UPLL Clock Monitor is disabled
                     1b - UPLL Clock Monitor is enabled

      15-0      Reserved

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1305 / 3791

<!-- page 1306 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

    —


34.7.1.42 LDO Control and Status Register (LDOCSR)

Offset


 Register                   Offset

 LDOCSR              800h


Function
Contains control bits and status bit for the LDO.

Diagram

 Bits       31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

       VOUT_
    R                                                               0
       OK

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                         0                                                                         LDOB                    LDOE
                                                                                        VOUT_SEL
                                                                                                         YPA...                    N   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       1      0      0      0


Fields


       Field       Function

      31      LDO VOUT OK Inform.
  VOUT_OK    This flag shows the status of LDO output VOUT.
                     0b - LDO output VOUT is not OK
                     1b - LDO output VOUT is OK

      30-5      Reserved
    —

       4      LDO Bypass
 LDOBYPASS  LDO is bypassed.

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1306 / 3791

<!-- page 1307 -->

NXP Semiconductors
                                                                                        System Clock Generator (SCG)


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                                                 NOTE
                                             Writes to this register are protected by the TRIM_LOCK register.

                     0b - LDO is not bypassed
                     1b - LDO is bypassed

       3-1     LDO output voltage select
  VOUT_SEL    Sets the LDO output voltage level.

                                                 NOTE
                                             Writes to this register are protected by the TRIM_LOCK register.

                     000b - VOUT = 1V
                     001b - VOUT = 1V
                     010b - VOUT = 1V
                     011b - VOUT = 1.05V
                     100b - VOUT = 1.1V
                     101b - VOUT = 1.15V
                     110b - VOUT = 1.2V
                     111b - VOUT = 1.25V

       0      LDO Enable
   LDOEN    LDO is enabled.

                                                 NOTE
                                                  Disable LDO before entering the Power Down mode.

                     0b - LDO is disabled
                     1b - LDO is enabled





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1307 / 3791
