#ifdef EDGE_DETECTOR_AT_EN
#ifndef SOBEL_EDGE_DETECTOR_HPP
#define SOBEL_EDGE_DETECTOR_HPP

#include <systemc.h>
#include "AddressMap.h"

SC_MODULE(Edge_Detector)
{
  
  sc_inout<sc_uint<64>> data;
  sc_in<sc_uint<24>> address;
  
  const sc_int<16> sobelGradientX[3][3] = {{-1,  0,  1},
                                           {-2,  0,  2},
                                           {-1,  0,  1}};
  const sc_int<16> sobelGradientY[3][3] = {{-1, -2, -1},
                                           { 0,  0,  0},
                                           { 1,  2,  1}};
  
  sc_int<16> localWindow[3][3];
  
  sc_int<16> resultSobelGradientX;
  sc_int<16> resultSobelGradientY;
  
  sc_int<16> localMultX[3][3];
  sc_int<16> localMultY[3][3];
  
  sc_event gotLocalWindow;
  
  sc_event rd_t, wr_t;
  
  sc_event mult_x, mult_y, sum_x, sum_y;
  
  SC_CTOR(Edge_Detector)
  {
    SC_THREAD(wr);
    SC_THREAD(rd);
    SC_THREAD(compute_sobel_gradient_x);
    SC_THREAD(compute_sobel_gradient_y);
    SC_THREAD(perform_mult_gradient_x);
    SC_THREAD(perform_mult_gradient_y);
    SC_THREAD(perform_sum_gradient_x);
    SC_THREAD(perform_sum_gradient_y);
  }
  
  void write();
  
  void read();
  
  void wr();
  
  void rd();
  
  void compute_sobel_gradient_x();
  
  void compute_sobel_gradient_y();
  
  void perform_mult_gradient_x();
  
  void perform_mult_gradient_y();
  
  void perform_sum_gradient_x();
  
  void perform_sum_gradient_y();
  
};

#endif // SOBEL_EDGE_DETECTOR_HPP
#endif // EDGE_DETECTOR_AT_EN
