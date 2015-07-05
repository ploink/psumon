avrdude -p t24 -P usb -c usbasp -U lfuse:w:0xe2:m  -U hfuse:w:0xdc:m -U efuse:w:0xff:m
@if errorlevel 1 pause
avrdude -p t24 -P usb -c usbasp    -U flash:w:PsuMon.hex
@if errorlevel 1 pause
