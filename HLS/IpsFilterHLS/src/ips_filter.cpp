#ifndef IPS_FILTER_CPP
#define IPS_FILTER_CPP

#include "ips_filter.hpp"

#define IPS_FILTER_KERNEL_SIZE 3

void Filter::filter()
{
  float result;

  while(true) {
    wait();
    result = 0;
    result = input_window_0.read() + input_window_1.read() + input_window_2.read() + input_window_3.read() + input_window_4.read() + input_window_5.read() +input_window_6.read() + input_window_7.read() + input_window_8.read();
    
    #pragma HLS RESOURCE variable=result core=FMul_nodsp
    result = result * (0.1111);

    #ifndef __SYNTHESIS__
      // cout << "Step: " << 1 << " Window Input = " << input_window_0.read() << endl;
      // cout << "Step: " << 2 << " Window Input = " << input_window_1.read() << endl;
      // cout << "Step: " << 3 << " Window Input = " << input_window_2.read() << endl;
      // cout << "Step: " << 4 << " Window Input = " << input_window_3.read() << endl;
      // cout << "Step: " << 5 << " Window Input = " << input_window_4.read() << endl;
      // cout << "Step: " << 6 << " Window Input = " << input_window_5.read() << endl;
      // cout << "Step: " << 7 << " Window Input = " << input_window_6.read() << endl;
      // cout << "Step: " << 8 << " Window Input = " << input_window_7.read() << endl;
      // cout << "Step: " << 9 << " Window Input = " << input_window_8.read() << endl;
      // cout << "Result: " << (int) result << endl;
    #endif //__SYNTHESIS__

    // // Perform the convolution
    // for (int i = 0; i < N; ++i) {
    //   for (int j = 0; j < N; ++j) {
    //     result += input_window[i * N + j];
    // #ifndef __SYNTHESIS__
    //     cout << "Step: " << i * N + j << " Window Input = " << (int) input_window[i * N + j] << (float) result << endl;
    // #endif //__SYNTHESIS__
    //   }
    // }

    output.write( (sc_uint<8>) result);

  }
}


#endif // IPS_FILTER_CPP
