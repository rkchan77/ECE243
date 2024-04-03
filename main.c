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
void startScreen(int x);

/*GLOBAL VARIABLES*/
volatile int pixel_buffer_start;  
short int Buffer1[240][512];
short int Buffer2[240][512];
int keyPress;

void main(void) { 
  config_PS2();
  config_KEYS();
  enableInterrupts(); 

  // set spaceBarPressed to 1 if its pressed;
  int spaceBarPressed;
  startScreen(spaceBarPressed);

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
  
  volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
	pixel_buffer_start = *(pixel_ctrl_ptr);
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

void startScreen(spaceBarPressed){
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
	
  int xLocation[3];
  int yLocation = 180;
	xLocation[0] = 250;
	int deltaX = -1;
	
	draw_image(titlePage, 0,0, 320, 240);	
	wait_for_vsync();
	pixel_buffer_start = *(pixel_ctrl_ptr + 1);
	draw_image(titlePage, 0,0, 320, 240);	
	
	
	while(1){
  	clear_text(0, 16, 1000);
  	clear_text(0, 23, 1000);
	  clear_text(0, 45, 1000);
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
      break;
    }
	}
}

/* The assembly language code below handles CPU reset processing */
void the_reset(void) __attribute__((section(".reset")));
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
void the_exception(void) __attribute__((section(".exceptions")));
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
