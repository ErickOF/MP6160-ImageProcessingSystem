/*!
 \file Simulator.cpp
 \brief Top level simulation entity
 \author Màrius Montón
 \date September 2018
 */

#define SC_INCLUDE_DYNAMIC_PROCESSES

#include "systemc"
#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"
#include "tlm_utils/simple_target_socket.h"

#include <csignal>
#include <unistd.h>
#include <chrono>
#include "CPU.h"
#include "Memory.h"
#include "BusCtrl.h"
#include "Trace.h"
#include "Timer.h"
#include "Debug.h"

//For pre-loading images
#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "include/stb_image_write.h"

//Our modules
#include "address_map.hpp"
#include "ips_filter_tlm.hpp"
#include "sobel_edge_detector_tlm.hpp"
#include "img_receiver_tlm.hpp"
#include "img_transmiter_tlm.hpp"

std::string filename;
bool debug_session = false;

/**
 * @class Simulator
 * This class instantiates all necessary modules, connects its ports and starts
 * the simulation.
 *
 * @brief Top simulation entity
 */
SC_MODULE(Simulator) {
	CPU *cpu;
	Memory *MainMemory;
	BusCtrl *Bus;
	Trace *trace;
	Timer *timer;

	//Our modules
  	ips_filter_tlm *filter_DUT;
	sobel_edge_detector_tlm *sobel_edge_detector_DUT;
	img_receiver_tlm *receiver_DUT;
	img_transmiter_tlm *transmiter_DUT;
	

	SC_CTOR(Simulator) {
		uint32_t start_PC;

		MainMemory = new Memory("Main_Memory", filename);
		start_PC = MainMemory->getPCfromHEX();

		cpu = new CPU("cpu", start_PC, debug_session);

		Bus = new BusCtrl("BusCtrl");
		trace = new Trace("Trace");
		timer = new Timer("Timer");

		//Our modules
    	filter_DUT = new ips_filter_tlm("filter_DUT");
		sobel_edge_detector_DUT = new sobel_edge_detector_tlm("sobel_edge_detector_DUT");
		receiver_DUT = new img_receiver_tlm("receiver_DUT");
		transmiter_DUT = new img_transmiter_tlm("transmiter_DUT");

		cpu->instr_bus.bind(Bus->cpu_instr_socket);
		cpu->mem_intf->data_bus.bind(Bus->cpu_data_socket);

		Bus->memory_socket.bind(MainMemory->socket);
		Bus->trace_socket.bind(trace->socket);
		Bus->timer_socket.bind(timer->socket);

		timer->irq_line.bind(cpu->irq_line_socket);

		//Our modules
		Bus->filter_socket.bind(filter_DUT->socket);
		Bus->sobel_edge_detector_socket.bind(sobel_edge_detector_DUT->socket);

		Bus->receiver_socket.bind(receiver_DUT->socket);
		Bus->transmiter_socket.bind(transmiter_DUT->socket);

		if (debug_session) {
			Debug debug(cpu, MainMemory);
		}
	}

	~Simulator() {
		delete MainMemory;
		delete cpu;
		delete Bus;
		delete trace;
		delete timer;
	}

	void load_img_from_memory()
	{
		int width, height, channels, pixel_count;
		unsigned char *noisy_img, *noisy_img2;

		noisy_img = stbi_load("inputs/car_sobel_x_result.png", &width, &height, &channels, 0);
		pixel_count = width * height * channels;
		printf("Pixel Count %0d, Width: %0d, Height: %0d \n", pixel_count, width, height);
		receiver_DUT->backdoor_write(noisy_img, pixel_count, IMG_INPUT_ADDRESS_LO);

		noisy_img = stbi_load("inputs/car_sobel_y_result.png", &width, &height, &channels, 0);
		pixel_count = width * height * channels;
		receiver_DUT->backdoor_write(noisy_img, pixel_count, IMG_INPUT_ADDRESS_LO + pixel_count);
	}

	void save_img_from_memory()
	{
		int width, height, channels, pixel_count;
		unsigned char *img_ptr;

		channels = 1;
		width = 640;
		height = 452;
		pixel_count = width * height * channels;
		transmiter_DUT->backdoor_read(img_ptr, pixel_count, IMG_OUTPUT_ADDRESS_LO);
		
		stbi_write_png("outputs/car_filtered_image.png", width, height, channels, img_ptr, width*channels);
	}
};

Simulator *top;

void intHandler(int dummy) {
	delete top;
	(void) dummy;
	//sc_stop();
	exit(-1);
}

void process_arguments(int argc, char *argv[]) {

	int c;
	int debug_level;
	Log *log;

	log = Log::getInstance();
	log->setLogLevel(Log::ERROR);

	debug_session = false;

	while ((c = getopt(argc, argv, "DL:f:?")) != -1) {
		switch (c) {
		case 'D':
			debug_session = true;
			break;
		case 'L':
			debug_level = std::atoi(optarg);

			switch (debug_level) {
			case 3:
				log->setLogLevel(Log::INFO);
				break;
			case 2:
				log->setLogLevel(Log::WARNING);
				break;
			case 1:
				log->setLogLevel(Log::DEBUG);
				break;
			case 0:
				log->setLogLevel(Log::ERROR);
				break;
			default:
				log->setLogLevel(Log::INFO);
				break;
			}
			break;
		case 'f':
			filename = std::string(optarg);
			break;
		case '?':
			std::cout << "Call ./RISCV_TLM -D -L <debuglevel> (0..3) filename.hex"
					<< std::endl;
			break;
		default:
			std::cout << "unknown" << std::endl;

		}
	}

	if (filename.empty()) {
		filename = std::string(argv[optind]);
	}

	std::cout << "file: " << filename << '\n';
}

