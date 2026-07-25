# Race-day wireless frame relay: streaming a car's camera to a big screen

Research note. Status: feasible, with one gating hardware check and one
significant race-day risk. No commitment made.

## The question

On race day we want one car's live camera view on a large screen for the
audience. The car already emits frames over USB1 as CDC with our own protocol.
The idea: a small board that rides on the chassis, plugs into that USB port,
and forwards frames over Wi-Fi to a PC running the existing viewer.

The constraint that shapes everything: **transparent to the student's code**.
Whatever we build must consume the stream the car already produces.

## Verdict

An FRDM-RW612 can do this. It is the right shape of part - it has a
host-capable high-speed USB and a Wi-Fi 6 radio on one board, which is exactly
the two halves of the job.

Three things are worth knowing before committing:

1. **The radio is 1x1 and 20 MHz only.** Full-rate streaming is 24 Mbps, which
   is a large fraction of what that radio can realistically deliver in a
   crowded hall. Decimating frames fixes this completely and costs nothing
   visually.
2. **The venue is the risk, not the silicon.** A conference hall full of phones
   is the worst RF environment we could pick. The mitigation is architectural
   and cheap: make the dongle its own access point and let the display laptop
   join it, so we never touch venue Wi-Fi.
3. **The car's USB stream is currently disabled** in the competition image.
   Race day needs it on, which is a config change with a frame-budget cost that
   must be measured, not assumed.

Two of your assumptions need correcting: the RW612 core is **260 MHz**, not
300, and the **FRDM-RW612 has no SD card slot**. Ethernet is real (RJ45,
KSZ8081 PHY, RMII, 10/100) but useless on a moving car.

## What the link actually has to carry

From our own measurements in `AVC_USB_Debug_Display_Current_State.md`, not
estimates:

| Quantity | Measured |
|---|---|
| Frame size, 320x200 RGB565 | 128,000 bytes |
| Sustained rate, native receiver, 60.0 s | 2.870 MiB/s at 23.43 FPS |
| Sustained rate, Python receiver, 10.0 s | 2.868 MiB/s at 23.43 FPS |
| Synthetic ceiling (`START_SYNTH`) | ~34 MiB/s |
| Frame drops observed | none |

So the real payload is **24.1 Mbps**, and USB itself has roughly 12x headroom
over what we use. **USB is not the bottleneck anywhere in this design.** The
32-byte packet header is noise at this frame size.

Decimation is the lever, and it is a good one:

| Sent | Effective FPS | Wi-Fi payload |
|---|---|---|
| every frame | 23.4 | 24.1 Mbps |
| every 2nd | 11.7 | 12.0 Mbps |
| every 4th | 5.9 | 6.0 Mbps |
| every 8th | 2.9 | 3.0 Mbps |
| every 2nd, downscaled to 160x100 | 11.7 | 3.0 Mbps |

For a spectator screen showing what the car sees, **6 to 12 FPS is entirely
convincing**. Nobody watching a projector is going to notice the difference
between 12 and 23 FPS on a car doing a lap. Dropping to every 4th frame buys a
4x margin against interference, which is the difference between a demo that
survives race day and one that stutters when the room fills up.

The decimation belongs in the dongle, not the car. The car keeps emitting at
full rate into a port that may or may not have anything attached, and the
dongle throws away what it cannot send. That preserves transparency and means a
Wi-Fi problem can never cost a student frame time.

## RW612 capability, checked

| Claim | Finding |
|---|---|
| Core | 260 MHz Cortex-M33, 1.2 MB on-chip SRAM |
| Wi-Fi | 1x1, **20 MHz only**, 802.11ax, plus BLE 5.4 and 802.15.4 |
| USB | Two controllers (FS and HS), each device- and host-capable |
| USB on FRDM board | Only the HS controller and PHY are used, wired to Type-C **J8** |
| Ethernet | KSZ8081 PHY, RMII, 10/100, RJ45 |
| SD card | **None on the FRDM board** |
| SDK | `usb_host_cdc` exists in the MCUXpresso USB stack; the host stack has an EHCI controller driver, which is what RW612 uses |

The host side is the part that could have killed this and does not. RW612 has an
EHCI host controller, the SDK ships a CDC host example, and our car presents as
a straightforward CDC-ACM device. That is the happy path.

### The one hardware check to make first

USB host must supply VBUS. A board whose Type-C port is device-only will not,
and no amount of firmware fixes that.

The FRDM-RW612 schematic shows an **NX5P3090UK** on J8. That part is a
current-limited USB VBUS power switch - a *source*-side component. A
device-only port has no reason to carry one. Together with the reference to
Type-C CC logic, this strongly suggests J8 is built to act as a host.

**Confirm this against the official schematic before ordering anything.** You
have direct access to it; I am reading a third-party schematic overview and
should not be trusted on a power-path detail. Specifically: is the switch
populated, is it under firmware control, and are the CC pins configured for DRP
or Rd-only? If it turns out to be device-only, the fallback is a powered hub
between dongle and car, which works but is another thing to velcro to a chassis.

Note the car is battery-powered and self-powered on USB, so VBUS here is for
detection and enumeration, not for delivering real current.

## The race-day risk, stated plainly

A 1x1 20 MHz radio in a conference hall is the weak point of this whole idea.
FIT will have hundreds of phones, several APs, and everyone's hotspot. At
24 Mbps we would be asking for a large share of a contended channel.

The fix is not a better radio, it is not being on the contended network at all:

- **Run the dongle as its own SoftAP** and connect the display laptop directly
  to it. No venue infrastructure, no DHCP surprises, no IT conversation. One
  client, one AP, one purpose.
- **Prefer 5 GHz** - far less occupied than 2.4 GHz at an event, and the 2.4 GHz
  band will be unusable.
