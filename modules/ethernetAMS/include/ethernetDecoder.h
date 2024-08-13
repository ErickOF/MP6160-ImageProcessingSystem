#ifndef ETHERNET_DECODER_H
#define ETHERNET_DECODER_H

#include <systemc-ams.h>
#include <map>
#include <systemc.h>
#include <deque>

SCA_TDF_MODULE(ethernetDecoder)
{
public:
    sca_tdf::sca_in<double> mlt3_in;  // MLT-3 input signal
    sca_tdf::sca_out<sc_dt::sc_bv<4>> data_out;  // 4-bit output

    std::map<std::string, std::string> decoding_map;

    void set_attributes();
    void initialize();
    void processing();

    SCA_CTOR(ethernetDecoder)
    : mlt3_in("mlt3_in"), data_out("data_out"), previous_level(0), bit_count(0), sample_count(0), found_sequence(false),
      received_first_eight_decodes(false), data_length(0), decode_count(0)
    {
    }

private:
    int previous_level;
    int current_level;
    int bit_count;
    int sample_count;
    std::deque<char> bit_sequence;
    bool found_sequence;
    const std::string target_sequence = "10110";
    const std::string end_sequence = "10111";

    bool check_sequence(const std::deque<char>& sequence);

    // New members
    bool received_first_eight_decodes;
    int data_length;
    sc_dt::sc_bv<4> first_eight_decodes[8];
    int decode_count;
};

#endif // ETHERNET_DECODER_H
