/*
 *  This file is part of the HCAN tools suite.
 *
 *  HCAN is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your
 *  option) any later version.
 *
 *  HCAN is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with HCAN; if not, write to the Free Software Foundation, Inc., 51
 *  Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
 *
 *  (c) 2013 by Martin Kramer and Ingo Lages, i (dot) lages (at) gmx (dot) de
 *  based on hcan:
 *  (c) 2006 by Martin Haller, mah (at) iuse (dot) org
 */
#include <avr/wdt.h>
#include <avr/io.h>
#include <avr/interrupt.h> // fuer cli(), sei()
#include <util/delay.h>

#include <stdlib.h>
#include <string.h>

#include "ip_arp_udp_tcp.h"
#include "websrv_help_functions.h"
#include "enc28j60.h"
#include "dhcp_client.h"

#include "usart.h" // fuer DEBUG

#include <canix/canix.h>
#include <canix/led.h>
#include <canix/tools.h>
#include <canix/rtc.h>
#include <canix/syslog.h>
#include <canix/mcp2515.h> // fuer can_disable_mcp2515(), can_enable_mcp2515()
#include <hcan.h>

#include <hms_interface.h> // fuer hauselektrik_callback

#define LEDOFF PORTB|=(1<<PORTB1) // set
#define LEDON PORTB&=~(1<<PORTB1) // set
#define LEDISOFF PORTB&(1<<PORTB1) // check

static uint8_t mymac[6] = {0x54,0x55,0x58,0x10,0x00,0x29};
static uint8_t myStaticIP[4] = {192,168,1,40}; // http://192.168.1.40/
static uint8_t myip[4]= {0,0,0,0}; // My own IP (DHCP will provide a value for it)

#define MYWWWPORT 80 // server listen port for www

#define BUFFER_SIZE 850 // global packet buffer
static uint8_t buf[BUFFER_SIZE+1];

static void handleDHCP (void)
{
	uint8_t rval=0;
    uint16_t plen;

	LEDON;
    // DHCP handling. Get the initial IP (siehe dhcp_client.h)
    init_mac(mymac);
    while(rval==0)
    {
    	wdt_reset();
    	plen=enc28j60PacketReceive(BUFFER_SIZE, buf);
		buf[BUFFER_SIZE]='\0';
		rval=packetloop_dhcp_initial_ip_assignment(buf,plen,mymac[5]);
    }
    // we have an IP:
    dhcp_get_my_ip(myip,0,0); // (0=NULL) we just want the IP, as a web server we do technically not need mask and gateway. Those fiels are only needed for a client.
    DEBUG("\n\rmyIP=");
    char strIP[30];
    mk_net_str(strIP,myip,4,'.',10);
    usart_write_str(strIP);
    DEBUG("-E\n\r");
    LEDOFF;
}

