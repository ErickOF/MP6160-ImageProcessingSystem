#!/bin/sh
echo 'Compiling *.c *cpp files'
rm -rf ips_jpg.o
export SYSTEMC_HOME=/usr/local/systemc-2.3.3/
export OPENCV_H_DIR=/usr/local/include/opencv4/
export OPENCV_L_DIR=/usr/local/lib/
export LD_LIBRARY_PATH=$SYSTEMC_HOME/lib-linux64
g++ -I$SYSTEMC_HOME/include -I$OPENCV_H_DIR -L$SYSTEMC_HOME/lib-linux64 -L$OPENCV_L_DIR ips_jpg_lt_testbench.cpp ips_jpg_lt_model.cpp  -lsystemc -lm -o ips_jpg.o
echo 'Simulation Started'
./ips_jpg.o
echo 'Simulation Ended'
