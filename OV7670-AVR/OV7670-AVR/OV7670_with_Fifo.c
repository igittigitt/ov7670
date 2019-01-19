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

 void OV7670_captureNewImage (void)
 {
	//Preparing the Fifo		
		//Reset the WritePointer //Low active

	while(!getValueOfPin(OV_VSync_PIN,OV_VSync_PinNo));	//Wait for VSync to indicate a new Frame
		OV_WRST_PORT &= ~(1<<OV_WRST_PinNo);
		_delay_us(6);	
		//Enable Write Pointer
	OV_WRST_PORT |= (1<<OV_WRST_PinNo);
	//OV_WRST_PORT |= (1<<OV_WRST_PIN);	//Set Write Pointer to 0
	//OV_WRST_PORT &= ~(1<<OV_WRST_PIN);	
	OV_WR_PORT &= ~(1<<OV_WR_PinNo);	//Write Enable so that the camera can write on the FrameBuffer
	while(!getValueOfPin(OV_VSync_PIN,OV_VSync_PinNo)); //Wait for the next VSync Pulse, so the frame is complete
	OV_WR_PORT |= (1<<OV_WR_PinNo); //Write Disabled so that the picture is safe in the Framebuffer
	_delay_ms(20);
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


//Not used at the moment.
 void sendFrameBufferToUART (int ImageWidth, int ImageHeight, int BytesPerPixel)
 {
		OV7670_ResetFifoReadPointer();

	for(int height = 0; height < ImageHeight;height++)
	{
		for(int width =0; width < ImageWidth; width++)
		{
			for(int ByteNumber =0; ByteNumber < BytesPerPixel; ByteNumber++)
			{
				UART0_senden_Byte(readByte());
				OV_RCK_PORT |= (1<<OV_RCK_PinNo);
				_delay_ms(1);
				OV_RCK_PORT &= ~(1<<OV_RCK_PinNo);
				_delay_ms(1);
				
				_delay_ms(10);
			}
		}
		UART0_senden_newLine();
	}
	UART0_senden("done!");
	UART0_senden_newLine();
 }

/**
 * \brief 
 *		This function sends the next Image Line from the Fifo Buffer. 
 *		Therefore the ReadBuffer is clocked as often, as the wdith by BytesPerPixel is overhanded
 * \param width
 *		xResolution of the image
 * \param BytesPerPixel
 *		BytesPerPixel dedicated to the ColorFormat of the IMage (e.g. RGB888, RGB565)
 * \return void
 */
void OV7670_sendNextLine(int width, int BytesPerPixel){
	for(int i = 0; i < width;i++){
		for(int j = 0; j< BytesPerPixel;j++){
			UART0_senden_Byte(readByte());
			OV_RCK_PORT |= (1<<OV_RCK_PinNo);
			_delay_ms(1);
			OV_RCK_PORT &= ~(1<<OV_RCK_PinNo);
			_delay_ms(1);
			
			//Wait due to speed of UART (Avoid errors due to overwrite)
			_delay_ms(15);
		}
	}
	//Send Line End
	UART0_senden_Byte(13);
	UART0_senden_Byte(10);
}


/**
 * \brief 
*		This function resets the ReadPointer of the Fifo and clock the readpointer until it reaches the imageLineToRead.
 *		When reached, this Line is send via UART
 * \param imageLineToRepeat
 *		This is the imageLine which should be repeated. (starting with 0)
 * \param width
 *		The width of the Image (xResolution)
 * \param BytesPerPixel
 *		Bytes per Pixel (dedicated on the imageformat (RGB888,RGB565, etc)
 * \return void
 */
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

//This function read the Byte from the 8-Bit Kamera/Fifo Port
 char readByte (void)
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

 //This function get the Value of a Pin
 char getValueOfPin (char thePort, char thePinNo)
 {
	char tempChar = (thePort & (1<<thePinNo)) >> thePinNo;
	return (tempChar);
 }

 //This function scan the I2C-Bus for connected Devices with the addresses 0...127
 
void init_OV7670WithFifo(void)
{
	//Start I2C Connection

	//Reset all neccessary camera registers

	//read the registers

	//Setup the camera
}

char OV7670_init (void){
  char ErrorCode;
  
  UART0_senden("OV7670_init... start");
  UART0_senden_newLine();
  
	//Pins konfigurieren
		OV_VSync_DDR	&= ~(1<<OV_VSync_PinNo);
		OV_HREF_DDR	&= ~(1<<OV_HREF_PinNo);
		OV_D0_DDR		&= ~(1<<OV_D0_PinNo);
		OV_D1_DDR		&= ~(1<<OV_D1_PinNo);
		OV_D2_DDR		&= ~(1<<OV_D2_PinNo);
		OV_D3_DDR		&= ~(1<<OV_D3_PinNo);
		OV_D4_DDR		&= ~(1<<OV_D4_PinNo);
		OV_D5_DDR		&= ~(1<<OV_D5_PinNo);
		OV_D6_DDR		&= ~(1<<OV_D6_PinNo);
		OV_D7_DDR		&= ~(1<<OV_D7_PinNo);
		OV_RRST_DDR	|= (1<<OV_RRST_PinNo);	//Low active
		OV_WRST_DDR	|= (1<<OV_WRST_PinNo);	//Low active
		OV_WR_DDR	|= (1<<OV_WR_PinNo);	//Low active
		OV_RCK_DDR	|= (1<<OV_RCK_PinNo);
		OV_OE_DDR	|= (1<<OV_OE_PinNo);
	
	//Preparing the Fifo
		//disable write Enable
		OV_WR_PORT |= (1<<OV_WR_PinNo); //low active
		//Reset the WritePointer //Low active
		OV_WRST_PORT &= ~(1<<OV_WRST_PinNo);
		_delay_ms(1);
		OV_WRST_PORT |= (1<<OV_WRST_PinNo);	
		_delay_ms(1);
		
		//Reset the ReadPointer
		OV_RRST_PORT &= ~(1<<OV_RRST_PinNo);
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
		OV_RRST_PORT |= (1<<OV_RRST_PinNo);
		//Set low active OE Output Enable to low
		OV_OE_PORT &= ~(1<<OV_OE_PinNo);

  //Power Down deaktivieren 
  //OV_PWDN_PORT &= ~(1<<OV_PWDN_PinNo);	//can be added when a PWDN Pin is available


  UART0_senden("Hardware Reset");
  UART0_senden_newLine();
  //OV_RESET_PORT &= ~(1<<OV_RESET_PinNo);  //Can be activated if a Pin is available
  _delay_ms(1);
  //OV_RESET_PORT |= (1<<OV_RESET_PinNo);	//Can be activated if a Pin is available
  _delay_ms(2);	//just one millisecond necesarry for Bootup

  //Initianilizatioin of communication bus
	if(!(ErrorCode=OV_SCCB_Init()))
	{
		UART0_senden("init Ov7670... done");
		UART0_senden_newLine();
	}
	else
	{
		UART0_senden("Fehler bei Initialisierung: ErrorCode: ");
		UART0_senden_Byte(ErrorCode);
	}
	return ErrorCode;
  }

int OV7670_checkConnection( void )
{
	char querryData;
	OV7670_read(0x0A,&querryData);
	if (querryData==0x76){
		UART0_senden("OV7670 connected");
		UART0_senden_newLine();
		return (1);
	}
	else {
		UART0_senden("no device connected");
		UART0_senden_Byte(querryData);
		UART0_senden_newLine();
		return (0);
	}
}

char OV7670_getByte (void)
{
	char newByte = 0x00;
	
	  //Build the newByte
	  newByte |= (OV_D7_PIN>>OV_D7_PinNo)&(0x01);
	  newByte = (newByte << 1)|((OV_D6_PIN>>OV_D6_PinNo)&(0x01));
	  newByte = (newByte << 1)|((OV_D5_PIN>>OV_D5_PinNo)&(0x01));
	  newByte = (newByte << 1)|((OV_D4_PIN>>OV_D4_PinNo)&(0x01));
	  newByte = (newByte << 1)|((OV_D3_PIN>>OV_D3_PinNo)&(0x01));
	  newByte = (newByte << 1)|((OV_D2_PIN>>OV_D2_PinNo)&(0x01));
	  newByte = (newByte << 1)|((OV_D1_PIN>>OV_D1_PinNo)&(0x01));
	  newByte = (newByte << 1)|((OV_D0_PIN>>OV_D0_PinNo)&(0x01));
	
	  return newByte;

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