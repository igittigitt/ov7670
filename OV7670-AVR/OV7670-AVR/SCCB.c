/*
 * SCCB.c
 *
 * Created: 10.04.2018 23:39:35
 *  Author: David
 *
 *	Important informations:
 *	- make sure to install a pull-up resistor on each Bus-Line (SCL and SDA). The Resistor value depends on the SCL frequency, for more information take a look in the data sheet
 *    (e.g. Atmega 328P Page 222) (min. 866 Ohm by VCC=3V and min. 1,5kOhm by VCC=5V)
 *
 */ 

 #include "SCCB.h"
 #include <util/twi.h>
 #include <avr/common.h>
#include <avr/io.h>
#include <avr/delay.h>
#include "OV7670_with_Fifo.h"

 //generate en enumerated operating Mode, which can later easy be checked
 typedef enum {
	 Ready,
	 Initializing,
	 RepeatedStartSent,
	 MasterTransmitter,
	 MasterReceuver,
	 SlaveTransmitter,
	 SlaveReciever
 } TWIMode;
 

unsigned char OV7670_Address = 0x21;
		
	//address at the bus (7-Bit in the Most significant Bits. The LSB of the Byte indicates a write or Read transmission
unsigned char OwnAddress = 0x10;

 void init_TWI()
 {
	//Set the TWI pins to a defined state
	OV_SIOC_DDR	 |= (1<<OV_SIOC_PinNo);
	OV_SIOC_PORT |= (1<<OV_SIOC_PinNo);
	OV_SIOD_DDR  |= (1<<OV_SIOD_PinNo);
	OV_SIOD_PORT |= (1<<OV_SIOD_PinNo);

	/*Set the SCL frequency
	* notice: the SCL frequency have to be at least 16 times lower than the clock of the slowest slave
	* In this case we assume, that the slowest slave has a frequency of 8Mhz, which mean that the SCL-Frequency must be lower that 500Khz
	* f_SCL = f_CPU/(16+2*(TWBR)*Prescaler)
	* f_SCL = 16Mhz/(16+2*(TWBR)*Prescaler)
	* So we set TWBR to 0x09 and the prescaler to 1 and get a f_SCL of about 470 KHz
	*/
	//set the Bit Rate Register to the calculated value
	TWBR=0x09;
	//TWBR=0x48;
	//set the Prescaler
	/*
	*	TWPS1	TWPS0	Prescaler Value
	*	0		0		1	
	*	0		1		4
	*	1		0		16
	*	1		1		64
	*/
	TWSR &= ~( (1<<TWPS1) | (1<<TWPS0) ); //clear both Prescaler bits to get a Prescaler value of 1


	//Each participant of the bus should have an unique address, which in case of the AVR-TWI-Interface is set in the TwoWireAddressRegister (TWAR)
	TWAR = OwnAddress;
	UART0_senden("Init done!\n");
	_delay_ms(20);
 }

 void TWI_start(void){
 //To activate the TWI-Interface the Interrupt Flag is cleared by writing a 1 to it, the start condition is send and the 2-Wire Interface is activated
 TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTA);


 while(!(TWCR & (1<<TWINT)));
 	//_delay_us(200);
 //Place for Checking Ack/NACK
  //check if the start condition was send correctly
  if((TWSR & 0xF8) != TW_START)	//Mask 0xF8 to mask the Prescaler Bits
  {
  //Error treatment
   UART0_senden("Start Err");
  }
 }

 void TWI_stop(void){
 //Send a Stop condition and drop controll of the Bus
	 TWCR = (1<<TWINT) | (1<<TWSTO) | (1<<TWEN);
	_delay_us(20);
 }

 /**
  * \brief 
  * This function implements a 3-Phase write transmission cycle according to the Data-Sheet of the SCCB-Interface
  *	Phase1: ID-Address of the Slave, Phase2: SubAddress, Phase3: Write Data
  * \param SubAddress
  * \param data
  * 
  * \return bool
  *
  *	checklist: 
  *	check the Ack-Bit settings according to the Datasheet
  *
  *
  *
  */

