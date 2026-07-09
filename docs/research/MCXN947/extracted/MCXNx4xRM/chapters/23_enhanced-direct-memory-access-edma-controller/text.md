# 23 Enhanced Direct Memory Access (eDMA) Controller

Source: `docs/mcxn947/MCXNx4xRM.pdf`
Pages: 730-829

<!-- page 730 -->

NXP Semiconductors

Chapter 23
Enhanced Direct Memory Access (eDMA) Controller

23.1  Chip-specific eDMA information

Table 291. Reference links to related information

         Topic             Related module                                Reference

      Full description         eDMA                            eDMA

  System memory map                                           Memory map

        Clocking                                                        Clock distribution

   Signal multiplexing          Port control                                  Signal multiplexing
    Input multiplexing       INPUTMUX              See DMAn_REQ_ENABLEn registers in INPUTMUX

23.1.1 Module instances
This device has two 16 channel eDMA controllers.

23.1.2  Direct Memory Access Multiplexer (DMAMUX)
The peripheral request line for each channel of the Direct Memory Access Controller (DMA) is driven from a Direct Memory Access
Multiplexer (DMAMUX). This is a flexible configuration that allows the user to select the appropriate peripheral to connect to
each channel of the DMA Controller. The DMAMUX for this device allows up to 128 DMA request signals (6 unused signals are
reserved) to be mapped to each channel. Because of the mux, there is not a correlation between any of the DMA request sources
and a specific DMA channel.
The DMAMUX is an integrated component of the DMA Controller. This allows each DMA channel and DMA Multiplexer to
configured as secure or non-secure on initial configuration of the device and not be changed without the correct permissions.

23.1.2.1 DMAMUX0 Request Assignments

Table 292. DMAMUX0 request assignments

   DMAMUX                 Alias           Source Description
    Number

                                                Modules

       0          —             Disabled

       1                 FlexSPI0         Receive event

       2                 FlexSPI0          Transmit event

       3                PINT0          INT0

       4                PINT0          INT1

       5                PINT0          INT2

       6                PINT0          INT3

       7             CTIMER0       DMAREQ_M0

       8             CTIMER0       DMAREQ_M1

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    730 / 3791

<!-- page 731 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Table 292. DMAMUX0 request assignments (continued)

   DMAMUX                 Alias           Source Description
    Number

       9             CTIMER1       DMAREQ_M0

       10             CTIMER1       DMAREQ_M1

       11             CTIMER2       DMAREQ_M0

       12             CTIMER2       DMAREQ_M1

       13             CTIMER3       DMAREQ_M0

       14             CTIMER3       DMAREQ_M1

       15             CTIMER4       DMAREQ_M0

       16             CTIMER4       DMAREQ_M1

       17            WUU0        Wake up event

       18               MICFIL0          FIFO_request

       19              SCT0        DMA0

       20              SCT0        DMA1

       21             ADC0          FIFO A request

       22             ADC0          FIFO B request

       23             ADC1          FIFO A request

       24             ADC1          FIFO B request

       25             DAC0           FIFO_request

       26             DAC1           FIFO_request

       27             DAC2           FIFO_request

       28             CMP0          DMA_request

       29             CMP1          DMA_request

       30             CMP2          DMA_request

       31             EVTG0        OUT0A

       32             EVTG0        OUT0B

       33             EVTG0        OUT1A

       34             EVTG0        OUT1B

       35             EVTG0        OUT2A

       36             EVTG0        OUT2B

       37             EVTG0        OUT3A

       38             EVTG0        OUT3B

       39            PWM0          Req_capt0

       40            PWM0          Req_capt1

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    731 / 3791

<!-- page 732 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Table 292. DMAMUX0 request assignments (continued)

   DMAMUX                 Alias           Source Description
    Number

       41            PWM0          Req_capt2

       42            PWM0          Req_capt3

       43            PWM0          Req_val0

       44            PWM0          Req_val1

       45            PWM0          Req_val2

       46            PWM0          Req_val3

       47            PWM1          Req_capt0

       48            PWM1          Req_capt1

       49            PWM1          Req_capt2

       50            PWM1          Req_capt3

       51            PWM1          Req_val0

       52            PWM1          Req_val1

       53            PWM1          Req_val2

       54            PWM1          Req_val3

       55               Reserved     —

       56               Reserved     —

       57             LPTMR0          Counter match event

       58             LPTMR1          Counter match event

       59             CAN0       DMA request

       60             CAN1       DMA request

       61                 FlexIO0            Shifter0 Status DMA request OR Timer0 Status DMA request

       62                 FlexIO0            Shifter1 Status DMA request OR Timer1 Status DMA request

       63                 FlexIO0            Shifter2 Status DMA request OR Timer2 Status DMA request

       64                 FlexIO0            Shifter3 Status DMA request OR Timer3 Status DMA request

       65                 FlexIO0            Shifter4 Status DMA request OR Timer4 Status DMA request

       66                 FlexIO0            Shifter5 Status DMA request OR Timer5 Status DMA request

       67                 FlexIO0            Shifter6 Status DMA request OR Timer6 Status DMA request

       68                 FlexIO0            Shifter7 Status DMA request OR Timer7 Status DMA request

       69         LP_FLEXCOMM0      Receive request

       70         LP_FLEXCOMM0      Transmit request

       71         LP_FLEXCOMM1      Receive request

       72         LP_FLEXCOMM1      Transmit request

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    732 / 3791

<!-- page 733 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Table 292. DMAMUX0 request assignments (continued)

   DMAMUX                 Alias           Source Description
    Number

       73         LP_FLEXCOMM2      Receive request

       74         LP_FLEXCOMM2      Transmit request

       75         LP_FLEXCOMM3      Receive request

       76         LP_FLEXCOMM3      Transmit request

       77         LP_FLEXCOMM4      Receive request

       78         LP_FLEXCOMM4      Transmit request

       79         LP_FLEXCOMM5      Receive request

       80         LP_FLEXCOMM5      Transmit request

       81         LP_FLEXCOMM6      Receive request

       82         LP_FLEXCOMM6      Transmit request

       83         LP_FLEXCOMM7      Receive request

       84         LP_FLEXCOMM7      Transmit request

       85         LP_FLEXCOMM8      Receive request

       86         LP_FLEXCOMM8      Transmit request

       87         LP_FLEXCOMM9      Receive request

       88         LP_FLEXCOMM9      Transmit request

       89               Reserved     —

       90               Reserved     —

       91            EMVSIM0         Receive request

       92            EMVSIM0         Transmit request

       93            EMVSIM1         Receive request

       94            EMVSIM1         Transmit request

       95                 I3C0           Receive request

       96                 I3C0            Transmit request

       97                 I3C1           Receive request

       98                 I3C1            Transmit request

       99                 SAI0           Receive request

      100                SAI0            Transmit request

      101                SAI1           Receive request

      102                SAI1            Transmit request

      103              SINC0            ipd_req_sinc[0] or ipd_req_alt [0]

      104              SINC0            ipd_req_sinc[1] or ipd_req_alt [1]

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    733 / 3791

<!-- page 734 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Table 292. DMAMUX0 request assignments (continued)

   DMAMUX                 Alias           Source Description
    Number

      105              SINC0            ipd_req_sinc[2] or ipd_req_alt [2]

      106              SINC0            ipd_req_sinc[3] or ipd_req_alt [3]

      107              SINC0            ipd_req_sinc[4] or ipd_req_alt [4]

      108              GPIO0           Pin event request 0

      109              GPIO0           Pin event request 1

      110              GPIO1           Pin event request 0

      111              GPIO1           Pin event request 1

      112              GPIO2           Pin event request 0

      113              GPIO2           Pin event request 1

      114              GPIO3           Pin event request 0

      115              GPIO3           Pin event request 1

      116              GPIO4           Pin event request 0

      117              GPIO4           Pin event request 1

      118              GPIO5           Pin event request 0

      119              GPIO5           Pin event request 1

      120                 TSI0          End of Scan

      121                 TSI0           Out of Range

23.1.2.2 DMAMUX1 Request Assignments

Table 293. DMAMUX1 request assignments

   DMAMUX                 Alias           Source Description
    Number

                                                Modules

       0          —             Disabled

       1                 FlexSPI0         Receive event

       2                 FlexSPI0          Transmit event

       3                PINT0          INT0

       4                PINT0          INT1

       5                PINT0          INT2

       6                PINT0          INT3

       7             CTIMER0       DMAREQ_M0

       8             CTIMER0       DMAREQ_M1

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    734 / 3791

<!-- page 735 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Table 293. DMAMUX1 request assignments (continued)

   DMAMUX                 Alias           Source Description
    Number

       9             CTIMER1       DMAREQ_M0

       10             CTIMER1       DMAREQ_M1

       11             CTIMER2       DMAREQ_M0

       12             CTIMER2       DMAREQ_M1

       13             CTIMER3       DMAREQ_M0

       14             CTIMER3       DMAREQ_M1

       15             CTIMER4       DMAREQ_M0

       16             CTIMER4       DMAREQ_M1

       17            WUU0        Wake up event

       18               MICFIL0          FIFO_request

       19              SCT0        DMA0

       20              SCT0        DMA1

       21             ADC0          FIFO A request

       22             ADC0          FIFO B request

       23             ADC1          FIFO A request

       24             ADC1          FIFO B request

       25             DAC0           FIFO_request

       26             DAC1           FIFO_request

       27             DAC2           FIFO_request

       28             CMP0          DMA_request

       29             CMP1          DMA_request

       30             CMP2          DMA_request

       31             EVTG0        OUT0A

       32             EVTG0        OUT0B

       33             EVTG0        OUT1A

       34             EVTG0        OUT1B

       35             EVTG0        OUT2A

       36             EVTG0        OUT2B

       37             EVTG0        OUT3A

       38             EVTG0        OUT3B

       39            PWM0          Req_capt0

       40            PWM0          Req_capt1

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    735 / 3791

<!-- page 736 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Table 293. DMAMUX1 request assignments (continued)

   DMAMUX                 Alias           Source Description
    Number

       41            PWM0          Req_capt2

       42            PWM0          Req_capt3

       43            PWM0          Req_val0

       44            PWM0          Req_val1

       45            PWM0          Req_val2

       46            PWM0          Req_val3

       47            PWM1          Req_capt0

       48            PWM1          Req_capt1

       49            PWM1          Req_capt2

       50            PWM1          Req_capt3

       51            PWM1          Req_val0

       52            PWM1          Req_val1

       53            PWM1          Req_val2

       54            PWM1          Req_val3

       55               Reserved     —

       56               Reserved     —

       57             LPTMR0          Counter match event

       58             LPTMR1          Counter match event

       59             CAN0       DMA request

       60             CAN1       DMA request

       61                 FlexIO0            Shifter0 Status DMA request OR Timer0 Status DMA request

       62                 FlexIO0            Shifter1 Status DMA request OR Timer1 Status DMA request

       63                 FlexIO0            Shifter2 Status DMA request OR Timer2 Status DMA request

       64                 FlexIO0            Shifter3 Status DMA request OR Timer3 Status DMA request

       65                 FlexIO0            Shifter4 Status DMA request OR Timer4 Status DMA request

       66                 FlexIO0            Shifter5 Status DMA request OR Timer5 Status DMA request

       67                 FlexIO0            Shifter6 Status DMA request OR Timer6 Status DMA request

       68                 FlexIO0            Shifter7 Status DMA request OR Timer7 Status DMA request

       69         LP_FLEXCOMM0      Receive request

       70         LP_FLEXCOMM0      Transmit request

       71         LP_FLEXCOMM1      Receive request

       72         LP_FLEXCOMM1      Transmit request

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    736 / 3791

<!-- page 737 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Table 293. DMAMUX1 request assignments (continued)

   DMAMUX                 Alias           Source Description
    Number

       73         LP_FLEXCOMM2      Receive request

       74         LP_FLEXCOMM2      Transmit request

       75         LP_FLEXCOMM3      Receive request

       76         LP_FLEXCOMM3      Transmit request

       77         LP_FLEXCOMM4      Receive request

       78         LP_FLEXCOMM4      Transmit request

       79         LP_FLEXCOMM5      Receive request

       80         LP_FLEXCOMM5      Transmit request

       81         LP_FLEXCOMM6      Receive request

       82         LP_FLEXCOMM6      Transmit request

       83         LP_FLEXCOMM7      Receive request

       84         LP_FLEXCOMM7      Transmit request

       85         LP_FLEXCOMM8      Receive request

       86         LP_FLEXCOMM8      Transmit request

       87         LP_FLEXCOMM9      Receive request

       88         LP_FLEXCOMM9      Transmit request

       89               Reserved     —

       90               Reserved     —

       91            EMVSIM0         Receive request

       92            EMVSIM0         Transmit request

       93            EMVSIM1         Receive request

       94            EMVSIM1         Transmit request

       95                 I3C0           Receive request

       96                 I3C0            Transmit request

       97                 I3C1           Receive request

       98                 I3C1            Transmit request

       99                 SAI0           Receive request

      100                SAI0            Transmit request

      101                SAI1           Receive request

      102                SAI1            Transmit request

      103              SINC0            ipd_req_sinc[0] or ipd_req_alt [0]

      104              SINC0            ipd_req_sinc[1] or ipd_req_alt [1]

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    737 / 3791

<!-- page 738 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Table 293. DMAMUX1 request assignments (continued)

   DMAMUX                 Alias           Source Description
    Number

      105              SINC0            ipd_req_sinc[2] or ipd_req_alt [2]

      106              SINC0            ipd_req_sinc[3] or ipd_req_alt [3]

      107              SINC0            ipd_req_sinc[4] or ipd_req_alt [4]

      108              GPIO0           Pin event request 0

      109              GPIO0           Pin event request 1

      110              GPIO1           Pin event request 0

      111              GPIO1           Pin event request 1

      112              GPIO2           Pin event request 0

      113              GPIO2           Pin event request 1

      114              GPIO3           Pin event request 0

      115              GPIO3           Pin event request 1

      116              GPIO4           Pin event request 0

      117              GPIO4           Pin event request 1

      118              GPIO5           Pin event request 0

      119              GPIO5           Pin event request 1

      120                 TSI0          End of Scan

      121                 TSI0           Out of Range

23.1.3 DSPI
DSPI is external to the DMA and is only referred as an example of DMA hardware request usage.

23.1.4  Security considerations
When TrustZone-M is used, the following configurations are recommended for eDMA usage:
   • Use strict checking (AHBSC.MISC_CTRL_REG[DISABLE_STRICT] = 01b). Strict mode enabled is also the
    default configuration.
   • Configure both eDMAs for secure and privileged level in the AHBSC’s MASTER_SEC_LEVEL and
   MASTER_SEC_ANTI_POL_REG registers (note: this is not the default setting).
   • Enable the eDMA master ID replication feature for all DMA channels. The master ID replication feature is enabled globally by
    setting MP_CSR[GMRC], and then is enabled on a per channel basis by setting CHn_SBR[EMI].
   • AHBSC peripheral slots and CHn_SBR[PAL, SEC] for each DMA channel need to be preconfigured for secure/non-secure
   and privileged/non-privileged based on the intended usage of each channel. This allows masters of each type to configure
    allocated channels. Unused DMA channels should be configured for secure and privileged access to prevent misuse.
When the settings above are followed, the master ID is captured when the TCDn_CSR is written according to the configuration of
the master that wrote TCDn_CSR. The final DMA channel access security level can be downgraded from secure to non-secure
depending on the source and destination addresses. See the “Master Security Wrapper (MSW)” section of the device Security
Reference Manual.




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    738 / 3791

<!-- page 739 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

                                           NOTE
         DMA scatter/gather operations must use an address matching the configuration of the DMA channel. Secure
             channels must access a secure memory address to load new TCDs. Non-secure channels must access a
             non-secure memory address to load new TCDs.

23.2 Overview
The enhanced direct memory access (eDMA) controller is capable of performing complex data transfers with minimal intervention
from a host processor. The hardware microarchitecture includes:
   • A DMA engine that performs:
   — Source address and destination address calculations
   — Data-movement operations
   • Local memory containing transfer control descriptors for each of the 16 channels

23.2.1  Block diagram
Figure 104 illustrates the components of the eDMA system, including the eDMA module (engine).


                  eDMA system
                                                                                                                Write Address

                                                                                                                   Write Data



                                                                                                  0
                                                                                                  1
                                                                                                  2
                                                                                                                                                                      Bus
                                                  fabric
                                                                              Transfer Control
                                                                             Descriptor (TCD)                                                                                                                 n-1                                                      Peripheral
                                                                                       64                                                                                                   interconnect                                                                                                                                                                                                                                                                                                                                                                                                                                                          Internal
                  eDMA Engine                                                     Read Data                                                          To/from                                              Program Model/
                                                                 Channel Arbitration
                        Read Data

                                                           Address Path

                                                                                             Control
                                     Data Path



                               Write Data

                           Address



                                                           eDMA   eDMA
                                                                                        Peripheral  Done
                                                                            Request

 Figure 104. Block diagram

23.2.2  Block parts
The eDMA module is partitioned into two major modules: the eDMA engine and the transfer control descriptor local memory.
The eDMA engine is further partitioned into four submodules:


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    739 / 3791

<!-- page 740 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Table 294. eDMA engine submodules

      Submodule                                                 Function
 Address path             This block:
                                      • Implements a primary channel and secondary (preempt) channel
                                      • Manages all master bus-address calculations
                                 All the channels provide the same functionality. This structure allows data transfers associated
                            with one channel to be preempted after the completion of a read/write sequence if a higher priority
                         channel activation is asserted while the primary channel is active.
                             After a channel is activated, it runs until the minor loop is completed, unless preempted by a
                           higher priority channel. This provides a mechanism (enabled by CHn_PRI[ECP]) where a large data
                             transfer can be preempted to minimize the time another channel is blocked from execution.
                   When any channel is selected to execute, the contents of its TCD are read from local memory and
                         loaded into the address path channel x registers for a normal start and into channel y registers for
                      a preemption start. After the minor loop completes execution, the address path hardware writes the
                    new values for the TCDn_{SADDR, DADDR, CITER} back to local memory. If the major iteration
                         count is exhausted, additional processing is performed, including the final address pointer updates,
                           reloading the TCDn_CITER field, and a possible fetch of a new TCDnfrom memory as part of a
                            scatter/gather operation. See Dynamic scatter/gather for more details.

 Data path                This block implements the bus master read/write data path. It includes a data buffer and the
                        necessary multiplex logic to support any required data alignment. The internal read data bus is the
                          primary input, and the internal write data bus is the primary output.
                      The address and data path modules directly support the 2-stage pipelined internal bus. The address
                         path module represents the first stage of the bus pipeline (address phase), and the data path module
                        implements the second stage of the pipeline (data phase).

 Program model/channel   This block implements the first section of the eDMA programming model as well as the channel
 arbitration                   arbitration logic. The programming model registers are connected to the internal peripheral bus.
                      The eDMA peripheral request inputs and interrupt request outputs are also connected to this
                          block (via control logic).
 Control                   This block provides all the control functions for the eDMA engine. For data transfers where
                           the source and destination sizes are equal, the eDMA engine performs a series of source read/
                            destination write operations until the number of bytes specified in the minor loop byte count has been
                     moved from the source to the destination.
                         For descriptors where the sizes are not equal, multiple accesses of the smaller size data are required
                                for each reference of the larger size. As an example, if the source size references 16-bit data and
                           the destination is 32-bit data, the eDMA performs two reads, then one 32-bit write.

The transfer control descriptor local memory is further partitioned into:

Table 295. Transfer control descriptor memory

      Submodule                                                  Description

 Memory controller        This logic implements the required dual-ported controller, and manages accesses from the eDMA
                         engine as well as references from the internal peripheral bus. In simultaneous accesses, the
                  eDMA engine is given priority and the peripheral transaction is stalled.

 Memory array        TCD storage for each channel's transfer profile.




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    740 / 3791

<!-- page 741 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

23.2.3  Features
The eDMA is a highly programmable data-transfer engine optimized to minimize any required intervention from the host processor.
It is intended for use in applications where the data size to be transferred is statically known and is not defined within the transferred
data itself. The eDMA module features:
   •  All data movement via dual-address transfers: read from source, write to destination
   — Programmable source and destination addresses and transfer size
   — Support for complex address calculations
   • 16-channel implementation that performs complex data transfers with minimal intervention from a host processor
   — Internal data buffer, used as temporary storage for all transfers
   — Connections to the crossbar switch for bus mastering the data movement
   • TCD organized to support two-deep, nested transfer operations
   — 32-byte TCD stored in local memory for each channel
   — An inner data transfer loop defined by a minor byte transfer count
   — An outer data transfer loop defined by a major iteration count
   • Channel activation via one of three methods:
   — Explicit software initiation
   — Initiation via a channel-to-channel linking mechanism for continuous transfers
   — Peripheral-paced hardware requests, one per channel
   • Fixed-priority and round-robin channel arbitration
   • Channel completion reported via programmable interrupt requests
   — One interrupt per channel, which can be asserted at completion of major iteration count
   — Programmable error terminations per channel that are logically summed together to form one error interrupt to the
           interrupt controller
   • Programmable support for scatter/gather DMA processing
   • Support for complex data structures
In the discussion of this module, nis used to reference the channel number.

23.3  Functional description
The operation of eDMA is described in the following subsections.

23.3.1 Modes of operation
The eDMA operates in the following modes:

Table 296. Modes of operation

        Mode                                                     Description
 Normal                     In Normal mode, eDMA transfers data between a source and a destination. The source and
                            destination can be a memory block or an I/O block capable of operation with eDMA.
                   A service request initiates a transfer of a specific number of bytes (NBYTES) as specified in the TCD.
                      The minor loop is the sequence of read-write operations that transfers these NBYTES per service

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    741 / 3791

<!-- page 742 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Table 296. Modes of operation (continued)

        Mode                                                     Description

                           request. Each service request executes one iteration of the major loop, which transfers NBYTES
                              of data.

 Debug             eDMA operation is configurable in Debug mode via the control register:
                                      •  If CSR[EDBG] is cleared to 0, eDMA continues to operate.
                                      •  If CSR[EDBG] is set to 1, eDMA stops transferring data. If Debug mode is entered when a
                            channel is active, eDMA continues operation until the channel retires.

