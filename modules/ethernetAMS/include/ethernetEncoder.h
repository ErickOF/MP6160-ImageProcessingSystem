#ifndef ETHERNET_ENCODER_H
#define ETHERNET_ENCODER_H

#include <systemc-ams.h>
#include <map>
#include <systemc.h>

SCA_TDF_MODULE(InputSignalGenerator)
{
    sca_tdf::sca_out<sc_dt::sc_bv<4>> out;

    SCA_CTOR(InputSignalGenerator) : out("out"), cycle_count(0), bitCount(0)
    {
        set_timestep(10, sc_core::SC_NS); // Same timestep as encoder
    }

    void processing()
    {
        if (bitCount == 0) // Change output only after 5 timesteps (50 ns)
        {
            switch (cycle_count)
            {
                case 0: out.write("0000"); break;
                case 1: out.write("0001"); break;
                case 2: out.write("0010"); break;
                case 3: out.write("0011"); break;
                case 4: out.write("0100"); break;
                case 5: out.write("0101"); break;
                case 6: out.write("0110"); break;
                case 7: out.write("0111"); break;
                case 8: out.write("1000"); break;
                case 9: out.write("1001"); break;
                default: out.write("0000"); break;
            }
            cycle_count = (cycle_count + 1) % 10;
        }
        bitCount = (bitCount + 1) % 5; // Increment and reset bit count every 5 timesteps
    }

private:
    int cycle_count;
    int bitCount;
};

SCA_TDF_MODULE(ethernetEncoder)
{
public:
    sca_tdf::sca_in<sc_dt::sc_bv<4>> data_in;  // 4-bit input
    sca_tdf::sca_out<double> mlt3_out;  // MLT-3 output signal

    sc_dt::sc_bv<5> code_out;  // Internal signal for 5-bit code
    int currentLevel = 0;
    int nextLevel = 0;
    int bitCount = 4;
    int sampleCount = 0;
    int lastMlt3Out = 0;
    std::string lastCodeOut = "";

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
