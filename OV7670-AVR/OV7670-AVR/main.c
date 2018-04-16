/*
 * OV7670-AVR.c
 *
 * Created: 10.04.2018 23:36:29
 * Author : David
 */ 

#include <avr/io.h>
#include <avr/common.h>
#include "avr/delay.h"

#include "SCCB.h"

#ifndef F_CPU
	#define F_CPU 16000000UL
#endif

int main(void)
{
    /* Replace with your application code */
	char Test;
	init_TWI();
    while (1) 
    {
		OV7670_read_register(0x0A,&Test);
				
    }
}

