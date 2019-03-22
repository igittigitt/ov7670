/*
 * UART.c
 *
 * Created: 10.04.2018 23:41:36
 *  Author: David
 */ 

 /*
 * UART.cpp
 *
 * Created: 06.06.2016 20:52:46
 *  Author: David
 */ 

 
 #include <avr/io.h>
 #include <avr/interrupt.h>


#include <avr/io.h> 		
#include <avr/interrupt.h>
#include <stdint.h>  
#include <string.h>
#include "UART.h"


//------------------------------------------------------------------------------

#define TX_DDR DDRD
#define TX_Port PORTD
#define TX_Pin 1

#define RX_DDR DDRD
#define RX_Port PORTD
#define RX_Pin 0

#define UART0_rx_size 64					// Nur Werte 2^n zulässig !
#define UART0_rx_mask (UART0_rx_size-1)

#define UART0_tx_size 64					// Nur Werte 2^n zulässig !
#define UART0_tx_mask (UART0_tx_size-1)

#define UART0_Befehl_FiFo 10

//------------------------------------------------------------------------------

int temp = 0;	
struct UART0_rx				//Receive Buffer
{
uint8_t data[UART0_rx_size];
uint8_t read;
uint8_t write;
}UART0_rx= {{}, 0, 0};
	
struct UART0_tx				//Transmit Buffer
{
uint8_t data[UART0_tx_size];
uint8_t read;
uint8_t write;
}UART0_tx= {{}, 0, 0};

//------------------------------------------------------------------------------

void UART0_init (void)
{
	TX_DDR |=(1<<TX_Pin);		//definiere TX Pin als Ausgang
	RX_DDR &=~(1<<RX_Pin);		//definiere RX Pin als Eingang


	UBRR0H = 0b00000000;			// Setzt Baudrate
	UBRR0L = 0b00110011;			// dezimal : 103->9600; dez 68->14400;dez 51->19200; dez 8 ->115200
	
	UCSR0A |=0b00000000;
				//	 ^---U2X  
	UCSR0B = 0b10011000;
			/* ^||||||| Einschalten	Interrupt Receive Complete
				^|||||| Einschalten Interrupt Transmit Complete
				 ^||||| Einschalten Interrupt Data Register Leer
			      ^||||	Einschalten Receiver
				   ^||| Einschalten Transmitter
					^|| Character Size
					 ^| Receive Data Bit 8
					  ^ Transmit Data Bit  8		*/
	UCSR0C = 0b00001110;
			/*  ^^|||||| USART Mode Select
				  ^^|||| Parity Mode Bits
				    ^||| STOP Bit Select
				     ^^| Character Size     11 -> 8Bit
					   ^ Clock Polarity 			*/
}

//-------------------------------------------------- UART senden --------------------------------------------------

int UART0_tx_in (uint8_t input)
{
	uint8_t next= ((UART0_tx.write+1)&UART0_tx_mask);
	if (next==UART0_tx.read)
	{
		return 0;
	}
	UART0_tx.data[UART0_tx.write]=(uint8_t)input;
	UART0_tx.write =next;
	return 1;
}

