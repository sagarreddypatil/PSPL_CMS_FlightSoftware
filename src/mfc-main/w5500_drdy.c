// Device Includes
#include <w5500.h>

// EMU Includes
#include <mfc.h>

// enables specific interrupts on a given socket
void w5500_enable_sint(myspi_device_t *spi, w5500_socket_t socket,
                        w5500_socket_interrupt_t intr) {
    w5500_write8(spi, socket, W5500_Sn_IMR, intr.value);
}

// reads w5500 socket interrupt
uint8_t w5500_read_sint(myspi_device_t *spi, w5500_socket_t socket) {
    uint8_t data;
    w5500_read(spi, socket, W5500_SIR, &data, 1);
    return data;
}
// clears w5500 specific socket interrupt
void w5500_clear_sint(myspi_device_t *spi, w5500_socket_t socket) {
}