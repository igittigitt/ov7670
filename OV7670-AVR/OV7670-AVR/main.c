/*
 * OV7670-AVR.c
 *
 * Created: 10.04.2018 23:36:29
 * Author : David

 ToDos:
	-	In set Testpattern die neuen Funktionen benutzen und so den Code verschlanken
	-	


 */ 

 
 #ifndef F_CPU
 #define F_CPU 16000000UL
 #endif

#include <avr/io.h>
#include <avr/common.h>
#include <util/delay.h>
#include <avr/interrupt.h>

//#include "SCCB.h"
#include "UART.h"
#include "SCCB_Old.h"
#include "OV7670_with_Fifo.h"

int main(void)
{
    //Initial Variables
	int Programstatus = -1; 
	int height = 480;
	int width = 640;
	int BytesPerPixel = 3;
	int imageLineToRead = 0;	//Image Line, which will be read out from the FIFO


//	init_TWI();
	UART0_init();
	OV7670_init();
	sei();
	//scanForI2CDevice();
	volatile char Byte=0;
	char Errorcode;
    while (1) 
    {
		switch (Programstatus){
			case 0:  // Testing
				//read RegID	
					UART0_senden("DeviceID: ");
					OV_SCCB_RegisterToUart(0x0A);
					OV_SCCB_RegisterToUart(0x0B);
					UART0_senden_newLine();

				//set CLKRC to 1
					UART0_senden("set CLKRC:...");
					if(!(Errorcode=OV_SCCB_set_Bit(0,OV_SCCB_CLKRC)))
						UART0_senden("done!");
					else {
						UART0_senden("failed! Errorcode:");
						UART0_senden_Byte(Errorcode);
						UART0_senden_newLine();
					}

				//set Testpattern
					UART0_senden("set Testpattern:...");
					if(!(Errorcode=OV_SCCB_setTestPattern(0x10)))	//Color Bars
						UART0_senden("done!");
					else {
						UART0_senden("failed! Errorcode:");
						UART0_senden_Byte(Errorcode);
						UART0_senden_newLine();
					}
				//set RGB format:


				//Other Adjustment Trys
					UART0_senden("---------------------");
					UART0_senden_newLine();
					
					UART0_senden("CLK: ");
					OV_SCCB_RegisterToUart(OV_SCCB_CLKRC);
					UART0_senden_newLine();

					UART0_senden("setting Videoformat: ");
					OV_SCCB_RegisterToUart(OV_SCCB_COM3);
					OV_SCCB_RegisterToUart(OV_SCCB_COM7);
					OV_SCCB_RegisterToUart(OV_SCCB_COM14);
					OV_SCCB_setVideoFormat(CIF,RGB);
					OV_SCCB_RegisterToUart(OV_SCCB_COM3);
					OV_SCCB_RegisterToUart(OV_SCCB_COM7);
					OV_SCCB_RegisterToUart(OV_SCCB_COM14);
					UART0_senden_newLine();

					Programstatus=-1;
				break;
			case 1: //ReadRegister and sent it to Uart
					UART0_senden_Byte(receivedAddress);
					OV_SCCB_RegisterToUart(receivedAddress);
					
					//OV7670_captureNewImage();
					
					//sendFrameBufferToUART(width,height,BytesPerPixel);
					Programstatus=-1;

				break;
			case 2: //Change Register Value
					Errorcode = OV7670_write(receivedAddress,receivedData1);
					UART0_senden_Byte(Errorcode);
					
					Programstatus =-1;
					break;
			case 3:		//Capture Image
				OV7670_captureNewImage();
				OV7670_sendNextLine(width,BytesPerPixel);	//send the first Picture Row and wait for feedback
				imageLineToRead = 0;	//resets the Line to read from the Buffer
				Programstatus=-1;
				break;
			case 4:
				width = ((receivedData1<<8)|(receivedData2));
				Programstatus =-1;
				break;
			case 5:
				height = ((receivedData1<<8)|(receivedData2));
				Programstatus =-1;
				break;
			case 6:
				BytesPerPixel = receivedData1;
				Programstatus=-1;
				break;

			case 0x0A:	//next Line of Imagebuffer requested
				OV7670_sendNextLine(width,BytesPerPixel);
				imageLineToRead++;
				Programstatus=-1;
				break;
			case 0x0B:	//repeat of Line of Imagebuffer requested
				imageLineToRead--;
				OV7670_sendLineRepeat(imageLineToRead,width, BytesPerPixel);
				imageLineToRead++;
				Programstatus=-1;
					break;
			default:
					if(UART0_rx_complete()){
						UART0_rx_work(&Programstatus);
					}
				break;
			}
			_delay_ms(5);
	
	}
}