static uint16_t http200ok(void)
{
	/* 	"Content-Type: text/html"
	 	Origin http://localhost is not allowed by Access-Control-Allow-Origin.
	 	Daher ist jsonp und "Content-Type: application/javascript" notwendig. */
	return(fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: application/javascript\r\nPragma: no-cache\r\n\r\n")));
}

static void web_handler(void)
{
    uint16_t plen;
	uint16_t dat_p;

	// handle ping and wait for a tcp packet
	plen=enc28j60PacketReceive(BUFFER_SIZE, buf);
	buf[BUFFER_SIZE]='\0';
	dat_p=packetloop_arp_icmp_tcp(buf,plen);

	// dat_p will be unequal to zero if there is a valid  http get
	if(dat_p==0){
			// no http request
			if (enc28j60linkup()){
					LEDON;
			}else{
					LEDOFF;
			}
			return; //IL-while ist ausserhalb...: continue;
	}
	// tcp port 80 begin
	if (strncmp("GET ",(char *)&(buf[dat_p]),4)!=0){
			// head, post and other methods:
			dat_p=http200ok();
			dat_p=fill_tcp_data_p(buf,dat_p,PSTR("<h1>200 OK</h1>"));
			goto SENDTCP;
	}

	// z.B.  http://192.168.1.40/?callback=myjp&cmd=aus&newpage=f&page=sonstige&selectedid=2&ids=33,83,102,76,75,2&querystates=t
	char * input = (char *)&(buf[dat_p+4]);
	// DEBUG("%s", input);
	getAjaxInput (input, &(global.query)); // url-Analyse

    // CS-ETH (PB3): CSACTIVE,CSPASSIVE // enc28j60WriteOp() usw. tun es schon :-)
	can_enable_mcp2515 (); // CS-CAN (PB4): can_enable_mcp2515(), can_disable_mcp2515()
    sei (); // enable interrupts: ISR(TIMER0_OVF_vect) {fuer den mcp2512}
	hauselektrik_handler (); // processCmd, processQuery
	cli (); // disable interrupts: ISR(TIMER0_OVF_vect) {fuer den mcp2512}
    can_disable_mcp2515 (); // damit der mcp2515 keine falschen Nachrichten interprtiert

	plen=http200ok(); // "Content-Type: application/javascript"
	plen=fill_tcp_data_p(buf, plen, PSTR("myjp({"));
	plen=fill_tcp_data(buf, plen, global.query.JSONbuf);
	plen=fill_tcp_data_p(buf, plen, PSTR("})"));
SENDTCP:
	www_server_reply(buf,plen); // send web page data
	// tcp port 80 end
}

static void web_init(void)
{
	// set the clock speed to 8MHz
	// set the clock prescaler. First write CLKPCE to enable setting of clock the
	// next four instructions.
	CLKPR=(1<<CLKPCE);
	CLKPR=0; // 8 MHZ
	_delay_loop_1(0); // 60us
	DDRB|= (1<<DDB1); // LED, enable PB1, LED as output
	LEDOFF;

	//initialize the hardware driver for the enc28j60
	enc28j60Init(mymac);
	enc28j60clkout(2); // change clkout from 6.25MHz to 12.5MHz
	_delay_loop_1(0); // 60us
	enc28j60PhyWrite(PHLCON,0x476); // Magjack leds configuration, see enc28j60 datasheet, page 11

	handleDHCP ();

	//init the ethernet/ip layer:
	init_udp_or_www_server(mymac,myip); // myStaticIP
	www_server_port(MYWWWPORT);
}

static void wrap_web_init(void)
{
	/* Maßnahme zur gemeinsamen Nutzung des SPI-Busses:
		A) Bei CS-ETH muss CS-CAN verhindert werden
		B) Bei CS-CAN muss CS-ETH verhindert werden

		CS-CAN (PB4): can_enable_mcp2515(), can_disable_mcp2515()
		CS-ETH (PB3): CSACTIVE,CSPASSIVE // enc28j60WriteOp() usw. tun es schon :-)
	*/
	can_disable_mcp2515(); // damit der mcp2515 keine falschen Nachrichten interprtiert
	cli(); // disable interrupts: ISR(TIMER0_OVF_vect) {fuer den mcp2512}
	web_init(); // enc28j60Init()->CS
	can_enable_mcp2515();
	sei(); // enable interrupts: ISR(TIMER0_OVF_vect) {fuer den mcp2512}
}

static void wrap_web_handler(void)
{
    can_disable_mcp2515(); // damit der mcp2515 keine enc28j60-Nachrichten interprtiert
    cli(); // disable interrupts
    web_handler();
    can_enable_mcp2515();
    sei(); // enable interrupts
}

int main(void)
{
	/* 	Nachricht:HCAN_HMS_DEVICE_RESET des HCAN Frame Callback zum BL wegen eines Firmware-Load:
 	 	Nachricht:HCAN_HMS_DEVICE_RESET->canix_reset()->wdt_enable(WDTO_15MS) */

	wdt_enable (WDTO_8S); // BL stellte 2 Sekunden ein

#ifdef USE_USART
    usart_init(BAUDRATE); // setup the UART
#endif //USE_USART
	DEBUG("\n\rSystem Ready\n\r");
    DEBUG("Compiliert am "__DATE__" um "__TIME__"\r\n");
    DEBUG("Compiliert mit GCC Version "__VERSION__"\r\n");

    // canix_init() haengt schon einige frame-callbacks ein, wenn CANIX_HMS usw. definiert sind:
	canix_init (); // 1. spi_init()-> SPI_CS, 2. sei()
    wrap_web_init ();

	wdt_enable (WDTO_8S);

	// Um Nachrichten vom CAN-Bus empfangen zu koennen:
	canix_reg_frame_callback (hauselektrik_callback, -1,HCAN_PROTO_SFP, HCAN_SRV_HES);

	canix_reg_idle_callback (wrap_web_handler);

	canix_mainloop (); // canix_process_messages()->can_message_available()==true->canix_deliver_frame()->frame_callback_entry
										//		 ->can_message_available()==false->idle_callback
	return 0;
}
