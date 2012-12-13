/* Awesome Teensy Button
 * 
 * Author: Peter Bakhirev
 *
 * based on... Keyboard example for Teensy USB Development Board
 * http://www.pjrc.com/teensy/usb_keyboard.html
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

#define CPU_PRESCALE(n)  (CLKPR = 0x80, CLKPR = (n))

#define PINB_BIT (1 << 4)

#define FALSE 0
#define TRUE 1

void send_key_press(uint8_t modifiers, uint8_t key);
void send_modifiers(uint8_t modifiers);

volatile uint16_t idle_count=0;
volatile uint8_t long_press_reported = FALSE;

int main(void)
{
  uint8_t b;
  uint8_t b_prev=0xFF;
  uint8_t press_started = FALSE;
  uint8_t press_ended = FALSE;
  uint8_t local_long_press_happened = FALSE;

  // set for 16 MHz clock
  // http://www.pjrc.com/teensy/prescaler.html
  CPU_PRESCALE(0);

  // Configure all port B pins as inputs with pullup resistors.
  // See the "Using I/O Pins" page for details.
  // http://www.pjrc.com/teensy/pins.html
  DDRB = 0x00;
  PORTB = 0xFF;

  // Configure timer 0 to generate a timer overflow interrupt every
  // 256*1024 clock cycles, or approx 61 Hz when using 16 MHz clock
  // This demonstrates how to use interrupts to implement a simple
  // inactivity timeout.
  TCCR0A = 0x00;
  TCCR0B = 0x05;
  TIMSK0 = (1<<TOIE0);

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

    // check if key was just pressed
    // (if any pins are low, but were high previously)
    if (!press_started && ((b & PINB_BIT) == 0) && (b_prev & PINB_BIT) != 0) {

      // Disable interrupts to avoid interference with ISR
      cli();
      idle_count = 0;
      long_press_reported = FALSE;
      // re-enable interrupts
      sei();

      press_started = TRUE;
      press_ended = FALSE;
      b_prev = b;

      // debounce
      _delay_ms(5);
    }

    // check if key was released and we haven't registered key press yet
    // (if any pins are high, but were low previously)
    if (press_started && !press_ended && ((b & PINB_BIT) != 0) && (b_prev & PINB_BIT) == 0) {
      press_started = FALSE;
      press_ended = TRUE;
      send_key_press(KEY_GUI | KEY_ALT | KEY_SHIFT, KEYPAD_1);
    }

    // Disable interrupts to avoid interference with ISR
    cli();
    // Store value in a non-shared variable so that it's easier to read it
    local_long_press_happened = long_press_reported;
    // re-enable interrupts
    sei();

    // check if key is being held for "long press" time
    // interrupt handler keeps track of time
    if (press_started && !press_ended && local_long_press_happened) {
      press_started = FALSE;
      press_ended = TRUE;
      send_key_press(KEY_GUI | KEY_ALT | KEY_SHIFT, KEYPAD_0);
    }

    // now the current pins will be the previous
    b_prev = b;
  }
}

/*
 * Simulates USB keybord keys being pressed by human
 */
void send_key_press(uint8_t modifiers, uint8_t key)
{
  // send modifiers first, no other keys
  keyboard_keys[0] = 0;
  keyboard_keys[1] = 0;
  keyboard_keys[2] = 0;
  keyboard_keys[3] = 0;
  keyboard_keys[4] = 0;
  keyboard_keys[5] = 0;

  send_modifiers(modifiers);
  usb_keyboard_send();
  _delay_ms(50);

  // send key
  keyboard_keys[0] = key;
  usb_keyboard_send();
  _delay_ms(50);

  // reset all keys
  keyboard_keys[0] = 0;
  keyboard_modifier_keys = 0;
  usb_keyboard_send();
  _delay_ms(50);
}

void send_modifiers(uint8_t modifier)
{
  int bit;
  for(bit=0; bit < 8; bit++) {
    if (modifier & (1 << bit)) {
      keyboard_modifier_keys |= (1 << bit);
      usb_keyboard_send();
      _delay_ms(50);
    }
  }
}

// This interrupt routine is run approx 61 times per second.
ISR(TIMER0_OVF_vect)
{
  idle_count++;
  if (idle_count > 120) {
    idle_count = 0;
    long_press_reported = TRUE;
  }
}
