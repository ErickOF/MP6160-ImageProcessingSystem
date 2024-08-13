#ifndef IMG_UNIFICATION_CPP
#define IMG_UNIFICATION_CPP

#include <../inc/address_map.hpp>
#include <math.h>

//Test Unificate in FreeRTOS
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

void unificate_img(unsigned char *x_img, unsigned char *y_img, unsigned char *unificated_img, int img_size)
{
  //Iterate over image
  for(unsigned char *x = x_img, *y = y_img, *u = unificated_img; x < x_img + img_size && y < y_img + img_size && u < unificated_img + img_size; x++, y++, u++){
    int pixel_magnitude;
    int pixel_x = (int) *x;
    int pixel_y = (int) *y;
      
    pixel_magnitude = norm(pixel_x, pixel_y);

    if (pixel_magnitude > 255) {pixel_magnitude = 255;};
    *u = (unsigned char) pixel_magnitude;
  }
}

#endif // IMG_UNIFICATION_CPP