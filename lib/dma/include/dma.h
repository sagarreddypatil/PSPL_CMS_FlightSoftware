#pragma once

#include <hardware/dma.h>
#include <spi.h>
#include <hardware/regs/dreq.h>

/*! \brief uses direct memory access to copy memory from one address to another
 *  1 byte at a time
 *  \param src pointer to memory address that will be copied
 *  \param dst pointer to memory address that will be written to
 *  \param size size of memory transfer in bytes
 */
void dmacpy(volatile void *src, volatile void *dst, size_t size);

/*! \brief uses direct memory access to copy memory from one address to another
 *   4 bytes at a time
 *  \param src pointer to memory address that will be copied
 *  \param dst pointer to memory address that will be written to
 *  \param size size of memory transfer in bytes
 */
void dmacpy4(volatile void *src, volatile void *dst, size_t size);

/*! \brief performs 2 DMA transfers syncronously
 *  \param src1
 *  \param src2
 *  \param dst1
 *  \param dst2
 *  \param size
 */
void dmacpy2(volatile void *src1, volatile void *src2, volatile void *dst1,
             volatile void *dst2, size_t size);
/*! \brief performs a DMA transfer to and from a data register with a DREQ as
 * pacing
 * \param src
 * \param dr
 * \param dst
 * \param size
 * \param dreq1
 * \param dreq2
 */
void dmatransfer(uint8_t *src, volatile uint32_t *dr, uint8_t *dst, size_t size,
                 uint dreqTX, uint dreqRX);

void dma_write(uint32_t *src, volatile uint32_t *dr, size_t size);