int sc_main(int argc, char *argv[]) {

  Performance *perf = Performance::getInstance();

	/* Capture Ctrl+C and finish the simulation */
	signal(SIGINT, intHandler);

	/* SystemC time resolution set to 1 ns*/
	sc_core::sc_set_time_resolution(1, sc_core::SC_NS);

	/* Parse and process program arguments. -f is mandatory */
	process_arguments(argc, argv);

	// Open VCD file
	sc_trace_file* wf = sc_create_vcd_trace_file("riscv_tb");
	wf->set_time_unit(1, SC_PS);

	top = new Simulator("top");

	sc_trace(wf, top->filter_DUT->img_window[0], "filter_window(0)(0)");
	sc_trace(wf, top->filter_DUT->img_window[1], "filter_window(0)(1)");
	sc_trace(wf, top->filter_DUT->img_window[2], "filter_window(0)(2)");
	sc_trace(wf, top->filter_DUT->img_window[3], "filter_window(1)(0)");
	sc_trace(wf, top->filter_DUT->img_window[4], "filter_window(1)(1)");
	sc_trace(wf, top->filter_DUT->img_window[5], "filter_window(1)(2)");
	sc_trace(wf, top->filter_DUT->img_window[6], "filter_window(2)(0)");
	sc_trace(wf, top->filter_DUT->img_window[7], "filter_window(2)(1)");
	sc_trace(wf, top->filter_DUT->img_window[8], "filter_window(2)(2)");
	sc_trace(wf, top->filter_DUT->img_result, "filter_result");
	sc_trace(wf, top->sobel_edge_detector_DUT->Edge_Detector::data, "sobel_data");
	sc_trace(wf, top->sobel_edge_detector_DUT->Edge_Detector::address, "sobel_address");
	sc_trace(wf, top->sobel_edge_detector_DUT->localWindow[0][0], "sobel_localWindow(0)(0)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localWindow[0][1], "sobel_localWindow(0)(1)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localWindow[0][2], "sobel_localWindow(0)(2)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localWindow[1][0], "sobel_localWindow(1)(0)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localWindow[1][1], "sobel_localWindow(1)(1)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localWindow[1][2], "sobel_localWindow(1)(2)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localWindow[2][0], "sobel_localWindow(2)(0)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localWindow[2][1], "sobel_localWindow(2)(1)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localWindow[2][2], "sobel_localWindow(2)(2)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localMultX[0][0], "sobel_localMultX(0)(0)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localMultX[0][1], "sobel_localMultX(0)(1)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localMultX[0][2], "sobel_localMultX(0)(2)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localMultX[1][0], "sobel_localMultX(1)(0)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localMultX[1][1], "sobel_localMultX(1)(1)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localMultX[1][2], "sobel_localMultX(1)(2)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localMultX[2][0], "sobel_localMultX(2)(0)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localMultX[2][1], "sobel_localMultX(2)(1)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localMultX[2][2], "sobel_localMultX(2)(2)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localMultY[0][0], "sobel_localMultY(0)(0)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localMultY[0][1], "sobel_localMultY(0)(1)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localMultY[0][2], "sobel_localMultY(0)(2)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localMultY[1][0], "sobel_localMultY(1)(0)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localMultY[1][1], "sobel_localMultY(1)(1)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localMultY[1][2], "sobel_localMultY(1)(2)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localMultY[2][0], "sobel_localMultY(2)(0)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localMultY[2][1], "sobel_localMultY(2)(1)");
	sc_trace(wf, top->sobel_edge_detector_DUT->localMultY[2][2], "sobel_localMultY(2)(2)");
	sc_trace(wf, top->sobel_edge_detector_DUT->resultSobelGradientX, "sobel_resultSobelGradientX");
	sc_trace(wf, top->sobel_edge_detector_DUT->resultSobelGradientY, "sobel_resultSobelGradientY");

	top->load_img_from_memory();

	auto start = std::chrono::steady_clock::now();
	sc_core::sc_start();
	auto end = std::chrono::steady_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;
	double instructions = perf->getInstructions() / elapsed_seconds.count();

	std::cout << "Total elapsed time: " << elapsed_seconds.count() << "s" << std::endl;
	std::cout << "Simulated " << int(std::round(instructions)) << " instr/sec" << std::endl;
	
	//Generate output image
	top->save_img_from_memory();
	
	sc_close_vcd_trace_file(wf);
	std::cout << "Press Enter to finish" << std::endl;
	std::cin.ignore();

	// call all destructors, clean exit.
	delete top;

	return 0;
}
