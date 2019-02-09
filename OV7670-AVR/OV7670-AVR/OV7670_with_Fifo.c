/*
 * OV7670_with_Fifo.c
 *
 * Created: 17.08.2018 23:45:16
 *  Author: David
 */ 

 #include <stdio.h>
 #include <avr/io.h>
 #include "OV7670_with_Fifo.h"
 #include "SCCB_Old.h"
 #include <util/delay.h>
 #include "UART.h"
 #include <stdbool.h>

 int PhotoHeight;
 int PhotoWidth;
 int BytesPerPixel;

 
 char OV7670_init (void){
	 char ErrorCode = 0;
	 
	 Ov7670_initPins();	//set Data directions of every pin and set default value
	 //stop and start the OV7670
	 OV_RST_PORT &= ~(1<<OV_RST_PinNo);
	 _delay_ms(100);
	 OV_RST_PORT |= (1<<OV_RST_PinNo);
	 _delay_ms(1);	//ts of OV7670

	 //Preparing the Fifo
	 //Initialization From Datasheet: "Apply/WRST and //RRST 0.1ms after power on
	 _delay_us(100);
	 OV_WRST_PORT &= ~(1<<OV_WRST_PinNo);
	 OV_RRST_PORT &= ~(1<<OV_RRST_PinNo);
	 _delay_ms(1);	//check if this step is really necessary, not written in data sheet.

	 //Pulse to Reset the Read Pointer
	 OV_RCK_PORT |= (1<<OV_RCK_PinNo);
	 _delay_ms(1);
	 OV_RCK_PORT &= ~(1<<OV_RCK_PinNo);
	 _delay_ms(1);
	 OV_RCK_PORT |= (1<<OV_RCK_PinNo);
	 _delay_ms(1);
	 OV_RCK_PORT &= ~(1<<OV_RCK_PinNo);
	 _delay_ms(1);

	 //Initialization of communication bus
	 ErrorCode=OV_SCCB_Init();
	 if(ErrorCode){
		ErrorCode = ErrorCode+100;
	 }

	 return ErrorCode;
 }


 void Ov7670_initPins(void)
 {	
 //Set the Data direction pins
	//FiFo output
	OV_D0_DDR		&= ~(1<<OV_D0_PinNo);
	OV_D1_DDR		&= ~(1<<OV_D1_PinNo);
	OV_D2_DDR		&= ~(1<<OV_D2_PinNo);
	OV_D3_DDR		&= ~(1<<OV_D3_PinNo);
	OV_D4_DDR		&= ~(1<<OV_D4_PinNo);
	OV_D5_DDR		&= ~(1<<OV_D5_PinNo);
	OV_D6_DDR		&= ~(1<<OV_D6_PinNo);
	OV_D7_DDR		&= ~(1<<OV_D7_PinNo);
	 
	//Fifo Control
	OV_VSync_DDR &= ~(1<<OV_VSync_PinNo);
	OV_HREF_DDR	&= ~(1<<OV_HREF_PinNo);
	OV_RRST_DDR	|= (1<<OV_RRST_PinNo);	//Low active
	OV_WRST_DDR	|= (1<<OV_WRST_PinNo);	//Low active
	OV_WR_DDR	|= (1<<OV_WR_PinNo);
	OV_RCK_DDR	|= (1<<OV_RCK_PinNo);

	//OV7670 control
	OV_RST_DDR	|= (1<<OV_RST_PinNo);	//Low active
	OV_SIOC_DDR |= (1<<OV_SIOC_PinNo);
	OV_SIOD_DDR |= (1<<OV_SIOD_PinNo);

	//set the default values of the output pins:
	OV_RRST_PORT |= (1<<OV_RRST_PinNo);	//low active
	OV_WRST_PORT |= (1<<OV_WRST_PinNo); //low active
	OV_WR_PORT &= ~(1<<OV_WR_PinNo);
	OV_RCK_PORT	&= ~(1<<OV_RCK_PinNo);
	OV_RST_PORT |= (1<<OV_RRST_PinNo);	//low active
 }

 //Version of igel
 //void OV7670_captureNewImage (void)
 //{
	//OV_WR_PORT |= (1<<OV_WR_PinNo); //Write Disabled (defined start value)
	//OV_WRST_PORT |= (1<<OV_WRST_PinNo);
	//while(!getValueOfPin(OV_VSync_PIN,OV_VSync_PinNo));	//Wait for VSync to indicate a new Frame
	//while(getValueOfPin(OV_VSync_PIN,OV_VSync_PinNo));
	//OV_WRST_PORT &= ~(1<<OV_WRST_PinNo);
	//_delay_ms(1);
	//OV_WRST_PORT |= (1<<OV_WRST_PinNo);
