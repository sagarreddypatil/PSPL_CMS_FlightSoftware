
#include <w5500.h>

SPI_DEVICE(w5500, spi0, 10);

int main()
{
    uint8_t data[4] = {1,4,5, 5};
    //w5500_rw(w5500, w5500_gar0, sn2, data, 4, R);
    
    return 0;
}