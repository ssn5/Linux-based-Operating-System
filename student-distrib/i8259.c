/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */
// some code adapted from wiki.osdev.org and kernel.org
#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */


/*
 * i8259_init
 *   DESCRIPTION: Initialize the 8259 PIC.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: initializes slave and master pics. masks all interrupts.
 */
void i8259_init(void) {
    outb(ICW1, MASTER_8259_PORT);  // ICW1: select 8259A-1 init
    outb(ICW2_MASTER, 0x21);       // ICW2: 8259A-1 IR0-7 mapped to 0x20-0x27
    outb(ICW3_MASTER, 0x21);       // ICW3: primary pic has secondary at irq2
    outb(ICW4, 0x21);              // ICW4: isa=x86, normal eoi

    outb(ICW1, SLAVE_8259_PORT);  // ICW1: select 8259A-2 init
    outb(ICW2_SLAVE, 0xa1);       // ICW2: 8259A-2 IR0-7 mapped to 0x28-0x2f
    outb(ICW3_SLAVE, 0xa1);       // ICW3: secondary pic is at primary irq2
    outb(ICW4, 0xa1);             // ICW4: isa=x86, normal eoi

    master_mask = 0xFb;       // master mask all except irq2
    slave_mask = 0xff;        // slave mask all irq
    outb(master_mask, 0x21);  // send mask to master command port
    outb(slave_mask, 0xa1);   // send mask to slave command port
    // printf("pic i8259_init complete\n"); //print statement for debugging
}

/*
 * enable_irq
 *   DESCRIPTION: Enable (unmask) the specified IRQ.
 *   INPUTS: irq_num -- IRQ number to unmask.
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: unmasks a specified IRQ on slave or master PIC.
 */
void enable_irq(uint32_t irq_num) {
    if (irq_num < 0 || irq_num > 16) return; //if argument is not valid, return
    uint16_t port;  // initialize variable to store port
    uint8_t value;  // initialize variable to store new mask

    if (irq_num < 8) {  // if irq is on slave pic or master pic
        port = 0x21;    // select master pic command port
    } else {
        port = 0xa1;   // select slave pic command port
        irq_num -= 8;  // subtract to match irq0-7
    }
    cli();                            // start critical section
    value = inb(port);                // get mask from correct pic
    value = value & ~(1 << irq_num);  // update mask to unmask particular irq
    outb(value, port);                // send mask to pic
    sti();                            // end critical section
}

/*
 * disable_irq
 *   DESCRIPTION: Disable (mask) the specified IRQ.
 *   INPUTS: irq_num -- IRQ number to mask.
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: masks a specified IRQ on slave or master PIC.
 */
void disable_irq(uint32_t irq_num) {
    if (irq_num < 0 || irq_num > 16) return; //if argument is not valid, return
    uint16_t port;  // initialize variable to store port
    uint8_t value;  // initialize variable to store new mask

    if (irq_num < 8) {  // if irq is on slave pic or master pic
        port = 0x21;    // select master pic command port
    } else {
        port = 0xa1;   // select slave pic command port
        irq_num -= 8;  // subtract to match irq0-7
    }

    cli();                               // start critical section
    value = inb(port) | (1 << irq_num);  // get mask from correct pic and update mask to unmask particular irq
    outb(value, port);                   // send mask to pic
    sti();                               // end critical section
}

/*
 * send_eoi
 *   DESCRIPTION: Send end-of-interrupt signal for the specified IRQ.
 *   INPUTS: irq_num -- IRQ number to send EOI to.
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: sends EOI for particular IRQ on master PIC or master/slave PICs.
 */
void send_eoi(uint32_t irq_num) {
    if (irq_num < 0 || irq_num > 16) return; //if argument is not valid, return
    if (irq_num >= 8) {                                // if irq is on slave or master pic
        outb((EOI | (irq_num - 8)), SLAVE_8259_PORT);  // send eoi to slave pic after correcting irqnum to irq0-7
        outb((EOI | 2), MASTER_8259_PORT);             // send eoi to irq2 on master pic
    } else {
        outb((EOI | irq_num), MASTER_8259_PORT);  // send eoi to irq_num on master pic because irq is only on master
    }
}

