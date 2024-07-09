#!/bin/bash

case $1 in
  "save_log")
    make EDGE_DETECTOR_LT_EN=1 USING_TLM_TB_EN=1 all > output.log
    ;;
  *)
    make EDGE_DETECTOR_LT_EN=1 USING_TLM_TB_EN=1 all
    ;;
esac
