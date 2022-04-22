#include <SPI.h>

SPISettings XBEESettings(350000000, MSBFIRST, SPI_MODE0);
class XBeeSPI
{
private:
    SPIClass *spi;

public:
    XBeeSPI(SPIClass *spi)
    {
        this->spi = spi;
    }
};