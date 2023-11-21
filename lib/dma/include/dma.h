#pragma once

#include <hardware/dma.h>
#include <spi.h>
#include <hardware/regs/dreq.h>

/*! \brief uses direct memory access to copy memory from one address to another
 *  1 byte at a time
 *  \param dst pointer to memory address that will be written to
 *  \param src pointer to memory address that will be copied
 *  \param size size of memory transfer in bytes
 */
void dmacpy(volatile void *dst, volatile void *src, size_t size);

/*! \brief uses direct memory access to copy memory from one address to another
 *   4 bytes at a time
 *  \param dst pointer to memory address that will be written to
 *  \param src pointer to memory address that will be copied
 *  \param size size of memory transfer in bytes
 */
void dmacpy4(volatile void *dst, volatile void *src, size_t size);

/*! \brief performs 2 DMA transfers syncronously
 *  \param src1 pointer to first data that will be copied
 *  \param src2 pointer to second data that will be copied
 *  \param dst1 memory address that will be written to
 *  \param dst2 second memory address that will be written to
 *  \param size size of memory transfer in bytes
 */
void dmacpysync(volatile void *src1, volatile void *src2, volatile void *dst1,
             volatile void *dst2, size_t size);

/*! \brief performs a DMA transfer to and from a data register with a DREQ as
 * pacing
 * \param src memory address of what will be written to the data register (dr)
 * \param dr address of the data register to be read from/written to
 * \param dst memory address that received data will be written to
 * \param size size of transfer in bytes (max between src and dst)
 * \param tx_dreq data request 
 * \param rx_dreq
 */
void dmatransfer(volatile void *src, io_rw_32 *dr, volatile void *dst, size_t size,
                 uint tx_dreq, uint rx_dreq);

void dma_write(volatile void *src, io_rw_32 *dr, size_t size, uint dreq);
