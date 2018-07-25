This repository contains a design for an IV-11 based Vacuum Fluorescent Display (VFD) display unit.

Key points of the design:

Parts count/prices kept down by using 'generic' ebay modules to provide:

* DS3231 Real Time Clock
* Boost converter module to provide the ~25VDC to power the VFD
* Buck converter module to provide 1.5v to heat the cathodes
* Wemos D1 Mini board to provide the brains
* 5mm encapsulated addressable RGB LEDs to provide the colour effects.

The design has a few other features I think are quite neat:

The boards are designed using direct drive, so no flickering, and are easy to drive.
Uses 74HC595 shift registers.
Boards are desigend to be 'chainable' so you can put a number of them together to make a longer display with more digits.
Designed for 8, but if you want a defined clock, you can install 6 and wire links to skip over the shift registers you aren't installing.
