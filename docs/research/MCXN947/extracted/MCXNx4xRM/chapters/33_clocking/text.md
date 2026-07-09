# 33 Clocking

Source: `docs/mcxn947/MCXNx4xRM.pdf`
Pages: 1197-1215

<!-- page 1197 -->

NXP Semiconductors

Chapter 33
Clocking

33.1  Configuring the main clock and system clock
The clock source for the registers and memories is derived from the main clock. The main clock can be selected from the sources
listed in step 1 below.
The main clock, after optionally being divided by the CPU clock divider, is called the system clock. The system clock provides a
clock signal to the core, memories, and peripherals (register interfaces and peripheral clocks).
   1. Select the main clock in the SCG RCCR register from these options:
           • 12-MHz free-running oscillator (FRO) output (FRO_12M) from internal oscillator.
           • FRO high-speed output (fro_hf) from internal oscillator. By default, its speed is 48 MHz. fro_hf is default main clock.
           • External oscillator.
           • Output of PLL0.
           • Output of PLL1.
           • RTC 32-kHz oscillator.
           • Output of USB PLL (usb_pll_clk).
   2. Select the divider value for the system clock AHBCLKDIV register.
   3. Enable the clock for the memories and peripherals used in the application.

                                           NOTE
           You must disable the peripheral clock (via AHBCLKCTRLx registers) when performing software reset on
               peripherals (via PRESETCTRLx registers). After asserting and de-asserting reset, you can enable the respective
               peripheral clock.

33.2 Clock generation
The system control block facilitates clock generation. Many clocking variations are possible. Figure 127 gives an overview of
potential clock options. Table 320 describes signals on the clocking diagram. The maximum clock frequency is 150 MHz.

                                           NOTE
            The indicated clock multiplexers shown in Figure 127 are synchronized. For the multiplexer to switch gracefully
             between the two clocks without glitches, the currently selected clock and the clock to be switched to must be
               running. Other clock multiplexers are not synchronized. If a glitch-free output is needed, the output divider can be
             stopped and restarted gracefully during switching.

FRO_12M provides a 1-MHz clock (clk_1m) which is independent of watchdog 0. This clock is accurate within a limited range (±3%
of each value) of the temperature, voltage, and silicon processing variations made during assembly after trimming. See the data
sheet for specifications. To determine the actual watchdog oscillator output, use the frequency measuring block. See Chip-specific
Frequency Measurement information.
The device contains two PLLs (PLL0 and PLL1) that can be configured to use a number of clock inputs and produce an output
clock. This output clock can then be used to run most on-chip functions. You can monitor the output of the PLL via the CLKOUT pin.

                                           NOTE
            The maximum allowed frequency for the main clock and system clock (to CPU0, AHB bus, sync, and others) is
            150 MHz.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1197 / 3791

<!-- page 1198 -->

NXP Semiconductors
                                                                                                                           Clocking

Table 320. Clocking diagram signal name descriptions

 Name        Description

 lp_osc     Low power clock source. It is selected as either FRO_16K or XTAL32K in the RTC register.

 system_clk  AHB bus clock. Used by the CPU, AHB bus, APB bus, and others. Derived from main_clk.

 slow_clk     Slow clock derived from system_clk divided by 4. slow_clk provides the bus clock for FMU, SPC, CMC, TDET,
           CMP0, CMP1, VBAT, LPTRM0, LPTRM1, RTC, GPIO5, PORT5, and TSI.
 clk_in         Internal clock from the external oscillator. clk_in connects to different modules as three functional clock
             source options.
                      • Ungated, clk_in goes to ADC0, ADC1, DAC0, DAC1, DAC2, CMP0, CMP1, CMP2, ENET, and FlexIO.
                      • Gated by SYSCON.CLOCK_CTRL[1], clk_in goes to FREQME, USBHS, LPTMR0, and LPTMR1
                      • Gated by SYSCON.CLOCK_CTRL[5], clk_in goes to MICFIL, EMVSIM0, EMVSIM1, CAN0, CAN1, I3C0,
                  I3C1, SAI0, SAI1, SINC Filter (SINC), TSI, USBFS, SCT, uSDHC, and CLKOUT.

 CLKOUT    Clock used for debugging purposes or to drive some external logic (see data sheet for limitations on pin output
              frequency). Many on-chip clocks can be selected to be output on the CLKOUT function. To use CLKOUT, you
            must connect it to a pin by selecting it in the PORT block.

 fro_12m    12-MHz output from FRO_12M.

 clk_1m     1-MHz output of FRO_12M.

 fro_hf       Clock output from FRO_144M. SCG_FIRCCFG[RANGE] controls the frequency. This clock can only be used
                 for USB devices and is not reliable for the USB host timing requirements for data signaling rate.

 fro_hf_div     fro_hf clock, potentially divided by the FRO_HF divider. Used to save power, when fro_hf is faster than needed.

 clk_16k     16-kHz clock output from FRO_16K.

 main_clk    Main clock used by the CPU and AHB bus, and potentially many others. The main clock and its source
               selection are shown in Figure 127.
 mclk_in     The MCLK input function, when it is connected to a pin by selecting it in the port control block. There are two
         MCLK signals: mclk_in_0 connects to SAI0, and mclk_in_1 connects to SAI1.

 none         Tied-off source. To save power when the output of the related multiplexer is not used, select this source.

 pll_clk_div   Output of PLL0 or PLL1, potentially divided. The output may be divided to save power, or because the
             frequency of the raw PLL output may be too high to be used directly.
 pll0_clk      Output of PLL0. PLL0 and its source selection are shown in Figure 127.

 pll1_clk      Output of PLL1. PLL1 and its source selection are shown in Figure 127.

 wdt_clk      Clock to the watchdog timer.

 XTAL32K    Output of the 32-kHz crystal oscillator.

