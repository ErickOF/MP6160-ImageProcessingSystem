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

#include "ips_jpg_at_model.cpp"

const int n_rows = 208;
const int n_cols = 288;

struct Image {
    int matrix[n_rows][n_cols];
};

Image dummy_img(int i_rows, int i_cols)
{
    Image dummy;
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

void print_matrix(Image image, int image_rows, int image_cols)
{
    for (int i = 0; i < image_rows; ++i) {
        for (int j = 0; j < image_cols; ++j) {
            cout << image.matrix[i][j]<<" ";
        }
        cout << endl;
    }
}

void print_array(signed char *Arr, int array_length)
{
    for (int i = 0; i < array_length; ++i) {
       cout << int(Arr[i])<<" ";
    }
	cout << endl;
}
int sc_main (int argc, char* argv[]) {
  sc_signal<sc_int<32> > matrix_element;
  sc_signal<sc_int<32> > matrix_row;
  sc_signal<sc_int<32> > matrix_col;
  sc_signal<sc_int<8> > Array_element;
  sc_signal<sc_int<32> > array_index;
  sc_signal<sc_int<32> > Array_size;
  
  Image input_image = dummy_img(n_rows, n_cols);
  int image_rows = sizeof(input_image.matrix)/ sizeof(input_image.matrix[0]);
  int image_cols = sizeof(input_image.matrix[0])/ sizeof(int);
  //Image output_image;

  //cv::Mat car_image = cv::imread("../../tools/datagen/src/imgs/car.jpg", cv::IMREAD_GRAYSCALE);
  //cout<<car_image.rows<<" "<<car_image.cols<<endl;
  
  jpg_output jpg_comp("jpeg_compressor");
  jpg_comp.pixel_value_signal(matrix_element);
  jpg_comp.row_signal(matrix_row);
  jpg_comp.col_signal(matrix_col);
  jpg_comp.element_signal(Array_element);
  jpg_comp.index_signal(array_index);
  jpg_comp.output_size_signal(Array_size);
  
  // Open VCD file
  sc_trace_file *wf = sc_create_vcd_trace_file("jpeg_compressor");
  wf->set_time_unit(1, SC_NS);
  // Dump the desired signals
  sc_trace(wf, input_image.matrix, "input_matrix");
  
  sc_start(0,SC_NS);
  cout << "@" << sc_time_stamp()<< endl;

  matrix_row = image_rows;
  matrix_col = image_cols;
  jpg_comp.Starter();
  sc_start(100,SC_NS);

  cout << "INPUT: an array with " << image_rows*image_cols << " elements" << endl;
  //print_matrix(input_image, image_rows, image_cols);
  for (int i = 0; i < image_rows; ++i) {
  	for (int j = 0; j < image_cols; ++j) {
		matrix_row = i;
		matrix_col = j;
		matrix_element = input_image.matrix[i][j];
  		jpg_comp.input_pixel();
		sc_start(100,SC_NS);
  	}
  }
  
  //output_image = input_image;
  //for (int i = 0; i < image_rows; ++i) {
  //	for (int j = 0; j < image_cols; ++j) {
  //		jpg_comp.output_pixel(&output_image.matrix[i][j],i,j);
  //	}
  //}
  //print_matrix(output_image, image_rows, image_cols);
  
  int output_size = 0;
  jpg_comp.jpeg_compression();
  sc_start(700000,SC_NS);
  output_size = Array_size.read();
  
  signed char output_array[output_size];
  sc_trace(wf, output_array, "output_array");
  for (int i = 0; i < output_size; ++i) {
	  array_index = i;
	  jpg_comp.output_byte();
	  sc_start(100,SC_NS);
	  output_array[i] = Array_element.read();
  }
  
  cout << "OUTPUT: an array with " << output_size << " elements" << endl;
  //print_array(output_array,output_size);
  
  cout << "@" << sc_time_stamp() <<" Terminating simulation\n" << endl;
  sc_close_vcd_trace_file(wf);
  return 0;// Terminate simulation

 }