- **Decimate.** 6 Mbps survives conditions that 24 Mbps will not.
- **Send UDP, not TCP.** For a live view, a dropped frame should be forgotten,
  not retransmitted. TCP will convert interference into growing latency, which
  looks far worse on a screen than an occasional skipped frame.

That last point matters more than it sounds. A TCP stream that falls behind
keeps falling behind, and by lap three the screen is showing where the car was
ten seconds ago. UDP with a frame sequence number and "display newest, discard
late arrivals" keeps the screen honest.

## Was there an off-the-shelf option?

You asked whether a point-to-point USB wire replacement would do it. Short
answer: **no, not usefully.**

Wireless USB as a standard is dead - the UWB-based effort was abandoned years
ago and nothing shipping today implements it. The products that market
themselves as wireless USB extenders are overwhelmingly *host-side* devices
that put a hub at the far end of a proprietary radio link, are built for
keyboards, webcams and printers rather than a 24 Mbps custom CDC stream, and
would still need mains power and a PC-side dongle. None of them is a thing you
bolt to a moving car.

So the choice is a small computer that speaks USB host on one side and a
network on the other. Which is exactly the RW612 proposal.

## Alternative silicon, honestly compared

| Option | USB host | Radio | Effort | Notes |
|---|---|---|---|---|
| **FRDM-RW612** | HS, EHCI, SDK example | Wi-Fi 6, 1x1 20 MHz | Firmware project | On-brand for an NXP event. Full USB rate available. |
| **ESP32-S3** | **Full-speed only (12 Mbps)** | Wi-Fi 4 | Firmware project | FS USB caps us at ~10 Mbps real - forces heavy decimation. Cheap and small. |
| **Raspberry Pi Zero 2 W** | HS via Linux | Wi-Fi 5 | ~50 lines of Python | Least work by a wide margin. Off-brand at an NXP event. |

The Pi is the pragmatic answer and I should say so plainly: `pyserial` reading
a CDC device and forwarding datagrams is an afternoon, not a project, and it
would almost certainly work the first time.

The RW612 is the *better* answer for this specific event, for reasons that are
real rather than sentimental: it is an NXP race, run by NXP, in front of
students being introduced to NXP parts, and a dongle we built on NXP silicon is
part of the show. It also has genuine headroom the ESP32-S3 lacks. The cost is
that it is a firmware project competing with four weeks of race preparation.

If you want the demo guaranteed, build the Pi version as the fallback and the
RW612 version as the one you hope to use. They consume the identical stream, so
the PC side is written once either way.

## What has to change on the car

Less than you might expect, which is the point of the design - but not nothing.

`CONFIG__USB_DEBUG_STREAM_ENABLE` is **`(0)` in the competition image**, and an
L99 signoff test asserts that, deliberately. Race day means turning it on, and
that is a real change to the image the students tuned:

- The stream copies each frame before `USB_DeviceCdcAcmSend()`. Our own notes
  record this as comfortably inside the 41 ms budget at 24 FPS, but "comfortably"
  was measured with the LCD doing its normal work and no student overlay code
  competing for the same milliseconds.
- **Measure it with a student's actual algorithm running**, late in the week,
  not with the reference code. The teams that win are the ones running closest
  to the deadline, and they are exactly the ones a surprise memcpy would hurt.
- If it turns out to cost too much, the decimation trick applies on the car side
  too: send every Nth frame and skip the copy entirely on the others.

One car needs this, not all of them. That is worth remembering - we can pick the
car, build it a dedicated image, and leave everyone else's untouched.

## Open questions

1. J8 VBUS sourcing - confirm against the official schematic. Gates everything.
2. Does `usb_host_cdc` build for `frdmrw612` in the current SDK, or does the
   example need porting? The host stack supports EHCI; whether the board target
   is present is a five-minute check once the SDK is installed.
3. Real Wi-Fi goodput from RW612 SoftAP to a laptop, 5 GHz, one client. The
   PHY ceiling for 1x1 20 MHz 802.11ax is 143 Mbps; embedded goodput is a
   fraction of that and I could not find a published measurement worth quoting.
   This is a measurement, not a literature question.
4. Physical mounting and power. The board needs 5 V on a chassis that already
   carries a battery, and it needs to not become a projectile.

## References

- `docs/research/AVC_USB_Debug_Transport_Protocol.md` - packet format
- `docs/research/AVC_USB_Debug_Display_Current_State.md` - the measurements above
- `docs/research/N947_USB_Host_Assessment.md` - prior host-side work on the car
- [FRDM-RW612 board, Zephyr documentation](https://docs.zephyrproject.org/latest/boards/nxp/frdm_rw612/doc/index.html)
- [FRDM-RW612 board user manual UM12160](https://www.mouser.com/pdfDocs/NXP_FRDM-RW612_UM.pdf)
- [RW612 product page](https://www.nxp.com/products/wireless-connectivity/wi-fi-plus-bluetooth-plus-802-15-4/wireless-mcu-with-integrated-tri-radio-1x1-wi-fi-6-plus-bluetooth-low-energy-5-4-802-15-4:RW612)
- [MCUXpresso SDK usb_host_cdc example](https://mcuxpresso.nxp.com/mcuxsdk/latest/html/examples/usb_examples/usb_host_cdc/readme.html)
- [MCUXpresso SDK USB Host Stack Reference Manual](https://mcuxpresso.nxp.com/mcuxsdk/latest/html/_static/usb/MCUXpresso_SDK_USB_Stack_Host_Reference_Manual.pdf)
- [FRDM-RW612 schematic overview](https://www.scribd.com/document/880975511/FRDM-RW612-SCH-3) - source of the NX5P3090UK observation, third-party and unverified
