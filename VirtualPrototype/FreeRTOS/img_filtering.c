#ifndef IMG_FILTERING_CPP
#define IMG_FILTERING_CPP

#include <string.h>
#include <stdlib.h>
#include "../inc/address_map.hpp"
#include "../inc/common_func.hpp"

#define IPS_FILTER_KERNEL_SIZE 9


void filter_img (unsigned char* input_img, unsigned char* output_img, int img_width, int img_height) 
{
    unsigned char* filter_kernel_ptr = (unsigned char*) IMG_FILTER_KERNEL_ADDRESS_LO;
    unsigned char* filter_output_ptr = (unsigned char*) IMG_FILTER_OUTPUT_ADDRESS_LO;
    unsigned char* local_window_ptr = malloc(IPS_FILTER_KERNEL_SIZE*sizeof(char));
    unsigned char* read_ptr;
    unsigned char data_returned;

    int local_count = 0;
    int current_number_of_pixels = 0;
    int next_target_of_completion = 10;
    int local_group_count = 0;
    int total_number_of_pixels = img_width*img_height;
    unsigned char* local_results;

    for (int i = 0; i < img_width; i++) 
    {
        local_group_count = 0;
        for (int j = 0; j < img_height; j++)
        {
            extract_window(i, j, input_img, local_window_ptr, img_width, img_height);
            memcpy(filter_kernel_ptr, local_window_ptr, IPS_FILTER_KERNEL_SIZE*sizeof(char)); //Write to filter kernel
            memcpy(read_ptr, filter_output_ptr, sizeof(char)); //Read filter output
            data_returned = *read_ptr;

            if (local_count == 0)
            {
                local_results = malloc(8);
            }

            if (data_returned > 255) {
                *(local_results + local_count) = 255;
            }
            else {
                *(local_results + local_count) = data_returned;
            }

            local_count++;

            if (local_count == 8)
            {
                memcpy(output_img + ((i * img_height) + (local_group_count * 8 * sizeof(char))), local_results, 8 * sizeof(char));
                local_count = 0;
                local_group_count++;
            }

            current_number_of_pixels++;
            if ((((current_number_of_pixels*100) / (total_number_of_pixels))) >= next_target_of_completion) {
                printf("Image Filtering completed at %f\n", next_target_of_completion);
                next_target_of_completion += 10.0;
            }
        } 
    }
}


#endif // IMG_FILTERING_CPP