23.3.2 eDMA basic data flow
The basic flow of a data transfer can be partitioned into three segments.
As shown in the following diagram, the first segment involves the channel activation:


                  eDMA
                                                                                                                Write address

                                                                                                                   Write data



                                                                                                  0
                                                                                                  1
                                                                                                  2
                                                                                                                                                                      bus

                                                                                     Transfer control                                                  switch                                                                 descriptor (TCD)                                                                                                                 n-1                                                      peripheral
                                                                                       64
                                                                  crossbar                                                                                                                                                                                                                                                                                                                                                                                                                                                          Internal
                  eDMA engine                                                     Read data                                                          To/from                                              Program model/
                                                                     channel arbitration
                        Read data

                                                             Address path

                                                                                               Control
                                     Data path


                               Write data

                           Address




                                                           eDMA   eDMA
                                                                                          peripheral  done
                                                                                     request

 Figure 105. eDMA operation, part 1

This example uses the assertion of the eDMA peripheral request signal to request service for channel n. Channel activation via
software and the TCDn_CSR[START] field follows the same basic flow as peripheral requests. The eDMA request input signal is
registered internally and then routed through the eDMA engine: first through the control module, then into the program model and
channel arbitration.
In the next cycle, the channel arbitration begins using fixed-priority plus the optional round-robin algorithm. After arbitration is
complete, the activated channel number is sent through the address path and converted into the required address to access the


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    742 / 3791

<!-- page 743 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

local memory for TCDn. Next, the TCD memory is accessed and the required descriptor is read from the local memory and then
loaded into the eDMA engine address path's primary or secondary channel execution registers. The TCD memory is 64 bits wide
to minimize the time needed to fetch the activated channel descriptor and load it into the address path registers.
The following diagram illustrates the second part of the basic data flow:


                  eDMA
                                                                                                                Write address

                                                                                                                   Write data



                                                                                                  0
                                                                                                  1
                                                                                                  2
                                                                                                                                                                      bus

                                                                                     Transfer control                                                  switch                                                                 descriptor (TCD)                                                                                                                 n-1                                                      peripheral
                                                                                       64
                                                                  crossbar                                                                                                                                                                                                                                                                                                                                                                                                                                                          Internal
                  eDMA engine                                                     Read data                                                          To/from                                              Program model/
                                                                     channel arbitration
                        Read data

                                                           Address path

                                                                                             Control
                                     Data path


                               Write data

                           Address




                                                           eDMA   eDMA
                                                                                          peripheral  done
                                                                                     request

 Figure 106. eDMA operation, part 2

The modules associated with the data transfer (address path, data path, and control) go through the required sequence of
source reads and destination writes to perform the actual data movement. The source reads are initiated, and the fetched
data is temporarily stored in the data path block until it is gated onto the internal bus during the destination write. This source
read/destination write processing continues until the byte count, NBYTES, has been transferred.
After NBYTES of data has been moved, the final phase of the basic data flow is performed. In this segment, the address path
logic performs the required updates to certain fields in the appropriate TCD (for example, SADDR, DADDR, CITER). If the major
iteration count is exhausted, additional operations are performed. These include the final address adjustments and reloading of
the BITER field into the CITER field. Assertion of an optional interrupt request also occurs at this time, as does a possible fetch
of a new TCD from memory using the scatter/gather address pointer included in the descriptor (if scatter/gather is enabled). The
updates to the TCD memory and the assertion of an interrupt request are shown in the following diagram.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    743 / 3791

<!-- page 744 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller



                  eDMA
                                                                                                                Write address

                                                                                                                   Write data



                                                                                                  0
                                                                                                  1
                                                                                                  2
                                                                                                                                                                      bus

                                                                                     Transfer control                                                  switch                                                                 descriptor (TCD)                                                                                                                 n-1                                                      peripheral
                                                                                       64
                                                                  crossbar                                                                                                                                                                                                                                                                                                                                                                                                                                                          Internal
                  eDMA engine                                                     Read data                                                          To/from                                              Program model/
                                                                     channel arbitration
                        Read data

                                                          Address path

                                                                                            Control
                                     Data path


                               Write data

                           Address




                                                           eDMA   eDMA
                                                                                          peripheral  done
                                                                                     request

 Figure 107. eDMA operation, part 3

23.3.3  Fault reporting and handling
Channel errors are reported in the Error Status register (CHn_ES) and can be caused by any of the following:
   • A configuration error, which is an illegal setting in the transfer control descriptor
   • An active channel canceled via a "cancel transfer with error" hardware or software request
   • An error termination to a bus master read or write cycle
A configuration error is reported when an inconsistent state is represented by one of these factors:
   • Starting source or destination address
   • Source or destination offsets
   • Minor loop byte count
   • Transfer size
Each of these possible causes is detailed below:
   • The addresses and offsets must be aligned on zero-modulo-transfer-sized boundaries.
   • The minor loop byte count must be a multiple of the source and destination transfer sizes.
   •  All source reads and destination writes must be configured to the natural boundary of the programmed transfer size.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    744 / 3791

<!-- page 745 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

                                           NOTE
            To aid in debugging, set the Halt After Error field in the DMA’s Control Status register, CSR[HAE]. Upon any error
                condition, the DMA is halted after the error is recorded. The DMA remains halted and does not process any channel
               service requests. After the error is fixed, the DMA may be enabled again by clearing the Halt field, CSR[HALT].

   •  If a scatter/gather operation is enabled upon channel completion, a configuration error is reported if the scatter/gather
    address (TCDn_DLAST_SGA) is not aligned on a 32-byte boundary.
   •  If minor loop channel linking is enabled upon channel completion, a configuration error is reported when the link is
    attempted if the TCDn_CITER[ELINK] field does not equal the TCDn_BITER[ELINK] field.
If enabled, all configuration error conditions, except the scatter/gather and minor-loop link errors, are reported as the channel
activates and asserts an error interrupt request. A scatter/gather configuration error is reported when the scatter/gather operation
begins at major loop completion if properly enabled. A minor loop channel link configuration error is reported when the link
operation is serviced at minor loop completion.
If a system bus read or write is terminated with an error, the data transfer is stopped and the appropriate bus error flag set. In
this case, the state of the channel's transfer control descriptor is updated by the eDMA engine with the current source address,
destination address, and current iteration count at the point of the fault. When a system bus error occurs, the channel terminates
after the next transfer. Due to pipeline effect, the next transfer is already in progress when the bus error is received by the eDMA.
If a bus error occurs on the last read prior to beginning the write sequence, the write executes using the data captured during the
bus error. If a bus error occurs on the last write prior to switching to the next read sequence, the read sequence executes before
the channel terminates due to the destination bus error.
The occurrence of any error causes the eDMA engine to stop normal processing of the active channel immediately (it goes to
its error processing states and the transaction to the system bus still has pipeline effect), and the appropriate channel field in
the eDMA error register is set to 1. At the same time, the details of the error condition are loaded into the Error Status register
(CHn_ES). The major loop complete indicators, setting the transfer control descriptor DONE flag, and the possible assertion of
an interrupt request are not affected when an error is detected.
After the error status has been updated, the eDMA engine continues operating by servicing the next appropriate channel. A
channel that experiences an error condition is not automatically disabled. If a channel is terminated by an error and then issues
another service request before the error is fixed, that channel executes and terminates with the same error condition.
The error status fields are read-only. These error indicators are sticky and cannot be cleared. They show the last recorded error
until the DMA is reset. CHn_ES[ERR] is used to determine if a new error condition exists. This field is the logical OR of each
channel's error interrupt field (ERR).
After the software has resolved all errors and cleared all of the error interrupt fields, the MP_ES[VLD] is cleared to 0 but the cause
of the last error is still indicated.

23.3.4 Channel preemption
The eDMA uses a priority vector value to determine the highest priority channel requesting service.
The priority vector is a combination of:
   1. the channel's group priority, CHn_GRPRI
   2. the channel's priority level, CHn_PRI[APL]
It can be considered a number composed of these concatenated priority levels: CHn_GRPRI : CHn_PRI[APL]
Priority vector = ((CHn_GRPRI << 8) + (CHn_PRI[APL] << 5 ) + CHn_*)
A channel requesting service with the highest priority vector value will receive the next execution slot.
An execution slot is available:
   1. immediately if the eDMA is idle
   2. when an active channel retires
   3. when valid preemption conditions exist



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    745 / 3791

<!-- page 746 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

                                           NOTE
             Preemption is strictly priority based. Preemption is not bound by a specific group number as defined
             by CHn_GRPRI.

Channel preemption is enabled on a per-channel basis by setting the CHn_PRI[ECP] field. Channel preemption allows the
executing channel’s data transfers to temporarily suspend in favor of starting a higher-priority channel. After the preempting
channel has completed all of its minor loop data transfers, the preempted channel is restored and resumes execution.
After the restored channel completes one read/write sequence, it is again eligible for preemption. If any higher priority channel
is requesting service, the restored channel is suspended, and the higher-priority channel is serviced. Nested preemption, that
is, attempting to preempt a preempting channel, is not supported. After a preempting channel begins execution, it cannot
be preempted.
A channel’s ability to preempt another channel can be disabled by setting CHn_PRI[DPA] to 1. When a channel’s preempt ability
is disabled, that channel cannot suspend a lower-priority channel’s data transfer, regardless of the lower-priority channel’s ECP
setting. This allows for a pool of low-priority, large-data-moving channels to be defined.
You can configure these low-priority channels to not preempt each other, thus preventing a low-priority channel from consuming
the preempt slot normally available to a true high-priority channel. When you enable round-robin channel arbitration mode
(CSR[ERCA] is set to 1), any channel with a priority level equal to 0 (CHn_PRI[APL] = 0) has preemption disabled and cannot
preempt another channel.

23.3.5  Clocking
This module has no clocking considerations.

23.3.6  Interrupts
Software can enable the interrupt for each channel for the following events:
   1. The major loop is half complete (INTHALF)
   2. The major loop is complete (INTMAJOR)
   3. A configuration error occurs (EEI)

23.4  External signals
This module has no external signals.

23.5  Initialization
The following sections discuss initialization of the eDMA and programming considerations.

23.5.1 eDMA initialization
To initialize the eDMA:
   1. Write to the MP_CSR if a configuration other than the default is wanted.
   2. Write the channel priority levels to the CHn_PRI registers and group priority levels to the CHn_GRPRI registers if a
       configuration other than the default is wanted.
   3. Enable error interrupts in the CHn_CSR[EEI] registers if they are wanted.
   4. Write the 32-byte TCD for each channel that may request service.
   5. Enable any hardware service requests via the CHn_CSR[ERQ] registers.
   6. Request channel service via either:
           • Software: setting TCDn_CSR[START]
           • Hardware: slave device asserting its eDMA peripheral request signal


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    746 / 3791

<!-- page 747 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

After any channel requests service, a channel is selected for execution based on the arbitration and priority levels written into the
programmer's model. The eDMA engine reads the entire TCD, including the TCD control and status fields, as shown in Table 297,
for the selected channel into its internal address path module.
As the TCD is read, the first transfer is initiated on the internal bus, unless a configuration error is detected. Transfers from the
source, defined by TCDn_SADDR, to the destination, defined by TCD_DADDR, continue until the number of bytes specified by
TCDn_NBYTES are transferred.
When the transfer is complete, the eDMA engine's local TCDn_SADDR, TCDn_DADDR, and TCDn_CITER are written back to
the main TCD memory and any minor loop channel linking is performed, if enabled. If the major loop is exhausted, then eDMA
executes further post-processing, such as interrupts, major loop channel linking, and scatter/gather operations, if enabled.

Table 297. TCD control and status (TCDn_CSR) fields

 TCDn_CSR field name                                             Description

      START         Control field to start the channel explicitly when using a software-initiated DMA service (automatically
                         cleared by hardware)

      EEOP          Control field to enable end-of-packet processing

      ESDA          Control field to enable storing of the destination address to system memory after the major loop
                       completes

      DREQ          Control field to disable hardware-initiated DMA service requests after major loop completion

      BWC          Control field for throttling the bandwidth control of a channel

      ESG           Control field to enable the scatter-gather feature

      INTHALF        Control field to enable interrupt when major loop is half-complete

     INTMAJOR       Control field to enable interrupt when major loop completes


Table 298. Channel control and status (CHn_CSR) fields

 CHn_CSR field name                                              Description

      ACTIVE         Status field indicating the channel is currently in execution

      DONE          Status field indicating major loop completion (cleared by software when a channel begins execution)

         EEI           Control field to enable error interrupts

      EARQ          Control field to enable external, asynchronous wakeup event in conjunction with the ERQ field

      ERQ           Control field to enable hardware service requests

The following figure shows how each DMA request initiates one minor-loop transfer, or iteration, without CPU intervention. DMA
arbitration can occur after each minor loop, and one level of minor loop DMA preemption is allowed. The number of minor loops
in a major loop is specified by the beginning iteration count (BITER).





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    747 / 3791

<!-- page 748 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller



                                                                                                                 Current major
                                                                                                                  loop iteration
                                      Source or destination memory                                         count (CITER)
                  DMA request



                                                                                         •                    Minor loop                          3
                                                                                         •
                                                                                         •


                  DMA request



                                                                                         •                    Minor loop         Major loop         2
                                                                                         •
                                                                                         •


                  DMA request



                                                                                         •                    Minor loop                          1
                                                                                         •
                                                                                         •



 Figure 108. Example of multiple loop iterations

The following figure lists the memory array terms and how the TCD settings are related.



                xADDR: (starting address)       xSIZE: (size of one
                                                      data transfer)          Minor loop
                                                            (NBYTES in
                                                                                                 •                minor loop,       Offset (xOFF): number of bytes added to
                                                                                                 •                often the same     current address after each transfer
                                                                                                 •              value as xSIZE)     (often the same value as xSIZE)


                                                                               Each DMA source (S) and
                                                                                                       destination (D) has its own:
                                                   •                                            •
                                                   •                                            •                              Address (xADDR)
                                                   •                                            •                                 Size (xSIZE)                                                                           Minor loop                                                   •                                            •                                    Offset (xOFF)
                                                   •                                            •                             Modulo (xMOD)
                                                   •                                            •                                  Last Address Adjustment (xLAST)
                                                                                   where x = S or D


                                                                                                   Peripheral queues typically
                                                                                   have size and offset equal
                                                                                                 •
                                                                                                 •               Last minor loop     to NBYTES
                                                                                                 •
               xLAST: Number of bytes added to
                   current address after major loop
                        (typically used to loop back)


 Figure 109. Memory array terms

23.5.2 eDMA arbitration
The eDMA uses a layered arbitration scheme composed of multiple priority levels. The eDMA uses a fixed-priority arbitration
scheme with optional round-robin arbitration under specific conditions. The priorities are evaluated in the following order:





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    748 / 3791

<!-- page 749 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Table 299. eDMA arbitration priorities

                    Priority                         Scheme                               Description

 1 (Highest)                                 Arbitration group priority              Each channel is assigned an arbitration
                                                                          group via the CHn_GRPRI registers.
                                                                                                 Priority is given to the highest value (31
                                                                             being the highest possible value) down to
                                                                                 the lowest value (zero, the default).

 2                                  Channel priority                    Each channel is assigned a channel
                                                                                                     priority level via the CHn_PRI registers.
                                                                  The channel priority is a relative priority
                                                                                            level within an arbitration group. Priority
                                                                                                        is given to the highest value (seven being
                                                                                 the highest possible value) down to the
                                                                                lowest value (zero, the default). Channel
                                                                                                    priorities within each arbitration group
                                                                    need not be unique. If multiple channels
                                                                      have the same channel priority level, the
                                                                           channel number will be used to determine
                                                                                                     priority as defined in row three.

 3                                  Channel number                 When two or more channels have
                                                                                 the same arbitration group priority and
                                                                           channel priority, the channel number
                                                              (CHn_NUM) is used to determine the
                                                                                  highest priority. Priority is given to the
                                                                                  highest channel number. Lowest priority
                                                                                                        is channel 0. The channel numbers are
                                                                                                 static and cannot be changed in the
                                                                          programmer's model.

 4 (Lowest)                            Round-robin                    When round-robin is enabled, any
                                                                           channel configured for round-robin
                                                                                 operation has lowest priority within an
                                                                                             arbitration group. Round-robin is enabled
                                                                       by setting the MP_CSR[ERCA] field to
                                                                                          1. After being enabled, channels with
                                                                    a channel priority of zero (CHn_PRI=0)
                                                                                                             will use round-robin arbitration. Round-
                                                                                     robin arbitration will rotate the channel
                                                                                      selection among the channels requesting
                                                                                   service with CHn_PRI=0 within the
                                                                                             arbitration group. Any non-zero channel
                                                                                        within the arbitration group will continue
                                                                                          to use fixed-priority arbitration, and if
                                                                                 requesting service will be selected over
                                                                      any round-robin channels.

For fixed arbitration, the overall priority can be considered a number composed of three concatenated priority levels:
CHn_GRPRI:CHn_PRI:CH_NUM. The largest number has the highest priority and the lowest number has the lowest priority.
For round-robin arbitration, the priority number is CHn_GRPRI:0:X. The module rotates through the CHn_PRI=0 channels
requesting service without regard to priority among these channels. Any channel within the arbitration group for which CHn_PRI
is greater than 0 will be serviced before the round-robin channels.



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    749 / 3791

<!-- page 750 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

23.5.3 Programming errors
The eDMA performs various tests on the transfer control descriptor to verify consistency in the descriptor data.
The channel number causing the error is recorded in the Error Status register (CHn_ES). If the error source is not removed before
the next activation of the problematic channel, the error is detected and recorded again. Setting the halt after error field, CSR[HAE],
will halt the DMA and prevent recurrence of the error.

23.5.4  Arbitration mode considerations
This section discusses arbitration considerations for eDMA.

23.5.4.1  Fixed group arbitration, fixed channel arbitration
In this mode, eDMA selects for execution the channel service request from the highest-priority channel in the highest-priority
group. If eDMA is programmed so that the channels within a high-priority group have a high number of requests or large data
transfers, that group may consume all the bandwidth of the eDMA controller. That is, no lower-priority groups are serviced if there
is always at least one DMA request pending on a channel in the highest-priority group when the controller arbitrates the next DMA
request. The advantage of this scenario is that latency can be small for channels that need to be serviced quickly.

23.5.4.2  Fixed group arbitration, round-robin channel arbitration
The highest-priority group with a request is serviced. Lower-priority groups are serviced if no pending requests exist in the
higher-priority groups.
Within each group, channels are serviced starting with the highest non-zero channel priority. For all channels with a channel
priority programmed to 0, selection begins with the highest channel number requesting service and then rotates through to
the lowest channel number requesting service. The round-robin channel arbitration can provide a fairness mechanism to
lower-priority channels.
This scenario could cause the same bandwidth consumption problem as indicated in Fixed group arbitration, fixed channel
arbitration, but all the channels in the highest-priority group will be serviced. Service latency is short on the highest-priority group,
but could potentially be very much longer as the group priority decreases.

23.5.5  Performing DMA transfers
This section presents examples on how to perform DMA transfers with the eDMA.

23.5.5.1  Single request
To perform a simple transfer of n bytes of data with one activation, set the major loop to one (TCDn_CITER = TCDn_BITER =
1). The data transfer begins after the channel service request is acknowledged and the channel is selected to execute. After the
transfer is complete, the CHn_CSR[DONE] field is set to 1 and an interrupt is generated if properly enabled.
For example, the following TCD entry is configured to transfer 16 bytes of data. The eDMA is programmed for one iteration of the
major loop transferring 16 bytes per iteration. The source memory has a byte-wide memory port located at 0x1000. The destination
memory has a 32-bit port located at 0x2000. The address offsets are programmed in increments to match the transfer size: one
byte for the source, and four bytes for the destination. The final source and destination addresses are adjusted to return to their
beginning values.



     TCDn_CITER = TCDn_BITER = 1
     TCDn_NBYTES = 16
     TCDn_SADDR = 0x1000
     TCDn_SOFF = 1
     TCDn_ATTR[SSIZE] = 0
     TCDn_SLAST = -16
     TCDn_DADDR = 0x2000
     TCDn_DOFF = 4
     TCDn_ATTR[DSIZE] = 2


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    750 / 3791

<!-- page 751 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


     TCDn_DLAST_SGA= –16
     TCDn_CSR[INTMAJ] = 1
     TCDn_CSR[START] = 1 (should be written last after all other fields have been initialized)
     All other TCDn fields = 0

This generates the following event sequence:
   1. User write to the TCDn_CSR[START] field requests channel service.
   2. The channel is selected by arbitration for servicing.
   3. eDMA engine writes:
           • CHn_CSR[DONE] = 0
           • TCDn_CSR[START] = 0
           • CHn_CSR[ACTIVE] = 1
   4. eDMA engine reads: channel TCD data from local memory to internal register file.
   5. The source-to-destination transfers are executed as follows:
          a. Read byte from location 0x1000, read byte from location 0x1001, read byte from 0x1002, read byte from 0x1003.
          b. Write 32 bits to location 0x2000 → first iteration of the minor loop.
           c. Read byte from location 0x1004, read byte from location 0x1005, read byte from 0x1006, read byte from 0x1007.
          d. Write 32 bits to location 0x2004 → second iteration of the minor loop.
          e. Read byte from location 0x1008, read byte from location 0x1009, read byte from 0x100A, read byte from 0x100B.
                f.  Write 32 bits to location 0x2008 → third iteration of the minor loop.
          g. Read byte from location 0x100C, read byte from location 0x100D, read byte from 0x100E, read byte from 0x100F.
          h. Write 32 bits to location 0x200C → last iteration of the minor loop → major loop complete.
   6. The eDMA engine writes: TCDn_SADDR = 0x1000, TCDn_DADDR = 0x2000, TCDn_CITER = 1 (TCDn_BITER).
   7. The eDMA engine writes: CHn_CSR[ACTIVE] = 0, CHn_CSR[DONE] = 1, CHn_INT[INT] = 1.
   8. The channel retires and the eDMA goes idle or services the next channel.