int UART0_tx_empty(void)
{
	if(UART0_tx.read==UART0_tx.write)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


void UART0_senden (char input[16])		// Senden mit UART0	
{
	int a=0;
	while(input[a] != 0)				// Solange Daten nicht ASCII 'NULL'
	{	
		UART0_tx_in(input[a]);			// Daten in FiFo_tx 
		a++;
	}
//	UART0_tx_in(0);
//	UART0_tx_in(255);
	UCSR0B |= 0b00100000;				// Data Register empty Interrupt anschalten 
}

void UART0_senden_zahl(long zahl)
{
	int input[4];
	for(int i = 0; i<4; i++)
	{
		input[i] = (zahl % 10) + 48;
		zahl = zahl / 10;
	}
	for(int i=3; i>=0; i--)
	{
		UART0_tx_in(input[i]);
	}

	UCSR0B |= 0b00100000;				// Data Register empty Interrupt anschalten 
}
void UART0_senden_Byte(uint8_t Byte)
{
UART0_tx_in((uint8_t) Byte);
}

ISR(USART_RX_vect)                    	// Receive Complete Interrupt
{ 
	UART0_rx_in(UDR0);					// Empfangene Daten ins FiFO speichern
}

ISR(USART_UDRE_vect)					// Data Register Empty Interupt
{   
	if(UART0_tx_empty())				// FiFo TX leer ?
	{
		UCSR0B &= ~0b00100000;			// Data Register empty Interrupt ausschalten
	}
	else								// Wenn FiFo nicht leer 
	{
		UDR0 = UART0_tx_out();			// Daten ausgeben
	}	
}

//------------------------------------------------------------- empfangen --------------------------------

int UART0_rx_in (uint8_t input)
{
	uint8_t temp= ((UART0_rx.write+1)&UART0_rx_mask);	//
	if (temp==UART0_rx.read)							//FiFo voll ?
	{	
		return 0;										//return 0 -> Fifo voll
	}
	UART0_rx.data[UART0_rx.write]=(uint8_t)input;		//Daten ins Array legen
	UART0_rx.write = temp;								//write auf nächste Specicherzelle schieben
	return 1;											//return 1 -> Fifo abspeichern erfolgreich 
}

char UART0_rx_out (void)						
{
	if(UART0_rx.read==UART0_rx.write)					//FiFo leer ? 
		{
	 		return 0;									//return 0 -> FiFo ist leer
		}
	char temp = UART0_rx.data[UART0_rx.read];				//FiFo Inhalt in Data schreiben
	UART0_rx.read = (UART0_rx.read +1) & UART0_rx_mask;	//read auf nächste Speicherzelle schreiben
	return temp;										//return Data
}

int UART0_rx_empty(void)								//1 wenn FIFO leer, 0 wenn fifo voll
{
	if(UART0_rx.read==UART0_rx.write)					//FiFo leer ? 
	{
		return 1;										//return 1 -> FiFo ist leer
	}
	else
	{
		return 0;
	}
}




//Empfangene Befehle identifizieren
int UART0_rx_complete(void)
{
	//Wenn CR+LF empfangen wird, ist der Befehl vollständig
	if(UART0_rx.data[(UART0_rx.write -2)&UART0_rx_mask] == 0x0D &&UART0_rx.data[(UART0_rx.write-1)&UART0_rx_mask]==0x0A)
	{																						
		return 1;
	}
	else
	{
		return 0;
	}
}

//-------------------------------------------------------- FiFo Arbeiten--------------------------------

int UART0_rx_work(int Programmstatus)
{
	char Befehl[UART0_Befehl_FiFo] = {};			// Hier steht der Befehl
	int j =0;
	volatile int i = 0;						
	do
	{	
		Befehl[i] = UART0_rx_out();		
		i++;

		//Künstlicher Zähler, um das Abfragen der beiden Bits auch beim ersten zu ermöglichen
		if(i<2)
			j=1;
		else
			j=0;
	}while((Befehl[i+j-2] != 0x0D) || (Befehl[i-1] != 0x0A));				// solange kein CR+LF erkannt wird
	

	// CR+LF löschen
	UART0_rx_out();								
	UART0_rx_out();								
	//Beginn der Befehlsauswertung
	if(Befehl==0x01)
	{
	UART0_senden("jo");
	}

	return 0;
}

//---------------------------------------------------- Senden --------

int UART0_tx_out (void)
{
	if(UART0_tx.read==UART0_tx.write)
		{
		 return 0;
		}
	temp=UART0_tx.data[UART0_tx.read];
	UART0_tx.read = (UART0_tx.read +1) & UART0_tx_mask;
	return temp;
}