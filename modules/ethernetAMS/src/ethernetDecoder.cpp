#ifndef ETHERNET_DECODER_CPP
#define ETHERNET_DECODER_CPP

#include "ethernetDecoder.h"

void ethernetDecoder::set_attributes()
{
    set_timestep(10, sc_core::SC_NS); // Set a sample period of 10 ns
}

void ethernetDecoder::initialize()
{
    previous_level = 0;
    current_level = 0;
    bit_count = 0;
    sample_count = 0;
    found_sequence = false;
    bit_sequence.clear();

    // Reset new members
    received_first_eight_decodes = false;
    data_length = 0;
    decode_count = 0;

    // Initialize 5B/4B decoding map
    decoding_map = {
        {"11110", "0000"}, {"01001", "0001"}, {"10100", "0010"},
        {"10101", "0011"}, {"01010", "0100"}, {"01011", "0101"},
        {"01110", "0110"}, {"01111", "0111"}, {"10010", "1000"},
        {"10011", "1001"}, {"10110", "1010"}, {"10111", "1011"},
        {"11010", "1100"}, {"11011", "1101"}, {"11100", "1110"},
        {"11101", "1111"}
    };
}

bool ethernetDecoder::check_sequence(const std::deque<char>& sequence)
{
    int target_count = 0;
    std::string bit_str(sequence.begin(), sequence.end());

    for (size_t i = 0; i <= bit_str.size() - 5; ++i)
    {
        std::string current_bits = bit_str.substr(i, 5);

        if (current_bits == target_sequence)
        {
            target_count++;
        }
        else if (target_count == 15 && current_bits == end_sequence)
        {
            return true;
        }
    }

    return false;
}

void ethernetDecoder::processing()
{
    current_level = static_cast<int>(mlt3_in.read());

    if (found_sequence && bit_sequence.size() >= 5)
    {
        bit_sequence.clear();
    }

    // MLT-3 to binary conversion based on level change
    if (current_level != previous_level)
    {
        bit_sequence.push_back('1');
    }
    else
    {
        bit_sequence.push_back('0');
    }

    if (bit_sequence.size() > 80)
    {
        bit_sequence.pop_front();
    }

    previous_level = current_level;
    bit_count++;

    if (found_sequence && bit_sequence.size() >= 5)
    {
        std::string last_five_bits(bit_sequence.end() - 5, bit_sequence.end());

        // 5B/4B decoding
        if (decoding_map.find(last_five_bits) != decoding_map.end())
        {
            std::string decoded_bits = decoding_map[last_five_bits];
            sc_dt::sc_bv<4> decoded_bv;
            decoded_bv = decoded_bits.c_str(); // Assign the value using operator=

            // Store the first eight decodes
            if (!received_first_eight_decodes)
            {
                first_eight_decodes[decode_count] = decoded_bv;
                decode_count++;
                if (decode_count == 8)
                {
                    data_length = (first_eight_decodes[7].to_uint() << 28) |
                                  (first_eight_decodes[6].to_uint() << 24) |
                                  (first_eight_decodes[5].to_uint() << 20) |
                                  (first_eight_decodes[4].to_uint() << 16) |
                                  (first_eight_decodes[3].to_uint() << 12) |
                                  (first_eight_decodes[2].to_uint() << 8) |
                                  (first_eight_decodes[1].to_uint() << 4) |
                                  first_eight_decodes[0].to_uint();
                    received_first_eight_decodes = true;
                    std::cout << "/////////////////////////////////Data length: " << std::hex << data_length << std::endl;

                    decode_count = 0;
                }
            }
            else
            {
                data_out.write(decoded_bv);
                decode_count++;
                if (decode_count >= data_length * 2)
                {
                    // Reset state to wait for the sequence again
                    found_sequence = false;
                    received_first_eight_decodes = false;
                    decode_count = 0;
                    data_length = 0;
                }
            }
            bit_sequence.erase(bit_sequence.begin(), bit_sequence.begin() + 5);
        }
        else
        {
            std::cout << "last_five_bits:" << last_five_bits;

            std::cout << "********Sample: " << sample_count << ", mlt3_in: " << current_level 
                      << ", bit_sequence: " << std::string(bit_sequence.begin(), bit_sequence.end()) 
                      << ", found_sequence: " << found_sequence << std::endl;
            SC_REPORT_ERROR("ethernetDecoder", "Invalid input for decoding");
        }
    }

    if (bit_sequence.size() == 80 && !found_sequence)
    {
        found_sequence = check_sequence(bit_sequence);
    }

    // Debugging output
    std::cout << "Sample: " << sample_count << ", mlt3_in: " << current_level 
              << ", bit_sequence: " << std::string(bit_sequence.begin(), bit_sequence.end()) 
              << ", found_sequence: " << found_sequence << std::endl;

    sample_count++;
    // Set the next trigger
    next_trigger(10, SC_NS);
}

#endif // ETHERNET_DECODER_CPP
