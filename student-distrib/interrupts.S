//#include "interrupts.h"
//#include "handlers.h"

/* Steps: pushing all registers and flags to stack,
* calling relevant interrupt handler, restoring registers
* and flags, iret context
*/

/* interrupt linkage
* INPUTS: name, func,
* OUTPUTS: none
* RETURN VALUE: none
* DESCRIPTION: assembly linkage for hardware interrupts
*/
#define INTR_LINK(name, func)   \
    .globl name         ;\
    name:               ;\
        pushal          ;\
        pushfl          ;\
        call func       ;\
        popfl           ;\
        popal           ;\
        iret

INTR_LINK(rtc_handler_linkage, rtc_handler);
INTR_LINK(keyboard_handler_linkage, keyboard_handler);
INTR_LINK(pit_handler_linkage, pit_handler);

/* Steps: parameter validation of system call number,
* pushing args to stack, invoking jumptable with system
* call number in eax, saving return value, restore regs,
* iret context
*/

/* system call linkage
* INPUTS: name, func,
* OUTPUTS: none
* RETURN VALUE: none
* DESCRIPTION: assembly linkage for system calls
*/
#define SYS_LINK(name2, func)   \
    .globl name2, jumptable_asm            ;\
                            ;\
    ret_value: .long 0x0    ;\
    jumptable_asm:          ;\
    .long halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn ;\
    name2:                  ;\
        pushal              ;\
        pushfl              ;\
        addl $-1, %eax;     ;\
        cmpl $9, %eax       ;\
        jle number_valid_upper    ;\
        movl $-1, ret_value ;\
        jmp get_out         ;\
    number_valid_upper:           ;\
        cmpl $0, %eax       ;\
        jge number_valid_lower  ;\
        movl $-1, ret_value ;\
        jmp get_out ;\
    number_valid_lower: ;\
        pushl %edx          ;\
        pushl %ecx          ;\
        pushl %ebx          ;\
        call *jumptable_asm(,%eax,4)           ;\
        addl $12, %esp       ;\
        movl %eax, ret_value    ;\
    get_out:                ;\
        popfl               ;\
        popal               ;\
        movl ret_value, %eax    ;\
        iret

SYS_LINK(system_call_linkage, jumpTable);
