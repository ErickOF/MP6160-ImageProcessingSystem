#ifdef EDGE_DETECTOR_AT_EN
#ifndef SOBEL_EDGE_DETECTOR_CPP
#define SOBEL_EDGE_DETECTOR_CPP

#include "sobel_edge_detector_at_model.hpp"

void Edge_Detector::write()
{
  wr_t.notify(5, SC_NS);
}

void Edge_Detector::wr()
{
  while (true)
    {
    wait(wr_t);
#ifdef USING_TLM_TB_EN
    if ((address - SOBEL_INPUT_0) == 0)
#else
    if ((address.read() - SOBEL_INPUT_0) == 0)
#endif // USING_TLM_TB_EN
    {
      int j = 0;
#ifdef USING_TLM_TB_EN
      localWindow[0][0] = data.range((j + 1) * 8 - 1, j * 8);
      j++;
      localWindow[0][1] = data.range((j + 1) * 8 - 1, j * 8);
      j++;
      localWindow[0][2] = data.range((j + 1) * 8 - 1, j * 8);
      j++;
      localWindow[1][0] = data.range((j + 1) * 8 - 1, j * 8);
      j++;
      localWindow[1][1] = data.range((j + 1) * 8 - 1, j * 8);
      j++;
      localWindow[1][2] = data.range((j + 1) * 8 - 1, j * 8);
      j++;
      localWindow[2][0] = data.range((j + 1) * 8 - 1, j * 8);
      j++;
      localWindow[2][1] = data.range((j + 1) * 8 - 1, j * 8);
#else
      for (int i = 0; i < 8; i++)
      {
        localWindow[0][0][i] = data.read()[i + (j * 8)];
      }
      j++;
      for (int i = 0; i < 8; i++)
      {
        localWindow[0][1][i] = data.read()[i + (j * 8)];
      }
      j++;
      for (int i = 0; i < 8; i++)
      {
        localWindow[0][2][i] = data.read()[i + (j * 8)];
      }
      j++;
      for (int i = 0; i < 8; i++)
      {
        localWindow[1][0][i] = data.read()[i + (j * 8)];
      }
      j++;
      for (int i = 0; i < 8; i++)
      {
        localWindow[1][1][i] = data.read()[i + (j * 8)];
      }
      j++;
      for (int i = 0; i < 8; i++)
      {
        localWindow[1][2][i] = data.read()[i + (j * 8)];
      }
      j++;
      for (int i = 0; i < 8; i++)
      {
        localWindow[2][0][i] = data.read()[i + (j * 8)];
      }
      j++;
      for (int i = 0; i < 8; i++)
      {
        localWindow[2][1][i] = data.read()[i + (j * 8)];
      }
#endif // USING_TLM_TB_EN
      gotLocalWindow.notify(0, SC_NS);
    }
#ifdef USING_TLM_TB_EN
    else if ((address - SOBEL_INPUT_1) == 0)
#else
    else if ((address.read() - SOBEL_INPUT_1) == 0)
#endif // USING_TLM_TB_EN
    {
#ifdef USING_TLM_TB_EN
      localWindow[2][2] = data.range(15, 0);
#else
      for (int i = 0; i < 8; i++)
      {
        localWindow[2][2][i] = data.read()[i];
      }
#endif // USING_TLM_TB_EN
      gotLocalWindow.notify(0, SC_NS);
    }
  }
}

void Edge_Detector::read()
{
  rd_t.notify(5, SC_NS);
}

void Edge_Detector::rd()
{
  while (true)
  {
    wait(rd_t);
#ifdef USING_TLM_TB_EN
    if ((address - SOBEL_OUTPUT) == 0)
#else
    if ((address.read() - SOBEL_OUTPUT) == 0)
#endif // USING_TLM_TB_EN
    {
      data = (sc_uint<32>(0), resultSobelGradientY, resultSobelGradientX);
    }
  }
}

void Edge_Detector::compute_sobel_gradient_x()
{
  while (true)
  {
    wait(gotLocalWindow);
    
    mult_x.notify(delay_multiplier, SC_NS);
  }
}

void Edge_Detector::compute_sobel_gradient_y()
{
  while (true)
  {
    wait(gotLocalWindow);
    
    mult_y.notify(delay_multiplier, SC_NS);
  }
}

void Edge_Detector::perform_mult_gradient_x()
{
  while (true)
  {
    wait(mult_x);
    
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        localMultX[i][j] = localWindow[i][j] * sobelGradientX[i][j];
      }
    }
    
    sum_x.notify(delay_full_adder, SC_NS);
  }
}

void Edge_Detector::perform_mult_gradient_y()
{
  while (true)
  {
    wait(mult_y);
    
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        localMultY[i][j] = localWindow[i][j] * sobelGradientY[i][j];
      }
    }
    
    sum_y.notify(delay_full_adder, SC_NS);
  }
}

void Edge_Detector::perform_sum_gradient_x()
{
  while (true)
  {
    wait(sum_x);
    resultSobelGradientX = 0;
    
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        resultSobelGradientX += localMultX[i][j];
      }
    }
  }
}

void Edge_Detector::perform_sum_gradient_y()
{
  while (true)
  {
    wait(sum_y);
    resultSobelGradientY = 0;
    
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
        resultSobelGradientY += localMultY[i][j];
      }
    }
  }
}

#endif // SOBEL_EDGE_DETECTOR_CPP
#endif // EDGE_DETECTOR_AT_EN
