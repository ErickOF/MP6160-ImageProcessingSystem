#include <systemc-ams.h>
#include <systemc.h>
#include "ethernetEncoder.h"
#include "packetGenerator.h"

int sc_main(int argc, char* argv[])
{
    sca_tdf::sca_signal<sc_dt::sc_bv<4>> data_in_signal;
    sca_tdf::sca_signal<double> mlt3_out_signal;

    sc_signal<bool> cclk;
    sc_signal<bool> data_out_valid;
    sc_signal<sc_dt::sc_bv<4>> data_out;

    unsigned char data[8];

    InputSignalGenerator inputGen("inputGen");
    ethernetEncoder encoder("encoder");
    clock_generator clk_gen("clk_gen");
    packetGenerator pkt_gen("pkt_gen");

    inputGen.out(data_in_signal);
    encoder.data_in(data_in_signal);
    encoder.mlt3_out(mlt3_out_signal);

    clk_gen.clk_out(cclk);
    pkt_gen.cclk(cclk);
    pkt_gen.data_out_valid(data_out_valid);
    pkt_gen.data_out(data_out);

    // Trace file setup
    sca_util::sca_trace_file *traceFile = sca_util::sca_create_vcd_trace_file("ethernetEncoder");

    if (traceFile)
    {
        sca_util::sca_trace(traceFile, data_in_signal, "data_in");
        sca_util::sca_trace(traceFile, mlt3_out_signal, "mlt3_out");
        sca_util::sca_trace(traceFile, cclk, "cclk");
        sca_util::sca_trace(traceFile, data_out_valid, "data_out_valid");
        sca_util::sca_trace(traceFile, data_out, "data_out");
        sca_util::sca_trace(traceFile, pkt_gen.data_in, "pkt_gen_data_in");
        sca_util::sca_trace(traceFile, pkt_gen.data_in_valid, "pkt_gen_data_in_valid");
        sca_util::sca_trace(traceFile, pkt_gen.next_data_in_valid, "pkt_gen_next_data_in_valid");
        sca_util::sca_trace(traceFile, pkt_gen.next_data_out_valid, "pkt_gen_next_data_out_valid");
        sca_util::sca_trace(traceFile, pkt_gen.next_data_out, "pkt_gen_next_data_out");
        sca_util::sca_trace(traceFile, pkt_gen.sigBitCount, "pkt_gen_sigBitCount");
    }

    sc_start(45, SC_NS);
    data[0] = 113;
    data[1] = 255;
    data[2] = 204;
    pkt_gen.fill_data(data, 5);

    sc_start(455, SC_NS); // Run the simulation for 500 ns

    if (traceFile)
    {
        sca_util::sca_close_vcd_trace_file(traceFile);
    }

    return 0;
}
