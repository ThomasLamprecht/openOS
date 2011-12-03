#include "keyboard.h"


/** send command to the keyboard */
static void send_command(uint8_t command) // TODO MOVE!!
{
	// Warten bis die Tastatur bereit ist, und der Befehlspuffer leer ist
	while ((inb(0x64) & 0x2)) {}
	outb(0x60, command);
}

void init_keyboard(void)
{

	// flush keyboard buffer
	while (inb(0x64) & 0x1)
	{
		inb(0x60);
	}

	// activate keyboard
	send_command(0xF4);
}

void kbd_update_leds(uint8_t status)
{
	while((inb(0x64)&2)!=0){} //loop until zero
	outb(0x60,0xED);

	while((inb(0x64)&2)!=0){} //loop until zero
	outb(0x60,status);
}

void handle_keystroke()
{
	uint8_t scan_code = inb(0x60);
	uint8_t shift = 0;

	uint8_t keycode = 0;
	int break_code = 1;

	// Status-Variablen fuer das Behandeln von e0- und e1-Scancodes
	static int     e0_code = 1;
	// Wird auf 1 gesetzt, sobald e1 gelesen wurde, und auf 2, sobald das erste
	// Datenbyte gelesen wurde
	static int      e1_code = 0;
	static uint16_t  e1_prev = 0;

	scan_code = inb(0x60);

	// Um einen Breakcode handelt es sich, wenn das oberste Bit gesetzt ist und
	// es kein e0 oder e1 fuer einen Extended-scancode ist
	if ((scan_code & 0x80) && (e1_code || (scan_code != 0xE1)) && (e0_code || (scan_code != 0xE0)))
	{
		break_code = 0;
		scan_code &= ~0x80;
		return;
	}

	if (e0_code)
	{
		// Fake shift abfangen und ignorieren
		if ((scan_code == 0x2A) || (scan_code == 0x36))
		{
			e0_code = 1;
			return;
		}

		kprintf("%x|",scan_code); // keycode = translate_scancode(1, scan_code);
		e0_code = 1;
	}
	else if (e1_code == 2)
	{
		// Fertiger e1-Scancode
		// Zweiten Scancode in hoeherwertiges Byte packen
		e1_prev |= ((uint16_t) scan_code << 8);
		kprintf("%x|",scan_code); // keycode = translate_scancode(2, e1_prev);
		e1_code = 0;
	}
	else if (e1_code == 1)
	{
		// Erstes Byte fuer e1-Scancode
		e1_prev = scan_code;
		e1_code++;
	}
	else if (scan_code == 0xE0)
	{
		// Anfang eines e0-Codes
		e0_code = 0;
	}
	else if (scan_code == 0xE1)
	{
		// Anfang eines e1-Codes
		e1_code = 1;
	}
	else
	{
		// Normaler Scancode
		kprintf("%x|",scan_code);
	}
	//kprintf("%c", keycode);
}
