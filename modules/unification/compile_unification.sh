#!/bin/sh
echo 'Compiling *.c *cpp files'
rm -rf unification.o
#export SYSTEMC_HOME=/usr/local/systemc-3.0.0/
#export LD_LIBRARY_PATH=$SYSTEMC_HOME/lib-linux64
g++ -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib-linux64 unification_tb.cpp unification_pv_model.cpp  -lsystemc -lm -o unification.o
echo 'Simulation Started'
./unification.o
echo 'Simulation Ended'
