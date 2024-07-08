#ifndef TB_EDGE_DETECTOR_TLM_CPP
#define TB_EDGE_DETECTOR_TLM_CPP

// #define int64  systemc_int64
// #define uint64 systemc_uint64
#include <systemc.h>
// #undef int64
// #undef uint64
#define int64  opencv_int64
#define uint64 opencv_uint64
#include <opencv2/opencv.hpp>
#undef int64
#undef uint64

#include <cmath>

using namespace cv;
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"
#include "tlm_utils/peq_with_cb_and_phase.h"

#include "sobel_edge_detector_tlm.hpp"
#include "img_initiator.cpp"

SC_MODULE(Tb_top)
{
  img_initiator *initiator;
  sobel_edge_detector_tlm *edge_detector_DUT;

  SC_CTOR(Tb_top)
  {
    // Instantiate components   
    initiator = new img_initiator("initiator");   
    edge_detector_DUT = new sobel_edge_detector_tlm("edge_detector_DUT");
   
    // Bind initiator socket to target socket   
    initiator->socket.bind(edge_detector_DUT->socket);

    SC_THREAD(thread_process);     
  }

  void thread_process()
  {
    Mat greyImage;
    int localResult;

    #ifndef EDGE_DETECTOR_AT_EN
      int localWindow[3][3];
      int localGradientX, localGradientY;
    #else
      sc_uint<8> localWindow[3][3];
      sc_int<16> localGradientX, localGradientY;
    #endif // EDGE_DETECTOR_AT_EN
    
    #ifdef TEST_NORMALIZE_MAGNITUDE
      int** tmpValues;
      int maxTmpValue = 0;
    #endif // TEST_NORMALIZE_MAGNITUDE

    // #ifdef EDGE_DETECTOR_AT_EN
    //   sc_signal<sc_uint<64>> data;
    //   sc_signal<sc_uint<24>> address;
    // #endif // EDGE_DETECTOR_AT_EN

    #if defined(EDGE_DETECTOR_LT_EN) || defined(EDGE_DETECTOR_AT_EN)
      int total_number_of_pixels;
      int current_number_of_pixels = 0;
      int next_target_of_completion = 10.0;
    #endif // EDGE_DETECTOR_LT_EN || EDGE_DETECTOR_AT_EN

    greyImage = imread("../../tools/datagen/src/imgs/car_grayscale_image.jpg", IMREAD_GRAYSCALE);
  
    if (greyImage.empty())
    { 
      cout << "Image File " << "Not Found" << endl; 
      return;
    }

    Mat detectedImageX(greyImage.rows, greyImage.cols, CV_8UC1);
    Mat detectedImageY(greyImage.rows, greyImage.cols, CV_8UC1);
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
    
    for (int i = 0; i < greyImage.rows; i++) {
      for (int j = 0; j < greyImage.cols; j++) {
        for (int k = 0; k < 3; k++) {
          for (int l = 0; l < 3; l++) {
            if ((i == 0) && (j == 0)) { // Upper left corner
              if ((k == 0) || (l == 0)) {
                localWindow[k][l] = 0;
              }
              else {
                localWindow[k][l] = greyImage.at<uchar>(i + k - 1, j + l - 1);
              }
            }
            else if ((i == 0) && (j == greyImage.cols - 1)) { // Upper right corner
              if ((k == 0) || (l == 2)) {
                localWindow[k][l] = 0;
              }
              else {
                localWindow[k][l] = greyImage.at<uchar>(i + k - 1, j + l - 1);
              }
            }
            else if (i == 0) { // Upper border
              if (k == 0) {
                localWindow[k][l] = 0;
              }
              else {
                localWindow[k][l] = greyImage.at<uchar>(i + k - 1, j + l - 1);
              }
            }
            else if ((i == greyImage.rows - 1) && (j == 0)) { // Lower left corner
              if ((k == 2) || (l == 0)) {
                localWindow[k][l] = 0;
              }
              else {
                localWindow[k][l] = greyImage.at<uchar>(i + k - 1, j + l - 1);
              }
            }
            else if ((i == greyImage.rows - 1) && (j == greyImage.cols - 1)) { // Lower right corner
              if ((k == 2) || (l == 2)) {
                localWindow[k][l] = 0;
              }
              else {
                localWindow[k][l] = greyImage.at<uchar>(i + k - 1, j + l - 1);
              } 
            }
            else if (i == greyImage.rows - 1) { // Lower border
              if (k == 2) {
                localWindow[k][l] = 0;
              }
              else {
                localWindow[k][l] = greyImage.at<uchar>(i + k - 1, j + l - 1);
              }
            }
            else if (j == 0) { // Left border
              if (l == 0) {
                localWindow[k][l] = 0;
              }
              else {
                localWindow[k][l] = greyImage.at<uchar>(i + k - 1, j + l - 1);
              }
            }
            else if (j == greyImage.cols - 1) { // Right border
              if (l == 2) {
                localWindow[k][l] = 0;
              }
              else {
                localWindow[k][l] = greyImage.at<uchar>(i + k - 1, j + l - 1);
              }
            }
            else {
              localWindow[k][l] = greyImage.at<uchar>(i + k - 1, j + l - 1);
            }
          }
        }

#ifndef EDGE_DETECTOR_AT_EN
        //edge_detector.set_local_window(localWindow);
        int* local_window_ptr = new int[9];
        for (int i = 0, row = 0, col = 0; i < 9; i++){
          *(local_window_ptr+i) = localWindow[row++][col];
          if (row >= 3){
            row = 0;
            col++;
          }
        }
        initiator->write(local_window_ptr, 0, 9*4);
        printf("OUTSIDE");
        wait(sc_time(100, SC_NS));
        printf("OUTSIDE");
        int* data_returned = new int;
        initiator->read(data_returned, 0, 2*4);
        // printf("OUTSIDE");
        printf("Data_returned: %0d\n", *data_returned);
        printf("Data_returned: %0d\n", *(data_returned+1));
        
        //edge_detector_DUT->set_local_window(localWindow);
        localGradientX = edge_detector_DUT->obtain_sobel_gradient_x();
        localGradientY = edge_detector_DUT->obtain_sobel_gradient_y();

        printf("Data_returned2: %0d\n", localGradientX);
        printf("Data_returned2: %0d\n", localGradientY);

        
        //initiator->write(&localWindow[0][0]+8, 1);
        //wait(delay);
        //wait(delay);
#else
        address = SOBEL_INPUT_0;
        initiator->write(localWindow, 8);
        //wait(delay);
        
        address = SOBEL_INPUT_1;
        initiator->write(localWindow+8, 1);
        //wait(delay);
        
        address = SOBEL_OUTPUT;
        initiator->read(1);
        //wait(delay);
        
        for (int m = 0; m < 16; m++)
        {
          localGradientX[m] = data.read()[m];
        }
        for (int m = 0; m < 16; m++)
        {
          localGradientY[m] = data.read()[m + 16];
        }
        
        localResult = (int)sqrt((float)(pow((int)localGradientX, 2)) + (float)(pow((int)localGradientY, 2)));
#endif // EDGE_DETECTOR_AT_EN
        cout << "HERE01" << endl;
        localGradientX = *data_returned;
        localGradientY = *(data_returned+1);
        cout << "HERE01" << endl;
      
        localResult = (int)sqrt((float)(pow(localGradientX, 2)) + (float)(pow(localGradientY, 2)));
#ifdef TEST_NORMALIZE_MAGNITUDE
        tmpValues[i][j] = localResult;
        if (localResult > maxTmpValue) {
          maxTmpValue = localResult;
        }
#else
        if (localResult > 255) {
          detectedImage.at<uchar>(i, j) = 255;
        }
        else {
          detectedImage.at<uchar>(i, j) = localResult;
        }
#endif // TEST_NORMALIZE_MAGNITUDE
        
        if (localGradientX > 255) {
          detectedImageX.at<uchar>(i, j) = 255;
        } 
        else if (localGradientX < 0) {
          detectedImageX.at<uchar>(i, j) = (-localGradientX);
        }
        else {
          detectedImageX.at<uchar>(i, j) = localGradientX; 
        }
        if (localGradientY > 255) {
          detectedImageY.at<uchar>(i, j) = 255;
        }
        else if (localGradientY < 0) {
          detectedImageY.at<uchar>(i, j) = (-localGradientY); 
        }
        else {
          detectedImageY.at<uchar>(i, j) = localGradientY;
        }

#if defined(EDGE_DETECTOR_LT_EN) || defined(EDGE_DETECTOR_AT_EN)
        current_number_of_pixels++;
        if (((((float)(current_number_of_pixels)) / ((float)(total_number_of_pixels))) * 100.0) >= next_target_of_completion) {
          std::cout << "@" << sc_time_stamp() << " Image processing completed at " << next_target_of_completion << std::endl;
          next_target_of_completion += 10.0;
        }
#endif // EDGE_DETECTOR_LT_EN || EDGE_DETECTOR_AT_EN
      }
    }

#ifdef TEST_NORMALIZE_MAGNITUDE
    for (int i = 0; i < detectedImage.rows; i++) {
      for (int j = 0; j < detectedImage.cols; j++) {
        detectedImage.at<uchar>(i, j) = (char)((int)(255.0 * (((float)(tmpValues[i][j])) / ((float)(maxTmpValue)))));
      }
    }
#endif // TEST_NORMALIZE_MAGNITUDE

    //imshow("Original Image", greyImage);
    //waitKey(0);
  
    //imshow("Gradient X", detectedImageX);
    imwrite("detectedImageX.jpg", detectedImageX);
    //waitKey(0);
  
    //imshow("Gradient Y", detectedImageY);
    imwrite("detectedImageY.jpg", detectedImageY);
    //waitKey(0);
  
    //imshow("Gradient Total", detectedImage);
    imwrite("detectedImage.jpg", detectedImage);
    //waitKey(0);
  }
};



