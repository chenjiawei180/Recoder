#ifndef __USART_H
#define __USART_H

#include "STC12C5A60S2.H"
#include<stdio.h>  
#include<stdarg.h> 
#include "tpyedef.h"

#define FOSC 32768000L      //System frequency
#define BAUD 9600           //UART baudrate

/*Define UART parity mode*/
#define NONE_PARITY     0   //None parity
#define ODD_PARITY      1   //Odd parity
#define EVEN_PARITY     2   //Even parity
#define MARK_PARITY     3   //Mark parity
#define SPACE_PARITY    4   //Space parity

#define PARITYBIT NONE_PARITY   //Testing even parity



extern void usart_init(void);
extern void SendData(BYTE dat);
extern void SendString(char *s);
extern void uart_printf(const char *fmt, ...);


#endif