23.5.5.2  Multiple requests
The following example transfers 32 bytes via two hardware requests, but is otherwise the same as the previous example. The only
fields that change are the major loop iteration count and the final address offsets. The eDMA is programmed for two iterations
of the major loop, transferring 16 bytes per iteration. After the channel's hardware requests are enabled via the CHn_CSR[ERQ]
register field, the slave device initiates channel service requests.



      TCDn_CITER = TCDn_BITER = 2
      TCDn_SLAST = –32
      TCDn_DLAST_SGA = –32

This would generate the following sequence of events:
   1.  First hardware (eDMA peripheral) requests channel service.
   2. The channel is selected by arbitration for servicing.
   3. eDMA engine writes: CHn_CSR[DONE] = 0, TCDn_CSR[START] = 0, CHn_CSR[ACTIVE] = 1.
   4. eDMA engine reads: channel TCDndata from local memory to internal register file.
   5. The source-to-destination transfers are executed as follows:



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    751 / 3791

<!-- page 752 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

          a. Read byte from location 0x1000, read byte from location 0x1001, read byte from 0x1002, read byte from 0x1003.
          b. Write 32 bits to location 0x2000 → first iteration of the minor loop.
           c. Read byte from location 0x1004, read byte from location 0x1005, read byte from 0x1006, read byte from 0x1007.
          d. Write 32 bits to location 0x2004 → second iteration of the minor loop.
          e. Read byte from location 0x1008, read byte from location 0x1009, read byte from 0x100A, read byte from 0x100B.
                f.  Write 32 bits to location 0x2008 → third iteration of the minor loop.
          g. Read byte from location 0x100C, read byte from location 0x100D, read byte from 0x100E, read byte from 0x100F.
          h. Write 32 bits to location 0x200C → last iteration of the minor loop.
   6. eDMA engine writes: TCDn_SADDR = 0x1010, TCDn_DADDR = 0x2010, TCDn_CITER = 1.
   7. eDMA engine writes: CHn_CSR[ACTIVE] = 0.
   8. The channel retires, which concludes one iteration of the major loop. The eDMA goes idle or services the next channel.
   9. Second hardware (eDMA peripheral) requests channel service.
   10. The channel is selected by arbitration for servicing.
   11. eDMA engine writes: CHn_CSR[DONE] = 0, TCDn_CSR[START] = 0, CHn_CSR[ACTIVE] = 1.
   12. eDMA engine reads: Channel TCD data from local memory to internal register file.
   13. The source-to-destination transfers are executed as follows:
            a. Read byte from location 0x1010, read byte from location 0x1011, read byte from 0x1012, read byte from 0x1013.
            b. Write 32 bits to location 0x2010 → first iteration of the minor loop.
            c. Read byte from location 0x1014, read byte from location 0x1015, read byte from 0x1016, read byte from 0x1017.
            d. Write 32 bits to location 0x2014 → second iteration of the minor loop.
            e. Read byte from location 0x1018, read byte from location 0x1019, read byte from 0x101A, read byte from 0x101B.
                 f.  Write 32 bits to location 0x2018 → third iteration of the minor loop.
            g. Read byte from location 0x101C, read byte from location 0x101D, read byte from 0x101E, read byte from 0x101F.
            h. Write 32 bits to location 0x201C → last iteration of the minor loop → major loop complete.
   14. eDMA engine writes: TCDn_SADDR = 0x1000, TCDn_DADDR = 0x2000, TCDn_CITER = 2 (TCDn_BITER).
   15. eDMA engine writes: CHn_CSR[ACTIVE] = 0, CHn_CSR[DONE] = 1, CHn_INT[INT] = 1.
   16. The channel retires, which concludes with the major loop complete. The eDMA goes idle or services the next channel.

23.5.5.3  Using the modulo feature
The modulo feature of the eDMA allows implementation of a circular data queue in which the size of the queue is a power of 2.
xMOD is a 5-bit field for the source and destination in the TCD, and it specifies which lower address bits increment from their
original value after the address+offset calculation. All upper address bits remain the same as in the original value. A setting of 0
for this field disables the modulo feature. Modulo addressing applies to cases where the minor loop offset is enabled; that is, the
upper address bits remain the same after the minor loop offset is added to the source or destination address.
The following table shows how the transfer addresses are specified based on the setting of the MOD field. Here a circular buffer
is created where the address wraps to the original value but the 28 upper address bits (0x1234567x) retain their original value.
In this example, the source address is set to 0x12345670, the offset is set to four bytes, and the MOD field is set to four, which
allows for a 24 byte (16 byte) queue size.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    752 / 3791

<!-- page 753 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Table 300. Modulo example

    Transfer number                                           Address

          1                                             0x12345670

          2                                             0x12345674

          3                                             0x12345678

          4                                            0x1234567C

          5                                             0x12345670

          6                                             0x12345674

23.5.6  Monitoring transfer descriptor status
This section discusses how to monitor eDMA status.

23.5.6.1  Testing for minor loop completion
There are two methods to test for minor loop completion when using software-initiated service requests.
   1. The first method is to read the TCDn_CITER field and test for a change.
   2. The second method, extracted from the sequence shown below, is to test the TCDn_CSR[START] field and the
     CHn_CSR[ACTIVE] field. The minor-loop-complete condition is indicated by both fields reading 0 after TCDn_CSR[START]
        is set to 1. Polling the CHn_CSR[ACTIVE] field only may be inconclusive because the active status may be missed if the
      channel execution is short in duration.
The CHn_CSR and TCDn_CSR status fields execute the following sequence for a software-activated channel:

   Stage   TCDn_CSR field        CHn_CSR fields                                   State

            START        ACTIVE       DONE

     1            1              0              0            Initiate channel service request via software.

     2            0              1              0        Channel is executing.

    3a           0              0              0        Channel has completed the minor loop and is idle.

    3b           0              0              1        Channel has completed the major loop and is idle.

The best method to test for minor-loop completion when using hardware-initiated (that is, peripheral-initiated) service requests is
to read the TCDn_CITER field and test for a change. The hardware request and acknowledge handshake signals are not visible
in the programmer's model.
The TCD status fields execute the following sequence for a hardware-activated channel:





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    753 / 3791

<!-- page 754 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


   Stage   TCDn_CSR field        CHn_CSR fields                                   State

            START        ACTIVE       DONE

     1            0              0              0            Initiate channel service request via hardware (peripheral
                                                                 request asserted).

     2            0              1              0        Channel is executing.

    3a           0              0              0        Channel has completed the minor loop and is idle.

    3b           0              0              1        Channel has completed the major loop and is idle.

For both activation types, the major-loop-complete status is explicitly indicated via the CHn_CSR[DONE] field.
The TCDn_CSR[START] field is cleared to 0 automatically when the channel begins execution, regardless of how the
channel activates.

23.5.6.2  Reading the transfer descriptors of active channels
The eDMA reads back the true TCDn_SADDR, TCDn_DADDR, and TCDn_NBYTES values if they are read when a channel
executes. The true values of SADDR, DADDR, and NBYTES are the values the eDMA engine currently uses in its internal register
file, and not the values in the TCD local memory for that channel. The addresses, SADDR and DADDR, and NBYTES (which
decrements to zero as the transfer progresses), can give an indication of the progress of the transfer. All other values are read
back from the TCD local memory.

23.5.6.3  Checking channel preemption status
A preemptive situation is one in which a preempt-enabled channel is executing and a higher-priority request becomes active.
When round-robin channel arbitration mode is enabled, all channels with their channel priority set to 0 lose their preempt ability.
Channel priorities of 0 are treated as equal, that is, they are constantly rotating, when round-robin arbitration mode is enabled.
The CHn_CSR[ACTIVE] field for the preempted channel remains asserted throughout the preemption. The preempted channel
is temporarily suspended when the preempting channel executes one major loop iteration. If two CHn_CSR[ACTIVE] fields are
set simultaneously in the global TCD map, a higher-priority channel is actively preempting a lower-priority channel.

23.5.7 Channel linking
Channel linking (or chaining) is a mechanism in which one channel sets the TCDn_CSR[START] field of another channel (or itself),
thus initiating a service request for that channel. When properly enabled, the eDMA engine automatically performs this operation
at the major or minor loop completion.
The minor loop channel linking occurs at the completion of the minor loop (or one iteration of the major loop). The
TCDn_CITER[ELINK] field determines whether a minor loop link is requested. When enabled, the channel link is made after each
iteration of the major loop except for the last. When the major loop is exhausted, only the major loop channel link fields are used
to determine if a channel link should be made. For example, using an initial field setting of:



      MP_CSR[GCLC] = 1
      TCDn_CITER[ELINK] = 1
      TCDn_CITER[LINKCH] = 0xC
      TCDn_CITER[CITER] value = 0x4
      TCDn_CSR[MAJORELINK] = 1
      TCDn_CSR[MAJORLINKCH] = 0x7

executes as:


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    754 / 3791

<!-- page 755 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

   1. Minor loop done → set TCD12_CSR[START] field
   2. Minor loop done → set TCD12_CSR[START] field
   3. Minor loop done → set TCD12_CSR[START] field
   4. Minor loop done, major loop done→ set TCD7_CSR[START] field
When minor loop linking is enabled (TCDn_CITER[ELINK] = 1), the TCDn_CITER[CITER] field uses a nine-bit vector to form the
current iteration count. When minor loop linking is disabled (TCDn_CITER[ELINK] = 0), the TCDn_CITER[CITER] field uses a
15-bit vector to form the current iteration count. The bits associated with the TCDn_CITER[LINKCH] field are concatenated onto
the CITER value to increase the range of the CITER.

                                           NOTE
            The TCDn_CITER[ELINK] field and the TCDn_BITER[ELINK] field must be equal — if they are not, a configuration
                error is reported. The CITER and BITER vector widths must be equal to calculate the major loop halfway done
                 interrupt point.

The following table summarizes how a DMA channel can link to another DMA channel, that is, use another channel's TCD, at the
end of a loop.

Table 301. Channel linking parameters

   Wanted link     TCD control field name                                    Description
     behavior

 Link at end of    TCDn_CITER[ELINK]         Enable channel-to-channel linking on minor loop completion
 minor loop                                         (current iteration)

               TCDn_CITER[LINKCH]        Link channel number when linking at end of minor loop (current iteration)

 Link at end of    TCDn_CSR[MAJORELINK]    Enable channel-to-channel linking on major loop completion
 major loop
              TCDn_CSR[MAJORLINKCH]  Link channel number when linking at end of major loop

23.5.8 Dynamic programming
This section provides recommended methods to change the programming model during channel execution.

23.5.8.1  Dynamically changing the channel priority
To change group or channel priority levels:
   1. Halt the DMA by writing 1 to the CSR[HALT] field.
   2. Change the group or channel priorities as wanted.
   3. Enable normal DMA operations by writing 0 to the CSR[HALT] field.

23.5.8.2  Dynamic channel linking
Dynamic channel linking is the process of setting the TCDn_CSR[MAJORELINK] field during channel execution (see the diagram
in TCD structure). This field is read from the TCD local memory at the end of channel execution, thus allowing you to enable the
feature during channel execution.
Because you are allowed to change the configuration during execution, you need a coherency model. Consider the scenario
where you attempt to execute a dynamic channel link by enabling the TCDn_CSR[MAJORELINK] field at the same time the
eDMA engine is retiring the channel. TCDn_CSR[MAJORELINK] would be set in the programmer’s model, but it would be unclear
whether the actual link was made before the channel retired.
We recommend that you use the following coherency model when executing a dynamic channel link request.


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    755 / 3791

<!-- page 756 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

   1. Write 1 to the TCDn_CSR[MAJORELINK] field.
   2. Read back the TCDn_CSR[MAJORELINK] field.
   3. Test the TCDn_CSR[MAJORELINK] request status:
           •  If TCDn_CSR[MAJORELINK] = 1, the dynamic link attempt was successful.
           •  If TCDn_CSR[MAJORELINK] = 0, the attempted dynamic link did not succeed (the channel was already retiring).
For this request, the TCD local memory controller forces the TCDn_CSR[MAJORELINK] field to 0 on any writes to a channel’s
TCDn_CSR[7:0] after that channel’s CHn_CSR[DONE] field is set to 1, indicating the major loop is complete.

                                           NOTE
           You must clear the CHn_CSR[DONE] field to 0 before writing to the TCDn_CSR[MAJORELINK] field. The
           CHn_CSR[DONE] field is cleared to 0 automatically by the eDMA engine after a channel begins execution.


23.5.8.3  Dynamic scatter/gather
Scatter/gather is the process of automatically loading a new TCD into a channel. It allows a DMA channel to use multiple TCDs; this
enables a DMA channel to scatter the DMA data to multiple destinations or gather it from multiple sources. When scatter/gather
is enabled and the channel has finished its major loop, a new TCD is fetched from system memory and loaded into that channel’s
descriptor location in the eDMA programmer’s model, thus replacing the current descriptor.
Because you are allowed to change the configuration during execution, you need a coherency model. Consider the scenario
where you attempt to execute a dynamic scatter/gather operation by enabling the TCDn_CSR[ESG] field at the same time the
eDMA engine is retiring the channel. The TCDn_CSR[ESG] field would be set in the programmer’s model, but it would be unclear
whether the actual scatter/gather request was honored before the channel retired.
Two methods are recommended for executing a dynamic scatter/gather request. Whenever the TCDn_CSR is written, the TCD
local memory controller forces the TCDn_CSR[ESG] field to 0 on any writes to a channel’s TCDn_CSR[7:0] after that channel’s
CHn_CSR[DONE] field has been set to 1, indicating the major loop is complete. If attempting to set the ESG, ensure the DONE
field is cleared to 0.

                                           NOTE
           You must clear the CHn_CSR[DONE] field to 0 before writing the TCDn_CSR[MAJORELINK] or TCDn_CSR[ESG]
                  fields. The CHn_CSR[DONE] field is cleared to 0 automatically by the eDMA engine after a channel begins
              execution and is set to 1 upon major loop completion.

23.5.8.3.1  Method 1 (channel not using major loop channel linking)
For a channel not using major loop channel linking, the coherency model described here may be used for a dynamic scatter/
gather request.
When the TCDn_CSR[MAJORELINK] field is 0, the TCDn_CSR[MAJORLINKCH] field is not used by the eDMA. In this case, the
TCDn_CSR[MAJORLINKCH] bits may be used for other purposes. This method uses the TCDn_CSR[MAJORLINKCH] field as
a TCDn_CSR identification (ID).
When the descriptors are built, write a unique TCDn_CSR ID in the TCDn_CSR[MAJORLINKCH] field for each TCDn_CSR
associated with a channel using dynamic scatter/gather.
   1. Write a 1 to the TCDn_CSR[DREQ] field. Should a dynamic scatter/gather attempt fail, setting the TCDn_CSR[DREQ]
        field to 1 will prevent future hardware activation of this channel. This stops the channel from executing with a destination
      address (daddr) that was calculated using a scatter/gather address (written in the next step) instead of a DLAST final
       offset value.
   2. Write the TCDn_DLAST_SGA field with the scatter/gather address.
   3. Write a 1 to the TCDn_CSR[ESG] field.
   4. Read back the 16-bit TCDn_CSR control/status field.
   5. Test the TCDn_CSR[ESG] request status and TCDn_CSR[MAJORLINKCH] value:


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    756 / 3791

<!-- page 757 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

           •  If ESG = 1, the dynamic scatter/gather attempt was successful.
           •  If ESG = 0 and the MAJORLINKCH (ID) did not change, the dynamic scatter/gather attempt was not successful (the
          channel was already retiring).
           •  If ESG = 0 and the MAJORLINKCH (ID) changed, the dynamic scatter/gather attempt was successful (the new
        TCDn_CSR’s ESG value cleared the ESG field to 0).

23.5.8.3.2  Method 2 (channel using major loop channel linking)
For a channel using major loop channel linking, the coherency model described here may be used for a dynamic scatter/gather
request. This method uses the TCDn_DLAST_SGA field as a TCD identification (ID).
   1. Write a 1 to the TCDn_CSR[DREQ] field. Should a dynamic scatter/gather attempt fail, setting the DREQ field to 1 will
      prevent a future hardware activation of this channel. This stops the channel from executing with a destination address
     (DADDR) that was calculated using a scatter/gather address (written in the next step) instead of a DLAST final offset
       value.
   2. Write the TCDn_DLAST_SGA field with the scatter/gather address.
   3. Write a 1 to the TCDn_CSR[ESG] field.
   4. Read back the TCDn_CSR[ESG] field.
   5. Test the TCDn_CSR[ESG] request status:
           •  If ESG = 1, the dynamic scatter/gather attempt was successful.
           •  If ESG = 0, read the 32-bit TCDn_DLAST_SGA field.
           •  If ESG = 0 and the TCDn_DLAST_SGA did not change, the dynamic scatter/gather attempt was not successful (the
          channel was already retiring).
           •  If ESG = 0 and the TCDn_DLAST_SGA changed, the dynamic scatter/gather attempt was successful (the new
        TCDn_CSR’s ESG value cleared the ESG field to 0).

23.5.9 Suspend/resume a DMA channel with active hardware service requests
The DMA allows you to move data from memory or peripheral registers to another location in memory or to peripheral registers
without CPU interaction. After the DMA and peripherals are configured and active, it is rare but supported to suspend a peripheral's
service request dynamically. In this scenario, there are certain restrictions to disabling a DMA hardware service request. For
coherency, you must follow a specific procedure. This section provides guidance on how to coherently suspend and resume a
Direct Memory Access (DMA) channel when the DMA is triggered by a slave module such as the Serial Peripheral Interface
(DSPI), Sigma Delta Analog to Digital Convertor (SDADC), or other module.

23.5.9.1  Suspend an active DMA channel
To suspend an active DMA channel:
   1. Stop the DMA service request at the peripheral first. Confirm it has been disabled by reading back the appropriate register
       in the peripheral.
   2. Check the DMA's Hardware Request Status (MP_HRS) to ensure there is no service request to the DMA channel being
      suspended. Then disable the hardware service request by clearing the ERQ field to 0 on the appropriate DMA channel.
For example, assume the DSPI is set as a master for transmitting data via a DMA service request when the TXFIFO has an empty
slot. The DMA will transfer the next command and data to the TXFIFO upon the request. If you need to suspend the DMA/DSPI
transfer loop, perform the following steps:
   1. Disable the DMA service request at the source by writing 0 to DSPI_RSER[TFFF_RE]. Confirm that
     DSPI_RSER[TFFF_RE] is 0.
   2. Ensure there is no DMA service request from the DSPI by verifying that MP_HRS[HRS] is 0 for the appropriate channel.
          If no service request is present, disable the DMA channel by clearing the channel's ERQ field to 0. If a service request is
      present, wait until the request has been processed and the HRS field reads 0.


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    757 / 3791

<!-- page 758 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

23.5.9.2 Resume a DMA channel
To resume a DMA channel:
   1. Enable the DMA service request on the appropriate channel by setting its ERQ field to 1.
   2. Enable the DMA service request at the peripheral.

23.6 Memory map/register definition
The eDMA programming model is partitioned into three parts:
   1. The first part defines a number of registers providing overall control functions and is known as the management page.
   2. The second part corresponds to the channel (CH) control, status, and configuration.
   3. The third part corresponds to the local TCD memory.

TCD memory
Each channel requires a 32-byte transfer control descriptor for defining the data movement operation. Each TCDn definition is
presented as 11 registers of 16 or 32 bits. See DMA TCD register descriptions for details.

TCD initialization
Prior to activating a channel, you must initialize its TCD with the appropriate transfer profile.

TCD structure





         31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0

   0020h                                        SADDR

   0024h   SMOD         SSIZE     DMOD         DSIZE                      SOFF


   0028h                           MLOFF or NBYTES                                NBYTES                       SMLOE  DMLOE

   002Ch                                            SLAST

   0030h                                        DADDR



                   CITER or
   0034h                                 CITER                              DOFF
                  CITER.LINKCH                                                    Reserved                                                  CITER.ELINK

   0038h                                         DLAST_SGA



                    BITER or
   003Ch                                  BITER          BWC                  BITER.LINKCH                                                                                                               ESDA   EEOP       ESG  DREQ                 START                                                                                                                                                                                                                                                                                                                                                                 INTHALF                                                    Reserved                                                                                                                                                                                                                  Reserved                                                                                                                                                                                                                                                                                                                                                                                                                               INTMAJOR                                                  BITER.ELINK                                                                                                                                                                                                                                                                                                                                                                                                                                                                              MAJOR.LINKCH                                                                      MAJOR.ELINK
         31 30 29 28 27 26 25 24 23 22 21 20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5  4  3  2  1  0

 Figure 110. TCD structure


Accesses to reserved memory and fields
   • Reading reserved fields in a register returns the value of zero.


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    758 / 3791

<!-- page 759 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

   • Writes to reserved fields in a register are ignored.
   • Reading or writing a reserved memory location generates a bus error.

23.6.1 DMA MP register descriptions

23.6.1.1 MP memory map
eDMA_0_MP base address: 4008_0000h

     Offset     Register                                                        Width    Access   Reset value
                                                                                                     (In bits)

     0h      Management Page Control (MP_CSR)                            32    RW    0031_0000h

     4h      Management Page Error Status (MP_ES)                          32     R     0000_0000h

     8h      Management Page Interrupt Request Status (MP_INT)                32     R     0000_0000h

     Ch      Management Page Hardware Request Status (MP_HRS)              32     R     0000_0000h

 100h - 13Ch  Channel Arbitration Group (CH0_GRPRI - CH15_GRPRI)             32    RW    0000_0000h

23.6.1.2 Management Page Control (MP_CSR)

Offset


 Register                   Offset

 MP_CSR              0h


Function
The Management Page Control register defines the basic operating configuration of the DMA.
Arbitration uses a two-tier priority system; group and channel priority. The eDMA assigns each channel to a priority group. Group
arbitration is fixed-priority and cannot be changed. Channel arbitration uses fixed priority and may be configured to use a selective
round-robin scheme for specified channels within each priority group. For fixed-priority arbitration, eDMA selects for execution the
highest priority channel requesting service in the highest priority arbitration group.
The channel priority registers assign the relative priorities within each arbitration group; see CHn_PRI. All channels with a
non-zero CHn_PRI value use fixed-priority arbitration.
When you enable round-robin arbitration, all channels with channel priority set to zero do not have a priority and, of those channels
requesting service, are cycled through (from high to low channel number) without regard to priority relative to each other within
the same priority group. Any channel with a non-zero CHn_PRI value automatically has a higher priority over the round-robin
channels. A channel's priority group is assigned in Channel Arbitration Group (CH0_GRPRI - CH15_GRPRI).

                                           NOTE
              For correct operation, changes to the MP_CSR[ERCA, GCLC, GMRC] fields must be performed when the DMA
             channels are inactive; that is, when the MP_CSR[ACTIVE] field is 0.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    759 / 3791

