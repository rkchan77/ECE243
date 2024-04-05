#include "video.h"
#include "globals.h"

uint16_t read_video_pixel(int x, int y) {
    return *(volatile uint16_t*)(((int)Buffer) + (y << 10) + (x << 1));
}

void plot_pixel_vid(int x, int y, uint16_t line_color)
{
    volatile uint16_t *one_pixel_address;

    one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);

    *one_pixel_address = line_color;
}

void wait_for_vsync_vid(){
    // Set s bit to 1
	*pixel_ctrl_ptr = 1;
	
    // Mask bit 0 of status register
	int s_bit = *(pixel_ctrl_ptr + 3) & 1;

	// Poll s bit to detect when compiler is finished drawing to VGA screen
	while(s_bit != 0){
		s_bit = *(pixel_ctrl_ptr + 3) & 1;
	}
}

void liveVideo() {
    *Video_in_controller = 0x4;
    
    while (1) {
        for (int x = 0; x < MAX_X; x++) {
            for (int y = 0; y < MAX_Y; y++) {
                uint16_t value = read_video_pixel(x,y);
                if (mirrorFilter) {
                    if (x > MAX_X / 2) value = read_video_pixel(MAX_X - 1 - x, y);
                } else if (invertFilter) {
                    value = ~value & 0xFFFF;
                } else if (sepiaFilter) {
                    value = value & 0b1111111111000000;
                } else if (demonFilter) {
                    value = value & 0b1111100000000000;
                }else if (randomFilter) {
                     value -= read_video_pixel(x+1, y);
                }
                plot_pixel_vid(x, y, value);
            }
        }
        wait_for_vsync_vid();
        pixel_buffer_start = *(pixel_ctrl_ptr + 1);
    }
}
