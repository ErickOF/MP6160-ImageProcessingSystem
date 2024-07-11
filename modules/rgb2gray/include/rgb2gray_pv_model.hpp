#ifdef RGB2GRAY_PV_EN
#ifndef RGB2GRAY_HPP
#define RGB2GRAY_HPP

#include <systemc.h>

SC_MODULE(Rgb2Gray)
{

  unsigned char r;
  unsigned char g;
  unsigned char b;
  unsigned char gray_value;

  SC_CTOR(Rgb2Gray)
  {
  }
  
  void set_rgb_pixel(unsigned char r_val, unsigned char g_val, unsigned char b_val);
  
  void compute_gray_value();
  
  unsigned char obtain_gray_value();

};

#endif // RGB2GRAY_HPP
#endif // RGB2GRAY_PV_EN