Table 321. Maximum clock speed of modules in run modes
 Clock                          Maximum speed1
 clk_in                      OD mode: 50 MHz
                           SD mode: 50 MHz

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1198 / 3791

<!-- page 1199 -->

NXP Semiconductors
                                                                                                                           Clocking

Table 321. Maximum clock speed of modules in run modes (continued)
 Clock                          Maximum speed1

                        MD mode: 50 MHz

 fro_hf                      OD mode: 144 MHz
                           SD mode: 144 MHz
                        MD mode: 48 MHz

 pll0_clk                    OD mode: 150 MHz
                           SD mode: 100 MHz
                        MD mode: 50 MHz

 pll1_clk                    OD mode: 300 MHz
                           SD mode: 200 MHz
                        MD mode: 100 MHz

 usb_pll_clk and ref_pfd           OD mode: 150 MHz
                           SD mode: 100 MHz
                        MD mode: Non-functional

 utmi_clk                    OD mode: 30 MHz
                           SD mode: 30 MHz
                        MD mode: Non-functional

 system_clk                  OD mode: 150 MHz
                           SD mode: 100 MHz
                        MD mode: 50 MHz

 CLKOUT                   OD mode: 100 MHz
                           SD mode: 64 MHz
                        MD mode: 32 MHz

 pll1_clk0                    OD mode: 150 MHz
                           SD mode: 100 MHz
                        MD mode: 50 MHz

 pll1_clk1                    OD mode: 150 MHz
                           SD mode: 100 MHz
                        MD mode: 50 MHz

 pll_clk_div                   OD mode: 150 MHz
                           SD mode: 100 MHz
                        MD mode: 50 MHz

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1199 / 3791

<!-- page 1200 -->

NXP Semiconductors
                                                                                                                           Clocking

Table 321. Maximum clock speed of modules in run modes (continued)
 Clock                          Maximum speed1
 fro_hf_div                   OD mode: 144 MHz
                           SD mode: 72 MHz
                        MD mode: 48 MHz

 Trace clock                  OD mode: 96 MHz
                           SD mode: 96 MHz
                        MD mode: 50 MHz

 System tick clock               OD mode: 25 MHz
                           SD mode: 16 MHz
                        MD mode: 8 MHz

 FlexSPI serial root clock           OD mode: 300 MHz
                           SD mode: 200 MHz
                        MD mode: 100 MHz

 ADC clock                   OD mode: 60 MHz
                           SD mode: 48 MHz
                        MD mode: 24 MHz

 DAC clock                   OD mode: 50 MHz
                           SD mode: 32 MHz
                        MD mode: 16 MHz

 CTIMER clock                OD mode: 150 MHz
                           SD mode: 100 MHz
                        MD mode: 50 MHz

 SCTIMER clock               OD mode: 150 MHz
                           SD mode: 100 MHz
                        MD mode: 50 MHz

 OSTIMER clock               OD mode: 1 MHz
                           SD mode: 1 MHz
                        MD mode: 1 MHz

 wdt0_clk                    OD mode: 1 MHz
                           SD mode: 1 MHz
                        MD mode: 1 MHz

 wdt1_clk                    OD mode: 24 MHz

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1200 / 3791

