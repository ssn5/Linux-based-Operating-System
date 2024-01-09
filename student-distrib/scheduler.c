#include "scheduler.h"
#include "system_calls.h"
#include "terminal.h"

volatile int running_terminal = 0;

/* pit_init
* INPUTS: none
* OUTPUTS: none
* RETURN: none
* DESCRIPTION: initializing pit chip (reference to OSDev)
*/
void pit_init(){
    enable_irq(0);
    //cli();
    int32_t div = PIT_INPUT_CLOCK/FREQ;
    outb(CMD_DATA, CMD_REG);
    outb(div && BYTE_LOWER_MASK, CHANNEL0);
    outb(div >> 8, CHANNEL0);
    return;
}


/* pit_handler
* INPUTS: none
* OUTPUTS: none
* RETURN: none
* DESCRIPTION: handler for pit responsible for scheduling based on PIT interrupts
*/
void pit_handler(){
    cli();
    if((terminal_arr[running_terminal].curr_pcb == NULL)){      // base shell check
        sti();
        send_eoi(0);
        return;
    }
  
    scheduler();
    sti();
    
    return;
}

/* scheduler
* INPUTS: none
* OUTPUTS: none
* RETURN: none
* DESCRIPTION: helper called by pit_handler, responsible for scheduling
*/
void scheduler(){
    cli();

    int prev_terminal = running_terminal;
    register uint32_t local_esp_saved asm("esp");       // saving context switch information
    terminal_arr[prev_terminal].esp_saved = local_esp_saved;
    register uint32_t local_ebp_saved asm("ebp");
    terminal_arr[prev_terminal].ebp_saved = local_ebp_saved;

    int temp_terminal = (1 + running_terminal) % 3;         // updating terminal to schedule on
    pcb_t* temp_pcb = terminal_arr[temp_terminal].curr_pcb;
    
    if ((temp_pcb == NULL) || (temp_pcb->parent_pid < 0)){
        temp_terminal = (2 + running_terminal) % 3;
        temp_pcb = terminal_arr[temp_terminal].curr_pcb;
        if((temp_pcb == NULL) || (temp_pcb->parent_pid < 0)) {      // base shell check
            send_eoi(0);        // IRQ for PIT
            sti();
            return;
        }
    }
    
    int next_terminal = temp_terminal;

    terminal_arr[prev_terminal].esp0_term = tss.esp0;
    terminal_arr[prev_terminal].ss0_term = tss.ss0;

    flush_tlb(terminal_arr[next_terminal].curr_pid);            // flushing tlb and mapping memory for the next terminal

    if (terminal_id == next_terminal){      // if upcoming terminal is the same as current terminal, write to video memory
        schedule_visible_page();
    }
    else{                                   // else write to corresponding build buffer
        schedule_invisible_page(terminal_arr[next_terminal].vmem_location);
    }

    // setting esp, ebp for new process that must be run
    uint32_t temp_esp, temp_ebp = 0;
    temp_esp = terminal_arr[next_terminal].esp_saved;
    temp_ebp = terminal_arr[next_terminal].ebp_saved;

    tss.esp0 = terminal_arr[next_terminal].esp0_term;           // saving context switch information
    tss.ss0 = terminal_arr[next_terminal].ss0_term;

    running_terminal = next_terminal;

    send_eoi(0);        // IRQ for PIT
    sti();

    // inline assembly to update esp, ebp
    asm volatile(" \n\
        pushl %0  \n\
        pushl %1  \n\
        popl %%ebp \n\
        popl %%esp \n\
        leave \n\
        ret \n\
        "
        :
        : "r"(temp_esp), "r"(temp_ebp)
        : "cc"
    );

    return;
}

