//--------------------------------------------------------
//Testbench: Unification
//By: Roger Morales Monge
//Description: Simple TB for pixel unification modules
//--------------------------------------------------------

#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "include/stb_image_write.h"

#include <systemc.h>
#include "math.h"

#ifdef IMG_UNIFICATE_PV_EN
#include "unification_pv_model.hpp"
#endif

int sc_main (int argc, char* argv[]) {
              
  unsigned char pixel_x, pixel_y;
  unsigned char pixel_magnitude;
  int i;
  int width, height, channels, pixel_count;
  unsigned char *img_x, *img_y, *img_unificated;

  //Ref Image pointer
  unsigned char *img_ref;
  int error_count;
  float error_med;
  
  img_unification_module unification_U1 ("unification_U1");
  
  // Open VCD file
  sc_trace_file *wf = sc_create_vcd_trace_file("unification_U1");
  wf->set_time_unit(1, SC_NS);
  
  // Dump the desired signals
  sc_trace(wf, pixel_x, "pixel_x");
  sc_trace(wf, pixel_y, "pixel_y");
  sc_trace(wf, pixel_magnitude, "pixel_magnitude");
  
  // Load Image
  img_x = stbi_load("../../tools/datagen/src/imgs/car_sobel_x_result.jpg", &width, &height, &channels, 0);
  img_y = stbi_load("../../tools/datagen/src/imgs/car_sobel_y_result.jpg", &width, &height, &channels, 0);
  img_ref = stbi_load("../../tools/datagen/src/imgs/car_sobel_combined_result.jpg", &width, &height, &channels, 0);
  pixel_count = width * height * channels;

  //Allocate memory for output image
  img_unificated = (unsigned char *)(malloc(size_t(pixel_count)));
  if(img_unificated == NULL) {
	  printf("Unable to allocate memory for the output image.\n");
	  exit(1);
  }

  printf("Loaded images X and Y with Width: %0d, Height: %0d, Channels %0d. Total pixel count: %0d", width, height, channels, pixel_count);

  sc_start();
  cout << "@" << sc_time_stamp()<< endl;

  printf("Combined X and Y images...\n");
  
  //Iterate over image
  unification_U1.unificate_img(img_x, img_y, img_unificated, pixel_count, channels);
  printf("Unification finished.\n");

  //Compare with reference image
  error_count = 0;
  error_med = 0;
  for(unsigned char *ref = img_ref, *result = img_unificated; ref < img_ref + pixel_count && result< img_unificated + pixel_count; ref+=channels, result+=channels){
    //printf("Pixel #%0d, Ref Value: %0d, Result Value: %0d\n", int(ref-img_ref), *ref, *result);
    error_count += (*ref != *result);
    error_med += abs(*ref - *result);
  }
  error_med /= pixel_count;
  printf("-----------------------------------\n");
  printf("Comparison Results:\n");
  printf("Error Count: %0d, Error Rate: %0.2f\n", error_count, (100*(error_count+0.0))/pixel_count);
  printf("Mean Error Distance: %0.2f\n", error_med);
  printf("-----------------------------------\n");

  //FIXME: Add time measurement

  cout << "@" << sc_time_stamp() <<" Terminating simulation\n" << endl;
  
  //Write output image
  stbi_write_jpg("./car_unificated.jpg", width, height, channels, img_unificated, 100);
  sc_close_vcd_trace_file(wf); 
  return 0;// Terminate simulation

 }
