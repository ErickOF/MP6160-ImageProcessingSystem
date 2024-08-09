#include <systemc.h>

SC_MODULE (testbench) {
	sc_out<sc_uint<8> > img_window[9];
	sc_in<sc_uint<8> > mean;

	SC_CTOR (testbench) {
		SC_THREAD(process);
	}

	void process() {
		for (int i = 0; i < 9; i++){
			img_window[i] = 0;
		}
		wait(10, SC_NS);
		std::cout << "Result: " << mean << std::endl;
		std::cout << "HERE" << std::endl;
		for (int i = 0; i < 9; i++){
			img_window[i] = i;
		}
		wait(10, SC_NS);
		std::cout << "Result: " << mean << std::endl;
		sc_stop();
	}
};
