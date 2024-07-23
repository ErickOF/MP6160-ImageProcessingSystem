#!/bin/sh
export SYSTEMC_HOME=/usr/local/systemc-3.0.0
export SYSTEMC_AMS_HOME=/usr/local/sytemc-3.0.0
export OPENCV_SO_DIR=/lib/x86_64-linux-gnu
export OPENCV_H_DIR=/usr/include/opencv4
export LD_LIBRARY_PATH=$SYSTEMC_HOME/lib-linux64:$OPENCV_SO_DIR:$SYSTEMC_AMS_HOME/lib-linux64
export USER_DEF_SYSTEMC_DIR=1
# export INCLUDE_OPENCV=1
export USE_CPP17=1
