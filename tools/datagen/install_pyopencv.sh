#!/bin/bash

sudo pip3 install opencv-python

python3 -c "import cv2; print(f'OpenCV: {cv2.__version__}')"
