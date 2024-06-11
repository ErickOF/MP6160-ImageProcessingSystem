//-----------------------------------------------------
//Module: Unification (PV)
//By: Roger Morales Monge
//Description: Programmer's View Model of unification
//process for two images (magnitude)
//-----------------------------------------------------
#ifdef IMG_UNIFICATE_PV_EN
#ifndef IMG_UNIFICATE_CPP
#define IMG_UNIFICATE_CPP

#include "unification_pv_model.hpp"

//-----------NORM SELECTION MACROS-----------
//#define USE_L1_NORM
#define USE_L2_NORM //default
//#define USE_INF_NORM
//-------------------------------------------

void img_unification_module::unificate_pixel(unsigned char x_pixel, unsigned char y_pixel, unsigned char * unificated_pixel) {
  //Get the Norm
  *unificated_pixel = (unsigned char) this->norm(x_pixel, y_pixel);
}

void img_unification_module::unificate_img(unsigned char *x_img, unsigned char *y_img, unsigned char *unificated_img, int img_size, int channels){
  //Iterate over image
  for(unsigned char *x = x_img, *y = y_img, *u = unificated_img; x < x_img + img_size && y < y_img + img_size && u < unificated_img + img_size; x+=channels, y+=channels, u+=channels){
    unsigned char pixel_magnitude;
    unsigned char pixel_x = int(*x);
    unsigned char pixel_y = int(*y);
      
    this->unificate_pixel(pixel_x, pixel_y, &pixel_magnitude);
    *u = pixel_magnitude;
  }
}
  
int img_unification_module::norm(int a, int b) {
  int norm_result = 0;
    
  //L1 Norm
  #ifdef USE_L1_NORM
  norm_result = abs(a) + abs(b);
  #endif

  //L2 Norm
  #ifdef USE_L2_NORM
  norm_result = sqrt(pow(a, 2) + pow(b, 2));
  #endif
  
  //INFINITY Norm
  #ifdef USE_INF_NORM
  norm_result = (a > b ? a : b);
  #endif
  
  return norm_result;
}

#endif //IMG_UNIFICATE_CPP
#endif //IMG_UNIFICATE_PV_EN
