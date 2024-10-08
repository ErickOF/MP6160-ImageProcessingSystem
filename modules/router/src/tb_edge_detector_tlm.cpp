#ifndef USING_TLM_ROUTER_TB_EN
#ifdef USING_TLM_TB_EN
#ifndef TB_EDGE_DETECTOR_TLM_CPP
#define TB_EDGE_DETECTOR_TLM_CPP

// #define int64  systemc_int64
// #define uint64 systemc_uint64
#include <systemc.h>
// #undef int64
// #undef uint64
#define int64  opencv_int64
#define uint64 opencv_uint64
#include <opencv2/opencv.hpp>
#undef int64
#undef uint64

#include <cmath>

using namespace cv;
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>

#include <systemc-ams.h>

#include "common_func.hpp"
#include "important_defines.hpp"

#include "memory_tlm.hpp"
#include "rgb2gray_pv_model.hpp"
#include "ips_filter_tlm.hpp"
#include "sobel_edge_detector_tlm.hpp"
#include "unification_pv_model.hpp"
#include "ips_jpg_pv_model.hpp"
#include "packetGenerator_tlm.hpp"
#include "ethernetEncoder.h"
#include "adc.hpp"
#include "vga_tlm.hpp"
#include "seq_item_vga.hpp"
#include "img_initiator.cpp"
#include "img_router.cpp"

#if !defined(RGB2GRAY_PV_EN) || !defined(IPS_FILTER_LT_EN) || !defined(EDGE_DETECTOR_AT_EN) || !defined(IMG_UNIFICATE_PV_EN) || !defined(IPS_JPG_PV_EN)
#error "Not all the required macros (RGB2GRAY_PV_EN, IPS_FILTER_LT_EN, EDGE_DETECTOR_AT_EN, IMG_UNIFICATE_PV_EN and IPS_JPG_PV_EN) are defined."
#endif // Models

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

