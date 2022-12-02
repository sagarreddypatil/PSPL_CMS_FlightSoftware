#include <example.h>
#include <pico/stdlib.h>
#include <stdio.h>

int lmao = 0;

int main() {
  stdio_init_all();
  while (true) {
    printf("Hello, world %d!\n", lmao++);
    sleep_ms(1000);
  }
  return 0;
}