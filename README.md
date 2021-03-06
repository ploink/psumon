# PSUmon
PSUmon is a low-cost hardware ATX power supply monitor. 
It hotplugs to any free SATA power connector and signals if voltage or ripple exceeds the ATX specification.

The PCB is designed using Eagle 5, but should import ok in version 6 and 7. 
The board is very small, so the freeware edition of eagle can be used.
The firmware for the ATtiny24A microcontroller is written in C using Atmel Studio. An ISP header is included for in-system programming and debugging.

<img src="https://github.com/ploink/psumon/blob/master/doc/psumon1.jpg" width=600>

Voltage tresholds are:

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

## Parts and schematic
Most parts and values are obvious from <a href="https://raw.githubusercontent.com/ploink/psumon/master/doc/circuit.png">the schematic</a> and can be exported to a text file from within Eagle. If you want to check the gerber files, just open top.gvp and bottom.gvp in <a href="http://gerbv.geda-project.org/">gerbv</a>.

Note that the included parts.txt and also the gerber files may not be up to date!

The design includes two options for a <a href="https://raw.githubusercontent.com/ploink/psumon/master/doc/sata.jpg">SATA power connector</a>. A suitable connector with only power and no data connection is hard to find. I got mine from seller CASIBAO LED LIGHT on Aliexpress, but that is no longer available, but <a href="https://www.aliexpress.com/item/Original-new-100-SATA-interface-hard-disk-interface-socket-15P-male-180-degree-line-of-single/32738192943.html">this one</a> from Shenzhen MingSheng International Trading Co., Ltd. seems to be the same. You can also get <a href="http://www.ebay.com/itm/10-Pcs-7-15-Pin-Right-Angle-SMT-Male-Sata-Connector-For-2-5-Hard-Drive-HDD-/171720426198">this 7+15pin connector</a> from seller newdeparture2010 on Ebay and simply <a href="https://raw.githubusercontent.com/ploink/psumon/master/doc/psumon2.jpg"> cut the data connector off</a>.

To measure the voltages, the DC levels are divided down using a resistive divider to get them within range of the 1.1V reference from the ATtiny24A MCU. Using a capacitor, the AC ripple voltages are coupled directly to the same MCU pins to get a better sensitivity. This means the firmware needs to average to get the DC level and determine min/max values to get the ripple.

The 78L05 (IC2) is to make sure the MCU gets a stable 5V power even if the DUT (device under test) is very bad.

Do NOT replace the three tantalum capacitors with ceramics. The ESR is important for suppressing high frequency resonance in the power cable, but you may want to add a 100nF ceramic in parallel as is required in the <a href="http://www.formfactors.org/developer/specs/atx12v%20psdg2.01.pdf">ATX PSU design guide (pdf)</a> section 3.2.6.

## Calibration
The firmware has a provision for calibration. All you need to do after programming is to connect it to a reference supply for the first time and it wil measure and store the reference values. The PSU in one of my computers is accurate to about 1.5% so I just used that, but if you plan to sell this product, you should really make a good calibration setup.

Note that even when you use precision resistors, the 1.1V reference inside the MCU is really not accurate enough to skip the calibration step!

## To wrap it up
It is strongly recommended to protect the PCB from shorts inside your computer case. Wrapping it in 30mm diameter transparant heatshrink tubing will do the job.
