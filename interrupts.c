#include "address_map_nios2.h"
#include "nios2_ctrl_reg_macros.h"

volatile int edgeDetection = 0;
volatile int brightness = 0;
volatile int spaceBarPressed = 0;
volatile int keyPress = 0;

void config_PS2(void){
    volatile int *PS2_ptr = (int *)PS2_BASE;
    *(PS2_ptr) = 0xff; //reset 
    *(PS2_ptr + 1) = 0x1; //enable interrupts from PS2 by setting RE to 1
}

void config_KEYS(){
  volatile int *KEY_ptr = (int *)KEY_BASE; // address for the pushbuttons
  *(KEY_ptr + 2) = 0x3; // enable interrupts for all pushbuttons
}

void enableInterrupts(int IRQ) {
    //disable interrupts from KEYS but allow PS/2 to cause interrupts
  if(IRQ == 1){
    NIOS2_WRITE_IENABLE(0x80);
  //disable interrupts from PS/2 but allow KEYS to cause interrupts
  } else if (IRQ == 7){
    NIOS2_WRITE_IENABLE(0x2);
  } else {
  //allow IRQ 1 (keys) and IRQ 7 (PS/2) to cause interrupts
  NIOS2_WRITE_IENABLE(0x82);
  }
  // enable interrupts in NIOS II turn on the pie bit
  NIOS2_WRITE_STATUS(0x1);
}

void disableInterrupts(){
  // disable ALL interrupts by setting pie bit to 0
  NIOS2_WRITE_STATUS(0x0);
}

void ps2_ISR(void) {
  unsigned char byte1 = 0, byte2 = 0, byte3 = 0;

  volatile int *PS2_ptr = (int *)PS2_BASE;
  int PS2_data, RVALID;

  PS2_data = *(PS2_ptr);  // read data register in the PS / 2 port
  RVALID = (PS2_data & 0x8000); // flag indicating if valid data is present

  if(RVALID){
        //reading from the reg provides the data at the head of the FIFO in the data field
        // it also decrements the data register field by 1
        //reading RAVAIL provides number of entries in the FIFO
      byte1 = byte2; // to keep track of the recent history of key presses/releases
      byte2 = byte3;
      byte3 = PS2_data & 0xFF;

      volatile unsigned int compareBytes;

      compareBytes = (byte2 << 8) | byte3;
	  
      //break code
      if(compareBytes == (char) 0xf024){ //E key 
        edgeDetection = 1;
      } else if (compareBytes == (char) 0xf032){ //B key
        brightness = 1;
      } else if (compareBytes == (char) 0xf029){ // Space Bar
        spaceBarPressed = 1;
      }
    }
    return;
}

void pushbutton_ISR(void){
  volatile int* KEY_ptr = (int*)KEY_BASE;
  int press;

  press = *(KEY_ptr + 3); // read pushbutton interrupt reg
  *(KEY_ptr + 3) = press; // clear the interrupt

  if(press & 0x1){
    //key 0 was pressed
    keyPress = 0;
  } else if (press & 0x2){
    keyPress = 1;
  }

}

/* The assembly language code below handles CPU reset processing */
void the_reset(void)
/*******************************************************************************
* Reset code. By giving the code a section attribute with the name ".reset" we
* allow the linker program to locate this code at the proper reset vector
* address. This code just calls the main program.
******************************************************************************/
{
asm(".set noat"); /* Instruct the assembler NOT to use reg at (r1) as
* a temp register for performing optimizations */
asm(".set nobreak"); /* Suppresses a warning message that says that
* some debuggers corrupt regs bt (r25) and ba
* (r30)
*/
asm("movia r2, main"); // Call the C language main program
asm("jmp r2");
}
/* The assembly language code below handles CPU exception processing. This
* code should not be modified; instead, the C language code in the function
* interrupt_handler() can be modified as needed for a given application.
*/

