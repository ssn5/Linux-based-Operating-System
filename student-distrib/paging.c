#include "paging.h"
#include "system_calls.h"

unsigned int PDE_index = 32;

/* initializing paging
* INPUTS: none
* OUTPUTS: none
* RETURN: none
* DESCRIPTION: setting up paging
*/
void initialize_paging(){
    unsigned int i;   
    //go through each element in the page directory
    for (i = 0; i < 1024; i++){
        //set 4kb entry
        if (i == 0){
            pde[i].page_dir_vid.table_base_add = (unsigned int) pte >> 12;  //set base address to pte; right shift by 12 bits to fit in 31:12 of pde entry
            pde[i].page_dir_vid.avail = 0                               ;   //set parameter to 0
            pde[i].page_dir_vid.g = 0         ; //set parameter to 0
            pde[i].page_dir_vid.ps = 0            ; //set parameter to 0
            pde[i].page_dir_vid.reserved = 0       ;    //set parameter to 0
            pde[i].page_dir_vid.a = 0             ; //set parameter to 0
            pde[i].page_dir_vid.pcd = 0          ;  //set parameter to 0
            pde[i].page_dir_vid.pwt = 0           ; //set parameter to 0
            pde[i].page_dir_vid.us = 0            ; //set parameter to 0
            pde[i].page_dir_vid.rw = 1            ; //set parameter to 1
            pde[i].page_dir_vid.present = 1        ;    //set parameter to 1
        }
        //set 4mb entry
        else if (i == 1){
            pde[i].page_dir_kernel.page_base_add  = 1;  //set parameter to 1
            pde[i].page_dir_kernel.reserved       = 0;  //set parameter to 0
            pde[i].page_dir_kernel.pat            = 0;  //set parameter to 0    
            pde[i].page_dir_kernel.avail          = 0;        //set parameter to 0
            pde[i].page_dir_kernel.g              = 1;       //set parameter to 1     
            pde[i].page_dir_kernel.ps = 1       ;      //set parameter to 1
            pde[i].page_dir_kernel.d = 0        ;     //set parameter to 0
            pde[i].page_dir_kernel.a = 0        ;    //set parameter to 0
            pde[i].page_dir_kernel.pcd = 0      ;    //set parameter to 0
            pde[i].page_dir_kernel.pwt    =0    ;     //set parameter to 0
            pde[i].page_dir_kernel.us     =0    ;     //set parameter to 0
            pde[i].page_dir_kernel.rw     =1    ;    //set parameter to 1
            pde[i].page_dir_kernel.present=1    ;     //set parameter to 1
        }
        //disable these entries
        else{
            pde[i].page_dir_vid.table_base_add = 0; //set parameter to 0
            pde[i].page_dir_vid.avail = 0         ; //set parameter to 0
            pde[i].page_dir_vid.g = 0         ; //set parameter to 0
            pde[i].page_dir_vid.ps = 0            ; //set parameter to 0
            pde[i].page_dir_vid.reserved = 0       ;    //set parameter to 0
            pde[i].page_dir_vid.a = 0             ; //set parameter to 0
            pde[i].page_dir_vid.pcd = 0          ;  //set parameter to 0
            pde[i].page_dir_vid.pwt = 0           ; //set parameter to 0
            pde[i].page_dir_vid.us = 0            ; //set parameter to 0
            pde[i].page_dir_vid.rw = 1            ; //set parameter to 1
            pde[i].page_dir_vid.present = 0        ;    //set parameter to 0
        }
    }

    // for loop for each element in the page table
    for (i = 0; i < 1024; i++){
        //check if entry corresponds to video memory
        if (i == 184 || i == 185 || i == 186 || i == 187){ 
            pte[i].page_base_add = i; //set parameter to pageIndex
            pte[i].avail = 0         ; //set parameter to 0
            pte[i].g = 0         ; //set parameter to 0
            pte[i].pat = 0            ; //set parameter to 0
            pte[i].d = 0       ; //set parameter to 0
            pte[i].a = 0             ; //set parameter to 0
            pte[i].pcd = 0          ;//set parameter to 0
            pte[i].pwt = 0           ;//set parameter to 0
            pte[i].us = 0            ;//set parameter to 0
            pte[i].rw = 1            ;//set parameter to 1
            pte[i].present = 1        ;//set parameter to 1
        }
        //otherwise leave it empty
        else{
            pte[i].page_base_add = i;//set parameter to pageIndex
            pte[i].avail = 0         ;//set parameter to 0
            pte[i].g = 0         ;//set parameter to 0
            pte[i].pat = 0            ;//set parameter to 0
            pte[i].d = 0       ;//set parameter to 0
            pte[i].a = 0             ;//set parameter to 0
            pte[i].pcd = 0          ;//set parameter to 0
            pte[i].pwt = 0           ;//set parameter to 0
            pte[i].us = 0            ;//set parameter to 0
            pte[i].rw = 1            ;//set parameter to 1
            pte[i].present = 0        ;//set parameter to 0
        }
    }

    loadPageDirectory((unsigned int *)pde); // load page directory with page directory address
    enablePaging(); //switch on paging
}

