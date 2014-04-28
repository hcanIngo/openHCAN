/*********************************************
 * vim:sw=8:ts=8:si:et
 * To use the above modeline in vim you must have "set modeline" in your .vimrc
 * Author: Guido Socher 
 * Copyright: LGPL V2
 * See http://www.gnu.org/licenses/old-licenses/lgpl-2.0.html
 *
 * Some common utilities needed for IP and web applications.
 * The defines below are controlled via ip_config.h. By choosing
 * the right defines for your application in ip_config.h you can 
 * significantly reduce the size of the resulting code.
 *********************************************/
//@{
#ifndef WEBSRV_HELP_FUNCTIONS_H
#define WEBSRV_HELP_FUNCTIONS_H
#include "ip_config.h"

#ifdef FROMDECODE_websrv_help
// These functions are documented in websrv_help_functions.c. 
// find_key_val searches for key=value pairs in urls, returns 1 if the key was found (may be empty string) and returns 0 if the key was not found:
extern uint8_t find_key_val(char *str,char *strbuf, uint8_t maxlen,char *key);

extern void urldecode(char *urlbuf); // decode a url string e.g "hello%20joe" or "hello+joe" becomes "hello joe"
#endif
#ifdef URLENCODE_websrv_help
extern void urlencode(const char *str,char *urlbuf); // result goes into urlbuf. There must be enough space in urlbuf. In the worst case that would be 3 times the length of str.
#endif
// parse a string that is an IP address and extract the IP to bytestr
extern uint8_t parse_ip(uint8_t *ip_byte_str,const char *str);
extern void mk_net_str(char *resultstr,uint8_t *ip_byte_str,uint8_t len,char separator,uint8_t base);


#endif /* WEBSRV_HELP_FUNCTIONS_H */
//@}
