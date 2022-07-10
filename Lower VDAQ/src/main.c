// From
// https://github.com/platformio/platform-atmelavr/blob/develop/examples/native-blink/src/main.c
// Native Blink Example

#include <avr/io.h>
#include <example.h>
#include <util/delay.h>

int main(void) {
  // make the LED pin an output for PORTB5
  DDRB = 1 << 5;

  int fifteen = multiply(5, 3);

  while (1) {
    _delay_ms(500);

    // toggle the LED
    PORTB ^= 1 << 5;
  }

  return 0;
}