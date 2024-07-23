#ifndef USING_TLM_TB_EN
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

#include "rgb2gray_pv_model.hpp"

using namespace cv; 

int sc_main(int, char*[])
{
  Mat colorImage;
  
  unsigned char localR, localG, localB;
  unsigned char localResult;

  // Pass command linke arguments
  sc_argc();
  sc_argv();
  
  // Open VCD file
  sc_trace_file* wf = sc_create_vcd_trace_file("rgb2gray");
  wf->set_time_unit(1, SC_NS);

  Rgb2Gray rgb2gray("rgb2gray");
  
  colorImage = imread("../../tools/datagen/src/imgs/car.jpg", IMREAD_UNCHANGED);
  
  if (colorImage.empty())
  { 
    cout << "Image File " << "Not Found" << endl; 

    // wait for any key press 
    return -1; 
  }
  
  Mat greyImage(colorImage.rows, colorImage.cols, CV_8UC1);
  
  sc_start();
  
  for (int i = 0; i < colorImage.rows; i++)
  {
    for (int j = 0; j < colorImage.cols; j++)
    {      
      localR = colorImage.at<cv::Vec3b>(i, j)[2];
      localG = colorImage.at<cv::Vec3b>(i, j)[1];
      localB = colorImage.at<cv::Vec3b>(i, j)[0];
      rgb2gray.set_rgb_pixel(localR, localG, localB);
      localResult = rgb2gray.obtain_gray_value();

      greyImage.at<uchar>(i, j) = localResult;
    }
  }
  
  imshow("Window Name", colorImage);
  
  waitKey(0);
  
  imshow("Window Name", greyImage);
  
  waitKey(0);

  std::cout << "@" << sc_time_stamp() << " Terminating simulation" << std::endl;
  sc_close_vcd_trace_file(wf);

  return 0;
}
#endif // USING_TLM_TB_EN
