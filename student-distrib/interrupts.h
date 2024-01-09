#include "handlers.h"
#include "system_calls.h"

#if !defined(INTERRUPTS_H)
#define INTERRUPTS_H

void rtc_handler_linkage();
void keyboard_handler_linkage();
void system_call_linkage();
void pit_handler_linkage();

#endif 
