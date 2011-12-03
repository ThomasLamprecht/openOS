#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "asm/io.h"
#include "console.h"

#define SCROLL_LED 1
#define NUM_LED 2
#define CAPS_LED 4

void init_keyboard(void);
void kbd_update_leds(uint8_t status);

void handle_keystroke(void);
uint8_t translate_scancode(int set, uint16_t scancode);

#endif