<!-- page 1201 -->

NXP Semiconductors
                                                                                                                           Clocking

Table 321. Maximum clock speed of modules in run modes (continued)
 Clock                          Maximum speed1

                           SD mode: 24 MHz
                        MD mode: 8 MHz

 USBFS clock                 OD mode: 48 MHz
                           SD mode: 48 MHz
                        MD mode: 48 MHz

 EMVSIM clock                OD mode: 50 MHz
                           SD mode: 32 MHz
                        MD mode: 16 MHz

 CMP clock                  OD mode: 25 MHz
                           SD mode: 16 MHz
                        MD mode: 8 MHz

 LP_FLEXCOMM0/1/2 clock         OD mode: 75 MHz
                           SD mode: 50 MHz
                        MD mode: 25 MHz

 LP_FLEXCOMM3/4/5 clock         OD mode: 100 MHz
                           SD mode: 100 MHz
                        MD mode: 50 MHz

 LP_FLEXCOMM6/7/8/9 clock        OD mode: 150 MHz
                           SD mode: 100 MHz
                        MD mode: 50 MHz

 CAN clock                   OD mode: 150 MHz
                           SD mode: 100 MHz
                        MD mode: 50 MHz

 FlexIO clock                  OD mode: 150 MHz
                           SD mode: 100 MHz
                        MD mode: 50 MHz

 uSDHC clock                 OD mode: 104 MHz
                           SD mode: 100 MHz
                        MD mode: 50 MHz

 SINC Filter clock               OD mode: 150 MHz
                           SD mode: 100 MHz

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1201 / 3791

<!-- page 1202 -->

NXP Semiconductors
                                                                                                                           Clocking

Table 321. Maximum clock speed of modules in run modes (continued)
 Clock                          Maximum speed1

                        MD mode: 50 MHz

 I3C FCLK                   OD mode: 25 MHz
                           SD mode: 25 MHz
                        MD mode: 25 MHz

 CLK_SLOW_TC               OD mode: 25 MHz
                           SD mode: 25 MHz
                        MD mode: 25 MHz

 SAI clock                   OD mode: 50 MHz
                           SD mode: 32 MHz
                        MD mode: 16 MHz

 ENET RMII clock               OD mode: 50 MHz
                           SD mode: 50 MHz
                        MD mode: Not functional

 ENET PTP clock               OD mode: 150 MHz
                           SD mode: 100 MHz
                        MD mode: Not functional

 MICFIL clock                 OD mode: 50 MHz
                           SD mode: 32 MHz
                        MD mode: 16 MHz

 TSI clock                   OD mode: 25 MHz
                           SD mode: 25 MHz
                        MD mode: 25 MHz

1. OD = Over Drive, SD = Standard Drive, MD = Mid Drive





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1202 / 3791

<!-- page 1203 -->

NXP Semiconductors
                                                                                                                           Clocking



                                             SCG
                                                     clkout
               XTAL
                             System OSC                                                 0-50 MHz                                                                                                                                    clk_in
                           (OSC_SYS)   clkout_byp              EXTAL





                             144 MHz
                              IRC                                                    48/144 MHz            fro_hf
                         FRO_144M     1/3
                                                                                    48 MHz
                                                 CG                                   clk_48m


                                                                                    12 MHz
                                       CG                                                fro_12m
                         FRO_12M
                                                                                     1 MHz
                                                            1/12                                                clk_1m



                                                   0000
                                                    clk_in      0001
                                        fro_12m      0010
                                                    fro_hf      0011                                                                       Maximum CPU clock
                 Xtal32k[2]                             0100                                                    main_clk
                                                    pll0_clk      0101
                                                    pll1_clk      0110
               usb_pll_clk                             0111



                                      SCG SCS



                                                    clk_in      00
                                        clk_48m      01                        Maximum CPU clock                                                               PLL0                                          pll0_clk
                                     None       10
                                            0      11

                                           PLL0 clock select


                                                    clk_in      00
                                        clk_48m      01                       2× maximum CPU clock                                                               PLL1                                          pll1_clk
                                     None       10
                                            0      11

                                           PLL1 clock select


 Figure 127. SCG main clock





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1203 / 3791

