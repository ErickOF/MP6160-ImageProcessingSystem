#ifndef ETHERNET_ENCODER_H
#define ETHERNET_ENCODER_H

#include <systemc-ams.h>
#include <map>
#include <systemc.h>

SCA_TDF_MODULE(ethernetEncoder)
{
public:
    sca_tdf::sca_in<sc_dt::sc_bv<4>> data_in;  // 4-bit input
    sca_tdf::sca_out<double> mlt3_out;  // MLT-3 output signal
    sca_tdf::sca_in<bool> valid; // Valid signal

    sc_dt::sc_bv<5> code_out;  // Internal signal for 5-bit code
    int currentLevel = 0;
    int nextLevel = 1;
    int bitCount = 4;
    int sampleCount = 0;
    int lastMlt3Out = 0;
    std::string lastCodeOut = "";

    double next_mlt3_out = 0;  // Variable to store the next MLT-3 output value

    std::map<std::string, std::string> encoding_map;

    sca_core::sca_time sample_time;

    void set_attributes();
    void initialize();
    void processing();

    ethernetEncoder(sc_core::sc_module_name name, sca_core::sca_time sample_time)
    : sca_tdf::sca_module(name), data_in("data_in"), mlt3_out("mlt3_out"), valid("valid"), sample_time(sample_time)
    {
    }
};

#endif // ETHERNET_ENCODER_H
