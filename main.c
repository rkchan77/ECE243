#include <stdlib.h>

#include "address_map_nios2.h"
#include "interrupts.h"
#include "nios2_ctrl_reg_macros.h"
#include "img.h"

/* FUNCTION DECLARATIONS */
void plot_pixel(int x, int y, short int line_color);
void draw_image(const short int img_240x320[240][320], unsigned int xInitial, unsigned int yInitial, 
unsigned int width, unsigned int height);
void drawIcon(const short int img[40][47], unsigned int xInitial, unsigned int yInitial,
unsigned int width, unsigned int height);
void clearIcon(const short int img[240][320], unsigned int xInitial, unsigned int yInitial, unsigned int width, 
unsigned int height);
void entire_screen(short int colour);
void wait_for_vsync();
void draw_text(int x, int y, char* text_ptr);
void clear_text(int x, int y, int length);
void startScreen();

/*GLOBAL VARIABLES*/
volatile int pixel_buffer_start;  
short int Buffer1[240][512];
short int Buffer2[240][512];
extern volatile int edgeDetection = 0;
extern volatile int brightness = 0;
extern volatile int spaceBarPressed = 0;
extern volatile int keyPress = 0;

int xLocation[3];

int main(void) { 
  volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
  config_PS2();
  config_KEYS();
  enableInterrupts(); 

  // set spaceBarPressed to 1 if its pressed;
  startScreen();
	entire_screen(0x00);
	wait_for_vsync();
	pixel_buffer_start = *(pixel_ctrl_ptr + 1);
	
	mainMenu();

  while(1){
    /*
    - draw title screen "Press space bar to start"
    - draw main menu "press key0 for live video, key1 for image processing"
    - if key0 is pressed...
    - if key1 is pressed:
    - draw options page: 1 for edge detection, 2 for brightness change, 3 for halftone, 4 for original image
    - draw image

    */
  }  

}

void plot_pixel(int x, int y, short int colour) {
  volatile short int* one_pixel_address;
  one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);
  *one_pixel_address = colour;
}

void draw_image(const short int img_240x320[240][320], unsigned int xInitial, unsigned int yInitial,
                unsigned int width, unsigned int height) {

  unsigned int xFinal = xInitial + width;
  unsigned int yFinal = yInitial + height;

  for (unsigned int x = xInitial; x < xFinal; x++) {
    for (unsigned int y = yInitial; y < yFinal; y++) {
      // make sure the pixel is within the bounds of the screen
      if (x < 320 && y < 240 && x >= 0 && y >= 0) {
        plot_pixel(x, y, img_240x320[y][x]);
      }
    }
  }
}

void drawIcon(const short int img[40][47], unsigned int xInitial, unsigned int yInitial,
                unsigned int width, unsigned int height) {

  for (unsigned int x = 0; x < width; x++) {
    for (unsigned int y = 0; y < height; y++) {
		if(img[y][x] == 15416){
			continue;
		}
		unsigned int xPlot = xInitial + x;
		unsigned int yPlot = yInitial + y;
      // make sure the pixel is within the bounds of the screen
      if (xPlot < 320 && yPlot < 240 && xPlot >= 0 && yPlot >= 0) {
        plot_pixel(xPlot, yPlot, img[y][x]);
      }
    }
  }
}

void clearIcon(const short int img[240][320], unsigned int xInitial, unsigned int yInitial, unsigned int width, unsigned int height){
	for (unsigned int x = 0; x < width; x++) {
    	for (unsigned int y = 0; y < height; y++) {
			unsigned int xPlot = xInitial + x;
			unsigned int yPlot = yInitial + y;
      	// make sure the pixel is within the bounds of the screen
      	if (xPlot < 320 && yPlot < 240 && xPlot >= 0 && yPlot >= 0) {
        	plot_pixel(xPlot, yPlot, img[yPlot][xPlot]);
      	}
    }
  }
}

void entire_screen(short int colour) {
  for (int x = 0; x < 320; x++) {
    for (int y = 0; y < 240; y++) {
      plot_pixel(x, y, colour);
      // draw for every pixel on the screen
    }
  }
}

//waits for the display to stop drawin to avoid tearing
void wait_for_vsync(){
	volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
	*pixel_ctrl_ptr = 1; //set s bit to 1
	
	int s_bit = *(pixel_ctrl_ptr + 3) & 1;
	// poll s bit to see when its done drawing
	while(s_bit != 0){
		s_bit = *(pixel_ctrl_ptr + 3) & 1;
	}
}

/* subroutine to draw a string of text onto the VGA
 for characters, the entire screen is represented as a rectangular grid of 80
 cols (x) x 60 rows (y) */
void draw_text(int x, int y, char* text_ptr) {
  volatile char* character_buffer = (char*)FPGA_CHAR_BASE;

  /* each character is 1 byte in memory so to find determine the location, add
   * {x,y} offset to the base address*/
  int offset = (y << 7) + x;
  while (*(text_ptr)) {
    *(character_buffer + offset) = *(text_ptr);
    ++text_ptr;
    ++offset;
  }
}

void clear_text(int x, int y, int length) {
    int offset;
    volatile char *character_buffer = (char *)FPGA_CHAR_BASE; // video character buffer
    offset = (y << 7) + x;
    for (int i = 0; i < length; ++i) {
        *(character_buffer + offset + i) = 0; // write a space character
    }
}

void startScreen(){
  volatile int * pixel_ctrl_ptr = (int *)0xFF203020;

  /* set front pixel buffer to Buffer 1 */
  *(pixel_ctrl_ptr + 1) = (int) &Buffer1; // first store the address of Buffer1 in the back buffer
  /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr; // pixel_buffer_start points to the pixel buffer
    entire_screen(0xFFFF);
	
    /* set back pixel buffer to Buffer 2 */
    *(pixel_ctrl_ptr + 1) = (int) &Buffer2;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer
    entire_screen(0xFFFF); // pixel_buffer_start points to the pixel buffer
	
  int yLocation = 180;
	xLocation[0] = 250;
	int deltaX = -1;
	
	draw_image(titlePage, 0,0, 320, 240);	
	wait_for_vsync();
	pixel_buffer_start = *(pixel_ctrl_ptr + 1);
	draw_image(titlePage, 0,0, 320, 240);	
	
	
	while(1){
  	char text_top_row[60] = "Welcome to Image Processor & Live Video Display\0";
  	char text_bottom_row[40] = "Press the space bar to...\0";
  	char start[40] = "START!\0";
  	/* update color */
  	draw_text(16, 16, text_top_row);
  	draw_text(29, 23, text_bottom_row);
  	draw_text(37, 40, start);
		
		clearIcon(titlePage, xLocation[2], yLocation, 47, 40);
		drawIcon(cursor, xLocation[0], yLocation, 47, 40);
		if(xLocation[0] == 150){
			deltaX = 1;
		} else if (xLocation[0] == 250){
			deltaX = -1;
		}
		xLocation[2] = xLocation[1];
		xLocation[1] = xLocation[0];
		xLocation[0] += deltaX;
		
		wait_for_vsync();
		pixel_buffer_start = *(pixel_ctrl_ptr + 1);

    if(spaceBarPressed){
      clear_text(0, 16, 1000);
  	  clear_text(0, 23, 1000);
	    clear_text(0, 37, 1000);
      break;
    }
    wait_for_vsync();
		pixel_buffer_start = *(pixel_ctrl_ptr + 1);
	}
}