<!-- page 760 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits       31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

        ACTIV
    R                Reserved                 ACTIVE_ID                                     Reserved
         E

   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       0      0      0      1

 Bits      15     14     13     12     11     10      9      8       7      6      5      4       3      2      1      0

                                                                                                      Reserv                  Reserv
    R                    Reserved
                                                                                                  ed                    ed                                           CX    ECX  GMRC  GCLC  HALT   HAE         ERCA  EDBG
   W                      0                                                                     0                     0

Reset     0      0      0      0      0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31     DMA Active Status
    ACTIVE          0b - eDMA is idle
                     1b - eDMA is executing a channel

     30-28      Reserved
    —

     27-24       Active Channel ID
   ACTIVE_ID    This field identifies the channel number that is executing when the ACTIVE bit is 1.

     23-16      Reserved
    —

     15-10      Reserved
    —

       9        Cancel Transfer
     CX      When set to 1, this field cancels the remaining data transfer, stops the executing channel, and forces the
                 minor loop to finish. The cancel takes effect after the last write of the current read/write sequence. CX
                   clears itself to 0 after the cancel has been honored. This cancel retires the channel normally as if the
                 minor loop had been completed.
                     0b - Normal operation
                     1b - Cancel the remaining data transfer

       8        Cancel Transfer With Error
    ECX       Cancellation of the remaining data transfer is similar to that of the CX field. Execution of the channel is
                stopped and the minor loop is forced to finish. The cancellation takes effect after the last write of the
                   current read/write sequence. The ECX field clears itself to 0 after the cancel is honored. In addition to

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    760 / 3791

<!-- page 761 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                   cancelling the transfer, ECX treats the cancel as an error condition, thus updating Management Page
                   Error Status (MP_ES) and generating an optional error interrupt.
                     0b - Normal operation
                     1b - Cancel the remaining data transfer

       7        Global Master ID Replication Control
   GMRC                                                 NOTE
                                                         If master ID replication is disabled, the nonsecure, user protection level for DMA transfers
                                            is used.

                     0b - Master ID replication disabled for all channels
                     1b - Master ID replication available and controlled by each channel's CHn_SBR[EMI] setting

       6        Global Channel Linking Control
    GCLC           0b - Channel linking disabled for all channels
                     1b - Channel linking available and controlled by each channel's link settings

       5         Halt DMA Operations
    HALT      This field stalls the start of any new channels. Executing channels are allowed to complete. Channel
                  execution resumes when this field is cleared to 0.
                     0b - Normal operation
                     1b - Stall the start of any new channels

       4         Halt After Error
    HAE     When this field is set to 1, any error causes the HALT field to be set to 1. Then all service requests are
                  ignored until the HALT field is cleared to 0.
                     0b - Normal operation
                     1b - Any error causes the HALT field to be set to 1

       3        Reserved
    —

       2        Enable Round Robin Channel Arbitration
    ERCA           0b - Round-robin channel arbitration disabled. Fixed priority arbitration used for channel selection
                     1b - Round-robin channel arbitration enabled. Round-robin arbitration used for channel selection

       1        Enable Debug
    EDBG    When in debug mode, the DMA stalls the start of a new channel. Executing channels are allowed to
                 complete. DMA resumes channel execution when the system exits debug mode or clears the EDBG field
                    to 0.
                     0b - Debug mode disabled. When in debug mode, the DMA continues to operate

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    761 / 3791

<!-- page 762 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1b - Debug mode is enabled. When in debug mode, the DMA stalls the start of a new channel

       0        Reserved
    —


23.6.1.3 Management Page Error Status (MP_ES)

Offset


 Register                   Offset

 MP_ES                4h


Function
The ES provides information concerning the last recorded channel error. Channel errors can be caused by:
   • An illegal setting in the transfer control descriptor
   • An error termination to a bus master read or write cycle
   • An uncorrectable error that occurred when the device was accessing the TCD SRAM
   • A "cancel transfer with error" request was made via the corresponding cancel transfer field or input signal
Upon any error condition, the software must initialize the TCD of the channel that contains the error, as it is in an incomplete state
after an error. See Fault reporting and handling for more details.

Diagram

 Bits       31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R  VLD          Reserved              ERRCHN                                        0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6       5      4       3      2       1       0

    R                          0                      ECX   SAE   SOE   DAE   DOE   NCE   SGE   SBE   DBE

   W

Reset     0      0      0      0       0      0      0      0       0      0       0      0       0      0       0       0


Fields


       Field       Function

      31         Valid
                  Logical OR of all CHn_ES[ERR] status fields.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    762 / 3791

<!-- page 763 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function

     VLD            0b - No CHn_ES[ERR] fields are set to 1
                     1b - At least one CHn_ES[ERR] field is set to 1, indicating a valid error exists that software has
                         not cleared

     30-28      Reserved
    —

     27-24       Error Channel Number or Canceled Channel Number
   ERRCHN    The channel number of the last recorded error or last recorded error-canceled transfer.

      23-9      Reserved
    —

       8         Transfer Canceled
    ECX      The ECX operation is a management page function. When employed, the targeted channel's CHn_ES
                    register reports an unspecified error; that is, only the CHn_ES[ERR] field is set to 1. The management
               page has full view of the error condition.
                     0b - No canceled transfers
                     1b - Last recorded entry was a canceled transfer by the error cancel transfer input

       7        Source Address Error
     SAE     When this field is 1, it indicates that TCDn_SADDR is inconsistent with TCDn_ATTR[SSIZE].
                     0b - No source address configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_SADDR field

       6        Source Offset Error
    SOE     When this field is 1, it indicates that TCDn_SOFF is inconsistent with TCDn_ATTR[SSIZE].
                     0b - No source offset configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_SOFF field

       5         Destination Address Error
    DAE     When this field is 1, it indicates that TCDn_DADDR is inconsistent with TCDn_ATTR[DSIZE].
                     0b - No destination address configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_DADDR field

       4         Destination Offset Error
    DOE     When this field is 1, it indicates that TCDn_DOFF is inconsistent with TCDn_ATTR[DSIZE].
                     0b - No destination offset configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_DOFF field

       3       NBYTES/CITER Configuration Error

                                  Tablecontinuesonthenextpage...

                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    763 / 3791

<!-- page 764 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function
    NCE       This error indicates that one of the following has occurred:
                           • TCDn_NBYTES is not a multiple of TCDn_ATTR[SSIZE] and TCDn_ATTR[DSIZE]
                           • TCDn_CITER[CITER] is equal to zero
                           • TCDn_CITER[ELINK] is not equal to TCDn_BITER[ELINK]
                     0b - No NBYTES/CITER configuration error
                     1b - The last recorded error was NBYTES equal to zero or a CITER not equal to BITER error.
                        Last recorded error was a configuration error detected in the TCDn_NBYTES or TCDn_CITER
                              fields

       2         Scatter/Gather Configuration Error
    SGE     When this field is 1, it indicates that TCDn_DLAST_SGA is not on a 32-byte boundary. This field is
                checked at the beginning of a scatter/gather operation after major loop completion if TCDn_CSR[ESG] is
                 enabled.
                     0b - No scatter/gather configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_DLAST_SGA field

       1        Source Bus Error
     SBE            0b - No source bus error
                     1b - Last recorded error was a bus error on a source read

       0         Destination Bus Error
    DBE            0b - No destination bus error
                     1b - Last recorded error was a bus error on a destination write


23.6.1.4 Management Page Interrupt Request Status (MP_INT)

Offset


 Register                   Offset

 MP_INT               8h


Function
This register shows the current state of the interrupt service requests for all eDMA channels.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    764 / 3791

<!-- page 765 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                             INT

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-16      Reserved
    —

      15-0        Interrupt Request Status
      INT      The INT register presents the interrupt request status for each eDMA channel. Depending on the
                  appropriate field setting in the transfer control descriptors, the eDMA engine generates an interrupt
               on data transfer completion or an error condition. The eDMA routes channel interrupt requests to the
                     interrupt controller. During the interrupt service routine associated with any given channel, it is the software's
                    responsibility to clear the appropriate field in the channel’s interrupt request register, CHn_INT, thus
                 negating the interrupt request.
               0b - Interrupt request for corresponding channel not present
               1b - Interrupt request for corresponding channel present


23.6.1.5 Management Page Hardware Request Status (MP_HRS)

Offset


 Register                   Offset

 MP_HRS            Ch


Function
The hardware request status register (HRS) shows the current state of the hardware service request signaling as seen by eDMA's
arbitration logic.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    765 / 3791

<!-- page 766 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                              HRS

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                              HRS

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function

      31-0      Hardware Request Status
    HRS      The HRS bit for its respective channel remains asserted for the period when a hardware request is present
               on the channel.
               0b - Hardware service request for corresponding channel is not present
               1b - Hardware service request for corresponding channel is present


23.6.1.6  Channel Arbitration Group (CH0_GRPRI - CH15_GRPRI)

Offset
For n = 0 to 15:


 Register                   Offset

 CHn_GRPRI           100h + (n × 4h)


Function
The contents of this register define the arbitration group associated with each channel. Using a fixed-priority group arbitration
scheme, eDMA evaluates the arbitration group priorities by numeric value from highest group number to lowest; for example, 0 is
the lowest priority, 1 is the next higher priority, then 2, 3, and so on. The range of the group priority values is limited to the values
of 0 through 31. Within each arbitration group, the channel priority assignment CHn_PRI determines the highest-priority channel.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    766 / 3791

<!-- page 767 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                         0
                                                                                            GRPRI
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-5      Reserved
    —

       4-0         Arbitration Group For Channel n
    GRPRI       Fixed-priority arbitration group number.

23.6.2 DMA MP register descriptions

23.6.2.1 MP memory map
eDMA_1_MP base address: 400A_0000h

     Offset     Register                                                        Width    Access   Reset value
                                                                                                     (In bits)

     0h      Management Page Control (MP_CSR)                            32    RW    0031_0000h

     4h      Management Page Error Status (MP_ES)                          32     R     0000_0000h

     8h      Management Page Interrupt Request Status (MP_INT)                32     R     0000_0000h

     Ch      Management Page Hardware Request Status (MP_HRS)              32     R     0000_0000h

 100h - 13Ch  Channel Arbitration Group (CH0_GRPRI - CH15_GRPRI)             32    RW    0000_0000h

23.6.2.2 Management Page Control (MP_CSR)

Offset


 Register                   Offset

 MP_CSR              0h




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    767 / 3791

<!-- page 768 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Function
The Management Page Control register defines the basic operating configuration of the DMA.
Arbitration uses a two-tier priority system; group and channel priority. The eDMA assigns each channel to a priority group. Group
arbitration is fixed-priority and cannot be changed. Channel arbitration uses fixed priority and may be configured to use a selective
round-robin scheme for specified channels within each priority group. For fixed-priority arbitration, eDMA selects for execution the
highest priority channel requesting service in the highest priority arbitration group.
The channel priority registers assign the relative priorities within each arbitration group; see CHn_PRI. All channels with a
non-zero CHn_PRI value use fixed-priority arbitration.
When you enable round-robin arbitration, all channels with channel priority set to zero do not have a priority and, of those channels
requesting service, are cycled through (from high to low channel number) without regard to priority relative to each other within
the same priority group. Any channel with a non-zero CHn_PRI value automatically has a higher priority over the round-robin
channels. A channel's priority group is assigned in Channel Arbitration Group (CH0_GRPRI - CH15_GRPRI).

                                           NOTE
              For correct operation, changes to the MP_CSR[ERCA, GCLC, GMRC] fields must be performed when the DMA
             channels are inactive; that is, when the MP_CSR[ACTIVE] field is 0.


Diagram

 Bits       31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

        ACTIV
    R                Reserved                 ACTIVE_ID                                     Reserved
         E

   W

Reset     0      0      0      0       0      0      0      0       0      0      1      1       0      0      0      1

 Bits      15     14     13     12     11     10      9      8       7      6      5      4       3      2      1      0

                                                                                                      Reserv                  Reserv
    R                    Reserved
                                                                                                  ed                    ed                                           CX    ECX  GMRC  GCLC  HALT   HAE         ERCA  EDBG
   W                      0                                                                     0                     0

Reset     0      0      0      0      0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31     DMA Active Status
    ACTIVE          0b - eDMA is idle
                     1b - eDMA is executing a channel

     30-28      Reserved
    —

     27-24       Active Channel ID
   ACTIVE_ID    This field identifies the channel number that is executing when the ACTIVE bit is 1.

     23-16      Reserved

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    768 / 3791

<!-- page 769 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function

    —

     15-10      Reserved
    —

       9        Cancel Transfer
     CX      When set to 1, this field cancels the remaining data transfer, stops the executing channel, and forces the
                 minor loop to finish. The cancel takes effect after the last write of the current read/write sequence. CX
                   clears itself to 0 after the cancel has been honored. This cancel retires the channel normally as if the
                 minor loop had been completed.
                     0b - Normal operation
                     1b - Cancel the remaining data transfer

       8        Cancel Transfer With Error
    ECX       Cancellation of the remaining data transfer is similar to that of the CX field. Execution of the channel is
                stopped and the minor loop is forced to finish. The cancellation takes effect after the last write of the
                   current read/write sequence. The ECX field clears itself to 0 after the cancel is honored. In addition to
                   cancelling the transfer, ECX treats the cancel as an error condition, thus updating Management Page
                   Error Status (MP_ES) and generating an optional error interrupt.
                     0b - Normal operation
                     1b - Cancel the remaining data transfer

       7        Global Master ID Replication Control
   GMRC                                                 NOTE
                                                         If master ID replication is disabled, the nonsecure, user protection level for DMA transfers
                                            is used.

                     0b - Master ID replication disabled for all channels
                     1b - Master ID replication available and controlled by each channel's CHn_SBR[EMI] setting

       6        Global Channel Linking Control
    GCLC           0b - Channel linking disabled for all channels
                     1b - Channel linking available and controlled by each channel's link settings

       5         Halt DMA Operations
    HALT      This field stalls the start of any new channels. Executing channels are allowed to complete. Channel
                  execution resumes when this field is cleared to 0.
                     0b - Normal operation
                     1b - Stall the start of any new channels

       4         Halt After Error
    HAE

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    769 / 3791

<!-- page 770 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function

             When this field is set to 1, any error causes the HALT field to be set to 1. Then all service requests are
                  ignored until the HALT field is cleared to 0.
                     0b - Normal operation
                     1b - Any error causes the HALT field to be set to 1

       3        Reserved
    —

       2        Enable Round Robin Channel Arbitration
    ERCA           0b - Round-robin channel arbitration disabled. Fixed priority arbitration used for channel selection
                     1b - Round-robin channel arbitration enabled. Round-robin arbitration used for channel selection

       1        Enable Debug
    EDBG    When in debug mode, the DMA stalls the start of a new channel. Executing channels are allowed to
                 complete. DMA resumes channel execution when the system exits debug mode or clears the EDBG field
                    to 0.
                     0b - Debug mode disabled. When in debug mode, the DMA continues to operate
                     1b - Debug mode is enabled. When in debug mode, the DMA stalls the start of a new channel

       0        Reserved
    —


23.6.2.3 Management Page Error Status (MP_ES)

Offset


 Register                   Offset

 MP_ES                4h


Function
The ES provides information concerning the last recorded channel error. Channel errors can be caused by:
   • An illegal setting in the transfer control descriptor
   • An error termination to a bus master read or write cycle
   • An uncorrectable error that occurred when the device was accessing the TCD SRAM
   • A "cancel transfer with error" request was made via the corresponding cancel transfer field or input signal
Upon any error condition, the software must initialize the TCD of the channel that contains the error, as it is in an incomplete state
after an error. See Fault reporting and handling for more details.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    770 / 3791

<!-- page 771 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits       31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R  VLD          Reserved              ERRCHN                                        0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6       5      4       3      2       1       0

    R                          0                      ECX   SAE   SOE   DAE   DOE   NCE   SGE   SBE   DBE

   W

Reset     0      0      0      0       0      0      0      0       0      0       0      0       0      0       0       0


Fields


       Field       Function

      31         Valid
     VLD       Logical OR of all CHn_ES[ERR] status fields.
                     0b - No CHn_ES[ERR] fields are set to 1
                     1b - At least one CHn_ES[ERR] field is set to 1, indicating a valid error exists that software has
                         not cleared

     30-28      Reserved
    —

     27-24       Error Channel Number or Canceled Channel Number
   ERRCHN    The channel number of the last recorded error or last recorded error-canceled transfer.

      23-9      Reserved
    —

       8         Transfer Canceled
    ECX      The ECX operation is a management page function. When employed, the targeted channel's CHn_ES
                    register reports an unspecified error; that is, only the CHn_ES[ERR] field is set to 1. The management
               page has full view of the error condition.
                     0b - No canceled transfers
                     1b - Last recorded entry was a canceled transfer by the error cancel transfer input

       7        Source Address Error
     SAE     When this field is 1, it indicates that TCDn_SADDR is inconsistent with TCDn_ATTR[SSIZE].
                     0b - No source address configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_SADDR field

       6        Source Offset Error

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    771 / 3791

<!-- page 772 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function
    SOE     When this field is 1, it indicates that TCDn_SOFF is inconsistent with TCDn_ATTR[SSIZE].
                     0b - No source offset configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_SOFF field

       5         Destination Address Error
    DAE     When this field is 1, it indicates that TCDn_DADDR is inconsistent with TCDn_ATTR[DSIZE].
                     0b - No destination address configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_DADDR field

       4         Destination Offset Error
    DOE     When this field is 1, it indicates that TCDn_DOFF is inconsistent with TCDn_ATTR[DSIZE].
                     0b - No destination offset configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_DOFF field

       3       NBYTES/CITER Configuration Error
    NCE       This error indicates that one of the following has occurred:
                           • TCDn_NBYTES is not a multiple of TCDn_ATTR[SSIZE] and TCDn_ATTR[DSIZE]
                           • TCDn_CITER[CITER] is equal to zero
                           • TCDn_CITER[ELINK] is not equal to TCDn_BITER[ELINK]
                     0b - No NBYTES/CITER configuration error
                     1b - The last recorded error was NBYTES equal to zero or a CITER not equal to BITER error.
                        Last recorded error was a configuration error detected in the TCDn_NBYTES or TCDn_CITER
                              fields

       2         Scatter/Gather Configuration Error
    SGE     When this field is 1, it indicates that TCDn_DLAST_SGA is not on a 32-byte boundary. This field is
                checked at the beginning of a scatter/gather operation after major loop completion if TCDn_CSR[ESG] is
                 enabled.
                     0b - No scatter/gather configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_DLAST_SGA field

       1        Source Bus Error
     SBE            0b - No source bus error
                     1b - Last recorded error was a bus error on a source read

       0         Destination Bus Error
    DBE            0b - No destination bus error
                     1b - Last recorded error was a bus error on a destination write





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    772 / 3791

<!-- page 773 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

23.6.2.4 Management Page Interrupt Request Status (MP_INT)

Offset


 Register                   Offset

 MP_INT               8h


Function
This register shows the current state of the interrupt service requests for all eDMA channels.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                             INT

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
     31-16      Reserved
    —

      15-0        Interrupt Request Status
      INT      The INT register presents the interrupt request status for each eDMA channel. Depending on the
                  appropriate field setting in the transfer control descriptors, the eDMA engine generates an interrupt
               on data transfer completion or an error condition. The eDMA routes channel interrupt requests to the
                     interrupt controller. During the interrupt service routine associated with any given channel, it is the software's
                    responsibility to clear the appropriate field in the channel’s interrupt request register, CHn_INT, thus
                 negating the interrupt request.
               0b - Interrupt request for corresponding channel not present
               1b - Interrupt request for corresponding channel present





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    773 / 3791

<!-- page 774 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

23.6.2.5 Management Page Hardware Request Status (MP_HRS)

Offset


 Register                   Offset

 MP_HRS            Ch


Function
The hardware request status register (HRS) shows the current state of the hardware service request signaling as seen by eDMA's
arbitration logic.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                              HRS

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                              HRS

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function

      31-0      Hardware Request Status
    HRS      The HRS bit for its respective channel remains asserted for the period when a hardware request is present
               on the channel.
               0b - Hardware service request for corresponding channel is not present
               1b - Hardware service request for corresponding channel is present


23.6.2.6  Channel Arbitration Group (CH0_GRPRI - CH15_GRPRI)

Offset
For n = 0 to 15:


 Register                   Offset

 CHn_GRPRI           100h + (n × 4h)





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    774 / 3791

<!-- page 775 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Function
The contents of this register define the arbitration group associated with each channel. Using a fixed-priority group arbitration
scheme, eDMA evaluates the arbitration group priorities by numeric value from highest group number to lowest; for example, 0 is
the lowest priority, 1 is the next higher priority, then 2, 3, and so on. The range of the group priority values is limited to the values
of 0 through 31. Within each arbitration group, the channel priority assignment CHn_PRI determines the highest-priority channel.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                         0
                                                                                            GRPRI
   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-5      Reserved
    —

       4-0         Arbitration Group For Channel n
    GRPRI       Fixed-priority arbitration group number.

23.6.3 DMA TCD register descriptions

23.6.3.1 TCD memory map
eDMA_0_TCD base address: 4008_1000h

     Offset     Register                                                        Width    Access   Reset value
                                                                                                     (In bits)

  0h - F000h   Channel Control and Status (CH0_CSR - CH15_CSR)                32    RW    0000_0000h

  4h - F004h   Channel Error Status (CH0_ES - CH15_ES)                        32    RW    0000_0000h

  8h - F008h   Channel Interrupt Status (CH0_INT - CH15_INT)                    32    RW    0000_0000h

 Ch - F00Ch  Channel System Bus (CH0_SBR - CH15_SBR)                      32    RW    0000_0006h

  10h - F010h  Channel Priority (CH0_PRI - CH15_PRI)                           32    RW    0000_0000h

  14h - F014h  Channel Multiplexor Configuration (CH0_MUX - CH15_MUX)           32    RW    0000_0000h

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    775 / 3791

