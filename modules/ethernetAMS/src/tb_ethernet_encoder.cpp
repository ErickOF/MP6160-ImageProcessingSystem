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
    sca_util::sca_trace_file *traceFile = sca_util::sca_create_vcd_trace_file("ethernetEncoder");

    if (traceFile)
    {
        sca_util::sca_trace(traceFile, data_in_signal, "data_in");
        sca_util::sca_trace(traceFile, mlt3_out_signal, "mlt3_out");
    }

    sc_start(500, SC_NS); // Run the simulation for 500 ns

    if (traceFile)
    {
        sca_util::sca_close_vcd_trace_file(traceFile);
    }

    return 0;
}
