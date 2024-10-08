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

#include "systemc-ams.h"

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
#include "important_defines.hpp"
#include "ips_filter_tlm.hpp"
#include "sobel_edge_detector_tlm.hpp"
#include "adc.hpp"
#include "vga_tlm.hpp"
#include "seq_item_vga.hpp"
#include "packetGenerator_tlm.hpp"
#include "ethernetEncoder.h"
#include "ethernetDecoder_tlm.hpp"
#include "img_saver_tlm.hpp"

struct Ethernet_AMS_Signals {
	sca_tdf::sca_signal<bool> valid;
	sca_tdf::sca_signal<double> mlt3_out_signal;

	sca_tdf::sca_signal<bool> data_out_valid;
	sca_tdf::sca_signal<sc_dt::sc_bv<4>> data_out;

	sca_tdf::sca_signal<bool> tmp_data_out_valid;

	sca_tdf::sca_signal<bool> n2_data_out_valid;
	sca_tdf::sca_signal<sc_dt::sc_bv<4>> n2_data_out;
	sca_tdf::sca_signal<sc_dt::sc_bv<16>> n2_data_valid;
	sca_tdf::sca_signal<bool> n1_data_out_valid;
	sca_tdf::sca_signal<sc_dt::sc_bv<4>> n1_data_out;
	sca_tdf::sca_signal<sc_dt::sc_bv<16>> n1_data_valid;

	sca_tdf::sca_signal<sc_dt::sc_bv<64>> data_in;
	sca_tdf::sca_signal<sc_dt::sc_bv<16>> data_in_valid;

	sca_tdf::sca_signal<sc_dt::sc_bv<64>> data_to_send;
	sca_tdf::sca_signal<sc_dt::sc_bv<16>> data_valid_to_send;

	sca_tdf::sca_signal<sc_dt::sc_int<4>> n1_sigBitCount;
	sca_tdf::sca_signal<sc_dt::sc_int<4>> n2_sigBitCount;
	sca_tdf::sca_signal<sc_dt::sc_int<4>> sigBitCount;

	sca_tdf::sca_signal<sc_dt::sc_int<32>> remaining_bytes_to_send;

	sca_tdf::sca_signal<sc_dt::sc_bv<4>> data_out_signal;
};

struct vga_ams_signals_t
{
  // -- Inputs of VGA
  sc_signal<sc_uint<IPS_BITS>> s_tx_red;
  sc_signal<sc_uint<IPS_BITS>> s_tx_green;
  sc_signal<sc_uint<IPS_BITS>> s_tx_blue;
  // -- Outputs of VGA
  sc_signal<bool> s_hsync;
  sc_signal<bool> s_vsync;
  sc_signal<unsigned int> s_h_count;
  sc_signal<unsigned int> s_v_count;
  // -- Outputs of DAC
  sca_tdf::sca_signal<double> s_ana_red;
  sca_tdf::sca_signal<double> s_ana_green;
  sca_tdf::sca_signal<double> s_ana_blue;
};