<!-- page 776 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

     Offset     Register                                                        Width    Access   Reset value
                                                                                                     (In bits)

  20h - F020h  TCD Source Address (TCD0_SADDR - TCD15_SADDR)              32    RW    See section

  24h - F024h  TCD Signed Source Address Offset (TCD0_SOFF - TCD15_SOFF)      16    RW    See section

  26h - F026h  TCD Transfer Attributes (TCD0_ATTR - TCD15_ATTR)               16    RW    See section

  28h - F028h  TCD Transfer Size Without Minor Loop Offsets                      32    RW    See section
            (TCD0_NBYTES_MLOFFNO - TCD15_NBYTES_MLOFFNO)

  28h - F028h  TCD Transfer Size with Minor Loop Offsets                         32    RW    See section
            (TCD0_NBYTES_MLOFFYES - TCD15_NBYTES_MLOFFYES)

 2Ch - F02Ch TCD Last Source Address Adjustment / Store DADDR Address         32    RW    See section
            (TCD0_SLAST_SDA - TCD15_SLAST_SDA)

  30h - F030h  TCD Destination Address (TCD0_DADDR - TCD15_DADDR)           32    RW    See section

  34h - F034h  TCD Signed Destination Address Offset (TCD0_DOFF -               16    RW    See section
            TCD15_DOFF)

  36h - F036h  TCD Current Major Loop Count (Minor Loop Channel Linking           16    RW    See section
                Disabled) (TCD0_CITER_ELINKNO - TCD15_CITER_ELINKNO)

  36h - F036h  TCD Current Major Loop Count (Minor Loop Channel Linking           16    RW    See section
               Enabled) (TCD0_CITER_ELINKYES - TCD15_CITER_ELINKYES)

  38h - F038h  TCD Last Destination Address Adjustment / Scatter Gather Address     32    RW    See section
            (TCD0_DLAST_SGA - TCD15_DLAST_SGA)

 3Ch - F03Ch TCD Control and Status (TCD0_CSR - TCD15_CSR)                 16    RW    See section

 3Eh - F03Eh  TCD Beginning Major Loop Count (Minor Loop Channel Linking         16    RW    See section
                Disabled) (TCD0_BITER_ELINKNO - TCD15_BITER_ELINKNO)

 3Eh - F03Eh  TCD Beginning Major Loop Count (Minor Loop Channel Linking         16    RW    See section
               Enabled) (TCD0_BITER_ELINKYES - TCD15_BITER_ELINKYES)

23.6.3.2  Channel Control and Status (CH0_CSR - CH15_CSR)

Offset
For n = 0 to 15:


 Register                   Offset

 CHn_CSR             0h + (n × 1000h)


Function
This register contains several fields related to hardware and interrupt requests, configuration, and status for the given channel.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    776 / 3791

<!-- page 777 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits       31      30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

        ACTIV
    R       DONE
         E

   W        W1C                                                     0

Reset     0       0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2       1       0

    R
                                                                       EBW    EEI   EARQ  ERQ
   W                                            0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0       0       0


Fields


       Field       Function

      31       Channel Active
    ACTIVE    The ACTIVE field indicates the channel was selected by arbitration and is executing the prescribed
                    transfers. The eDMA sets it to 1 when channel service begins, and clears it to 0 as the minor loop
                 completes or when any error condition is detected. Except for dynamic scatter/gather or dynamic
                 channel linking, you must not modify the transfer control descriptor when a channel is active.

      30       Channel Done
    DONE     The DONE field indicates the eDMA has completed the major loop. The eDMA engine sets this field as
                  the CITER count reaches zero. If enabled, the eDMA generates an interrupt request corresponding to this
                completed channel. The software clears it, or the hardware clears it when the channel is activated.

                                                 NOTE
                                       This field must be cleared to 0 before writing the MAJORELINK or ESG fields.

      29-4      Reserved
    —

       3        Enable Buffered Writes
    EBW     When buffered writes are enabled, all writes except for the last write sequence of the minor loop are
                  signaled by the eDMA as bufferable.
                     0b - Buffered writes on system bus disabled. Buffered writes on system bus disabled
                     1b - Buffered writes on system bus enabled. Bufferable write signal asserted on all system bus
                          writes except during last write sequence

       2        Enable Error Interrupt
      EEI      The EEI field enables the error interrupt signal for the channel. The DMA error indicator and the error
                     interrupt enable flag must be asserted before an error interrupt request for a given channel is asserted to
                  the interrupt controller.
                     0b - Error signal for corresponding channel does not generate error interrupt

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    777 / 3791

<!-- page 778 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1b - Assertion of error signal for corresponding channel generates error interrupt request

       1        Enable Asynchronous DMA Request
    EARQ     The enable asynchronous DMA request field (EARQ) does not affect DMA operations. When set to 1,
                     this field allows the hardware service request enable field (ERQ) to propagate out of the DMA to the
               power controller. When cleared to 0, this field masks the hardware service request enable field to the
               power controller.
                     0b - Disable asynchronous DMA request for the channel
                     1b - Enable asynchronous DMA request for the channel

       0        Enable DMA Request
    ERQ       Disable a channel's hardware service request at the source before clearing the channel's ERQ field.
               The DMA hardware request input signal and the enable request field (ERQ) must be asserted before a
                  channel's hardware service request is accepted. The state of the eDMA enable request field does not
                     affect a channel service request made explicitly through software or channel linking. The state of the
            ERQ field does not affect the channel's START field.
                     0b - DMA hardware request signal for corresponding channel disabled
                     1b - DMA hardware request signal for corresponding channel enabled


23.6.3.3  Channel Error Status (CH0_ES - CH15_ES)

Offset
For n = 0 to 15:


 Register                   Offset

 CHn_ES               4h + (n × 1000h)


Function
The ES provides information concerning the last recorded channel error. Channel errors can be caused by:
   • An illegal setting in the transfer control descriptor
   • An error termination to a bus master read or write cycle
The ERR field signals the presence of an error for the channel. The eDMA engine signals the occurrence of an error condition by
setting the appropriate field in this register. The outputs of this register are enabled by the contents of the CHn_CSR[EEI] field,
then logically summed across all channels to form an error interrupt request, which may be routed to the interrupt controller. In
addition, this enabled error status is logically OR'd onto the channel done interrupt, CHn_INT[INT], thus forming a done or error
interrupt on a per channel basis.
During the execution of the interrupt service routine associated with any DMA errors, it is software's responsibility to clear the
appropriate bit, negating the error-interrupt request. The normal DMA channel completion indicators (setting the transfer control
descriptor DONE flag and the possible assertion of an interrupt request) are not affected when eDMA detects an error. The
contents of this ERR register field can also be polled because a non-zero value indicates the presence of a channel error,
regardless of the state of the EEI mask.



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    778 / 3791

<!-- page 779 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

The state of any given channel's error indicators is affected by writes to this register. Writing a 1 to the ERR field clears the
channel's error status, and writing a 0 has no effect.
An unspecified error, where only the ERR field is set to 1, indicates that either a transfer was cancelled with an error. The
Management Page Error Status register has full view of the error condition.
See Fault reporting and handling for more details.

Diagram

 Bits       31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R  ERR                                                        Reserved

   W  W1C                                                         0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6       5      4       3      2       1       0

    R                           Reserved                       SAE   SOE   DAE   DOE   NCE   SGE   SBE   DBE

   W                              0

Reset     0      0      0      0       0      0      0      0       0      0       0      0       0      0       0       0


Fields


       Field       Function
      31         Error In Channel
    ERR            0b - An error in this channel has not occurred
                     1b - An error in this channel has occurred

      30-8      Reserved
    —

       7        Source Address Error
     SAE     TCDn_SADDR is inconsistent with TCDn_ATTR[SSIZE].
                     0b - No source address configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_SADDR field

       6        Source Offset Error
    SOE     TCDn_SOFF is inconsistent with TCDn_ATTR[SSIZE].
                     0b - No source offset configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_SOFF field

       5         Destination Address Error
    DAE     TCDn_DADDR is inconsistent with TCDn_ATTR[DSIZE].
                     0b - No destination address configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_DADDR field

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    779 / 3791

<!-- page 780 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function

       4         Destination Offset Error
    DOE     TCDn_DOFF is inconsistent with TCDn_ATTR[DSIZE].
                     0b - No destination offset configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_DOFF field

       3       NBYTES/CITER Configuration Error
    NCE       This error indicates that one of the following has occurred:
                           • TCDn_NBYTES is not a multiple of TCDn_ATTR[SSIZE] and TCDn_ATTR[DSIZE]
                           • TCDn_CITER[CITER] is equal to zero
                           • TCDn_CITER[ELINK] is not equal to TCDn_BITER[ELINK]
                     0b - No NBYTES/CITER configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_NBYTES or
                   TCDn_CITER fields

       2         Scatter/Gather Configuration Error
    SGE     When this field is 1, it indicates that TCDn_DLAST_SGA is not on a 32-byte boundary. This field is
                checked at the beginning of a scatter/gather operation after major loop completion if TCDn_CSR[ESG]is
                 enabled.
                     0b - No scatter/gather configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_DLAST_SGA field

       1        Source Bus Error
     SBE            0b - No source bus error
                     1b - Last recorded error was bus error on source read

       0         Destination Bus Error
    DBE            0b - No destination bus error
                     1b - Last recorded error was bus error on destination write


23.6.3.4  Channel Interrupt Status (CH0_INT - CH15_INT)

Offset
For n = 0 to 15:


 Register                   Offset

 CHn_INT              8h + (n × 1000h)


Function
The INT field signals the presence of an interrupt request for the channel. Depending on the appropriate bit setting in the transfer
control descriptors, the eDMA engine generates an interrupt on data transfer completion or an error condition.


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    780 / 3791

<!-- page 781 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

The outputs of this register are directly routed to the interrupt controller. During the interrupt service routine associated with any
given channel, it is the software's responsibility to clear the appropriate bit, negating the interrupt request. On writes to INT, a 1
clears the channel's interrupt request. A zero has no effect on the channel's current interrupt status.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                        0                                                           INT

   W                                                                                     W1C

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-1      Reserved
    —

       0          Interrupt Request
      INT            0b - Interrupt request for corresponding channel cleared
                     1b - Interrupt request for corresponding channel active


23.6.3.5  Channel System Bus (CH0_SBR - CH15_SBR)

Offset
For n = 0 to 15:


 Register                   Offset

 CHn_SBR            Ch + (n × 1000h)


Function
The Channel System Bus register places identification and attribute information on the system bus interface for the eDMA.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    781 / 3791

<!-- page 782 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                      Reserved
                                                                                                                 EMI
   W                                                        0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits       15      14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                 0                                             MID
        PAL   SEC
   W

Reset     0       0      0      0       0      0      0      0       0      0      0      0       0      1      1      0


Fields


       Field       Function
     31-17      Reserved
    —

      16       Enable Master ID Replication
     EMI      The eDMA master ID replication field allows the eDMA to use the same protection level and system
                bus ID of the master programming the eDMA's TCD. When enabled, the eDMA uses the master ID and
                   protection level stored in the CHn_SBR registers, instead of the eDMA's default values. When a master
                      (for example a core) programs a TCD, its master ID is captured when the TCDn_CSR control attributes are
                     written. A scatter/gather operation does not affect the CHn_SBR registers. You can write the EMI only if
             MP_CSR[GMRC] = 1, which means Global Master ID Replication Control is enabled; otherwise, the EMI is
                  forced to zero.

                                                 NOTE
                                                         If master ID replication is disabled, the nonsecure, user protection level for DMA transfers
                                            is used.

                     0b - Master ID replication is disabled
                     1b - Master ID replication is enabled

      15         Privileged Access Level
     PAL       This field controls DMA's protection level on the system bus when the channel is active.

                                                 NOTE
                           The value written into this register cannot exceed the security and privilege level of the
                                core or other master writing the channel's system bus register; CHn_SBR. The order of
                              precedence is SecurePriv>SecureUser>NonsecurePriv>NonsecureUser

                     0b - User protection level for DMA transfers
                     1b - Privileged protection level for DMA transfers

      14        Security Level
    SEC      DMA's security level on the system bus when the channel is active.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    782 / 3791

<!-- page 783 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                                                 NOTE
                           The value written into this register cannot exceed the security and privilege level of the
                                core or other master writing the channel's system bus register; CHn_SBR. The order of
                              precedence is SecurePriv>SecureUser>NonsecurePriv>NonsecureUser

                     0b - Nonsecure protection level for DMA transfers
                     1b - Secure protection level for DMA transfers

      13-5      Reserved
    —

       4-0       Master ID
     MID       This field controls the DMA's master ID on the system bus when the channel is active.

                                                 NOTE
                           The ID captured in this register reflects the master ID of the core or other master writing the
                                  channel's security attributes, TCDn_SBR[SEC].


23.6.3.6  Channel Priority (CH0_PRI - CH15_PRI)

Offset
For n = 0 to 15:


 Register                   Offset

 CHn_PRI              10h + (n × 1000h)


Function
The contents of these registers define unique priorities associated with each channel within the same channel group. Channel
grouping is programmed via Channel Arbitration Group (CH0_GRPRI - CH15_GRPRI).
The channel priorities within a group are evaluated by numeric value; for example, 0 is the lowest priority, 1 is the next higher
priority, then 2, 3, and so on. Software must program the channel priorities with unique values; otherwise, channel numbers with
the same, non-zero value, will be selected based on channel number with the higher channel number having higher priority.
If more than one channel in a group has an arbitration priority level value of zero, then the arbitration mode field MP_CSR[ERCA]
is used to determine the arbitration scheme for all channels with APL=0 within a group.
When you enable round-robin channel arbitration (MP_CSR[ERCA] = 1), all channels with APL=0 within a group will use a
round-robin arbitration scheme, which rotates among these channels requesting service without regard to priority. Round-robin
provides a fairness mechanism within an arbitration group.
When you enable fixed-priority channel arbitration (MP_CSR[ERCA] = 0), eDMA selects channels with APL=0 based on channel
number, with the higher channel number having higher priority.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    783 / 3791

<!-- page 784 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits       31      30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                                  Reserved
       ECP   DPA
   W                                                                   0

Reset     0       0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                               Reserved
                                                                                                    APL
   W                                                 0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31       Enable Channel Preemption
    ECP            0b - Channel cannot be suspended by a higher-priority channel's service request
                     1b - Channel can be temporarily suspended by a higher-priority channel's service request

      30        Disable Preempt Ability
    DPA            0b - Channel can suspend a lower-priority channel
                     1b - Channel cannot suspend any other channel, regardless of channel priority

      29-3      Reserved
    —

       2-0         Arbitration Priority Level
     APL      Channel priority level for arbitration within the assigned arbitration group.


23.6.3.7  Channel Multiplexor Configuration (CH0_MUX - CH15_MUX)

Offset
For n = 0 to 15:


 Register                   Offset

 CHn_MUX             14h + (n × 1000h)


Function
Each of the DMA channels can be independently associated with various peripherals in the system. The Channel Multiplexor
Configuration register selects the peripheral assigned to each channel. Service requests from the peripheral should be disabled
when configuring a channel to a peripheral source.
Each channel must have a unique value when selecting a peripheral slot in the channel mux configuration. The only value that
may overlap is source 0. If there is an attempt to write a mux configuration value that is already consumed by any channel, a mux
configuration of 0 (SRC = 0) will be written.


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    784 / 3791

<!-- page 785 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

All channels will default to source 0. When a particular peripheral is needed, the channel’s mux configuration is set to that source
number. When the peripheral is no longer needed, the mux configuration for that channel should be written to 0, thus releasing
the resource.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                          Reserved

   W                                                            0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                Reserved
                                                                             SRC
   W                                  0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-7      Reserved
    —

       6-0        Service Request Source
    SRC      Hardware service request source for the channel.

                                                 NOTE
                                   With the exception of 0, attempts to write a value already in use will be forced to 0.


23.6.3.8 TCD Source Address (TCD0_SADDR - TCD15_SADDR)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_SADDR          20h + (n × 1000h)


Function
This register contains the address for the read transactions.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    785 / 3791

<!-- page 786 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                  SADDR
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                  SADDR
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      31-0      Source Address
   SADDR    Memory address pointing to the source data.


23.6.3.9 TCD Signed Source Address Offset (TCD0_SOFF - TCD15_SOFF)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_SOFF           24h + (n × 1000h)


Function
This register contains the sign-extended value added to Source Address register after each read transaction.

Diagram

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                     SOFF
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      15-0      Source Address Signed Offset
    SOFF      Sign-extended offset applied to the current source address to form the next-state value as each source
                 read is completed.




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    786 / 3791

<!-- page 787 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

23.6.3.10 TCD Transfer Attributes (TCD0_ATTR - TCD15_ATTR)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_ATTR           26h + (n × 1000h)


Function
This register contains size and option modulo addressing information for source and destination addresses.

Diagram

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                 SMOD                       SSIZE                  DMOD                       DSIZE
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

     15-11      Source Address Modulo
   SMOD      This field defines a specific address range, which is the value after the SADDR + SOFF calculation is
                 performed on the original register value. Setting this field makes it easy to implement a circular data queue.
                 For data queues requiring power-of-2-sized bytes, the queue must start at a 0-modulo-size address and
                  the SMOD field must be set to the appropriate value for the queue, freezing the required number of upper
                address bits.
               The value programmed into this field specifies the number of lower address bits that are allowed to change.
                 For a circular queue application, you typically set TCDn_SOFF[SOFF] to the transfer size to implement
                  post-increment addressing, with the SMOD function constraining the addresses to a 0-modulo-size range.
                     0_0000b - Source address modulo feature disabled
                     0_0001b - Source address modulo feature enabled for any non-zero value [1-31]

      10-8      Source Data Transfer Size
    SSIZE           000b - 8-bit
                     001b - 16-bit
                     010b - 32-bit
                     011b - 64-bit
                     100b - 16-byte
                     101b - 32-byte
                     110b - Reserved

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    787 / 3791

<!-- page 788 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     111b - Reserved

       7-3        Destination Address Modulo
   DMOD     See the SMOD definition.

       2-0        Destination Data Transfer Size
    DSIZE     See the SSIZE definition.


23.6.3.11 TCD Transfer Size Without Minor Loop Offsets (TCD0_NBYTES_MLOFFNO -
        TCD15_NBYTES_MLOFFNO)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_NBYTES_MLOFF  28h + (n × 1000h)
 NO


Function
The TCDn_NBYTES field defines the number of bytes to transfer per service request.
Minor loop offsets are address offset values added to the final source address (TCDn_SADDR), or destination
address (TCDn_DADDR), upon minor loop completion. Minor loop completion is when the channel has finished the
service request and has transferred NBYTES. When minor loop offsets are enabled, the minor loop offset value
(TCDn_NBYTES_MLOFFYES[MLOFF]) is added to the final source address (TCDn_SADDR), to the final destination address
(TCDn_DADDR), or to both, prior to the addresses being written back to the TCD. If the major loop is complete, the minor loop
offset is ignored and the major loop address offsets (TCDn_SLAST_SDA and TCDn_DLAST_SGA) are used to compute the next
TCDn_SADDR and TCDn_DADDR values.
When minor loop mapping is enabled (SMLOE or DMLOE is 1), TCDn_NBYTES_MLOFFNO/TCDn_NBYTES_MLOFFYES is
redefined. A portion of TCDn_NBYTES_MLOFFNO/TCDn_NBYTES_MLOFFYES is used to specify multiple fields:
   • A source enable bit (SMLOE) to specify the minor loop offset must be applied to the source address (TCDn_SADDR) upon
    minor loop completion
   • A destination enable bit (DMLOE) to specify the minor loop offset must be applied to the destination address (TCDn_DADDR)
   upon minor loop completion
   • The sign extended minor loop offset value (MLOFF)
The same offset value (MLOFF) is used for both source and destination minor loop offsets. When either minor loop offset is
enabled (SMLOE set or DMLOE set), the NBYTES field is reduced to 10 bits. If both minor loop offsets are disabled (SMLOE
cleared and DMLOE cleared), the NBYTES field is a 30-bit vector.
One of two register profiles (this register or TCDn_NBYTES_MLOFFYES), defines the number of bytes to transfer per request.
Which register to use depends on whether source or destination minor loop mapping is enabled.
TCDn_NBYTES_MLOFFNO/TCDn_NBYTES_MLOFFYES is defined as follows:
   •  If SMLOE = 0 and DMLOE = 0, then see the TCDn_NBYTES_MLOFFNO register description.



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    788 / 3791

<!-- page 789 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

   •  If either SMLOE or DMLOE is 1, then see the TCDn_NBYTES_MLOFFYES register description.

Diagram

 Bits       31      30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R  SMLO  DMLO
                                                           NBYTES
   W   E     E

Reset     u       u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                    NBYTES
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      31       Source Minor Loop Offset Enable
   SMLOE      Selects whether the minor loop offset is applied to the source address upon minor loop completion.
                     0b - Minor loop offset not applied to SADDR
                     1b - Minor loop offset applied to SADDR

      30        Destination Minor Loop Offset Enable
   DMLOE     Selects whether the minor loop offset is applied to the destination address upon minor loop completion.
                     0b - Minor loop offset not applied to DADDR
                     1b - Minor loop offset applied to DADDR

      29-0      Number of Bytes To Transfer Per Service Request
   NBYTES    Number of bytes to be transferred for each service request of the channel.
             When a channel activates, the module loads the appropriate TCD contents into the eDMA engine and
                 performs the appropriate reads and writes until the byte transfer count has been reached. This process is
                  normally an indivisible operation and cannot be halted. It can, however, be stalled by using the bandwidth
                    control field, or via preemption.
                    After the byte count is exhausted, the SADDR and DADDR values are written back into the TCD memory,
               and the major loop iteration count (CITER) is decremented by one and written back to the TCD memory. If
                  the major iteration count is complete, additional processing is performed.


