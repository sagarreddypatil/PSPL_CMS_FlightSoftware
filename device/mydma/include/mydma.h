#pragma once

#include <hardware/dma.h>
#include <myspi.h>
#include <hardware/regs/dreq.h>

/*! \brief uses direct memory access to copy memory from one address to another
 *  1 byte at a time
 *  \param dst pointer to memory address that will be written to
 *  \param src pointer to memory address that will be copied
 *  \param size size of memory transfer in bytes
 */
void dmacpy(volatile void *dst, const void *src, size_t size);