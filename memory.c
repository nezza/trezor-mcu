/*
 * This file is part of the TREZOR project, https://trezor.io/
 *
 * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libopencm3/stm32/flash.h>
#include <stdint.h>
#include "memory.h"
#include "sha2.h"

#define FLASH_OPTION_BYTES_1 (*(const uint64_t *)0x1FFFC000)
#define FLASH_OPTION_BYTES_2 (*(const uint64_t *)0x1FFFC008)

void memory_protect(void)
{
}

// Remove write-protection on all flash sectors.
//
// This is an undocumented feature/bug of STM32F205/F405 microcontrollers,
// where flash controller reads its write protection bits from FLASH_OPTCR
// register not from OPTION_BYTES, rendering write protection useless.
// This behaviour is fixed in future designs of flash controller used for
// example in STM32F427, where the protection bits are read correctly
// from OPTION_BYTES and not form FLASH_OPCTR register.
//
// Read protection is unaffected and always stays locked to the desired value.
void memory_write_unlock(void)
{
	flash_unlock_option_bytes();
	flash_program_option_bytes(0x0FFFCCEC);
	flash_lock_option_bytes();
}

int memory_bootloader_hash(uint8_t *hash)

{
	sha256_Raw(FLASH_PTR(FLASH_BOOT_START), FLASH_BOOT_LEN, hash);
	sha256_Raw(hash, 32, hash);
	return 32;
}
