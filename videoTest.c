#include <stdlib.h>
#include <stdio.h>

typedef unsigned int uint32_t;
typedef unsigned short int uint16_t;


//volatile uint32_t* Front_buffer = (void*)0xFF203020;
volatile uint32_t* Video_in_controller = (void*)0xFF20306C;
volatile uint16_t* Buffer = (void*)0x08000000;
int pixel_buffer_start; // global variable

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

volatile short int Buffer1[240][512]; // 240 rows, 512 (320 + padding) columns
volatile short int Buffer2[240][512];

void resetVideo() {
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    // declare other variables(not shown)
    // initialize location and direction of rectangles(not shown)

    /* set front pixel buffer to Buffer 1 */
    *(pixel_ctrl_ptr + 1) = (int) &Buffer1; // first store the address in the  back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    //clear_screen(); // pixel_buffer_start points to the pixel buffer

    /* set back pixel buffer to Buffer 2 */
    *(pixel_ctrl_ptr + 1) = (int) &Buffer2;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    //clear_screen(); // pixel_buffer_start points to the pixel buffer
}

// code for subroutines (not shown)

void wait_for_vsync(){
	volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
	*pixel_ctrl_ptr = 1; //set s bit to 1
	
	int s_bit = *(pixel_ctrl_ptr + 3) & 1;
	// poll s bit to see when its done drawing
	while(s_bit != 0){
		s_bit = *(pixel_ctrl_ptr + 3) & 1;
	}
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
}

// code not shown for clear_screen() and draw_line() subroutines

void clear_screen(){
    for (int x = 0; x < 320; x++){
        for (int y = 0; y < 240; y++){
            plot_pixel(x, y, 0);
            //draw a black pixel for every pixel on the screen
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
    }
    //while(1) {
    //    printf("%8x\n", *Buffer);
    //}
}