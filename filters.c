#define IMAGE_HEIGHT 240
#define IMAGE_WIDTH 320

// Output array for edge-detected image
short int edges[IMAGE_HEIGHT][IMAGE_WIDTH] = {0};
short int grayscale[240][320];
void rgb_to_grayscale(const short int (*rgb)[240][320], short int (*grayscale)[240][320]);
void sobel(const short int image[240][320]);

// Assuming image is a 2D array representing the grayscale image
// Also assuming image width and height are known

void sobel(const short int image[240][320]){
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