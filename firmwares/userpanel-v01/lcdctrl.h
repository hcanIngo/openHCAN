#ifndef LCDCTRL_H
#define LCDCTRL_H

#include <inttypes.h>

#define LCD_BACKLIGHT_DEFAULT 30
#define LCD_CONTRAST_DEFAULT 10
#define LCD_BACKLIGHT_DEFAULT_LOW 3
#define LCD_CONTRAST_DEFAULT_LOW 15

#define LCD_MODE_INACTIVE 0
#define LCD_MODE_ACTIVE 1

void lcdctrl_init(void);
void lcdctrl_blink(void);
void lcdctrl_set_backlight(uint8_t amount);
void lcdctrl_set_contrast(uint8_t amount);
void lcdctrl_set_display_light(uint8_t mode);

#endif
