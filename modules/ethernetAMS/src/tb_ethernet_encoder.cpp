#include <systemc-ams.h>
#include <systemc.h>
#include "ethernetEncoder.h"
#include "packetGenerator.h"

int sc_main(int argc, char* argv[])
{
    sca_tdf::sca_signal<bool> valid;
    sca_tdf::sca_signal<double> mlt3_out_signal;

    sca_tdf::sca_signal<bool> data_out_valid;
    sca_tdf::sca_signal<sc_dt::sc_bv<4>> data_out;

    sca_tdf::sca_signal<bool> tmp_data_out_valid;

    sca_tdf::sca_signal<bool> n2_data_out_valid;
    sca_tdf::sca_signal<sc_dt::sc_bv<4>> n2_data_out;
    sca_tdf::sca_signal<sc_dt::sc_bv<16>> n2_data_in_valid;
    sca_tdf::sca_signal<bool> n1_data_out_valid;
    sca_tdf::sca_signal<sc_dt::sc_bv<4>> n1_data_out;
    sca_tdf::sca_signal<sc_dt::sc_bv<16>> n1_data_in_valid;

    sca_tdf::sca_signal<sc_dt::sc_bv<64>> data_in;
    sca_tdf::sca_signal<sc_dt::sc_bv<16>> data_in_valid;
    sca_tdf::sca_signal<sc_dt::sc_int<4>> n1_sigBitCount;
    sca_tdf::sca_signal<sc_dt::sc_int<4>> n2_sigBitCount;
    sca_tdf::sca_signal<sc_dt::sc_int<4>> sigBitCount;


    unsigned char data[8];

    ethernetEncoder encoder("encoder");
    packetGenerator pkt_gen("pkt_gen");

    encoder.data_in(data_out);
    encoder.mlt3_out(mlt3_out_signal);
    encoder.valid(data_out_valid);

    pkt_gen.data_out_valid(data_out_valid);
    pkt_gen.data_out(data_out);

    pkt_gen.tmp_data_out_valid_(tmp_data_out_valid);

    pkt_gen.n2_data_out_valid_(n2_data_out_valid);
    pkt_gen.n2_data_out_(n2_data_out);
    pkt_gen.n2_data_in_valid_(n2_data_in_valid);

    pkt_gen.n1_data_out_valid_(n1_data_out_valid);
    pkt_gen.n1_data_out_(n1_data_out);
    pkt_gen.n1_data_in_valid_(n1_data_in_valid);

    pkt_gen.data_in_(data_in);
    pkt_gen.data_in_valid_(data_in_valid);
    pkt_gen.n1_sigBitCount_(n1_sigBitCount);
    pkt_gen.n2_sigBitCount_(n2_sigBitCount);
    pkt_gen.sigBitCount(sigBitCount);

    // Trace file setup
    sca_util::sca_trace_file *traceFile = sca_util::sca_create_vcd_trace_file("ethernetEncoder");

    if (traceFile)
    {
        sca_util::sca_trace(traceFile, mlt3_out_signal, "mlt3_out");
        sca_util::sca_trace(traceFile, data_out_valid, "data_out_valid");
        sca_util::sca_trace(traceFile, data_out, "data_out");
        sca_util::sca_trace(traceFile, data_in, "pkt_gen_data_in");
        sca_util::sca_trace(traceFile, data_in_valid, "pkt_gen_data_in_valid");
        sca_util::sca_trace(traceFile, n2_data_in_valid, "pkt_gen_n2_data_in_valid");
        sca_util::sca_trace(traceFile, n2_data_out_valid, "pkt_gen_n2_data_out_valid");
        sca_util::sca_trace(traceFile, n2_data_out, "pkt_gen_n2_data_out");
        sca_util::sca_trace(traceFile, n1_data_in_valid, "pkt_gen_n1_data_in_valid");
        sca_util::sca_trace(traceFile, n1_data_out_valid, "pkt_gen_n1_data_out_valid");
        sca_util::sca_trace(traceFile, n1_data_out, "pkt_gen_n1_data_out");
        sca_util::sca_trace(traceFile, sigBitCount, "pkt_gen_sigBitCount");
        sca_util::sca_trace(traceFile, n1_sigBitCount, "pkt_gen_n1_sigBitCount");
        sca_util::sca_trace(traceFile, n2_sigBitCount, "pkt_gen_n2_sigBitCount");
        sca_util::sca_trace(traceFile, tmp_data_out_valid, "pkt_gen_tmp_data_out_valid");
    }

    sc_start(45, SC_NS);
    data[0] = 113;
    data[1] = 255;
    data[2] = 204;
    pkt_gen.fill_data(data, 3);

    sc_start(455, SC_NS); // Run the simulation for 500 ns

    if (traceFile)
    {
        sca_util::sca_close_vcd_trace_file(traceFile);
    }

    return 0;
}
