# PSUmon
PSUmon is a simple ATX power supply monitor. 
It hotplugs to any free SATA power connector and signals if voltage or ripple exceeds the ATX specification.

Tresholds are:

|Voltage |Tolerance |Ripple |LED color |
|--------|----------|-------|----------|
|3.3V    | ±5%      | 50mV  |GREEN     |
|5V      | ±5%      | 50mV  |RED       |
|12V     | ±5%      |120mV  |YELLOW    |

When voltage and ripple are within limits, the corresponding LED lights continuously.  
When voltage OR ripple exceeds the limit once, it blinks once every 3 seconds or so.  
When it exceeds any limit twice it blinks twice, ... and so on, up to 5x.
When any led blinks 3x or more, the buzzer beeps 1x or more.  
The 3.3V power may not be present on every connector, in that case the green LED will be off and the voltage is ignored.

There is an optional header to connect an external LED, for example your system power LED.
It wil signal the worst of the three voltages.
