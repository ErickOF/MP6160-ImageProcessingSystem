#ifdef EDGE_DETECTOR_LT_EN
#ifndef SOBEL_EDGE_DETECTOR_CPP
#define SOBEL_EDGE_DETECTOR_CPP

#include "sobel_edge_detector_lt_model.hpp"

void Edge_Detector::set_local_window(int window[3][3])
{
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      this->localWindow[i][j] = window[i][j];
    }
  }
  gotLocalWindow.notify(10, SC_NS);
}

void Edge_Detector::compute_sobel_gradient_x()
{
  while (true)
  {
    wait(gotLocalWindow);
    this->resultSobelGradientX = 0;
    
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        this->resultSobelGradientX += this->localWindow[i][j] * this->sobelGradientX[i][j];
      }
    }
    finishedSobelGradientX.notify(5, SC_NS);
  }
}

void Edge_Detector::compute_sobel_gradient_y()
{
  while (true)
  {
    wait(gotLocalWindow);
    this->resultSobelGradientY = 0;
    
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        this->resultSobelGradientY += this->localWindow[i][j] * this->sobelGradientY[i][j];
      }
    }
    finishedSobelGradientY.notify(5, SC_NS);
  }
}

int Edge_Detector::obtain_sobel_gradient_x()
{
  return this->resultSobelGradientX;
}

int Edge_Detector::obtain_sobel_gradient_y()
{
  return this->resultSobelGradientY;
}

#endif // SOBEL_EDGE_DETECTOR_CPP
#endif // EDGE_DETECTOR_LT_EN
