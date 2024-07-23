#ifndef IPS_MEMORY_HPP
#define IPS_MEMORY_HPP
#include <systemc.h>

template <unsigned int SIZE>
SC_MODULE(memory)
{
protected:
  int *mem;

public:
  sc_core::sc_in<bool> clk;
  sc_core::sc_in<bool> we;
  sc_core::sc_in<unsigned long long int> address;
  sc_core::sc_in<sc_uint<24>> wdata;
  sc_core::sc_out<sc_uint<24>> rdata;

  // Constructor for memory
  SC_CTOR(memory)
  {
    this->mem = new int[SIZE];

    SC_METHOD(run);
    sensitive << clk.pos();
  }

  void run()
  {
    if (clk.read())
    {
      const unsigned long long int ADDR = static_cast<unsigned long long int>(this->address.read());

      if (we.read())
      {
        this->mem[ADDR] = this->wdata.read();
      }

      this->rdata.write(this->mem[ADDR]);
    }
  }
};
#endif // IPS_MEMORY_HPP
