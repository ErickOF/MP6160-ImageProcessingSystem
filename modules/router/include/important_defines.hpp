#ifndef IMPORTANT_DEFINES_HPP
#define IMPORTANT_DEFINES_HPP

#include "address_map.hpp"

#define IMAG_ROWS 480
#define IMAG_COLS 640

#ifndef IPS_FILTER_KERNEL_SIZE
#define IPS_FILTER_KERNEL_SIZE 3
#endif // IPS_FILTER_KERNEL_SIZE
#ifndef IPS_IN_TYPE_TB
#define IPS_IN_TYPE_TB float
#endif // IPS_IN_TYPE_TB
#ifndef IPS_OUT_TYPE_TB
#define IPS_OUT_TYPE_TB float
#endif // IPS_OUT_TYPE_TB

// Main clock frequency in Hz - 25.175 MHz
#ifndef IPS_CLK_FREQ
#define IPS_CLK_FREQ 25175000
#endif // IPS_CLK_FREQ
// VGA settings
#ifndef IPS_H_ACTIVE
#define IPS_H_ACTIVE 640
#endif // IPS_H_ACTIVE
#ifndef IPS_H_FP
#define IPS_H_FP 16
#endif // IPS_H_FP
#ifndef IPS_H_SYNC_PULSE
#define IPS_H_SYNC_PULSE 96
#endif // IPS_H_SYNC_PULSE
#ifndef IPS_H_BP
#define IPS_H_BP 48
#endif // IPS_H_BP
#ifndef IPS_V_ACTIVE
#define IPS_V_ACTIVE 480
#endif // IPS_V_ACTIVE
#ifndef IPS_V_FP
#define IPS_V_FP 10
#endif // IPS_V_FP
#ifndef IPS_V_SYNC_PULSE
#define IPS_V_SYNC_PULSE 2
#endif // IPS_V_SYNC_PULSE
#ifndef IPS_V_BP
#define IPS_V_BP 33
#endif // IPS_V_BP
// Compute the total number of pixels
#define IPS_TOTAL_VERTICAL (H_ACTIVE + H_FP + H_SYNC_PULSE + H_BP)
#define IPS_TOTAL_HORIZONTAL (V_ACTIVE + V_FP + V_SYNC_PULSE + V_BP)
#define IPS_TOTAL_PIXELES (TOTAL_VERTICAL * TOTAL_HORIZONTAL)
// Number of bits for ADC, DAC and VGA
#ifndef IPS_BITS
#define IPS_BITS 8
#endif // IPS_BITS
#ifndef IPS_VOLTAGE_MIN
#define IPS_VOLTAGE_MIN 0
#endif // IPS_VOLTAGE_MIN
#ifndef IPS_VOLTAGE_MAX
#define IPS_VOLTAGE_MAX 3300
#endif // IPS_VOLTAGE_MAX

#endif // IMPORTANT_DEFINES_HPP
