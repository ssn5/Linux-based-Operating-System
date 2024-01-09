#if !defined(SCHEDULER_H)
#define SCHEDULER_H

#include "lib.h"
#include "i8259.h"
#include "x86_desc.h"

#define PIT_INPUT_CLOCK 1193180
#define FREQ 100
#define CMD_DATA 0x36
#define CMD_REG 0x43
#define BYTE_LOWER_MASK 0xFF
#define CHANNEL0 0x40

void pit_init();
void pit_handler();
void scheduler();

extern volatile int running_terminal;

#endif
