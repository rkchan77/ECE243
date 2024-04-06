#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "address_map_nios2.h"


volatile int* pixel_ctrl_ptr = (int*)PIXEL_BUF_CTRL_BASE;
volatile int* Buffer = (int*)FPGA_PIXEL_BUF_BASE;

extern int pixel_buffer_start; 

bool mirrorFilter = true;
bool invertFilter = false;
bool sepiaFilter = false; 
bool demonFilter = false;
bool randomFilter = false;

#define MAX_X 320
#define MAX_Y 240


uint16_t read_video_pixel(int x, int y);
void plot_pixel_vid(int x, int y, uint16_t line_color);
void wait_for_vsync_vid();
void liveVideo();