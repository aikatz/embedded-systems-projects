#ifndef _SPI_H
#define _SPI_H

#include "stdint.h"

// Defs taken from from https://community.nxp.com/thread/372146
#define SPI_PUSHR_PCS0_ON 0x10000
#define SPI_CTAR_FMSZ_8BIT 0x38000000

// Initializes the hardware SPI
void SPI_init(void);

// Writes using the hardware SPI
void SPI_write(uint8_t data);

#endif /* _SPI_H */
