/* kernel.c - the C part of the kernel
 * vim:ts=4 noexpandtab
 */

#include "multiboot.h"
#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "debug.h"
// #include "tests.h"
#include "interrupts.h"
#include "paging.h"
#include "filesystem.h"
#include "keyboard.h"
#include "rtc.h"

#define RUN_TESTS

/* Macros. */
/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags, bit)   ((flags) & (1 << (bit)))

/* Check if MAGIC is valid and print the Multiboot information structure
   pointed by ADDR. */
void entry(unsigned long magic, unsigned long addr) {

    multiboot_info_t *mbi;

    /* Clear the screen. */
    clear();

    /* Am I booted by a Multiboot-compliant boot loader? */
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        printf("Invalid magic number: 0x%#x\n", (unsigned)magic);
        return;
    }

    /* Set MBI to the address of the Multiboot information structure. */
    mbi = (multiboot_info_t *) addr;

    /* Print out the flags. */
    printf("flags = 0x%#x\n", (unsigned)mbi->flags);

    /* Are mem_* valid? */
    if (CHECK_FLAG(mbi->flags, 0))
        printf("mem_lower = %uKB, mem_upper = %uKB\n", (unsigned)mbi->mem_lower, (unsigned)mbi->mem_upper);

    /* Is boot_device valid? */
    if (CHECK_FLAG(mbi->flags, 1))
        printf("boot_device = 0x%#x\n", (unsigned)mbi->boot_device);

    /* Is the command line passed? */
    if (CHECK_FLAG(mbi->flags, 2))
        printf("cmdline = %s\n", (char *)mbi->cmdline);

    if (CHECK_FLAG(mbi->flags, 3)) {
        int mod_count = 0;
        int i;
        module_t* mod = (module_t*)mbi->mods_addr;

        bootblock_init(mod->mod_start);         // calling function from filesystem.h to initialize bootblock structure
        
        while (mod_count < mbi->mods_count) {
            printf("Module %d loaded at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_start);
            printf("Module %d ends at address: 0x%#x\n", mod_count, (unsigned int)mod->mod_end);
            printf("First few bytes of module:\n");
            for (i = 0; i < 16; i++) {
                printf("0x%x ", *((char*)(mod->mod_start+i)));
            }
            printf("\n");
            mod_count++;
            mod++;
        }
    }
    /* Bits 4 and 5 are mutually exclusive! */
    if (CHECK_FLAG(mbi->flags, 4) && CHECK_FLAG(mbi->flags, 5)) {
        printf("Both bits 4 and 5 are set.\n");
        return;
    }

    /* Is the section header table of ELF valid? */
    if (CHECK_FLAG(mbi->flags, 5)) {
        elf_section_header_table_t *elf_sec = &(mbi->elf_sec);
        printf("elf_sec: num = %u, size = 0x%#x, addr = 0x%#x, shndx = 0x%#x\n",
                (unsigned)elf_sec->num, (unsigned)elf_sec->size,
                (unsigned)elf_sec->addr, (unsigned)elf_sec->shndx);
    }

    /* Are mmap_* valid? */
    if (CHECK_FLAG(mbi->flags, 6)) {
        memory_map_t *mmap;
        printf("mmap_addr = 0x%#x, mmap_length = 0x%x\n",
                (unsigned)mbi->mmap_addr, (unsigned)mbi->mmap_length);
        for (mmap = (memory_map_t *)mbi->mmap_addr;
                (unsigned long)mmap < mbi->mmap_addr + mbi->mmap_length;
                mmap = (memory_map_t *)((unsigned long)mmap + mmap->size + sizeof (mmap->size)))
            printf("    size = 0x%x, base_addr = 0x%#x%#x\n    type = 0x%x,  length    = 0x%#x%#x\n",
                    (unsigned)mmap->size,
                    (unsigned)mmap->base_addr_high,
                    (unsigned)mmap->base_addr_low,
                    (unsigned)mmap->type,
                    (unsigned)mmap->length_high,
                    (unsigned)mmap->length_low);
    }

    /* Construct an LDT entry in the GDT */
    {
        seg_desc_t the_ldt_desc;
        the_ldt_desc.granularity = 0x0;
        the_ldt_desc.opsize      = 0x1;
        the_ldt_desc.reserved    = 0x0;
        the_ldt_desc.avail       = 0x0;
        the_ldt_desc.present     = 0x1;
        the_ldt_desc.dpl         = 0x0;
        the_ldt_desc.sys         = 0x0;
        the_ldt_desc.type        = 0x2;

        SET_LDT_PARAMS(the_ldt_desc, &ldt, ldt_size);
        ldt_desc_ptr = the_ldt_desc;
        lldt(KERNEL_LDT);
    }

    /* Construct a TSS entry in the GDT */
    {
        seg_desc_t the_tss_desc;
        the_tss_desc.granularity   = 0x0;
        the_tss_desc.opsize        = 0x0;
        the_tss_desc.reserved      = 0x0;
        the_tss_desc.avail         = 0x0;
        the_tss_desc.seg_lim_19_16 = TSS_SIZE & 0x000F0000;
        the_tss_desc.present       = 0x1;
        the_tss_desc.dpl           = 0x0;
        the_tss_desc.sys           = 0x0;
        the_tss_desc.type          = 0x9;
        the_tss_desc.seg_lim_15_00 = TSS_SIZE & 0x0000FFFF;

        SET_TSS_PARAMS(the_tss_desc, &tss, tss_size);

        tss_desc_ptr = the_tss_desc;

        tss.ldt_segment_selector = KERNEL_LDT;
        tss.ss0 = KERNEL_DS;
        tss.esp0 = 0x800000;
        ltr(KERNEL_TSS);
    }

    unsigned int i;
    for (i = 0x00; i <= 0x2F; i++){
        idt[i].seg_selector = KERNEL_CS; // setting the segment selector, for the exceptions and interrupts to be Code Segment.
        idt[i].reserved4 = 0; // setting the reserved bits for the exceptions
        idt[i].reserved2 = 1;// setting the reserved bits for the exceptions
        idt[i].reserved1 = 1; // setting the reserved bits for the exceptions
        idt[i].size = 1;     // setting the size of the trap gate to be 32 bits
        idt[i].reserved0 = 0; // setting the reserved bits for the exceptions
        idt[i].present = 1;  // setting the present bit to 1
        idt[i].dpl = 0;  // setting the privilege level for the exceptions to be 0, the highest level
        if (i < 0x1F){idt[i].reserved3 = 1;} // if the gate is a trap gate i.e exception set the reserved3 to be 1.
        else{idt[i].reserved3 = 0;}   // if the gate is an interrupt gate you set the reserved to b0
    }
    
    idt[0x80].seg_selector = KERNEL_CS;
    idt[0x80].reserved4 = 0; // setting the reserved bits for the system call
    idt[0x80].reserved2 = 1;  // setting the reserved bits for the system call
    idt[0x80].reserved1 = 1; // setting the reserved bits for the system call
    idt[0x80].size = 1;    // setting the gate size to be 32 bits
    idt[0x80].reserved0 = 0; // setting the reserved bits for the system call
    idt[0x80].present = 1;  // setting the present bit to 1
    idt[0x80].reserved3 = 1; //setting the reserve bit to 1
    idt[0x80].dpl = 3; // setting the privilege level for the system call gate to 3
    
    // for the name of the respective exceptions and its handler's description, refer to the interfaces in handlers.c file
    
    SET_IDT_ENTRY(idt[0], exception0); // populate the IDT for the first exception, linking it to its respective handler
    SET_IDT_ENTRY(idt[1], exception1); // populate the IDT for the second exception, linking it to its respective handler
    SET_IDT_ENTRY(idt[2], exception2); // populate the IDT for the third exception, linking it to its respective handler
    SET_IDT_ENTRY(idt[3], exception3); // populate the IDT for the fourth exception, linking it to its respective handler
    SET_IDT_ENTRY(idt[4], exception4); // populate the IDT for the fifth exception, linking it to its respective handler
    SET_IDT_ENTRY(idt[5], exception5); // populate the IDT for the sixth exception, linking it to its respective handler
    SET_IDT_ENTRY(idt[6], exception6); // populate the IDT for the seventh exception, linking it to its respective handler
    SET_IDT_ENTRY(idt[7], exception7); // populate the IDT for the eighth exception, linking it to its respective handler
    SET_IDT_ENTRY(idt[8], exception8); // populate the IDT for the ninth exception, linking it to its respective handler
    SET_IDT_ENTRY(idt[9], exception9); // populate the IDT for the tenth exception, linking it to its respective handler
    SET_IDT_ENTRY(idt[10], exception10); // populate the IDT for the eleventh exception, linking it to its respective handler
    SET_IDT_ENTRY(idt[11], exception11); // populate the IDT for the twelfth exception, linking it to its respective handler
    SET_IDT_ENTRY(idt[12], exception12); // populate the IDT for the thirteenth exception, linking it to its respective handler
    SET_IDT_ENTRY(idt[13], exception13); // populate the IDT for the fourteenth exception, linking it to its respective handler
    SET_IDT_ENTRY(idt[14], exception14); // populate the IDT for the fifteenth exception, linking it to its respective handler
    //SET_IDT_ENTRY(idt[15], exception15);
    SET_IDT_ENTRY(idt[16], exception16); // populate the IDT for the seventeenth exception, linking it to its respective handler
    SET_IDT_ENTRY(idt[17], exception17); // populate the IDT for the eighteenth exception, linking it to its respective handler
    SET_IDT_ENTRY(idt[18], exception18); // populate the IDT for the nineteenth exception, linking it to its respective handler
    SET_IDT_ENTRY(idt[19], exception19); // populate the IDT for the twentieth exception, linking it to its respective handler

    SET_IDT_ENTRY(idt[0x20], pit_handler_linkage);
    SET_IDT_ENTRY(idt[0x21], keyboard_handler_linkage); // populate the IDT with the interrupt line/gate for the keyboard, linking to the keyboard handler
    SET_IDT_ENTRY(idt[0x28], rtc_handler_linkage); // populate the IDT with the interrupt line/gate for the rtc, linking to the rtc handler

    SET_IDT_ENTRY(idt[0x80], system_call_linkage); // populate the IDT with the system call (trap gate), linking to the system call handler

    lidt(idt_desc_ptr); //load idtr with idt descriptor

    /* Init the PIC */
    i8259_init();

    /* Initialize devices, memory, filesystem, enable device interrupts on the
     * PIC, any other initialization stuff... */
    initialize_paging(); //initialize paging
    rtc_init(); //initialize rtc
    keyboard_init(); //initialize keyboard
    terminal_init();
    //pit_init();

    /* Enable interrupts */
    /* Do not enable the following until after you have set up your
     * IDT correctly otherwise QEMU will triple fault and simple close
     * without showing you any output */
    // printf("Enabling Interrupts\n");
    
    // sti(); // end critical section and enable interrupts

#ifdef RUN_TESTS
    /* Run tests */
    // launch_tests();
#endif
    /* Execute the first program ("shell") ... */

    clear();

    execute((uint8_t*)"shell");
    sti();

    

    /* Spin (nicely, so we don't chew up cycles) */
    asm volatile (".1: hlt; jmp .1;");
}
