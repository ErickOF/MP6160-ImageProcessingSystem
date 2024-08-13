#ifndef ETHERNET_DECODER_TLM_CPP
#define ETHERNET_DECODER_TLM_CPP

#include "ethernetDecoder_tlm.hpp"

void ethernetDecoder_tlm::processing()
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

          if ((decode_count < data_length * 2) && (decode_count / 2 < IMAG_ROWS * IMAG_COLS))
          {
            if (decode_count % 2 == 0)
            {
              decoded_byte.range(3, 0) = decoded_bv;
            }
            else
            {
              decoded_byte.range(7, 4) = decoded_bv;
              data[decode_count / 2] = static_cast<unsigned char>(decoded_byte.to_uint());
            }
          }

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

  sample_count++;
}

#endif // ETHERNET_DECODER_TLM_CPP
