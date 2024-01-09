#include "x86_desc.h"
#include "lib.h"
// #include "handlers.h"
#include "i8259.h"
#include "rtc.h"


/*
 * 	printRTCReg
 *   DESCRIPTION: Prints register A of the RTC to the screen.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void printRTCReg(){
    cli();
    outb(0x0a, 0x70);
    char prev = inb(0x71);
    printf("reg A in rtc: %x\n", prev);
    sti();
}

/*
 * 	rtc_init
 *   DESCRIPTION: Initialization the rtc interrupt line by unmasking the interrupt line on the slave PIC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: enables the IRQ 8 interrupt line (IRQ 0  on the slave PIC) 
 */
void rtc_init(){
   
    cli(); //start critical section
    outb(0x8B, 0x70);		// select register B, and disable NMI
    char prev=inb(0x71);	// read the current value of register B
    outb(0x8B, 0x70);		// set the index again (a read will reset the index to register D)
    outb( prev | 0x40, 0x71);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
    sti();
    enable_irq(8); //enable interrupts from irq8 rtc
    cli();
    outb(0x8A, 0x70);		// set index to register A, disable NMI
    prev=inb(0x71);	// get initial value of register A
    outb( 0x8A, 0x70);		// reset index to A
    outb( (prev & 0xF0) | MAX_RTC_FREQ_BM, 0x71); //write only our rate to A. Note, rate is the bottom 4 bits.

    rtc_frequency = MIN_RTC_FREQ;
    rtc_frequency_counter_limit = MAX_RTC_FREQ / MIN_RTC_FREQ; // number of ticks to occur before read
    rtc_frequency_counter = rtc_frequency_counter_limit; // set current counter to tick limit
    rtc_initialized = 1; // raise intialization flag
    rtc_interrupt_occurred = 0;

    sti(); //end critical section
}

/*
 * 	rtc_handler
 *   DESCRIPTION: Executes the code inside this handler periodically as the RTC raises an interrupt
 *   INPUTS: none
 *   OUTPUTS: runs the test_interrupts, printing/flashing alternate character on the screen
 *   RETURN VALUE: none
 *   SIDE EFFECTS: clears and sets the interrupt flag, while executing the code inside the handler
 */ 
void rtc_handler(){
    cli(); //start critical section
    //test_interrupts(); //call function that checkpoint 1 requires
    outb(0x0C, 0x70); // select register C
    inb(0x71); //throw away contents

    if (rtc_frequency_counter > 0) rtc_frequency_counter--; //tick

    if (rtc_frequency_counter  == 0){ // if tick limit reached
        rtc_interrupt_occurred = 0; // lower flag
        rtc_frequency_counter = rtc_frequency_counter_limit; // reset tick counter
    }

    send_eoi(8); //end of interrupts for irq8 rtc
    sti(); //end critical section
}

/*
 * 	rtc_read
 *   DESCRIPTION: RTC read system call. Waits for rtc interrupt to occur and returns.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t - always 0
 *   SIDE EFFECTS: raises rtc_interrupt_occurred flag temporarily.
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes){
    rtc_interrupt_occurred = 1; // raises flag
    while (rtc_interrupt_occurred != 0){} // waits for flag to be lowered by rtc handler
    return 0;
}

/*
 * 	rtc_close
 *   DESCRIPTION: RTC close system call.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t - always 0
 *   SIDE EFFECTS: none
 */
int32_t rtc_close(int32_t fd){
    rtc_initialized = 0; // lower rtc initialized flag
    return 0;
}

/*
 * 	rtc_write
 *   DESCRIPTION: RTC write system call. Changes RTC frequency.
 *   INPUTS: buf - buffer, n - number of bytes
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t - 4 if success, -1 if failure
 *   SIDE EFFECTS: changes the rtc frequency.
 */
int32_t rtc_write(int32_t fd, const void* buf_arg, int32_t n){
    // printf("rtc_write call\n");
    cli();
    uint8_t* buf = (uint8_t*) buf_arg;
    int32_t freq;
    freq = (uint32_t) (*buf); // get frequency from buffer
    if (freq > 1024 || freq < 2 || freq == NULL) { // parameter checking
        printf("Error setting RTC\n");
        sti();
        return -1;
    } 
    rtc_frequency_counter_limit = MAX_RTC_FREQ / freq; // redefine tick limit
    rtc_frequency_counter = rtc_frequency_counter_limit; // reset tick counter
    sti();
    return -1; // return error if freq is not power of 2
}

/*
 * 	rtc_open
 *   DESCRIPTION: RTC open system call.
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: int32_t - always 0
 *   SIDE EFFECTS: sets rtc frequency to 2 Hz.
 */
int32_t rtc_open(const uint8_t* filename){
    if (rtc_initialized == 0) rtc_init(); // if rtc not initialized, initialize it first
    uint8_t i = 2;
    rtc_write(0,&i,4); // set freq to 2 Hz, required by addendum
    return 0;
}
