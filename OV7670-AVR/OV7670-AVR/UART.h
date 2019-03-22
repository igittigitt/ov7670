/*
 * UART.h
 *
 * Created: 06.06.2016 20:52:56
 *  Author: David
 */ 


#ifndef UART_H_
	#define UART_H_

#ifndef F_CPU
 #define F_CPU 16000000UL
#endif

extern char receivedAddress;
extern char receivedData1;
extern char receivedData2;

void UART0_init (void);
void UART0_senden (char input[16]);
void UART0_senden_newLine ();
int UART0_rx_in (char input);
char UART0_rx_out (void);
int UART0_rx_empty(void);
int UART0_rx_complete(void);
int UART0_rx_work(int* Programmstatus);;

int UART0_tx_in (char input);
char UART0_tx_out (void);
int UART0_tx_empty(void);
void UART0_senden_zahl(long zahl);
void UART0_senden_Byte(char Byte);
#endif /* UART_H_ */