/* executable_page
* INPUTS: none
* OUTPUTS: none
* RETURN: none
* DESCRIPTION: creates new page for current program being loaded
*/
void executable_page(){
    pde[PDE_index].page_dir_kernel.page_base_add  = get_global_pid() + 2;  //set parameter to 1
    pde[PDE_index].page_dir_kernel.reserved       = 0;  //set parameter to 0
    pde[PDE_index].page_dir_kernel.pat            = 0;  //set parameter to 0    
    pde[PDE_index].page_dir_kernel.avail          = 0;        //set parameter to 0
    pde[PDE_index].page_dir_kernel.g              = 1;       //set parameter to 1     
    pde[PDE_index].page_dir_kernel.ps = 1       ;      //set parameter to 1
    pde[PDE_index].page_dir_kernel.d = 0        ;     //set parameter to 0
    pde[PDE_index].page_dir_kernel.a = 0        ;    //set parameter to 0
    pde[PDE_index].page_dir_kernel.pcd = 0      ;    //set parameter to 0
    pde[PDE_index].page_dir_kernel.pwt    = 0    ;     //set parameter to 0
    pde[PDE_index].page_dir_kernel.us     = 1    ;     //set parameter to 0
    pde[PDE_index].page_dir_kernel.rw     = 1    ;    //set parameter to 1
    pde[PDE_index].page_dir_kernel.present = 1    ;     //set parameter to 1
    //PDE_index++;
    loadPageDirectory((unsigned int *)pde);
}

/* disable_page
* INPUTS: none
* OUTPUTS: none
* RETURN: none
* DESCRIPTION: creates new page for current program being loaded
*/
void disable_page(uint32_t vmem_loc){
    pde[0].page_dir_vid.table_base_add = (unsigned int) pte >> 12;

    pte[vmem_loc/(FOUR_KB)].page_base_add = vmem_loc/(FOUR_KB); //set parameter to pageIndex

    pte[vmem_loc/(FOUR_KB)].present = 1        ;//set parameter to 1

    loadPageDirectory((unsigned int *)pde);
}

/* enable_page
* INPUTS: vmem_loc
* OUTPUTS: none
* RETURN: none
* DESCRIPTION: enabling paging for a giving video memory location
*/

void enable_page(uint32_t vmem_loc){
    pde[0].page_dir_vid.table_base_add = (unsigned int) pte >> 12;
 
    pte[vmem_loc/(FOUR_KB)].page_base_add = 184; //set parameter to pageIndex

    pte[vmem_loc/(FOUR_KB)].present = 1        ;//set parameter to 1

    loadPageDirectory((unsigned int *)pde);
}

/* schedule_visible_page
* INPUTS: none
* OUTPUTS: none
* RETURN: none
* DESCRIPTION: used by scheduler to do paginging for the visible terminal
*/
void schedule_visible_page(){
    pde[0].page_dir_vid.table_base_add = (unsigned int) pte >> 12;

    pte[184].page_base_add = 184; //set parameter to pageIndex

    loadPageDirectory((unsigned int *)pde);
}

/* schedule_invisible_page
* INPUTS: none
* OUTPUTS: none
* RETURN: none
* DESCRIPTION: used by scheduler to do paginging for the invisible terminal
*/
void schedule_invisible_page(uint32_t vmem_loc){
    pde[0].page_dir_vid.table_base_add = (unsigned int) pte >> 12;

    pte[184].page_base_add = vmem_loc/(FOUR_KB); //set parameter to pageIndex

    loadPageDirectory((unsigned int *)pde);
}

/* flush_tlb
* INPUTS: pid
* OUTPUTS: none
* RETURN: none
* DESCRIPTION: flushes tlb by writing to cr3 reg
*/
void flush_tlb(uint32_t curr_pid){
    pde[PDE_index].page_dir_kernel.page_base_add  = curr_pid + 2;
    loadPageDirectory((unsigned int *)pde);
}

/* vidmem_assign
* INPUTS: none
* OUTPUTS: none
* RETURN: none
* DESCRIPTION: assigning new page for video memory
*/
void vidmem_assign(int vid_addr){
    int vidmem_pde_index = vid_addr >> 22;

    pde[vidmem_pde_index].page_dir_vid.table_base_add = (unsigned int)vidmem_pte >> 12; // set base address to pte; right shift by 12 bits to fit in 31:12 of pde entry
    pde[vidmem_pde_index].page_dir_vid.avail = 0;                                // set parameter to 0
    pde[vidmem_pde_index].page_dir_vid.g = 0;                                    // set parameter to 0
    pde[vidmem_pde_index].page_dir_vid.ps = 0;                                   // set parameter to 0
    pde[vidmem_pde_index].page_dir_vid.reserved = 0;                             // set parameter to 0
    pde[vidmem_pde_index].page_dir_vid.a = 0;                                    // set parameter to 0
    pde[vidmem_pde_index].page_dir_vid.pcd = 0;                                  // set parameter to 0
    pde[vidmem_pde_index].page_dir_vid.pwt = 0;                                  // set parameter to 0
    pde[vidmem_pde_index].page_dir_vid.us = 1;                                   // set parameter to 0
    pde[vidmem_pde_index].page_dir_vid.rw = 1;                                   // set parameter to 1
    pde[vidmem_pde_index].page_dir_vid.present = 1;                              // set parameter to 1

    vidmem_pte[0].page_base_add = 184 + 1 + terminal_id; // set parameter to pageIndex
    vidmem_pte[0].avail = 0;         // set parameter to 0
    vidmem_pte[0].g = 0;             // set parameter to 0
    vidmem_pte[0].pat = 0;           // set parameter to 0
    vidmem_pte[0].d = 0;             // set parameter to 0
    vidmem_pte[0].a = 0;             // set parameter to 0
    vidmem_pte[0].pcd = 0;           // set parameter to 0
    vidmem_pte[0].pwt = 0;           // set parameter to 0
    vidmem_pte[0].us = 1;            // set parameter to 0
    vidmem_pte[0].rw = 1;            // set parameter to 1
    vidmem_pte[0].present = 1;       // set parameter to 1

    // flush tlb
    asm volatile(
        "mov %%cr3, %%eax;"
        "mov %%eax, %%cr3;"
        :
        :
        :"%eax"
    );
}
