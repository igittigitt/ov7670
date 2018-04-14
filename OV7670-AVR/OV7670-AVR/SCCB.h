/*
 * SCCB.h
 *
 * Created: 10.04.2018 23:39:47
 *  Author: David
 *
 * Attention: When you use this script, make sure you use the TWI-Bus Pins at your AVR: SCL and SDA
 *
 */ 


#ifndef SCCB_H_
#define SCCB_H_

//definitions:




//Functions:

/**
 * \brief 
 * This function initialize the TWI-Interface
 * 
 * \return void
 */
void init_TWI();	
int write(char Sla_Write_Adress, char data);
int SCCB_Read(char SubAddress, char* readData);
#endif /* SCCB_H_ */