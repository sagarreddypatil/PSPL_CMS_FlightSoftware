/* spi.simple.c
Copyright 2021 Carl John Kugler III

Licensed under the Apache License, Version 2.0 (the License); you may not use
this file except in compliance with the License. You may obtain a copy of the
License at

   http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software distributed
under the License is distributed on an AS IS BASIS, WITHOUT WARRANTIES OR
CONDITIONS OF ANY KIND, either express or implied. See the License for the
specific language governing permissions and limitations under the License.
*/
#include <string.h>

#include "spi2.h"

void spi_dma_isr();

// Hardware Configuration of SPI "objects"
// Note: multiple SD cards can be driven by one SPI if they use different slave
// selects.

StaticSemaphore_t spi_sem_buffers[2];

spi_t spis[] = {         // One for each SPI.
    {.hw_inst   = spi1,  // SPI component
     .miso_gpio = 12,    // GPIO number (not pin number)
     .mosi_gpio = 15,
     .sck_gpio  = 14,
     /* The choice of SD card matters! SanDisk runs at the highest speed. PNY
        can only mangage 5 MHz. Those are all I've tried. */
     //.baud_rate = 1000 * 1000,
     .baud_rate = 12500 * 1000,  // The limitation here is SPI slew rate.
     //.baud_rate = 6250 * 1000,  // The limitation here is SPI slew rate.
     //.baud_rate = 25 * 1000 * 1000, // Actual frequency: 20833333. Has
     // worked for me with SanDisk.

     // Following attributes are dynamically assigned
     .dma_isr     = spi_dma_isr,
     .initialized = false,  // initialized flag
     .owner       = 0,      // Owning task, assigned dynamically
     .mutex       = 0,      // Guard semaphore, assigned dynamically
     .sem_buffer  = &spi_sem_buffers[0]}};

void spi_dma_isr() {
  spi_irq_handler(&spis[0]);
}

int main() {
  my_spi_init(&spis[0]);
}

/* [] END OF FILE */