<!-- page 1204 -->

NXP Semiconductors
                                                                                                                           Clocking





                                                                                                            xtal32k[0]
                                      VBAT wrapper                                          To VBAT domain
                                                                                                            xtal32k[1]
                                                                          32.768 kHz                   To VSYS domain
                                 OSC_32K                                                xtal32k[2]
                                                                                                   To WAKE domain
                                                                                                            xtal32k[3]
                                                                                                   To MAIN domain

                                                                                                      clk_16k[0]
                                                                                                   To VBAT domain
                                                                                                      clk_16k[1]
                                                                          16.384 kHz                   To VSYS domain
                                 FRO_16K                                             clk_16k[2]
                                                                                                   To WAKE domain
                                                                                                      clk_16k[3]
                                                                                                   To MAIN domain


                                   RTC Subsystem

                          xtal32k[0]             1                                  lp_osc
                         clk_16k[0]             0

                                   RTC.CTRL[CLK_SEL]

                       clk_1m       IRC CLK


                          lp_osc         WUU                 lp_osc           TDET



                                                     25 MHz
                       fro_12m      00
                         clk_16k[1]      01       LPTMRn
                           xtal32k[1]      10
                               clk_in      11



                         LPTMRn_PSR[PCS]
                                           In LPTMR


               12, 16, 19.2, 20, 24, 30 and 32 MHz                                           USB PHY
                   24, 32, 38.4, 40 and 48 MHz                                           Up to 160 MHz
                                                                                                 usb_pll_clk
                                           clk_in             usb_pll           pfd



 Figure 128. Clock generation

33.3 Module clocking
These sections show the clock generation diagrams for the modules of the chip.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1204 / 3791

<!-- page 1205 -->

NXP Semiconductors
                                                                                                                           Clocking

33.3.1 CPU clock divider


                         CPU clock   system_clk to CPU, AHB bus, sync APB, and so on                  main_clk                                                         Max CPU clock
                                            divider

                                                                       slow_clk to flash memory APB bus, VSYS and
                           AHBCLKDIV           1/4     VBAT domain peripherals' APB/IPS bus, and so on
                                                                                                                           1/4 CPU clock
                                                                divider


                                      SLOWCLKDIV

 Figure 129. Main CPU clocking diagram

33.3.2 CLKOUT clock divider



                                        main_clk      000
                                                 pll0_clk      001                            100 MHz
                                                  clk_in      010
                                                  fro_hf      011      CLKOUT      CLKOUT
                                      fro_12m      100        clock divider
                                              pll1_clk0      101
                                           lp_osc      110
                                          usb_pll_clk      111     CLKOUTDIV[7:0]



                                     CLKOUT select
                                           CLKOUTSEL[2:0]

 Figure 130. CLKOUT diagram

33.3.3 PLL1 clock divider


                                                                    Max CPU clock
                                                       PLL1               pll1_clk0                                                     pll1_clk
                                                                           divider 0


                                                       PLL1CLK0DIV[7:0]
                                                                    Max CPU clock
                                                       PLL1               pll1_clk1                                                     pll1_clk
                                                                           divider 1


                                                       PLL1CLK1DIV[7:0]

 Figure 131. PLL1 divider diagram

33.3.4 PLL clock divider


                                                                     Max CPU clock
                                                      pll0_clk      000                                                            PLL         pll_clk_div
                                                  pll1_clk0      001                                                                                   divider
                                       None      111


                                                              PLLCLKDIV[7:0] (for LP_FLEXCOMM modules)

                                               PLL select
                                               PLLCLKDIVSEL[7:0]

 Figure 132. PLL clock divider diagram



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1205 / 3791

<!-- page 1206 -->

NXP Semiconductors
                                                                                                                           Clocking

33.3.5 FRO_HF clock divider


                                                                    Max CPU clock
                                              FRO_HF        fro_hf_div                                                    fro_hf
                                                                          divider


                                                      FROHFDIV[7:0]

 Figure 133. FRO_HF clock diagram

33.3.6  Trace clock divider clocking


                                                          25 MHz
                                                  Trace clock
                                     main_clk                              000   To trace clock                                                              divider
                                                             clk_1m      001       for SWO
                                                                    lp_osc      010
                                                        None      111                                      TRACECLKDIV


                                                                       Trace clock select
                                                              TRACECLKSEL[2:0]


 Figure 134. Trace clock divider diagram

