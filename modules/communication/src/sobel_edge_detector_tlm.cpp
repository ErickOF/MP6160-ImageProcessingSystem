#ifndef SOBEL_EDGE_DETECTOR_TLM_CPP
#define SOBEL_EDGE_DETECTOR_TLM_CPP
#include <systemc.h>
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include <tlm.h>
#include <tlm_utils/simple_initiator_socket.h>
#include <tlm_utils/simple_target_socket.h>
#include <tlm_utils/peq_with_cb_and_phase.h>

#include "sobel_edge_detector_tlm.hpp"

void sobel_edge_detector_tlm::do_when_read_transaction(unsigned char*& data){
        int sobel_results[2];

        sobel_results[0] = obtain_sobel_gradient_x();
        sobel_results[1] = obtain_sobel_gradient_y();
        cout << sobel_results[0] << endl;
        cout << sobel_results[1] << endl;
        memcpy(data, &sobel_results[0], 2*sizeof(int));
    }
void sobel_edge_detector_tlm::do_when_write_transaction(unsigned char*&data){
    int window[3][3];
    for (int* i = reinterpret_cast<int*>(data), row = 0, col = 0; i - reinterpret_cast<int*>(data) < 9; i++) {
        window[row][col] = *i;
        printf("VAL: %0d -> %0d", i-reinterpret_cast<int*>(data), *i);
        row++;
        if (row == 3) {
            row = 0;
            col++;
        }
    }
    this->set_local_window(window);
}

#endif
