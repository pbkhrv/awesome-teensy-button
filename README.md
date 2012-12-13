awesome-teensy-button
=====================

Source code for the Staples' "Easy Button" hack (runs on Teensy 2.0 board).

Inspiration: <http://blog.makezine.com/2011/04/08/the-awesome-button/>

Based on sample code from <http://pjrc.com/teensy/usb_keyboard.html>

---

**What does this do?**

Board poses as a USB keyboard.
You need to hook up the Easy Button's button between pin B4 and GND.

Whenever button is pressed and then released less than 2 seconds later, board sends CTRL+OPT+CMD+numpad1 key press.
Whenever button is pressed and held for longer than 2 seconds, board sends CTRL+OPT+CMD+numpad0 key press (without waiting for button to be released).

**What can you do with this?**

Use KeyboardMaestro or any other keyboard macro app to do something whenever the button is pressed for short or long periods of time.

**How do you build this?**

Follow instructions at http://pjrc.com/teensy/first_use.html to get your environment setup, then run 'make'.

