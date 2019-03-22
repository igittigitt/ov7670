/*
 * SCCB_Old.c
 *
 * Created: 22.08.2018 22:46:57
 *  Author: David
 */ 
#include "SCCB_Old.h"
#include "OV7670_with_Fifo.h"
#include <avr/io.h>
#include <util/delay.h>
#include "UART.h"


 /**
  * \brief 
  * this function initializes the SCCB and returns an error code if something goes wrong
  * \return char
  * 0: success
  * 100+n: Error, n= Errorcode of subfunctions 
  */
 char OV_SCCB_Init (void){
	char ErrorCode;

	// Initialisieren der Pins und Festlegung der Datenrichtung
	OV_SIOD_DDR	|= (1<<OV_SIOD_PinNo);
	OV_SIOC_DDR	|= (1<<OV_SIOC_PinNo);
	OV_SIOC_PORT	|= (1<<OV_SIOC_PinNo);
	OV_SIOD_PORT	|= (1<<OV_SIOD_PinNo);

	//Software Reset
		//read Register required to prevent errors
		char temp;
		OV7670_read(OV_SCCB_COM7,&temp);
		//write the Reset Bit
	if(!(ErrorCode=OV7670_write(OV_SCCB_COM7,0x80))){
		//UART0_senden("init SCCB ...done!");
		//UART0_senden_newLine();
		_delay_ms(500);	//Delay of 500ms is necessary
		return ErrorCode;
	}
	else{
		//UART0_senden("Reset SCCB... failed, ErrorCode: ");
		//UART0_senden_Byte(ErrorCode);
		//UART0_senden_newLine();
		return 10+ErrorCode;
	}
}

 void OV_SCCB_Start (void){
	 
	 //Sicherstellen, dass beide Pins auf HIGH sind
	 OV_SIOD_PORT	|=	(1<<OV_SIOD_PinNo);
	 _delay_us(OV_SIO_CLKDELAY);
	 OV_SIOC_PORT	|=	(1<<OV_SIOC_PinNo);
	 _delay_us(OV_SIO_CLKDELAY);

	 //Starten das Start-Prozesses laut Datenblatt
	 OV_SIOD_PORT	&= ~(1<<OV_SIOD_PinNo);
	 _delay_us(OV_SIO_CLKDELAY);
	 OV_SIOC_PORT	&=	~(1<<OV_SIOC_PinNo);

 }

 void OV_SCCB_Stopp (void){
	 

	 //Sicherstellen, dass beide Pins auf LOW sind
	 OV_SIOC_PORT	&= ~(1<<OV_SIOC_PinNo);
	 OV_SIOD_PORT	&= ~(1<<OV_SIOD_PinNo);
	 _delay_us(OV_SIO_CLKDELAY);
	 //Starten des Stop Prozesses laut Datenblatt
	 OV_SIOC_PORT	|=	(1<<OV_SIOC_PinNo);
	 _delay_us(OV_SIO_CLKDELAY);
	 OV_SIOD_PORT	|=	(1<<OV_SIOD_PinNo);
	 _delay_us(OV_SIO_CLKDELAY);
	 
 }

 char OV_SCCB_Write (char Data){
	 char Errorcode

	 //Alle 8 Bit des Data Byte ?bermitteln, MSB zuerst
	 for(int j=7; j>=0;j--){
		 if((Data>>j)	&	0x01){
			 OV_SIOD_PORT	|=	(1<<OV_SIOD_PinNo);
		 }
		 
		 else{
			 OV_SIOD_PORT	&=	~(1<<OV_SIOD_PinNo);
		 }
		 _delay_us(OV_SIO_CLKDELAY);
		 OV_SIOC_PORT |=	(1<<OV_SIOC_PinNo);
		 _delay_us(OV_SIO_CLKDELAY);
		 OV_SIOC_PORT &=	~(1<<OV_SIOC_PinNo);
		 _delay_us(OV_SIO_CLKDELAY);
	 }

	 // Don't Care Bit ?bermitteln, SIOD auf LOW ziehen, um einen undefinierten Status zu vermeiden, Au?erdem wird der PIN auf eingang gestellt um eine Hohe Impedanz zu bekommen.
	 //Wenn der Slave das Byte akzeptiert wird SIOD auf low gezogen
	 OV_SIOD_PORT	&=	~(1<<OV_SIOD_PinNo);
	 OV_SIOD_DDR		&=	~(1<<OV_SIOD_PinNo);

	 _delay_us(OV_SIO_CLKDELAY);
	 OV_SIOC_PORT	|=	(1<<OV_SIOC_PinNo);
	 _delay_us(OV_SIO_CLKDELAY);

	 //Auslesen ob die ?bertragung erfolgreich war
	 if(OV_SIOD_PIN & (1<<OV_SIOD_PinNo)){
		 Errorcode=0x15;
	 }
	 else{
		 Errorcode=0;
	 }

	 OV_SIOC_PORT	&=	~(1<<OV_SIOC_PinNo);
	 _delay_us(OV_SIO_CLKDELAY);
	 OV_SIOD_DDR		|=	(1<<OV_SIOD_PinNo);

	 return Errorcode;
 }

 char OV_SCCB_Read	(char* Data){

	 //DDR auf Eingang schalten
	 OV_SIOD_DDR		&=	~(1<<OV_SIOD_PinNo);
	 OV_SIOD_PORT	&=	~(1<<OV_SIOD_PinNo);	//Pull-Up Deaktivieren

	 for(unsigned char j=0; j<8;j++){
		 _delay_us(OV_SIO_CLKDELAY);
		 OV_SIOC_PORT	|=	(1<<OV_SIOC_PinNo);
		 
		 if(OV_SIOD_PIN & (1<<OV_SIOD_PinNo)){
			 *Data = (*Data<<1)	|	0x01;
		 }
		 else{
			 *Data = (*Data<<1) & ~(0x01);
		 }
		 _delay_us(OV_SIO_CLKDELAY);
		 OV_SIOC_PORT	&=	~(1<<OV_SIOC_PinNo);
		 _delay_us(OV_SIO_CLKDELAY);
	 }

	 //Die ersten 8 Bits wurden gelesen, jetzt folgt das letzte Don't Care Bit
	 //SIOD muss w?hrend dessen auf 1 gehalten werden
	 OV_SIOD_DDR	|=	(1<<OV_SIOD_PinNo);
	 OV_SIOD_PORT	|=	(1<<OV_SIOD_PinNo);

	 _delay_us(OV_SIO_CLKDELAY);
	 OV_SIOC_PORT	|=	(1<<OV_SIOC_PinNo);
	 _delay_us(OV_SIO_CLKDELAY);
	 OV_SIOC_PORT	&=	~(1<<OV_SIOC_PinNo);
	 _delay_us(OV_SIO_CLKDELAY);

	 //Aufr?umen und SIOD auf null setzten (ist das notwendig? ich wei? es nicht)
	 OV_SIOD_PORT	&=	~(1<<OV_SIOD_PinNo);
	 return 1;
 }

 char OV7670_read (char regID, char* regData){
	//This function reads the Value of a Register (regID) of the SCCB and writes its Value to the input Byte (regData)
	//return Value: 0 if everything works, error code >0 if not. 

	 char ErrorCode = 0x00;
	 
	 OV_SCCB_Start();
	 //2 Write Phasen notwendig: Write the IP Address for the right register
	 //Phase1: Write Register ansprechen
	 if(OV_SCCB_Write(OV_SCCB_write_Addr)==0){
		 ErrorCode=0x01;
		 OV_SCCB_Stopp();
		 return ErrorCode;
	 }
	 _delay_us(OV_SIO_CLKDELAY);

	 //Phase2: Write Adress ansprechen
	 if(OV_SCCB_Write(regID)==0){
		 ErrorCode=0x02;
		 OV_SCCB_Stopp();
		 return ErrorCode;
	 }
	 _delay_us(OV_SIO_CLKDELAY);


	 //Finish the First Tarnsmission (2 Phase Write)
	 OV_SCCB_Stopp();

	 _delay_us(OV_SIO_CLKDELAY);

	 //Start the Second Part (2 Phase Read Cycle)
	 //2 Read Phasen notwendig
	 //Phase 1: ID des Leseregisters
	 OV_SCCB_Start();
	 if(OV_SCCB_Write(OV_SCCB_read_Addr)==0){
		 ErrorCode=0x03;
		 OV_SCCB_Stopp();
		 return ErrorCode;
	 }
	 _delay_us(OV_SIO_CLKDELAY);

	 //Phase 2: Data Einlesen
	 if(OV_SCCB_Read(regData)==0){
		 ErrorCode=0x04;
		 OV_SCCB_Stopp();
		 return ErrorCode;
	 }
	 //Beenden
	 OV_SCCB_Stopp();
	 return ErrorCode;
 }

 char OV7670_write	(char regID, char regData){
	//This function writes a Data Byte (regData) to a Register (regID) of the SCCB
	//return Value: 0 if everything works, error code >0 if not. 
	
	
	char ErrorCode =0x00;
	
	OV_SCCB_Start();

	//Phase1: Write Register ansprechen
	if(OV_SCCB_Write(OV_SCCB_write_Addr)==0){
		ErrorCode = 0x05;
		OV_SCCB_Stopp();
		return ErrorCode;
	}

	_delay_us(OV_SIO_CLKDELAY);

	//Phase 2: UnterRegister des SCCB ansprechen
	if(OV_SCCB_Write(regID)==0){
		ErrorCode= 0x06;
		OV_SCCB_Stopp();
		return ErrorCode;
	}

	_delay_us(OV_SIO_CLKDELAY);

	//Phase 3: Daten in Register schreiben
	if(OV_SCCB_Write(regData)==0){
		ErrorCode =0x07;
		OV_SCCB_Stopp();
		return ErrorCode;
	}

	OV_SCCB_Stopp();
	return ErrorCode;
}

