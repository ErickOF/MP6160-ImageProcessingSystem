#ifndef USING_TLM_TB_EN

#ifndef TB_ETHERNET_ENCODER_CPP
#define TB_ETHERNET_ENCODER_CPP

#include <systemc-ams.h>
#include <systemc.h>
#include "ethernetEncoder.h"
#include "packetGenerator.h"
#include "ethernetDecoder.h"

int sc_main(int argc, char* argv[])
{
    sca_tdf::sca_signal<bool> valid;
    sca_tdf::sca_signal<double> mlt3_out_signal;

    sca_tdf::sca_signal<bool> data_out_valid;
    sca_tdf::sca_signal<sc_dt::sc_bv<4>> data_out_signal;
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

    sca_core::sca_time sample_time(10, SC_NS);

    unsigned char data[16];

    ethernetEncoder encoder("encoder", sample_time);
    ethernetDecoder decoder("decoder");
    packetGenerator pkt_gen("pkt_gen", sample_time);

    encoder.data_in(data_out);
    encoder.mlt3_out(mlt3_out_signal);
    decoder.mlt3_in(mlt3_out_signal);
    decoder.data_out(data_out_signal);
    encoder.valid(data_out_valid);

    pkt_gen.data_out_valid(data_out_valid);
    pkt_gen.data_out(data_out);

    pkt_gen.tmp_data_out_valid_(tmp_data_out_valid);

    pkt_gen.n2_data_out_valid_(n2_data_out_valid);
    pkt_gen.n2_data_out_(n2_data_out);
    pkt_gen.n2_data_valid_(n2_data_valid);

    pkt_gen.n1_data_out_valid_(n1_data_out_valid);
    pkt_gen.n1_data_out_(n1_data_out);
    pkt_gen.n1_data_valid_(n1_data_valid);

    pkt_gen.data_in_(data_in);
    pkt_gen.data_in_valid_(data_in_valid);

    pkt_gen.data_to_send_(data_to_send);
    pkt_gen.data_valid_to_send_(data_valid_to_send);

    pkt_gen.n1_sigBitCount_(n1_sigBitCount);
    pkt_gen.n2_sigBitCount_(n2_sigBitCount);
    pkt_gen.sigBitCount(sigBitCount);

    pkt_gen.remaining_bytes_to_send(remaining_bytes_to_send);

    // Trace file setup
    sca_util::sca_trace_file *traceFile = sca_util::sca_create_vcd_trace_file("ethernetEncoder");

    if (traceFile)
    {
        sca_util::sca_trace(traceFile, mlt3_out_signal, "mlt3_out");
        sca_util::sca_trace(traceFile, data_out_signal, "data_out_signal");
        sca_util::sca_trace(traceFile, data_out_valid, "data_out_valid");
        sca_util::sca_trace(traceFile, data_out, "data_out");
        sca_util::sca_trace(traceFile, data_in, "pkt_gen_data_in");
        sca_util::sca_trace(traceFile, data_in_valid, "pkt_gen_data_in_valid");
        sca_util::sca_trace(traceFile, n2_data_valid, "pkt_gen_n2_data_valid");
        sca_util::sca_trace(traceFile, n2_data_out_valid, "pkt_gen_n2_data_out_valid");
        sca_util::sca_trace(traceFile, n2_data_out, "pkt_gen_n2_data_out");
        sca_util::sca_trace(traceFile, n1_data_valid, "pkt_gen_n1_data_valid");
        sca_util::sca_trace(traceFile, n1_data_out_valid, "pkt_gen_n1_data_out_valid");
        sca_util::sca_trace(traceFile, n1_data_out, "pkt_gen_n1_data_out");
        sca_util::sca_trace(traceFile, sigBitCount, "pkt_gen_sigBitCount");
        sca_util::sca_trace(traceFile, n1_sigBitCount, "pkt_gen_n1_sigBitCount");
        sca_util::sca_trace(traceFile, n2_sigBitCount, "pkt_gen_n2_sigBitCount");
        sca_util::sca_trace(traceFile, tmp_data_out_valid, "pkt_gen_tmp_data_out_valid");
        sca_util::sca_trace(traceFile, data_to_send, "pkt_gen_data_to_send");
        sca_util::sca_trace(traceFile, data_valid_to_send, "pkt_gen_data_valid_to_send");
        sca_util::sca_trace(traceFile, remaining_bytes_to_send, "pkt_gen_remaining_bytes_to_send");
    }

    sc_start(45, SC_NS);
    data[0] = 113;
    data[1] = 255;
    data[2] = 204;
    data[3] = 85;
    data[4] = 173;
    data[5] = 37;
    data[6] = 142;
    data[7] = 69;
    data[8] = 91;
    data[9] = 178;
    data[10] = 220;
    data[11] = 33;
    data[12] = 77;
    data[13] = 111;
    data[14] = 146;
    data[15] = 194;
    pkt_gen.fill_data(data, 13);

    sc_start(3455, SC_NS); // Run the simulation for 500 ns

    pkt_gen.fill_data(data, 8);

    sc_start(3500, SC_NS); // Run the simulation for 500 ns

    if (traceFile)
    {
        sca_util::sca_close_vcd_trace_file(traceFile);
    }

    return 0;
}

#endif // TB_ETHERNET_ENCODER_CPP

#endif // USING_TLM_TB_EN
