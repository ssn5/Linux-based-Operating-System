#define ASM     1

.text

.globl loadPageDirectory #declare function as global
loadPageDirectory:
#stack setup
push %ebp
mov %esp, %ebp  
#set cr3 with pde address
mov 8(%esp), %eax
mov %eax, %cr3
#set bit 4(pse) of cr4
movl %cr4, %eax
orl $0x00000010, %eax #or with the 5th bit (bit 4)
movl %eax, %cr4 
#stack teardown
mov %ebp, %esp
pop %ebp
ret

.globl enablePaging #declare function as global
enablePaging:
#stack setup
push %ebp
mov %esp, %ebp
#set cr0 with pg and pe flag
mov %cr0, %eax
or $0x80000001, %eax    # or with 32nd bit and 1st bit
mov %eax, %cr0
#stack teardown
mov %ebp, %esp
pop %ebp
ret

