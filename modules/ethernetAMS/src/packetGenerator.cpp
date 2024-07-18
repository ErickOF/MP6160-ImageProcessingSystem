#include <systemc-ams.h>
#include <systemc.h>
#include "packetGenerator.h"


void packetGenerator::fill_data(unsigned char* data, int packet_length)
{
  int actual_length = (packet_length * 2 > N) ? N : packet_length * 2;
  unsigned char tmp_data;
  sc_dt::sc_bv<N * 4> tmp_data_in = data_in;  // Ensure data_in is properly connected
  sc_dt::sc_bv<N> tmp_data_valid = data_in_valid;  // Ensure data_in_valid is properly connected

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

  data_in = tmp_data_in;
  data_in_valid = tmp_data_valid;
  n1_data_in_valid = tmp_data_valid;
  n2_data_in_valid = tmp_data_valid;
}

void packetGenerator::processing()
{
  generate_packet();
}

void packetGenerator::generate_packet()
{
  sc_dt::sc_bv<N * 4> tmp_data_in = data_in;
  sc_dt::sc_bv<N> tmp_data_valid = data_in_valid;

  bool manual_update = false;

  n1_data_out = n2_data_out;
  n1_data_out_valid = n2_data_out_valid;
  n1_data_in_valid = n2_data_in_valid;

  if (tmp_data_valid.or_reduce())
  {
    for (int i = 0; i < N; i++)
    {
      if (tmp_data_valid[i] == 1)
      {
        if ((bitCount == 0) && (tmp_data_out_valid == false) && (n1_data_out_valid == false))
        {
          tmp_data_valid[i] = 0;
          n2_data_out = tmp_data_in.range(i * 4 + 3, i * 4);
          n2_data_out_valid = true;
          n2_data_in_valid = tmp_data_valid;
          std::cout << "@" << sc_time_stamp() << " Inside generate_packet(): data to sent " << n2_data_out << std::endl;
          break;
        }
        else if ((bitCount == 0) && (tmp_data_out_valid == true))
        {
          tmp_data_valid[i] = 0;
          data_out.write(tmp_data_in.range(i * 4 + 3, i * 4));
          data_out_valid.write(1);
          tmp_data_out_valid = true;
          data_in_valid = tmp_data_valid;
          n2_data_out = tmp_data_in.range(i * 4 + 3, i * 4);
          n2_data_out_valid = true;
          n2_data_in_valid = tmp_data_valid;
          n1_data_out = n2_data_out;
          n1_data_out_valid = n2_data_out_valid;
          n1_data_in_valid = n2_data_in_valid;
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
    data_in_valid = n1_data_in_valid;
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
  n2_data_in_valid_.write(n2_data_in_valid);

  n1_data_out_valid_.write(n1_data_out_valid);
  n1_data_out_.write(n1_data_out);
  n1_data_in_valid_.write(n1_data_in_valid);

  data_in_.write(data_in);
  data_in_valid_.write(data_in_valid);
}
