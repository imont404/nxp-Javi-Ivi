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

## Update: the relay is a phone

Everything below surveys boards to buy. It is superseded by a better answer that
costs nothing: **an Android phone riding on the chassis**.

A phone is already the exact machine this job needs, and every student has one:

| Need | Board survey answer | Phone |
|---|---|---|
| USB host for the CDC stream | the gating question for every board | OTG, built in |
| Dual-band Wi-Fi | disqualified half the survey | built in, 5 GHz |
| Power | 12-20 V PD, or another cable | own battery |
| Storage for recording | microSD, sometimes absent | tens of GB |
| A screen | none of them had one | it is a screen |
| Procurement, four weeks out | stock risk, backorders | already in a pocket |
| Cost | 15-30 dollars each | zero if borrowed, ~100 dollars for a dedicated one |

There is space under the top deck for a small phone, so it can stay attached and
serve as both recorder and transmitter: read CDC over OTG, write frames to
storage at full rate, and stream a decimated copy over Wi-Fi to the big screen.

**Recording is the part no board offered.** A phone can capture every frame to
local storage at full rate with no radio involved at all, and the network path
becomes a nice-to-have rather than the thing the demo depends on. That removes
the single largest race-day risk in this document - the contended 2.4/5 GHz
hall - because a lost network means a lost live view, not a lost recording.

### It also removes the LCD from the frame budget

If the phone is the display, the on-board LCD stops being necessary. That is
worth **34 ms per frame** - see `docs/plans/lcd-spi-throughput` - which is more
than any display optimisation on the table, and it makes the parallel-panel Rev B
work optional rather than important.

The existing behaviour already supports this: the USB stream does not transmit
when nothing is attached, so a cable can be plugged and yanked freely. Laptop at
the track, removed for the run, phone left aboard.

### What to check

1. **Weight and mounting.** A phone is 150-200 g on the top deck, which is the
   highest point and the worst place for mass. This is the same fairness point
   raised below: put it on a demonstration car, or on every car.
2. **USB host on the student's specific phone.** OTG is near-universal but not
   guaranteed, and cheap handsets sometimes omit it.
3. **CDC-ACM access without root.** `usb-serial-for-android` handles this in a
   native app; Chrome's WebUSB is the fallback and there is already a spike.
4. **Charging while acting as OTG host.** Some phones cannot do both without a
   powered Y-cable, which matters for a phone left aboard all day.

### Recommendation

Build the phone path first. A quick native Android app reading CDC and writing
to storage is a small piece of work, WebUSB is the fallback that needs no
install, and neither needs anything ordered. Treat the boards below as the
contingency if phone USB host turns out to be unreliable across the students'
handsets.

**A dedicated cheap handset is around 100 dollars**, which is three to six times
the board options and still the better buy. It arrives with a battery, a screen,
storage, dual-band Wi-Fi, a charger and a case, and it needs no driver work, no
PD trigger, no enclosure and no soldering. The boards look cheaper because their
price tag excludes everything a phone includes.

Buying one also removes the awkward parts of borrowing a student's: no reliance
on whatever handset a team turns up with, no OTG lottery, no asking someone to
strap their own phone to a moving car. It stays configured, stays charged, and
is the same every year.

## Verdict (superseded - see above)

The idea works. **Use a small Linux board, not an MCU** - under Linux the whole
firmware problem collapses into `pyserial` reading a CDC device and forwarding
UDP, which is an afternoon rather than a project. See the board survey below;
the short version is **FRDM-IMX93 if you can get one this week, otherwise a
Radxa Zero 3W** (Rockchip RK3566, $15, dual-band Wi-Fi 6, proper host port).

An FRDM-RW612 can also do it and is the right *shape* of part - host-capable
high-speed USB and a Wi-Fi 6 radio on one board. But it is a firmware project
competing with four weeks of race preparation, and its radio is the weakest of
the candidates. It is the better demo and the worse plan; build it after the
race.

Three things are worth knowing whichever way you go:

1. **Full-rate streaming is 24 Mbps**, which is a large fraction of what a small
   board's radio can realistically deliver in a crowded hall - and on the RW612,
   whose radio is 1x1 and 20 MHz only, it is most of it. Decimating frames fixes
   this completely and costs nothing visually.
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

## Board survey

Under a Linux board the firmware problem disappears entirely: `pyserial` reading
a CDC device and forwarding UDP datagrams is an afternoon, not a project. So the
selection is about hardware fit, and **two criteria do most of the sorting**:

- **Dual-band Wi-Fi.** Since 2.4 GHz will be unusable at a conference, a
  2.4-only board is disqualified from the good solution and relegated to
  needing a USB dongle.
- **A USB host port that is not also the power port.** The car plugs into the
  host port. If the board's only data port is also how it is powered, that is a
  hub and another cable on a moving chassis.