/**
 * \brief 
 *  This function sets the test pattern
	if the Input Char is more then 2 Bit all other Bytes are masked
 * \param Pattern

	char Pattern:
	00:	No test output
	01:	Shifting "1"
	10:	8-bar color bar
	11:	Fade to gray color bar
 * 
 * \return void
 */
char OV_SCCB_setTestPattern(char Pattern)
{
	char Errorcode;
	Pattern = (Pattern & 0x03);	//Mask all other Bits
	char Pattern1 = ((Pattern & 0x02)>>1);
	char Pattern2 = (Pattern & 0x01);
	char tempData =0x00;

	//Write first bit of the Pattern
		//The Pattern-Bits are divided in two Register
		Errorcode = OV7670_read(OV_SCCB_SCALING_XSC,&tempData);
		tempData |= (Pattern1<<7);	//The 7Bit of the Register is the Pattern Bit
		tempData &= ((Pattern1<<7) | 0x7F);	//Mask all other Bits to ensure they will not be modifyed
		Errorcode = OV7670_write(OV_SCCB_SCALING_XSC,tempData);

	//Write second bit of the Pattern
		Errorcode = OV7670_read(OV_SCCB_SCALING_YSC,&tempData);
		tempData |= (Pattern2<<7);//The 7Bit of the Register is the Pattern Bit
		tempData &= ((Pattern2<<7) | 0x7F);//Mask all other Bits to ensure they will not be modifyed
		Errorcode = OV7670_write(OV_SCCB_SCALING_YSC,tempData);
		

	//Enable Colorbar
	Errorcode = OV_SCCB_set_Bit(1,OV_SCCB_COM7);
	//Enable DSP Colorbar
	Errorcode = OV_SCCB_set_Bit(3,OV_SCCB_COM17);

		
	return Errorcode;
}