33.3.7  Systick clocking


                                                          25 MHz
                                                       Systick clock
                                     main_clk                              000   To System Tick                                                              divider
                                                             clk_1m      001       Timer
                                                                    lp_osc      010
                                                        None      111                                       SYSTICKCLKDIVn


                                                                               Systick clock select
                                                              SYSTICKCLKSELn[2:0]


 Figure 135. Systick clock select diagram

33.3.8  FlexSPI clocking



                                      0      0000                  Maximum CPU speed
                                            pll0_clk      0001
                                      0      0010                      To FlexSPI
                                             fro_hf      0011       FlexSPI        function clock
                                      0      0100          divider
                                            pll1_clk      0101                                                                             FlexSPI is expected to
                                     usb_pll_clk      0110                                                                                 support up to 1/2 Max CPU
                                None      1111    FLEXSPICLK                                                                   Speed DDR mode.
                                                                     DIV[2:0]


                                      FLEXSPI clock select
                                        FLEXSPICLKSEL[3:0]


 Figure 136. FlexSPI clocking diagram



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1206 / 3791

<!-- page 1207 -->

NXP Semiconductors
                                                                                                                           Clocking

33.3.9 ADC clocking



                                                0      000
                                                         pll0_clk      001                          60 MHz
                                                         fro_hf      010
                                             fro_12m      011    ADC clock      To ADC
                                                         clk_in      100         divider
                                                     pll1_clk0      101
                                                 usb_pll_clk      110
                                         None      111   ADCnCLKDIV[2:0]

                                     ADCn clock select ADCnCLKSEL[2:0]

                                                 1× instance per ADC

 Figure 137. ADC clocking diagram

33.3.10 DAC clocking


                                                   1× instance per DAC



                                           0      000
                                                   pll0_clk      001                          50 MHz
                                                    clk_in      010
                                                    fro_hf      011      DAC        To DAC
                                        fro_12m      100         divider
                                                pll1_clk0      101
                                           0      110
                                     None      111   DACnCLKDIV[2:0]



                                    DAC clock select
                                            DACnCLKSEL[2:0]

 Figure 138. DAC clocking diagram

33.3.11 CTIMER clocking


                                   1× instance per CTIMER


                                        clk_1m      0000
                                                  pll0_clk      0001
                                                                      Maximum CPU clock                                               pll1_clk0      0010
                                                   fro_hf      0011                                                      CTIMER      To CTIMER
                                       fro_12m      0100                                                                          clock divider
                                        mclk_in_0      0101
                                            lp_osc      0110
                                     None      0111
                                                         CTIMERnCLKDIV[7:0]                                        mclk_in_1      1000
                           SAI0 TX_BCLK clock      1001
                           SAI0 RX_BCLK clock      1010
                           SAI1 TX_BCLK clock      1011
                           SAI1 RX_BCLK clock      1100



                                  CTIMER CTIMERCLKSELn[3:0]

 Figure 139. CTIMER clocking diagram





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1207 / 3791

<!-- page 1208 -->

NXP Semiconductors
                                                                                                                           Clocking

33.3.12 SCTIMER clocking



                                             0      0000
                                                     pll0_clk      0001                  Maximum CPU clock
                                                      clk_in      0010                  To SCTIMER/PWM
                                                      fro_hf      0011       SCTimer/        input clock 7
                                                  pll1_clk0      0100    PWM clock
                                          mclk_in_0      0101          divider
                                              usb_pll_clk      0110
                                       None      0111
                                          mclk_in_1      1111    SCTCLKDIV[7:0]



                                        SCT clock select
                                               SCTCLKSEL[3:0]

 Figure 140. SCTIMER clock diagram

33.3.13 UTICK clocking


                                            clk_1m                                 utick_clk


 Figure 141. UTICK clocking diagram

33.3.14 OSTIMER clocking



                                                 clk_16k[2]      00
                                                    xtal32k[2]      01               To OSTIMER
                                            clk_1m      10
                                        None      11



                                       OSTIMER clock select
                                            OSTIMERCLKSEL[1:0]


 Figure 142. OSTIMER clocking diagram

33.3.15 QDC clocking


                                              system_clk                    QDC


 Figure 143. QDC clocking diagram

