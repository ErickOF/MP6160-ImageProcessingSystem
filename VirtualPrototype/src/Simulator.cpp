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

#include "address_map.hpp"
#include "CPU.h"
#include "Memory.h"
#include "BusCtrl.h"
#include "Trace.h"
#include "Timer.h"
#include "Debug.h"

//For pre-loading images
#define STB_IMAGE_IMPLEMENTATION
#include "inc/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "inc/stb_image_write.h"

//Our modules
#include "ips_filter_tlm.hpp"
#include "sobel_edge_detector_tlm.cpp"

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
		

		cpu->instr_bus.bind(Bus->cpu_instr_socket);
		cpu->mem_intf->data_bus.bind(Bus->cpu_data_socket);

		Bus->memory_socket.bind(MainMemory->socket);
		Bus->trace_socket.bind(trace->socket);
		Bus->timer_socket.bind(timer->socket);

		timer->irq_line.bind(cpu->irq_line_socket);

		//Our modules
		Bus->filter_socket.bind(filter_DUT->socket); 

		if (debug_session) {
			Debug debug(cpu, MainMemory);
		}

		pre_load_memory();
	}

	~Simulator() {
		delete MainMemory;
		delete cpu;
		delete Bus;
		delete trace;
		delete timer;
	}

	void pre_load_memory()
	{
		int width, height, channels, pixel_count;
		unsigned char *noisy_img, *noisy_img2;

		noisy_img = stbi_load("inputs/car_noisy_image.png", &width, &height, &channels, 0);
		pixel_count = width * height * channels;
		printf("Pixel Count %0d, Width: %0d, Height: %0d \n", pixel_count, width, height);
		
		noisy_img2 = new unsigned char [pixel_count];

		MainMemory->backdoor_write(noisy_img, pixel_count, INPUT_IMG_MEMORY_LO);
		//stbi_write_png("outputs/car_filtered_image.png", width, height, channels, noisy_img, width*channels);
	}

	void save_img_from_memory()
	{
		int width, height, channels, pixel_count;
		unsigned char *img_ptr;

		channels = 1;
		width = 640;
		height = 452;
		pixel_count = width * height * channels;
		MainMemory->backdoor_read(img_ptr, pixel_count, OUTPUT_IMG_MEMORY_LO);
		
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

	top = new Simulator("top");

	auto start = std::chrono::steady_clock::now();
	sc_core::sc_start();
	auto end = std::chrono::steady_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;
	double instructions = perf->getInstructions() / elapsed_seconds.count();

	std::cout << "Total elapsed time: " << elapsed_seconds.count() << "s" << std::endl;
	std::cout << "Simulated " << int(std::round(instructions)) << " instr/sec" << std::endl;
	
	//Generate output image
	top->save_img_from_memory();
	
	std::cout << "Press Enter to finish" << std::endl;
	std::cin.ignore();

	// call all destructors, clean exit.
	delete top;

	return 0;
}