| Board | SoC | Size (mm) | Wi-Fi | USB host | Power | ~Cost |
|---|---|---|---|---|---|---|
| **Radxa Zero 3W** | **Rockchip RK3566** | 65 x 30 | **Wi-Fi 6, dual-band** | USB 3.0 host Type-C, **separate** from the OTG/power port | 5 V | **$15** 1 GB, ~$21 with 8 GB eMMC |
| **FRDM-IMX93** | i.MX93 | 105 x 65 | **Wi-Fi 6, dual-band** (IW612) | **USB 2.0 Type-A**, plus a Type-C | **12-20 V PD, will not take 5 V** | NXP board |
| Raspberry Pi Zero 2 W | BCM2710A1 | 65 x 30 | **2.4 GHz only** | one micro-USB OTG, the only data port | 5 V | ~$15 |
| Orange Pi Zero 2W | Allwinner H618 | 65 x 30 | Wi-Fi 5, dual-band | needs the 24-pin adapter board for USB-A | 5 V / 2 A | ~$20 |
| Milk-V Duo S | Sophgo SG2000 | tiny | Wi-Fi 6, dual-band (RTL8852BE) | USB 2.0 host | 5 V, ~5 W | ~$15 |
| Luckfox Pico Ultra W | **Rockchip RV1106** | 50 x 50 | **2.4 GHz only**, 256 MB RAM | USB-A, HS 480 Mbps, but **muxed with the Type-C power port** | 5 V (use the GPIO header) | ~$18-30 |
| Luckfox Lyra Zero W | **Rockchip RK3506B** | tiny | **2.4 GHz only** | USB 2.0 | 5 V | low |
| FRDM-RW612 | RW612 | - | Wi-Fi 6 but **1x1 20 MHz** | HS, EHCI, VBUS unconfirmed | 5 V | NXP board |
| ESP32-S3 | ESP32-S3 | tiny | Wi-Fi 4 | **full-speed only, 12 Mbps** | 5 V | ~$10 |

### On the cheap Rockchip boards specifically

You are right that this is where the small cheap Linux boards live, and the
board I am recommending is one of them - the **Radxa Zero 3W is a Rockchip
RK3566**, at **$15 for the 1 GB version**. It is not a premium choice; it is
simultaneously the cheapest and the best fit.

The rest of the cheap Rockchip family falls out on the radio. The **Luckfox
Pico** line (RV1106, RV1103) and the newer **Lyra Zero W** (RK3506B) are
genuinely tiny and genuinely cheap, and they are all **2.4 GHz only**. The Pico
Ultra W also has 256 MB of RAM, which is survivable for this job but leaves no
room to be careless. Given that 2.4 GHz is the band that will be saturated, they
are out for the same reason the Pi Zero 2 W is.

**One caveat on the Radxa: check stock before planning around it.** At least one
US distributor currently lists it out of stock, and a four-week runway does not
absorb a backorder. This is precisely why the FRDM-IMX93 ranks first if you can
get one from inside NXP - availability you control beats a better spec you are
waiting on.

### Luckfox Pico Ultra W, looked at properly

Asked specifically. The answer is **yes, it can work** - the USB side is better
than I expected - **but only if you power it through the GPIO header**, and the
radio remains the reason not to choose it.

What the RV1106 datasheet says, quoted:

> Support one USB 2.0 Host/Device
> Supports high-speed(480Mbps), full-speed(12Mbps) and low-speed(1.5Mbps) mode

So high-speed host is real at the silicon level, and 480 Mbps against our
24 Mbps is ample. The board brings it out to an actual USB-A socket, which is
more than the Pi Zero manages.

**The catch is in how that socket is wired.** From the board specification:

> 1 x USB-A port (Switching with USB Type-C by chip, enabled automatically for
> USB communication when USB-C is not connected)

There is **one** USB controller, muxed between the Type-C and the USB-A socket.
Power is "5V via USB-C port or PoE". So the obvious arrangement - power in on
Type-C, car on USB-A - **disables the host port**. PoE solves it and is absurd
on a moving car.

The way out is to **feed 5 V into the 26-pin GPIO header** and leave Type-C
unplugged, which lets the mux settle on USB-A. That is a clean arrangement for a
car that already has a 5 V rail, and it is the thing to verify first if you go
this way.

Two more checks before trusting it:

1. **How is the Wi-Fi module attached?** The RV1106 has exactly one USB
   controller. If the Wi-Fi 6 module were USB-attached it would collide with the
   camera stream outright. It is almost certainly SDIO, but "almost certainly"
   is not a thing to discover on race morning.
2. **Does the module do SoftAP?** The whole venue strategy depends on the relay
   hosting its own network. Worth confirming rather than assuming.

Specifications: single-core Cortex-A7 at 1.2 GHz, **256 MB DDR3L**, 8 GB eMMC,
50 x 50 mm, **2.4 GHz Wi-Fi 6 only**, ~$18-30.

**The honest summary: it is not cheaper than the Radxa, and it is worse.** More
money, a quarter of the RAM, one A7 core instead of four A55s, and the wrong
band. 256 MB is fine for forwarding frames, and one A7 will manage 6 Mbps
comfortably, so neither of those is disqualifying - the band is.

