#include <systemc-ams.h>
#include <systemc.h>
#include "ethernetEncoder.h"

int sc_main(int argc, char* argv[])
{
    sca_tdf::sca_signal<sc_dt::sc_bv<4>> data_in_signal;
    sca_tdf::sca_signal<double> mlt3_out_signal;

    InputSignalGenerator inputGen("inputGen");
    ethernetEncoder encoder("encoder");

    inputGen.out(data_in_signal);
    encoder.data_in(data_in_signal);
    encoder.mlt3_out(mlt3_out_signal);

    // Trace file setup
    sca_util::sca_trace_file* tf = sca_util::sca_create_vcd_trace_file("ethernetEncoder");

    if (tf) {
        sca_util::sca_trace(tf, data_in_signal, "data_in");
        sca_util::sca_trace(tf, mlt3_out_signal, "mlt3_out");
        // tf->set_time_unit(1.0, sc_core::SC_NS); // This line can be omitted
    }

    sc_start(100, SC_NS);

    if (tf) {
        sca_util::sca_close_vcd_trace_file(tf);
    }

    return 0;
}