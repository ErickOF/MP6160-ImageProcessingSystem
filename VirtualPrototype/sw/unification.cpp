#include <string.h>
#include <stdio.h>
#include <../inc/address_map.hpp>
#include <math.h>

#define IPS_FILTER_KERNEL_SIZE 3
#define IMAG_ROWS 452
#define IMAG_COLS 640

// Non Efficient implementation of Integer SQRT. But compatible with RV32 instructions.
int intSqrt(int x) 
{
  unsigned int s = 0;
  for (unsigned int i = (1 << 15); i > 0; i >>= 1){
    if (((s+i) * (s+i)) <= x){
      s += i;
    }
  }
  return s;
}

int norm(int a, int b) 
{
  int norm_result = 0;

  norm_result = intSqrt(a*a+b*b); //sqrt(pow(a, 2) + pow(b, 2));

  return norm_result;
}

void unificate_img(unsigned char *x_img, unsigned char *y_img, unsigned char *unificated_img, int img_size, int channels)
{
  //Iterate over image
  for(unsigned char *x = x_img, *y = y_img, *u = unificated_img; x < x_img + img_size && y < y_img + img_size && u < unificated_img + img_size; x+=channels, y+=channels, u+=channels){
    int pixel_magnitude;
    int pixel_x = (int) *x;
    int pixel_y = (int) *y;
      
    pixel_magnitude = norm(pixel_x, pixel_y);

    if (pixel_magnitude > 255) {pixel_magnitude = 255;};
    *u = (unsigned char) pixel_magnitude;
  }
}

int main(void) {

  // unsigned char *local_window_ptr = new unsigned char[9];
  // unsigned char *result_ptr = new unsigned char;
  // unsigned char *filter = (unsigned char*)IMG_FILTER_KERNEL_ADDRESS_LO;
  // unsigned char *filter_output = (unsigned char*)IMG_FILTER_OUTPUT_ADDRESS_LO;
  int img_size = 452*640;
  int channels = 1;

  unsigned char *img_x = (unsigned char*) IMG_INPUT_ADDRESS_LO;
  unsigned char *img_y = (unsigned char*) IMG_INPUT_ADDRESS_LO + img_size;
  unsigned char *img_result = (unsigned char*) IMG_OUTPUT_ADDRESS_LO;

  unificate_img(img_x, img_y, img_result, img_size, channels);

  printf("FINISHED\n");
  asm volatile ("ecall");
  return 0;
}
