#include "ethernetEncoder.h"

void ethernetEncoder::set_attributes()
{
    set_timestep(10, sc_core::SC_NS); // Set a sample period of 10 ns
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
    std::string input = data_in.read().to_string();

    if (bitCount == 4) // Process new input only when bitCount is 4 (rightmost bit)
    {
        std::cout << "Processing sample: " << sampleCount << ", received input: " << input << std::endl;

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
    mlt3_out.write(currentLevel);
    bitCount = (bitCount == 0) ? 4 : bitCount - 1; // Decrement bitCount from 4 to 0

    // Debugging output
    std::cout << "Sample: " << sampleCount << ", data_in: " << input 
              << ", code_out: " << lastCodeOut << ", mlt3_out: " << lastMlt3Out << std::endl;

    sampleCount++;
    // Set the next trigger
    next_trigger(10, SC_NS);
}
