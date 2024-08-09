#include <systemc.h>
#include <cstdlib>

#define IPS_FILTER_KERNEL_SIZE 3
#define DELAY_TIME (IPS_FILTER_KERNEL_SIZE * IPS_FILTER_KERNEL_SIZE * 1) + 4 + 2 + 1

SC_MODULE (testbench) {
	sc_out<sc_uint<8> > img_window[IPS_FILTER_KERNEL_SIZE*IPS_FILTER_KERNEL_SIZE];
	sc_in<sc_uint<8> > mean;

	SC_CTOR (testbench) {
		SC_THREAD(process);
	}

	void process() {

#ifdef TEST_MODE_ONE_WINDOW
		run_one_window();
#endif //TEST_MODE_ONE_WINDOW
		sc_stop();
	}

#ifdef TEST_MODE_ONE_WINDOW
	void run_one_window()
	{
		sc_uint<8> local_window[IPS_FILTER_KERNEL_SIZE * IPS_FILTER_KERNEL_SIZE];
		sc_uint<8>  result;

		#ifdef IPS_DEBUG_EN
		#ifdef TEST_MODE_ONE_WINDOW_RANDOM
		SC_REPORT_INFO("TEST_MODE_ONE_WINDOW_NORMAL", "Running test");
		#elif defined(TEST_MODE_ONE_WINDOW_NORMAL)
		SC_REPORT_INFO("TEST_MODE_ONE_WINDOW_NORMAL", "Running test");
		#else
		SC_REPORT_INFO("TEST_MODE_ONE_WINDOW_DEFAULT", "Running test");
		#endif // TEST_MODE_ONE_WINDOW_RANDOM
		SC_REPORT_INFO("Initialize window", "Window value");
		#endif // IPS_DEBUG_EN

		// Initialize image window
		for (int i = 0; i < IPS_FILTER_KERNEL_SIZE; i++)
		{
			for (int j = 0; j < IPS_FILTER_KERNEL_SIZE; j++)
			{
       			sc_uint<8>  value;
				
#ifdef TEST_MODE_ONE_WINDOW_RANDOM
       			value = (sc_uint<8> ) (rand() % 256);
#elif defined(TEST_MODE_ONE_WINDOW_NORMAL)
       			value = (sc_uint<8> ) (i * IPS_FILTER_KERNEL_SIZE + j);
#else
       			value = (sc_uint<8> ) i;
#endif // TEST_MODE_ONE_WINDOW

				local_window[i * IPS_FILTER_KERNEL_SIZE + j] = value;
#ifdef IPS_DEBUG_EN
				std::cout << "[" << (int) local_window[i * IPS_FILTER_KERNEL_SIZE + j] << "]";
#endif // IPS_DUMP_EN
			}
			
#ifdef IPS_DEBUG_EN
				std::cout << std::endl;
#endif // IPS_DEBUG_EN	
		}

		for (int i = 0; i < IPS_FILTER_KERNEL_SIZE*IPS_FILTER_KERNEL_SIZE; i++) {
#ifdef IPS_DEBUG_EN
			SC_REPORT_INFO("[DEBUG]", "Writing to image window port the value:");
			std::cout << "Iteration: " << i << " local_window = " << local_window[i] << std::endl;
#endif // IPS_DEBUG_EN
			img_window[i].write(local_window[i]); //Write to port
		}

		wait(DELAY_TIME + 10, SC_NS);
		result = mean.read();

#ifdef IPS_DEBUG_EN
		SC_REPORT_INFO("TEST_MODE_ONE_WINDOW", "filtering");
		std::cout << "Result = " << result << std::endl;
#endif // IPS_DEBUG_E

		//delete [] img_window;
	}
	#endif //TEST_MODE_ONE_WINDOW
};
