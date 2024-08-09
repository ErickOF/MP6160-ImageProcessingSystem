#ifndef IPS_FILTER_CPP
#define IPS_FILTER_CPP

#include "ips_filter.hpp"

void Filter::filter()
{
  while (true) {
    wait();
    sc_uint<8> result = 0;
    result = result + input_window_0.read();
    result = result + input_window_1.read();
    result = result + input_window_2.read();
    result = result + input_window_3.read();
    result = result + input_window_4.read();
    result = result + input_window_5.read();
    result = result + input_window_6.read();
    result = result + input_window_7.read();
    result = result + input_window_8.read();

    #ifndef __SYNTHESIS__
      cout << "Step: " << 1 << " Window Input = " << input_window_0 << " Result = " << (float) result/(N*N) << endl;
      cout << "Step: " << 2 << " Window Input = " << input_window_1 << " Result = " << (float) result/(N*N) << endl;
      cout << "Step: " << 3 << " Window Input = " << input_window_2 << " Result = " << (float) result/(N*N) << endl;
      cout << "Step: " << 4 << " Window Input = " << input_window_3 << " Result = " << (float) result/(N*N) << endl;
      cout << "Step: " << 5 << " Window Input = " << input_window_4 << " Result = " << (float) result/(N*N) << endl;
      cout << "Step: " << 6 << " Window Input = " << input_window_5 << " Result = " << (float) result/(N*N) << endl;
      cout << "Step: " << 7 << " Window Input = " << input_window_6 << " Result = " << (float) result/(N*N) << endl;
      cout << "Step: " << 8 << " Window Input = " << input_window_7 << " Result = " << (float) result/(N*N) << endl;
      cout << "Step: " << 9 << " Window Input = " << input_window_8 << " Result = " << (float) result/(N*N) << endl;
    #endif //__SYNTHESIS__

    // // Perform the convolution
    // for (int i = 0; i < N; ++i) {
    //   for (int j = 0; j < N; ++j) {
    //     result += input_window[i * N + j];
    // #ifndef __SYNTHESIS__
    //     cout << "Step: " << i * N + j << " Window Input = " << (int) input_window[i * N + j] << " Result = " << (float) result << endl;
    // #endif //__SYNTHESIS__
    //   }
    // }
    result = result / (N*N);

    output.write(result);
  }
}


#endif // IPS_FILTER_CPP
