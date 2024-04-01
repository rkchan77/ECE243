#include "address_map_nios2.h"
#include "nios2_ctrl_reg_macros.h"

extern int keyPress;

void config_PS2(void){
    volatile int *PS2_ptr = (int *)PS2_BASE;
    *(PS2_ptr) = 0xff; //reset 
    *(PS2_ptr + 1) = 0x1; //enable interrupts from PS2 by setting RE to 1
}

void config_KEYS(){
  volatile int *KEY_ptr = (int *)KEY_BASE; // address for the pushbuttons
  *(KEY_ptr + 2) = 0x3; // enable interrupts for all pushbuttons
}

void enableInterrupts(void) {
  //allow IRQ 1 (keys) and IRQ 7 (PS/2) to cause interrupts
  NIOS2_WRITE_IENABLE(0x82);
  // enable interrupts in NIOS II turn on the pie bit
  NIOS2_WRITE_STATUS(0x1);
}

void disableInterrupts(int IRQ){
  //disable interrupts from IRQ 1 but allow IRQ 7 to cause interrupts
  if(IRQ == 1){
    NIOS2_WRITE_IENABLE(0x80);
  //disable interrupts from IRQ 7 but allow IRQ 1 to cause interrupts
  } else if (IRQ == 7){
    NIOS2_WRITE_IENABLE(0x2);
  } else {
  // disable ALL interrupts by setting pie bit to 0
    NIOS2_WRITE_STATUS(0x0);

  }
}

void interrupt_handler(void){
  int ipending;
  NIOS_READ_IPENDING(ipending);

  if(ipending & 0x2){
    pushbutton_ISR();
  } else if (ipending & 0x80){
    ps2_ISR();
  }
}

void ps2_ISR(void) {
  unsigned char byte1 = 0;
  unsigned char byte2 = 0;
  unsigned char byte3 = 0;

  volatile int *PS2_ptr = (int *)PS2_BASE;
  int PS2_data, RVALID;

  PS2_data = *(PS2_ptr);  // read data register in the PS / 2 port
  RVALID = (PS2_data & 0x8000);

  if(RVALID){
        //reading from the reg provides the data at the head of the FIFO in the data field
        // it also decrements the data register field by 1
        //reading RAVAIL provides number of entries in the FIFO
      byte1 = byte2;
      byte2 = byte3;
      byte3 = PS2_data & 0xFF;

     
    }
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