23.6.3.12 TCD Transfer Size with Minor Loop Offsets (TCD0_NBYTES_MLOFFYES -
        TCD15_NBYTES_MLOFFYES)

Offset
For n = 0 to 15:





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    789 / 3791

<!-- page 790 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


 Register                   Offset

 TCDn_NBYTES_MLOFF  28h + (n × 1000h)
 YES


Function
The TCDn_NBYTES field defines the number of bytes to transfer per service request.
Minor loop offset is an address offset value added to the final source address (TCDn_SADDR) or destination address
(TCDn_DADDR) upon minor loop completion. Minor loop completion occurs when the channel has finished the service request
and has transferred NBYTES. Minor loop offsets are enabled by setting either the source enable bit (SMLOE) or the destination
enable bit (DMLOE).
The source enable bit (SMLOE) specifies the minor loop offset value (MLOFF) that is to be applied to the source address
(TCDn_SADDR) upon minor loop completion. The destination enable bit (DMLOE) specifies the minor loop offset (MLOFF) that
is to be applied to the destination address (TCDn_DADDR) upon minor loop completion.
If the major loop is complete, the minor loop offsets are ignored and the major loop address offsets (TCDn_SLAST_SDA and
TCDn_DLAST_SGA) are used to compute the next TCDn_SADDR and TCDn_DADDR values.
When you enable the minor loop offset overlay (either SMLOE or DMLOE is 1), eDMA redefines TCDn_NBYTES_MLOFFNO/
TCDn_NBYTES_MLOFFYES. A portion of TCDn_NBYTES_MLOFFNO/TCDn_NBYTES_MLOFFYES specifies the sign-
extended minor loop offset value (MLOFF). The same offset value (MLOFF) applies to both source and destination minor
loop offsets. When the minor loop offset is enabled, you must align it to the transfer size of the source or destination it is associated
with. When either minor loop offset is enabled (SMLOE set or DMLOE set), the NBYTES field is reduced to 10 bits. If both minor
loop offsets are disabled (SMLOE cleared and DMLOE cleared), the NBYTES field is a 30-bit vector.
One of two register profiles (this register or TCDn_NBYTES_MLOFFNO) defines the number of bytes to transfer per request.
Which register to use depends on whether source or destination minor loop mapping is enabled.
TCDn_NBYTES_MLOFFYES is defined as follows:
   •  If either minor loop offset is enabled (SMLOE or DMLOE = 1), then see the TCDn_NBYTES_MLOFFYES register description.
   •  If SMLOE and DMLOE are both 0, then see the TCDn_NBYTES_MLOFFNO register description.

Diagram

 Bits       31      30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R  SMLO  DMLO
                                                          MLOFF
   W   E     E

Reset     u       u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                      MLOFF                                            NBYTES
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    790 / 3791

<!-- page 791 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Fields


       Field       Function

      31       Source Minor Loop Offset Enable
   SMLOE      Selects whether the minor loop offset is applied to the source address upon minor loop completion.
                     0b - Minor loop offset not applied to SADDR
                     1b - Minor loop offset applied to SADDR

      30        Destination Minor Loop Offset Enable
   DMLOE     Selects whether the minor loop offset is applied to the destination address upon minor loop completion.
                     0b - Minor loop offset not applied to DADDR
                     1b - Minor loop offset applied to DADDR

     29-10      Minor Loop Offset
   MLOFF          If SMLOE or DMLOE is 1, this field represents a sign-extended offset applied to the source or destination
                address to form the next-state value after the minor loop completes.

       9-0      Number of Bytes To Transfer Per Service Request
   NBYTES    The number of bytes to be transferred in each service request of the channel.
              As a channel activates, the module loads the appropriate TCD contents into the eDMA engine and performs
                  the appropriate reads and writes until the minor byte transfer count has been reached. This is an indivisible
                  operation and cannot be halted. It can, however, be stalled by using the bandwidth control field, or
                    via preemption.
                    After the minor count is exhausted, the SADDR and DADDR values are written back into the TCD memory,
               and the major iteration count is decremented and restored to the TCD memory. If the major iteration count
                       is complete, additional processing is performed.


23.6.3.13 TCD Last Source Address Adjustment / Store DADDR Address (TCD0_SLAST_SDA -
        TCD15_SLAST_SDA)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_SLAST_SDA     2Ch + (n × 1000h)


Function
This register contains the value added to the source address when the major loop is complete. When the store destination
address option is enabled, this field provides a pointer to memory for storing the final destination address.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    791 / 3791

<!-- page 792 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                      SLAST_SDA
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      SLAST_SDA
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      31-0       Last Source Address Adjustment / Store DADDR Address
  SLAST_SDA   Source last address adjustment or the system memory address for destination address (DADDR) storage.
                              If (TCDn_CSR[ESDA] = 0), then:
                           • Adjustment value is added to the source address at the completion of the major iteration count.
                      This value can be used to restore the source address to the initial value or adjust the address to
                      reference the next data structure.
                           • This field uses two's complement notation for the final source address adjustment.
                 Otherwise:
                           • This address points to the 32-bit-aligned memory location where the destination address (DADDR)
                            is to be stored in system memory. By saving the final destination address in system memory via
                      the ESDA feature, you are able to compute the size of a variable destination data buffer by simply
                       subtracting the beginning DADDR from the final, saved DADDR. This feature is used together with
                      the scatter/gather operation to prevent the loss of the final DADDR, which is overwritten during the
                       scatter/gather operation.
                  The "Store Destination Address" (SDA) value must be a 32-bit-aligned location because the eDMA
                       forces the lower two address bits of the SLAST_SDA field to zero when ESDA is enabled. The module
                     performs this write operation when the major loop is done; that is, when the major iteration count
                    (CITER) decrements to zero.


23.6.3.14 TCD Destination Address (TCD0_DADDR - TCD15_DADDR)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_DADDR          30h + (n × 1000h)


Function
This register contains the address for the write transactions.


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    792 / 3791

<!-- page 793 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                 DADDR
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                 DADDR
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      31-0       Destination Address
   DADDR    Memory address pointing to the destination data.


23.6.3.15 TCD Signed Destination Address Offset (TCD0_DOFF - TCD15_DOFF)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_DOFF           34h + (n × 1000h)


Function
This register contains the sign-extended value added to Destination Address register after each write transaction.

Diagram

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                    DOFF
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      15-0       Destination Address Signed Offset
    DOFF      Sign-extended offset that is applied to the current destination address to form the next-state value as
               each destination write is completed.




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    793 / 3791

<!-- page 794 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

23.6.3.16 TCD Current Major Loop Count (Minor Loop Channel Linking Disabled) (TCD0_CITER_ELINKNO -
        TCD15_CITER_ELINKNO)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_CITER_ELINKNO  36h + (n × 1000h)


Function
If TCDn_CITER[ELINK] is 0, the TCDn_CITER register is defined as follows.

Diagram

 Bits       15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
        ELINK                                                  CITER
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      15       Enable Link
    ELINK     As the channel completes the minor loop, this flag enables linking to another channel as defined by the
                   relevant LINKCH field. The link target channel initiates a channel service request via an internal mechanism
                    that sets the TCDn_CSR[START] bit of the specified channel to 1.
                              If channel linking is disabled, the CITER value is extended to 15 bits in place of a link channel number. If the
                 major loop is exhausted, this link mechanism is suppressed in favor of MAJORELINK channel linking.

                                                 NOTE
                                 This field must be equal to the BITER[ELINK] field; otherwise, a configuration error
                                            is reported.

                     0b - Channel-to-channel linking disabled
                     1b - Channel-to-channel linking enabled

      14-0       Current Major Iteration Count
    CITER      This 9-bit (ELINK = 1) or 15-bit (ELINK = 0) count represents the current major loop count for the channel.
                              It is decremented each time the channel finishes a service request and is written back to TCD memory.
                    After the major iteration count is exhausted, the channel performs a number of operations — for example,
                       final source and destination address calculations — and optionally generates an interrupt to signal channel
                 completion before reloading the CITER field from the Beginning Iteration Count (BITER) field.

                                  Tablecontinuesonthenextpage...





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    794 / 3791

<!-- page 795 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                                                 NOTE
                        When the CITER field is initially loaded by software, it must be set to the same value as that
                                 contained in the BITER field.

                                                 NOTE
                                                         If the channel is configured to execute a single service request, the initial values of BITER
                            and CITER should be 0x0001.


23.6.3.17 TCD Current Major Loop Count (Minor Loop Channel Linking Enabled) (TCD0_CITER_ELINKYES -
         TCD15_CITER_ELINKYES)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_CITER_ELINKYE  36h + (n × 1000h)
 S


Function
If TCDn_CITER[ELINK] is 1, the TCDn_CITER register is defined as follows.

Diagram

 Bits       15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R            Reserved
        ELINK                         LINKCH                                       CITER
   W               0

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      15       Enable Link
    ELINK     As the channel completes the minor loop, this flag enables linking to another channel as defined by the
                   relevant LINKCH field. When enabled, an internal mechanism sets the TCDn_CSR[START] field of the
                   specified channel (LINKCH) upon minor loop completion.
                              If channel linking is disabled, the CITER value is extended to 15 bits in place of a link channel number. If the
                 major loop is exhausted, this link mechanism is suppressed in favor of MAJORELINK channel linking.

                                  Tablecontinuesonthenextpage...





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    795 / 3791

<!-- page 796 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                                                 NOTE
                                 This field must be equal to the BITER[ELINK] field; otherwise, a configuration error
                                            is reported.

                     0b - Channel-to-channel linking disabled
                     1b - Channel-to-channel linking enabled

     14-13      Reserved
    —

      12-9       Minor Loop Link Channel Number
    LINKCH         If channel-to-channel linking is enabled (ELINK = 1), then after the minor loop is exhausted the eDMA
                 engine initiates a channel service request to the channel defined by this field by writing that channel’s
              TCDn_CSR[START] field to 1.

       8-0        Current Major Iteration Count
    CITER      This 9-bit (ELINK = 1) or 15-bit (ELINK = 0) count represents the current major loop count for the channel.
                              It is decremented each time the channel finishes a service request and is written back to the TCD memory.
                    After the major iteration count is exhausted, the channel performs a number of operations — for example,
                       final source and destination address calculations — and optionally generates an interrupt to signal channel
                 completion before reloading the CITER field from the Beginning Iteration Count (BITER) field.

                                                 NOTE
                        When the CITER field is initially loaded by software, it must be set to the same value as that
                                 contained in the BITER field.

                                                 NOTE
                                                         If the channel is configured to execute a single service request, the initial values of BITER
                            and CITER should be 0x0001.


23.6.3.18 TCD Last Destination Address Adjustment / Scatter Gather Address (TCD0_DLAST_SGA -
        TCD15_DLAST_SGA)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_DLAST_SGA     38h + (n × 1000h)


Function
This register contains the value added to the destination address when the major loop is complete. When the Scatter/Gather
option is enabled, this field provides a pointer to memory for fetching a transfer control descriptor to reprogram the channel.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    796 / 3791

<!-- page 797 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                    DLAST_SGA
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                    DLAST_SGA
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      31-0       Last Destination Address Adjustment / Scatter Gather Address
  DLAST_SGA   Adjustment of the last destination address or the memory address for the next transfer control descriptor to
               be loaded into this channel (scatter/gather).
                              If (TCDn_CSR[ESG] = 0) then:
                           • Adjustment value is added to the destination address at the completion of the major iteration count.
                      This value can apply to restore the destination address to the initial value or adjust the address to
                      reference the next data structure.
                           • This field uses two's complement notation for the final destination address adjustment.
                 Otherwise:
                           • This address points to the beginning of a 0-modulo 32-byte region containing the next transfer
                        control descriptor to be loaded into this channel. This channel reload is performed as the major
                          iteration count completes. The scatter/gather address must be 0-modulo 32-byte, or else a
                       configuration error is reported.


23.6.3.19 TCD Control and Status (TCD0_CSR - TCD15_CSR)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_CSR           3Ch + (n × 1000h)


Function
This register is used to enable optional features.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    797 / 3791

<!-- page 798 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits      15     14     13     12      11     10      9      8       7      6       5      4       3      2       1       0

    R                                                                 MAJO                INTHA  INTMA  STAR
         BWC                        MAJORLINKCH        ESDA  EEOP        ESG  DREQ
                                                                                                  REL...                  LF    JOR     T   W                      0

Reset     u      u      u      u       u      u      u      u       u      u       u      u       u      u       u       0


Fields


       Field       Function

     15-14      Bandwidth Control
    BWC       Throttles the amount of bus bandwidth consumed by the eDMA. Generally, as the eDMA processes the
                 minor loop, it continuously generates read/write sequences until the minor count is exhausted. This field
                  forces eDMA to stall after the completion of each read/write access, to control the bus request bandwidth
               seen by the system bus interconnect.

                                                 NOTE
                                                         If the source and destination sizes are equal, this field is ignored between the first and
                             second transfers and after the last write of each minor loop. This behavior is a side effect of
                                 reducing start-up latency.

                     00b - No eDMA engine stalls
                     01b - Reserved
                     10b - eDMA engine stalls for 4 cycles after each R/W
                     11b - eDMA engine stalls for 8 cycles after each R/W

     13-12      Reserved
    —

      11-8       Major Loop Link Channel Number
 MAJORLINKCH   If (MAJORELINK = 0) then:
                           • No channel-to-channel linking, or chaining, is performed after the major loop counter is exhausted.
                 Otherwise:
                           • After the major loop counter is exhausted, the eDMA engine initiates a channel service request at
                      the channel defined by this field by setting that channel’s TCDn_CSR[START] field to 1.

       7        Enable Store Destination Address
    ESDA     As the channel completes the major loop by either the current iteration counter (CITER) decrementing to 0,
                   or by receiving an enabled end-of-packet signal, this field enables writing the destination address (DADDR)
                    to the address stored in the SLAST_SDA field. The value written to system memory is the last DADDR value
                     prior to the DLAST_SGA offset being applied, or overwritten by an enabled scatter/gather operation. When
                  the ESDA bit is 1, SLAST_SDA contains the write pointer instead of the final source address offset. Because
                     this is a pointer and not a final offset, a last source address offset of zero is applied to SADDR instead of
                  the SLAST_SGA value.
                     0b - Ability to store destination address to system memory disabled

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    798 / 3791

<!-- page 799 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1b - Ability to store destination address to system memory enabled

       6        Enable End-Of-Packet Processing
    EEOP     When enabled by the EEOP field, an end-of-packet hardware input signal directs eDMA to discontinue
                  executing the active channel, and to treat the shutdown as the major-loop-completed event. If the EEOP
                       field is 1, the end-of-packet signal from supported peripherals is accepted. If the EEOP field is 0, the
                  end-of-packet input is ignored. With an end-of-packet retirement, the current TCD destination address (or
               ESDA-saved destination address), minus the software-saved initial address (DADDR), reflects the total
               amount of data transferred.
                     0b - End-of-packet operation disabled
                     1b - End-of-packet hardware input signal enabled

       5        Enable Link When Major Loop Complete
 MAJORELINK  As the channel completes the major loop, this flag enables linking to another channel defined by
             MAJORLINKCH. The link target channel initiates a channel service request via an internal mechanism that
                  sets the TCDn_CSR[START] field of the specified channel.

                                                 NOTE
                           To support the dynamic linking coherency model, this field is forced to 0 if written when
                         TCDn_CSR[DONE] is 1.

                     0b - Channel-to-channel linking disabled
                     1b - Channel-to-channel linking enabled

       4        Enable Scatter/Gather Processing
    ESG      As the channel completes the major loop, this flag enables scatter/gather processing in the current channel.
                              If enabled, the eDMA engine uses TCDn_DLAST_SGA as a memory pointer to a 0-modulo 32-bit address
                  containing a 32-byte data structure, which is loaded as the transfer control descriptor into local memory.

                                                 NOTE
                           To support the dynamic scatter/gather coherency model, this field is forced to 0 if written
                          when TCDn_CSR[DONE] is 1.

                     0b - Current channel’s TCD is normal format
                     1b - Current channel’s TCD specifies scatter/gather format.

       3         Disable Request
    DREQ          If this flag is 1, the eDMA hardware automatically clears the corresponding ERQ bit when the current
                 major iteration count reaches 0.
                     0b - No operation. Channel’s ERQ field not affected
                     1b - Clear the ERQ field to 0 upon major loop completion, thus disabling hardware service
                         requests. Channel’s ERQ field cleared to 0 when major loop complete

       2        Enable Interrupt If Major Counter Half-complete

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    799 / 3791

<!-- page 800 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function

   INTHALF        If this flag is 1, the channel generates an interrupt request by setting the appropriate field in the INT
                    register to 1 when the current major iteration count reaches the halfway point. Specifically, the comparison
                 performed by the eDMA engine is (CITER = (BITER/2)). This halfway point interrupt request is provided to
                  support double-buffered, also known as ping-pong, schemes, or other types of data movement where the
                 processor needs an early indication of the transfer’s progress.

                                                 NOTE
                                                                                If BITER = 1, do not use INTHALF; use INTMAJOR instead.

                     0b - Halfway point interrupt disabled
                     1b - Halfway point interrupt enabled

       1        Enable Interrupt If Major count complete
  INTMAJOR      If this flag is 1, the channel generates an interrupt request by setting the appropriate field in the INT
                    register to 1 when the current major iteration count (CITER) reaches 0.
                     0b - End-of-major loop interrupt disabled
                     1b - End-of-major loop interrupt enabled

       0        Channel Start
    START          If this flag is 1, the channel is requesting service. The eDMA hardware automatically clears this flag to 0
                     after the channel begins execution.
                     0b - Channel not explicitly started
                     1b - Channel explicitly started via a software-initiated service request


23.6.3.20 TCD Beginning Major Loop Count (Minor Loop Channel Linking Disabled) (TCD0_BITER_ELINKNO -
        TCD15_BITER_ELINKNO)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_BITER_ELINKNO  3Eh + (n × 1000h)


Function
If the TCDn_BITER[ELINK] field is 0, the TCDn_BITER register is defined as follows.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    800 / 3791

<!-- page 801 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits       15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
        ELINK                                                   BITER
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      15       Enables Link
    ELINK     As the channel completes the minor loop, this flag enables linking to another channel as defined by
                BITER[LINKCH]. The link target channel initiates a channel service request via an internal mechanism that
                  sets the TCDn_CSR[START] field of the specified channel. If channel linking is disabled, the BITER value
                 extends to 15 bits in place of a link channel number. If the major loop is exhausted, this link mechanism is
                suppressed in favor of the MAJORELINK channel linking.

                                                 NOTE
                        When the software loads the TCD, this field must be set equal to the corresponding CITER
                                             field; otherwise, a configuration error is reported. As the major iteration count is exhausted,
                       eDMA reloads the contents of this field into the CITER field.

                     0b - Channel-to-channel linking disabled
                     1b - Channel-to-channel linking enabled

      14-0        Starting Major Iteration Count
    BITER     As the transfer control descriptor is first loaded by software, this 9-bit (ELINK = 1) or 15-bit (ELINK = 0) field
               must be set equal to the value in the CITER field. As the major iteration count is exhausted, eDMA reloads
                  the contents of this field into the CITER field. If the channel is configured to execute a single service request,
                  the initial values of BITER and CITER must be 0x0001.


23.6.3.21 TCD Beginning Major Loop Count (Minor Loop Channel Linking Enabled) (TCD0_BITER_ELINKYES -
         TCD15_BITER_ELINKYES)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_BITER_ELINKYE  3Eh + (n × 1000h)
 S


Function
If the TCDn_BITER[ELINK] field is set, the TCDn_BITER register is defined as follows.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    801 / 3791

<!-- page 802 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits       15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R            Reserved
        ELINK                         LINKCH                                        BITER
   W               0

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      15       Enable Link
    ELINK     As the channel completes the minor loop, this flag enables linking to another channel as defined by
                BITER[LINKCH]. The link target channel initiates a channel service request via an internal mechanism that
                  sets the TCDn_CSR[START] field of the specified channel. If channel linking disables, the BITER value
                 extends to 15 bits in place of a link channel number. If the major loop is exhausted, this link mechanism is
                suppressed in favor of the MAJORELINK channel linking.

                                                 NOTE
                        When the software loads the TCD, this field must be set equal to the corresponding CITER
                                             field; otherwise, a configuration error is reported. As the major iteration count is exhausted,
                       eDMA reloads the contents of this field into the CITER field.

                     0b - Channel-to-channel linking disabled
                     1b - Channel-to-channel linking enabled

     14-13      Reserved
    —

      12-9       Link Channel Number
    LINKCH         If channel-to-channel linking is enabled (ELINK = 1), then after the minor loop is exhausted, the eDMA
                 engine initiates a channel service request at the channel defined by this field by setting that channel’s
              TCDn_CSR[START] field.

                                                 NOTE
                        When the software loads the TCD, this field must be set equal to the corresponding CITER
                                             field; otherwise, a configuration error is reported. As the major iteration count is exhausted,
                       eDMA reloads the contents of this field into the CITER field.

       8-0        Starting Major Iteration Count
    BITER     As the transfer control descriptor is first loaded by software, this 9-bit (ELINK = 1) or 15-bit (ELINK = 0) field
               must be set equal to the value in the CITER field. As the major iteration count is exhausted, eDMA reloads
                  the contents of this field into the CITER field. If the channel is configured to execute a single service request,
                  the initial values of BITER and CITER must be 0x0001.

23.6.4 DMA TCD register descriptions

23.6.4.1 TCD memory map
eDMA_1_TCD base address: 400A_1000h


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    802 / 3791

