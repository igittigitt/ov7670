/*
 * TWI.h
 *
 * Created: 10.04.2018 23:39:47
 *  Author: David
 */ 


#ifndef TWI_H_
#define TWI_H_

//definitions:	
	//address at the bus (7-Bit in the Most significant Bits. The LSB of the Byte indicates a write or Read transmission
	char OwnAddress = 0x10;



//Functions:

/**
 * \brief 
 * This function initialize the TWI-Interface
 * 
 * \return void
 */
void init_TWI();	
bool write(char Sla_Write_Adress, char data);
#endif /* TWI_H_ */