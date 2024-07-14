#ifndef ETHERNET_ENCODER_H
#define ETHERNET_ENCODER_H

#include <systemc-ams.h>
#include <map>
#include <systemc.h>

SCA_TDF_MODULE(InputSignalGenerator)
{
public:
    sca_tdf::sca_out<sc_dt::sc_bv<4>> out; // Output port

    SCA_CTOR(InputSignalGenerator) : out("out"), cycle(0)
    {
    }

    void set_attributes()
    {
        // Set the output port rate
        out.set_rate(1);
    }

    void processing()
    {
        if (cycle == 0)
            out.write("0000");
        else if (cycle == 1)
            out.write("1111");

        cycle++;
    }

private:
    int cycle;
};

SCA_TDF_MODULE(ethernetEncoder)
{
public:
    sca_tdf::sca_in<sc_dt::sc_bv<4>> data_in;  // 4-bit input
    sca_tdf::sca_out<double> mlt3_out;  // MLT-3 output signal

    sc_dt::sc_bv<5> code_out;  // Internal signal for 5-bit code
    int current_level;
    int next_level;
    int bit_count;

    std::map<std::string, std::string> encoding_map;

    void set_attributes();
    void initialize();
    void processing();

    SCA_CTOR(ethernetEncoder)
    : data_in("data_in"), mlt3_out("mlt3_out")
    {
    }
};

#endif // ETHERNET_ENCODER_H
