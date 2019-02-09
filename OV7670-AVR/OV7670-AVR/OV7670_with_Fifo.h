/*
 * OV7670_with_Fifo.h
 *
 * Created: 17.08.2018 23:45:39
 *  Author: David
 */ 


#ifndef OV7670_WITH_FIFO_H_
#define OV7670_WITH_FIFO_H_

#ifndef F_CPU
 #define F_CPU 16000000UL
#endif
//Pin definitions

	#define OV_SIOC_PORT	PORTC
	#define OV_SIOC_PIN		PINC
	#define OV_SIOC_DDR		DDRC
	#define OV_SIOC_PinNo	5

	#define OV_SIOD_PORT	PORTC
	#define OV_SIOD_PIN		PINC
	#define OV_SIOD_DDR		DDRC
	#define OV_SIOD_PinNo	4

	#define OV_VSync_PORT	PORTC
	#define OV_VSync_PIN	PINC
	#define OV_VSync_DDR	DDRC
	#define OV_VSync_PinNo	1

	#define OV_HREF_PORT	PORTC
	#define OV_HREF_PIN		PINC
	#define OV_HREF_DDR		DDRC
	#define OV_HREF_PinNo	0

	#define OV_D7_PORT		PORTD
	#define OV_D7_PIN		PIND
	#define OV_D7_DDR		DDRD
	#define OV_D7_PinNo		7

	#define OV_D6_PORT		PORTD
	#define OV_D6_PIN		PIND
	#define OV_D6_DDR		DDRD
	#define OV_D6_PinNo		6

	#define OV_D5_PORT		PORTD
	#define OV_D5_PIN		PIND
	#define OV_D5_DDR		DDRD
	#define OV_D5_PinNo		5

	#define OV_D4_PORT		PORTD
	#define OV_D4_PIN		PIND
	#define OV_D4_DDR		DDRD
	#define OV_D4_PinNo		4

	#define OV_D3_PORT		PORTB
	#define OV_D3_PIN		PINB
	#define OV_D3_DDR		DDRB
	#define OV_D3_PinNo		3

	#define OV_D2_PORT		PORTB
	#define OV_D2_PIN		PINB
	#define OV_D2_DDR		DDRB
	#define OV_D2_PinNo		2

	#define OV_D1_PORT		PORTB
	#define OV_D1_PIN		PINB
	#define OV_D1_DDR		DDRB
	#define OV_D1_PinNo		1

	#define OV_D0_PORT		PORTB
	#define OV_D0_PIN		PINB
	#define OV_D0_DDR		DDRB
	#define OV_D0_PinNo		0

	#define OV_RST_PORT	PORTB
	#define OV_RST_PIN		PINB
	#define OV_RST_DDR		DDRB
	#define OV_RST_PinNo	5

//not connected in current setup
//	#define OV_PWDN_PORT	PORTx
//	#define OV_PWDN_PIN		PINx
//	#define OV_PWDN_DDR		DDRx
//	#define OV_PWDN_PinNo	x

//not connected in current setup
//	#define OV_STR_PORT		PORTx
//	#define OV_STR_PIN		PINx
//	#define OV_STR_DDR		DDRx
//	#define OV_STR_PinNo	x

	#define OV_RCK_PORT		PORTD
	#define OV_RCK_PIN		PIND
	#define OV_RCK_DDR		DDRD
	#define OV_RCK_PinNo	2

	#define OV_WR_PORT		PORTB
	#define OV_WR_PIN		PINB
	#define OV_WR_DDR		DDRB
	#define OV_WR_PinNo		4

//not connected in current setup
//	#define OV_OE_PORT		PORTx
//	#define OV_OE_PIN		PINx
//	#define OV_OE_DDR		DDRx
//	#define OV_OE_PinNo	x

	#define OV_WRST_PORT	PORTC
	#define OV_WRST_PIN		PINC
	#define OV_WRST_DDR		DDRC
	#define OV_WRST_PinNo	3

	#define OV_RRST_PORT	PORTC
	#define OV_RRST_PIN		PINC
	#define OV_RRST_DDR		DDRC
	#define OV_RRST_PinNo	2

