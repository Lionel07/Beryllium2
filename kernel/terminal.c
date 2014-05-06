#include <terminal.h>
#include <types.h>
#include <beryllium/video.h>
#ifdef X86
#include <x86/ports.h>
#endif
volatile uint32_t term_x;
volatile uint32_t term_y;

///
///Determines if the screen needs to be scrolled, and scrolls.
///

void scroll()
{
	if(term_y >= 25)
	{
		video_scroll(0,24);
		term_y = 24;
	}
}

///
///Better than textmode_write, it formats the output at a basic level.
///
void printc(unsigned char c)
{
	if (c == 0x08 && term_x) //Backspace
	{
		term_x--;
	}
	else if (c == 0x09) //Tab
	{
		term_x = (term_x+8) & ~(8-1);
	}
	else if (c == '\r') //Return
	{
	   term_x = 0;
	}

	else if (c == '\n') //Newline
	{
	   term_x = 0;
	   term_y++;
	}
	else if(c >= ' ') //Anything else
	{
		video_printchar(term_x,term_y,c);
		term_x++;
	}

	if (term_x >= 80)
	{
		term_x = 0;
		term_y ++;
	}

	// Scroll the screen if needed.
	scroll();
	// Move the hardware cursor.
	video_setcursor(term_x,term_y);
}

///
///Prints a basic string
///
void print(const char *c)
{
	int i = 0;
	while (c[i])
	{
		printc(c[i++]);
	}
}
void terminal_clear()
{
	video_clear();
}
///
///Initialises the whole thing
///
void terminal_init()
{
	term_x = 0;
	term_y = 0;
	terminal_clear();
	video_setcursor(term_x,term_y);
	//terminal_clear();
}
