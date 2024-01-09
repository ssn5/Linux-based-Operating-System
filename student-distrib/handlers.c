#include "x86_desc.h"
#include "lib.h"
#include "handlers.h"
#include "i8259.h"
#include "rtc.h"

/*
 * 	exception0
 *   DESCRIPTION: When the OS encounters a Divide Error exception it executes this handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes the handler code, prints the exception name, gets stuck in a while loop,
 *			     freezing the screen
 */
void exception0()
{
    cli();                              // start critical section
    printf("Divide Error Exception\n"); // print exception message
    sti();                              // end critical section (never reaches this line)

    int call_number = 1;
    int firstArg = 256;
    int sys_call_ret;

    asm volatile("            \n\
            movl %1, %%eax      \n\
            movl %2, %%ebx   \n\
            int $0x80              \n\
            movl %%eax, %0                \n\
            "
                 : "=r"(sys_call_ret)
                 : "a"(call_number), "b"(firstArg));
}

/*
 * 	exception1
 *   DESCRIPTION: When the OS encounters an Debug exception it executes this handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes the handler code, prints the exception name, gets stuck in a while loop,
 *			     freezing the screen
 */
void exception1()
{
    cli();
    printf("Debug Exception\n");
    sti();

    int call_number = 1;
    int firstArg = 256;
    int sys_call_ret;

    asm volatile("            \n\
            movl %1, %%eax      \n\
            movl %2, %%ebx   \n\
            int $0x80              \n\
            movl %%eax, %0                \n\
            "
                 : "=r"(sys_call_ret)
                 : "a"(call_number), "b"(firstArg));
}

/*
 * 	exception2
 *   DESCRIPTION: When the OS encounters an NMI Interrupt exception it executes this handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes the handler code, prints the exception name, gets stuck in a while loop,
 *			     freezing the screen
 */
void exception2()
{
    cli();
    printf("NMI Interrupt\n");
    sti();

    int call_number = 1;
    int firstArg = 256;
    int sys_call_ret;

    asm volatile("            \n\
            movl %1, %%eax      \n\
            movl %2, %%ebx   \n\
            int $0x80              \n\
            movl %%eax, %0                \n\
            "
                 : "=r"(sys_call_ret)
                 : "a"(call_number), "b"(firstArg));
}

/*
 * 	exception3
 *   DESCRIPTION: When the OS encounters an Breakpoint Exception it executes this handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes the handler code, prints the exception name, gets stuck in a while loop,
 *			     freezing the screen
 */
void exception3()
{
    cli();
    printf("Breakpoint Exception\n");
    sti();

    int call_number = 1;
    int firstArg = 256;
    int sys_call_ret;

    asm volatile("            \n\
            movl %1, %%eax      \n\
            movl %2, %%ebx   \n\
            int $0x80              \n\
            movl %%eax, %0                \n\
            "
                 : "=r"(sys_call_ret)
                 : "a"(call_number), "b"(firstArg));
}

/*
 * 	exception4
 *   DESCRIPTION: When the OS encounters an Overflow Exception it executes this handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes the handler code, prints the exception name, gets stuck in a while loop,
 *			     freezing the screen
 */
void exception4()
{
    cli();
    printf("Overflow Exception\n");
    sti();

    int call_number = 1;
    int firstArg = 256;
    int sys_call_ret;

    asm volatile("            \n\
            movl %1, %%eax      \n\
            movl %2, %%ebx   \n\
            int $0x80              \n\
            movl %%eax, %0                \n\
            "
                 : "=r"(sys_call_ret)
                 : "a"(call_number), "b"(firstArg));
}

/*
 * 	exception5
 *   DESCRIPTION: When the OS encounters an BOUND Range Exceeded Exception it executes this handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes the handler code, prints the exception name, gets stuck in a while loop,
 *			     freezing the screen
 */
void exception5()
{
    cli();
    printf("BOUND Range Exceeded Exception\n");
    sti();

    int call_number = 1;
    int firstArg = 256;
    int sys_call_ret;

    asm volatile("            \n\
            movl %1, %%eax      \n\
            movl %2, %%ebx   \n\
            int $0x80              \n\
            movl %%eax, %0                \n\
            "
                 : "=r"(sys_call_ret)
                 : "a"(call_number), "b"(firstArg));
}