<!-- page 803 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


     Offset     Register                                                        Width    Access   Reset value
                                                                                                     (In bits)

  0h - F000h   Channel Control and Status (CH0_CSR - CH15_CSR)                32    RW    0000_0000h

  4h - F004h   Channel Error Status (CH0_ES - CH15_ES)                        32    RW    0000_0000h

  8h - F008h   Channel Interrupt Status (CH0_INT - CH15_INT)                    32    RW    0000_0000h

 Ch - F00Ch  Channel System Bus (CH0_SBR - CH15_SBR)                      32    RW    0000_0007h

  10h - F010h  Channel Priority (CH0_PRI - CH15_PRI)                           32    RW    0000_0000h

  14h - F014h  Channel Multiplexor Configuration (CH0_MUX - CH15_MUX)           32    RW    0000_0000h

  20h - F020h  TCD Source Address (TCD0_SADDR - TCD15_SADDR)              32    RW    See section

  24h - F024h  TCD Signed Source Address Offset (TCD0_SOFF - TCD15_SOFF)      16    RW    See section

  26h - F026h  TCD Transfer Attributes (TCD0_ATTR - TCD15_ATTR)               16    RW    See section

  28h - F028h  TCD Transfer Size Without Minor Loop Offsets                      32    RW    See section
            (TCD0_NBYTES_MLOFFNO - TCD15_NBYTES_MLOFFNO)

  28h - F028h  TCD Transfer Size with Minor Loop Offsets                         32    RW    See section
            (TCD0_NBYTES_MLOFFYES - TCD15_NBYTES_MLOFFYES)

 2Ch - F02Ch TCD Last Source Address Adjustment / Store DADDR Address         32    RW    See section
            (TCD0_SLAST_SDA - TCD15_SLAST_SDA)

  30h - F030h  TCD Destination Address (TCD0_DADDR - TCD15_DADDR)           32    RW    See section

  34h - F034h  TCD Signed Destination Address Offset (TCD0_DOFF -               16    RW    See section
            TCD15_DOFF)

  36h - F036h  TCD Current Major Loop Count (Minor Loop Channel Linking           16    RW    See section
                Disabled) (TCD0_CITER_ELINKNO - TCD15_CITER_ELINKNO)

  36h - F036h  TCD Current Major Loop Count (Minor Loop Channel Linking           16    RW    See section
               Enabled) (TCD0_CITER_ELINKYES - TCD15_CITER_ELINKYES)

  38h - F038h  TCD Last Destination Address Adjustment / Scatter Gather Address     32    RW    See section
            (TCD0_DLAST_SGA - TCD15_DLAST_SGA)

 3Ch - F03Ch TCD Control and Status (TCD0_CSR - TCD15_CSR)                 16    RW    See section

 3Eh - F03Eh  TCD Beginning Major Loop Count (Minor Loop Channel Linking         16    RW    See section
                Disabled) (TCD0_BITER_ELINKNO - TCD15_BITER_ELINKNO)

 3Eh - F03Eh  TCD Beginning Major Loop Count (Minor Loop Channel Linking         16    RW    See section
               Enabled) (TCD0_BITER_ELINKYES - TCD15_BITER_ELINKYES)

23.6.4.2  Channel Control and Status (CH0_CSR - CH15_CSR)

Offset
For n = 0 to 15:


 Register                   Offset

 CHn_CSR             0h + (n × 1000h)


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    803 / 3791

<!-- page 804 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Function
This register contains several fields related to hardware and interrupt requests, configuration, and status for the given channel.

Diagram

 Bits       31      30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

        ACTIV
    R       DONE
         E

   W        W1C                                                     0

Reset     0       0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2       1       0

    R
                                                                       EBW    EEI   EARQ  ERQ
   W                                            0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0       0       0


Fields


       Field       Function

      31       Channel Active
    ACTIVE    The ACTIVE field indicates the channel was selected by arbitration and is executing the prescribed
                    transfers. The eDMA sets it to 1 when channel service begins, and clears it to 0 as the minor loop
                 completes or when any error condition is detected. Except for dynamic scatter/gather or dynamic
                 channel linking, you must not modify the transfer control descriptor when a channel is active.

      30       Channel Done
    DONE     The DONE field indicates the eDMA has completed the major loop. The eDMA engine sets this field as
                  the CITER count reaches zero. If enabled, the eDMA generates an interrupt request corresponding to this
                completed channel. The software clears it, or the hardware clears it when the channel is activated.

                                                 NOTE
                                       This field must be cleared to 0 before writing the MAJORELINK or ESG fields.

      29-4      Reserved
    —

       3        Enable Buffered Writes
    EBW     When buffered writes are enabled, all writes except for the last write sequence of the minor loop are
                  signaled by the eDMA as bufferable.
                     0b - Buffered writes on system bus disabled. Buffered writes on system bus disabled
                     1b - Buffered writes on system bus enabled. Bufferable write signal asserted on all system bus
                          writes except during last write sequence

       2        Enable Error Interrupt
      EEI

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    804 / 3791

<!-- page 805 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function

               The EEI field enables the error interrupt signal for the channel. The DMA error indicator and the error
                     interrupt enable flag must be asserted before an error interrupt request for a given channel is asserted to
                  the interrupt controller.
                     0b - Error signal for corresponding channel does not generate error interrupt
                     1b - Assertion of error signal for corresponding channel generates error interrupt request

       1        Enable Asynchronous DMA Request
    EARQ     The enable asynchronous DMA request field (EARQ) does not affect DMA operations. When set to 1,
                     this field allows the hardware service request enable field (ERQ) to propagate out of the DMA to the
               power controller. When cleared to 0, this field masks the hardware service request enable field to the
               power controller.
                     0b - Disable asynchronous DMA request for the channel
                     1b - Enable asynchronous DMA request for the channel

       0        Enable DMA Request
    ERQ       Disable a channel's hardware service request at the source before clearing the channel's ERQ field.
               The DMA hardware request input signal and the enable request field (ERQ) must be asserted before a
                  channel's hardware service request is accepted. The state of the eDMA enable request field does not
                     affect a channel service request made explicitly through software or channel linking. The state of the
            ERQ field does not affect the channel's START field.
                     0b - DMA hardware request signal for corresponding channel disabled
                     1b - DMA hardware request signal for corresponding channel enabled


23.6.4.3  Channel Error Status (CH0_ES - CH15_ES)

Offset
For n = 0 to 15:


 Register                   Offset

 CHn_ES               4h + (n × 1000h)


Function
The ES provides information concerning the last recorded channel error. Channel errors can be caused by:
   • An illegal setting in the transfer control descriptor
   • An error termination to a bus master read or write cycle
The ERR field signals the presence of an error for the channel. The eDMA engine signals the occurrence of an error condition by
setting the appropriate field in this register. The outputs of this register are enabled by the contents of the CHn_CSR[EEI] field,
then logically summed across all channels to form an error interrupt request, which may be routed to the interrupt controller. In
addition, this enabled error status is logically OR'd onto the channel done interrupt, CHn_INT[INT], thus forming a done or error
interrupt on a per channel basis.




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    805 / 3791

<!-- page 806 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

During the execution of the interrupt service routine associated with any DMA errors, it is software's responsibility to clear the
appropriate bit, negating the error-interrupt request. The normal DMA channel completion indicators (setting the transfer control
descriptor DONE flag and the possible assertion of an interrupt request) are not affected when eDMA detects an error. The
contents of this ERR register field can also be polled because a non-zero value indicates the presence of a channel error,
regardless of the state of the EEI mask.
The state of any given channel's error indicators is affected by writes to this register. Writing a 1 to the ERR field clears the
channel's error status, and writing a 0 has no effect.
An unspecified error, where only the ERR field is set to 1, indicates that either a transfer was cancelled with an error. The
Management Page Error Status register has full view of the error condition.
See Fault reporting and handling for more details.

Diagram

 Bits       31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R  ERR                                                        Reserved

   W  W1C                                                         0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6       5      4       3      2       1       0

    R                           Reserved                       SAE   SOE   DAE   DOE   NCE   SGE   SBE   DBE

   W                              0

Reset     0      0      0      0       0      0      0      0       0      0       0      0       0      0       0       0


Fields


       Field       Function
      31         Error In Channel
    ERR            0b - An error in this channel has not occurred
                     1b - An error in this channel has occurred

      30-8      Reserved
    —

       7        Source Address Error
     SAE     TCDn_SADDR is inconsistent with TCDn_ATTR[SSIZE].
                     0b - No source address configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_SADDR field

       6        Source Offset Error
    SOE     TCDn_SOFF is inconsistent with TCDn_ATTR[SSIZE].
                     0b - No source offset configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_SOFF field

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    806 / 3791

<!-- page 807 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function

       5         Destination Address Error
    DAE     TCDn_DADDR is inconsistent with TCDn_ATTR[DSIZE].
                     0b - No destination address configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_DADDR field

       4         Destination Offset Error
    DOE     TCDn_DOFF is inconsistent with TCDn_ATTR[DSIZE].
                     0b - No destination offset configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_DOFF field

       3       NBYTES/CITER Configuration Error
    NCE       This error indicates that one of the following has occurred:
                           • TCDn_NBYTES is not a multiple of TCDn_ATTR[SSIZE] and TCDn_ATTR[DSIZE]
                           • TCDn_CITER[CITER] is equal to zero
                           • TCDn_CITER[ELINK] is not equal to TCDn_BITER[ELINK]
                     0b - No NBYTES/CITER configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_NBYTES or
                   TCDn_CITER fields

       2         Scatter/Gather Configuration Error
    SGE     When this field is 1, it indicates that TCDn_DLAST_SGA is not on a 32-byte boundary. This field is
                checked at the beginning of a scatter/gather operation after major loop completion if TCDn_CSR[ESG]is
                 enabled.
                     0b - No scatter/gather configuration error
                     1b - Last recorded error was a configuration error detected in the TCDn_DLAST_SGA field

       1        Source Bus Error
     SBE            0b - No source bus error
                     1b - Last recorded error was bus error on source read

       0         Destination Bus Error
    DBE            0b - No destination bus error
                     1b - Last recorded error was bus error on destination write


23.6.4.4  Channel Interrupt Status (CH0_INT - CH15_INT)

Offset
For n = 0 to 15:





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    807 / 3791

<!-- page 808 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


 Register                   Offset

 CHn_INT              8h + (n × 1000h)


Function
The INT field signals the presence of an interrupt request for the channel. Depending on the appropriate bit setting in the transfer
control descriptors, the eDMA engine generates an interrupt on data transfer completion or an error condition.
The outputs of this register are directly routed to the interrupt controller. During the interrupt service routine associated with any
given channel, it is the software's responsibility to clear the appropriate bit, negating the interrupt request. On writes to INT, a 1
clears the channel's interrupt request. A zero has no effect on the channel's current interrupt status.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                            0

   W

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                        0                                                           INT

   W                                                                                     W1C

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-1      Reserved
    —

       0          Interrupt Request
      INT            0b - Interrupt request for corresponding channel cleared
                     1b - Interrupt request for corresponding channel active


23.6.4.5  Channel System Bus (CH0_SBR - CH15_SBR)

Offset
For n = 0 to 15:


 Register                   Offset

 CHn_SBR            Ch + (n × 1000h)


Function
The Channel System Bus register places identification and attribute information on the system bus interface for the eDMA.


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    808 / 3791

<!-- page 809 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                      Reserved
                                                                                                                 EMI
   W                                                        0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits       15      14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                                 0                                             MID
        PAL   SEC
   W

Reset     0       0      0      0       0      0      0      0       0      0      0      0       0      1      1      1


Fields


       Field       Function
     31-17      Reserved
    —

      16       Enable Master ID Replication
     EMI      The eDMA master ID replication field allows the eDMA to use the same protection level and system
                bus ID of the master programming the eDMA's TCD. When enabled, the eDMA uses the master ID and
                   protection level stored in the CHn_SBR registers, instead of the eDMA's default values. When a master
                      (for example a core) programs a TCD, its master ID is captured when the TCDn_CSR control attributes are
                     written. A scatter/gather operation does not affect the CHn_SBR registers. You can write the EMI only if
             MP_CSR[GMRC] = 1, which means Global Master ID Replication Control is enabled; otherwise, the EMI is
                  forced to zero.

                                                 NOTE
                                                         If master ID replication is disabled, the nonsecure, user protection level for DMA transfers
                                            is used.

                     0b - Master ID replication is disabled
                     1b - Master ID replication is enabled

      15         Privileged Access Level
     PAL       This field controls DMA's protection level on the system bus when the channel is active.

                                                 NOTE
                           The value written into this register cannot exceed the security and privilege level of the
                                core or other master writing the channel's system bus register; CHn_SBR. The order of
                              precedence is SecurePriv>SecureUser>NonsecurePriv>NonsecureUser

                     0b - User protection level for DMA transfers
                     1b - Privileged protection level for DMA transfers

      14        Security Level
    SEC      DMA's security level on the system bus when the channel is active.

                                  Tablecontinuesonthenextpage...


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    809 / 3791

<!-- page 810 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                                                 NOTE
                           The value written into this register cannot exceed the security and privilege level of the
                                core or other master writing the channel's system bus register; CHn_SBR. The order of
                              precedence is SecurePriv>SecureUser>NonsecurePriv>NonsecureUser

                     0b - Nonsecure protection level for DMA transfers
                     1b - Secure protection level for DMA transfers

      13-5      Reserved
    —

       4-0       Master ID
     MID       This field controls the DMA's master ID on the system bus when the channel is active.

                                                 NOTE
                           The ID captured in this register reflects the master ID of the core or other master writing the
                                  channel's security attributes, TCDn_SBR[SEC].


23.6.4.6  Channel Priority (CH0_PRI - CH15_PRI)

Offset
For n = 0 to 15:


 Register                   Offset

 CHn_PRI              10h + (n × 1000h)


Function
The contents of these registers define unique priorities associated with each channel within the same channel group. Channel
grouping is programmed via Channel Arbitration Group (CH0_GRPRI - CH15_GRPRI).
The channel priorities within a group are evaluated by numeric value; for example, 0 is the lowest priority, 1 is the next higher
priority, then 2, 3, and so on. Software must program the channel priorities with unique values; otherwise, channel numbers with
the same, non-zero value, will be selected based on channel number with the higher channel number having higher priority.
If more than one channel in a group has an arbitration priority level value of zero, then the arbitration mode field MP_CSR[ERCA]
is used to determine the arbitration scheme for all channels with APL=0 within a group.
When you enable round-robin channel arbitration (MP_CSR[ERCA] = 1), all channels with APL=0 within a group will use a
round-robin arbitration scheme, which rotates among these channels requesting service without regard to priority. Round-robin
provides a fairness mechanism within an arbitration group.
When you enable fixed-priority channel arbitration (MP_CSR[ERCA] = 0), eDMA selects channels with APL=0 based on channel
number, with the higher channel number having higher priority.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    810 / 3791

<!-- page 811 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits       31      30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                                  Reserved
       ECP   DPA
   W                                                                   0

Reset     0       0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                               Reserved
                                                                                                    APL
   W                                                 0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31       Enable Channel Preemption
    ECP            0b - Channel cannot be suspended by a higher-priority channel's service request
                     1b - Channel can be temporarily suspended by a higher-priority channel's service request

      30        Disable Preempt Ability
    DPA            0b - Channel can suspend a lower-priority channel
                     1b - Channel cannot suspend any other channel, regardless of channel priority

      29-3      Reserved
    —

       2-0         Arbitration Priority Level
     APL      Channel priority level for arbitration within the assigned arbitration group.


23.6.4.7  Channel Multiplexor Configuration (CH0_MUX - CH15_MUX)

Offset
For n = 0 to 15:


 Register                   Offset

 CHn_MUX             14h + (n × 1000h)


Function
Each of the DMA channels can be independently associated with various peripherals in the system. The Channel Multiplexor
Configuration register selects the peripheral assigned to each channel. Service requests from the peripheral should be disabled
when configuring a channel to a peripheral source.
Each channel must have a unique value when selecting a peripheral slot in the channel mux configuration. The only value that
may overlap is source 0. If there is an attempt to write a mux configuration value that is already consumed by any channel, a mux
configuration of 0 (SRC = 0) will be written.


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    811 / 3791

<!-- page 812 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

All channels will default to source 0. When a particular peripheral is needed, the channel’s mux configuration is set to that source
number. When the peripheral is no longer needed, the mux configuration for that channel should be written to 0, thus releasing
the resource.

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R                                                          Reserved

   W                                                            0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R                                Reserved
                                                                             SRC
   W                                  0

Reset     0      0      0      0       0      0      0      0       0      0      0      0       0      0      0      0


Fields


       Field       Function
      31-7      Reserved
    —

       6-0        Service Request Source
    SRC      Hardware service request source for the channel.

                                                 NOTE
                                   With the exception of 0, attempts to write a value already in use will be forced to 0.


23.6.4.8 TCD Source Address (TCD0_SADDR - TCD15_SADDR)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_SADDR          20h + (n × 1000h)


Function
This register contains the address for the read transactions.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    812 / 3791

<!-- page 813 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                  SADDR
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                  SADDR
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      31-0      Source Address
   SADDR    Memory address pointing to the source data.


23.6.4.9 TCD Signed Source Address Offset (TCD0_SOFF - TCD15_SOFF)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_SOFF           24h + (n × 1000h)


Function
This register contains the sign-extended value added to Source Address register after each read transaction.

Diagram

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                     SOFF
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      15-0      Source Address Signed Offset
    SOFF      Sign-extended offset applied to the current source address to form the next-state value as each source
                 read is completed.




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    813 / 3791

<!-- page 814 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

23.6.4.10 TCD Transfer Attributes (TCD0_ATTR - TCD15_ATTR)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_ATTR           26h + (n × 1000h)


Function
This register contains size and option modulo addressing information for source and destination addresses.

Diagram

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                 SMOD                       SSIZE                  DMOD                       DSIZE
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

     15-11      Source Address Modulo
   SMOD      This field defines a specific address range, which is the value after the SADDR + SOFF calculation is
                 performed on the original register value. Setting this field makes it easy to implement a circular data queue.
                 For data queues requiring power-of-2-sized bytes, the queue must start at a 0-modulo-size address and
                  the SMOD field must be set to the appropriate value for the queue, freezing the required number of upper
                address bits.
               The value programmed into this field specifies the number of lower address bits that are allowed to change.
                 For a circular queue application, you typically set TCDn_SOFF[SOFF] to the transfer size to implement
                  post-increment addressing, with the SMOD function constraining the addresses to a 0-modulo-size range.
                     0_0000b - Source address modulo feature disabled
                     0_0001b - Source address modulo feature enabled for any non-zero value [1-31]

      10-8      Source Data Transfer Size
    SSIZE           000b - 8-bit
                     001b - 16-bit
                     010b - 32-bit
                     011b - 64-bit
                     100b - 16-byte
                     101b - 32-byte
                     110b - Reserved

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    814 / 3791

<!-- page 815 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     111b - Reserved

       7-3        Destination Address Modulo
   DMOD     See the SMOD definition.

       2-0        Destination Data Transfer Size
    DSIZE     See the SSIZE definition.


23.6.4.11 TCD Transfer Size Without Minor Loop Offsets (TCD0_NBYTES_MLOFFNO -
        TCD15_NBYTES_MLOFFNO)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_NBYTES_MLOFF  28h + (n × 1000h)
 NO


Function
The TCDn_NBYTES field defines the number of bytes to transfer per service request.
Minor loop offsets are address offset values added to the final source address (TCDn_SADDR), or destination
address (TCDn_DADDR), upon minor loop completion. Minor loop completion is when the channel has finished the
service request and has transferred NBYTES. When minor loop offsets are enabled, the minor loop offset value
(TCDn_NBYTES_MLOFFYES[MLOFF]) is added to the final source address (TCDn_SADDR), to the final destination address
(TCDn_DADDR), or to both, prior to the addresses being written back to the TCD. If the major loop is complete, the minor loop
offset is ignored and the major loop address offsets (TCDn_SLAST_SDA and TCDn_DLAST_SGA) are used to compute the next
TCDn_SADDR and TCDn_DADDR values.
When minor loop mapping is enabled (SMLOE or DMLOE is 1), TCDn_NBYTES_MLOFFNO/TCDn_NBYTES_MLOFFYES is
redefined. A portion of TCDn_NBYTES_MLOFFNO/TCDn_NBYTES_MLOFFYES is used to specify multiple fields:
   • A source enable bit (SMLOE) to specify the minor loop offset must be applied to the source address (TCDn_SADDR) upon
    minor loop completion
   • A destination enable bit (DMLOE) to specify the minor loop offset must be applied to the destination address (TCDn_DADDR)
   upon minor loop completion
   • The sign extended minor loop offset value (MLOFF)
The same offset value (MLOFF) is used for both source and destination minor loop offsets. When either minor loop offset is
enabled (SMLOE set or DMLOE set), the NBYTES field is reduced to 10 bits. If both minor loop offsets are disabled (SMLOE
cleared and DMLOE cleared), the NBYTES field is a 30-bit vector.
One of two register profiles (this register or TCDn_NBYTES_MLOFFYES), defines the number of bytes to transfer per request.
Which register to use depends on whether source or destination minor loop mapping is enabled.
TCDn_NBYTES_MLOFFNO/TCDn_NBYTES_MLOFFYES is defined as follows:
   •  If SMLOE = 0 and DMLOE = 0, then see the TCDn_NBYTES_MLOFFNO register description.



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    815 / 3791

<!-- page 816 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

   •  If either SMLOE or DMLOE is 1, then see the TCDn_NBYTES_MLOFFYES register description.

Diagram

 Bits       31      30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R  SMLO  DMLO
                                                           NBYTES
   W   E     E

Reset     u       u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                    NBYTES
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      31       Source Minor Loop Offset Enable
   SMLOE      Selects whether the minor loop offset is applied to the source address upon minor loop completion.
                     0b - Minor loop offset not applied to SADDR
                     1b - Minor loop offset applied to SADDR

      30        Destination Minor Loop Offset Enable
   DMLOE     Selects whether the minor loop offset is applied to the destination address upon minor loop completion.
                     0b - Minor loop offset not applied to DADDR
                     1b - Minor loop offset applied to DADDR

      29-0      Number of Bytes To Transfer Per Service Request
   NBYTES    Number of bytes to be transferred for each service request of the channel.
             When a channel activates, the module loads the appropriate TCD contents into the eDMA engine and
                 performs the appropriate reads and writes until the byte transfer count has been reached. This process is
                  normally an indivisible operation and cannot be halted. It can, however, be stalled by using the bandwidth
                    control field, or via preemption.
                    After the byte count is exhausted, the SADDR and DADDR values are written back into the TCD memory,
               and the major loop iteration count (CITER) is decremented by one and written back to the TCD memory. If
                  the major iteration count is complete, additional processing is performed.


