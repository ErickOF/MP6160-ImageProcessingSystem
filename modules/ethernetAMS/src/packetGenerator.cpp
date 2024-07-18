#include "packetGenerator.h"

void packetGenerator::fill_data(unsigned char* data, int nibbles_length)
{
  int actual_length = (nibbles_length > N) ? N : nibbles_length;
  unsigned char tmp_data;
  sc_dt::sc_bv<N * 4> tmp_data_in = data_in.read();
  sc_dt::sc_bv<N> tmp_data_valid = data_in_valid.read();
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
  data_in.write(tmp_data_in);
  data_in_valid.write(tmp_data_valid);
  next_data_in_valid.write(tmp_data_valid);
}

void packetGenerator::generate_packet()
{
  sc_dt::sc_bv<N * 4> tmp_data_in = data_in.read();
  sc_dt::sc_bv<N> tmp_data_valid = data_in_valid.read();
  
  bool manual_update = false;
  
  if (tmp_data_valid.or_reduce())
  {
    for (int i = 0; i < N; i++)
    {
      if (tmp_data_valid[i] == 1)
      {
        if ((bitCount == 0) && (next_data_out_valid.read() == 0))
        {
          tmp_data_valid[i] = 0;
          next_data_out.write(tmp_data_in.range(i * 4 + 3, i * 4));
          next_data_out_valid.write(1);
          next_data_in_valid.write(tmp_data_valid);
          break;
        }
        else if (bitCount == 4)
        {
          tmp_data_valid[i] = 0;
          data_out.write(tmp_data_in.range(i * 4 + 3, i * 4));
          data_out_valid.write(1);
          data_in_valid.write(tmp_data_valid);
          next_data_out.write(tmp_data_in.range(i * 4 + 3, i * 4));
          next_data_out_valid.write(1);
          next_data_in_valid.write(tmp_data_valid);
          manual_update = true;
          break;
        }
      }
    }
  }

  if (data_out_valid.read() == 1)
  {
    bitCount++;
  }

  if(bitCount == 5)
  {
    bitCount = 0;
    if (manual_update == false)
    {
      next_data_out_valid.write(0);
      data_out_valid.write(0);
    }
  }
  else if (manual_update == false)
  {
    data_out_valid.write(next_data_out_valid.read());
  }

  if (manual_update == false)
  {
    data_in_valid.write(next_data_in_valid.read());
    data_out.write(next_data_out.read());
  }

  sigBitCount.write(bitCount);
}