/*
 * 	exception6
 *   DESCRIPTION: When the OS encounters an Invalid Opcode Exception it executes this handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes the handler code, prints the exception name, gets stuck in a while loop,
 *			     freezing the screen
 */
void exception6()
{
    cli();
    printf("Invalid Opcode Exception\n");
    sti();

    int call_number = 1;
    int firstArg = 256;
    int sys_call_ret;

    asm volatile("            \n\
            movl %1, %%eax      \n\
            movl %2, %%ebx   \n\
            int $0x80              \n\
            movl %%eax, %0                \n\
            "
                 : "=r"(sys_call_ret)
                 : "a"(call_number), "b"(firstArg));
}

/*
 * 	exception7
 *   DESCRIPTION: When the OS encounters an Device Not Available Exception it executes this handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes the handler code, prints the exception name, gets stuck in a while loop,
 *			     freezing the screen
 */
void exception7()
{
    cli();
    printf("Device Not Available Exception\n");
    sti();

    int call_number = 1;
    int firstArg = 256;
    int sys_call_ret;

    asm volatile("            \n\
            movl %1, %%eax      \n\
            movl %2, %%ebx   \n\
            int $0x80              \n\
            movl %%eax, %0                \n\
            "
                 : "=r"(sys_call_ret)
                 : "a"(call_number), "b"(firstArg));
}

/*
 * 	exception8
 *   DESCRIPTION: When the OS encounters an Double Fault Exception it executes this handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes the handler code, prints the exception name, gets stuck in a while loop,
 *			     freezing the screen
 */
void exception8()
{
    cli();
    printf("Double Fault Exception\n");
    sti();

    int call_number = 1;
    int firstArg = 256;
    int sys_call_ret;

    asm volatile("            \n\
            movl %1, %%eax      \n\
            movl %2, %%ebx   \n\
            int $0x80              \n\
            movl %%eax, %0                \n\
            "
                 : "=r"(sys_call_ret)
                 : "a"(call_number), "b"(firstArg));
}

/*
 * 	exception9
 *   DESCRIPTION: When the OS encounters an Coprocessor Segment Overrun it executes this handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes the handler code, prints the exception name, gets stuck in a while loop,
 *			     freezing the screen
 */
void exception9()
{
    cli();
    printf("Coprocessor Segment Overrun\n");
    sti();

    int call_number = 1;
    int firstArg = 256;
    int sys_call_ret;

    asm volatile("            \n\
            movl %1, %%eax      \n\
            movl %2, %%ebx   \n\
            int $0x80              \n\
            movl %%eax, %0                \n\
            "
                 : "=r"(sys_call_ret)
                 : "a"(call_number), "b"(firstArg));
}

/*
 * 	exception10
 *   DESCRIPTION: When the OS encounters an Invalid TSS Exception it executes this handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes the handler code, prints the exception name, gets stuck in a while loop,
 *			     freezing the screen
 */
void exception10()
{
    cli();
    printf("Invalid TSS Exception\n");
    sti();

    int call_number = 1;
    int firstArg = 256;
    int sys_call_ret;

    asm volatile("            \n\
            movl %1, %%eax      \n\
            movl %2, %%ebx   \n\
            int $0x80              \n\
            movl %%eax, %0                \n\
            "
                 : "=r"(sys_call_ret)
                 : "a"(call_number), "b"(firstArg));
}

/*
 * 	exception11
 *   DESCRIPTION: When the OS encounters an Segment Not Present exception it executes this handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes the handler code, prints the exception name, gets stuck in a while loop,
 *			     freezing the screen
 */
void exception11()
{
    cli();
    printf("Segment Not Present\n");
    sti();

    int call_number = 1;
    int firstArg = 256;
    int sys_call_ret;

    asm volatile("            \n\
            movl %1, %%eax      \n\
            movl %2, %%ebx   \n\
            int $0x80              \n\
            movl %%eax, %0                \n\
            "
                 : "=r"(sys_call_ret)
                 : "a"(call_number), "b"(firstArg));
}

/*
 * 	exception12
 *   DESCRIPTION: When the OS encounters an Stack Fault Exception it executes this handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes the handler code, prints the exception name, gets stuck in a while loop,
 *			     freezing the screen
 */
void exception12()
{
    cli();
    printf("Stack Fault Exception\n");
    sti();

    int call_number = 1;
    int firstArg = 256;
    int sys_call_ret;

    asm volatile("            \n\
            movl %1, %%eax      \n\
            movl %2, %%ebx   \n\
            int $0x80              \n\
            movl %%eax, %0                \n\
            "
                 : "=r"(sys_call_ret)
                 : "a"(call_number), "b"(firstArg));
}

