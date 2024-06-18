#ifdef IPS_JPG_LT_EN
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

#include "ips_jpg_lt_model.hpp"

const int n_rows = 208;
const int n_cols = 288;

namespace ips
{
struct Image {
    int matrix[n_rows][n_cols] = {0};
};
}

ips::Image dummy_img(int i_rows, int i_cols)
{
    ips::Image dummy = {};
    // Fill the image with values
	int filler = 10;
    for (int i = 0; i < i_rows; i++) {
        for (int j = 0; j < i_cols; j++) {
            dummy.matrix[i][j] = filler++;
			if(filler>180){filler=0;}
        }
    }
 	return dummy;
}

int sc_main (int, char*[]) {
  ips::Image input_image = dummy_img(n_rows, n_cols);
  int image_rows = sizeof(input_image.matrix)/ sizeof(input_image.matrix[0]);
  int image_cols = sizeof(input_image.matrix[0])/ sizeof(int);
  //Image output_image;
  
  //cv::Mat car_image = cv::imread("../../tools/datagen/src/imgs/car.jpg", cv::IMREAD_GRAYSCALE);
  //cout<<car_image.rows<<" "<<car_image.cols<<endl;
  
  jpg_output jpg_comp("jpeg_compressor",image_rows,image_cols);
  
  // Open VCD file
  sc_trace_file *wf = sc_create_vcd_trace_file("jpeg_compressor");
  wf->set_time_unit(1, SC_NS);
  // Dump the desired signals
  sc_trace(wf, input_image.matrix, "input_matrix");
  
  sc_start(0,SC_NS);
  cout << "@" << sc_time_stamp()<< endl;

  cout << "INPUT: an array with " << image_rows*image_cols << " elements" << endl;
  //printMatrix(input_image, image_rows, image_cols);
  for (int i = 0; i < image_rows; ++i) {
  	for (int j = 0; j < image_cols; ++j) {
  		jpg_comp.input_pixel(input_image.matrix[i][j],i,j);
		sc_start(100,SC_NS);
  	}
  }
  
  //output_image = input_image;
  //for (int i = 0; i < image_rows; ++i) {
  //	for (int j = 0; j < image_cols; ++j) {
  //		jpg_comp.OutputPixel(&output_image.matrix[i][j],i,j);
  //	}
  //}
  //printMatrix(output_image, image_rows, image_cols);
  
  int output_size = 0;
  jpg_comp.jpeg_compression(&output_size);
  sc_start(700000,SC_NS);
  
  signed char output_array[output_size];
  sc_trace(wf, output_array, "output_array");
  for (int i = 0; i < output_size; ++i) {
	  jpg_comp.output_byte(output_array,i);
	  sc_start(100,SC_NS);
  }
  
  cout << "OUTPUT: an array with " << output_size << " elements" << endl;
  //printArray(output_array,output_size);
  
  cout << "@" << sc_time_stamp() <<" Terminating simulation\n" << endl;
  sc_close_vcd_trace_file(wf);
  return 0;// Terminate simulation
}
#endif // IPS_JPG_LT_EN
