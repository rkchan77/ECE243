#include <stdlib.h>

void clear_screen();
void plot_pixel(int x, int y, short int line_color);
void wait_for_vsync();

volatile int pixel_buffer_start; // global variable
short int Buffer1[480][640]; // 480 rows, 640 columns
short int Buffer2[480][640];

int main(void)
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020; // VGA Front Buffer Control Register

    /* Enable video streaming from the composite video input */
    volatile int * video_in_ctrl_ptr = (int *)0xFF20306C; // Video In Controller Register
    *video_in_ctrl_ptr = 0x4; // Set Bit 2 to enable video streaming

    /* Set up edge detection */
    volatile int * edge_detection_ctrl_ptr = (int *)0xFF302070; // Edge Detection Register
    *edge_detection_ctrl_ptr = 0x1; // Set Bit 0 to enable edge detection

    /* Set front pixel buffer to Buffer 1 */
    *(pixel_ctrl_ptr + 1) = (int) &Buffer1;
    wait_for_vsync(); // Wait for synchronization
    pixel_buffer_start = *pixel_ctrl_ptr; // Initialize pixel buffer pointer
    clear_screen(); // Clear the screen

    /* Set back pixel buffer to Buffer 2 */
    *(pixel_ctrl_ptr + 1) = (int) &Buffer2;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // Draw on the back buffer
    clear_screen(); // Clear the screen

    while (1)
    {
        wait_for_vsync(); // Wait for synchronization
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // Update back buffer
    }
}

void wait_for_vsync()
{
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    *pixel_ctrl_ptr = 1; // Set S bit to 1 to request sync

    int status = *(pixel_ctrl_ptr + 3); // Read status register
    while ((status & 0x01) != 0) // Wait for S bit to become 0
    {
        status = *(pixel_ctrl_ptr + 3);
    }
}

void clear_screen()
{
    for (int x = 0; x < 640; x++)
    {
        for (int y = 0; y < 480; y++)
        {
            plot_pixel(x, y, 0); // Draw black pixel
        }
    }
}

void plot_pixel(int x, int y, short int line_color)
{
    volatile short int *one_pixel_address;
    one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);
    *one_pixel_address = line_color;
}
