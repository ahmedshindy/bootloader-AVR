/*
 * main.c
 *
 *  Created on: Jun 28, 2022
 *      Author: ahmedshindy
 */
/*
 * App.c
 *
 *  Created on: Feb 4, 2020
 *      Author: elProf
 */
#define F_CPU 8000000
#include "avr/io.h"
#include "avr/delay.h"
#include "avr/boot.h"
#include "avr/interrupt.h"

/*Toggle A0, filled with 0xFF to fit page size (256Bytes)*/
unsigned char App_Code[]=
{ 0x0C, 0x94, 0x2A, 0x00, 0x0C, 0x94, 0x34, 0x00, 0x0C, 0x94, 0x34, 0x00, 0x0C, 0x94, 0x34, 0x00
, 0x0C, 0x94, 0x34, 0x00, 0x0C, 0x94, 0x34, 0x00, 0x0C, 0x94, 0x34, 0x00, 0x0C, 0x94, 0x34, 0x00
, 0x0C, 0x94, 0x34, 0x00, 0x0C, 0x94, 0x34, 0x00, 0x0C, 0x94, 0x34, 0x00, 0x0C, 0x94, 0x34, 0x00
, 0x0C, 0x94, 0x34, 0x00, 0x0C, 0x94, 0x34, 0x00, 0x0C, 0x94, 0x34, 0x00, 0x0C, 0x94, 0x34, 0x00
, 0x0C, 0x94, 0x34, 0x00, 0x0C, 0x94, 0x34, 0x00, 0x0C, 0x94, 0x34, 0x00, 0x0C, 0x94, 0x34, 0x00
, 0x0C, 0x94, 0x34, 0x00, 0x11, 0x24, 0x1F, 0xBE, 0xCF, 0xE5, 0xD8, 0xE0, 0xDE, 0xBF, 0xCD, 0xBF
, 0x0E, 0x94, 0x36, 0x00, 0x0C, 0x94, 0x46, 0x00, 0x0C, 0x94, 0x00, 0x00, 0x8F, 0xEF, 0x8A, 0xBB
, 0x41, 0xE0, 0x28, 0xEC, 0x30, 0xE0, 0x8B, 0xB3, 0x84, 0x27, 0x8B, 0xBB, 0x80, 0xE1, 0x97, 0xE2
, 0xF9, 0x01, 0x31, 0x97, 0xF1, 0xF7, 0x01, 0x97, 0xD9, 0xF7, 0xF5, 0xCF, 0xF8, 0x94, 0xFF, 0xCF

, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF
, 0xFF, 0xFF, 0xFF, 0xFF};

#define CODE_SIZE  256
#define NO_OF_PAGES  2

void boot_program_page (uint16_t page, uint8_t *buf)
{
	uint16_t i;
	uint8_t sreg;
	uint32_t address;
	uint16_t word ;

	// Disable interrupts.
	sreg = SREG;
	cli();
	address = page * SPM_PAGESIZE ;


	boot_page_erase_safe (address);

	for (i=0; i<SPM_PAGESIZE; i+=2)
	{
		// Set up little Endian word.
		word  = *buf++;
		word += (*buf++) << 8;

		boot_page_fill_safe(address + i, word);
	}

	boot_page_write_safe (address);     // Store buffer in flash page.

	// Reenable RWW-section again. We need this if we want to jump back
	// to the application after bootloading.

	boot_rww_enable_safe();

	// Re-enable interrupts (if they were ever enabled).

	SREG = sreg;
}

int main(void)
{

	uint8_t  i , pageNo ;

	/*Blink a Led to indicate the BLD SW */
	DDRD |= 1 << PD0;
	for(i = 0 ; i < 3 ; i++)
	{
		PORTD ^= (1 << PD0);
		_delay_ms(1000);
	}


	//start at flash_start = 0x0000
	for(pageNo = 0 ; pageNo < NO_OF_PAGES ; pageNo++)
	{
		boot_program_page( pageNo, &App_Code[SPM_PAGESIZE*pageNo]);
	}
	//start the actual program
	asm("jmp 0");
}
