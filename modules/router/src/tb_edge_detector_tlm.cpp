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

#include "common_func.hpp"
#include "ImportantDefines.h"

#include "memory_tlm.hpp"
#include "rgb2gray_tlm.hpp"
#include "ips_filter_tlm.hpp"
#include "sobel_edge_detector_tlm.hpp"
#include "img_initiator.cpp"

#if !defined(RGB2GRAY_PV_EN) || !defined(EDGE_DETECTOR_AT_EN)
#error "Not all the required macros (RGB2GRAY_PV_EN and EDGE_DETECTOR_LT_EN) are defined is defined."
#endif

SC_MODULE(Tb_top)
{
  img_initiator *sobel_initiator;
  sobel_edge_detector_tlm *edge_detector_DUT;
  Rgb2Gray *rgb2gray_DUT;
  img_initiator *memory_initiator;
  memory_tlm *memory_DUT;
  img_initiator *filter_initiator;
  ips_filter_tlm *filter_DUT;
  
  SC_CTOR(Tb_top)
  {
    sobel_initiator = new img_initiator("sobel_initiator");
    edge_detector_DUT = new sobel_edge_detector_tlm("edge_detector_DUT");
    rgb2gray_DUT = new Rgb2Gray("rgb2gray_DUT");
    memory_initiator = new img_initiator("memory_initiator");
    memory_DUT = new memory_tlm("memory_DUT");
    filter_initiator = new img_initiator("filter_initiator");
    filter_DUT = new ips_filter_tlm("filter_DUT");
    
    sobel_initiator->start_img_initiators();
    sobel_initiator->set_delays(sc_time(10, SC_NS), sc_time(10, SC_NS));
    edge_detector_DUT->set_delays(sc_time(10, SC_NS), sc_time(3, SC_NS));
    memory_initiator->start_img_initiators();
    memory_initiator->set_delays(sc_time(10, SC_NS), sc_time(10, SC_NS));
    memory_DUT->set_delays(sc_time(10, SC_NS), sc_time(3, SC_NS));
    filter_initiator->start_img_initiators();
    filter_initiator->set_delays(sc_time(10, SC_NS), sc_time(10, SC_NS));
    filter_DUT->set_delays(sc_time(10, SC_NS), sc_time(3, SC_NS));
    
    // Bind initiator socket to target socket
    sobel_initiator->socket.bind(edge_detector_DUT->socket);
    memory_initiator->socket.bind(memory_DUT->socket);
    filter_initiator->socket.bind(filter_DUT->socket);
    
    SC_THREAD(thread_process);
  }
  
  void thread_process()
  {
    Mat colorImage;
    
    unsigned char localR, localG, localB;
    unsigned short int localResult;
    unsigned char* local_results;
    int local_count = 0;
    
    short int localGradientX, localGradientY;
    
    int total_number_of_pixels;
    int current_number_of_pixels = 0;
    float next_target_of_completion = 10.0;
    
    colorImage = imread("../../tools/datagen/src/imgs/car.jpg", IMREAD_UNCHANGED);
  
    if (colorImage.empty())
    { 
      cout << "Image File " << "Not Found" << endl;

      return; 
    }
    if ((colorImage.rows > IMAG_ROWS) || (colorImage.cols > IMAG_COLS))
    {
      cout << "Image size " << colorImage.rows << "x" << colorImage.cols << " won't fit in the designated space";
      
      return;
    }
    
    dbgprint("Saving image in memory");
    for (int i = 0; i < colorImage.rows; i++)
    {
      for (int j = 0; j < colorImage.cols; j++)
      {
        localR = colorImage.at<cv::Vec3b>(i, j)[2];
        localG = colorImage.at<cv::Vec3b>(i, j)[1];
        localB = colorImage.at<cv::Vec3b>(i, j)[0];
        
        local_results = new unsigned char[3];
        *(local_results    ) = localR;
        *(local_results + 1) = localG;
        *(local_results + 2) = localB;
        memory_DUT->backdoor_write(local_results, 3 * sizeof(char), IMG_INPUT + ((i * IMAG_COLS * 3) + (j * 3)));
      }
    }
    
    // Fill remaining columns in case that width of the image was smaller than IMAG_COLS
    for (int j = colorImage.cols; j < IMAG_COLS; j++)
    {
      for (int i = 0; i < colorImage.rows; i++)
      {
        local_results = new unsigned char[3];
        *(local_results    ) = 0;
        *(local_results + 1) = 0;
        *(local_results + 2) = 0;
        memory_DUT->backdoor_write(local_results, 3 * sizeof(char), IMG_INPUT + ((i * IMAG_COLS * 3) + (j * 3)));
      }
    }
    
    // Fill remaining columns in case that height of the image was smaller than IMAG_ROWS
    for (int i = colorImage.rows; i < IMAG_ROWS; i++)
    {
      for (int j = 0; j < IMAG_COLS; j++)
      {
        local_results = new unsigned char[3];
        *(local_results    ) = 0;
        *(local_results + 1) = 0;
        *(local_results + 2) = 0;
        memory_DUT->backdoor_write(local_results, 3 * sizeof(char), IMG_INPUT + ((i * IMAG_COLS * 3) + (j * 3)));
      }
    }
    
    Mat grayImagePrevMem(IMAG_ROWS, IMAG_COLS, CV_8UC1);
    Mat filteredImagePrevMem(IMAG_ROWS, IMAG_COLS, CV_8UC1);
    Mat detectedImagePrevMemX(IMAG_ROWS, IMAG_COLS, CV_8UC1);
    Mat detectedImagePrevMemY(IMAG_ROWS, IMAG_COLS, CV_8UC1);
    Mat detectedImagePrevMem(IMAG_ROWS, IMAG_COLS, CV_8UC1);
    
    Mat grayImageAfterMem(IMAG_ROWS, IMAG_COLS, CV_8UC1);
    Mat filteredImageAfterMem(IMAG_ROWS, IMAG_COLS, CV_8UC1);
    Mat detectedImageAfterMemX(IMAG_ROWS, IMAG_COLS, CV_8UC1);
    Mat detectedImageAfterMemY(IMAG_ROWS, IMAG_COLS, CV_8UC1);
    Mat detectedImageAfterMem(IMAG_ROWS, IMAG_COLS, CV_8UC1);
    
    total_number_of_pixels = IMAG_ROWS * IMAG_COLS;
    
    for (int i = 0; i < IMAG_ROWS; i++)
    {
      int local_group_count = 0;
      for (int j = 0; j < IMAG_COLS; j++)
      {
        unsigned char* read_ptr;
        dbgprint("Before doing a read in TB");
        memory_initiator->read(read_ptr, IMG_INPUT + ((i * IMAG_COLS * 3) + (j * 3)), 3 * sizeof(char));
        dbgprint("After doing a read in TB");
        localR = *(read_ptr    );
        localG = *(read_ptr + 1);
        localB = *(read_ptr + 2);
        rgb2gray_DUT->set_rgb_pixel(localR, localG, localB);
        localResult = (unsigned short int)rgb2gray_DUT->obtain_gray_value();
        
        dbgprint("Data_returned: %0d", localResult);

        grayImagePrevMem.at<uchar>(i, j) = (unsigned char)localResult;
        
        if (local_count == 0)
        {
          local_results = new unsigned char[8];
        }
        
        *(local_results + local_count) = (unsigned char)localResult;
        
        local_count++;
        
        if (local_count == 8)
        {
          dbgprint("Before doing a write in TB");
          memory_initiator->write(local_results, IMG_INPROCESS_A + ((i * IMAG_COLS) + (local_group_count * 8 * sizeof(char))), 8 * sizeof(char));
          dbgprint("After doing a write in TB");
          local_count = 0;
          local_group_count++;
        }
        
        current_number_of_pixels++;
        if (((((float)(current_number_of_pixels)) / ((float)(total_number_of_pixels))) * 100.0) >= next_target_of_completion) {
          dbgprint("Image processing completed at %f", next_target_of_completion);
          next_target_of_completion += 10.0;
        }
      }
    }
    
    dbgprint("Finished gray scale conversion");
    
    // Sanity check that the image was written in memory as expected
    for (int i = 0; i < IMAG_ROWS; i++)
    {
      for (int j = 0; j < IMAG_COLS; j++)
      {
        unsigned char* read_ptr;
        memory_DUT->backdoor_read(read_ptr, 1 * sizeof(char), IMG_INPROCESS_A + ((i * IMAG_COLS) + j));
        grayImageAfterMem.at<uchar>(i, j) = *read_ptr;
      }
    }
    
    local_count = 0;
    current_number_of_pixels = 0;
    next_target_of_completion = 10.0;
    
    for (int i = 0; i < IMAG_ROWS; i++)
    {
      int local_group_count = 0;
      for (int j = 0; j < IMAG_COLS; j++)
      {
        unsigned char* local_window_ptr = new unsigned char[9];
        IPS_OUT_TYPE_TB* data_returned_ptr;
        unsigned char* write_ptr;
        unsigned char* read_ptr;
        IPS_OUT_TYPE_TB data_returned;
        
        extract_window(i, j, IMG_INPROCESS_A, local_window_ptr);
        
        write_ptr = local_window_ptr;
        dbgprint("Before doing a write in TB");
        filter_initiator->write(write_ptr, IMG_FILTER_KERNEL, 9 * sizeof(char));
        dbgprint("After doing a write in TB");
        dbgprint("Before doing a read in TB");
        filter_initiator->read(read_ptr, IMG_FILTER_KERNEL, sizeof(IPS_OUT_TYPE_TB));
        dbgprint("After doing a read in TB");
        data_returned_ptr = reinterpret_cast<IPS_OUT_TYPE_TB*>(read_ptr);
        dbgprint("Data_returned: %f\n", *data_returned_ptr);
        
        data_returned = *data_returned_ptr;
        
        if (data_returned > 255.0) {
          filteredImagePrevMem.at<uchar>(i, j) = 255;
        }
        else {
          filteredImagePrevMem.at<uchar>(i, j) = (unsigned char)data_returned; 
        }
        
        if (local_count == 0)
        {
          local_results = new unsigned char[8];
        }
        
        if (data_returned > 255.0) {
          *(local_results + local_count) = 255;
        }
        else {
          *(local_results + local_count) = (unsigned char)data_returned; 
        }
        
        local_count++;
        
        if (local_count == 8)
        {
          dbgprint("Before doing a write in TB");
          memory_initiator->write(local_results, IMG_COMPRESSED + ((i * IMAG_COLS) + (local_group_count * 8 * sizeof(char))), 8 * sizeof(char));
          dbgprint("After doing a write in TB");
          local_count = 0;
          local_group_count++;
        }
        
        current_number_of_pixels++;
        if (((((float)(current_number_of_pixels)) / ((float)(total_number_of_pixels))) * 100.0) >= next_target_of_completion) {
          dbgprint("Image processing completed at %f", next_target_of_completion);
          next_target_of_completion += 10.0;
        }
      }
    }
    
    dbgprint("Finished filtering the image");
    
    // Sanity check that the image was written in memory as expected
    for (int i = 0; i < IMAG_ROWS; i++)
    {
      for (int j = 0; j < IMAG_COLS; j++)
      {
        unsigned char* read_ptr;
        memory_DUT->backdoor_read(read_ptr, 1 * sizeof(char), IMG_COMPRESSED + ((i * IMAG_COLS) + j));
        filteredImageAfterMem.at<uchar>(i, j) = *read_ptr;
      }
    }
    
    local_count = 0;
    current_number_of_pixels = 0;
    next_target_of_completion = 10.0;
    
    for (int i = 0; i < IMAG_ROWS; i++)
    {
      int local_group_count = 0;
      for (int j = 0; j < IMAG_COLS; j++)
      {
        unsigned char* local_window_ptr = new unsigned char[16];
        short int* data_returned_ptr;
        unsigned char* write_ptr;
        unsigned char* read_ptr;
        
        extract_window(i, j, IMG_COMPRESSED, local_window_ptr);
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
        data_returned_ptr = reinterpret_cast<short int*>(read_ptr);
        dbgprint("Data_returned: %0d\n", *data_returned_ptr);
        dbgprint("Data_returned: %0d\n", *(data_returned_ptr+1));
        
        localGradientX = *data_returned_ptr;
        localGradientY = *(data_returned_ptr+1);
        
        if ((localGradientX > 255) || (localGradientX < -255)) {
          detectedImagePrevMemX.at<uchar>(i, j) = 255;
        } 
        else if (localGradientX < 0) {
          detectedImagePrevMemX.at<uchar>(i, j) = (unsigned char)(-localGradientX);
        }
        else {
          detectedImagePrevMemX.at<uchar>(i, j) = (unsigned char)localGradientX; 
        }
        if ((localGradientY > 255) || (localGradientY < -255)) {
          detectedImagePrevMemY.at<uchar>(i, j) = 255;
        }
        else if (localGradientY < 0) {
          detectedImagePrevMemY.at<uchar>(i, j) = (unsigned char)(-localGradientY); 
        }
        else {
          detectedImagePrevMemY.at<uchar>(i, j) = (unsigned char)localGradientY;
        }
      
        localResult = (unsigned short int)sqrt((float)(pow(localGradientX, 2)) + (float)(pow(localGradientY, 2)));
        if (localResult > 255)
        {
          detectedImagePrevMem.at<uchar>(i, j) = 255;
        }
        else
        {
          detectedImagePrevMem.at<uchar>(i, j) = (unsigned char)localResult;
        }
        
        if (local_count == 0)
        {
          local_results = new unsigned char[16];
        }
        
        memcpy((local_results + (local_count * 2)                          ), (data_returned_ptr    ), sizeof(short int));
        memcpy((local_results + (local_count * 2) + (4 * sizeof(short int))), (data_returned_ptr + 1), sizeof(short int));
        local_count++;
        
        if (local_count == 4)
        {
          write_ptr = local_results;
          dbgprint("Before doing a write in TB");
          memory_initiator->write(write_ptr, IMG_INPROCESS_B + ((i * IMAG_COLS * sizeof(short int)) + (local_group_count * 4 * sizeof(short int))), 4 * sizeof(short int));
          dbgprint("After doing a write in TB");
          write_ptr = (local_results + 8);
          dbgprint("Before doing a write in TB");
          memory_initiator->write(write_ptr, IMG_INPROCESS_C + ((i * IMAG_COLS * sizeof(short int)) + (local_group_count * 4 * sizeof(short int))), 4 * sizeof(short int));
          dbgprint("After doing a write in TB");
          local_count = 0;
          local_group_count++;
        }
        
        current_number_of_pixels++;
        if (((((float)(current_number_of_pixels)) / ((float)(total_number_of_pixels))) * 100.0) >= next_target_of_completion) {
          dbgprint("Image processing completed at %f", next_target_of_completion);
          next_target_of_completion += 10.0;
        }
      }
    }
    
    // Sanity check that the image was written in memory as expected
    for (int i = 0; i < IMAG_ROWS; i++)
    {
      for (int j = 0; j < IMAG_COLS; j++)
      {
        unsigned char* read_ptr;
        short int* data_returned_ptr;
        
        memory_DUT->backdoor_read(read_ptr, sizeof(short int), IMG_INPROCESS_B + ((i * IMAG_COLS * sizeof(short int)) + (j * sizeof(short int))));
        data_returned_ptr = reinterpret_cast<short int*>(read_ptr);
        localGradientX = *data_returned_ptr;
        if ((localGradientX > 255) || (localGradientX < -255)) {
          detectedImageAfterMemX.at<uchar>(i, j) = 255;
        } 
        else if (localGradientX < 0) {
          detectedImageAfterMemX.at<uchar>(i, j) = (unsigned char)(-localGradientX);
        }
        else {
          detectedImageAfterMemX.at<uchar>(i, j) = (unsigned char)localGradientX; 
        }
        
        memory_DUT->backdoor_read(read_ptr, sizeof(short int), IMG_INPROCESS_C + ((i * IMAG_COLS * sizeof(short int)) + (j * sizeof(short int))));
        data_returned_ptr = reinterpret_cast<short int*>(read_ptr);
        localGradientY = *data_returned_ptr;
        if ((localGradientY > 255) || (localGradientY < -255)) {
          detectedImageAfterMemY.at<uchar>(i, j) = 255;
        } 
        else if (localGradientY < 0) {
          detectedImageAfterMemY.at<uchar>(i, j) = (unsigned char)(-localGradientY);
        }
        else {
          detectedImageAfterMemY.at<uchar>(i, j) = (unsigned char)localGradientY; 
        }
      }
    }
    
    imwrite("grayImagePrevMem.jpg", grayImagePrevMem);
    imwrite("grayImageAfterMem.jpg", grayImageAfterMem);
    imwrite("filteredImagePrevMem.jpg", filteredImagePrevMem);
    imwrite("filteredImageAfterMem.jpg", filteredImageAfterMem);
    imwrite("detectedImagePrevMemX.jpg", detectedImagePrevMemX);
    imwrite("detectedImageAfterMemX.jpg", detectedImageAfterMemX);
    imwrite("detectedImagePrevMemY.jpg", detectedImagePrevMemY);
    imwrite("detectedImageAfterMemY.jpg", detectedImageAfterMemY);
    imwrite("detectedImagePrevMem.jpg", detectedImagePrevMem);
    imwrite("detectedImageAfterMem.jpg", detectedImageAfterMem);
  }
  
  void extract_window(int i, int j, unsigned int initial_address, unsigned char*& local_window_ptr)
  {
    unsigned char* read_ptr;
    if ((i == 0) && (j == 0)) // Upper left corner of the image
    {
      // First row
      *(local_window_ptr    ) = 0;
      *(local_window_ptr + 1) = 0;
      *(local_window_ptr + 2) = 0;
      // Second row
      dbgprint("Before doing a read in TB");
      memory_initiator->read(read_ptr, initial_address, 2 * sizeof(char));
      dbgprint("After doing a read in TB");
      *(local_window_ptr + 3) = 0;
      *(local_window_ptr + 4) = *(read_ptr    );
      *(local_window_ptr + 5) = *(read_ptr + 1);
      // Third row
      dbgprint("Before doing a read in TB");
      memory_initiator->read(read_ptr, initial_address  + IMAG_COLS, 2 * sizeof(char));
      dbgprint("After doing a read in TB");
      *(local_window_ptr + 6) = 0;
      *(local_window_ptr + 7) = *(read_ptr    );
      *(local_window_ptr + 8) = *(read_ptr + 1);
    }
    else if ((i == 0) && (j == IMAG_COLS - 1)) // Upper right corner of the image
    {
      // First row
      *(local_window_ptr    ) = 0;
      *(local_window_ptr + 1) = 0;
      *(local_window_ptr + 2) = 0;
      // Second row
      dbgprint("Before doing a read in TB");
      memory_initiator->read(read_ptr, initial_address + (IMAG_COLS - 2), 2 * sizeof(char));
      dbgprint("After doing a read in TB");
      *(local_window_ptr + 3) = *(read_ptr    );
      *(local_window_ptr + 4) = *(read_ptr + 1);
      *(local_window_ptr + 5) = 0;
      // Third row
      dbgprint("Before doing a read in TB");
      memory_initiator->read(read_ptr, initial_address + (IMAG_COLS + (IMAG_COLS - 2)), 2 * sizeof(char));
      dbgprint("After doing a read in TB");
      *(local_window_ptr + 6) = *(read_ptr    );
      *(local_window_ptr + 7) = *(read_ptr + 1);
      *(local_window_ptr + 8) = 0;
    }
    else if (i == 0) // Upper border
    {
      // First row
      *(local_window_ptr    ) = 0;
      *(local_window_ptr + 1) = 0;
      *(local_window_ptr + 2) = 0;
      // Second row
      dbgprint("Before doing a read in TB");
      memory_initiator->read(read_ptr, initial_address + (j - 1), 3 * sizeof(char));
      dbgprint("After doing a read in TB");
      *(local_window_ptr + 3) = *(read_ptr    );
      *(local_window_ptr + 4) = *(read_ptr + 1);
      *(local_window_ptr + 5) = *(read_ptr + 2);
      // Third row
      dbgprint("Before doing a read in TB");
      memory_initiator->read(read_ptr, initial_address + (IMAG_COLS + (j - 1)), 3 * sizeof(char));
      dbgprint("After doing a read in TB");
      *(local_window_ptr + 6) = *(read_ptr    );
      *(local_window_ptr + 7) = *(read_ptr + 1);
      *(local_window_ptr + 8) = *(read_ptr + 2);
    }
    else if ((i == IMAG_ROWS - 1) && (j == 0)) // Lower left corner of the image
    {
      // First row
      dbgprint("Before doing a read in TB");
      memory_initiator->read(read_ptr, initial_address + ((IMAG_ROWS - 2) * IMAG_COLS), 2 * sizeof(char));
      dbgprint("After doing a read in TB");
      *(local_window_ptr    ) = 0;
      *(local_window_ptr + 1) = *(read_ptr    );
      *(local_window_ptr + 2) = *(read_ptr + 1);
      // Second row
      dbgprint("Before doing a read in TB");
      memory_initiator->read(read_ptr, initial_address + ((IMAG_ROWS - 1) * IMAG_COLS), 2 * sizeof(char));
      dbgprint("After doing a read in TB");
      *(local_window_ptr + 3) = 0;
      *(local_window_ptr + 4) = *(read_ptr    );
      *(local_window_ptr + 5) = *(read_ptr + 1);
      // Third row
      *(local_window_ptr + 6) = 0;
      *(local_window_ptr + 7) = 0;
      *(local_window_ptr + 8) = 0;
    }
    else if ((i == IMAG_ROWS - 1) && (j == IMAG_COLS - 1)) // Lower right corner of the image
    {
      // First row
      dbgprint("Before doing a read in TB");
      memory_initiator->read(read_ptr, initial_address + (((IMAG_ROWS - 2) * IMAG_COLS) + (IMAG_COLS - 2)), 2 * sizeof(char));
      dbgprint("After doing a read in TB");
      *(local_window_ptr    ) = *(read_ptr    );
      *(local_window_ptr + 1) = *(read_ptr + 1);
      *(local_window_ptr + 2) = 0;
      // Second row
      dbgprint("Before doing a read in TB");
      memory_initiator->read(read_ptr, initial_address + (((IMAG_ROWS - 1) * IMAG_COLS) + (IMAG_COLS - 2)), 2 * sizeof(char));
      dbgprint("After doing a read in TB");
      *(local_window_ptr + 3) = *(read_ptr    );
      *(local_window_ptr + 4) = *(read_ptr + 1);
      *(local_window_ptr + 5) = 0;
      // Third row
      *(local_window_ptr + 6) = 0;
      *(local_window_ptr + 7) = 0;
      *(local_window_ptr + 8) = 0;
    }
    else if (i == IMAG_ROWS - 1) // Lower border of the image
    {
      // First row
      dbgprint("Before doing a read in TB");
      memory_initiator->read(read_ptr, initial_address + (((IMAG_ROWS - 2) * IMAG_COLS) + (j - 1)), 3 * sizeof(char));
      dbgprint("After doing a read in TB");
      *(local_window_ptr    ) = *(read_ptr    );
      *(local_window_ptr + 1) = *(read_ptr + 1);
      *(local_window_ptr + 2) = *(read_ptr + 2);
      // Second row
      dbgprint("Before doing a read in TB");
      memory_initiator->read(read_ptr, initial_address + (((IMAG_ROWS - 1) * IMAG_COLS) + (j - 1)), 3 * sizeof(char));
      dbgprint("After doing a read in TB");
      *(local_window_ptr + 3) = *(read_ptr    );
      *(local_window_ptr + 4) = *(read_ptr + 1);
      *(local_window_ptr + 5) = *(read_ptr + 2);
      // Third row
      *(local_window_ptr + 6) = 0;
      *(local_window_ptr + 7) = 0;
      *(local_window_ptr + 8) = 0;
    }
    else if (j == 0) // Left border of the image
    {
      // First row
      dbgprint("Before doing a read in TB");
      memory_initiator->read(read_ptr, initial_address + ((i - 1) * IMAG_COLS), 2 * sizeof(char));
      dbgprint("After doing a read in TB");
      *(local_window_ptr    ) = 0;
      *(local_window_ptr + 1) = *(read_ptr    );
      *(local_window_ptr + 2) = *(read_ptr + 1);
      // Second row
      dbgprint("Before doing a read in TB");
      memory_initiator->read(read_ptr, initial_address + (i * IMAG_COLS), 2 * sizeof(char));
      dbgprint("After doing a read in TB");
      *(local_window_ptr + 3) = 0;
      *(local_window_ptr + 4) = *(read_ptr    );
      *(local_window_ptr + 5) = *(read_ptr + 1);
      // Third row
      dbgprint("Before doing a read in TB");
      memory_initiator->read(read_ptr, initial_address + ((i + 1) * IMAG_COLS), 2 * sizeof(char));
      dbgprint("After doing a read in TB");
      *(local_window_ptr + 6) = 0;
      *(local_window_ptr + 7) = *(read_ptr    );
      *(local_window_ptr + 8) = *(read_ptr + 1);
    }
    else if (j == IMAG_COLS - 1) // Right border of the image
    {
      // First row
      dbgprint("Before doing a read in TB");
      memory_initiator->read(read_ptr, initial_address + (((i - 1) * IMAG_COLS) + (j - 1)), 2 * sizeof(char));
      dbgprint("After doing a read in TB");
      *(local_window_ptr    ) = *(read_ptr    );
      *(local_window_ptr + 1) = *(read_ptr + 1);
      *(local_window_ptr + 2) = 0;
      // Second row
      dbgprint("Before doing a read in TB");
      memory_initiator->read(read_ptr, initial_address + ((i * IMAG_COLS) + (j - 1)), 2 * sizeof(char));
      dbgprint("After doing a read in TB");
      *(local_window_ptr + 3) = *(read_ptr    );
      *(local_window_ptr + 4) = *(read_ptr + 1);
      *(local_window_ptr + 5) = 0;
      // Third row
      dbgprint("Before doing a read in TB");
      memory_initiator->read(read_ptr, initial_address + (((i + 1) * IMAG_COLS) + (j - 1)), 2 * sizeof(char));
      dbgprint("After doing a read in TB");
      *(local_window_ptr + 6) = *(read_ptr    );
      *(local_window_ptr + 7) = *(read_ptr + 1);
      *(local_window_ptr + 8) = 0;
    }
    else // Rest of the image
    {
      // First row
      dbgprint("Before doing a read in TB");
      memory_initiator->read(read_ptr, initial_address + (((i - 1) * IMAG_COLS) + (j - 1)), 3 * sizeof(char));
      dbgprint("After doing a read in TB");
      *(local_window_ptr    ) = *(read_ptr    );
      *(local_window_ptr + 1) = *(read_ptr + 1);
      *(local_window_ptr + 2) = *(read_ptr + 2);
      // Second row
      dbgprint("Before doing a read in TB");
      memory_initiator->read(read_ptr, initial_address + ((i * IMAG_COLS) + (j - 1)), 3 * sizeof(char));
      dbgprint("After doing a read in TB");
      *(local_window_ptr + 3) = *(read_ptr    );
      *(local_window_ptr + 4) = *(read_ptr + 1);
      *(local_window_ptr + 5) = *(read_ptr + 2);
      // Third row
      dbgprint("Before doing a read in TB");
      memory_initiator->read(read_ptr, initial_address + (((i + 1) * IMAG_COLS) + (j - 1)), 3 * sizeof(char));
      dbgprint("After doing a read in TB");
      *(local_window_ptr + 6) = *(read_ptr    );
      *(local_window_ptr + 7) = *(read_ptr + 1);
      *(local_window_ptr + 8) = *(read_ptr + 2);
    }
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
  sc_trace(wf, top.filter_DUT->img_window[0], "filter_window(0)(0)");
  sc_trace(wf, top.filter_DUT->img_window[1], "filter_window(0)(1)");
  sc_trace(wf, top.filter_DUT->img_window[2], "filter_window(0)(2)");
  sc_trace(wf, top.filter_DUT->img_window[3], "filter_window(1)(0)");
  sc_trace(wf, top.filter_DUT->img_window[4], "filter_window(1)(1)");
  sc_trace(wf, top.filter_DUT->img_window[5], "filter_window(1)(2)");
  sc_trace(wf, top.filter_DUT->img_window[6], "filter_window(2)(0)");
  sc_trace(wf, top.filter_DUT->img_window[7], "filter_window(2)(1)");
  sc_trace(wf, top.filter_DUT->img_window[8], "filter_window(2)(2)");
  sc_trace(wf, top.filter_DUT->img_result, "filter_result");
  sc_trace(wf, top.edge_detector_DUT->Edge_Detector::data, "sobel_data");
  sc_trace(wf, top.edge_detector_DUT->Edge_Detector::address, "sobel_address");
  sc_trace(wf, top.edge_detector_DUT->localWindow[0][0], "sobel_localWindow(0)(0)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[0][1], "sobel_localWindow(0)(1)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[0][2], "sobel_localWindow(0)(2)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[1][0], "sobel_localWindow(1)(0)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[1][1], "sobel_localWindow(1)(1)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[1][2], "sobel_localWindow(1)(2)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[2][0], "sobel_localWindow(0)(0)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[2][1], "sobel_localWindow(2)(1)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[2][2], "sobel_localWindow(2)(2)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[0][0], "sobel_localMultX(0)(0)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[0][1], "sobel_localMultX(0)(1)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[0][2], "sobel_localMultX(0)(2)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[1][0], "sobel_localMultX(1)(0)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[1][1], "sobel_localMultX(1)(1)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[1][2], "sobel_localMultX(1)(2)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[2][0], "sobel_localMultX(0)(0)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[2][1], "sobel_localMultX(2)(1)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[2][2], "sobel_localMultX(2)(2)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[0][0], "sobel_localMultY(0)(0)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[0][1], "sobel_localMultY(0)(1)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[0][2], "sobel_localMultY(0)(2)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[1][0], "sobel_localMultY(1)(0)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[1][1], "sobel_localMultY(1)(1)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[1][2], "sobel_localMultY(1)(2)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[2][0], "sobel_localMultY(0)(0)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[2][1], "sobel_localMultY(2)(1)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[2][2], "sobel_localMultY(2)(2)");
  sc_trace(wf, top.edge_detector_DUT->resultSobelGradientX, "sobel_resultSobelGradientX");
  sc_trace(wf, top.edge_detector_DUT->resultSobelGradientY, "sobel_resultSobelGradientY");
  sc_trace(wf, top.memory_DUT->mem_data, "memory_data");
  sc_trace(wf, top.memory_DUT->mem_address, "memory_address");
  sc_trace(wf, top.memory_DUT->mem_we, "memory_we");
  
  sc_start();

  dbgprint("Terminating simulation");
  sc_close_vcd_trace_file(wf);

  return 0;
}
#endif // TB_EDGE_DETECTOR_TLM_CPP
#endif // USING_TLM_TB_EN
