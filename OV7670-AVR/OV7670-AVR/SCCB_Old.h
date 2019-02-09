/*
 * SCCB_Old.h
 *
 * Created: 22.08.2018 22:47:25
 *  Author: David
 */ 


#ifndef SCCB_OLD_H_
	#define SCCB_OLD_H_

#ifndef F_CPU
	#define F_CPU 16000000UL
#endif
//Register addresses

	#define OV_SCCB_Addr		0x21
	#define OV_SCCB_write_Addr	0x42
	#define OV_SCCB_read_Addr	0x43
	#define OV_SCCB_PID			0x0A
	#define OV_SCCB_CLKRC		0x11
	#define OV_SCCB_COM7		0x12
	#define OV_SCCB_COM3		0x0C
	#define OV_SCCB_COM14		0x3E
	#define OV_SCCB_COM10		0x15
	#define OV_SCCB_SCALING_PCLK	0x73
	#define OV_SCCB_SCALING_XSC	0x70
	#define OV_SCCB_SCALING_YSC	0x71
	
	#define OV_SCCB_COM15		0x40
	#define OV_SCCB_COM17		0x42




//Defaultvalues
#define OV_SCCB_PID_default	0x76


//Timing definitionen
#define OV_SIO_CLKDELAY		200

//ResulutionFormats
	#define CIF		0x20
	#define QVGA	0x10
	#define QCIF	0x08
//ColorFormats
	#define YUV		0x00
	#define RGB		0x04
	#define RAW		0x01
	#define processedRAW	0x05


//Functions
char OV_SCCB_Init (void);
void OV_SCCB_Start (void);
void OV_SCCB_Stopp	(void);
char OV_SCCB_Write	(char Data);
char OV_SCCB_Read	(char* Data);
char OV7670_write	(char regID, char regData);
char OV7670_read (char regID, char* regData);
char OV_SCCB_setTestPattern(char Pattern);
void OV_SCCB_RegisterToUart(char RegisterAddress);

char OV_SCCB_setVideoFormat(char Resolution, char ColorFormat);
char OV_SCCB_set_Bit(char BitNo, char RegisterAddress);
char OV_SCCB_cle_Bit(char BitNo, char RegisterAddress);

char OV_SCCB_setTestPattern(char Pattern);
void OV_SCCB_RegisterToUart(char RegisterAddress);

#endif /* SCCB_OLD_H_ */