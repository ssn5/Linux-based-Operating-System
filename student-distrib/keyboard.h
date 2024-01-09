#if !defined(KEYBOARD_H)
#define KEYBOARD_H

#include "lib.h"
#include "i8259.h"
#include "terminal.h"

extern void keyboard_init();
extern void keyboard_handler();

unsigned char line_buffer[128]; //keyboard buffer to keep track of typed characters
unsigned int line_buf_index; //index to keep track of number of characters typed


#endif
