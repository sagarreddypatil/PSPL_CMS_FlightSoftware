
#include <spi.h>
#include <stdio.h>

void print8 (const uint8_t* arr, size_t len)
{
    for(int i = 0; i < len; i++)
    {
        uint8_t addr = *(arr + i);
        printf("0x%02x,",  addr);
    }
}

void print16 (const uint16_t* arr, size_t len)
{
    for(int i = 0; i < len; i++)
    {
        uint16_t addr = *(arr + i);
        printf("0x%04x,",  addr);
    }
}

void spi_set_format (spi_inst_t * spi, uint data_bits, spi_cpol_t cpol, spi_cpha_t cpha, spi_order_t order )
{
}

int spi_set_baudrate(spi_inst_t * spi, uint baud)
{
    return baud;
}

int spi_write_read_blocking	(spi_inst_t * spi, const uint8_t * src, uint8_t * dst, size_t len)
{
    printf("SPI_WRITE_READ_BLOCKING\n");
    printf("src: ");
    print8(src, len);
    printf("\n");
}		

int spi_write16_read16_blocking	(spi_inst_t * spi, const uint16_t * src, uint16_t * dst, size_t len)
{  
    printf("SPI_WRITE16_READ16_BLOCKING\n");
    printf("src: ");
    print16(src, len);
    printf("\n");
}	

int spi_write_blocking	(spi_inst_t * spi, const uint8_t * src, size_t len)
{
    printf("SPI_WRITE_BLOCKING\n");
    printf("src : ");
    print8(src, len);
    printf("\n");
}

int spi_read_blocking (spi_inst_t * spi, uint8_t repeated_tx_data, uint8_t * dst, size_t len)
{
    printf("SPI_READ_BLOCKING\n");
    printf("data: ");
    print8(dst, len);
    printf("\n");
}


int spi_write16_blocking (spi_inst_t * spi, const uint16_t * src, size_t len)
{
    printf("SPI_WRITE16_BLOCKING\n");
    printf("src: ");
    print16(src, len);
    printf("\n");
}

int spi_read16_blocking (spi_inst_t * spi, uint16_t repeated_tx_data, uint16_t * dst, size_t len) 
{
    printf("SPI_READ16_BLOCKING\n");
    printf("dst: ");
    print16(dst, len);
    printf("\n");
}