//while(!getValueOfPin(OV_VSync_PIN,OV_VSync_PinNo)); //Wait for the next VSync Pulse, so the frame is complete
	//OV_WR_PORT &= ~(1<<OV_WR_PinNo); //Write Disabled that the picture is safe in the Framebuffer
	//_delay_ms(20);
//
	//}
//my version
 void OV7670_captureNewImage (void)
 {
	 OV_WR_PORT &= ~(1<<OV_WR_PinNo); //Write Disabled (defined start value)
	 while(!getValueOfPin(OV_VSync_PIN,OV_VSync_PinNo));	//Wait for VSync to indicate a new Frame
	 OV7670_ResetFifoWritePointer();//Enable Write Pointer
	 while(getValueOfPin(OV_VSync_PIN,OV_VSync_PinNo));
	 OV_WR_PORT |= (1<<OV_WR_PinNo);	//Write Enable so that the camera can write on the FrameBuffer
	 while(!getValueOfPin(OV_VSync_PIN,OV_VSync_PinNo)); //Wait for the next VSync Pulse, so the frame is complete
	 OV_WR_PORT &= ~(1<<OV_WR_PinNo); //Write Disabled that the picture is safe in the Framebuffer
	 _delay_ms(20);

	 //Reset the Write Pointer, (not sure, if that will work)
	 OV7670_ResetFifoWritePointer();
	 
 }


void OV7670_ResetFifoReadPointer()
{
	//Reset the ReadPointer
	OV_RRST_PORT &= ~(1<<OV_RRST_PinNo);	//changed
	//Pulse two cycles
	OV_RCK_PORT |= (1<<OV_RCK_PinNo);
	_delay_ms(1);
	OV_RCK_PORT &= ~(1<<OV_RCK_PinNo);
	_delay_ms(1);
	OV_RCK_PORT |= (1<<OV_RCK_PinNo);
	_delay_ms(1);
	OV_RCK_PORT &= ~(1<<OV_RCK_PinNo);
	_delay_ms(1);
	//Set RRST again to HIGH
	OV_RRST_PORT |= (1<<OV_RRST_PinNo);		//Changed
}

void OV7670_ResetFifoWritePointer()
{
	OV_WRST_PORT &= ~(1<<OV_WRST_PinNo);
	_delay_us(6);	
	OV_WRST_PORT |= (1<<OV_WRST_PinNo);
}


void OV7670_sendNextLine(int width, int BytesPerPixel){
	char test =0;
	for(int i = 0; i < width;i++){
		for(int j = 0; j< BytesPerPixel;j++){
			//if(test==0){
				//UART0_senden_Byte(0xCE);
				//test=1;
			//}
			//else
			//{
			//UART0_senden_Byte(0xE2);
			//test=0;
			//}
			UART0_senden_Byte(Ov7670_readByte());
			//UART0_senden_Byte(test);
			OV_RCK_PORT |= (1<<OV_RCK_PinNo);
			//_delay_ms(1);
			//_delay_us(0.1);
			OV_RCK_PORT &= ~(1<<OV_RCK_PinNo);
			//_delay_ms(1);
			//_delay_us(0.1);
			
			//Wait due to speed of UART (Avoid errors due to overwrite)
			//_delay_ms(15);

			
		}
	}
	//Send Line End
	UART0_senden_Byte(13);
	UART0_senden_Byte(10);
}



void OV7670_sendLineRepeat(int imageLineToRepeat, int width, int BytesPerPixel){
	//Reset the ReadCounter
	OV7670_ResetFifoReadPointer();
	//Clock till requested Line is reached
	for(int i = 1;i<imageLineToRepeat;i++)	//start at 1, to stop when requested Line is reached
	{
		for(int j = 0; j<width;j++)
		{
			for(int h =0; h<BytesPerPixel;h++){
			//clock without reading the data, to reach the next Line
			OV_RCK_PORT |= (1<<OV_RCK_PinNo);
			_delay_ms(1);
			OV_RCK_PORT &= ~(1<<OV_RCK_PinNo);
			_delay_ms(1);
			}
		}
	}
	//send the Line
	OV7670_sendNextLine(width,BytesPerPixel);
}

 char Ov7670_readByte (void)
 {
	char newByte;
	newByte = getValueOfPin(OV_D0_PIN, OV_D0_PinNo);
	newByte |= (getValueOfPin(OV_D1_PIN, OV_D1_PinNo)<<1);
	newByte |= (getValueOfPin(OV_D2_PIN, OV_D2_PinNo)<<2);
	newByte |= (getValueOfPin(OV_D3_PIN, OV_D3_PinNo)<<3);
	newByte |= (getValueOfPin(OV_D4_PIN, OV_D4_PinNo)<<4);
	newByte |= (getValueOfPin(OV_D5_PIN, OV_D5_PinNo)<<5);
	newByte |= (getValueOfPin(OV_D6_PIN, OV_D6_PinNo)<<6);
	newByte |= (getValueOfPin(OV_D7_PIN, OV_D7_PinNo)<<7);
	
	return (newByte);
 }


 char getValueOfPin (char thePort, char thePinNo)
 {
	char tempChar = (thePort & (1<<thePinNo)) >> thePinNo;
	return (tempChar);
 }