33.3.16 PWM clocking


                                              system_clk                  PWM


 Figure 144. PWM clocking diagram





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1208 / 3791

<!-- page 1209 -->

NXP Semiconductors
                                                                                                                           Clocking

33.3.17 NPU clocking


                                              system_clk                     NPU


 Figure 145. NPU clocking diagram

33.3.18 EWM clocking



                                                         xtal32k[2]      1       To EWM LPO CLK
                                                      clk_16k[2]      0



                                    EWM clock select
                                               EWM0CLKSEL[0]

 Figure 146. EWM clock diagram

33.3.19 WWDT clocking


                                                                                   1 MHz
                                           WDT         wdt0_clk                                            clk_1m
                                                                             divider


                                                     WDT0CLKDIV[5:0]

 Figure 147. WWDT0 clock diagram


                                                                                    25 MHz
                                                  clk_16k[2]      00
                                                   fro_hf_div      01       WDT       wdt1_clk
                                             clk_1m      10            divider
                                             clk_1m      11

                                                             WDT1CLKDIV[5:0]

                               WDT1 clock select WDT1CLKSEL[1:0]

 Figure 148. WWDT1 clock diagram

33.3.20 USBFS clocking



                                                0      000
                                                         pll0_clk      001                          48 MHz
                                                0      010
                                            clk_48m      011     FS USB clock  To FS USB
                                                         clk_in      100           divider
                                                     pll1_clk0      101
                                                 usb_pll_clk      110
                                         None      111    USB0CLKDIV[7:0]



                                         USB clock select
                                                 USB0CLKSEL[2:0]

 Figure 149. USBFS clocking diagram




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1209 / 3791

<!-- page 1210 -->

NXP Semiconductors
                                                                                                                           Clocking

33.3.21 EMVSIM clocking


                                                 1× instance per EMVSIM


                                         0      000
                                                pll0_clk      001                             50 MHz
                                                 clk_in      010
                                                 fro_hf      011                       To EMVSIM                                                   EMVSIMxCLKDIV
                                     fro_12m      100
                                             pll1_clk0      101
                                         0      110
                                   None      111            DIV[2:0]



                                     EMVSIMxCLKSEL
                                                      SEL[2:0]

                                                   xtal32k[3]                  EMV_SIM


 Figure 150. EMVSIM clocking diagram

33.3.22 CMP clocking



                               0      000                                                                                 CMP0/1
                                   pll0_clk      001
                                    fro_hf      010                         25 MHz
                           fro_12m      011                                         CMPxRRCLKDIV                          11
                                    clk_in      100                               xtal32k[1]              10
                                 pll1_clk0      101            DIV[3:0]         clk_16k[1]              01      rr_clk
                             usb_pll_clk      110                                                                    Reserved              00
                          None      111

                                                                     CMPx_RRCR0[RR_CLK_SEL]
                            CMPxRRCLKSEL
                                           SEL[2:0]


                               0      000
                                   pll0_clk      001
                                    fro_hf      010                         25 MHz
                           fro_12m      011                                          CMPxFCLKDIV                          11
                                    clk_in      100                               xtal32k[1]              10
                                 pll1_clk0      101            DIV[3:0]         clk_16k[1]              01     func_clk
                             usb_pll_clk      110                                                                    Reserved              00
                          None      111

                                                                     CMPn_CCR1[FUNC_CLK_SEL]
                            CMPxFCLKSEL
                                          SEL[2:0]





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1210 / 3791

<!-- page 1211 -->

NXP Semiconductors
                                                                                                                           Clocking



                               0      000                                                                      CMP2
                                   pll0_clk      001
                                    fro_hf      010                         25 MHz
                           fro_12m      011                                         CMPxRRCLKDIV                          11
                                    clk_in      100                               xtal32k[2]              10
                                 pll1_clk0      101            DIV[3:0]         clk_16k[2]              01      rr_clk
                             usb_pll_clk      110                                                                    Reserved              00
                          None      111

                                                                     CMPx_RRCR0[RR_CLK_SEL]
                            CMPxRRCLKSEL
                                           SEL[2:0]


                               0      000
                                   pll0_clk      001
                                    fro_hf      010                         25 MHz
                           fro_12m      011                                          CMPxFCLKDIV                          11
                                    clk_in      100                               xtal32k[2]              10
                                 pll1_clk0      101            DIV[3:0]         clk_16k[2]              01     func_clk
                             usb_pll_clk      110                                                                    Reserved              00
                          None      111

                                                                     CMPn_CCR1[FUNC_CLK_SEL]
                            CMPxFCLKSEL
                                          SEL[2:0]

 Figure 151. CMP clocking diagram

