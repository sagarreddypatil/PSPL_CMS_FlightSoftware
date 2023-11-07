#include <hardware/dma.h>
#include <spi.h>

/*! \brief uses direct memory access to copy memory from one address to another
 *  \param src pointer to memory address that will be copied
 *  \param dst pointer to memory address that will be written to
 *  \param size size of memory transfer in bytes
 */
void dmacpy(void *src, void *dst, size_t size);

/*! \brief performs 2 DMA transfers syncronously 
 *  \param src1
 *  \param src2
 *  \param dst1
 *  \param dst2
 *  \param 
 */
void dmacpy2(void *src1, void *src2, void *dst1, void *dst2, size_t size);
