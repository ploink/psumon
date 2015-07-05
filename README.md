# PSUmon
PSUmon is a low-cost hardware ATX power supply monitor. 
It hotplugs to any free SATA power connector and signals if voltage or ripple exceeds the ATX specification.

The PCB is designed using Eagle 5, but should import ok in version 6 and 7. 
The board is very small, so the freeware edition of eagle can be used.
The firmware for the ATtiny24A microcontroller is written in C using Atmel Studio. An ISP header is included for in-system programming and debugging.

FIXME: insert image

Treshold values are:

|Voltage |Tolerance |Ripple |LED color |
|--------|----------|-------|----------|
|3.3V    | ±5%      | 50mV  |GREEN     |
|5V      | ±5%      | 50mV  |RED       |
|12V     | ±5%      |120mV  |YELLOW    |

This is how it works:
* When voltage and ripple are within limits, the corresponding LED lights continuously.
* When voltage or ripple exceeds the limit once, it blinks once every 3 seconds or so.
* When it exceeds the limit twice it blinks twice, ... and so on, up to 5x.
* When any led blinks 3x or more, the buzzer beeps 1x or more.
* The 3.3V power may not be present on every connector, in that case the green LED will be off and the voltage is ignored.
* There is a header to connect an optional external LED, for example your system power LED. It signals the worst of the three voltages.

## Parts
FIXME

## Calibration
FIXME
