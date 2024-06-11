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

const int Nrows = 16;
const int Ncols = 24;

struct Image {
    int matrix[Nrows][Ncols];
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

void printMatrix(Image image, int image_rows, int image_cols)
{
    for (int i = 0; i < image_rows; ++i) {
        for (int j = 0; j < image_cols; ++j) {
            cout << image.matrix[i][j]<<" ";
        }
        cout << endl;
    }
}

void printArray(signed char *Arr, int array_length)
{
    for (int i = 0; i < array_length; ++i) {
       cout << int(Arr[i])<<" ";
    }
	cout << endl;
}
int sc_main (int argc, char* argv[]) {
  sc_signal<sc_int<32> > Matrix_element;
  sc_signal<sc_int<32> > Matrix_row;
  sc_signal<sc_int<32> > Matrix_col;
  sc_signal<sc_int<8> > Array_element;
  sc_signal<sc_int<32> > Array_index;
  sc_signal<sc_int<32> > Array_size;
  
  Image input_image = dummy_img(Nrows, Ncols);
  int image_rows = sizeof(input_image.matrix)/ sizeof(input_image.matrix[0]);
  int image_cols = sizeof(input_image.matrix[0])/ sizeof(int);
  //Image output_image;

  //cv::Mat car_image = cv::imread("../../tools/datagen/src/imgs/car.jpg", cv::IMREAD_GRAYSCALE);
  //cout<<car_image.rows<<" "<<car_image.cols<<endl;
  
  jpg_output jpg_comp("jpeg_compressor");
  jpg_comp.PixelValue_signal(Matrix_element);
  jpg_comp.row_signal(Matrix_row);
  jpg_comp.col_signal(Matrix_col);
  jpg_comp.Element_signal(Array_element);
  jpg_comp.index_signal(Array_index);
  jpg_comp.output_size_signal(Array_size);
  
  // Open VCD file
  sc_trace_file *wf = sc_create_vcd_trace_file("jpeg_compressor");
  wf->set_time_unit(1, SC_NS);
  // Dump the desired signals
  sc_trace(wf, input_image.matrix, "input_matrix");
  
  sc_start(0,SC_NS);
  cout << "@" << sc_time_stamp()<< endl;

  Matrix_row = image_rows;
  Matrix_col = image_cols;
  jpg_comp.Starter();
  sc_start(100,SC_NS);

  cout << "INPUT: an array with " << image_rows*image_cols << " elements" << endl;
  //printMatrix(input_image, image_rows, image_cols);
  for (int i = 0; i < image_rows; ++i) {
  	for (int j = 0; j < image_cols; ++j) {
		Matrix_row = i;
		Matrix_col = j;
		Matrix_element = input_image.matrix[i][j];
  		jpg_comp.InputPixel();
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
  
  int output_size;
  jpg_comp.JPEG_compression();
  sc_start(10000,SC_NS);
  output_size = Array_size.read();
  
  signed char output_array[output_size];
  sc_trace(wf, output_array, "output_array");
  for (int i = 0; i < output_size; ++i) {
	  Array_index = i;
	  jpg_comp.OutputByte();
	  sc_start(100,SC_NS);
	  output_array[i] = Array_element.read();
  }
  
  cout << "OUTPUT: an array with " << output_size << " elements" << endl;
  //printArray(output_array,output_size);
  
  cout << "@" << sc_time_stamp() <<" Terminating simulation\n" << endl;
  sc_close_vcd_trace_file(wf);
  return 0;// Terminate simulation

 }
