/*
 * OV7670_with_Fifo.h
 *
 * Created: 17.08.2018 23:45:39
 *  Author: David
 */ 


#ifndef OV7670_WITH_FIFO_H_
#define OV7670_WITH_FIFO_H_


//Pin definitions
	#define OV_WRST_PORT	PORTC
	#define OV_WRST_PIN		PINC
	#define OV_WRST_DDR		DDRC
	#define OV_WRST_PinNo	3

	#define OV_RRST_PORT	PORTC
	#define OV_RRST_PIN		PINC
	#define OV_RRST_DDR		DDRC
	#define OV_RRST_PinNo	2

	#define OV_WR_PORT		PORTB
	#define OV_WR_PIN		PINB
	#define OV_WR_DDR		DDRB
	#define OV_WR_PinNo		4

	#define OV_VSync_PORT	PORTC
	#define OV_VSync_PIN	PINC
	#define OV_VSync_DDR	DDRC
	#define OV_VSync_PinNo	1

	#define OV_HREF_PORT	PORTC
	#define OV_HREF_PIN		PINC
	#define OV_HREF_DDR		DDRC
	#define OV_HREF_PinNo	0

	#define OV_D0_PORT		PORTB
	#define OV_D0_PIN		PINB
	#define OV_D0_DDR		DDRB
	#define OV_D0_PinNo		0

	#define OV_D1_PORT		PORTB
	#define OV_D1_PIN		PINB
	#define OV_D1_DDR		DDRB
	#define OV_D1_PinNo		1

	#define OV_D2_PORT		PORTB	
	#define OV_D2_PIN		PINB
	#define OV_D2_DDR		DDRB
	#define OV_D2_PinNo		2

	#define OV_D3_PORT		PORTB
	#define OV_D3_PIN		PINB
	#define OV_D3_DDR		DDRB
	#define OV_D3_PinNo		3

	#define OV_D4_PORT		PORTD
	#define OV_D4_PIN		PIND
	#define OV_D4_DDR		DDRD
	#define OV_D4_PinNo		4

	#define OV_D5_PORT		PORTD
	#define OV_D5_PIN		PIND
	#define OV_D5_DDR		DDRD
	#define OV_D5_PinNo		5

	#define OV_D6_PORT		PORTD
	#define OV_D6_PIN		PIND
	#define OV_D6_DDR		DDRD
	#define OV_D6_PinNo		6

	#define OV_D7_PORT		PORTD
	#define OV_D7_PIN		PIND
	#define OV_D7_DDR		DDRD
	#define OV_D7_PinNo		7

	#define OV_RCK_PORT		PORTD
	#define OV_RCK_PIN		PIND
	#define OV_RCK_DDR		DDRD
	#define OV_RCK_PinNo	2

	#define OV_OE_PORT		PORTB
	#define OV_OE_PIN		PINB
	#define OV_OE_DDR		DDRB
	#define OV_OE_PinNo		5

	#define OV_SIOC_PORT	PORTC
	#define OV_SIOC_PIN		PINC
	#define OV_SIOC_DDR		DDRC
	#define OV_SIOC_PinNo	5

	#define OV_SIOD_PORT	PORTC
	#define OV_SIOD_PIN		PINC
	#define OV_SIOD_DDR		DDRC
	#define OV_SIOD_PinNo	4




//other variables

extern int PhotoHeight;
extern int PhotoWidth;
extern int BytesPerPixel;

//Functions

int OV7670_checkConnection (void);


//Function which capture an Image in the FrameBuffer
	char OV7670_init (void);
	void OV7670_captureNewImage (void);
	char readByte (void);
	char getValueOfPin (char thePort, char thePinNo);
	char OV7670_getByte (void);
	void sendFrameBufferToUART (int ImageWidth, int ImageHeight, int BytesPerPixel);
	void OV7670_sendNextLine(int width, int BytesPerPixel);
	void OV7670_sendLineRepeat(int imageLineToRepeat, int width, int BytesPerPixel);

#endif /* OV7670_WITH_FIFO_H_ */