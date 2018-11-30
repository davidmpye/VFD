This repository contains a design for an IV-11 based Vacuum Fluorescent Display (VFD) display unit.

Key points of the design:

* Support for 8 tubes (can install only 6 if you want the traditional 2-2-2 clock display)
* Can daisy chain boards to make longer displays
* Wifi capable (MQTT etc? - firmware doesn't yet enable this)
* Runs off a 5V 2A supply (USB B socket)
* Four buttons for setting time/other functions

Parts count/prices kept down by using 'generic' ebay modules to provide:

* DS3231 Real Time Clock (battery backed)
* Boost converter module to provide the ~25VDC to power the VFD
* Buck converter module to provide 1.5v to heat the cathodes
* Wemos D1 Mini board to provide the brains
* 5mm encapsulated addressable RGB LEDs to provide the colour effects.

![Finished clock](https://raw.githubusercontent.com/wiki/davidmpye/VFD/images/finished_clock.jpg)


Please see the project wiki for more details.

https://github.com/davidmpye/VFD/wiki