/**
 * \brief 
 * This function read a RegisterValue and send this Value in Hex format to the UART
 * \param RegisterAddress
 * RegisterAddress: Address of the register where the value should be sent 
 * \return void
 */
void OV_SCCB_RegisterToUart(char RegisterAddress){
	char tempByte = 0x00;
	char ErrorByte;
	if(!(ErrorByte=OV7670_read(RegisterAddress,&tempByte)))
		UART0_senden_Byte(tempByte);
	else
	{
		UART0_senden("Error: ");
		UART0_senden_Byte(ErrorByte);
	}
}

char OV_SCCB_setVideoFormat(char Resolution, char ColorFormat){

char writeData = (Resolution | ColorFormat);
char ErrorCode=0;
char temp;

//enable scaling. For Scaling COM3 and COM14 must be modified!
//in COM3 the Scaling Bit [3] must be set to 1
ErrorCode=OV_SCCB_set_Bit(3,OV_SCCB_COM3);
if(ErrorCode)
	return ErrorCode;
//if(ErrorCode=OV_SCCB_set_Bit(2,OV_SCCB_COM3))		//dcw Enable... no Idea what that is
	//return ErrorCode;

temp=0x00;
//Also COM14 Bit 3 must be set to 1 to enable manual adjustment of the format
ErrorCode=OV7670_read(OV_SCCB_COM14,&temp);
if(ErrorCode){
	return ErrorCode;
}
temp |= 0x08;

ErrorCode = OV7670_write(OV_SCCB_COM14,temp);
if(ErrorCode){
	return ErrorCode;
}
temp=0x00;
//Set the video format
ErrorCode = OV7670_read(OV_SCCB_COM7,&temp);
if(ErrorCode)
	{
	UART0_senden("Fehler! Errorcode: ");
	UART0_senden_Byte(ErrorCode);
	return ErrorCode;
	}

temp &= (writeData | 0xC2);
temp |= (writeData & ~(0xC2));

ErrorCode = OV7670_write(OV_SCCB_COM7,temp);
if(ErrorCode)
	{
		return ErrorCode;
		UART0_senden("Fehler! Errorcode: ");
		UART0_senden_Byte(ErrorCode);
	}

//set RGB to RGB 565	(COM 15 Bit[5:4]: 01)
ErrorCode = OV_SCCB_cle_Bit(5,OV_SCCB_COM15);
if(ErrorCode)
	return ErrorCode;

ErrorCode = OV_SCCB_set_Bit(4,OV_SCCB_COM15);
if(ErrorCode)
	return ErrorCode;
return ErrorCode;
}


