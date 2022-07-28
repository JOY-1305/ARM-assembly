#include "hardware/regs/addressmap.h"
#include "hardware/regs/m0plus.h"

.syntax unified                 @ Specify unified assembly syntax
.cpu    cortex-m0plus           @ Specify CPU type is Cortex M0+
.thumb                          @ Specify thumb assembly for RP2040
.global main_asm                @ Provide program starting address to the linker
.align 4                        @ Specify code alignment

.equ    SLEEP_TIME, 500         @ Specify the sleep time (in ms)
.equ    LED_GPIO_PIN, 25        @ Specify the pin that the LED is connected to
.equ    LED_GPIO_OUT, 1         @ Specify the direction of the GPIO pin
.equ    LED_VALUE_ON, 1         @ Specify the value that turns the LED "on"
.equ    LED_VALUE_OFF, 0        @ Specify the value that turns the LED "off"
.equ    SVC_ISR_OFFSET, 0x2C    @ The SVC is entry 11 in the vector table
.equ    SVC_MAX_INSTRS, 0x01    @ Maximum allowed SVC subroutines

@ Entry point to the ASM portion of the program
main_asm:
    bl      init_gpio_led       @ Initialise the GPIO LED pin
    bl      install_svc_isr     @ Install the SVC interrupt service routine
loop:
    svc     #0                  @ Call the SVC ISR with value 0 (turns on LED)
    nop                         @ Add a no-op instruction for alignment after SVC
    bl      do_sleep            @ Short pause before proceeding
    svc     #1                  @ Call the SVC ISR with value 1 (turns off LED)
    nop                         @ Add a no-op instruction for alignment after SVC
    bl      do_sleep            @ Add a short pause before proceeding
    b       loop                @ Always jump back to the start of the loop

@ Subroutine used to introduce a short delay in the application
do_sleep:
 push     {lr}
 ldr     r0, =SLEEP_TIME             @ Set the value of SLEEP_TIME we want to wait for
 bl      sleep_ms                    @ Sleep until SLEEP_TIME has elapsed then toggle the LED GPIO pin
 pop     {pc}

@ Subroutine used to initialise the PI Pico built-in LED
init_gpio_led:
    push     {lr}
    movs    r0, #LED_GPIO_PIN           @ This value is the GPIO LED pin on the PI PICO board
    bl      asm_gpio_init               @ Call the subroutine to initialise the GPIO pin specified by r0
    movs    r0, #LED_GPIO_PIN           @ This value is the GPIO LED pin on the PI PICO board
    movs    r1, #LED_GPIO_OUT           @ We want this GPIO pin to be setup as an output pin
    bl      asm_gpio_set_dir            @ Call the subroutine to set the GPIO pin specified by r0 to state specified by r1
    pop    {pc}

@ Subroutine used to install the SVC interrupt service handler
install_svc_isr:
    ldr     r2, =(PPB_BASE + M0PLUS_VTOR_OFFSET)    @  load the memory location of vector table calculated by adding the offset of vector table with the base address
    ldr     r1, [r2]                                @  ldr means the load register ,the address or memory location of r2 is loaded in r1
    movs    r2, #SVC_ISR_OFFSET                     @  the value 0x2C is moved in register r2 and the flags are updated 
    add     r2, r1                                  @  the values of r1 and r2 are added and stored in r2
    ldr     r0, =svc_isr                            @  memory location of thumb function svc_isr is loaded in r0
    str     r0, [r2]                                @  stores the address value of r2 into register r0
    bx      lr                                      @  branch exchange instruction is set on link register

@ SVC interrupt service handler routine
.thumb_func                     @ Required for all interrupt service routines
svc_isr:
    push    {lr}                @  pushing the address to the link register
    ldr     r0, [sp, #0x1C]     @  the hex value 0x1C from the stack pointer is loaded in register r0 
    subs    r0, #0x2            @  subtracts the hex value 0x2 from r0 and stores it in the register  r0
    ldr     r0, [r0]            @  loads the address value of r0 in register r0
    ldr     r1, =#0xFF          @  loads the hex value 0xFF is in register r1
    ands    r0, r1              @  AND the values of r1 and r0 and updates the flags
    cmp     r0, #SVC_MAX_INSTRS @  compares the hex value 0x01 with r0
    bgt     svc_done            @  r0 branches to thumb function svc_done if r0 is greater than SVC_MAX_INSTRS 
    adr     r1, svc_jmptbl      @  the address value is loaded in r1
    lsls    r0, #2              @  ls is left shifted in order to shift the value in register r0 to the left 
    ldr     r1, [r1, r0]        @  the address of r0 added to r1 and then in register r1
    mov     pc, r1              @  the value of r1 is moved to pc i.e. program counter
svc_done:
    pop     {pc}                @  pop lr from the stack and places it in pc

@ First function of SVC subroutine - turn on the LED
svc_num0:
    movs    r0, #LED_GPIO_PIN
    movs    r1, #LED_VALUE_ON
    bl      asm_gpio_put
    
    b       svc_done            @ Branch back to the main ISR when done

@ Second function of SVC subroutine - turn off the LED
svc_num1:
    movs    r0, #LED_GPIO_PIN
    movs    r1, #LED_VALUE_OFF
    bl      asm_gpio_put
    b       svc_done            @ Branch back to the main ISR when done

@ SVC function entry jump table.
.align 2
svc_jmptbl:
    .word svc_num0              @ Entry zero goes to SVC function #0.
    .word svc_num1              @ Entry one goes to SVC function #1.
    .word 0                     @ Null termination of the jump table.

@ Set data alignment
.data
    .align 4