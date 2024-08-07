#ifndef ADDRESS_MAP_HPP
#define ADDRESS_MAP_HPP

#define IMG_FILTER_KERNEL  0x00000003u
#define SOBEL_INPUT_0      0x00000027u
#define SOBEL_INPUT_1      0x0000002Fu
#define SOBEL_OUTPUT       0x00000030u
#define IMG_INPUT          0x00000034u
#define IMG_INPROCESS_A    0x000E1034u
#define IMG_INPROCESS_B    0x0012C034u
#define IMG_INPROCESS_C    0x001C2034u
#define IMG_COMPRESSED     0x00258034u

#define IMG_INPUT_SZ       0x000E1000u
#define IMG_INPROCESS_A_SZ 0x0004B000u
#define IMG_INPROCESS_B_SZ 0x00096000u
#define IMG_INPROCESS_C_SZ 0x00096000u
#define IMG_COMPRESSED_SZ  0x0004B000u

#define MEM_START          IMG_INPUT
#define MEM_FINISH         IMG_COMPRESSED + IMG_COMPRESSED_SZ

#endif // ADDRESS_MAP_HPP