SC_MODULE(Tb_top)
{
  img_router<5> *router;
  //img_initiator *sobel_initiator;
  //img_initiator *tb_initiator;
  //img_initiator *filter_initiator;
  img_initiator *tb_initiator;
  sobel_edge_detector_tlm *edge_detector_DUT;
  Rgb2Gray *rgb2gray_DUT;
  memory_tlm *memory_DUT;
  ips_filter_tlm *filter_DUT;
  img_unification_module* unification_DUT;
  jpg_output *jpg_comp_DUT;
  packetGenerator_tlm *packetGenerator_DUT;
  ethernetEncoder *ethernetEncoder_DUT;
  adc<IPS_BITS, IPS_VOLTAGE_MIN, IPS_VOLTAGE_MAX, VUnit::mv> *adc_red_DUT;
  adc<IPS_BITS, IPS_VOLTAGE_MIN, IPS_VOLTAGE_MAX, VUnit::mv> *adc_green_DUT;
  adc<IPS_BITS, IPS_VOLTAGE_MIN, IPS_VOLTAGE_MAX, VUnit::mv> *adc_blue_DUT;
  vga_tlm *vga_DUT;
  seq_item_vga<
    IPS_BITS,
    IPS_H_ACTIVE, IPS_H_FP, IPS_H_SYNC_PULSE, IPS_H_BP,
    IPS_V_ACTIVE, IPS_V_FP, IPS_V_SYNC_PULSE, IPS_V_BP,
    IPS_VOLTAGE_MIN, IPS_VOLTAGE_MAX, VUnit::mv> *ips_seq_item_vga;

  Ethernet_AMS_Signals ethernetSignals;
  vga_ams_signals_t vga_signals;

  sca_core::sca_time sample_time;

  bool use_prints = true;
  
  SC_CTOR(Tb_top) : sample_time(10, SC_NS)
  {
    router = new img_router<5>("router");
    edge_detector_DUT = new sobel_edge_detector_tlm("edge_detector_DUT");
    rgb2gray_DUT = new Rgb2Gray("rgb2gray_DUT");
    tb_initiator = new img_initiator("tb_initiator");
    //sobel_initiator = new img_initiator("sobel_initiator");
    //tb_initiator = new img_initiator("tb_initiator");
    //filter_initiator = new img_initiator("filter_initiator");
    memory_DUT = new memory_tlm("memory_DUT");
    filter_DUT = new ips_filter_tlm("filter_DUT");
    unification_DUT = new img_unification_module("unification_DUT");
    jpg_comp_DUT = new jpg_output("jpg_comp_DUT", IMAG_ROWS, IMAG_COLS);
    packetGenerator_DUT = new packetGenerator_tlm("packetGenerator_DUT", sample_time);
    ethernetEncoder_DUT = new ethernetEncoder("ethernetEncoder_DUT", sample_time);
    
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

    vga_DUT = new vga_tlm("vga_DUT");
    vga_DUT->red(vga_signals.s_tx_red);
    vga_DUT->green(vga_signals.s_tx_green);
    vga_DUT->blue(vga_signals.s_tx_blue);
    vga_DUT->o_h_count(vga_signals.s_h_count);
    vga_DUT->o_v_count(vga_signals.s_v_count);
    vga_DUT->o_hsync(vga_signals.s_hsync);
    vga_DUT->o_vsync(vga_signals.s_vsync);
    
    router->set_delays(sc_time(20, SC_NS), sc_time(20, SC_NS));
    // sobel_initiator->start_img_initiators();
    // sobel_initiator->set_delays(sc_time(10, SC_NS), sc_time(10, SC_NS));
    // tb_initiator->start_img_initiators();
    // tb_initiator->set_delays(sc_time(10, SC_NS), sc_time(10, SC_NS));
    // filter_initiator->start_img_initiators();
    // filter_initiator->set_delays(sc_time(10, SC_NS), sc_time(10, SC_NS));
    tb_initiator->start_img_initiators();
    tb_initiator->set_delays(sc_time(10, SC_NS), sc_time(10, SC_NS));
    edge_detector_DUT->set_delays(sc_time(10, SC_NS), sc_time(3, SC_NS));
    memory_DUT->set_delays(sc_time(10, SC_NS), sc_time(3, SC_NS));
    filter_DUT->set_delays(sc_time(10, SC_NS), sc_time(3, SC_NS));
    
    // Bind initiator socket to target socket
    router->initiator_socket[0]->bind(filter_DUT->socket);
    router->initiator_socket[1]->bind(edge_detector_DUT->socket);
    router->initiator_socket[2]->bind(memory_DUT->socket);
    router->initiator_socket[3]->bind(packetGenerator_DUT->socket);
    router->initiator_socket[4]->bind(vga_DUT->socket);
    tb_initiator->socket.bind(router->target_socket);
    
    SC_THREAD(thread_process);

#ifdef DISABLE_TB_DEBUG
    this->use_prints = false;
#endif //DISABLE_TB_DEBUG

    tb_initiator->use_prints = this->use_prints;

    checkprintenable(use_prints);
  }
  
  void thread_process()
  {
    Mat colorImage;
    
    unsigned char localR, localG, localB;
    unsigned short int localResult;
    unsigned char* local_results;
    unsigned char* local_read;
    int local_count = 0;
    
    short int localGradientX, localGradientY;
    
    int total_number_of_pixels;
    int current_number_of_pixels = 0;
    float next_target_of_completion = 10.0;
    
    int local_group_count;
    int compression_output_size = 0;
    
    signed char* compression_results;
    
    /*colorImage = imread("../../tools/datagen/src/imgs/car_rgb_noisy_image.jpg", IMREAD_UNCHANGED);
  
    if (colorImage.empty())
    { 
      cout << "Image File " << "Not Found" << endl;

      return; 
    }
    if ((colorImage.rows > IMAG_ROWS) || (colorImage.cols > IMAG_COLS))
    {
      cout << "Image size " << colorImage.rows << "x" << colorImage.cols << " won't fit in the designated space";
      
      return;
    }
    
    dbgprint("Saving image in memory");
    for (int i = 0; i < colorImage.rows; i++)
    {
      for (int j = 0; j < colorImage.cols; j++)
      {
        localR = colorImage.at<cv::Vec3b>(i, j)[2];
        localG = colorImage.at<cv::Vec3b>(i, j)[1];
        localB = colorImage.at<cv::Vec3b>(i, j)[0];
        
        local_results = new unsigned char[3];
        *(local_results    ) = localR;
        *(local_results + 1) = localG;
        *(local_results + 2) = localB;
        memory_DUT->backdoor_write(local_results, 3 * sizeof(char), IMG_INPUT_ADDRESS_LO + ((i * IMAG_COLS * 3) + (j * 3)));
      }
    }
    
    // Fill remaining columns in case that width of the image was smaller than IMAG_COLS
    for (int j = colorImage.cols; j < IMAG_COLS; j++)
    {
      for (int i = 0; i < colorImage.rows; i++)
      {
        local_results = new unsigned char[3];
        *(local_results    ) = 0;
        *(local_results + 1) = 0;
        *(local_results + 2) = 0;
        memory_DUT->backdoor_write(local_results, 3 * sizeof(char), IMG_INPUT_ADDRESS_LO + ((i * IMAG_COLS * 3) + (j * 3)));
      }
    }
    
    // Fill remaining columns in case that height of the image was smaller than IMAG_ROWS
    for (int i = colorImage.rows; i < IMAG_ROWS; i++)
    {
      for (int j = 0; j < IMAG_COLS; j++)
      {
        local_results = new unsigned char[3];
        *(local_results    ) = 0;
        *(local_results + 1) = 0;
        *(local_results + 2) = 0;
        memory_DUT->backdoor_write(local_results, 3 * sizeof(char), IMG_INPUT_ADDRESS_LO + ((i * IMAG_COLS * 3) + (j * 3)));
      }
    }

    dbgprint("Saved image in memory");*/

    Mat grayImagePrevMem(IMAG_ROWS, IMAG_COLS, CV_8UC1);
    Mat filteredImagePrevMem(IMAG_ROWS, IMAG_COLS, CV_8UC1);
    Mat detectedImagePrevMemX(IMAG_ROWS, IMAG_COLS, CV_8UC1);
    Mat detectedImagePrevMemY(IMAG_ROWS, IMAG_COLS, CV_8UC1);
    Mat detectedImagePrevMem(IMAG_ROWS, IMAG_COLS, CV_8UC1);
    
    Mat originalImageAfterMem(IMAG_ROWS, IMAG_COLS, CV_8UC3);
    Mat grayImageAfterMem(IMAG_ROWS, IMAG_COLS, CV_8UC1);
    Mat filteredImageAfterMem(IMAG_ROWS, IMAG_COLS, CV_8UC1);
    Mat detectedImageAfterMemX(IMAG_ROWS, IMAG_COLS, CV_8UC1);
    Mat detectedImageAfterMemY(IMAG_ROWS, IMAG_COLS, CV_8UC1);
    Mat detectedImageAfterMem(IMAG_ROWS, IMAG_COLS, CV_8UC1);

    dbgprint("Starting receiving of the image through VGA");

    unsigned char *vga_start = new unsigned char;
    unsigned char *vga_done;
    *vga_start = 1;
    tb_initiator->write(vga_start, IMG_INPUT_START_ADDRESS_LO, sizeof(char));

    tb_initiator->read(vga_done, IMG_INPUT_DONE_ADDRESS_LO, sizeof(char));
    while (*vga_done == 0)
    {
      delete[] vga_done;
      wait(10, SC_US);
      tb_initiator->read(vga_done, IMG_INPUT_DONE_ADDRESS_LO, sizeof(char));
    }
    delete[] vga_done;

    dbgprint("Finished receiving of the image through VGA");

    tb_initiator->read(local_results, IMG_INPUT_ADDRESS_LO, IMAG_ROWS * IMAG_COLS * 3);

    // Sanity check that the image was written in memory as expected
    for (int i = 0; i < IMAG_ROWS; i++)
    {
      for (int j = 0; j < IMAG_COLS; j++)
      {
        originalImageAfterMem.at<cv::Vec3b>(i, j)[2] = local_results[(i * IMAG_COLS * 3) + (j * 3) + 2];
        originalImageAfterMem.at<cv::Vec3b>(i, j)[1] = local_results[(i * IMAG_COLS * 3) + (j * 3) + 1];
        originalImageAfterMem.at<cv::Vec3b>(i, j)[0] = local_results[(i * IMAG_COLS * 3) + (j * 3)    ];
      }
    }

    total_number_of_pixels = IMAG_ROWS * IMAG_COLS;

    dbgprint("Starting gray scale conversion");
    
    for (int i = 0; i < IMAG_ROWS; i++)
    {
      local_group_count = 0;
      for (int j = 0; j < IMAG_COLS; j++)
      {
        unsigned char* read_ptr;
        dbgmodprint(use_prints, "Before doing a read in TB");
        tb_initiator->read(read_ptr, IMG_INPUT_ADDRESS_LO + ((i * IMAG_COLS * 3) + (j * 3)), 3 * sizeof(char));
        dbgmodprint(use_prints, "After doing a read in TB");
        localR = *(read_ptr    );
        localG = *(read_ptr + 1);
        localB = *(read_ptr + 2);
        rgb2gray_DUT->set_rgb_pixel(localR, localG, localB);
        localResult = (unsigned short int)rgb2gray_DUT->obtain_gray_value();
        
        dbgmodprint(use_prints, "Data_returned gray_result: %0d", localResult);

        grayImagePrevMem.at<uchar>(i, j) = (unsigned char)localResult;
        
        if (local_count == 0)
        {
          local_results = new unsigned char[8];
        }
        
        *(local_results + local_count) = (unsigned char)localResult;
        
        local_count++;
        
        if (local_count == 8)
        {
          dbgmodprint(use_prints, "Before doing a write in TB");
          sanity_check_address(IMG_INPROCESS_A_ADDRESS_LO + ((i * IMAG_COLS) + (local_group_count * 8 * sizeof(char))), IMG_INPROCESS_A_ADDRESS_LO, IMG_INPROCESS_A_ADDRESS_LO + IMG_INPROCESS_A_SIZE);
          tb_initiator->write(local_results, IMG_INPROCESS_A_ADDRESS_LO + ((i * IMAG_COLS) + (local_group_count * 8 * sizeof(char))), 8 * sizeof(char));
          dbgmodprint(use_prints, "After doing a write in TB");
          local_count = 0;
          local_group_count++;
        }
        
        current_number_of_pixels++;
        if (((((float)(current_number_of_pixels)) / ((float)(total_number_of_pixels))) * 100.0) >= next_target_of_completion) {
          dbgprint("Image processing completed at %f", next_target_of_completion);
          next_target_of_completion += 10.0;
        }
      }
    }
    
    dbgprint("Finished gray scale conversion");
    
    // Sanity check that the image was written in memory as expected
    for (int i = 0; i < IMAG_ROWS; i++)
    {
      for (int j = 0; j < IMAG_COLS; j++)
      {
        unsigned char* read_ptr;
        memory_DUT->backdoor_read(read_ptr, 1 * sizeof(char), IMG_INPROCESS_A_ADDRESS_LO + ((i * IMAG_COLS) + j));
        grayImageAfterMem.at<uchar>(i, j) = *read_ptr;
      }
    }
    
    local_count = 0;
    current_number_of_pixels = 0;
    next_target_of_completion = 10.0;

    dbgprint("Starting filtering the image");
    
    for (int i = 0; i < IMAG_ROWS; i++)
    {
      local_group_count = 0;
      for (int j = 0; j < IMAG_COLS; j++)
      {
        unsigned char* local_window_ptr = new unsigned char[9];
        IPS_OUT_TYPE_TB* data_returned_ptr;
        unsigned char* write_ptr;
        unsigned char* read_ptr;
        IPS_OUT_TYPE_TB data_returned;
        
        extract_window(i, j, IMG_INPROCESS_A_ADDRESS_LO, local_window_ptr);
        
        write_ptr = local_window_ptr;
        dbgmodprint(use_prints, "Before doing a write in TB");
        tb_initiator->write(write_ptr, IMG_FILTER_KERNEL_ADDRESS_LO, 9 * sizeof(char));
        dbgmodprint(use_prints, "After doing a write in TB");
        dbgmodprint(use_prints, "Before doing a read in TB");
        tb_initiator->read(read_ptr, IMG_FILTER_KERNEL_ADDRESS_LO, sizeof(IPS_OUT_TYPE_TB));
        dbgmodprint(use_prints, "After doing a read in TB");
        data_returned_ptr = reinterpret_cast<IPS_OUT_TYPE_TB*>(read_ptr);
        dbgmodprint(use_prints, "Data_returned filtered_result: %f", *data_returned_ptr);
        
        data_returned = *data_returned_ptr;
        
        if (data_returned > 255.0) {
          filteredImagePrevMem.at<uchar>(i, j) = 255;
        }
        else {
          filteredImagePrevMem.at<uchar>(i, j) = (unsigned char)data_returned; 
        }
        
        if (local_count == 0)
        {
          local_results = new unsigned char[8];
        }
        
        if (data_returned > 255.0) {
          *(local_results + local_count) = 255;
        }
        else {
          *(local_results + local_count) = (unsigned char)data_returned; 
        }
        
        local_count++;
        
        if (local_count == 8)
        {
          dbgmodprint(use_prints, "Before doing a write in TB");
          sanity_check_address(IMG_INPROCESS_D_ADDRESS_LO + ((i * IMAG_COLS) + (local_group_count * 8 * sizeof(char))), IMG_INPROCESS_D_ADDRESS_LO, IMG_INPROCESS_D_ADDRESS_LO + IMG_INPROCESS_D_SIZE);
          tb_initiator->write(local_results, IMG_INPROCESS_D_ADDRESS_LO + ((i * IMAG_COLS) + (local_group_count * 8 * sizeof(char))), 8 * sizeof(char));
          dbgmodprint(use_prints, "After doing a write in TB");
          local_count = 0;
          local_group_count++;
        }
        
        current_number_of_pixels++;
        if (((((float)(current_number_of_pixels)) / ((float)(total_number_of_pixels))) * 100.0) >= next_target_of_completion) {
          dbgprint("Image processing completed at %f", next_target_of_completion);
          next_target_of_completion += 10.0;
        }
      }
    }
    
    dbgprint("Finished filtering the image");
    
    // Sanity check that the image was written in memory as expected
    for (int i = 0; i < IMAG_ROWS; i++)
    {
      for (int j = 0; j < IMAG_COLS; j++)
      {
        unsigned char* read_ptr;
        memory_DUT->backdoor_read(read_ptr, 1 * sizeof(char), IMG_INPROCESS_D_ADDRESS_LO + ((i * IMAG_COLS) + j));
        filteredImageAfterMem.at<uchar>(i, j) = *read_ptr;
      }
    }
    
    local_count = 0;
    current_number_of_pixels = 0;
    next_target_of_completion = 10.0;

    dbgprint("Starting calculating sobel gradients of the image");
    
    for (int i = 0; i < IMAG_ROWS; i++)
    {
      local_group_count = 0;
      for (int j = 0; j < IMAG_COLS; j++)
      {
        unsigned char* local_window_ptr = new unsigned char[16];
        short int* data_returned_ptr;
        unsigned char* write_ptr;
        unsigned char* read_ptr;
        
        extract_window(i, j, IMG_INPROCESS_D_ADDRESS_LO, local_window_ptr);
        for (int k = 9; k < 16; k++)
        {
          *(local_window_ptr + k) = 0;
        }
        
        write_ptr = local_window_ptr;
        dbgmodprint(use_prints, "Before doing a write in TB");
        tb_initiator->write(write_ptr, SOBEL_INPUT_0_ADDRESS_LO, 8 * sizeof(char));
        dbgmodprint(use_prints, "After doing a write in TB");
        write_ptr = (local_window_ptr + 8);
        dbgmodprint(use_prints, "Before doing a write in TB");
        tb_initiator->write(write_ptr, SOBEL_INPUT_1_ADDRESS_LO, 8 * sizeof(char));
        dbgmodprint(use_prints, "After doing a write in TB");
        dbgmodprint(use_prints, "Before doing a read in TB");
        tb_initiator->read(read_ptr, SOBEL_OUTPUT_ADDRESS_LO, 8 * sizeof(char));
        dbgmodprint(use_prints, "After doing a read in TB");
        data_returned_ptr = reinterpret_cast<short int*>(read_ptr);
        dbgmodprint(use_prints, "Data_returned localGradientX: %0d", *data_returned_ptr);
        dbgmodprint(use_prints, "Data_returned localGradientY: %0d", *(data_returned_ptr+1));
        
        localGradientX = *data_returned_ptr;
        localGradientY = *(data_returned_ptr+1);
        
        if ((localGradientX > 255) || (localGradientX < -255)) {
          detectedImagePrevMemX.at<uchar>(i, j) = 255;
        } 
        else if (localGradientX < 0) {
          detectedImagePrevMemX.at<uchar>(i, j) = (unsigned char)(-localGradientX);
        }
        else {
          detectedImagePrevMemX.at<uchar>(i, j) = (unsigned char)localGradientX; 
        }
        if ((localGradientY > 255) || (localGradientY < -255)) {
          detectedImagePrevMemY.at<uchar>(i, j) = 255;
        }
        else if (localGradientY < 0) {
          detectedImagePrevMemY.at<uchar>(i, j) = (unsigned char)(-localGradientY); 
        }
        else {
          detectedImagePrevMemY.at<uchar>(i, j) = (unsigned char)localGradientY;
        }
        
        if (local_count == 0)
        {
          local_results = new unsigned char[16];
        }
        
        memcpy((local_results + (local_count * 2)                          ), (data_returned_ptr    ), sizeof(short int));
        memcpy((local_results + (local_count * 2) + (4 * sizeof(short int))), (data_returned_ptr + 1), sizeof(short int));
        local_count++;
        
        if (local_count == 4)
        {
          write_ptr = local_results;
          dbgmodprint(use_prints, "Before doing a write in TB");
          sanity_check_address(IMG_INPROCESS_B_ADDRESS_LO + ((i * IMAG_COLS * sizeof(short int)) + (local_group_count * 4 * sizeof(short int))), IMG_INPROCESS_B_ADDRESS_LO, IMG_INPROCESS_B_ADDRESS_LO + IMG_INPROCESS_B_SIZE);
          tb_initiator->write(write_ptr, IMG_INPROCESS_B_ADDRESS_LO + ((i * IMAG_COLS * sizeof(short int)) + (local_group_count * 4 * sizeof(short int))), 4 * sizeof(short int));
          dbgmodprint(use_prints, "After doing a write in TB");
          write_ptr = (local_results + 8);
          dbgmodprint(use_prints, "Before doing a write in TB");
          sanity_check_address(IMG_INPROCESS_C_ADDRESS_LO + ((i * IMAG_COLS * sizeof(short int)) + (local_group_count * 4 * sizeof(short int))), IMG_INPROCESS_C_ADDRESS_LO, IMG_INPROCESS_C_ADDRESS_LO + IMG_INPROCESS_C_SIZE);
          tb_initiator->write(write_ptr, IMG_INPROCESS_C_ADDRESS_LO + ((i * IMAG_COLS * sizeof(short int)) + (local_group_count * 4 * sizeof(short int))), 4 * sizeof(short int));
          dbgmodprint(use_prints, "After doing a write in TB");
          local_count = 0;
          local_group_count++;
        }
        
        current_number_of_pixels++;
        if (((((float)(current_number_of_pixels)) / ((float)(total_number_of_pixels))) * 100.0) >= next_target_of_completion) {
          dbgprint("Image processing completed at %f", next_target_of_completion);
          next_target_of_completion += 10.0;
        }
      }
    }
    
    dbgprint("Finished calculating sobel gradients of the image");
    
    // Sanity check that the image was written in memory as expected
    for (int i = 0; i < IMAG_ROWS; i++)
    {
      for (int j = 0; j < IMAG_COLS; j++)
      {
        unsigned char* read_ptr;
        short int* data_returned_ptr;
        
        memory_DUT->backdoor_read(read_ptr, sizeof(short int), IMG_INPROCESS_B_ADDRESS_LO + ((i * IMAG_COLS * sizeof(short int)) + (j * sizeof(short int))));
        data_returned_ptr = reinterpret_cast<short int*>(read_ptr);
        localGradientX = *data_returned_ptr;
        if ((localGradientX > 255) || (localGradientX < -255)) {
          detectedImageAfterMemX.at<uchar>(i, j) = 255;
        } 
        else if (localGradientX < 0) {
          detectedImageAfterMemX.at<uchar>(i, j) = (unsigned char)(-localGradientX);
        }
        else {
          detectedImageAfterMemX.at<uchar>(i, j) = (unsigned char)localGradientX; 
        }
        
        memory_DUT->backdoor_read(read_ptr, sizeof(short int), IMG_INPROCESS_C_ADDRESS_LO + ((i * IMAG_COLS * sizeof(short int)) + (j * sizeof(short int))));
        data_returned_ptr = reinterpret_cast<short int*>(read_ptr);
        localGradientY = *data_returned_ptr;
        if ((localGradientY > 255) || (localGradientY < -255)) {
          detectedImageAfterMemY.at<uchar>(i, j) = 255;
        } 
        else if (localGradientY < 0) {
          detectedImageAfterMemY.at<uchar>(i, j) = (unsigned char)(-localGradientY);
        }
        else {
          detectedImageAfterMemY.at<uchar>(i, j) = (unsigned char)localGradientY; 
        }
      }
    }
    
    local_count = 0;
    current_number_of_pixels = 0;
    next_target_of_completion = 10.0;

    dbgprint("Starting with the unification of the magnitude of the gradients of the image");
    
    for (int i = 0; i < IMAG_ROWS; i++)
    {
      local_group_count = 0;
      for (int j = 0; j < IMAG_COLS; j++)
      {
        unsigned char* read_ptr;
        short int* gradients_ptr;
        unsigned char unification_result;
        
        if (local_count == 0)
        {
          local_read = new unsigned char[16];
        
          dbgmodprint(use_prints, "Before doing a read in TB");
          tb_initiator->read(read_ptr, IMG_INPROCESS_B_ADDRESS_LO + ((i * IMAG_COLS * sizeof(short int)) + (local_group_count * 4 * sizeof(short int))), 4 * sizeof(short int));
          dbgmodprint(use_prints, "After doing a read in TB");
          memcpy((local_read                          ), read_ptr, 4 * sizeof(short int));
          dbgmodprint(use_prints, "Before doing a read in TB");
          tb_initiator->read(read_ptr, IMG_INPROCESS_C_ADDRESS_LO + ((i * IMAG_COLS * sizeof(short int)) + (local_group_count * 4 * sizeof(short int))), 4 * sizeof(short int));
          dbgmodprint(use_prints, "After doing a read in TB");
          memcpy((local_read + (4 * sizeof(short int))), read_ptr, 4 * sizeof(short int));
        }
        
        gradients_ptr = reinterpret_cast<short int*>(local_read);
        localGradientX = *(gradients_ptr + local_count);
        gradients_ptr = reinterpret_cast<short int*>(local_read + (4 * sizeof(short int)));
        localGradientY = *(gradients_ptr + local_count);
        
        unification_DUT->unificate_pixel((int)localGradientX, (int)localGradientY, &unification_result);
        
        dbgmodprint(use_prints, "Data_returned unification_result: %0d", unification_result);
        
        detectedImagePrevMem.at<uchar>(i, j) = unification_result;
        
        if (local_count == 0)
        {
          local_results = new unsigned char[4];
        }
        
        *(local_results + local_count) = unification_result;
        
        local_count++;
        
        if (local_count == 4)
        {
          dbgmodprint(use_prints, "Before doing a write in TB");
          sanity_check_address(IMG_INPROCESS_A_ADDRESS_LO + ((i * IMAG_COLS) + j), IMG_INPROCESS_A_ADDRESS_LO, IMG_INPROCESS_A_ADDRESS_LO + IMG_INPROCESS_A_SIZE);
          tb_initiator->write(local_results, IMG_INPROCESS_A_ADDRESS_LO + ((i * IMAG_COLS) + (local_group_count * 4 * sizeof(char))), 4 * sizeof(char));
          dbgmodprint(use_prints, "After doing a write in TB");
          local_count = 0;
          local_group_count++;
        }
        
        current_number_of_pixels++;
        if (((((float)(current_number_of_pixels)) / ((float)(total_number_of_pixels))) * 100.0) >= next_target_of_completion) {
          dbgprint("Image processing completed at %f", next_target_of_completion);
          next_target_of_completion += 10.0;
        }
      }
    }
    
    dbgprint("Finished with the unification of the magnitude of the gradients of the image");
    
    // Sanity check that the image was written in memory as expected
    for (int i = 0; i < IMAG_ROWS; i++)
    {
      for (int j = 0; j < IMAG_COLS; j++)
      {
        unsigned char* read_ptr;
        memory_DUT->backdoor_read(read_ptr, 1 * sizeof(char), IMG_INPROCESS_A_ADDRESS_LO + ((i * IMAG_COLS) + j));
        detectedImageAfterMem.at<uchar>(i, j) = *read_ptr;
      }
    }
    
    local_count = 0;
    current_number_of_pixels = 0;
    next_target_of_completion = 10.0;

    dbgprint("Starting with the compression of the image");
    
    for (int i = 0; i < IMAG_ROWS; i++)
    {
      local_group_count = 0;
      for (int j = 0; j < IMAG_COLS; j++)
      {
        if (local_count == 0)
        {
          unsigned char* read_ptr;
          local_read = new unsigned char[8];
        
          dbgmodprint(use_prints, "Before doing a read in TB");
          tb_initiator->read(read_ptr, IMG_INPROCESS_A_ADDRESS_LO + ((i * IMAG_COLS) + (local_group_count * 8 * sizeof(char))), 8 * sizeof(char));
          dbgmodprint(use_prints, "After doing a read in TB");
          memcpy(local_read, read_ptr, 8 * sizeof(char));
        }
        
        jpg_comp_DUT->input_pixel((int)*(local_read + local_count), i, j);
        local_count++;
        
        if (local_count == 8)
        {
          local_count = 0;
          local_group_count++;
        }

        current_number_of_pixels++;
        if (((((float)(current_number_of_pixels)) / ((float)(total_number_of_pixels))) * 100.0) >= next_target_of_completion) {
          dbgprint("Image processing completed at %f", next_target_of_completion);
          next_target_of_completion += 10.0;
        }
      }
    }
    
    jpg_comp_DUT->jpeg_compression(&compression_output_size);
    
    local_count = 0;
    local_group_count = 0;
    current_number_of_pixels = 0;
    next_target_of_completion = 10.0;
    compression_results = new signed char[compression_output_size];
    total_number_of_pixels = compression_output_size;
    for (int i = 0; i < compression_output_size; i++)
    {
      jpg_comp_DUT->output_byte(compression_results, i);
      
      dbgmodprint(use_prints, "Data_returned compression_result: %0d", *(compression_results + i));
      
      local_count++;
      
      if ((local_count == 8))
      {
        local_results = reinterpret_cast<unsigned char*>(compression_results + (local_group_count * 8 * sizeof(char)));
        dbgmodprint(use_prints, "Before doing a write in TB");
        sanity_check_address(IMG_OUTPUT_ADDRESS_LO + (local_group_count * 8 * sizeof(char)), IMG_OUTPUT_ADDRESS_LO, IMG_OUTPUT_ADDRESS_LO + IMG_OUTPUT_SIZE);
        tb_initiator->write(local_results, IMG_OUTPUT_ADDRESS_LO + (local_group_count * 8 * sizeof(char)), 8 * sizeof(char));
        dbgmodprint(use_prints, "After doing a write in TB");
        local_count = 0;
        local_group_count++;
      }
      
      current_number_of_pixels++;
      if (((((float)(current_number_of_pixels)) / ((float)(total_number_of_pixels))) * 100.0) >= next_target_of_completion) {
        dbgprint("Image processing completed at %f", next_target_of_completion);
        next_target_of_completion += 10.0;
      }
    }

    dbgprint("Finished with the compression algorithm of the image");

    if (compression_output_size % 8 != 0)
    {
      local_results = reinterpret_cast<unsigned char*>(compression_results + (local_group_count * 8 * sizeof(char)));
      dbgmodprint(use_prints, "Before doing a write in TB");
      sanity_check_address(IMG_OUTPUT_ADDRESS_LO + (local_group_count * 8 * sizeof(char)), IMG_OUTPUT_ADDRESS_LO, IMG_OUTPUT_ADDRESS_LO + IMG_OUTPUT_SIZE);
      tb_initiator->write(local_results, IMG_OUTPUT_ADDRESS_LO + (local_group_count * 8 * sizeof(char)), (compression_output_size % 8) * sizeof(char));
      dbgmodprint(use_prints, "After doing a write in TB");
      local_count = 0;
      local_group_count++;
    }
    
    dbgprint("Finished with the compression of the image");
    
    local_count = 0;
    local_group_count = 0;
    current_number_of_pixels = 0;
    next_target_of_completion = 10.0;

    unsigned char* read_ptr;
    unsigned char* write_ptr;

    dbgprint("Starting with the transmision of the image");
    dbgprint("Preparing to transmit %0d bytes", compression_output_size);

    write_ptr = (unsigned char *)&compression_output_size;
    tb_initiator->write(write_ptr, IMG_OUTPUT_SIZE_ADDRESS_LO, sizeof(int));

    write_ptr = new unsigned char[1];
    *write_ptr = 1;
    tb_initiator->write(write_ptr, IMG_OUTPUT_DONE_ADDRESS_LO, sizeof(char));
    delete[] write_ptr;

    tb_initiator->read(read_ptr, IMG_OUTPUT_STATUS_ADDRESS_LO, 1 * sizeof(char));
    while (*read_ptr == 1)
    {
      delete[] read_ptr;
      wait(100, SC_NS);
      tb_initiator->read(read_ptr, IMG_OUTPUT_STATUS_ADDRESS_LO, 1 * sizeof(char));
    }

    dbgprint("Finished with the transmision of the image");
    
    
    imwrite("originalImageAfterMem.jpg", originalImageAfterMem);
    
    imwrite("grayImagePrevMem.jpg", grayImagePrevMem);
    imwrite("grayImageAfterMem.jpg", grayImageAfterMem);
    imwrite("filteredImagePrevMem.jpg", filteredImagePrevMem);
    imwrite("filteredImageAfterMem.jpg", filteredImageAfterMem);
    imwrite("detectedImagePrevMemX.jpg", detectedImagePrevMemX);
    imwrite("detectedImageAfterMemX.jpg", detectedImageAfterMemX);
    imwrite("detectedImagePrevMemY.jpg", detectedImagePrevMemY);
    imwrite("detectedImageAfterMemY.jpg", detectedImageAfterMemY);
    imwrite("detectedImagePrevMem.jpg", detectedImagePrevMem);
    imwrite("detectedImageAfterMem.jpg", detectedImageAfterMem);

    sc_stop();
  }
  
  void extract_window(int i, int j, unsigned int initial_address, unsigned char*& local_window_ptr)
  {
    unsigned char* read_ptr;
    if ((i == 0) && (j == 0)) // Upper left corner of the image
    {
      // First row
      *(local_window_ptr    ) = 0;
      *(local_window_ptr + 1) = 0;
      *(local_window_ptr + 2) = 0;
      // Second row
      dbgmodprint(use_prints, "Before doing a read in TB");
      tb_initiator->read(read_ptr, initial_address, 2 * sizeof(char));
      dbgmodprint(use_prints, "After doing a read in TB");
      *(local_window_ptr + 3) = 0;
      *(local_window_ptr + 4) = *(read_ptr    );
      *(local_window_ptr + 5) = *(read_ptr + 1);
      // Third row
      dbgmodprint(use_prints, "Before doing a read in TB");
      tb_initiator->read(read_ptr, initial_address  + IMAG_COLS, 2 * sizeof(char));
      dbgmodprint(use_prints, "After doing a read in TB");
      *(local_window_ptr + 6) = 0;
      *(local_window_ptr + 7) = *(read_ptr    );
      *(local_window_ptr + 8) = *(read_ptr + 1);
    }
    else if ((i == 0) && (j == IMAG_COLS - 1)) // Upper right corner of the image
    {
      // First row
      *(local_window_ptr    ) = 0;
      *(local_window_ptr + 1) = 0;
      *(local_window_ptr + 2) = 0;
      // Second row
      dbgmodprint(use_prints, "Before doing a read in TB");
      tb_initiator->read(read_ptr, initial_address + (IMAG_COLS - 2), 2 * sizeof(char));
      dbgmodprint(use_prints, "After doing a read in TB");
      *(local_window_ptr + 3) = *(read_ptr    );
      *(local_window_ptr + 4) = *(read_ptr + 1);
      *(local_window_ptr + 5) = 0;
      // Third row
      dbgmodprint(use_prints, "Before doing a read in TB");
      tb_initiator->read(read_ptr, initial_address + (IMAG_COLS + (IMAG_COLS - 2)), 2 * sizeof(char));
      dbgmodprint(use_prints, "After doing a read in TB");
      *(local_window_ptr + 6) = *(read_ptr    );
      *(local_window_ptr + 7) = *(read_ptr + 1);
      *(local_window_ptr + 8) = 0;
    }
    else if (i == 0) // Upper border
    {
      // First row
      *(local_window_ptr    ) = 0;
      *(local_window_ptr + 1) = 0;
      *(local_window_ptr + 2) = 0;
      // Second row
      dbgmodprint(use_prints, "Before doing a read in TB");
      tb_initiator->read(read_ptr, initial_address + (j - 1), 3 * sizeof(char));
      dbgmodprint(use_prints, "After doing a read in TB");
      *(local_window_ptr + 3) = *(read_ptr    );
      *(local_window_ptr + 4) = *(read_ptr + 1);
      *(local_window_ptr + 5) = *(read_ptr + 2);
      // Third row
      dbgmodprint(use_prints, "Before doing a read in TB");
      tb_initiator->read(read_ptr, initial_address + (IMAG_COLS + (j - 1)), 3 * sizeof(char));
      dbgmodprint(use_prints, "After doing a read in TB");
      *(local_window_ptr + 6) = *(read_ptr    );
      *(local_window_ptr + 7) = *(read_ptr + 1);
      *(local_window_ptr + 8) = *(read_ptr + 2);
    }
    else if ((i == IMAG_ROWS - 1) && (j == 0)) // Lower left corner of the image
    {
      // First row
      dbgmodprint(use_prints, "Before doing a read in TB");
      tb_initiator->read(read_ptr, initial_address + ((IMAG_ROWS - 2) * IMAG_COLS), 2 * sizeof(char));
      dbgmodprint(use_prints, "After doing a read in TB");
      *(local_window_ptr    ) = 0;
      *(local_window_ptr + 1) = *(read_ptr    );
      *(local_window_ptr + 2) = *(read_ptr + 1);
      // Second row
      dbgmodprint(use_prints, "Before doing a read in TB");
      tb_initiator->read(read_ptr, initial_address + ((IMAG_ROWS - 1) * IMAG_COLS), 2 * sizeof(char));
      dbgmodprint(use_prints, "After doing a read in TB");
      *(local_window_ptr + 3) = 0;
      *(local_window_ptr + 4) = *(read_ptr    );
      *(local_window_ptr + 5) = *(read_ptr + 1);
      // Third row
      *(local_window_ptr + 6) = 0;
      *(local_window_ptr + 7) = 0;
      *(local_window_ptr + 8) = 0;
    }
    else if ((i == IMAG_ROWS - 1) && (j == IMAG_COLS - 1)) // Lower right corner of the image
    {
      // First row
      dbgmodprint(use_prints, "Before doing a read in TB");
      tb_initiator->read(read_ptr, initial_address + (((IMAG_ROWS - 2) * IMAG_COLS) + (IMAG_COLS - 2)), 2 * sizeof(char));
      dbgmodprint(use_prints, "After doing a read in TB");
      *(local_window_ptr    ) = *(read_ptr    );
      *(local_window_ptr + 1) = *(read_ptr + 1);
      *(local_window_ptr + 2) = 0;
      // Second row
      dbgmodprint(use_prints, "Before doing a read in TB");
      tb_initiator->read(read_ptr, initial_address + (((IMAG_ROWS - 1) * IMAG_COLS) + (IMAG_COLS - 2)), 2 * sizeof(char));
      dbgmodprint(use_prints, "After doing a read in TB");
      *(local_window_ptr + 3) = *(read_ptr    );
      *(local_window_ptr + 4) = *(read_ptr + 1);
      *(local_window_ptr + 5) = 0;
      // Third row
      *(local_window_ptr + 6) = 0;
      *(local_window_ptr + 7) = 0;
      *(local_window_ptr + 8) = 0;
    }
    else if (i == IMAG_ROWS - 1) // Lower border of the image
    {
      // First row
      dbgmodprint(use_prints, "Before doing a read in TB");
      tb_initiator->read(read_ptr, initial_address + (((IMAG_ROWS - 2) * IMAG_COLS) + (j - 1)), 3 * sizeof(char));
      dbgmodprint(use_prints, "After doing a read in TB");
      *(local_window_ptr    ) = *(read_ptr    );
      *(local_window_ptr + 1) = *(read_ptr + 1);
      *(local_window_ptr + 2) = *(read_ptr + 2);
      // Second row
      dbgmodprint(use_prints, "Before doing a read in TB");
      tb_initiator->read(read_ptr, initial_address + (((IMAG_ROWS - 1) * IMAG_COLS) + (j - 1)), 3 * sizeof(char));
      dbgmodprint(use_prints, "After doing a read in TB");
      *(local_window_ptr + 3) = *(read_ptr    );
      *(local_window_ptr + 4) = *(read_ptr + 1);
      *(local_window_ptr + 5) = *(read_ptr + 2);
      // Third row
      *(local_window_ptr + 6) = 0;
      *(local_window_ptr + 7) = 0;
      *(local_window_ptr + 8) = 0;
    }
    else if (j == 0) // Left border of the image
    {
      // First row
      dbgmodprint(use_prints, "Before doing a read in TB");
      tb_initiator->read(read_ptr, initial_address + ((i - 1) * IMAG_COLS), 2 * sizeof(char));
      dbgmodprint(use_prints, "After doing a read in TB");
      *(local_window_ptr    ) = 0;
      *(local_window_ptr + 1) = *(read_ptr    );
      *(local_window_ptr + 2) = *(read_ptr + 1);
      // Second row
      dbgmodprint(use_prints, "Before doing a read in TB");
      tb_initiator->read(read_ptr, initial_address + (i * IMAG_COLS), 2 * sizeof(char));
      dbgmodprint(use_prints, "After doing a read in TB");
      *(local_window_ptr + 3) = 0;
      *(local_window_ptr + 4) = *(read_ptr    );
      *(local_window_ptr + 5) = *(read_ptr + 1);
      // Third row
      dbgmodprint(use_prints, "Before doing a read in TB");
      tb_initiator->read(read_ptr, initial_address + ((i + 1) * IMAG_COLS), 2 * sizeof(char));
      dbgmodprint(use_prints, "After doing a read in TB");
      *(local_window_ptr + 6) = 0;
      *(local_window_ptr + 7) = *(read_ptr    );
      *(local_window_ptr + 8) = *(read_ptr + 1);
    }
    else if (j == IMAG_COLS - 1) // Right border of the image
    {
      // First row
      dbgmodprint(use_prints, "Before doing a read in TB");
      tb_initiator->read(read_ptr, initial_address + (((i - 1) * IMAG_COLS) + (j - 1)), 2 * sizeof(char));
      dbgmodprint(use_prints, "After doing a read in TB");
      *(local_window_ptr    ) = *(read_ptr    );
      *(local_window_ptr + 1) = *(read_ptr + 1);
      *(local_window_ptr + 2) = 0;
      // Second row
      dbgmodprint(use_prints, "Before doing a read in TB");
      tb_initiator->read(read_ptr, initial_address + ((i * IMAG_COLS) + (j - 1)), 2 * sizeof(char));
      dbgmodprint(use_prints, "After doing a read in TB");
      *(local_window_ptr + 3) = *(read_ptr    );
      *(local_window_ptr + 4) = *(read_ptr + 1);
      *(local_window_ptr + 5) = 0;
      // Third row
      dbgmodprint(use_prints, "Before doing a read in TB");
      tb_initiator->read(read_ptr, initial_address + (((i + 1) * IMAG_COLS) + (j - 1)), 2 * sizeof(char));
      dbgmodprint(use_prints, "After doing a read in TB");
      *(local_window_ptr + 6) = *(read_ptr    );
      *(local_window_ptr + 7) = *(read_ptr + 1);
      *(local_window_ptr + 8) = 0;
    }
    else // Rest of the image
    {
      // First row
      dbgmodprint(use_prints, "Before doing a read in TB");
      tb_initiator->read(read_ptr, initial_address + (((i - 1) * IMAG_COLS) + (j - 1)), 3 * sizeof(char));
      dbgmodprint(use_prints, "After doing a read in TB");
      *(local_window_ptr    ) = *(read_ptr    );
      *(local_window_ptr + 1) = *(read_ptr + 1);
      *(local_window_ptr + 2) = *(read_ptr + 2);
      // Second row
      dbgmodprint(use_prints, "Before doing a read in TB");
      tb_initiator->read(read_ptr, initial_address + ((i * IMAG_COLS) + (j - 1)), 3 * sizeof(char));
      dbgmodprint(use_prints, "After doing a read in TB");
      *(local_window_ptr + 3) = *(read_ptr    );
      *(local_window_ptr + 4) = *(read_ptr + 1);
      *(local_window_ptr + 5) = *(read_ptr + 2);
      // Third row
      dbgmodprint(use_prints, "Before doing a read in TB");
      tb_initiator->read(read_ptr, initial_address + (((i + 1) * IMAG_COLS) + (j - 1)), 3 * sizeof(char));
      dbgmodprint(use_prints, "After doing a read in TB");
      *(local_window_ptr + 6) = *(read_ptr    );
      *(local_window_ptr + 7) = *(read_ptr + 1);
      *(local_window_ptr + 8) = *(read_ptr + 2);
    }
  }
  
  void sanity_check_address(unsigned int address, unsigned int lower_address_limit, unsigned int upper_address_limit)
  {
    if (((lower_address_limit != 0) && (address < lower_address_limit)) || (address > upper_address_limit))
    {
      SC_REPORT_FATAL("TB MEM", "Access to memory crossing boundary");
    }
  }
};