/**
 * \brief 
 * This function sets the Bit of a Register 
 * \param BitNo	The Bit-Number in the Register (0-7)
 * \param RegisterAddress	The Register Address of the SCCB Register
 * 
 * \return char	 Errorcode = 0 when no Error occures
 */
char OV_SCCB_set_Bit(char BitNo, char RegisterAddress)
{
	char readData;
	char WriteData;
	char ErrorCode=0;

	//read the current SCCB-Register Value
	ErrorCode = OV7670_read(RegisterAddress,&readData);
	if(ErrorCode)
		return ErrorCode;
	//Add the Bit which should be set (Param. BitNo)
	WriteData = (readData | (1<<BitNo));

	//Write the new Register Value
	ErrorCode = OV7670_write(RegisterAddress,WriteData);
	if(ErrorCode)
		return ErrorCode;
	//Read the RegisterValue again
	ErrorCode = OV7670_read(RegisterAddress,&readData);
	if(ErrorCode)
		return ErrorCode;
	//Check if the change is fullfilled
	if(!readData == WriteData)
		ErrorCode=0x10;
	//return the ErrorCode (0 when no Error, 0x10 when Change is not fullfilled)
	return ErrorCode;
}

/**
 * \brief 
 * This function clears the Bit of a Register 
 * \param BitNo	The Bit-Number in the Register (0-7)
 * \param RegisterAddress	The Register Address of the SCCB Register
 * 
 * \return char	 Errorcode = 0 when no Error occures
 */
char OV_SCCB_cle_Bit(char BitNo, char RegisterAddress)
{
	char readData;
	char WriteData;
	char ErrorCode=0;

	//read the current SCCB-Register Value
	ErrorCode = OV7670_read(RegisterAddress,&readData);
	if(ErrorCode)
		return ErrorCode;
	//Clear the Bit which should be cleared (Param. BitNo)
	WriteData = (readData & ~(1<<BitNo));

	//Write the new Register Value
	ErrorCode = OV7670_write(RegisterAddress,WriteData);
	if(ErrorCode)
		return ErrorCode;

	//Read the RegisterValue again
	ErrorCode = OV7670_read(RegisterAddress,&readData);
	if(ErrorCode)
		return ErrorCode;
	//Check if the change is fullfilled
	if(!readData == WriteData)
		ErrorCode=0x11;
	//return the ErrorCode (0 when no Error, 0x10 when Change is not fullfilled)
	return ErrorCode;
}


/* To implemented
void scanForI2CDevice (void)
 {
	 int nConnected = 0;
	 char ErrorCode, DeviceAddress;
	 ErrorCode=99;

	 for(DeviceAddress=1; DeviceAddress<=127; DeviceAddress++)
	 {
		 //Begin I2C Transmission
		 //SCCB_StartTransmission(DeviceAddress);
		 //Get the device respond of the Device
		 //ErrorCode = SCCB_EndTransmission();
		 
		 if(ErrorCode==0)
		 {
			 //Device Detected
			 UART0_senden("Address:");
			 UART0_senden_Byte( DeviceAddress);

			 nConnected++;
		 }
		 else if(ErrorCode==4)
		 {
			 //Unknown Error
			 UART0_senden("unknown Error");
		 }
		 _delay_ms(200);
		
	 }

	 if(nConnected == 0)
	 {
		 UART0_senden("no Devices connected");
	 }
	 else
	 {
		 UART0_senden("Done!\n");
	 }
 }
 */