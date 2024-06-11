#!/bin/sh
echo 'Compiling *.c *cpp files'
rm -rf ips_jpg.o
export SYSTEMC_HOME=/usr/local/systemc-2.3.3/
export OPENCV_H_DIR=/usr/local/include/opencv4/
export OPENCV_SO_DIR=/usr/local/lib/
export LD_LIBRARY_PATH=$SYSTEMC_HOME/lib-linux64
g++ -I$SYSTEMC_HOME/include -I$OPENCV_H_DIR -Iinclude/ -L$SYSTEMC_HOME/lib-linux64 -L$OPENCV_SO_DIR src/ips_jpg_at_testbench.hpp include/ips_jpg_at_model.cpp  -lsystemc -lm -o ips_jpg.o
echo 'Simulation Started'
./ips_jpg.o
echo 'Simulation Ended'
