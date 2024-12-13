// Note: Most of the code comes from the MacResearch OpenCL podcast

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <iostream>

#include <omp.h>

struct BMPHeader {
    char bfType[2]; /* "BM" */
    int bfSize; /* Size of file in bytes */
    int bfReserved; /* set to 0 */
    int bfOffBits; /* Byte offset to actual bitmap data (= 54) */
    int biSize; /* Size of BITMAPINFOHEADER, in bytes (= 40) */
    int biWidth; /* Width of image, in pixels */
    int biHeight; /* Height of images, in pixels */
    short biPlanes; /* Number of planes in target device (set to 1) */
    short biBitCount; /* Bits per pixel (24 in this case) */
    int biCompression; /* Type of compression (0 if no compression) */
    int biSizeImage; /* Image size, in bytes (0 if no compression) */
    int biXPelsPerMeter; /* Resolution in pixels/meter of display device */
    int biYPelsPerMeter; /* Resolution in pixels/meter of display device */
    int biClrUsed; /* Number of colors in the color table (if 0, use maximum allowed by biBitCount) */
    int biClrImportant; /* Number of important colors. If 0, all colors are important */
};

int write_bmp(const char *filename, int width, int height, char *rgb)
{
    int i, j, ipos;
    int bytesPerLine;
    unsigned char *line;
    FILE *file;
    struct BMPHeader bmph;

    /* The length of each line must be a multiple of 4 bytes */
    bytesPerLine = (3 * (width + 1) / 4) * 4;

    strncpy(bmph.bfType, "BM", 2);
    bmph.bfOffBits = 54;
    bmph.bfSize = bmph.bfOffBits + bytesPerLine * height;
    bmph.bfReserved = 0;
    bmph.biSize = 40;
    bmph.biWidth = width;
    bmph.biHeight = height;
    bmph.biPlanes = 1;
    bmph.biBitCount = 24;
    bmph.biCompression = 0;
    bmph.biSizeImage = bytesPerLine * height;
    bmph.biXPelsPerMeter = 0;
    bmph.biYPelsPerMeter = 0;
    bmph.biClrUsed = 0;
    bmph.biClrImportant = 0;

    file = fopen (filename, "wb");
    if (file == NULL) return(0);

    fwrite(&bmph.bfType, 2, 1, file);
    fwrite(&bmph.bfSize, 4, 1, file);
    fwrite(&bmph.bfReserved, 4, 1, file);
    fwrite(&bmph.bfOffBits, 4, 1, file);
    fwrite(&bmph.biSize, 4, 1, file);
    fwrite(&bmph.biWidth, 4, 1, file);
    fwrite(&bmph.biHeight, 4, 1, file);
    fwrite(&bmph.biPlanes, 2, 1, file);
    fwrite(&bmph.biBitCount, 2, 1, file);
    fwrite(&bmph.biCompression, 4, 1, file);
    fwrite(&bmph.biSizeImage, 4, 1, file);
    fwrite(&bmph.biXPelsPerMeter, 4, 1, file);
    fwrite(&bmph.biYPelsPerMeter, 4, 1, file);
    fwrite(&bmph.biClrUsed, 4, 1, file);
    fwrite(&bmph.biClrImportant, 4, 1, file);

    line = (unsigned char *) malloc(bytesPerLine);
    if (line == NULL) {
        fprintf(stderr, "Can't allocate memory for BMP file.\n");
        return(0);
    }

    for (i = height - 1; i >= 0; i--) {
        for (j = 0; j < width; j++) {
            ipos = 3 * (width * i + j);
            line[3*j] = rgb[ipos + 2];
            line[3*j+1] = rgb[ipos + 1];
            line[3*j+2] = rgb[ipos];
        }
        fwrite(line, bytesPerLine, 1, file);
    }
    free(line);
    fclose(file);

    return(1);
}

void render(char *out, int width, int height) {
  int x,y;

  #pragma omp parallel for private(y) schedule(dynamic)
  for(x=0;x<width;x++) {
  	for(y=0;y<height; y++) {		
  		int index = 3*width*y + x*3;
  		float x_origin = ((float) x/width)*3.25 - 2;
  		float y_origin = ((float) y/width)*2.5 - 1.25;

  		float xi = 0.0;
  		float yi = 0.0;

  		int iteration = 0;
  		int max_iteration = 5000;

  		while(xi*xi + yi*yi <= 4 && iteration < max_iteration) {
  			float xtemp = xi*xi - yi*yi + x_origin;
  			yi = 2*xi*yi + y_origin;
  			xi = xtemp;
  			iteration++;
  		}

  		if(iteration == max_iteration) {
            out[index] = 0;
            out[index + 1] = 0;
            out[index + 2] = 0;
  		} else {
            out[index] = iteration;
            out[index + 1] = iteration;
            out[index + 2] = iteration;
  		}
  	}
  }
}

void run(int width, int height) {
	// Multiply by 3 here, since we need red, green and blue for each pixel
	size_t buffer_size = sizeof(char) * width * height * 3;
	char *host_image = (char *) malloc(buffer_size);
  
	render(host_image, width, height);
  
	write_bmp("output.bmp", width, height, host_image);
  
	free(host_image);
}

double timestamp(){
    struct timeval tempoValor;
    gettimeofday(&tempoValor, 0);
    return tempoValor.tv_sec + (tempoValor.tv_usec / 1e6);
}

int main(int argc, char const * argv[]) {

	if(argc < 3) {
		printf("Please use: ./mandelbrot dim_x dim_y\n");
	}
	int dim_x = atoi(argv[1]);
	int dim_y = atoi(argv[2]);

	double tinit = timestamp();
	run(dim_x, dim_y);
	double tend = timestamp();

	printf("Execution time: %lf \n", tend-tinit);

	return 0;
}
