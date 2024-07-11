#!/bin/bash

case $1 in
  "save_log")
    make RGB2GRAY_PV_EN=1 EDGE_DETECTOR_LT_EN=1 USING_TLM_TB_EN=1 all > output.log
    ;;
  "print_all")
    make RGB2GRAY_PV_EN=1 EDGE_DETECTOR_LT_EN=1 USING_TLM_TB_EN=1 print-all
    ;;
  *)
    make RGB2GRAY_PV_EN=1 EDGE_DETECTOR_LT_EN=1 USING_TLM_TB_EN=1 all
    ;;
esac
