#include "hardware/regs/addressmap.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/timer.h"
#include "hardware/regs/m0plus.h"


.syntax unified                 @ Specify unified assembly syntax
.cpu    cortex-m0plus           @ Specify CPU type is Cortex M0+
.thumb                          @ Specify thumb assembly for RP2040
.global main_asm                @ Provide program starting address to the linker
.align 4                        @ Specify code alignment

.equ    DFLT_ALARM_TIME, 1000000      @ set the default alarm timeout 
.equ    ALRM_ISR_OFFSET, 0x40         @ ALARM0 is int #0 in vector table entry 16


@ Entry point to the ASM portion of the program
main_asm:
    bl    install_alrm_isr                       @ subroutine is called to initailize the alarm isr
    ldr   r4, =DFLT_ALARM_TIME                   @ a alarm delay is set
    bl    set_alarm                              @ a new alarm is set
    ldr  r2, =(TIMER_BASE + TIMER_INTE_OFFSET)   @ enable alarm
    movs r1,#1                                   @ enable alarm
    str   r1, [r2]     

loop:
    wfi                                          @ Wait here for interrupt
    b     loop      
    bl    std_adc_func_read
   
install_alrm_isr:
    ldr     r2, =(PPB_BASE + M0PLUS_VTOR_OFFSET)        @ fetches the address of the RAM vector table using the (PPB_BASE + M0PLUS_VTOR_OFFSET) register
    ldr     r1, [r2]                                    @ Make r1 point to the location of entry point of the jump table
    movs    r2, #ALRM_ISR_OFFSET                        @ Store the address of the alrm_isr handler to the correct offset for ALARM0 in the vector table
    add     r2, r1                                      @ Find the location of this interrupt in this table and store in r2
    ldr     r0, =alrm_isr                               @ Load the entry point of this interrupt into r0
    str     r0, [r2]                                    @ Set the table location of R0 as the value caluated in r2
    ldr     r2, =(PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET)   @ Disable the ALARM0 IRQ by writing the correct value to (PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET)
    movs    r1, #1                                      @ Disable the ALARM0 IRQ by writing the correct value to (PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET)
    str     r1 ,[r2]                                    @ Disable the ALARM0 IRQ by writing the correct value to (PPB_BASE + M0PLUS_NVIC_ICPR_OFFSET)
    ldr     r2, =(PPB_BASE + M0PLUS_NVIC_ISER_OFFSET)   @ Enable the ALARM0 IRQ by writing the correct value to (PPB_BASE + M0PLUS_NVIC_ISER_OFFSET)
    movs    r1, #1                                      @ Enable the ALARM0 IRQ by writing the correct value to (PPB_BASE + M0PLUS_NVIC_ISER_OFFSET)
    str     r1 ,[r2]                                    @ Enable the ALARM0 IRQ by writing the correct value to (PPB_BASE + M0PLUS_NVIC_ISER_OFFSET)
    bx      lr                                          @ Branch to the point at which the intterupt was called (stored in lr)

set_alarm: 
    ldr     r1, =(TIMER_BASE + TIMER_TIMELR_OFFSET)  @ fetches current timer value for relavant address
    ldr     r2,[r1]                                  @ fetches current timer value for relavant address
    movs     r1, r4                                  @ fetches desired delay  
    add     r1, r1, r2                               @ Add desired delay to current timer 
    ldr     r2, =(TIMER_BASE + TIMER_ALARM0_OFFSET)  @ push desired delay to revlvant alarm adress
    str     r1, [r2]                                 @ push desired delay to revlvant alarm adress
    bx      lr                                       @ exit subrouthine

alrm_isr:
 
   push    {lr}                                      @ Store the link register to the stack as we will call nested subroutines
   ldr     r1, =(TIMER_BASE+TIMER_INTR_OFFSET)       @ Acknowledge interrupt has been recieved
   movs     r0 ,#1                                   @ Acknowledge interrupt has been recieved
   str     r0,[r1]                                   @ Acknowledge interrupt has been recieved
   bl      set_alarm
   pop     {pc}                                     @ Pop the link register from the stack to the program counter


@ Set data alignment
.data
    .align 4
