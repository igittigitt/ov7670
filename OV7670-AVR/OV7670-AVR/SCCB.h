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
#ifndef F_CPU
 #define F_CPU 16000000UL
#endif



//Functions:

/**
 * \brief 
 * This function initialize the TWI-Interface
 * 
 * \return void
 */
void init_TWI();	

void TWI_start(void);
void TWI_stop(void);
int sccb_write_3_phase(char SubAddress, char data);
void sccb_write_2_phase(char SubAddress);
int sccb_read_2_phase(char* readData);

void scanForI2CDevice (void);

char sccb_readRegister(char RegisterAddress);
void OV7670_read_register (char SubAddress, char* readData);
void OV7670_write_register (char SubAddress, char data);
#endif /* SCCB_H_ */