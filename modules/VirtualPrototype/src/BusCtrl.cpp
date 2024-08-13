/**
 @file BusCtrl.cpp
 @brief Basic TLM-2 Bus controller
 @author Màrius Montón
 @date September 2018
 */
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BusCtrl.h"
#include "address_map.hpp"
#include "common_func.hpp"

SC_HAS_PROCESS(BusCtrl);
BusCtrl::BusCtrl(sc_core::sc_module_name const name, bool use_prints_) :
		sc_module(name), cpu_instr_socket("cpu_instr_socket"), cpu_data_socket(
				"cpu_data_socket"), memory_socket("memory_socket"), trace_socket(
				"trace_socket") {
	cpu_instr_socket.register_b_transport(this, &BusCtrl::b_transport);
	cpu_data_socket.register_b_transport(this, &BusCtrl::b_transport);
	log = Log::getInstance();
	cpu_instr_socket.register_get_direct_mem_ptr(this,
			&BusCtrl::instr_direct_mem_ptr);
	memory_socket.register_invalidate_direct_mem_ptr(this,
			&BusCtrl::invalidate_direct_mem_ptr);

	//Sockets for our modules
		use_prints = use_prints_;
		checkprintenable(use_prints);

}

void BusCtrl::b_transport(tlm::tlm_generic_payload &trans,
		sc_core::sc_time &delay) {

	sc_dt::uint64 adr = trans.get_address() / 4;

	switch (adr) {
	case TIMER_MEMORY_ADDRESS_HI / 4:
	case TIMER_MEMORY_ADDRESS_LO / 4:
	case TIMERCMP_MEMORY_ADDRESS_HI / 4:
	case TIMERCMP_MEMORY_ADDRESS_LO / 4:
		timer_socket->b_transport(trans, delay);
		break;
	case TRACE_MEMORY_ADDRESS / 4:
		trace_socket->b_transport(trans, delay);
		break;
	[[likely]] default:
		if ((IMG_FILTER_KERNEL_ADDRESS_LO / 4 <= adr  && adr < IMG_FILTER_KERNEL_ADDRESS_HI / 4) || 
		    (IMG_FILTER_OUTPUT_ADDRESS_LO / 4 <= adr  && adr < IMG_FILTER_OUTPUT_ADDRESS_HI / 4))
		{
			dbgmodprint(use_prints, "Writing/Reading to Filter!");
			filter_socket->b_transport(trans, delay);
		}
		else if ((SOBEL_INPUT_0_ADDRESS_LO / 4 <= adr && adr < SOBEL_INPUT_0_ADDRESS_HI / 4) || 
		         (SOBEL_INPUT_1_ADDRESS_LO / 4 <= adr && adr < SOBEL_INPUT_1_ADDRESS_HI / 4) ||
		         (SOBEL_OUTPUT_ADDRESS_LO / 4 <= adr && adr < SOBEL_OUTPUT_ADDRESS_HI / 4))
		{
			dbgmodprint(use_prints, "Writing/Reading to Sobel!");
			sobel_edge_detector_socket->b_transport(trans, delay);
		}
		else if ((IMG_INPUT_ADDRESS_LO / 4 <= adr && adr < IMG_INPUT_ADDRESS_HI / 4) ||
						 (IMG_INPUT_START_ADDRESS_LO / 4 <= adr && adr < IMG_INPUT_START_ADDRESS_HI / 4) ||
						 (IMG_INPUT_DONE_ADDRESS_LO / 4 <= adr && adr < IMG_INPUT_DONE_ADDRESS_HI / 4))
		{
			dbgmodprint(use_prints, "Writing/Reading to Receiver!");
			receiver_socket->b_transport(trans, delay);
		}
		else if ((IMG_OUTPUT_ADDRESS_LO / 4 <= adr && adr < IMG_OUTPUT_ADDRESS_HI / 4) ||
						 (IMG_OUTPUT_SIZE_ADDRESS_LO / 4 <= adr && adr < IMG_OUTPUT_SIZE_ADDRESS_HI / 4) ||
						 (IMG_OUTPUT_DONE_ADDRESS_LO / 4 <= adr && adr < IMG_OUTPUT_DONE_ADDRESS_HI / 4) ||
						 (IMG_OUTPUT_STATUS_ADDRESS_LO / 4 <= adr && adr < IMG_OUTPUT_STATUS_ADDRESS_HI / 4))
		{
			dbgmodprint(use_prints, "Writing/Reading to Transmiter!");
			transmiter_socket->b_transport(trans, delay);
		}
		else if ((IMG_SAVER_ID_ADDRESS_LO / 4 <= adr && adr < IMG_SAVER_ID_ADDRESS_HI / 4) ||
						 (IMG_SAVER_START_ADDRESS_LO / 4 <= adr && adr < IMG_SAVER_START_ADDRESS_HI / 4))
		{
			dbgmodprint(use_prints, "Writing/Reading to Image Saver!");
			image_saver_socket->b_transport(trans, delay);
		}
		else {
			memory_socket->b_transport(trans, delay);
		}
		break;
	}

#if 0
    if (cmd == tlm::TLM_READ_COMMAND) {
      log->SC_log(Log::DEBUG) << "RD Address: @0x" << hex << adr << dec << endl;
    } else {
      log->SC_log(Log::DEBUG) << "WR Address: @0x" << hex << adr << dec << endl;
    }
#endif

	trans.set_response_status(tlm::TLM_OK_RESPONSE);
}

bool BusCtrl::instr_direct_mem_ptr(tlm::tlm_generic_payload &gp,
		tlm::tlm_dmi &dmi_data) {
	return memory_socket->get_direct_mem_ptr(gp, dmi_data);
}

void BusCtrl::invalidate_direct_mem_ptr(sc_dt::uint64 start,
		sc_dt::uint64 end) {
	cpu_instr_socket->invalidate_direct_mem_ptr(start, end);
}

