#define int64  systemc_int64
#define uint64 systemc_uint64
#include <systemc.h>
#undef int64
#undef uint64
#define int64  opencv_int64
#define uint64 opencv_uint64
#include <opencv2/opencv.hpp>
#undef int64
#undef uint64

#include <cmath>

#ifdef EDGE_DETECTOR_PV_EN
#include "sobel_edge_detector_pv_model.hpp"
#elif defined(EDGE_DETECTOR_LT_EN)
#include "sobel_edge_detector_lt_model.hpp"
#elif defined(EDGE_DETECTOR_AT_EN)
#include "sobel_edge_detector_at_model.hpp"
#else
#error "No EDGE_DETECTOR macro is defined. Please define one of EDGE_DETECTOR_AT_EN, EDGE_DETECTOR_LT_EN, or EDGE_DETECTOR_PV_EN."
#endif // EDGE_DETECTOR_XX_EN

using namespace cv; 

int sc_main(int, char*[])
{
  Mat greyImage, colorImage;
  
#ifndef EDGE_DETECTOR_AT_EN
  int localWindow[3][3];
  int localGradientX, localGradientY;
#else
  sc_uint<8> localWindow[3][3];
  sc_int<16> localGradientX, localGradientY;
#endif // EDGE_DETECTOR_AT_EN
  int localResult;
#ifdef TEST_NORMALIZE_MAGNITUDE
  int** tmpValues;
  int maxTmpValue = 0;
#endif // TEST_NORMALIZE_MAGNITUDE
#ifdef EDGE_DETECTOR_AT_EN
  sc_signal<sc_uint<64>> data;
  sc_signal<sc_uint<24>> address;
#endif // EDGE_DETECTOR_AT_EN
#if defined(EDGE_DETECTOR_LT_EN) || defined(EDGE_DETECTOR_AT_EN)
  int total_number_of_pixels;
  int current_number_of_pixels = 0;
  int next_target_of_completion = 10.0;
#endif // EDGE_DETECTOR_LT_EN || EDGE_DETECTOR_AT_EN

  // Pass command linke arguments
  sc_argc();
  sc_argv();
  
  // Open VCD file
  sc_trace_file* wf = sc_create_vcd_trace_file("edge_detector");
  wf->set_time_unit(1, SC_PS);

  Edge_Detector edge_detector("edge_detector");
#ifdef EDGE_DETECTOR_AT_EN
  edge_detector.data(data);
  edge_detector.address(address);
  
  // Dump the desired signals
  sc_trace(wf, data, "data");
  sc_trace(wf, address, "address");
  sc_trace(wf, edge_detector.localWindow[0][0], "localWindow(0)(0)");
  sc_trace(wf, edge_detector.localWindow[0][1], "localWindow(0)(1)");
  sc_trace(wf, edge_detector.localWindow[0][2], "localWindow(0)(2)");
  sc_trace(wf, edge_detector.localWindow[1][0], "localWindow(1)(0)");
  sc_trace(wf, edge_detector.localWindow[1][1], "localWindow(1)(1)");
  sc_trace(wf, edge_detector.localWindow[1][2], "localWindow(1)(2)");
  sc_trace(wf, edge_detector.localWindow[2][0], "localWindow(0)(0)");
  sc_trace(wf, edge_detector.localWindow[2][1], "localWindow(2)(1)");
  sc_trace(wf, edge_detector.localWindow[2][2], "localWindow(2)(2)");
  sc_trace(wf, edge_detector.localMultX[0][0], "localMultX(0)(0)");
  sc_trace(wf, edge_detector.localMultX[0][1], "localMultX(0)(1)");
  sc_trace(wf, edge_detector.localMultX[0][2], "localMultX(0)(2)");
  sc_trace(wf, edge_detector.localMultX[1][0], "localMultX(1)(0)");
  sc_trace(wf, edge_detector.localMultX[1][1], "localMultX(1)(1)");
  sc_trace(wf, edge_detector.localMultX[1][2], "localMultX(1)(2)");
  sc_trace(wf, edge_detector.localMultX[2][0], "localMultX(0)(0)");
  sc_trace(wf, edge_detector.localMultX[2][1], "localMultX(2)(1)");
  sc_trace(wf, edge_detector.localMultX[2][2], "localMultX(2)(2)");
  sc_trace(wf, edge_detector.localMultY[0][0], "localMultY(0)(0)");
  sc_trace(wf, edge_detector.localMultY[0][1], "localMultY(0)(1)");
  sc_trace(wf, edge_detector.localMultY[0][2], "localMultY(0)(2)");
  sc_trace(wf, edge_detector.localMultY[1][0], "localMultY(1)(0)");
  sc_trace(wf, edge_detector.localMultY[1][1], "localMultY(1)(1)");
  sc_trace(wf, edge_detector.localMultY[1][2], "localMultY(1)(2)");
  sc_trace(wf, edge_detector.localMultY[2][0], "localMultY(0)(0)");
  sc_trace(wf, edge_detector.localMultY[2][1], "localMultY(2)(1)");
  sc_trace(wf, edge_detector.localMultY[2][2], "localMultY(2)(2)");
  sc_trace(wf, edge_detector.resultSobelGradientX, "resultSobelGradientX");
  sc_trace(wf, edge_detector.resultSobelGradientY, "resultSobelGradientY");
#endif // EDGE_DETECTOR_AT_EN
  
  colorImage = imread("../../tools/datagen/src/imgs/car.jpg", IMREAD_UNCHANGED);
  
  if (colorImage.empty())
  { 
    cout << "Image File " << "Not Found" << endl; 

    // wait for any key press 
    return -1; 
  }
  
  cvtColor(colorImage, greyImage, cv::COLOR_BGR2GRAY);
  
  Mat detectedImage(greyImage.rows, greyImage.cols, CV_8UC1);
  
#if defined(EDGE_DETECTOR_LT_EN) || defined(EDGE_DETECTOR_AT_EN)
  total_number_of_pixels = greyImage.rows * greyImage.cols;
#endif // EDGE_DETECTOR_LT_EN || EDGE_DETECTOR_AT_EN
  
#ifdef TEST_NORMALIZE_MAGNITUDE
  tmpValues = new int*[greyImage.rows];
  for (int i = 0; i < greyImage.rows; i++)
  {
    tmpValues[i] = new int[greyImage.cols];
  }
#endif // TEST_NORMALIZE_MAGNITUDE
  
  sc_start();
  
  for (int i = 0; i < greyImage.rows; i++)
  {
    for (int j = 0; j < greyImage.cols; j++)
    {
      for (int k = 0; k < 3; k++)
      {
        for (int l = 0; l < 3; l++)
        {
          if ((i == 0) && (j == 0)) // Upper left corner
          {
            if ((k == 0) || (l == 0))
            {
              localWindow[k][l] = 0;
            }
            else
            {
              localWindow[k][l] = greyImage.at<uchar>(i + k - 1, j + l - 1);
            }
          }
          else if ((i == 0) && (j == greyImage.cols - 1)) // Upper right corner
          {
            if ((k == 0) || (l == 2))
            {
              localWindow[k][l] = 0;
            }
            else
            {
              localWindow[k][l] = greyImage.at<uchar>(i + k - 1, j + l - 1);
            }
          }
          else if (i == 0) // Upper border
          {
            if (k == 0)
            {
              localWindow[k][l] = 0;
            }
            else
            {
              localWindow[k][l] = greyImage.at<uchar>(i + k - 1, j + l - 1);
            }
          }
          else if ((i == greyImage.rows - 1) && (j == 0)) // Lower left corner
          {
            if ((k == 2) || (l == 0))
            {
              localWindow[k][l] = 0;
            }
            else
            {
              localWindow[k][l] = greyImage.at<uchar>(i + k - 1, j + l - 1);
            }
          }
          else if ((i == greyImage.rows - 1) && (j == greyImage.cols - 1)) // Lower right corner
          {
            if ((k == 2) || (l == 2))
            {
              localWindow[k][l] = 0;
            }
            else
            {
              localWindow[k][l] = greyImage.at<uchar>(i + k - 1, j + l - 1);
            }
          }
          else if (i == greyImage.rows - 1) // Lower border
          {
            if (k == 2)
            {
              localWindow[k][l] = 0;
            }
            else
            {
              localWindow[k][l] = greyImage.at<uchar>(i + k - 1, j + l - 1);
            }
          }
          else if (j == 0) // Left border
          {
            if (l == 0)
            {
              localWindow[k][l] = 0;
            }
            else
            {
              localWindow[k][l] = greyImage.at<uchar>(i + k - 1, j + l - 1);
            }
          }
          else if (j == greyImage.cols - 1) // Right border
          {
            if (l == 2)
            {
              localWindow[k][l] = 0;
            }
            else
            {
              localWindow[k][l] = greyImage.at<uchar>(i + k - 1, j + l - 1);
            }
          }
          else
          {
            localWindow[k][l] = greyImage.at<uchar>(i + k - 1, j + l - 1);
          }
        }
      }
      
#ifndef EDGE_DETECTOR_AT_EN
      edge_detector.set_local_window(localWindow);
#else
      data = (localWindow[2][1], localWindow[2][0], localWindow[1][2], localWindow[1][1], localWindow[1][0], localWindow[0][2], localWindow[0][1], localWindow[0][0]);
      address = SOBEL_INPUT_0;
      edge_detector.write();
      sc_start(10, SC_NS);
      
      data = (sc_uint<56>(0), localWindow[2][2]);
      address = SOBEL_INPUT_1;
      edge_detector.write();
      sc_start(10, SC_NS);
      
      sc_start(20, SC_NS);
      
      address = SOBEL_OUTPUT;
      edge_detector.read();
      sc_start(10, SC_NS);
      
      for (int m = 0; m < 16; m++)
      {
        localGradientX[m] = data.read()[m];
      }
      for (int m = 0; m < 16; m++)
      {
        localGradientY[m] = data.read()[m + 16];
      }
      
      localResult = localResult = (int)sqrt((float)(pow((int)localGradientX, 2)) + (float)(pow((int)localGradientY, 2)));
#endif // EDGE_DETECTOR_AT_EN
#ifndef EDGE_DETECTOR_AT_EN
#ifdef EDGE_DETECTOR_LT_EN
      sc_start(30, SC_NS);
#endif // EDGE_DETECTOR_LT_EN
      localGradientX = edge_detector.obtain_sobel_gradient_x();
      localGradientY = edge_detector.obtain_sobel_gradient_y();
      
      localResult = (int)sqrt((float)(pow(localGradientX, 2)) + (float)(pow(localGradientY, 2)));
#endif // EDGE_DETECTOR_AT_EN
#ifdef TEST_NORMALIZE_MAGNITUDE
      tmpValues[i][j] = localResult;
      if (localResult > maxTmpValue)
      {
        maxTmpValue = localResult;
      }
#else
      if (localResult > 255)
      {
        detectedImage.at<uchar>(i, j) = 255;
      }
      else
      {
        detectedImage.at<uchar>(i, j) = localResult;
      }
#endif // TEST_NORMALIZE_MAGNITUDE
#if defined(EDGE_DETECTOR_LT_EN) || defined(EDGE_DETECTOR_AT_EN)
      current_number_of_pixels++;
      if (((((float)(current_number_of_pixels)) / ((float)(total_number_of_pixels))) * 100.0) >= next_target_of_completion)
      {
        std::cout << "@" << sc_time_stamp() << " Image processing completed at " << next_target_of_completion << std::endl;
        next_target_of_completion += 10.0;
      }
#endif // EDGE_DETECTOR_LT_EN || EDGE_DETECTOR_AT_EN
    }
  }
  
#ifdef TEST_NORMALIZE_MAGNITUDE
  for (int i = 0; i < detectedImage.rows; i++)
  {
    for (int j = 0; j < detectedImage.cols; j++)
    {
      detectedImage.at<uchar>(i, j) = (char)((int)(255.0 * (((float)(tmpValues[i][j])) / ((float)(maxTmpValue)))));
    }
  }
#endif // TEST_NORMALIZE_MAGNITUDE
  
  imshow("Window Name", colorImage);
  
  waitKey(0);
  
  imshow("Window Name", greyImage);
  
  waitKey(0);
  
  imshow("Window Name", detectedImage);
  
  waitKey(0);

  std::cout << "@" << sc_time_stamp() << " Terminating simulation" << std::endl;
  sc_close_vcd_trace_file(wf);

  return 0;
}
