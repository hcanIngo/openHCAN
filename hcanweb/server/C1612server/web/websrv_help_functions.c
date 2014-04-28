/*********************************************
 * vim:sw=8:ts=8:si:et
 * To use the above modeline in vim you must have "set modeline" in your .vimrc
 * Author: Guido Socher
 * Copyright:LGPL V2
 * See http://www.gnu.org/licenses/old-licenses/lgpl-2.0.html
 *
 * Some common utilities needed for IP and web applications.
 * The defines below are controlled via ip_config.h. By choosing
 * the right defines for your application in ip_config.h you can 
 * significantly reduce the size of the resulting code.
 *********************************************/
#include <avr/io.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ip_config.h"

#ifdef FROMDECODE_websrv_help
// search for a string of the form key=value in
// a string that looks like q?xyz=abc&uvw=defgh HTTP/1.1\r\n
//
// The returned value is stored in strbuf. You must allocate
// enough storage for strbuf, maxlen is the size of strbuf.
// I.e the value it is declared with: strbuf[5]-> maxlen=5
uint8_t find_key_val(char *str,char *strbuf, uint8_t maxlen,char *key)
{
        uint8_t found=0;
        uint8_t i=0;
        char *kp;
        kp=key;
        while(*str &&  *str!=' ' && *str!='\r' && found==0){
                if (*str == *kp){
                        // At the beginning of the key we must check
                        // if this is the start of the key otherwise we will
                        // match on 'foobar' when only looking for 'bar', by andras tucsni
                        if (kp==key &&  ! ( *(str-1) == '?' || *(str-1) == '&' ) ) goto NEXT;
                        kp++;
                        if (*kp == '\0'){
                                str++;
                                kp=key;
                                if (*str == '='){
                                        found=1;
                                }
                        }
                }else{
                        kp=key;
                }
NEXT:
                str++;
        }
        if (found==1){
                // copy the value to a buffer and terminate it with '\0'
                while(*str &&  *str!=' ' && *str!='\r' && *str!='&' && i<maxlen-1){
                        *strbuf=*str;
                        i++;
                        str++;
                        strbuf++;
                }
                *strbuf='\0';
        }
        // return 1 if found (the string in strbuf might still be an empty string)
        // otherwise return 0
        return(found);
}

// convert a single hex digit character to its integer value
unsigned char h2int(char c)
{
        if (c >= '0' && c <='9'){
                return((unsigned char)c - '0');
        }
        if (c >= 'a' && c <='f'){
                return((unsigned char)c - 'a' + 10);
        }
        if (c >= 'A' && c <='F'){
                return((unsigned char)c - 'A' + 10);
        }
        return(0);
}

// decode a url string e.g "hello%20joe" or "hello+joe" becomes "hello joe"
void urldecode(char *urlbuf)
{
        char c;
        char *dst;
        dst=urlbuf;
        while ((c = *urlbuf)) {
                if (c == '+') c = ' ';
                if (c == '%') {
                        urlbuf++;
                        c = *urlbuf;
                        urlbuf++;
                        c = (h2int(c) << 4) | h2int(*urlbuf);
                }
                *dst = c;
                dst++;
                urlbuf++;
        }
        *dst = '\0';
}

#endif //  FROMDECODE_websrv_help

#ifdef URLENCODE_websrv_help

// convert a single character to a 2 digit hex str
// a terminating '\0' is added
void int2h(char c, char *hstr)
{
        hstr[1]=(c & 0xf)+'0';
        if ((c & 0xf) >9){
                hstr[1]=(c & 0xf) - 10 + 'a';
        }
        c=(c>>4)&0xf;
        hstr[0]=c+'0';
        if (c > 9){
                hstr[0]=c - 10 + 'a';
        }
        hstr[2]='\0';
}

// There must be enough space in urlbuf. In the worst case that would be
// 3 times the length of str
void urlencode(const char *str,char *urlbuf)
{
        char c;
        while ((c = *str)) {
                if (c == ' '||isalnum(c)){ 
                        if (c == ' '){ 
                                c = '+';
                        }
                        *urlbuf=c;
                        str++;
                        urlbuf++;
                        continue;
                }
                *urlbuf='%';
                urlbuf++;
                int2h(c,urlbuf);
                urlbuf++;
                urlbuf++;
                str++;
        }
        *urlbuf='\0';
}

#endif // URLENCODE_websrv_help

// parse a string that is an IP address and extract the IP to ip_byte_str
uint8_t parse_ip(uint8_t *ip_byte_str,const char *str)
{
        char strbuf[4];
        uint8_t bufpos=0;
        uint8_t i=0;
        while(i<4){
                ip_byte_str[i]=0;
                i++;
        }
        i=0;
        while(*str && i<4){
                // if a number then start
                if (bufpos < 3 && isdigit(*str)){
                        strbuf[bufpos]=*str; // copy
                        bufpos++;
                }
                if (bufpos && *str == '.'){
                        strbuf[bufpos]='\0';
                        ip_byte_str[i]=(atoi(strbuf)&0xff);
                        i++;
                        bufpos=0;
                }
                str++;
        }
        if (i==3){ // must have read the first componets of the IP
                strbuf[bufpos]='\0';
                ip_byte_str[i]=(atoi(strbuf)&0xff);
                return(0);
        }
        return(1);
}

// take a byte string and convert it to a human readable display string  (base is 10 for ip and 16 for mac addr), len is 4 for IP addr and 6 for mac.
void mk_net_str(char *resultstr,uint8_t *ip_byte_str,uint8_t len,char separator,uint8_t base)
{
        uint8_t i=0;
        uint8_t j=0;
        while(i<len){
                itoa((int)ip_byte_str[i],&resultstr[j],base);
                // search end of str:
                while(resultstr[j]){j++;}
                if (separator){ // no separator, separator==NULL is as well possible, suggested by andras tucsni
                        resultstr[j]=separator;
                        j++;
                }
                i++;
        }
        j--;
        resultstr[j]='\0';
}

// end of websrv_help_functions.c