33.3.23 LP_FLEXCOMM clocking


                                              1× instance per LP_FLEXCOMM


                                                                     Maximum CPU clock
                                            0      000
                                                 pll_clk_div      001
                                         fro_12m      010
                                               fro_hf_div      011     LP_FLEXCOMM To LP_FLEXCOMM[n]
                                          clk_1m      100         clock divider
                                             usb_pll_clk      101
                                              lp_osc      110
                                      None      111  FLEXCOMMnCLKDIV[7:0]



                                   LP_FLEXCOMM clock select
                                               FCCLKSELn[2:0]

 Figure 152. LP_FLEXCOMM clocking diagram

33.3.24 FlexCAN clocking


                                                 1× instance per FlexCAN



                                          0      000
                                                  pll0_clk      001                Maximum CPU clock
                                                  clk_in      010
                                                  fro_hf      011      FlexCAN    To FlexCAN
                                          0      100         divider
                                              pll1_clk0      101
                                          usb_pll_clk      110
                                    None      111  FLEXCANnCLKDIV[7:0]



                                          FlexCAN clock select
                                        FLEXCANnCLKSEL[2:0]

 Figure 153. FlexCAN clocking diagram



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1211 / 3791

<!-- page 1212 -->

NXP Semiconductors
                                                                                                                           Clocking

33.3.25  FlexIO clocking



                                           0      000
                                                   pll0_clk      001             Maximum CPU speed
                                                    clk_in      010
                                                    fro_hf      011       FlexIO       To FlexIO
                                        fro_12m      100        divider
                                                pll1_clk0      101
                                            usb_pll_clk      110
                                     None      111  FLEXIOCLKDIV[7:0]



                                                   FlexIO clock select
                                             FLEXIOCLKSEL[2:0]

 Figure 154. FlexIO clocking diagram

33.3.26 uSDHC clocking


                                          0      000                     100 MHz
                                                 pll0_clk      001                Should support 50 M DDR
                                                  clk_in      010
                                                  fro_hf      011     uSDHC   ToTo uSDHCuSDHC
                                       fro_12m      100        divider
                                              pll1_clk1      101
                                          usb_pll_clk      110
                                    None      111  uSDHCCLKDIV[7:0]


                                         uSDHC clock select
                                                uSDHCCLKSEL[2:0]
                                           lp_osc                         uSDHC 32 kHz clock

 Figure 155. uSDHC clocking diagram

33.3.27 SINC Filter clocking



                                             0      000
                                                      pll0_clk      001          Maximum CPU speed
                                                      clk_in      010
                                                      fro_hf      011  To SINC Filter
                                          fro_12m      100
                                                  pll1_clk0      101
                                              usb_pll_clk      110
                                       None      111



                                          SINC Filter clock select
                                               SINCFILTCLKSEL[2:0]

 Figure 156. SINC Filter clocking diagram





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1212 / 3791

<!-- page 1213 -->

NXP Semiconductors
                                                                                                                           Clocking

33.3.28 I3C clocking


                                                    1× instance per I3C


                                    0      000                                         25 MHz
                                         pll0_clk      001
                                          clk_in      010                                   To I3C0/1
                                          fro_hf      011          I3C FCLK               FCLK
                                    0      100               divider
                                      pll1_clk0      101
                                  usb_pll_clk      110
                              None      111       I3CnFCLKDIV[7:0]
                                                                                       25 MHz
                                                                                    To I3C0/1
                                    I3CnFCLKSEL[2:0]      0           I3C      CLK_SLOW_TC
                                                        FCLK_STC
                                            clk_1m        1            divider


                                                             I3CnFCLKSTCDIV[7:0]

                                               I3CnFCLKSTCSEL[2:0]

                                                                                1 MHz
                                                                          To I3C0/1
                                                      I3C slow clock      CLK_SLOW
                                      clk_1m
                                                                       divider


                                                   I3CnFCLKSDIV[7:0]


 Figure 157. I3C clocking diagram