void the_exception(void)
/*******************************************************************************
* Exceptions code. By giving the code a section attribute with the name
* ".exceptions" we allow the linker program to locate this code at the proper
* exceptions vector address.
* This code calls the interrupt handler and later returns from the exception.
******************************************************************************/
{
asm("subi sp, sp, 128");
asm("stw et, 96(sp)");
asm("rdctl et, ctl4");
asm("beq et, r0, SKIP_EA_DEC"); // Interrupt is not external
asm("subi ea, ea, 4"); /* Must decrement ea by one instruction
* for external interupts, so that the
* interrupted instruction will be run */
asm("SKIP_EA_DEC:");
asm("stw r1, 4(sp)"); // Save all registers
asm("stw r2, 8(sp)");
asm("stw r3, 12(sp)");
asm("stw r4, 16(sp)");
asm("stw r5, 20(sp)");
asm("stw r6, 24(sp)");
asm("stw r7, 28(sp)");
asm("stw r8, 32(sp)");
asm("stw r9, 36(sp)");
asm("stw r10, 40(sp)");
asm("stw r11, 44(sp)");
asm("stw r12, 48(sp)");
asm("stw r13, 52(sp)");
asm("stw r14, 56(sp)");
asm("stw r15, 60(sp)");
asm("stw r16, 64(sp)");
asm("stw r17, 68(sp)");
asm("stw r18, 72(sp)");
asm("stw r19, 76(sp)");
asm("stw r20, 80(sp)");
asm("stw r21, 84(sp)");
asm("stw r22, 88(sp)");
asm("stw r23, 92(sp)");
asm("stw r25, 100(sp)"); // r25 = bt (skip r24 = et, because it is saved
// above)
asm("stw r26, 104(sp)"); // r26 = gp
// skip r27 because it is sp, and there is no point in saving this
asm("stw r28, 112(sp)"); // r28 = fp
asm("stw r29, 116(sp)"); // r29 = ea
asm("stw r30, 120(sp)"); // r30 = ba
asm("stw r31, 124(sp)"); // r31 = ra
asm("addi fp, sp, 128");
asm("call interrupt_handler"); // Call the C language interrupt handler
asm("ldw r1, 4(sp)"); // Restore all registers
asm("ldw r2, 8(sp)");
asm("ldw r3, 12(sp)");
asm("ldw r4, 16(sp)");
asm("ldw r5, 20(sp)");
asm("ldw r6, 24(sp)");
asm("ldw r7, 28(sp)");
asm("ldw r8, 32(sp)");
asm("ldw r9, 36(sp)");
asm("ldw r10, 40(sp)");
asm("ldw r11, 44(sp)");
asm("ldw r12, 48(sp)");
asm("ldw r13, 52(sp)");
asm("ldw r14, 56(sp)");
asm("ldw r15, 60(sp)");
asm("ldw r16, 64(sp)");
asm("ldw r17, 68(sp)");
asm("ldw r18, 72(sp)");
asm("ldw r19, 76(sp)");
asm("ldw r20, 80(sp)");
asm("ldw r21, 84(sp)");
asm("ldw r22, 88(sp)");
asm("ldw r23, 92(sp)");
asm("ldw r24, 96(sp)");
asm("ldw r25, 100(sp)"); // r25 = bt
asm("ldw r26, 104(sp)"); // r26 = gp
// skip r27 because it is sp, and we did not save this on the stack
asm("ldw r28, 112(sp)"); // r28 = fp
asm("ldw r29, 116(sp)"); // r29 = ea
asm("ldw r30, 120(sp)"); // r30 = ba
asm("ldw r31, 124(sp)"); // r31 = ra
asm("addi sp, sp, 128");
asm("eret");
}

void interrupt_handler(void){
  int ipending;
  NIOS_READ_IPENDING(ipending);

  if(ipending & 0x2){
    pushbutton_ISR();
  } else if (ipending & 0x80){
    ps2_ISR();
  }
  return;
}