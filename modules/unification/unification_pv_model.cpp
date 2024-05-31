//-----------------------------------------------------
//Module: Unification (PV)
//By: Roger Morales Monge
//Description: Programmer's View Model of unification
//process for two pixel values (magnitude)
//-----------------------------------------------------
#include "systemc.h"
#include "math.h"

//NORM SELECTION MACROS
//#define USE_L1_NORM
#define USE_L2_NORM //default
//#define USE_INF_NORM

SC_MODULE (unification_module) {
  
  //-----------Internal variables-------------------
  int x, y;
  int *magnitude;
  
  //-----------Constructor-------------------
  SC_CTOR(unification_module) {
  } // End of Constructor

  //------------Code Starts Here-------------------------
  void unificate(int x, int y, int* magnitude) {
    this->x = x;
    this->y = y;
    this->magnitude = magnitude;
    
    //Get the Norm
    *(this->magnitude) = norm(this->x, this->y);
    
    //Values are 1byte -> 0 to 255
    *(this->magnitude) = min(255,*(this->magnitude));
  }
  
  int norm(int a, int b) {
    int norm_result = 0;
    
    //L1 Norm
    #ifdef USE_L1_NORM
    norm_result = abs(a) + abs(b);
  	#endif
  
    //L2 Norm
    #ifdef USE_L2_NORM
  	norm_result = sqrt(pow(a, 2) + pow(b, 2));
  	#endif
    
    //INFINITY Norm
    #ifdef USE_INF_NORM
    norm_result = max(a, b);
  	#endif
    
    return norm_result;
  }
  
  //maximum and minimum functions
  int max(int a, int b){
    return (a > b ? a : b);
  }
  int min(int a, int b){
    return (a < b ? a : b);
  }
}; 

