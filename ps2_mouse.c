#include "address_map_nios2.h"

#define LEDs ((volatile long *)LEDR_BASE)

void config_PS2(void){
    //enable interrupts from PS2 by setting RE to 1
    volatile int *PS2_ptr = (int *)PS2_BASE;
    *(PS2_ptr + 1) = 0x01;
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

    if((byte2 == 0xAA) && (byte3 == 0x00)){
        *(PS2_ptr) = 0xF4;
    }
    *LEDs = byte3;
}
