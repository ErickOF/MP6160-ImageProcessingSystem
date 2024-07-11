#ifdef USING_TLM_TB_EN
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

#include "rgb2gray_tlm.hpp"
#include "sobel_edge_detector_tlm.hpp"
#include "img_initiator.cpp"

#include "common_func.hpp"
#include "AddressMap.h"

#if !defined(RGB2GRAY_PV_EN) || !defined(EDGE_DETECTOR_AT_EN)
#error "Not all the required macros (RGB2GRAY_PV_EN and EDGE_DETECTOR_LT_EN) are defined is defined."
#endif

SC_MODULE(Tb_top)
{
  img_initiator *sobel_initiator;
  img_initiator *gray_initiator;
  sobel_edge_detector_tlm *edge_detector_DUT;
  rgb2gray_tlm *rgb2gray_DUT;
  
  SC_CTOR(Tb_top)
  {
    sobel_initiator = new img_initiator("sobel_initiator");
    gray_initiator = new img_initiator("gray_initiator");
    edge_detector_DUT = new sobel_edge_detector_tlm("edge_detector_DUT");
    rgb2gray_DUT = new rgb2gray_tlm("rgb2gray_DUT");
    
    sobel_initiator->start_img_initiators();
    sobel_initiator->set_delays(sc_time(10, SC_NS), sc_time(3, SC_NS));
    edge_detector_DUT->set_delays(sc_time(10, SC_NS), sc_time(3, SC_NS));
    gray_initiator->start_img_initiators();
    gray_initiator->set_delays(sc_time(0, SC_NS), sc_time(0, SC_NS));
    rgb2gray_DUT->set_delays(sc_time(0, SC_NS), sc_time(0, SC_NS));
    
    // Bind initiator socket to target socket
    sobel_initiator->socket.bind(edge_detector_DUT->socket);
    gray_initiator->socket.bind(rgb2gray_DUT->socket);
    
    SC_THREAD(thread_process);
  }
  
  void thread_process()
  {
    Mat colorImage;
    
    unsigned char localR, localG, localB;
    unsigned char localResult;
    
    int localWindow[3][3];
    int localGradientX, localGradientY;
    
    int total_number_of_pixels;
    int current_number_of_pixels = 0;
    float next_target_of_completion = 10.0;
    
    colorImage = imread("../../tools/datagen/src/imgs/car.jpg", IMREAD_UNCHANGED);
  
    if (colorImage.empty())
    { 
      cout << "Image File " << "Not Found" << endl;

      return; 
    }
    
    Mat grayImage(colorImage.rows, colorImage.cols, CV_8UC1);
    Mat detectedImageX(colorImage.rows, colorImage.cols, CV_8UC1);
    Mat detectedImageY(colorImage.rows, colorImage.cols, CV_8UC1);
    Mat detectedImage(colorImage.rows, colorImage.cols, CV_8UC1);
    
    total_number_of_pixels = colorImage.rows * colorImage.cols;
    
    for (int i = 0; i < colorImage.rows; i++)
    {
      for (int j = 0; j < colorImage.cols; j++)
      {
        unsigned char* rgb_pixel = new unsigned char[3];
        unsigned char* data_returned;
        localR = colorImage.at<cv::Vec3b>(i, j)[2];
        localG = colorImage.at<cv::Vec3b>(i, j)[1];
        localB = colorImage.at<cv::Vec3b>(i, j)[0];
        *(rgb_pixel + 0) = localR;
        *(rgb_pixel + 1) = localG;
        *(rgb_pixel + 2) = localB;
        
        dbgprint("Before doing a write in TB");
        gray_initiator->write(rgb_pixel, 0, 3 * sizeof(char));
        dbgprint("After doing a write in TB");
        dbgprint("Before doing a read in TB");
        gray_initiator->read(data_returned, 0, sizeof(char));
        dbgprint("After doing a read in TB");
        dbgprint("Data_returned: %0d", *data_returned);
        
        localResult = *data_returned;
        grayImage.at<uchar>(i, j) = localResult;
        
        current_number_of_pixels++;
        if (((((float)(current_number_of_pixels)) / ((float)(total_number_of_pixels))) * 100.0) >= next_target_of_completion) {
          dbgprint("Image processing completed at %0d", next_target_of_completion);
          next_target_of_completion += 10.0;
        }
      }
    }
    
    dbgprint("Finished gray scale conversion");
    
    current_number_of_pixels = 0;
    next_target_of_completion = 10.0;
    
    for (int i = 0; i < grayImage.rows; i++)
    {
      for (int j = 0; j < grayImage.cols; j++)
      {
        unsigned char* local_window_ptr = new unsigned char[16];
        short int* data_returned;
        unsigned char* write_ptr;
        unsigned char* read_ptr;
        
        if ((i == 0) && (j == 0)) // Upper left corner of the image
        {
          // First row
          *(local_window_ptr    ) = 0;
          *(local_window_ptr + 1) = 0;
          *(local_window_ptr + 2) = 0;
          // Second row
          *(local_window_ptr + 3) = 0;
          *(local_window_ptr + 4) = grayImage.at<uchar>(i    , j    );
          *(local_window_ptr + 5) = grayImage.at<uchar>(i    , j + 1);
          // Third row
          *(local_window_ptr + 6) = 0;
          *(local_window_ptr + 7) = grayImage.at<uchar>(i + 1, j    );
          *(local_window_ptr + 8) = grayImage.at<uchar>(i + 1, j + 1);
        }
        else if ((i == 0) && (j == grayImage.cols - 1)) // Upper right corner of the image
        {
          // First row
          *(local_window_ptr    ) = 0;
          *(local_window_ptr + 1) = 0;
          *(local_window_ptr + 2) = 0;
          // Second row
          *(local_window_ptr + 3) = grayImage.at<uchar>(i    , j - 1);
          *(local_window_ptr + 4) = grayImage.at<uchar>(i    , j    );
          *(local_window_ptr + 5) = 0;
          // Third row
          *(local_window_ptr + 6) = grayImage.at<uchar>(i + 1, j - 1);
          *(local_window_ptr + 7) = grayImage.at<uchar>(i + 1, j    );
          *(local_window_ptr + 8) = 0;
        }
        else if (i == 0) // Upper border
        {
          // First row
          *(local_window_ptr    ) = 0;
          *(local_window_ptr + 1) = 0;
          *(local_window_ptr + 2) = 0;
          // Second row
          *(local_window_ptr + 3) = grayImage.at<uchar>(i    , j - 1);
          *(local_window_ptr + 4) = grayImage.at<uchar>(i    , j    );
          *(local_window_ptr + 5) = grayImage.at<uchar>(i    , j + 1);
          // Third row
          *(local_window_ptr + 6) = grayImage.at<uchar>(i + 1, j - 1);
          *(local_window_ptr + 7) = grayImage.at<uchar>(i + 1, j    );
          *(local_window_ptr + 8) = grayImage.at<uchar>(i + 1, j + 1);
        }
        else if ((i == grayImage.rows - 1) && (j == 0)) // Lower left corner of the image
        {
          // First row
          *(local_window_ptr    ) = 0;
          *(local_window_ptr + 1) = grayImage.at<uchar>(i - 1, j    );
          *(local_window_ptr + 2) = grayImage.at<uchar>(i - 1, j + 1);
          // Second row
          *(local_window_ptr + 3) = 0;
          *(local_window_ptr + 4) = grayImage.at<uchar>(i    , j    );
          *(local_window_ptr + 5) = grayImage.at<uchar>(i    , j + 1);
          // Third row
          *(local_window_ptr + 6) = 0;
          *(local_window_ptr + 7) = 0;
          *(local_window_ptr + 8) = 0;
        }
        else if ((i == grayImage.rows - 1) && (j == grayImage.cols - 1)) // Lower right corner of the image
        {
          // First row
          *(local_window_ptr    ) = grayImage.at<uchar>(i - 1, j - 1);
          *(local_window_ptr + 1) = grayImage.at<uchar>(i - 1, j    );
          *(local_window_ptr + 2) = 0;
          // Second row
          *(local_window_ptr + 3) = grayImage.at<uchar>(i    , j - 1);
          *(local_window_ptr + 4) = grayImage.at<uchar>(i    , j    );
          *(local_window_ptr + 5) = 0;
          // Third row
          *(local_window_ptr + 6) = 0;
          *(local_window_ptr + 7) = 0;
          *(local_window_ptr + 8) = 0;
        }
        else if (i == grayImage.rows - 1) // Lower border of the image
        {
          // First row
          *(local_window_ptr    ) = grayImage.at<uchar>(i - 1, j - 1);
          *(local_window_ptr + 1) = grayImage.at<uchar>(i - 1, j    );
          *(local_window_ptr + 2) = grayImage.at<uchar>(i - 1, j + 1);
          // Second row
          *(local_window_ptr + 3) = grayImage.at<uchar>(i    , j - 1);
          *(local_window_ptr + 4) = grayImage.at<uchar>(i    , j    );
          *(local_window_ptr + 5) = grayImage.at<uchar>(i    , j + 1);
          // Third row
          *(local_window_ptr + 6) = 0;
          *(local_window_ptr + 7) = 0;
          *(local_window_ptr + 8) = 0;
        }
        else if (j == 0) // Left border of the image
        {
          // First row
          *(local_window_ptr    ) = 0;
          *(local_window_ptr + 1) = grayImage.at<uchar>(i - 1, j    );
          *(local_window_ptr + 2) = grayImage.at<uchar>(i - 1, j + 1);
          // Second row
          *(local_window_ptr + 3) = 0;
          *(local_window_ptr + 4) = grayImage.at<uchar>(i    , j    );
          *(local_window_ptr + 5) = grayImage.at<uchar>(i    , j + 1);
          // Third row
          *(local_window_ptr + 6) = 0;
          *(local_window_ptr + 7) = grayImage.at<uchar>(i + 1, j    );
          *(local_window_ptr + 8) = grayImage.at<uchar>(i + 1, j + 1);
        }
        else if (j == grayImage.cols - 1) // Right border of the image
        {
          // First row
          *(local_window_ptr    ) = grayImage.at<uchar>(i - 1, j - 1);
          *(local_window_ptr + 1) = grayImage.at<uchar>(i - 1, j    );
          *(local_window_ptr + 2) = 0;
          // Second row
          *(local_window_ptr + 3) = grayImage.at<uchar>(i    , j - 1);
          *(local_window_ptr + 4) = grayImage.at<uchar>(i    , j    );
          *(local_window_ptr + 5) = 0;
          // Third row
          *(local_window_ptr + 6) = grayImage.at<uchar>(i + 1, j - 1);
          *(local_window_ptr + 7) = grayImage.at<uchar>(i + 1, j    );
          *(local_window_ptr + 8) = 0;
        }
        else // Rest of the image
        {
          // First row
          *(local_window_ptr    ) = grayImage.at<uchar>(i - 1, j - 1);
          *(local_window_ptr + 1) = grayImage.at<uchar>(i - 1, j    );
          *(local_window_ptr + 2) = grayImage.at<uchar>(i - 1, j + 1);
          // Second row
          *(local_window_ptr + 3) = grayImage.at<uchar>(i    , j - 1);
          *(local_window_ptr + 4) = grayImage.at<uchar>(i    , j    );
          *(local_window_ptr + 5) = grayImage.at<uchar>(i    , j + 1);
          // Third row
          *(local_window_ptr + 6) = grayImage.at<uchar>(i + 1, j - 1);
          *(local_window_ptr + 7) = grayImage.at<uchar>(i + 1, j    );
          *(local_window_ptr + 8) = grayImage.at<uchar>(i + 1, j + 1);
        }
        for (int k = 9; k < 16; k++)
        {
          *(local_window_ptr + k) = 0;
        }
        
        write_ptr = local_window_ptr;
        dbgprint("Before doing a write in TB");
        sobel_initiator->write(write_ptr, SOBEL_INPUT_0, 8 * sizeof(char));
        dbgprint("After doing a write in TB");
        write_ptr = (local_window_ptr + 8);
        dbgprint("Before doing a write in TB");
        sobel_initiator->write(write_ptr, SOBEL_INPUT_1, 8 * sizeof(char));
        dbgprint("After doing a write in TB");
        dbgprint("Before doing a read in TB");
        sobel_initiator->read(read_ptr, SOBEL_OUTPUT, 8 * sizeof(char));
        dbgprint("After doing a read in TB");
        data_returned = reinterpret_cast<short int*>(read_ptr);
        dbgprint("Data_returned: %0d\n", *data_returned);
        dbgprint("Data_returned: %0d\n", *(data_returned+1));
        
        localGradientX = *data_returned;
        localGradientY = *(data_returned+1);
        
        if ((localGradientX > 255) || (localGradientX < 255)) {
          detectedImageX.at<uchar>(i, j) = 255;
        } 
        else if (localGradientX < 0) {
          detectedImageX.at<uchar>(i, j) = (unsigned char)(-localGradientX);
        }
        else {
          detectedImageX.at<uchar>(i, j) = (unsigned char)localGradientX; 
        }
        if ((localGradientY > 255) || (localGradientY < 255)) {
          detectedImageY.at<uchar>(i, j) = 255;
        }
        else if (localGradientY < 0) {
          detectedImageY.at<uchar>(i, j) = (unsigned char)(-localGradientY); 
        }
        else {
          detectedImageY.at<uchar>(i, j) = (unsigned char)localGradientY;
        }
      
        localResult = (unsigned char)sqrt((float)(pow(localGradientX, 2)) + (float)(pow(localGradientY, 2)));
        detectedImage.at<uchar>(i, j) = localResult;
        
        current_number_of_pixels++;
        if (((((float)(current_number_of_pixels)) / ((float)(total_number_of_pixels))) * 100.0) >= next_target_of_completion) {
          dbgprint("Image processing completed at %0d", next_target_of_completion);
          next_target_of_completion += 10.0;
        }
      }
    }
    
    imwrite("grayImage.jpg", grayImage);
    imwrite("detectedImageX.jpg", detectedImageX);
    imwrite("detectedImageY.jpg", detectedImageY);
    imwrite("detectedImage.jpg", detectedImage);
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
  
  // Dump the desired signals
  sc_trace(wf, top.edge_detector_DUT->Edge_Detector::data, "data");
  sc_trace(wf, top.edge_detector_DUT->Edge_Detector::address, "address");
  sc_trace(wf, top.edge_detector_DUT->localWindow[0][0], "localWindow(0)(0)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[0][1], "localWindow(0)(1)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[0][2], "localWindow(0)(2)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[1][0], "localWindow(1)(0)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[1][1], "localWindow(1)(1)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[1][2], "localWindow(1)(2)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[2][0], "localWindow(0)(0)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[2][1], "localWindow(2)(1)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[2][2], "localWindow(2)(2)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[0][0], "localMultX(0)(0)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[0][1], "localMultX(0)(1)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[0][2], "localMultX(0)(2)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[1][0], "localMultX(1)(0)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[1][1], "localMultX(1)(1)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[1][2], "localMultX(1)(2)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[2][0], "localMultX(0)(0)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[2][1], "localMultX(2)(1)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[2][2], "localMultX(2)(2)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[0][0], "localMultY(0)(0)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[0][1], "localMultY(0)(1)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[0][2], "localMultY(0)(2)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[1][0], "localMultY(1)(0)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[1][1], "localMultY(1)(1)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[1][2], "localMultY(1)(2)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[2][0], "localMultY(0)(0)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[2][1], "localMultY(2)(1)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[2][2], "localMultY(2)(2)");
  sc_trace(wf, top.edge_detector_DUT->resultSobelGradientX, "resultSobelGradientX");
  sc_trace(wf, top.edge_detector_DUT->resultSobelGradientY, "resultSobelGradientY");
  
  sc_start();

  dbgprint("Terminating simulation");
  sc_close_vcd_trace_file(wf);

  return 0;
}
#endif // TB_EDGE_DETECTOR_TLM_CPP
#endif // USING_TLM_TB_EN
