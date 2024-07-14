#include "ethernetEncoder.h"

void ethernetEncoder::set_attributes()
{
    set_timestep(10, sc_core::SC_NS); // Set a sample period of 10 ns
}

void ethernetEncoder::initialize()
{
    current_level = 0;
    next_level = 0;
    bit_count = 0;

    // Initialize 4B/5B encoding map
    encoding_map = {
        {"0000", "11110"}, {"0001", "01001"}, {"0010", "10100"},
        {"0011", "10101"}, {"0100", "01010"}, {"0101", "01011"},
        {"0110", "01110"}, {"0111", "01111"}, {"1000", "10010"},
        {"1001", "10011"}, {"1010", "10110"}, {"1011", "10111"},
        {"1100", "11010"}, {"1101", "11011"}, {"1110", "11100"},
        {"1111", "11101"}
    };
}

void ethernetEncoder::processing()
{
    // 4B/5B Encoding
    std::string input = data_in.read().to_string();
    if (encoding_map.find(input) != encoding_map.end())
    {
        code_out = encoding_map[input].c_str();
    }
    else
    {
        SC_REPORT_ERROR("ethernetEncoder", "Invalid input for encoding");
        return;
    }

    // MLT-3 Encoding
    if (code_out[bit_count] == '1')
    {
        if (current_level == 0)
        {
            next_level = (bit_count % 2 == 0) ? 1 : -1;
        }
        else
        {
            next_level = 0;
        }
        current_level = next_level;
    }
    bit_count = (bit_count + 1) % 5;
    mlt3_out.write(current_level);

    // Debugging output
    std::cout << "data_in: " << input << ", code_out: " << code_out << ", mlt3_out: " << current_level << std::endl;

    // Set the next trigger
    next_trigger(10, SC_NS);
}