//other variables

extern int PhotoHeight;
extern int PhotoWidth;
extern int BytesPerPixel;

//Functions

char OV7670_init (void);

/**
 * \brief 
 * this functions sets the right data direction of the OV7670 Pins defined in the header file
 */
void Ov7670_initPins (void);

/**
 * \brief 
 * This function check if a connection to a OV7670 module exists. For that purpose register 0x0A is read.
 If the value of the register is 0x76 then a connection is established.

 * \return int
 0: no Device connected
 1: Device connected
 */
char OV7670_checkConnection (void);


void OV7670_captureNewImage (void);

 /**
  * \brief 
  * This function reads the Output Byte of the OV7670 or of the connected Fifo (D1-D7) and returns this value as a character
  * 
  * \return char
  * value of the D0-D7 connected pins. MSB: D7
  */
char Ov7670_readByte (void);

 /**
  * \brief 
  * This function determines the value of a pin
  * \param thePort
  * Port of the requested pin
  * \param thePinNo
  * Pin number of the requested pin
  * \return char
  * 0: if value = low (0)
  * 1: if value = high (1)
  */
char getValueOfPin (char thePort, char thePinNo);

/**
 * \brief 
 *		This function sends the next Image Line from the Fifo Buffer to UART. 
 *		Therefore the ReadBuffer is clocked as often, as the width multiplied by BytesPerPixel.
 *		Due to the slow UART connection a delay function is integrated.
 *		If the applied baud rate of the UART changes. It is may useful or necessary to adjust this delay in order to increase
 *		the performance or in case of decreasing the baud rate to avoid an overflow of the UART buffer.
 * \param width
 *		xResolution of the image
 * \param BytesPerPixel
 *		BytesPerPixel dedicated to the ColorFormat of the Image (e.g. RGB888, RGB565)
 * \return void
 */
void OV7670_sendNextLine(int width, int BytesPerPixel);

/**
 * \brief 
*		This function resets the ReadPointer of the Fifo and clock the readpointer until it reaches the imageLineToRead.
 *		When reached, this Line is send via UART. Due to the slow UART connection a delay function is integrated.
 *		If the applied baud rate of the UART changes. It is may useful or necessary to adjust this delay in order to increase
 *		the performance or in case of decreasing the baud rate to avoid an overflow of the UART buffer.
 * \param imageLineToRepeat
 *		This is the imageLine which should be repeated. (starting with 0)
 * \param width
 *		The width of the Image (xResolution)
 * \param BytesPerPixel
 *		Bytes per Pixel (dedicated on the imageformat (RGB888,RGB565, etc)
 * \return void
 */
void OV7670_sendLineRepeat(int imageLineToRepeat, int width, int BytesPerPixel);

/**
 * \brief 
 * This function resets the write Pointer of the Fifo to 0
 * \return void
 */
void OV7670_ResetFifoWritePointer(void);

/**
 * \brief 
 * This function resets the read Pointer of the Fifo to o
 * \return void
 */
void OV7670_ResetFifoReadPointer(void);

 /**
  * \brief 
  * This function resets the ReadPointer by calling OV7670_ResetFifoReadPointer() and afterwards sending each Byte from the Frame to the UART.
  * For that purpose RCK is clocked after each Byte. Due to the slow UART connection a delay function is integrated. 
  * If the applied baud rate of the UART changes. It is may useful or necessary to adjust this delay in order to increase 
  * the performance or in case of decreasing the baud rate to avoid an overflow of the UART buffer.
  * \param ImageWidth
  * x-Resolution of the image (according to the chosen imager settings)
  * \param ImageHeight
  * y-Resolution of the image (according to the chosen imager settings)
  * \param BytesPerPixel
  * Bytes per pixel according to the chosen imager settings (Color format)
  * \return void
  */
void sendFrameBufferToUART (int ImageWidth, int ImageHeight, int BytesPerPixel);

#endif /* OV7670_WITH_FIFO_H_ */