/*
 * 	exception13
 *   DESCRIPTION: When the OS encounters an General Protection Exception it executes this handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes the handler code, prints the exception name, gets stuck in a while loop,
 *			     freezing the screen
 */
void exception13()
{
    cli();
    printf("General Protection Exception\n");
    sti();

    int call_number = 1;
    int firstArg = 256;
    int sys_call_ret;

    asm volatile("            \n\
            movl %1, %%eax      \n\
            movl %2, %%ebx   \n\
            int $0x80              \n\
            movl %%eax, %0                \n\
            "
                 : "=r"(sys_call_ret)
                 : "a"(call_number), "b"(firstArg));
}

/*
 * 	exception14
 *   DESCRIPTION: When the OS encounters a Page Fault Exception it executes this handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes the handler code, prints the exception name, gets stuck in a while loop,
 *			     freezing the screen
 */
void exception14()
{
    cli();                            // start critical section
    printf("Page Fault Exception\n"); // print exception message
    while (1)
    {
    }
    sti(); // end critical section (never reaches this line)

    int call_number = 1;
    int firstArg = 256;
    int sys_call_ret;

    asm volatile("            \n\
            movl %1, %%eax      \n\
            movl %2, %%ebx   \n\
            int $0x80              \n\
            movl %%eax, %0                \n\
            "
                 : "=r"(sys_call_ret)
                 : "a"(call_number), "b"(firstArg));
}

// void exception15(){
//     printf("NMI Interrupt");
//     while(1){}
// }

/*
 * 	exception16
 *   DESCRIPTION: When the OS encounters an x87 FPU Floating Point Error it executes this handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes the handler code, prints the exception name, gets stuck in a while loop,
 *			     freezing the screen
 */
void exception16()
{
    cli();
    printf("x87 FPU Floating Point Error\n");
    sti();

    int call_number = 1;
    int firstArg = 256;
    int sys_call_ret;

    asm volatile("            \n\
            movl %1, %%eax      \n\
            movl %2, %%ebx   \n\
            int $0x80              \n\
            movl %%eax, %0                \n\
            "
                 : "=r"(sys_call_ret)
                 : "a"(call_number), "b"(firstArg));
}

/*
 * 	exception17
 *   DESCRIPTION: When the OS encounters an Aligment Check Exception it executes this handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes the handler code, prints the exception name, gets stuck in a while loop,
 *			     freezing the screen
 */
void exception17()
{
    cli();
    printf("Aligment Check Exception\n");
    sti();

    int call_number = 1;
    int firstArg = 256;
    int sys_call_ret;

    asm volatile("            \n\
            movl %1, %%eax      \n\
            movl %2, %%ebx   \n\
            int $0x80              \n\
            movl %%eax, %0                \n\
            "
                 : "=r"(sys_call_ret)
                 : "a"(call_number), "b"(firstArg));
}

/*
 * 	exception18
 *   DESCRIPTION: When the OS encounters an Machine-Check Exception it executes this handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes the handler code, prints the exception name, gets stuck in a while loop,
 *			     freezing the screen
 */
void exception18()
{
    cli();
    printf("Machine-Check Exception\n");
    sti();

    int call_number = 1;
    int firstArg = 256;
    int sys_call_ret;

    asm volatile("            \n\
            movl %1, %%eax      \n\
            movl %2, %%ebx   \n\
            int $0x80              \n\
            movl %%eax, %0                \n\
            "
                 : "=r"(sys_call_ret)
                 : "a"(call_number), "b"(firstArg));
}

/*
 * 	exception19
 *   DESCRIPTION: When the OS encounters an SIMD Floating-Point Exception it executes this handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: executes the handler code, prints the exception name, gets stuck in a while loop,
 *			     freezing the screen
 */
void exception19()
{
    cli();
    printf("SIMD Floating-Point Exception\n");
    sti();

    int call_number = 1;
    int firstArg = 256;
    int sys_call_ret;

    asm volatile("            \n\
            movl %1, %%eax      \n\
            movl %2, %%ebx   \n\
            int $0x80              \n\
            movl %%eax, %0                \n\
            "
                 : "=r"(sys_call_ret)
                 : "a"(call_number), "b"(firstArg));
}
