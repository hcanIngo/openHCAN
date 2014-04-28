/*----------------------------------------------------------------------------
 Copyright:      Radig Ulrich  mailto: mail@ulrichradig.de
 Author:         Radig Ulrich
 Remarks:        
 known Problems: none
 Version:        24.10.2007
 Description:    RS232 Routinen

 Dieses Programm ist freie Software. Sie k�nnen es unter den Bedingungen der 
 GNU General Public License, wie von der Free Software Foundation ver�ffentlicht, 
 weitergeben und/oder modifizieren, entweder gem�� Version 2 der Lizenz oder 
 (nach Ihrer Option) jeder sp�teren Version. 

 Die Ver�ffentlichung dieses Programms erfolgt in der Hoffnung, 
 da� es Ihnen von Nutzen sein wird, aber OHNE IRGENDEINE GARANTIE, 
 sogar ohne die implizite Garantie der MARKTREIFE oder der VERWENDBARKEIT 
 F�R EINEN BESTIMMTEN ZWECK. Details finden Sie in der GNU General Public License. 

 Sie sollten eine Kopie der GNU General Public License zusammen mit diesem 
 Programm erhalten haben. 
 Falls nicht, schreiben Sie an die Free Software Foundation, 
 Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA. 
------------------------------------------------------------------------------*/
#include "web/ip_config.h" // fuer DEBUG per USART
#include "avr/pgmspace.h"

#ifdef USE_USART
#ifndef _UART_H
	#define _UART_H

	#define USART_ECHO	1
	
    #define BUFFER_SIZE_USART	 100

	volatile unsigned int buffercounter;
	char usart_rx_buffer[BUFFER_SIZE_USART];
	
	char *rx_buffer_pointer_in;
	char *rx_buffer_pointer_out;
	
	struct {
			volatile unsigned char usart_ready:1;
			volatile unsigned char usart_rx_ovl:1;
			volatile unsigned char usart_disable:1; //ben�tigt f�r ftp2com
			}usart_status ;
	
	//Die Quarzfrequenz auf dem Board (in config.h)
	/*
	#ifndef SYSCLK
			#define SYSCLK 16000000UL
	#endif //SYSCLK	
	*/
	
	//Anpassen der seriellen Schnittstellen Register wenn ein ATMega128 benutzt wird
	#if defined (__AVR_ATmega128__)
		#define USR UCSR0A
		#define UCR UCSR0B
		#define UDR UDR0
		#define UBRR UBRR0L
		#define USART_RX USART0_RX_vect 
	#endif
	
	#if defined (__AVR_ATmega644__) || defined (__AVR_ATmega644P__)
		#define USR UCSR0A
		#define UCR UCSR0B
		#define UBRR UBRR0L
		#define EICR EICRB
		#define TXEN TXEN0
		#define RXEN RXEN0
		#define RXCIE RXCIE0
		#define UDR UDR0
		#define UDRE UDRE0
		#define USART_RX USART0_RX_vect   
	#endif
	
	#if defined (__AVR_ATmega32__)
		#define USR UCSRA
		#define UCR UCSRB
		#define UBRR UBRRL
		#define EICR EICRB
		#define USART_RX USART_RXC_vect  
	#endif
	
	#if defined (__AVR_ATmega8__)
		#define USR UCSRA
		#define UCR UCSRB
		#define UBRR UBRRL
	#endif
	
	#if defined (__AVR_ATmega88__)
		#define USR UCSR0A
		#define UCR UCSR0B
		#define UBRR UBRR0L
		#define TXEN TXEN0
		#define UDR UDR0
		#define UDRE UDRE0
	#endif
	//----------------------------------------------------------------------------
	
	void usart_init(unsigned long baudrate); 
	void usart_write_char(char c);
	void usart_write_str(char *str);
	
	void usart_write_P (const char *Buffer,...);
	#define usart_write(format, args...)   usart_write_P(PSTR(format) , ## args)
	
	//----------------------------------------------------------------------------

#endif //_UART_H
#endif //USE_USART