23.6.4.12 TCD Transfer Size with Minor Loop Offsets (TCD0_NBYTES_MLOFFYES -
        TCD15_NBYTES_MLOFFYES)

Offset
For n = 0 to 15:





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    816 / 3791

<!-- page 817 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


 Register                   Offset

 TCDn_NBYTES_MLOFF  28h + (n × 1000h)
 YES


Function
The TCDn_NBYTES field defines the number of bytes to transfer per service request.
Minor loop offset is an address offset value added to the final source address (TCDn_SADDR) or destination address
(TCDn_DADDR) upon minor loop completion. Minor loop completion occurs when the channel has finished the service request
and has transferred NBYTES. Minor loop offsets are enabled by setting either the source enable bit (SMLOE) or the destination
enable bit (DMLOE).
The source enable bit (SMLOE) specifies the minor loop offset value (MLOFF) that is to be applied to the source address
(TCDn_SADDR) upon minor loop completion. The destination enable bit (DMLOE) specifies the minor loop offset (MLOFF) that
is to be applied to the destination address (TCDn_DADDR) upon minor loop completion.
If the major loop is complete, the minor loop offsets are ignored and the major loop address offsets (TCDn_SLAST_SDA and
TCDn_DLAST_SGA) are used to compute the next TCDn_SADDR and TCDn_DADDR values.
When you enable the minor loop offset overlay (either SMLOE or DMLOE is 1), eDMA redefines TCDn_NBYTES_MLOFFNO/
TCDn_NBYTES_MLOFFYES. A portion of TCDn_NBYTES_MLOFFNO/TCDn_NBYTES_MLOFFYES specifies the sign-
extended minor loop offset value (MLOFF). The same offset value (MLOFF) applies to both source and destination minor
loop offsets. When the minor loop offset is enabled, you must align it to the transfer size of the source or destination it is associated
with. When either minor loop offset is enabled (SMLOE set or DMLOE set), the NBYTES field is reduced to 10 bits. If both minor
loop offsets are disabled (SMLOE cleared and DMLOE cleared), the NBYTES field is a 30-bit vector.
One of two register profiles (this register or TCDn_NBYTES_MLOFFNO) defines the number of bytes to transfer per request.
Which register to use depends on whether source or destination minor loop mapping is enabled.
TCDn_NBYTES_MLOFFYES is defined as follows:
   •  If either minor loop offset is enabled (SMLOE or DMLOE = 1), then see the TCDn_NBYTES_MLOFFYES register description.
   •  If SMLOE and DMLOE are both 0, then see the TCDn_NBYTES_MLOFFNO register description.

Diagram

 Bits       31      30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R  SMLO  DMLO
                                                          MLOFF
   W   E     E

Reset     u       u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                      MLOFF                                            NBYTES
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    817 / 3791

<!-- page 818 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Fields


       Field       Function

      31       Source Minor Loop Offset Enable
   SMLOE      Selects whether the minor loop offset is applied to the source address upon minor loop completion.
                     0b - Minor loop offset not applied to SADDR
                     1b - Minor loop offset applied to SADDR

      30        Destination Minor Loop Offset Enable
   DMLOE     Selects whether the minor loop offset is applied to the destination address upon minor loop completion.
                     0b - Minor loop offset not applied to DADDR
                     1b - Minor loop offset applied to DADDR

     29-10      Minor Loop Offset
   MLOFF          If SMLOE or DMLOE is 1, this field represents a sign-extended offset applied to the source or destination
                address to form the next-state value after the minor loop completes.

       9-0      Number of Bytes To Transfer Per Service Request
   NBYTES    The number of bytes to be transferred in each service request of the channel.
              As a channel activates, the module loads the appropriate TCD contents into the eDMA engine and performs
                  the appropriate reads and writes until the minor byte transfer count has been reached. This is an indivisible
                  operation and cannot be halted. It can, however, be stalled by using the bandwidth control field, or
                    via preemption.
                    After the minor count is exhausted, the SADDR and DADDR values are written back into the TCD memory,
               and the major iteration count is decremented and restored to the TCD memory. If the major iteration count
                       is complete, additional processing is performed.


23.6.4.13 TCD Last Source Address Adjustment / Store DADDR Address (TCD0_SLAST_SDA -
        TCD15_SLAST_SDA)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_SLAST_SDA     2Ch + (n × 1000h)


Function
This register contains the value added to the source address when the major loop is complete. When the store destination
address option is enabled, this field provides a pointer to memory for storing the final destination address.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    818 / 3791

<!-- page 819 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                      SLAST_SDA
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                      SLAST_SDA
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      31-0       Last Source Address Adjustment / Store DADDR Address
  SLAST_SDA   Source last address adjustment or the system memory address for destination address (DADDR) storage.
                              If (TCDn_CSR[ESDA] = 0), then:
                           • Adjustment value is added to the source address at the completion of the major iteration count.
                      This value can be used to restore the source address to the initial value or adjust the address to
                      reference the next data structure.
                           • This field uses two's complement notation for the final source address adjustment.
                 Otherwise:
                           • This address points to the 32-bit-aligned memory location where the destination address (DADDR)
                            is to be stored in system memory. By saving the final destination address in system memory via
                      the ESDA feature, you are able to compute the size of a variable destination data buffer by simply
                       subtracting the beginning DADDR from the final, saved DADDR. This feature is used together with
                      the scatter/gather operation to prevent the loss of the final DADDR, which is overwritten during the
                       scatter/gather operation.
                  The "Store Destination Address" (SDA) value must be a 32-bit-aligned location because the eDMA
                       forces the lower two address bits of the SLAST_SDA field to zero when ESDA is enabled. The module
                     performs this write operation when the major loop is done; that is, when the major iteration count
                    (CITER) decrements to zero.


23.6.4.14 TCD Destination Address (TCD0_DADDR - TCD15_DADDR)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_DADDR          30h + (n × 1000h)


Function
This register contains the address for the write transactions.


                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    819 / 3791

<!-- page 820 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                 DADDR
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                 DADDR
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      31-0       Destination Address
   DADDR    Memory address pointing to the destination data.


23.6.4.15 TCD Signed Destination Address Offset (TCD0_DOFF - TCD15_DOFF)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_DOFF           34h + (n × 1000h)


Function
This register contains the sign-extended value added to Destination Address register after each write transaction.

Diagram

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                    DOFF
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      15-0       Destination Address Signed Offset
    DOFF      Sign-extended offset that is applied to the current destination address to form the next-state value as
               each destination write is completed.




                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    820 / 3791

<!-- page 821 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

23.6.4.16 TCD Current Major Loop Count (Minor Loop Channel Linking Disabled) (TCD0_CITER_ELINKNO -
        TCD15_CITER_ELINKNO)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_CITER_ELINKNO  36h + (n × 1000h)


Function
If TCDn_CITER[ELINK] is 0, the TCDn_CITER register is defined as follows.

Diagram

 Bits       15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
        ELINK                                                  CITER
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      15       Enable Link
    ELINK     As the channel completes the minor loop, this flag enables linking to another channel as defined by the
                   relevant LINKCH field. The link target channel initiates a channel service request via an internal mechanism
                    that sets the TCDn_CSR[START] bit of the specified channel to 1.
                              If channel linking is disabled, the CITER value is extended to 15 bits in place of a link channel number. If the
                 major loop is exhausted, this link mechanism is suppressed in favor of MAJORELINK channel linking.

                                                 NOTE
                                 This field must be equal to the BITER[ELINK] field; otherwise, a configuration error
                                            is reported.

                     0b - Channel-to-channel linking disabled
                     1b - Channel-to-channel linking enabled

      14-0       Current Major Iteration Count
    CITER      This 9-bit (ELINK = 1) or 15-bit (ELINK = 0) count represents the current major loop count for the channel.
                              It is decremented each time the channel finishes a service request and is written back to TCD memory.
                    After the major iteration count is exhausted, the channel performs a number of operations — for example,
                       final source and destination address calculations — and optionally generates an interrupt to signal channel
                 completion before reloading the CITER field from the Beginning Iteration Count (BITER) field.

                                  Tablecontinuesonthenextpage...





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    821 / 3791

<!-- page 822 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                                                 NOTE
                        When the CITER field is initially loaded by software, it must be set to the same value as that
                                 contained in the BITER field.

                                                 NOTE
                                                         If the channel is configured to execute a single service request, the initial values of BITER
                            and CITER should be 0x0001.


23.6.4.17 TCD Current Major Loop Count (Minor Loop Channel Linking Enabled) (TCD0_CITER_ELINKYES -
         TCD15_CITER_ELINKYES)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_CITER_ELINKYE  36h + (n × 1000h)
 S


Function
If TCDn_CITER[ELINK] is 1, the TCDn_CITER register is defined as follows.

Diagram

 Bits       15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R            Reserved
        ELINK                         LINKCH                                       CITER
   W               0

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      15       Enable Link
    ELINK     As the channel completes the minor loop, this flag enables linking to another channel as defined by the
                   relevant LINKCH field. When enabled, an internal mechanism sets the TCDn_CSR[START] field of the
                   specified channel (LINKCH) upon minor loop completion.
                              If channel linking is disabled, the CITER value is extended to 15 bits in place of a link channel number. If the
                 major loop is exhausted, this link mechanism is suppressed in favor of MAJORELINK channel linking.

                                  Tablecontinuesonthenextpage...





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    822 / 3791

<!-- page 823 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                                                 NOTE
                                 This field must be equal to the BITER[ELINK] field; otherwise, a configuration error
                                            is reported.

                     0b - Channel-to-channel linking disabled
                     1b - Channel-to-channel linking enabled

     14-13      Reserved
    —

      12-9       Minor Loop Link Channel Number
    LINKCH         If channel-to-channel linking is enabled (ELINK = 1), then after the minor loop is exhausted the eDMA
                 engine initiates a channel service request to the channel defined by this field by writing that channel’s
              TCDn_CSR[START] field to 1.

       8-0        Current Major Iteration Count
    CITER      This 9-bit (ELINK = 1) or 15-bit (ELINK = 0) count represents the current major loop count for the channel.
                              It is decremented each time the channel finishes a service request and is written back to the TCD memory.
                    After the major iteration count is exhausted, the channel performs a number of operations — for example,
                       final source and destination address calculations — and optionally generates an interrupt to signal channel
                 completion before reloading the CITER field from the Beginning Iteration Count (BITER) field.

                                                 NOTE
                        When the CITER field is initially loaded by software, it must be set to the same value as that
                                 contained in the BITER field.

                                                 NOTE
                                                         If the channel is configured to execute a single service request, the initial values of BITER
                            and CITER should be 0x0001.


23.6.4.18 TCD Last Destination Address Adjustment / Scatter Gather Address (TCD0_DLAST_SGA -
        TCD15_DLAST_SGA)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_DLAST_SGA     38h + (n × 1000h)


Function
This register contains the value added to the destination address when the major loop is complete. When the Scatter/Gather
option is enabled, this field provides a pointer to memory for fetching a transfer control descriptor to reprogram the channel.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    823 / 3791

<!-- page 824 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits      31     30     29     28      27     26     25     24      23     22     21     20      19     18     17     16

    R
                                                    DLAST_SGA
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u

 Bits      15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
                                                    DLAST_SGA
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      31-0       Last Destination Address Adjustment / Scatter Gather Address
  DLAST_SGA   Adjustment of the last destination address or the memory address for the next transfer control descriptor to
               be loaded into this channel (scatter/gather).
                              If (TCDn_CSR[ESG] = 0) then:
                           • Adjustment value is added to the destination address at the completion of the major iteration count.
                      This value can apply to restore the destination address to the initial value or adjust the address to
                      reference the next data structure.
                           • This field uses two's complement notation for the final destination address adjustment.
                 Otherwise:
                           • This address points to the beginning of a 0-modulo 32-byte region containing the next transfer
                        control descriptor to be loaded into this channel. This channel reload is performed as the major
                          iteration count completes. The scatter/gather address must be 0-modulo 32-byte, or else a
                       configuration error is reported.


23.6.4.19 TCD Control and Status (TCD0_CSR - TCD15_CSR)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_CSR           3Ch + (n × 1000h)


Function
This register is used to enable optional features.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    824 / 3791

<!-- page 825 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits      15     14     13     12      11     10      9      8       7      6       5      4       3      2       1       0

    R                                                                 MAJO                INTHA  INTMA  STAR
         BWC                        MAJORLINKCH        ESDA  EEOP        ESG  DREQ
                                                                                                  REL...                  LF    JOR     T   W                      0

Reset     u      u      u      u       u      u      u      u       u      u       u      u       u      u       u       0


Fields


       Field       Function

     15-14      Bandwidth Control
    BWC       Throttles the amount of bus bandwidth consumed by the eDMA. Generally, as the eDMA processes the
                 minor loop, it continuously generates read/write sequences until the minor count is exhausted. This field
                  forces eDMA to stall after the completion of each read/write access, to control the bus request bandwidth
               seen by the system bus interconnect.

                                                 NOTE
                                                         If the source and destination sizes are equal, this field is ignored between the first and
                             second transfers and after the last write of each minor loop. This behavior is a side effect of
                                 reducing start-up latency.

                     00b - No eDMA engine stalls
                     01b - Reserved
                     10b - eDMA engine stalls for 4 cycles after each R/W
                     11b - eDMA engine stalls for 8 cycles after each R/W

     13-12      Reserved
    —

      11-8       Major Loop Link Channel Number
 MAJORLINKCH   If (MAJORELINK = 0) then:
                           • No channel-to-channel linking, or chaining, is performed after the major loop counter is exhausted.
                 Otherwise:
                           • After the major loop counter is exhausted, the eDMA engine initiates a channel service request at
                      the channel defined by this field by setting that channel’s TCDn_CSR[START] field to 1.

       7        Enable Store Destination Address
    ESDA     As the channel completes the major loop by either the current iteration counter (CITER) decrementing to 0,
                   or by receiving an enabled end-of-packet signal, this field enables writing the destination address (DADDR)
                    to the address stored in the SLAST_SDA field. The value written to system memory is the last DADDR value
                     prior to the DLAST_SGA offset being applied, or overwritten by an enabled scatter/gather operation. When
                  the ESDA bit is 1, SLAST_SDA contains the write pointer instead of the final source address offset. Because
                     this is a pointer and not a final offset, a last source address offset of zero is applied to SADDR instead of
                  the SLAST_SGA value.
                     0b - Ability to store destination address to system memory disabled

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    825 / 3791

<!-- page 826 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function

                     1b - Ability to store destination address to system memory enabled

       6        Enable End-Of-Packet Processing
    EEOP     When enabled by the EEOP field, an end-of-packet hardware input signal directs eDMA to discontinue
                  executing the active channel, and to treat the shutdown as the major-loop-completed event. If the EEOP
                       field is 1, the end-of-packet signal from supported peripherals is accepted. If the EEOP field is 0, the
                  end-of-packet input is ignored. With an end-of-packet retirement, the current TCD destination address (or
               ESDA-saved destination address), minus the software-saved initial address (DADDR), reflects the total
               amount of data transferred.
                     0b - End-of-packet operation disabled
                     1b - End-of-packet hardware input signal enabled

       5        Enable Link When Major Loop Complete
 MAJORELINK  As the channel completes the major loop, this flag enables linking to another channel defined by
             MAJORLINKCH. The link target channel initiates a channel service request via an internal mechanism that
                  sets the TCDn_CSR[START] field of the specified channel.

                                                 NOTE
                           To support the dynamic linking coherency model, this field is forced to 0 if written when
                         TCDn_CSR[DONE] is 1.

                     0b - Channel-to-channel linking disabled
                     1b - Channel-to-channel linking enabled

       4        Enable Scatter/Gather Processing
    ESG      As the channel completes the major loop, this flag enables scatter/gather processing in the current channel.
                              If enabled, the eDMA engine uses TCDn_DLAST_SGA as a memory pointer to a 0-modulo 32-bit address
                  containing a 32-byte data structure, which is loaded as the transfer control descriptor into local memory.

                                                 NOTE
                           To support the dynamic scatter/gather coherency model, this field is forced to 0 if written
                          when TCDn_CSR[DONE] is 1.

                     0b - Current channel’s TCD is normal format
                     1b - Current channel’s TCD specifies scatter/gather format.

       3         Disable Request
    DREQ          If this flag is 1, the eDMA hardware automatically clears the corresponding ERQ bit when the current
                 major iteration count reaches 0.
                     0b - No operation. Channel’s ERQ field not affected
                     1b - Clear the ERQ field to 0 upon major loop completion, thus disabling hardware service
                         requests. Channel’s ERQ field cleared to 0 when major loop complete

       2        Enable Interrupt If Major Counter Half-complete

                                  Tablecontinuesonthenextpage...



                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    826 / 3791

<!-- page 827 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller


                               Tablecontinuedfromthepreviouspage...

       Field       Function

   INTHALF        If this flag is 1, the channel generates an interrupt request by setting the appropriate field in the INT
                    register to 1 when the current major iteration count reaches the halfway point. Specifically, the comparison
                 performed by the eDMA engine is (CITER = (BITER/2)). This halfway point interrupt request is provided to
                  support double-buffered, also known as ping-pong, schemes, or other types of data movement where the
                 processor needs an early indication of the transfer’s progress.

                                                 NOTE
                                                                                If BITER = 1, do not use INTHALF; use INTMAJOR instead.

                     0b - Halfway point interrupt disabled
                     1b - Halfway point interrupt enabled

       1        Enable Interrupt If Major count complete
  INTMAJOR      If this flag is 1, the channel generates an interrupt request by setting the appropriate field in the INT
                    register to 1 when the current major iteration count (CITER) reaches 0.
                     0b - End-of-major loop interrupt disabled
                     1b - End-of-major loop interrupt enabled

       0        Channel Start
    START          If this flag is 1, the channel is requesting service. The eDMA hardware automatically clears this flag to 0
                     after the channel begins execution.
                     0b - Channel not explicitly started
                     1b - Channel explicitly started via a software-initiated service request


23.6.4.20 TCD Beginning Major Loop Count (Minor Loop Channel Linking Disabled) (TCD0_BITER_ELINKNO -
        TCD15_BITER_ELINKNO)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_BITER_ELINKNO  3Eh + (n × 1000h)


Function
If the TCDn_BITER[ELINK] field is 0, the TCDn_BITER register is defined as follows.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    827 / 3791

<!-- page 828 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits       15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R
        ELINK                                                   BITER
   W

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      15       Enables Link
    ELINK     As the channel completes the minor loop, this flag enables linking to another channel as defined by
                BITER[LINKCH]. The link target channel initiates a channel service request via an internal mechanism that
                  sets the TCDn_CSR[START] field of the specified channel. If channel linking is disabled, the BITER value
                 extends to 15 bits in place of a link channel number. If the major loop is exhausted, this link mechanism is
                suppressed in favor of the MAJORELINK channel linking.

                                                 NOTE
                        When the software loads the TCD, this field must be set equal to the corresponding CITER
                                             field; otherwise, a configuration error is reported. As the major iteration count is exhausted,
                       eDMA reloads the contents of this field into the CITER field.

                     0b - Channel-to-channel linking disabled
                     1b - Channel-to-channel linking enabled

      14-0        Starting Major Iteration Count
    BITER     As the transfer control descriptor is first loaded by software, this 9-bit (ELINK = 1) or 15-bit (ELINK = 0) field
               must be set equal to the value in the CITER field. As the major iteration count is exhausted, eDMA reloads
                  the contents of this field into the CITER field. If the channel is configured to execute a single service request,
                  the initial values of BITER and CITER must be 0x0001.


23.6.4.21 TCD Beginning Major Loop Count (Minor Loop Channel Linking Enabled) (TCD0_BITER_ELINKYES -
         TCD15_BITER_ELINKYES)

Offset
For n = 0 to 15:


 Register                   Offset

 TCDn_BITER_ELINKYE  3Eh + (n × 1000h)
 S


Function
If the TCDn_BITER[ELINK] field is set, the TCDn_BITER register is defined as follows.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    828 / 3791

<!-- page 829 -->

NXP Semiconductors
                                                                       Enhanced Direct Memory Access (eDMA) Controller

Diagram

 Bits       15     14     13     12      11     10      9      8       7      6      5      4       3      2      1      0

    R            Reserved
        ELINK                         LINKCH                                        BITER
   W               0

Reset     u      u      u      u       u      u      u      u       u      u      u      u       u      u      u      u


Fields


       Field       Function

      15       Enable Link
    ELINK     As the channel completes the minor loop, this flag enables linking to another channel as defined by
                BITER[LINKCH]. The link target channel initiates a channel service request via an internal mechanism that
                  sets the TCDn_CSR[START] field of the specified channel. If channel linking disables, the BITER value
                 extends to 15 bits in place of a link channel number. If the major loop is exhausted, this link mechanism is
                suppressed in favor of the MAJORELINK channel linking.

                                                 NOTE
                        When the software loads the TCD, this field must be set equal to the corresponding CITER
                                             field; otherwise, a configuration error is reported. As the major iteration count is exhausted,
                       eDMA reloads the contents of this field into the CITER field.

                     0b - Channel-to-channel linking disabled
                     1b - Channel-to-channel linking enabled

     14-13      Reserved
    —

      12-9       Link Channel Number
    LINKCH         If channel-to-channel linking is enabled (ELINK = 1), then after the minor loop is exhausted, the eDMA
                 engine initiates a channel service request at the channel defined by this field by setting that channel’s
              TCDn_CSR[START] field.

                                                 NOTE
                        When the software loads the TCD, this field must be set equal to the corresponding CITER
                                             field; otherwise, a configuration error is reported. As the major iteration count is exhausted,
                       eDMA reloads the contents of this field into the CITER field.

       8-0        Starting Major Iteration Count
    BITER     As the transfer control descriptor is first loaded by software, this 9-bit (ELINK = 1) or 15-bit (ELINK = 0) field
               must be set equal to the value in the CITER field. As the major iteration count is exhausted, eDMA reloads
                  the contents of this field into the CITER field. If the channel is configured to execute a single service request,
                  the initial values of BITER and CITER must be 0x0001.





                            MCX Nx4x Reference Manual, Rev. 5, 06/2024
Reference Manual                                 General Business Information                                    829 / 3791
