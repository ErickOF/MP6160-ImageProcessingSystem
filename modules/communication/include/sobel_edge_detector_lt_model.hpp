#ifdef EDGE_DETECTOR_LT_EN
#ifndef SOBEL_EDGE_DETECTOR_HPP
#define SOBEL_EDGE_DETECTOR_HPP

#include <systemc.h>

SC_MODULE(Edge_Detector)
{
  
  int localWindow[3][3];
  
  const int sobelGradientX[3][3] = {{-1,  0,  1},
                                    {-2,  0,  2},
                                    {-1,  0,  1}};
  const int sobelGradientY[3][3] = {{-1, -2, -1},
                                    { 0,  0,  0},
                                    { 1,  2,  1}};
  
  int resultSobelGradientX;
  int resultSobelGradientY;
  
  sc_event gotLocalWindow, finishedSobelGradientX, finishedSobelGradientY;
  
  SC_CTOR(Edge_Detector)
  {
    SC_THREAD(compute_sobel_gradient_x);
    SC_THREAD(compute_sobel_gradient_y);
  }
  
  void set_local_window(int window[3][3]);
  
  void compute_sobel_gradient_x();
  
  void compute_sobel_gradient_y();
  
  int obtain_sobel_gradient_x();
  
  int obtain_sobel_gradient_y();
  
};

#endif // SOBEL_EDGE_DETECTOR_HPP
#endif // EDGE_DETECTOR_LT_EN
