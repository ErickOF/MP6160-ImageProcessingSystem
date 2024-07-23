#ifndef PACKET_GENERATOR_CPP
#define PACKET_GENERATOR_CPP

#include <systemc-ams.h>
#include <systemc.h>
#include "packetGenerator.h"

void packetGenerator::set_attributes()
{
    // Set a timestep for the TDF module
    set_timestep(sample_time);
}

void packetGenerator::fill_data(unsigned char* data, int packet_length)
{
  local_data = new unsigned char[packet_length];
  memcpy(local_data, data, packet_length * sizeof(char));

  actual_data_length = packet_length;
  bytes_sent = 0;

  // Insert first the preamble
  create_pack_of_data(preamble_data, 8);

  preamble_in_process = true;
}

void packetGenerator::create_pack_of_data(unsigned char* data, int packet_length)
{
  int actual_length = (packet_length * 2 > N) ? N : packet_length * 2;
  unsigned char tmp_data;
  sc_dt::sc_bv<N * 4> tmp_data_in = 0;
  sc_dt::sc_bv<N> tmp_data_valid = 0;

  for (int i = 0; i < actual_length; i++)
  {
    tmp_data = *(data + (i/2));
    if ((i % 2) == 1)
    {
      tmp_data = tmp_data >> 4;
    }
    tmp_data_in.range(i * 4 + 3, i * 4) = tmp_data;
    tmp_data_valid[i] = 1;
  }

  data_to_send = tmp_data_in;
  data_valid_to_send = tmp_data_valid;

  n2_data_valid = data_valid_to_send;
  n1_data_valid = n2_data_valid;
}

void packetGenerator::processing()
{
  sc_dt::sc_bv<N * 4> tmp_data_to_send = data_to_send;
  sc_dt::sc_bv<N> tmp_data_valid_to_send = data_valid_to_send;

  bool manual_update = false;

  if ((tmp_data_valid_to_send.or_reduce() == 0) && (preamble_in_process == true))
  {
    sc_dt::sc_bv<32> local_length = 0;

    preamble_in_process = false;

    local_length = (unsigned int)actual_data_length;

    data_to_send = 0;
    data_to_send.range(31, 0) = local_length;
    data_valid_to_send = "11111111";
    tmp_data_to_send = data_to_send;
    tmp_data_valid_to_send = data_valid_to_send;

    n2_data_valid = data_valid_to_send;
    n1_data_valid = n2_data_valid;
  }
  else if ((tmp_data_valid_to_send.or_reduce() == 0) && (actual_data_length > 0))
  {
    if (actual_data_length > 8)
    {
      create_pack_of_data((local_data + bytes_sent), 8);
      actual_data_length -= 8;
      bytes_sent += 8;
    }
    else
    {
      create_pack_of_data((local_data + bytes_sent), actual_data_length);
      actual_data_length = 0;
      delete[] local_data;
    }
  }

  n1_data_out = n2_data_out;
  n1_data_out_valid = n2_data_out_valid;
  n1_data_valid = n2_data_valid;

  if (tmp_data_valid_to_send.or_reduce())
  {
    for (int i = 0; i < N; i++)
    {
      if (tmp_data_valid_to_send[i] == 1)
      {
        if ((bitCount == 0) && (tmp_data_out_valid == false) && (n1_data_out_valid == false))
        {
          tmp_data_valid_to_send[i] = 0;
          n2_data_out = tmp_data_to_send.range(i * 4 + 3, i * 4);
          n2_data_out_valid = true;
          n2_data_valid = tmp_data_valid_to_send;
          std::cout << "@" << sc_time_stamp() << " Inside generate_packet(): data to sent " << n2_data_out << std::endl;
          break;
        }
        else if ((bitCount == 0) && (tmp_data_out_valid == true))
        {
          tmp_data_valid_to_send[i] = 0;
          data_out.write(tmp_data_to_send.range(i * 4 + 3, i * 4));
          data_out_valid.write(1);
          tmp_data_out_valid = true;
          data_valid_to_send_ = tmp_data_valid_to_send;
          n2_data_out = tmp_data_to_send.range(i * 4 + 3, i * 4);
          n2_data_out_valid = true;
          n2_data_valid = tmp_data_valid_to_send;
          n1_data_out = n2_data_out;
          n1_data_out_valid = n2_data_out_valid;
          n1_data_valid = n2_data_valid;
          manual_update = true;
          std::cout << "@" << sc_time_stamp() << " Inside generate_packet(): data to sent " << n2_data_out << std::endl;
          break;
        }
      }
    }
  }

  if (!manual_update)
  {
    data_out_valid.write(n1_data_out_valid);
    tmp_data_out_valid = n1_data_out_valid;
  }

  if (tmp_data_out_valid == true)
  {
    bitCount++;
  }

  if (bitCount == 5)
  {
    bitCount = 0;
    if (!manual_update)
    {
      n2_data_out_valid = false;
      n1_data_out_valid = n2_data_out_valid;
      data_out_valid.write(true);
      tmp_data_out_valid = true;
    }
  }

  if (!manual_update)
  {
    data_valid_to_send = n1_data_valid;
    data_out.write(n1_data_out);
  }

  n1_sigBitCount = n2_sigBitCount;
  n2_sigBitCount = bitCount;
  n1_sigBitCount_.write(n1_sigBitCount);
  n2_sigBitCount_.write(n2_sigBitCount);
  sigBitCount.write(n1_sigBitCount);

  tmp_data_out_valid_.write(tmp_data_out_valid);

  n2_data_out_valid_.write(n2_data_out_valid);
  n2_data_out_.write(n2_data_out);
  n2_data_valid_.write(n2_data_valid);

  n1_data_out_valid_.write(n1_data_out_valid);
  n1_data_out_.write(n1_data_out);
  n1_data_valid_.write(n1_data_valid);

  data_in_.write(data_in);
  data_in_valid_.write(data_in_valid);

  data_to_send_.write(data_to_send);
  data_valid_to_send_.write(data_valid_to_send);

  remaining_bytes_to_send.write(actual_data_length);
}

#endif // PACKET_GENERATOR_CPP
