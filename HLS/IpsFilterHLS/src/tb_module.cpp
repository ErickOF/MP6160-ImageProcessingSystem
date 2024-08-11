#define int64  systemc_int64
#define uint64 systemc_uint64
#include <systemc.h>
#include <cstdlib>
#undef int64
#undef uint64

#define int64  opencv_int64
#define uint64 opencv_uint64
#include <opencv2/opencv.hpp>
#undef int64
#undef uint64

#define IPS_FILTER_KERNEL_SIZE 3
#define DELAY_TIME (IPS_FILTER_KERNEL_SIZE * IPS_FILTER_KERNEL_SIZE * 1) + 4 + 2 + 1
#define IPS_IMG_PATH_TB "../../../inputs/car_noisy_image.jpg"

SC_MODULE (testbench) {
	sc_out<sc_uint<8> > img_window[IPS_FILTER_KERNEL_SIZE*IPS_FILTER_KERNEL_SIZE];
	sc_in<sc_uint<8> > mean;

	SC_CTOR (testbench) {
		SC_THREAD(process);
	}

	void process() {

#ifdef TEST_MODE_ONE_WINDOW
		run_one_window();
#elif defined(TEST_MODE_IMAGE)
		run_image();
#endif //TEST_MODE_ONE_WINDOW

		sc_stop();
	}

#ifdef TEST_MODE_ONE_WINDOW
	void run_one_window()
	{
		sc_uint<8> local_window[IPS_FILTER_KERNEL_SIZE * IPS_FILTER_KERNEL_SIZE];
		sc_uint<8> result;

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
       			sc_uint<8> value;
				
#ifdef TEST_MODE_ONE_WINDOW_RANDOM
       			value = (sc_uint<8>) (rand() % 256);
#elif defined(TEST_MODE_ONE_WINDOW_NORMAL)
       			value = (sc_uint<8>) (i * IPS_FILTER_KERNEL_SIZE + j);
#else
       			value = (sc_uint<8>) i;
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
			std::cout << "Iteration: " << i << " local_window = " << (int) local_window[i] << std::endl;
#endif // IPS_DEBUG_EN
			img_window[i].write(local_window[i]); //Write to port
		}

		wait(DELAY_TIME + 10, SC_NS);
		result = mean.read();

#ifdef IPS_DEBUG_EN
		SC_REPORT_INFO("TEST_MODE_ONE_WINDOW", "filtering");
		std::cout << "Result = " << (int) result << std::endl;
#endif // IPS_DEBUG_E

		//delete [] img_window;
	}
	#endif //TEST_MODE_ONE_WINDOW

#ifdef TEST_MODE_IMAGE
	void run_image()
	{
#ifdef IPS_DEBUG_EN
 		SC_REPORT_INFO("TEST_MODE_IMAGE", "Running test");
#endif // IPS_DEBUG_EN

		const std::string img_path = IPS_IMG_PATH_TB;
		cv::Mat read_image = cv::imread(img_path, cv::IMREAD_GRAYSCALE);
		cv::Mat image;
		read_image.convertTo(image, CV_8U);

		cv::Mat o_img(image.size(), image.type());

		// Check if the image is loaded successfully
		if (image.empty())
		{
			std::cerr << "Error: Could not open or find the image!" << std::endl;
			exit(EXIT_FAILURE);
		}
		else 
		{
			std::cout << "Image info: ";
			std::cout << "rows = " << image.rows;
			std::cout << " cols = " << image.cols;
			std::cout << " channels = " << image.channels() << std::endl;
		}

		// image.rows = 100;
		// image.cols = 100;
		sc_uint<8> local_window[IPS_FILTER_KERNEL_SIZE * IPS_FILTER_KERNEL_SIZE];
		sc_uint<8> result;

		// Create each windown and filter
		for (int y = 0; y < image.rows - IPS_FILTER_KERNEL_SIZE; ++y)
		{
			for (int x = 0; x < image.cols - IPS_FILTER_KERNEL_SIZE; ++x)
			{
#ifdef IPS_DEBUG_EN
     			// SC_REPORT_INFO("TEST_MODE_IMAGE", "filtering");
#endif // IPS_DEBUG_EN

				// Define the Window
				extract_window(y, x, image, &local_window[0], image.cols, image.rows);

				for (int i = 0; i < IPS_FILTER_KERNEL_SIZE*IPS_FILTER_KERNEL_SIZE; ++i)
     			{
					//local_window[i* IPS_FILTER_KERNEL_SIZE + j] = sub_img.data[i* IPS_FILTER_KERNEL_SIZE + j];
					img_window[i].write(local_window[i]);
#ifdef IPS_DEBUG_EN
					std::cout << "[" << (int) local_window[i] << "]";
#endif // IPS_DEBUG_EN
				}
#ifdef IPS_DEBUG_EN
       			std::cout << std::endl;
#endif // IPS_DEBUG_EN
				wait(DELAY_TIME + 10, SC_NS);
 				result = mean.read();
				// cout << "Read Result: " << (int) result << endl;
				// result = (sc_uint<8>) local_window[0];
     			o_img.data[y*image.cols + x]= result;
				//*o_img.ptr(y, x) = result;
#ifdef IPS_DEBUG_EN
				// SC_REPORT_INFO("TEST_MODE_IMAGE", "filtering");
				std::cout << "Iteration: " << (int) y*image.cols + x << " Original Pixel: " << (int) image.data[y* image.rows + x] << " Result = " << (int) result << std::endl;
#endif // IPS_DEBUG_E
			}
		}
		// Convert the floating-point image to 8-bit unsigned integer for saving
		cv::Mat final_img;
		o_img.convertTo(final_img, CV_8U, 1.0);

		// Save the final image
		std::string output_img_path = "filtered_image.png";
		cv::imwrite(output_img_path, final_img);
	}
