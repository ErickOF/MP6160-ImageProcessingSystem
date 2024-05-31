//--------------------------------------------------------
//Testbench: Unification
//By: Roger Morales Monge
//Description: Simple TB for pixel unification modules
//--------------------------------------------------------
#include <systemc.h>
#include "unification_pv_model.cpp"
//#include "unification_lt_model.cpp"
//#include "unification_at_model.cpp"


int sc_main (int argc, char* argv[]) {
              
  int pixel_x, pixel_y;
  int pixel_magnitude;
  
  unification_module unification_U1 ("unification_U1");
  
  // Open VCD file
  sc_trace_file *wf = sc_create_vcd_trace_file("unification_U1");
  wf->set_time_unit(1, SC_NS);
  
  // Dump the desired signals
  sc_trace(wf, pixel_x, "pixel_x");
  sc_trace(wf, pixel_y, "pixel_y");
  sc_trace(wf, pixel_magnitude, "pixel_magnitude");
  
  sc_start();
  cout << "@" << sc_time_stamp()<< endl;
  
  printf("Writing in zero time\n");
  
  pixel_x = 212;
  pixel_y = 95;
  printf("Operands: pixel_x = %0d, pixel_y = %0d\n", pixel_x, pixel_y);
  unification_U1.unificate(pixel_x, pixel_y, &pixel_magnitude);
  printf("RESULT: pixel_magnitude = %d\n", pixel_magnitude);
  
  //FIXME: ADD more cases here -> img
    
  cout << "@" << sc_time_stamp() <<" Terminating simulation\n" << endl;
  sc_close_vcd_trace_file(wf);
  return 0;// Terminate simulation

 }
