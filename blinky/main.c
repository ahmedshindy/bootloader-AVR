/*
 * main.c
 *
 *  Created on: Jun 28, 2022
 *      Author: ahmedshindy
 */


#include "avr/io.h"
#include "avr/delay.h"
#include "avr/boot.h"
#include "avr/interrupt.h"

int main(void) {
	DDRD |= 1 << PD4;
	while (1) {
		PORTD |= 1 << PD4;
		_delay_ms(5);
		PORTD &= ~(1 << PD4);
		_delay_ms(1000);
	}
}