#endif //TEST_MODE_IMAGE

	void extract_window(int x, int y, cv::Mat image, sc_uint<8>*window, int img_width , int img_height)
	{
		int offset;

		//Populate with zeroes
		for (int i = 0; i < 9; i++){
			window[i] = 0;
		}

		if ((x == 0) && (y == 0)){
			//First Row
			//Second Row
			window[4] =*image.ptr(x + 0, y);
			window[5] =*image.ptr(x + 0, y+1);
			//Third Row
			
			window[7] =*image.ptr(x + 1, y + 0);
			window[8] =*image.ptr(x + 1, y + 1);
		}
		else if ((x + 0 == 0) && (y == img_height - 1)){
			//First Row
			//Second Row
			
			window[3] =*image.ptr(x + 0, y + 0);
			window[4] =*image.ptr(x + 0, y + 1);
			//Third Row
			
			window[6] =*image.ptr(x + 1, y + 0);
			window[7] =*image.ptr(x + 1, y + 1);
		}
		else if ((x + 0 == 0) ){
			//First Row
			//Second Row
			
			window[3] =*image.ptr(x + 0, y + 0);
			window[4] =*image.ptr(x + 0, y + 1);
			window[5] =*image.ptr(x + 0, y + 2);
			//Third Row
			
			window[6] =*image.ptr(x + 1, y + 0);
			window[7] =*image.ptr(x + 1, y + 1);
			window[8] =*image.ptr(x + 1, y + 2);
		}
		else if ((x + 0 == img_width-1) && (y == 0)){
			//First Row
			
			window[1] =*image.ptr(x - 1, y + 0);
			window[2] =*image.ptr(x - 1, y + 1);
			//Second Row
			
			window[4] =*image.ptr(x + 0, y + 0);
			window[5] =*image.ptr(x + 0, y + 1);
			//Third Row
		}
		else if ((x + 0 == img_width-1) && (y == img_height-1)){
			//First Row
			
			window[0] =*image.ptr(x - 1, y + 0);
			window[1] =*image.ptr(x - 1, y + 1);
			//Second Row
			
			window[3] =*image.ptr(x + 0, y + 0);
			window[4] =*image.ptr(x + 0, y + 1);
			//Third Row
		}
		else if ((x + 0 == img_width-1)){
			//First Row
			
			window[0] =*image.ptr(x - 1, y + 0);
			window[1] =*image.ptr(x - 1, y + 1);
			window[2] =*image.ptr(x - 1, y + 2);
			//Second Row
			
			window[3] =*image.ptr(x + 0, y + 0);
			window[4] =*image.ptr(x + 0, y + 1);
			window[5] =*image.ptr(x + 0, y + 2);
			//Third Row
		}
		else if ((y == 0)){
			//First Row
			
			window[1] =*image.ptr(x - 1, y + 0);
			window[2] =*image.ptr(x - 1, y + 1);
			//Second Row
			
			window[4] =*image.ptr(x + 0, y + 0);
			window[5] =*image.ptr(x + 0, y + 1);
			//Third Row
			
			window[7] =*image.ptr(x + 1, y + 0);
			window[8] =*image.ptr(x + 1, y + 1);
		}
		else if (y == img_height-1){
			//First Row
			
			window[0] =*image.ptr(x - 1, y + 0);
			window[1] =*image.ptr(x - 1, y + 1);
			//Second Row
			
			window[3] =*image.ptr(x + 0, y + 0);
			window[4] =*image.ptr(x + 0, y + 1);
			//Third Row
			
			window[6] =*image.ptr(x + 1, y + 0);
			window[7] =*image.ptr(x + 1, y + 1);
		}
		else
		{
			//First Row
			
			window[0] =*image.ptr(x - 1, y + 0);
			window[1] =*image.ptr(x - 1, y + 1);
			window[2] =*image.ptr(x - 1, y + 2);
			//Second Row
			
			window[3] =*image.ptr(x + 0, y + 0);
			window[4] =*image.ptr(x + 0, y + 1);
			window[5] =*image.ptr(x + 0, y + 2);
			//Third Row
			
			window[6] =*image.ptr(x + 1, y + 0);
			window[7] =*image.ptr(x + 1, y + 1);
			window[8] =*image.ptr(x + 1, y + 2);
		}
	}
};