char OV7670_checkConnection( void )
{
	char querryData;
	OV7670_read(0x0A,&querryData);
	if (querryData==0x76){
		return (1);
	}
	else {
		return (0);
	}
}

 void sendFrameBufferToUART (int ImageWidth, int ImageHeight, int BytesPerPixel)
 {
	OV7670_ResetFifoReadPointer();

	for(int height = 0; height < ImageHeight;height++)
	{
		for(int width =0; width < ImageWidth; width++)
		{
			for(int ByteNumber =0; ByteNumber < BytesPerPixel; ByteNumber++)
			{
				UART0_senden_Byte(Ov7670_readByte());
				OV_RCK_PORT |= (1<<OV_RCK_PinNo);
				_delay_ms(1);
				OV_RCK_PORT &= ~(1<<OV_RCK_PinNo);
				_delay_ms(1);
				
				_delay_ms(10);
			}
		}
		UART0_senden_newLine();
	}
 }



/*char OV7670_write	(char regID, char regData){
	OV_SCCB_Start();

	//Phase1: Write Register ansprechen
	if(OV_SCCB_Write(OV_SCCB_write_Addr)==0){
		UART0_senden("Error: Write: 1. Phase");
		UART0_senden_newLine(); 
		OV_SCCB_Stopp();
		return 0;
	}
	UART0_senden_newLine(); 
	_delay_us(OV_SIO_CLKDELAY);

	//Phase 2: UnterRegister des SCCB ansprechen
	if(OV_SCCB_Write(regID)==0){
		UART0_senden("Error: Write: 2.Phase");
		UART0_senden_newLine(); 
		OV_SCCB_Stopp();
		return 0;
	}
	UART0_senden_newLine();
	_delay_us(OV_SIO_CLKDELAY);

	//Phase 3: Daten in Register schreiben
	if(OV_SCCB_Write(regData)==0){
		UART0_senden("Error: Write: Phase 3");
		UART0_senden_newLine(); 
		OV_SCCB_Stopp();
		return 0;
	}
	UART0_senden_newLine();
	OV_SCCB_Stopp();
	return 1;
}
*/

/*char OV7670_read (char regID, char* regData){
  UART0_senden_newLine(); 
  	
	OV_SCCB_Start();
	//2 Write Phasen notwendig: Write the IP Address for the right register
	//Phase1: Write Register ansprechen
	if(OV_SCCB_Write(OV_SCCB_write_Addr)==0){
		UART0_senden("Error: Read: Write: 1. Phase");
		UART0_senden_newLine(); 
		OV_SCCB_Stopp();
		return 0;
	}
	_delay_us(OV_SIO_CLKDELAY);

	//Phase2: Write Adress ansprechen
	if(OV_SCCB_Write(regID)==0){
		UART0_senden("Error: Read: Write 2. Phase");
		UART0_senden_newLine(); 
		OV_SCCB_Stopp();
		return 0;
	}
	_delay_us(OV_SIO_CLKDELAY);

	OV_SCCB_Stopp();

	_delay_us(OV_SIO_CLKDELAY);
	//2 Read Phasen notwendig
	//Phase 1: ID des Leseregisters
	OV_SCCB_Start();
	if(OV_SCCB_Write(OV_SCCB_read_Addr)==0){
			UART0_senden("Error: Read: Read 1. Phase");
			UART0_senden_newLine(); 
			OV_SCCB_Stopp();
			return 0;
	}
	_delay_us(OV_SIO_CLKDELAY);

	//Phase 2: Data Einlesen
	if(OV_SCCB_Read(regData)==0){
		UART0_senden("Error: Read: Read 2. Phase");
		UART0_senden_newLine(); 
		OV_SCCB_Stopp();
		return 0;
	}

	//Beenden
	OV_SCCB_Stopp();
	return 1;
}
*/