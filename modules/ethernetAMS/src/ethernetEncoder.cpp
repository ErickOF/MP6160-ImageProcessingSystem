#ifndef ETHERNET_ENCODER_CPP
#define ETHERNET_ENCODER_CPP

#include "ethernetEncoder.h"

void ethernetEncoder::set_attributes()
{
    // Set a timestep for the TDF module
    set_timestep(sample_time);
}

void ethernetEncoder::initialize()
{
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
    // Update mlt3_out with the value calculated in the previous cycle
    mlt3_out.write(next_mlt3_out);

    if (!valid.read())  // Check the valid and clock edge signals before processing
    {
        return;
    }

    std::string input = data_in.read().to_string();

    if (bitCount == 0) // Process new input only when bitCount is 4 (rightmost bit)
    {
#ifndef USING_TLM_TB_EN
        std::cout << "Processing sample: " << sampleCount << ", received input: " << input << std::endl;
#endif // USING_TLM_TB_EN

        // 4B/5B Encoding
        if (encoding_map.find(input) != encoding_map.end())
        {
            lastCodeOut = encoding_map[input];
        }
        else
        {
            SC_REPORT_ERROR("ethernetEncoder", "Invalid input for encoding");
            return;
        }
    }

    // MLT-3 Encoding
    if (lastCodeOut[bitCount] == '1')
    {
        if (currentLevel == 0)
        {
            currentLevel = nextLevel;
            nextLevel = (currentLevel == 1) ? -1 : 1;
        }
        else
        {
            currentLevel = 0;
        }
    }

    lastMlt3Out = currentLevel;
    next_mlt3_out = currentLevel;  // Store the computed value for the next cycle
    bitCount = (bitCount == 4) ? 0 : bitCount + 1; // Decrement bitCount from 4 to 0

#ifndef USING_TLM_TB_EN
    // Debugging output
    std::cout << "Sample: " << sampleCount << ", data_in: " << input 
              << ", code_out: " << lastCodeOut << ", mlt3_out: " << next_mlt3_out << std::endl;
#endif // USING_TLM_TB_EN

    sampleCount++;
}

#endif // ETHERNET_ENCODER_CPP
