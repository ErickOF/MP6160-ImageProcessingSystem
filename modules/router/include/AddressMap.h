#ifndef ADDRESSMAP_H
#define ADDRESSMAP_H

#define IMG_FILTER_KERNEL 0x00000003u
#define SOBEL_INPUT_0     0x00000027u
#define SOBEL_INPUT_1     0x0000002Fu
#define SOBEL_OUTPUT      0x00000030u
#define MEM_START         0x00000034u
#define IMG_INPUT         0x00000034u
#define IMG_INPROCESS_A   0x000E1034u //Write here rgb2gray result and unification output
#define IMG_INPROCESS_B   0x0012C034u //Write here filter result
#define IMG_INPROCESS_C   0x00177034u //Write here gradient A
#define IMG_INPROCESS_D   0x0020D034u //Write here gradient B
#define IMG_COMPRESSED    0x002A3034u

#endif // ADDRESSMAP_H
