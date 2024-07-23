//-----------------------------------------------------
//Module: Image Unification (PV) Header
//By: Roger Morales Monge
//Description: Programmer's View Model of unification
//process for two images (magnitude)
//-----------------------------------------------------

#ifdef IMG_UNIFICATE_PV_EN
#ifndef IMG_UNIFICATE_HPP
#define IMG_UNIFICATE_HPP

#include <systemc.h>
#include <math.h>

SC_MODULE (img_unification_module) {

  //-----------Internal Variables-----------
  unsigned char x_pixel, y_pixel;
  unsigned char *unificated_pixel;
  
  //-----------Constructor-----------
  SC_CTOR(img_unification_module) {
  } // End of Constructor

  //------------Methods------------
  void unificate_pixel(int x_pixel, int y_pixel, unsigned char * unificated_pixel);
  void unificate_img(unsigned char *x_img, unsigned char *y_img, unsigned char *unificated_img, int img_size, int channels);
  int norm(int a, int b);
};

#endif //IMG_UNIFICATE_HPP
#endif //IMG_UNIFICATE_PV_EN

