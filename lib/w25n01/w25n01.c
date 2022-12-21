#include <spi_device_impl.h>
#include <string.h>
#include <w25n01.h>

#define SHORT_MSB(x) ((x) >> 8)
#define SHORT_LSB(x) ((x)&0xFF)

SPI_MODE0;

const uint baudrate = 5 * 1000 * 1000;  // 5 MHz, max for MAX31856
SPI_INITFUNC_IMPL(w25n01, baudrate);    // 50 MHz SPI clock

void w25n01_transfer(SPI_DEVICE_PARAM, void* src, void* dst, size_t len) {
  SPI_TRANSFER(src, dst, len);
}

void w25n01_write(SPI_DEVICE_PARAM, void* src, size_t len) {
  SPI_WRITE(src, len);
}

void w25n01_read_buffer(SPI_DEVICE_PARAM, uint16_t col, void* buf, size_t len) {
  uint8_t src[len + 4];
  src[0] = w25n01_ins_fast_read;
  src[1] = SHORT_MSB(col);
  src[2] = SHORT_LSB(col);
  memset(src + 3, 0, len + 1);

  uint8_t dst[len + 4];

  SPI_TRANSFER(src, dst, len + 4);
  memcpy(buf, dst + 4, len);
}

void w25n01_write_buffer(SPI_DEVICE_PARAM, uint16_t col, void* buf, size_t len, bool random) {
  uint8_t src[len + 3];
  src[0] = random ? w25n01_ins_random_program_load : w25n01_ins_program_load;
  src[1] = SHORT_MSB(col);
  src[2] = SHORT_LSB(col);
  memcpy(src + 3, buf, len);

  SPI_WRITE(src, len + 3);
}

void w25n01_write_status_register(SPI_DEVICE_PARAM, uint8_t addr, uint8_t value) {
  uint8_t src[3] = {w25n01_ins_write_status_register, addr, value};
  SPI_WRITE(src, 3);
}

uint8_t w25n01_read_status_register(SPI_DEVICE_PARAM, uint8_t addr) {
  uint8_t src[3] = {w25n01_ins_read_status_register, addr, 0};
  uint8_t dst[3];

  SPI_TRANSFER(src, dst, 3);

  return dst[2];
}

void bbm_add(SPI_DEVICE_PARAM, uint16_t lba, uint16_t pba) {
  uint8_t src = {w25n01_ins_bbm_swap, SHORT_MSB(lba), SHORT_LSB(lba), SHORT_MSB(pba), SHORT_LSB(pba)};
  SPI_WRITE(src, 5);
}

void bbm_read(SPI_DEVICE_PARAM, w25n01_bbm_entry* entries) {
  size_t len = 2 + (4 * 20);  // 20 BBM entires, 2 shorts per entry + 1 byte header + 1 byte dummy

  uint8_t src[len];
  src[0] = w25n01_ins_read_bbm_lut;
  memset(src + 1, 0, len - 1);

  uint8_t dst[len];

  SPI_TRANSFER(src, dst, len);
  uint8_t* bbm = dst + 2;

  for (int i = 0; i < 20; i++) {
    uint16_t lba = bbm[0] << 8 | bbm[1];
    uint16_t pba = bbm[2] << 8 | bbm[3];

    // LBA[15] indicates enabled, LBA[14] indicates invalid
    bool enabled = lba & (1 << 15);
    bool invalid = lba & (1 << 14);

    entries[i] = (w25n01_bbm_entry){.logical_block_address = lba, .physical_block_address = pba, .enabled = enabled, .invalid = invalid};

    bbm += 4;
  }
}