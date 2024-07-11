#ifdef RGB2GRAY_PV_EN
#ifndef RGB2GRAY_CPP
#define RGB2GRAY_CPP

#include "rgb2gray_pv_model.hpp"

void Rgb2Gray::set_rgb_pixel(unsigned char r_val, unsigned char g_val, unsigned char b_val)
{
  this->r = r_val;
  this->g = g_val;
  this->b = b_val;
}

void Rgb2Gray::compute_gray_value()
{
  this->gray_value = 0.299 * this->r + 0.587 * this->g + 0.114 * this->b;
}

unsigned char Rgb2Gray::obtain_gray_value()
{
  compute_gray_value();

  return this->gray_value;
}

#endif // RGB2GRAY_CPP
#endif // RGB2GRAY_PV_EN
