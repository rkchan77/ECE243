#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

volatile uint32_t* pixel_ctrl_ptr = (void*)0xFF203020;
volatile uint32_t* Video_in_controller = (void*)0xFF20306C;
volatile uint16_t* Buffer = (void*)0x08000000;

int pixel_buffer_start; 

#define MAX_X 320
#define MAX_Y 240

void wait_for_vsync();

uint16_t read_video_pixel(int x, int y) {
    return *(volatile uint16_t*)(((int)Buffer) + (y << 10) + (x << 1));
}

void plot_pixel(int x, int y, uint16_t line_color)
{
    volatile uint16_t *one_pixel_address;

    one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);

    *one_pixel_address = line_color;
}

volatile short int Buffer1[240][512];
volatile short int Buffer2[240][512];

void resetVideo() {
    /* set front pixel buffer to Buffer 1 */
    *(pixel_ctrl_ptr + 1) = (int) &Buffer1; // first store the address in the  back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer

    /* set back pixel buffer to Buffer 2 */
    *(pixel_ctrl_ptr + 1) = (int) &Buffer2;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    clear_screen(); // pixel_buffer_start points to the pixel buffer
}


void wait_for_vsync(){
    // Set s bit to 1
	*pixel_ctrl_ptr = 1;
	
    // Mask bit 0 of status register
	int s_bit = *(pixel_ctrl_ptr + 3) & 1;

	// Poll s bit to detect when compiler is finished drawing to VGA screen
	while(s_bit != 0){
		s_bit = *(pixel_ctrl_ptr + 3) & 1;
	}
}

void clear_screen(){
    for (int x = 0; x < 320; x++){
        for (int y = 0; y < 240; y++){
            plot_pixel(x, y, 0);
        }
    }
}

int main() {
    *Video_in_controller = 0x4;

    resetVideo();
    
    while (1) {
        for (int x = 0; x < MAX_X; x++) {
            for (int y = 0; y < MAX_Y; y++) {
                uint16_t value = read_video_pixel(x,y);
                if (x > MAX_X/2) value -= read_video_pixel(x+1, y);
                //uint16_t value = 0xFF00;
                plot_pixel(x, y, value);
            }
        }
        wait_for_vsync();
        pixel_buffer_start = *(pixel_ctrl_ptr + 1);
    }
}