33.3.29 SAI clocking


                                             1× instance per SAI


                                        0      000
                                               pll0_clk      001                                 50 MHz
                                                clk_in      010
                                                fro_hf      011      SAI clock    To SAI functional clock
                                            pll1_clk0      100          divider
                                        0      101
                                        usb_pll_clk      110
                                  None      111   SAInCLKDIV[2:0]



                                            SAI clock select
                                            SAInCLKSEL[2:0]

 Figure 158. SAI clock diagram





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1213 / 3791

<!-- page 1214 -->

NXP Semiconductors
                                                                                                                           Clocking

33.3.30 ENET clocking




                          ENET RMII divider                                     ENET 1588 divider
                                     DIV             clk_rmii                                         DIV             clk_ptp_ref_i





                 ENETRMIICLKSEL                                    ENETPTPREFCLKSEL





                                                              If P1_4 pinout is configured to
                            ENET0_TX_CLK and
                            ENETRMIICLKSEL is configured                        SYSCON.ENET_PHY_INTF_SEL.PHY_SEL
                                          to a valid clock

           ENET0_TX_CLK
                                                                                                0        25 MHz              (50 MHz/25 MHz)                                                                                                                                                       clk_tx_i
                                                                                                1      2.5/25 MHz
   P1_4       ENET0_TX_CLK



                                                                              DIV
                                                                                                  (2/20)



                                                        MAC speed
                                                                                   (10 MHz/100 MHz)          1      2.5/25 MHz
                                                                                                                                                    clk_rx_i
                                                               ENET0_RX_CLK          0        25 MHz

                                                        P1_11       ENET0_RX_CLK


                                                                        SYSCON.ENET_PHY_INTF_SEL.PHY_SEL


                                                                                GATE                 clk_rmii_i
                                                                                                         50 MHz





        0      000                                                      0      000
    pll0_clk      001                      50 MHz                           pll0_clk      001                     125 MHz
      clk_in      010                                                                  clk_in      010
        0      011      ENET RMII   To Ethernet                          0      011     ENET 1588   To Ethernet                                                                clk_rmii                                                                          clk_ptp_ref_i
        0      100           divider                        ENET0_TX_CLK      100           divider
   pll1_clk0      101                                                              pll1_clk1      101
        0      110                                                      0      110
     None      111  ENETRMIICLKDIV[7:0]                            None      111   ENETPTPREFCLKDIV[7:0]



       ENET RMII clock select                                    ENET 1588 PTP clock select
         ENETRMIICLKSEL[2:0]                                         ENETPTPREFCLKSEL[2:0]

 Figure 159. ENET clocking diagram





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1214 / 3791

<!-- page 1215 -->

NXP Semiconductors
                                                                                                                           Clocking

33.3.31 MICFIL clock divider clocking



                                      fro_12m      0000                                                                                50 MHz
                                                 pll0_clk      0001
                                                  clk_in      0010                       To MICFIL
                                                  fro_hf      0011                                                            MICFIL clock     subsystem
                                              pll1_clk0      0100                                                                                  divider
                                       mclk_in_0      0101
                                          usb_pll_clk      0110
                                    None      0111
                                                           MICFILFCLKDIV[2:0]                                       mclk_in_1      1000



                                            MICFIL clock select
                                            MICFILFCLKSEL[3:0]

 Figure 160. MICFIL clock divider diagram

33.3.32 TSI clocking



                                          0      000
                                          0      001                          25 MHz
                                                  clk_in      010
                                          0      011        TSI          To TSI
                                       fro_12m      100        divider
                                          0      101
                                          0      110
                                    None      111   TSICLKDIV[7:0]



                                                TSI clock select
                                               TSICLKSEL[2:0]

                                                  lp_osc                    TSI clk_32k

 Figure 161. TSI clocking diagram

33.3.33 GDET clocking

                                           clk_48m                               gdet_clk


 Figure 162. GDET clocking diagram

33.4 Set up PLL0
PLL0 creates a stable output clock at a higher frequency than the input clock. If you need a main clock with a frequency higher
than the 12-MHz FRO clock and the 144-MHz FRO clock (fro_hf) is not appropriate, use the PLL to boost the input frequency.

33.5 Set up PLL1
PLL1 creates a stable output clock at a higher frequency than the input clock. If you need a main clock with a frequency higher
than the 12-MHz FRO clock and the 144-MHz FRO clock (fro_hf) is not appropriate, use the PLL to boost the input frequency.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                   1215 / 3791
