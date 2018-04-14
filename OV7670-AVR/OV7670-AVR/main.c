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

int main(void)
{
    /* Replace with your application code */
	char Test;
    while (1) 
    {
		SCCB_Read(0x0A,&Test);
		_delay_ms(500);
    }
}