int sc_main(int, char*[])
{
  // Pass command linke arguments
  sc_argc();
  sc_argv();
  
  // Open VCD file
  sc_trace_file* wf = sc_create_vcd_trace_file("edge_detector");
  wf->set_time_unit(1, SC_PS);

  Tb_top top("top");


// #ifdef EDGE_DETECTOR_AT_EN
//   edge_detector.data(data);
//   edge_detector.address(address);
  
//   // Dump the desired signals
//   sc_trace(wf, data, "data");
//   sc_trace(wf, address, "address");
//   sc_trace(wf, edge_detector.localWindow[0][0], "localWindow(0)(0)");
//   sc_trace(wf, edge_detector.localWindow[0][1], "localWindow(0)(1)");
//   sc_trace(wf, edge_detector.localWindow[0][2], "localWindow(0)(2)");
//   sc_trace(wf, edge_detector.localWindow[1][0], "localWindow(1)(0)");
//   sc_trace(wf, edge_detector.localWindow[1][1], "localWindow(1)(1)");
//   sc_trace(wf, edge_detector.localWindow[1][2], "localWindow(1)(2)");
//   sc_trace(wf, edge_detector.localWindow[2][0], "localWindow(0)(0)");
//   sc_trace(wf, edge_detector.localWindow[2][1], "localWindow(2)(1)");
//   sc_trace(wf, edge_detector.localWindow[2][2], "localWindow(2)(2)");
//   sc_trace(wf, edge_detector.localMultX[0][0], "localMultX(0)(0)");
//   sc_trace(wf, edge_detector.localMultX[0][1], "localMultX(0)(1)");
//   sc_trace(wf, edge_detector.localMultX[0][2], "localMultX(0)(2)");
//   sc_trace(wf, edge_detector.localMultX[1][0], "localMultX(1)(0)");
//   sc_trace(wf, edge_detector.localMultX[1][1], "localMultX(1)(1)");
//   sc_trace(wf, edge_detector.localMultX[1][2], "localMultX(1)(2)");
//   sc_trace(wf, edge_detector.localMultX[2][0], "localMultX(0)(0)");
//   sc_trace(wf, edge_detector.localMultX[2][1], "localMultX(2)(1)");
//   sc_trace(wf, edge_detector.localMultX[2][2], "localMultX(2)(2)");
//   sc_trace(wf, edge_detector.localMultY[0][0], "localMultY(0)(0)");
//   sc_trace(wf, edge_detector.localMultY[0][1], "localMultY(0)(1)");
//   sc_trace(wf, edge_detector.localMultY[0][2], "localMultY(0)(2)");
//   sc_trace(wf, edge_detector.localMultY[1][0], "localMultY(1)(0)");
//   sc_trace(wf, edge_detector.localMultY[1][1], "localMultY(1)(1)");
//   sc_trace(wf, edge_detector.localMultY[1][2], "localMultY(1)(2)");
//   sc_trace(wf, edge_detector.localMultY[2][0], "localMultY(0)(0)");
//   sc_trace(wf, edge_detector.localMultY[2][1], "localMultY(2)(1)");
//   sc_trace(wf, edge_detector.localMultY[2][2], "localMultY(2)(2)");
//   sc_trace(wf, edge_detector.resultSobelGradientX, "resultSobelGradientX");
//   sc_trace(wf, edge_detector.resultSobelGradientY, "resultSobelGradientY");
// #endif // EDGE_DETECTOR_AT_EN
  
  sc_start();

  std::cout << "@" << sc_time_stamp() << " Terminating simulation" << std::endl;
  sc_close_vcd_trace_file(wf);

  return 0;
}
#endif