std::string filename;
bool debug_session = false;
int source_image = -1;
int output_image = 0;
bool use_prints = false;

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
	adc<IPS_BITS, IPS_VOLTAGE_MIN, IPS_VOLTAGE_MAX, VUnit::mv> *adc_red_DUT;
	adc<IPS_BITS, IPS_VOLTAGE_MIN, IPS_VOLTAGE_MAX, VUnit::mv> *adc_green_DUT;
	adc<IPS_BITS, IPS_VOLTAGE_MIN, IPS_VOLTAGE_MAX, VUnit::mv> *adc_blue_DUT;
	vga_tlm *vga_DUT;
	seq_item_vga<
		IPS_BITS,
		IPS_H_ACTIVE, IPS_H_FP, IPS_H_SYNC_PULSE, IPS_H_BP,
		IPS_V_ACTIVE, IPS_V_FP, IPS_V_SYNC_PULSE, IPS_V_BP,
		IPS_VOLTAGE_MIN, IPS_VOLTAGE_MAX, VUnit::mv> *ips_seq_item_vga;
	packetGenerator_tlm *packetGenerator_DUT;
	ethernetEncoder *ethernetEncoder_DUT;
	ethernetDecoder_tlm *ethernetDecoder_DUT;
	img_saver_tlm *img_saver_DUT;

	Ethernet_AMS_Signals ethernetSignals;
	vga_ams_signals_t vga_signals;
	
	sca_core::sca_time sample_time;

	SC_CTOR(Simulator) : sample_time(10, SC_NS) {
		uint32_t start_PC;

		MainMemory = new Memory("Main_Memory", filename);
		start_PC = MainMemory->getPCfromHEX();

		cpu = new CPU("cpu", start_PC, debug_session);

		Bus = new BusCtrl("BusCtrl", use_prints);
		trace = new Trace("Trace");
		timer = new Timer("Timer");

		//Our modules
    	filter_DUT = new ips_filter_tlm("filter_DUT", use_prints);
		sobel_edge_detector_DUT = new sobel_edge_detector_tlm("sobel_edge_detector_DUT", use_prints);
		packetGenerator_DUT = new packetGenerator_tlm("packetGenerator_DUT", use_prints, sample_time);
		ethernetEncoder_DUT = new ethernetEncoder("ethernetEncoder_DUT", sample_time);
		ethernetDecoder_DUT = new ethernetDecoder_tlm("ethernetDecoder_DUT", sample_time);
		img_saver_DUT = new img_saver_tlm("img_saver_DUT");

		// Connecting the signals to ethernet modules
		packetGenerator_DUT->data_out_valid(ethernetSignals.data_out_valid);
		packetGenerator_DUT->data_out(ethernetSignals.data_out);

		packetGenerator_DUT->tmp_data_out_valid_(ethernetSignals.tmp_data_out_valid);

		packetGenerator_DUT->n2_data_out_valid_(ethernetSignals.n2_data_out_valid);
		packetGenerator_DUT->n2_data_out_(ethernetSignals.n2_data_out);
		packetGenerator_DUT->n2_data_valid_(ethernetSignals.n2_data_valid);

		packetGenerator_DUT->n1_data_out_valid_(ethernetSignals.n1_data_out_valid);
		packetGenerator_DUT->n1_data_out_(ethernetSignals.n1_data_out);
		packetGenerator_DUT->n1_data_valid_(ethernetSignals.n1_data_valid);

		packetGenerator_DUT->data_in_(ethernetSignals.data_in);
		packetGenerator_DUT->data_in_valid_(ethernetSignals.data_in_valid);

		packetGenerator_DUT->data_to_send_(ethernetSignals.data_to_send);
		packetGenerator_DUT->data_valid_to_send_(ethernetSignals.data_valid_to_send);

		packetGenerator_DUT->n1_sigBitCount_(ethernetSignals.n1_sigBitCount);
		packetGenerator_DUT->n2_sigBitCount_(ethernetSignals.n2_sigBitCount);
		packetGenerator_DUT->sigBitCount(ethernetSignals.sigBitCount);

		packetGenerator_DUT->remaining_bytes_to_send(ethernetSignals.remaining_bytes_to_send);

		ethernetEncoder_DUT->data_in(ethernetSignals.data_out);
		ethernetEncoder_DUT->mlt3_out(ethernetSignals.mlt3_out_signal);
		ethernetEncoder_DUT->valid(ethernetSignals.data_out_valid);

		ethernetDecoder_DUT->mlt3_in(ethernetSignals.mlt3_out_signal);
		ethernetDecoder_DUT->data_out(ethernetSignals.data_out_signal);

		ips_seq_item_vga = new seq_item_vga<IPS_BITS,
				IPS_H_ACTIVE, IPS_H_FP, IPS_H_SYNC_PULSE, IPS_H_BP,
				IPS_V_ACTIVE, IPS_V_FP, IPS_V_SYNC_PULSE, IPS_V_BP,
				IPS_VOLTAGE_MIN, IPS_VOLTAGE_MAX, VUnit::mv>("ips_seq_item_vga");
		ips_seq_item_vga->hcount(vga_signals.s_h_count);
		ips_seq_item_vga->vcount(vga_signals.s_v_count);
		ips_seq_item_vga->o_red(vga_signals.s_ana_red);
		ips_seq_item_vga->o_green(vga_signals.s_ana_green);
		ips_seq_item_vga->o_blue(vga_signals.s_ana_blue);

		adc_red_DUT = new adc<IPS_BITS, IPS_VOLTAGE_MIN, IPS_VOLTAGE_MAX, VUnit::mv>("adc_red_DUT");
		adc_red_DUT->in(vga_signals.s_ana_red);
		adc_red_DUT->out(vga_signals.s_tx_red);

		adc_green_DUT = new adc<IPS_BITS, IPS_VOLTAGE_MIN, IPS_VOLTAGE_MAX, VUnit::mv>("adc_green_DUT");
		adc_green_DUT->in(vga_signals.s_ana_green);
		adc_green_DUT->out(vga_signals.s_tx_green);

		adc_blue_DUT = new adc<IPS_BITS, IPS_VOLTAGE_MIN, IPS_VOLTAGE_MAX, VUnit::mv>("adc_blue_DUT");
		adc_blue_DUT->in(vga_signals.s_ana_blue);
		adc_blue_DUT->out(vga_signals.s_tx_blue);

		vga_DUT = new vga_tlm("vga_DUT", use_prints);
		vga_DUT->red(vga_signals.s_tx_red);
		vga_DUT->green(vga_signals.s_tx_green);
		vga_DUT->blue(vga_signals.s_tx_blue);
		vga_DUT->o_h_count(vga_signals.s_h_count);
		vga_DUT->o_v_count(vga_signals.s_v_count);
		vga_DUT->o_hsync(vga_signals.s_hsync);
		vga_DUT->o_vsync(vga_signals.s_vsync);

		cpu->instr_bus.bind(Bus->cpu_instr_socket);
		cpu->mem_intf->data_bus.bind(Bus->cpu_data_socket);

		Bus->memory_socket.bind(MainMemory->socket);
		Bus->trace_socket.bind(trace->socket);
		Bus->timer_socket.bind(timer->socket);

		timer->irq_line.bind(cpu->irq_line_socket);

		//Our modules
		Bus->filter_socket.bind(filter_DUT->socket);
		Bus->sobel_edge_detector_socket.bind(sobel_edge_detector_DUT->socket);

		Bus->receiver_socket.bind(vga_DUT->socket);
		Bus->transmiter_socket.bind(packetGenerator_DUT->socket);
		Bus->image_saver_socket.bind(img_saver_DUT->socket);

		img_saver_DUT->img_input_ptr = vga_DUT->return_mem_ptr();
		img_saver_DUT->img_inprocess_a_ptr = MainMemory->get_pointer_to_mem_region(IMG_INPROCESS_A_ADDRESS_LO);
		img_saver_DUT->img_inprocess_b_ptr = MainMemory->get_pointer_to_mem_region(IMG_INPROCESS_B_ADDRESS_LO);
		img_saver_DUT->img_inprocess_c_ptr = MainMemory->get_pointer_to_mem_region(IMG_INPROCESS_C_ADDRESS_LO);
		img_saver_DUT->img_inprocess_d_ptr = MainMemory->get_pointer_to_mem_region(IMG_INPROCESS_D_ADDRESS_LO);
		img_saver_DUT->img_output_ptr = packetGenerator_DUT->tmp_data;
		img_saver_DUT->img_output_dec_ptr = ethernetDecoder_DUT->data;

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

	void load_img_from_memory(int source_image_)
	{
		switch (source_image_)
		{
		case 0:
			load_img_from_memory_from_fname("../../tools/datagen/src/imgs/car_rgb_noisy_image.jpg", IMG_INPUT_ADDRESS_LO, sizeof(char), false, true);
			break;
		case 1:
			load_img_from_memory_from_fname("../router/grayImagePrevMem.jpg", IMG_INPROCESS_A_ADDRESS_LO, sizeof(char), true, false);
			break;
		case 2:
			load_img_from_memory_from_fname("../router/filteredImagePrevMem.jpg", IMG_INPROCESS_D_ADDRESS_LO, sizeof(char), true, false);
			break;
		case 3:
			load_img_from_memory_from_fname("../router/detectedImagePrevMemX.jpg", IMG_INPROCESS_B_ADDRESS_LO, sizeof(short int), true, false);
			load_img_from_memory_from_fname("../router/detectedImagePrevMemY.jpg", IMG_INPROCESS_C_ADDRESS_LO, sizeof(short int), true, false);
			break;
		case 4:
			load_img_from_memory_from_fname("../router/detectedImagePrevMem.jpg", IMG_INPROCESS_A_ADDRESS_LO, sizeof(char), true, false);
			break;
		case 5:
			load_img_from_memory_from_fname("../router/detectedImagePrevMem.jpg", IMG_OUTPUT_ADDRESS_LO, sizeof(char), false, false);
			break;
		default:
			printf("WARNING: The image source option I <image source> (0...4) provided %0d didn't match any expected source, the original image will be loaded\n", source_image_);
			load_img_from_memory_from_fname("../../tools/datagen/src/imgs/car_rgb_noisy_image.jpg", IMG_INPUT_ADDRESS_LO, sizeof(char), false, true);
			break;
		}
	}

	void load_img_from_memory_from_fname(const char *filename, sc_dt::uint64 address, int pixel_byte_depth, bool use_memory, bool use_receiver)
	{
		int width, height, channels, pixel_count;
		unsigned char *img_pointer;

		img_pointer = stbi_load(filename, &width, &height, &channels, 0);

		if (img_pointer == NULL)
		{
			printf("WARNING: The image source was not found and will not be loaded\n");
			return;
		}

		pixel_count = width * height * channels;
		printf("Pixel Count %0d, Width: %0d, Height: %0d, Channels: %0d\n", pixel_count, width, height, channels);

		if (pixel_byte_depth == 1)
		{
			unsigned char pixel_value[3];
			unsigned char *pixel_value_ptr;
			pixel_value[0] = 0;
			pixel_value[1] = 0;
			pixel_value[2] = 0;

			for (int i = 0; i < height; i++)
			{
				pixel_value_ptr = img_pointer + (i * channels * width);
				if (use_memory == true)
				{
					MainMemory->backdoor_write(pixel_value_ptr, width * channels, address + (i * channels * IMAG_COLS));
				}
				else
				{
					if (use_receiver == true)
					{
						vga_DUT->backdoor_write(pixel_value_ptr, width * channels, address + (i * channels * IMAG_COLS));
					}
					else
					{
						packetGenerator_DUT->backdoor_write(pixel_value_ptr, width * channels, address + (i * channels * IMAG_COLS));
					}
				}
			}

			pixel_value_ptr = pixel_value;

			// Fill remaining rows with black pixels
			for (int i = height; i < IMAG_ROWS; i++)
			{
				for (int j = 0; j < IMAG_COLS; j++)
				{
					if (use_memory == true)
					{
						MainMemory->backdoor_write(pixel_value_ptr, channels, address + ((i * channels * IMAG_COLS) + (j * channels)));
					}
					else
					{
						if (use_receiver == true)
						{
							vga_DUT->backdoor_write(pixel_value_ptr, channels, address + ((i * channels * IMAG_COLS) + (j * channels)));
						}
						else
						{
							packetGenerator_DUT->backdoor_write(pixel_value_ptr, channels, address + ((i * channels * IMAG_COLS) + (j * channels)));
						}
					}
				}
			}

			// Fill remaining columns with black pixels
			for (int j = width; j < IMAG_COLS; j++)
			{
				for (int i = 0; i < IMAG_ROWS; i++)
				{
					if (use_memory == true)
					{
						MainMemory->backdoor_write(pixel_value_ptr, channels, address + ((i * channels * IMAG_COLS) + (j * channels)));
					}
					else
					{
						if (use_receiver == true)
						{
							vga_DUT->backdoor_write(pixel_value_ptr, channels, address + ((i * channels * IMAG_COLS) + (j * channels)));
						}
						else
						{
							packetGenerator_DUT->backdoor_write(pixel_value_ptr, channels, address + ((i * channels * IMAG_COLS) + (j * channels)));
						}
					}
				}
			}
		}
		else
		{
			unsigned char pixel_value = 0;
			unsigned char *pixel_value_ptr;

			for (int i = 0; i < height; i++)
			{
				for (int j = 0; j < width; j++)
				{
					pixel_value_ptr = img_pointer + ((i * width) + j);
					MainMemory->backdoor_write(pixel_value_ptr, 1, address + ((i * IMAG_COLS * pixel_byte_depth) + (j * pixel_byte_depth)));
				}
			}

			pixel_value_ptr = &pixel_value;

			// Fill remaining rows with black pixels
			for (int i = height; i < IMAG_ROWS; i++)
			{
				for (int j = 0; j < IMAG_COLS; j++)
				{
					MainMemory->backdoor_write(pixel_value_ptr, 1, address + ((i * IMAG_COLS * pixel_byte_depth) + (j * pixel_byte_depth)));
				}
			}

			// Fill remaining columns with black pixels
			for (int j = width; j < IMAG_COLS; j++)
			{
				for (int i = 0; i < IMAG_ROWS; i++)
				{
					MainMemory->backdoor_write(pixel_value_ptr, 1, address + ((i * IMAG_COLS * pixel_byte_depth) + (j * pixel_byte_depth)));
				}
			}
		}
	}

	void save_img_from_memory(int output_image_)
	{
		int channels, pixel_count;
		unsigned char *img_ptr;

		if (output_image_ == 0)
		{
			channels = 3;
			pixel_count = IMAG_COLS * IMAG_ROWS * channels;
			vga_DUT->backdoor_read(img_ptr, pixel_count, IMG_INPUT_ADDRESS_LO);

			stbi_write_png("output_image_0.png", IMAG_COLS, IMAG_ROWS, channels, img_ptr, IMAG_COLS * channels);
		}
		else
		{
			channels = 1;
			pixel_count = IMAG_COLS * IMAG_ROWS;
			switch (output_image_) {
			case 1:
				MainMemory->backdoor_read(img_ptr, pixel_count, IMG_INPROCESS_A_ADDRESS_LO);
				stbi_write_png("output_image_0.png", IMAG_COLS, IMAG_ROWS, channels, img_ptr, IMAG_COLS * channels);
				break;
			case 2:
				MainMemory->backdoor_read(img_ptr, pixel_count, IMG_INPROCESS_D_ADDRESS_LO);
				stbi_write_png("output_image_0.png", IMAG_COLS, IMAG_ROWS, channels, img_ptr, IMAG_COLS * channels);
				break;
			case 3:
				unsigned char *read_ptr;
				short int *value_ptr;

				img_ptr = new unsigned char[IMAG_COLS * IMAG_ROWS];

				MainMemory->backdoor_read(read_ptr, pixel_count * sizeof(short int), IMG_INPROCESS_B_ADDRESS_LO);
				value_ptr = (short int *)read_ptr;

				for (int i = 0; i < IMAG_ROWS; i++)
				{
					for (int j = 0; j < IMAG_COLS; j++)
					{
						if ((*(value_ptr + ((i * IMAG_COLS) + j)) > 255) || ((*(value_ptr + ((i * IMAG_COLS) + j)) < -255)))
						{
							*(img_ptr + ((i * IMAG_COLS) + j)) = 255;
						}
						else if (*(value_ptr + ((i * IMAG_COLS) + j)) < 0)
						{
							*(img_ptr + ((i * IMAG_COLS) + j)) = (unsigned char)(-*(value_ptr + ((i * IMAG_COLS) + j)));
						}
						else
						{
							*(img_ptr + ((i * IMAG_COLS) + j)) = (unsigned char)(*(value_ptr + ((i * IMAG_COLS) + j)));
						}
					}
				}
				delete[] read_ptr;

				stbi_write_png("output_image_0.png", IMAG_COLS, IMAG_ROWS, channels, img_ptr, IMAG_COLS * channels);

				MainMemory->backdoor_read(read_ptr, pixel_count * sizeof(short int), IMG_INPROCESS_C_ADDRESS_LO);
				value_ptr = (short int *)read_ptr;

				for (int i = 0; i < IMAG_ROWS; i++)
				{
					for (int j = 0; j < IMAG_COLS; j++)
					{
						if ((*(value_ptr + ((i * IMAG_COLS) + j)) > 255) || ((*(value_ptr + ((i * IMAG_COLS) + j)) < -255)))
						{
							*(img_ptr + ((i * IMAG_COLS) + j)) = 255;
						}
						else if (*(value_ptr + ((i * IMAG_COLS) + j)) < 0)
						{
							*(img_ptr + ((i * IMAG_COLS) + j)) = (unsigned char)(-*(value_ptr + ((i * IMAG_COLS) + j)));
						}
						else
						{
							*(img_ptr + ((i * IMAG_COLS) + j)) = (unsigned char)(*(value_ptr + ((i * IMAG_COLS) + j)));
						}
					}
				}
				delete[] read_ptr;

				stbi_write_png("output_image_1.png", IMAG_COLS, IMAG_ROWS, channels, img_ptr, IMAG_COLS * channels);
				break;
			case 4:
				MainMemory->backdoor_read(img_ptr, pixel_count, IMG_INPROCESS_A_ADDRESS_LO);
				stbi_write_png("output_image_0.png", IMAG_COLS, IMAG_ROWS, channels, img_ptr, IMAG_COLS * channels);
				break;
			case 5:
				packetGenerator_DUT->backdoor_read(img_ptr, pixel_count, IMG_OUTPUT_ADDRESS_LO);
				stbi_write_png("output_image_0.png", IMAG_COLS, IMAG_ROWS, channels, img_ptr, IMAG_COLS * channels);
				break;
			default:
				printf("WARNING: The image output option O <image output> (0...4) provided %0d didn't match any expected output, the original image be saved\n", output_image_);
				channels = 3;
				pixel_count = IMAG_COLS * IMAG_ROWS * channels;
				vga_DUT->backdoor_read(img_ptr, pixel_count, IMG_INPROCESS_A_ADDRESS_LO);
				stbi_write_png("output_image_0.png", IMAG_COLS, IMAG_ROWS, channels, img_ptr, IMAG_COLS * channels);
				break;
			}
		}
		delete[] img_ptr;
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

	if ((c = getopt(argc, argv, "I:")) == -1)
	{
		printf("WARNING: The image source option I <image source> (0...4) was not provided, the original image will be loaded\n");
	}
	if ((c = getopt(argc, argv, "O:")) == -1)
	{
		printf("WARNING: The image output option O <image output> (0...4) was not provided, the original image will be saved\n");
	}

	while ((c = getopt(argc, argv, "DL:I:O:Pf:?")) != -1) {
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
		case 'I':
			source_image = std::atoi(optarg);
			break;
		case 'O':
			output_image = std::atoi(optarg);
			break;
		case 'P':
			use_prints = true;
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
	sca_util::sca_trace_file* wf_ams_ethernet = sca_util::sca_create_vcd_trace_file("riscv_ams_ethernet_tb");
	sca_util::sca_trace_file* wf_ams_vga = sca_util::sca_create_vcd_trace_file("riscv_ams_vga_tb");
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

	sca_util::sca_trace(wf_ams_ethernet, top->ethernetSignals.mlt3_out_signal, "mlt3_out");
	sca_util::sca_trace(wf_ams_ethernet, top->ethernetSignals.data_out_valid, "data_out_valid");
	sca_util::sca_trace(wf_ams_ethernet, top->ethernetSignals.data_out, "data_out");
	sca_util::sca_trace(wf_ams_ethernet, top->ethernetSignals.data_out_signal, "data_out_decoded");
	// sca_util::sca_trace(wf_ams_ethernet, top->ethernetSignals.data_in, "pkt_gen_data_in");
	// sca_util::sca_trace(wf_ams_ethernet, top->ethernetSignals.data_in_valid, "pkt_gen_data_in_valid");
	// sca_util::sca_trace(wf_ams_ethernet, top->ethernetSignals.n2_data_valid, "pkt_gen_n2_data_valid");
	// sca_util::sca_trace(wf_ams_ethernet, top->ethernetSignals.n2_data_out_valid, "pkt_gen_n2_data_out_valid");
	// sca_util::sca_trace(wf_ams_ethernet, top->ethernetSignals.n2_data_out, "pkt_gen_n2_data_out");
	// sca_util::sca_trace(wf_ams_ethernet, top->ethernetSignals.n1_data_valid, "pkt_gen_n1_data_valid");
	// sca_util::sca_trace(wf_ams_ethernet, top->ethernetSignals.n1_data_out_valid, "pkt_gen_n1_data_out_valid");
	// sca_util::sca_trace(wf_ams_ethernet, top->ethernetSignals.n1_data_out, "pkt_gen_n1_data_out");
	// sca_util::sca_trace(wf_ams_ethernet, top->ethernetSignals.sigBitCount, "pkt_gen_sigBitCount");
	// sca_util::sca_trace(wf_ams_ethernet, top->ethernetSignals.n1_sigBitCount, "pkt_gen_n1_sigBitCount");
	// sca_util::sca_trace(wf_ams_ethernet, top->ethernetSignals.n2_sigBitCount, "pkt_gen_n2_sigBitCount");
	// sca_util::sca_trace(wf_ams_ethernet, top->ethernetSignals.tmp_data_out_valid, "pkt_gen_tmp_data_out_valid");
	sca_util::sca_trace(wf_ams_ethernet, top->ethernetSignals.data_to_send, "pkt_gen_data_to_send");
	// sca_util::sca_trace(wf_ams_ethernet, top->ethernetSignals.data_valid_to_send, "pkt_gen_data_valid_to_send");
	sca_util::sca_trace(wf_ams_ethernet, top->ethernetSignals.remaining_bytes_to_send, "pkt_gen_remaining_bytes_to_send");

	sca_util::sca_trace(wf_ams_vga, top->vga_signals.s_hsync, "s_hsync");
	sca_util::sca_trace(wf_ams_vga, top->vga_signals.s_vsync, "s_vsync");
	sca_util::sca_trace(wf_ams_vga, top->vga_signals.s_ana_red, "s_ana_red");
	sca_util::sca_trace(wf_ams_vga, top->vga_signals.s_ana_green, "s_ana_green");
	sca_util::sca_trace(wf_ams_vga, top->vga_signals.s_ana_blue, "s_ana_blue");

	if (source_image >= 0)
	{
		top->load_img_from_memory(source_image);
	}

	auto start = std::chrono::steady_clock::now();
	sc_core::sc_start();
	auto end = std::chrono::steady_clock::now();

	std::chrono::duration<double> elapsed_seconds = end - start;
	double instructions = perf->getInstructions() / elapsed_seconds.count();

	std::cout << "Total elapsed time: " << elapsed_seconds.count() << "s" << std::endl;
	std::cout << "Simulated " << int(std::round(instructions)) << " instr/sec" << std::endl;
	
	//Generate output image
	// top->save_img_from_memory(output_image);
	
	sc_close_vcd_trace_file(wf);
	sca_util::sca_close_vcd_trace_file(wf_ams_ethernet);
	sca_util::sca_close_vcd_trace_file(wf_ams_vga);
	std::cout << "Press Enter to finish" << std::endl;
	std::cin.ignore();

	// call all destructors, clean exit.
	delete top;

	return 0;
}
