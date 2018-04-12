/*
 * TWI.c
 *
 * Created: 10.04.2018 23:39:35
 *  Author: David
 *
 *	Important informations:
 *	- make sure to install a pull-up resistor on each Bus-Line (SCL and SDA). The Resistor value depends on the SCL frequency, for more information take a look in the data sheet
 *    (e.g. Atmega 328P Page 222) (min. 866 Ohm by VCC=3V and min. 1,5kOhm by VCC=5V)
 *
 */ 

 #include "TWI.h"
 #include <util/twi.h>
 #include <avr/common.h>

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

 //generate a struct, which includes important information about the current TWI-Interface
 typedef struct {
	TWIMode mode;
	uint8_t errorCode;
	uint8_t repStart;
	uint8_t acknoledExpected;
 } TWIInfoStruct;

 //create an element of the struct
 TWIInfoStruct TWI_Info;


 void init_TWI()
 {
	/*Set the SCL frequency
	* notice: the SCL frequency have to be at least 16 times lower than the clock of the slowest slave
	* In this case we assume, that the slowest slave has a frequency of 8Mhz, which mean that the SCL-Frequency must be lower that 500Khz
	* f_SCL = f_CPU/(16+2*(TWBR)*Prescaler)
	* f_SCL = 16Mhz/(16+2*(TWBR)*Prescaler)
	* So we set TWBR to 0x09 and the prescaler to 1 and get a f_SCL of about 470 KHz
	*/
	//set the Bit Rate Register to the calculated value
	TWBR=0x09;

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
 }

 /**
  * \brief 
  * This function first write the address of the Slave on the bus and after recognizing the Slave it sends the data on the TWI bus
  *
  * \param Sla_Write_Adress
  * \param data
  * 
  * \return bool
  */
 bool write(char Sla_Address, char data)
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
			//Error treatment
			return false;
		}
	//insert the slave write address into the Two-Wire Data Register and indicate a write transmission by setting the LSB to a logical 0
	TWDR = (Sla_Address<<1) & ~(0x01);

	//start the transmission on the bus
	TWCR = (1<<TWINT) | (1<<TWEN);

	//Wait until interrupt Flag is set, which means that the data is send
	while(!(TWCR & (1<<TWINT)));

	//check if Acknowledge Bit is send by slave
	if((TWSR & 0xF8) != TW_MT_SLA_ACK)	//QUESTION: why do we MASK this with 0xF8 and not with TW_MT_SLA_ACK?
	{
		//Error Treatment
		return false;
	}

	//write the data to the Two-Wire Data Register
	TWDR= data;

	//start the transmission on the bus
	TWCR = (1<<TWINT) | (1<<TWEN);

	//Wait until interrupt Flag is set, which means that the data is send
	while(!(TWCR & (1<<TWINT)));

	//check if Acknowledge Bit is send by slave
	if((TWSR & 0xF8) != TW_MT_SLA_ACK)	//QUESTION: why do we MASK this with 0xF8 and not with TW_MT_SLA_ACK?
	{
		//Error Treatment
		return false;
	}

	//send Stop Condition
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);

	return true;
 }

