#ifndef IPS_FILTER_DEFINES_HPP
#define IPS_FILTER_DEFINES_HPP

#ifndef IPS_FILTER_KERNEL_SIZE
#define IPS_FILTER_KERNEL_SIZE 3
#endif // IPS_FILTER_KERNEL_SIZE

#ifndef IPS_FILTER_PV_EN
// N * N * copy_pixel_to_mem_time + mult + redux + copy_pixel_to_mem_time
// Image is copied pixel by pixel
#define DELAY_TIME (IPS_FILTER_KERNEL_SIZE * IPS_FILTER_KERNEL_SIZE * 1) + 4 + 2 + 1
#endif // IPS_FILTER_PV_EN

#endif // IPS_FILTER_DEFINES_HPP
