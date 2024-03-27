#include <stdlib.h>
#include "nios2_ctrl_reg_macros.h"

void plot_pixel(int x, int y, short int line_color);
void wait_for_vsync();

volatile int pixel_buffer_start; // global variable

void convert_image(const uint8_t* img_array[]){
    //convert the image to 16 bit short ints
    

}

void plot_pixel(int x, int y, short int line_color) {
    volatile short int *one_pixel_address;
    one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);
    *one_pixel_address = line_color;
}

void enableInterrupts(void){
    config_PS2();

    NIOS2_WRITE_IENABLE(0x3);
    //enable interrupts in NIOS II turn on the pie bit
    NIOS2_WRITE_STATUS(0x1);
}