**If one is already in a drawer, it is worth an evening.** Power it from the
header, confirm the USB-A socket enumerates the car, run it as a SoftAP on the
quietest 2.4 GHz channel you can find, and decimate hard - 3 Mbps at every 8th
frame asks little enough airtime that a strong short-range link may punch
through a busy hall. That is a real possibility, not a consolation prize: at
close range you get a high MCS, and high MCS means short airtime, which is what
actually matters on a contended channel.

**If you are buying something, buy the Radxa.** There is no argument for
spending more to get less.

### Reading of that table

**The Pi Zero 2 W is the wrong answer here**, which surprised me - it is the
default choice for this kind of job and I suggested it myself before checking.
It is 2.4 GHz only, and it has a single USB data port. Fixing the radio means a
5 GHz dongle, which means a hub, because that one port is already carrying the
car. Two extra parts velcroed to a moving chassis to reach where other boards
start.

**Radxa Zero 3W is the best fit on paper.** Identical footprint to the Pi Zero,
dual-band Wi-Fi 6, and - the part that matters - a dedicated USB 3.0 host port
*separate* from the power port. Plug the car into one end, power into the other,
done. Runs Debian.

**FRDM-IMX93 is the best fit in practice, if you can get one this week.** It
carries an IW612 - the same dual-band Wi-Fi 6 radio family as the RW612 you
suggested - so it gets you the NXP story without the firmware project. It has a
real USB-A host socket, runs Debian or Yocto, and is enormously overpowered for
forwarding 24 Mbps, which is a virtue when you have four weeks and no time to
discover a performance problem.

Two caveats, and the second is the real one:

1. **105 x 65 mm** on a 270 x 197 mm chassis. It fits the top plate, but it is
   not a discreet little dongle.
2. **It will not run from 5 V.** Primary input is VBUS_IN at **12-20 V through
   USB-C PD**, regulated down on-board. The car has a 12 V battery, so the
   voltage is available - but the board expects a *PD source*, and feeding raw
   12 V into a USB-C connector skips the negotiation the sink is waiting for.
   **Check whether it powers up from a plain 12 V feed before planning around
   it.** If not, the clean answer is a small USB-C PD power bank, which also
   isolates the relay from the car's electrical system entirely - no shared
   ground with the motor drivers, which is worth something on its own.

## Recommendation

Pragmatically, and in order:

1. **If you can pull an FRDM-IMX93 off a shelf this week, use it.** Availability
   beats elegance with four weeks left, it is on-brand for an NXP event, and
   Linux plus `pyserial` makes the software a non-event. Resolve the 12 V
   question first.
2. **Otherwise order a Radxa Zero 3W.** Better mechanical fit, dual-band, proper
   host port, 5 V.
3. **Use a Pi Zero 2 W only if one is already in a drawer and you accept
   2.4 GHz** - and expect it to struggle when the hall fills.
4. **Keep the RW612 as the interesting version to build after the race**, when
   it is a project rather than a dependency. It is the better demo and the worse
   plan.

The PC side is written once regardless: all of these emit the same UDP frames.
That is what makes this low-risk - you can change your mind about the board in
week three without touching the viewer.

### The bit that is easy to forget

Whatever goes on the car adds mass, and it goes on the **top plate**, which is
the worst place for it - highest point, most leverage on weight transfer. A car
carrying a 105 x 65 mm board plus a power bank will not handle like the others.

If the streaming car is also competing for a podium, that is unfair to its team
in one direction or the other. Cleanest answer: **put the relay on a
demonstration car**, or on the same car every time and tell the teams. Not a
technical problem, but it is the kind of thing that turns into an argument on
race morning.

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
- [Radxa Zero 3W](https://radxa.com/products/zeros/zero3w/) - RK3566, Wi-Fi 6, USB 3.0 host
- [FRDM-IMX93 board](https://www.nxp.com/design/design-center/development-boards-and-designs/FRDM-IMX93) and [UM12181 user manual](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/6507/FRDM-IMX93.pdf) - source of the 12-20 V PD requirement
- [Raspberry Pi Zero 2 W product brief](https://pip.raspberrypi.com/documents/RP-008359-DS-raspberry-pi-zero-2-w-product-brief.pdf) - confirms 2.4 GHz only
- [Luckfox Pico Ultra W wiki](https://wiki.luckfox.com/Luckfox-Pico-Ultra/) - confirms 2.4 GHz only, 256 MB
- [Orange Pi Zero 2W](http://www.orangepi.org/html/hardWare/computerAndMicrocontrollers/details/Orange-Pi-Zero-2W.html)
- [Milk-V Duo S](https://milkv.io/duo-s)
- [Rockchip RV1106 datasheet v1.9](https://rockchip.fr/RV1106%20datasheet%20V1.9.pdf) - USB host and 480 Mbps quoted from the feature list
- [Luckfox Pico Ultra, CNX Software](https://www.cnx-software.com/2024/06/21/luckfox-pico-ultra-micro-development-board-features-mipi-csi-wifi-6-ethernet-poe-gpio-headers/) - source of the USB-A/Type-C mux wording