int sc_main(int, char*[])
{
  // Pass command linke arguments
  sc_argc();
  sc_argv();
  
  // Open VCD file
  sc_trace_file* wf = sc_create_vcd_trace_file("edge_detector");
  sca_util::sca_trace_file* wf_ams = sca_util::sca_create_vcd_trace_file("edge_detector_ams");
  wf->set_time_unit(1, SC_PS);

  Tb_top top("top");
  
  // Dump the desired signals
  //Transaction tracking
  sc_trace(wf, top.tb_initiator->transaction_sent_id, "tb_initiator_sent_trans_id");
  sc_trace(wf, top.tb_initiator->transaction_received_id, "tb_initiator_received_trans_id");
  sc_trace(wf, top.filter_DUT->transaction_in_progress_id, "filter_DUT_trans_id");
  sc_trace(wf, top.edge_detector_DUT->transaction_in_progress_id, "edge_detector_DUT_trans_id");
  sc_trace(wf, top.memory_DUT->transaction_in_progress_id, "memory_DUT_trans_id");
  sc_trace(wf, top.router->transaction_in_bw_path_id, "router_bw_path_trans_id");
  sc_trace(wf, top.router->transaction_in_fw_path_id, "router_fw_path_trans_id");

  sc_trace(wf, top.filter_DUT->img_window[0], "filter_window(0)(0)");
  sc_trace(wf, top.filter_DUT->img_window[1], "filter_window(0)(1)");
  sc_trace(wf, top.filter_DUT->img_window[2], "filter_window(0)(2)");
  sc_trace(wf, top.filter_DUT->img_window[3], "filter_window(1)(0)");
  sc_trace(wf, top.filter_DUT->img_window[4], "filter_window(1)(1)");
  sc_trace(wf, top.filter_DUT->img_window[5], "filter_window(1)(2)");
  sc_trace(wf, top.filter_DUT->img_window[6], "filter_window(2)(0)");
  sc_trace(wf, top.filter_DUT->img_window[7], "filter_window(2)(1)");
  sc_trace(wf, top.filter_DUT->img_window[8], "filter_window(2)(2)");
  sc_trace(wf, top.filter_DUT->img_result, "filter_result");
  sc_trace(wf, top.edge_detector_DUT->Edge_Detector::data, "sobel_data");
  sc_trace(wf, top.edge_detector_DUT->Edge_Detector::address, "sobel_address");
  sc_trace(wf, top.edge_detector_DUT->localWindow[0][0], "sobel_localWindow(0)(0)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[0][1], "sobel_localWindow(0)(1)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[0][2], "sobel_localWindow(0)(2)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[1][0], "sobel_localWindow(1)(0)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[1][1], "sobel_localWindow(1)(1)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[1][2], "sobel_localWindow(1)(2)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[2][0], "sobel_localWindow(2)(0)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[2][1], "sobel_localWindow(2)(1)");
  sc_trace(wf, top.edge_detector_DUT->localWindow[2][2], "sobel_localWindow(2)(2)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[0][0], "sobel_localMultX(0)(0)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[0][1], "sobel_localMultX(0)(1)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[0][2], "sobel_localMultX(0)(2)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[1][0], "sobel_localMultX(1)(0)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[1][1], "sobel_localMultX(1)(1)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[1][2], "sobel_localMultX(1)(2)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[2][0], "sobel_localMultX(2)(0)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[2][1], "sobel_localMultX(2)(1)");
  sc_trace(wf, top.edge_detector_DUT->localMultX[2][2], "sobel_localMultX(2)(2)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[0][0], "sobel_localMultY(0)(0)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[0][1], "sobel_localMultY(0)(1)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[0][2], "sobel_localMultY(0)(2)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[1][0], "sobel_localMultY(1)(0)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[1][1], "sobel_localMultY(1)(1)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[1][2], "sobel_localMultY(1)(2)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[2][0], "sobel_localMultY(2)(0)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[2][1], "sobel_localMultY(2)(1)");
  sc_trace(wf, top.edge_detector_DUT->localMultY[2][2], "sobel_localMultY(2)(2)");
  sc_trace(wf, top.edge_detector_DUT->resultSobelGradientX, "sobel_resultSobelGradientX");
  sc_trace(wf, top.edge_detector_DUT->resultSobelGradientY, "sobel_resultSobelGradientY");
  sc_trace(wf, top.memory_DUT->mem_data, "memory_data");
  sc_trace(wf, top.memory_DUT->mem_address, "memory_address");
  sc_trace(wf, top.memory_DUT->mem_we, "memory_we");
  
  sca_util::sca_trace(wf_ams, top.ethernetSignals.mlt3_out_signal, "mlt3_out");
  sca_util::sca_trace(wf_ams, top.ethernetSignals.data_out_valid, "data_out_valid");
  sca_util::sca_trace(wf_ams, top.ethernetSignals.data_out, "data_out");
  sca_util::sca_trace(wf_ams, top.ethernetSignals.data_in, "pkt_gen_data_in");
  sca_util::sca_trace(wf_ams, top.ethernetSignals.data_in_valid, "pkt_gen_data_in_valid");
  sca_util::sca_trace(wf_ams, top.ethernetSignals.n2_data_valid, "pkt_gen_n2_data_valid");
  sca_util::sca_trace(wf_ams, top.ethernetSignals.n2_data_out_valid, "pkt_gen_n2_data_out_valid");
  sca_util::sca_trace(wf_ams, top.ethernetSignals.n2_data_out, "pkt_gen_n2_data_out");
  sca_util::sca_trace(wf_ams, top.ethernetSignals.n1_data_valid, "pkt_gen_n1_data_valid");
  sca_util::sca_trace(wf_ams, top.ethernetSignals.n1_data_out_valid, "pkt_gen_n1_data_out_valid");
  sca_util::sca_trace(wf_ams, top.ethernetSignals.n1_data_out, "pkt_gen_n1_data_out");
  sca_util::sca_trace(wf_ams, top.ethernetSignals.sigBitCount, "pkt_gen_sigBitCount");
  sca_util::sca_trace(wf_ams, top.ethernetSignals.n1_sigBitCount, "pkt_gen_n1_sigBitCount");
  sca_util::sca_trace(wf_ams, top.ethernetSignals.n2_sigBitCount, "pkt_gen_n2_sigBitCount");
  sca_util::sca_trace(wf_ams, top.ethernetSignals.tmp_data_out_valid, "pkt_gen_tmp_data_out_valid");
  sca_util::sca_trace(wf_ams, top.ethernetSignals.data_to_send, "pkt_gen_data_to_send");
  sca_util::sca_trace(wf_ams, top.ethernetSignals.data_valid_to_send, "pkt_gen_data_valid_to_send");
  sca_util::sca_trace(wf_ams, top.ethernetSignals.remaining_bytes_to_send, "pkt_gen_remaining_bytes_to_send");
  
  sc_start();

  dbgprint("Terminating simulation");
  sc_close_vcd_trace_file(wf);
  sca_util::sca_close_vcd_trace_file(wf_ams);

  return 0;
}
#endif // TB_EDGE_DETECTOR_TLM_CPP
#endif // USING_TLM_TB_EN
#endif // USING_TLM_ROUTER_TB_EN
