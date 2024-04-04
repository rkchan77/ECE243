#define IMAGE_HEIGHT 240
#define IMAGE_WIDTH 320

// Output array for edge-detected image
unsigned short int edges[IMAGE_HEIGHT][IMAGE_WIDTH] = {0};
unsigned short int grayscale[IMAGE_HEIGHT][IMAGE_WIDTH] = {0};
unsigned short int brightness[IMAGE_HEIGHT][IMAGE_WIDTH] = {0};
void rgb_to_grayscale(const unsigned short int rgb[240][320]);
void sobel(const unsigned short int image[240][320]);
void adjust_brightness(const unsigned short int rgb[240][320], int brightness_change);

// Function to convert a 2D array of short int RGB values to grayscale
void rgb_to_grayscale(const unsigned short int rgb[240][320]) {
    for (int y = 0; y < 240; y++) {
        for (int x = 0; x < 320; x++) {
            // Extract RGB components 
            unsigned short int b = rgb[y][x] & 0x1F;        // Blue component
			unsigned short int g = (rgb[y][x] >> 5) & 0x3F;   // Green component
            unsigned short int r = (rgb[y][x] >> 11) & 0x1F;  // Red component
            
            // Compute luminance (weighted sum of RGB channels) for grayscale image
            unsigned short int gray = (short int)((0.2126 * r) + (0.7152 * g) + (0.0722 * b));
			
            // Store grayscale value
            grayscale[y][x] = gray;
        }
    }
}

// Assuming image is a 2D array representing the grayscale image
// Also assuming image width and height are known

void sobel(const unsigned short int image[240][320]){
	// Sobel operator kernels:
	int sobel_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
	// to detect the horizontal edges

	int sobel_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};
	// to detect the vertical edges
	
	// Perform convolution with Sobel operator
	for (int y = 1; y < IMAGE_HEIGHT - 1; y++) {
    	for (int x = 1; x < IMAGE_WIDTH - 1; x++) {
        	int gradient_x = 0;
        	int gradient_y = 0;

        	// Compute gradient in x direction
        for (int ky = -1; ky <= 1; ky++) {
            for (int kx = -1; kx <= 1; kx++) {
                gradient_x += image[y + ky][x + kx] * sobel_x[ky + 1][kx + 1];
            }
        }

        // Compute gradient in y direction
        for (int ky = -1; ky <= 1; ky++) {
            for (int kx = -1; kx <= 1; kx++) {
                gradient_y += image[y + ky][x + kx] * sobel_y[ky + 1][kx + 1];
            }
        }

        // Compute magnitude of gradient
        edges[y][x] = abs(gradient_x) + abs(gradient_y);
    }
}
	
}

// brightness_change value can go between -63 and 63 for maximum and minimum brightness
void adjust_brightness(const unsigned short int rgb[240][320], int brightness_change) {
    // Iterate over each pixel in the 320x240 array
    for (int x = 0; x < 320; x++) {
        for (int y = 0; y < 240; y++) {
            // Extract RGB components from the current pixel
            unsigned short int r = (rgb[y][x] >> 11) & 0x1F;  // Red component
            unsigned short int g = (rgb[y][x] >> 5) & 0x3F;   // Green component
            unsigned short int b = rgb[y][x] & 0x1F;          // Blue component

            // Adjust brightness for each component
			//can't allow the rgb values to go above 31 (for r and b) or 63 (for g) or below 0
            r = (r + brightness_change > 31) ? 31 : (r + brightness_change < 0) ? 0 : r + brightness_change;
            g = (g + brightness_change > 63) ? 63 : (g + brightness_change < 0) ? 0 : g + brightness_change;
            b = (b + brightness_change > 31) ? 31 : (b + brightness_change < 0) ? 0 : b + brightness_change;

            // Combine the adjusted RGB components into a single RGB value
            brightness[y][x] = (r << 11) | (g << 5) | b;
        }
    }
}