unsigned char sccb_start(unsigned char SlaveAddress)
{
//Perform a start Condition
return '0';
}

 int sccb_write_3_phase(char SubAddress, char data)
 {
	/*
	delete the TWINT by writing a logical 1 to it
	*set the start condition on the bus
	*enable Two-Wire
	*/
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);

	//Wait until interrupt Flag is set, which means that the start condition is send
	while(!(TWCR & (1<<TWINT)));

	//check if the start condition was send correctly
	if((TWSR & 0xF8) != TW_START)	//QUESTION: why do we MASK this with 0xF8 and not with TW_START?
 		{
// 			//Error treatment
// 			return 0;
 		}

	//1st-Phase
	//insert the slave write address into the Two-Wire Data Register and indicate a write transmission by setting the LSB to a logical 0
	TWDR = (OV7670_Address<<1) & ~(0x01);

	//start the transmission on the bus
	TWCR = (1<<TWINT) | (1<<TWEN);

	//Wait until interrupt Flag is set, which means that the data is send
	while(!(TWCR & (1<<TWINT)));

	//check if Acknowledge Bit is send by slave
	if((TWSR & 0xF8) != TW_MT_SLA_ACK)	//Mask with 0xF8 to eliminate the Prescaler Bits
	{
		//Error Treatment
	
	}

	//2nd-Phase
	//write the Sub-Address to the Two-Wire Data Register
	TWDR= SubAddress;

	//start the transmission on the bus
	TWCR = (1<<TWINT) | (1<<TWEN);

	//Wait until interrupt Flag is set, which means that the data is send
	while(!(TWCR & (1<<TWINT)));

	//check if Acknowledge Bit is send by slave
	if((TWSR & 0xF8) != TW_MT_SLA_ACK)	//Mask with 0xF8 to eliminate the Prescaler Bits
 	{
//	//Error Treatment
// 		return 0;
 	}

	//3rd-Phase
	//write the data to the Two-Wire Data Register
	TWDR= data;

	//start the transmission on the bus
	TWCR = (1<<TWINT) | (1<<TWEN);

	//Wait until interrupt Flag is set, which means that the data is send
	while(!(TWCR & (1<<TWINT)));

	//check if Acknowledge Bit is send by slave
 	if((TWSR & 0xF8) != TW_MT_SLA_ACK)	//Mask with 0xF8 to eliminate the Prescaler Bits
 	{
// 		//Error Treatment
// 		return 0;
 	}

	//send Stop Condition
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
	return 1;
 }

 void sccb_write_2_phase(char SubAddress){
 
	TWI_start(); //initiate a Transmittion

 //1st-Phase
 //insert the slave write address into the Two-Wire Data Register and indicate a write transmission by setting the LSB to a logical 0
 TWDR = (OV7670_Address<<1) & ~(0x01);

 //start the transmission on the bus
 TWCR = (1<<TWINT) | (1<<TWEN);

 //Wait until interrupt Flag is set, which means that the data is send
 while(!(TWCR & (1<<TWINT)));

	if((TWSR &0xF8 != 0x20) && (TWSR &0xF8 != 0x18))	//Transmit SLA+W is indicated with TWSR 0x18 (ACK received) or 0x20 (No ACK received) In Case of the SCCB it is not necessary to check the ACK Bit
	 {
		UART0_senden("Tx Err");
	 }
	 _delay_us(20);

 //check if Acknowledge Bit is send by slave
// if((TWSR & 0xF8) != TW_MT_SLA_ACK)	//Mask with 0xF8 to eliminate the Prescaler Bits
 //{
 ////Error Treatment
 //return false;
 //}
 
 //2nd-Phase
 //write the Sub-Address to the Two-Wire Data Register
 TWDR= SubAddress;

 //start the transmission on the bus
 TWCR = (1<<TWINT) | (1<<TWEN);

 //Wait until interrupt Flag is set, which means that the data is send
 while(!(TWCR & (1<<TWINT)));
 if((TWSR &0xF8 != 0x28) && (TWSR &0xF8 != 0x30))	//Transmit Data is indicated with TWSR 0x28 (ACK received) or 0x30 (No ACK received) In Case of the SCCB it is not necessary to check the ACK Bit
 {
	UART0_senden("Tx Err");
 }
	TWI_stop();
	
 }

 int sccb_read_2_phase(char* readData){
 	/*
	*	According to the Data sheet of the SCCB-Interface a Read action is 
	*	fulfilled by a 2-Phase Write transmission followed by a 2-Phase read Transmission
	*/

	TWI_start(); //get controll of the Bus

	//1st-Phase
	//insert the slave read address into the Two-Wire Data Register and indicate a read transmission by setting the LSB to a logical 1
	TWDR = (OV7670_Address<<1) | (0x01);

	//start the transmission on the bus
	TWCR = (1<<TWINT) | (1<<TWEN);

	//Wait until interrupt Flag is set, which means that the data is send
	while(!(TWCR & (1<<TWINT)));


	if((TWSR &0xF8 != 0x20) && (TWSR &0xF8 != 0x18))	//Transmit SLA+W is indicated with TWSR 0x18 (ACK received) or 0x20 (No ACK received) In Case of the SCCB it is not necessary to check the ACK Bit
	 {
		UART0_senden("Tx Err");
	 }
	 		_delay_us(20);

	TWCR = (1<<TWINT) | (1<<TWEN);
	while(!(TWCR & (1<<TWINT)));
	*readData = TWDR;
		
	
	TWI_stop();		//drop controll of the Bus
	/*
	//old Code should be removed after getting it work
	//2nd-Phase
	//write the Sub-Address to the Two-Wire Data Register
	TWDR= SubAddress;

	//start the transmission on the bus
	TWCR = (1<<TWINT) | (1<<TWEN);

	//Wait until interrupt Flag is set, which means that the data is send
	while(!(TWCR & (1<<TWINT)));

	//check if Acknowledge Bit is send by slave
	if((TWSR & 0xF8) != TW_MT_SLA_ACK)	//Mask with 0xF8 to eliminate the Prescaler Bits
	{
		//Error Treatment
		return false;
	}
	*/
 }
 

 //This function Scan the I2C/TWI Bus for connected devices.
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

 char sccb_readRegister(char RegisterAddress)
 {
 char RegisterValue;
 sccb_write_2_phase(RegisterAddress);
 sccb_read_2_phase(&RegisterValue);

 return RegisterValue;
 }

 void sccb_writeRegister(char RegisterAddress, char RegisterValue){
 sccb_write_3_phase(RegisterAddress,RegisterValue);

 }