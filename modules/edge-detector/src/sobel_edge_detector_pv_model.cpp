#ifdef EDGE_DETECTOR_PV_EN
#ifndef SOBEL_EDGE_DETECTOR_CPP
#define SOBEL_EDGE_DETECTOR_CPP

#include "sobel_edge_detector_pv_model.hpp"

void Edge_Detector::set_local_window(int window[3][3])
{
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      this->localWindow[i][j] = window[i][j];
    }
  }
}

void Edge_Detector::compute_sobel_gradient_x()
{
  this->resultSobelGradientX = 0;
  
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      this->resultSobelGradientX += this->localWindow[i][j] * this->sobelGradientX[i][j];
    }
  }
}

void Edge_Detector::compute_sobel_gradient_y()
{
  this->resultSobelGradientY = 0;
  
  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      this->resultSobelGradientY += this->localWindow[i][j] * this->sobelGradientY[i][j];
    }
  }
}

int Edge_Detector::obtain_sobel_gradient_x()
{
  this->compute_sobel_gradient_x();

  return this->resultSobelGradientX;
}

int Edge_Detector::obtain_sobel_gradient_y()
{
    this->compute_sobel_gradient_y();

  return this->resultSobelGradientY;
}

#endif // SOBEL_EDGE_DETECTOR_CPP
#endif // EDGE_DETECTOR_PV_EN
