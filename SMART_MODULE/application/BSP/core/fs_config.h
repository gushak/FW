#ifndef FS_CONFIG_H_
#define FS_CONFIG_H_

// includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "../drivers/STM32CubeUSBFS/BlockDevice/spi_nor.h"

#if defined(__cplusplus)
extern "C" {
#endif

//Minimum size of a block read. This determines the size of read buffers. This may be larger than the physical read size to improve performance by caching more of the block device.
#ifndef MBED_LFS_READ_SIZE
#define MBED_LFS_READ_SIZE		PAGE_SIZE
#endif

//Minimum size of a block program.This determines the size of program buffers.This may be larger than the physical program size to improve performance by caching more of the block device.
#ifndef MBED_LFS_PROG_SIZE
#define MBED_LFS_PROG_SIZE		PAGE_SIZE#endif	//Size of an erasable block. This does not impact ram consumption and may be larger than the physical erase size. However, this should be kept small as each file currently takes up an entire block.
#ifndef MBED_LFS_BLOCK_SIZE
#define MBED_LFS_BLOCK_SIZE		SECTOR_4K_SIZE
#endif

//Number of blocks to lookahead during block allocation. A larger lookahead reduces the number of passes required to allocate a block. The lookahead buffer requires only 1 bit per block so it can be quite large with little ram impact. Should be a multiple of 32.
#ifndef MBED_LFS_LOOKAHEAD
#define MBED_LFS_LOOKAHEAD		V_SECTOR_SIZE
#endif

#define CHIP_ERASE_FEATURE
	
#if defined(__cplusplus)
}
#endif

#endif /* FS_CONFIG_H_ */
