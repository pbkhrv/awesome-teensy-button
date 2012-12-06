/* Keyboard example for Teensy USB Development Board
 * http://www.pjrc.com/teensy/usb_keyboard.html
 * Copyright (c) 2008 PJRC.COM, LLC
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usb_keyboard.h"

#define LED_CONFIG	(DDRD |= (1<<6))
#define LED_ON		(PORTD &= ~(1<<6))
#define LED_OFF		(PORTD |= (1<<6))
#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))

#define PINB_BIT (1 << 4)

void send_key_press();

int main(void)
{
	uint8_t b;
	uint8_t b_prev=0xFF;

	// set for 16 MHz clock
	CPU_PRESCALE(0);

	// Configure all port B pins as inputs with pullup resistors.
	// See the "Using I/O Pins" page for details.
	// http://www.pjrc.com/teensy/pins.html
	DDRB = 0x00;
	PORTB = 0xFF;

	// Initialize the USB, and then wait for the host to set configuration.
	// If the Teensy is powered without a PC connected to the USB port,
	// this will wait forever.
	usb_init();
	while (!usb_configured()) /* wait */ ;

	// Wait an extra second for the PC's operating system to load drivers
	// and do whatever it does to actually be ready for input
	_delay_ms(1000);

	// Configure timer 0 to generate a timer overflow interrupt every
	// 256*1024 clock cycles, or approx 61 Hz when using 16 MHz clock
	// This demonstrates how to use interrupts to implement a simple
	// inactivity timeout.

	while (1) {
		// read all port B pins
		b = PINB;
		// check if any pins are low, but were high previously
    if (((b & PINB_BIT) == 0) && (b_prev & PINB_BIT) != 0) {
      send_key_press();
		}
    //
		// now the current pins will be the previous, and
		// wait a short delay so we're not highly sensitive
		// to mechanical "bounce".
		b_prev = b;
		_delay_ms(5);
	}
}

void send_key_press() {
  // first, press the GUI key
  keyboard_modifier_keys = KEY_GUI;
  keyboard_keys[0] = 0;
  keyboard_keys[1] = 0;
  keyboard_keys[2] = 0;
  keyboard_keys[3] = 0;
  keyboard_keys[4] = 0;
  keyboard_keys[5] = 0;
  usb_keyboard_send();
  _delay_ms(50);

  // next, press ALT while still holding GUI
  keyboard_modifier_keys = KEY_GUI | KEY_ALT;
  usb_keyboard_send();
  _delay_ms(50);
 
  // next, press SHIFT while still holding ALT and GUI
  keyboard_modifier_keys = KEY_GUI | KEY_ALT | KEY_SHIFT;
  usb_keyboard_send();
  _delay_ms(50);

  keyboard_keys[0] = KEYPAD_ASTERIX;
//  keyboard_keys[0] = KEY_F;
  usb_keyboard_send();
  _delay_ms(50);

  keyboard_keys[0] = 0;
  usb_keyboard_send();
  _delay_ms(50);

  keyboard_modifier_keys = 0;
  usb_keyboard_send();
}
