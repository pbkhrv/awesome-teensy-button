awesome-teensy-button
=====================

Source code for the Staples' "Easy Button" hack (runs on Teensy 2.0 board).

Inspiration: <http://blog.makezine.com/2011/04/08/the-awesome-button/>

Based on sample code from <http://pjrc.com/teensy/usb_keyboard.html>

---

**What does this do?**

Board poses as a USB keyboard.
Whenever pin B4 is grounded, board sends CTRL+OPT+CMD+* (numpad asterix) button press.

**What can you do with this?**

Use KeyboardMaestro or any other keyboard macro app to do something whenever the Easy button is pressed.

**How do you build this?**

Follow instructions at http://pjrc.com/teensy/first_use.html to get your environment setup, then run 'make'.

