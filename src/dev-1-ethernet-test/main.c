#include <w5500.h>

SPI_DEVICE(w5500, spi0, 10);
int main()
{
    int* data;
    w5500_rw(w5500, w5500_versionr, cmn, data , 1, R);
    printf("0x%x", *data);
}