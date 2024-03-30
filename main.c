#include <stdlib.h>

#include "nios2_ctrl_reg_macros.h"
#include "interrupts.h"

void plot_pixel(int x, int y, short int line_color);
void wait_for_vsync();

volatile int pixel_buffer_start;  // global variable


void main(void){
  enableInterrupts();
}

void convert_image(const uint8_t* img_array[], int width, int height) {
  // convert the image to 16 bit short ints
 
  for (int y = 0; y < height; ++y){
    for(int x = 0; x < width; ++x){
    }
  }
}

void draw_image(int img[], unsigned int xInitial, unsigned int yInitial, unsigned int width, unsigned int height){
    int i = 0;
    unsigned int xFinal = xInitial + width;
    unsigned int yFinal = yInitial + height;

    for(unsigned int x = xInitial; x < xFinal; x++){
        for (unsigned int y = yInitial; y < yFinal; y++){
          //make sure the pixel is within the bounds of the screen
            if(x < 320 && y < 240 && x >= 0 && y >= 0){
                plot_pixel(x, y, img[i]);
                ++i;
            }
        }
    }
}

void clear_screen(){
    for (int x = 0; x < 320; x++){
        for (int y = 0; y < 240; y++){
            plot_pixel(x, y, 0);
            //draw a black pixel for every pixel on the screen
        }
    }
}

void plot_pixel(int x, int y, short int line_color) {
  volatile short int* one_pixel_address;
  one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);
  *one_pixel_address = line_color;
}