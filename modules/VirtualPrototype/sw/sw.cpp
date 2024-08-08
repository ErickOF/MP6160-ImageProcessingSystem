#include <string.h>
#include <stdio.h>
#include "address_map.hpp"

#define IPS_FILTER_KERNEL_SIZE 3
#define IMAG_ROWS 452
#define IMAG_COLS 640

// void extract_window(int i, int j, unsigned char *initial_address_ptr, unsigned char*& local_window_ptr)
// {
//   unsigned char* read_ptr = new unsigned char[3];
//   if ((i == 0) && (j == 0)) // Upper left corner of the image
//   {
//     // First row
//     *(local_window_ptr    ) = 0;
//     *(local_window_ptr + 1) = 0;
//     *(local_window_ptr + 2) = 0;
//     // Second row
//     memcpy(read_ptr, initial_address_ptr, 2 * sizeof(char));
//     *(local_window_ptr + 3) = 0;
//     *(local_window_ptr + 4) = *(read_ptr    );
//     *(local_window_ptr + 5) = *(read_ptr + 1);
//     // Third row
//     memcpy(read_ptr, initial_address_ptr + IMAG_COLS, 2 * sizeof(char));
//     *(local_window_ptr + 6) = 0;
//     *(local_window_ptr + 7) = *(read_ptr    );
//     *(local_window_ptr + 8) = *(read_ptr + 1);
//   }
//   else if ((i == 0) && (j == IMAG_COLS - 1)) // Upper right corner of the image
//   {
//     // First row
//     *(local_window_ptr    ) = 0;
//     *(local_window_ptr + 1) = 0;
//     *(local_window_ptr + 2) = 0;
//     // Second row
//     memcpy(read_ptr, initial_address_ptr + (IMAG_COLS - 2), 2 * sizeof(char));
//     *(local_window_ptr + 3) = *(read_ptr    );
//     *(local_window_ptr + 4) = *(read_ptr + 1);
//     *(local_window_ptr + 5) = 0;
//     // Third row
//     memcpy(read_ptr, initial_address_ptr + (IMAG_COLS + (IMAG_COLS - 2)), 2 * sizeof(char));
//     *(local_window_ptr + 6) = *(read_ptr    );
//     *(local_window_ptr + 7) = *(read_ptr + 1);
//     *(local_window_ptr + 8) = 0;
//   }
//   else if (i == 0) // Upper border
//   {
//     // First row
//     *(local_window_ptr    ) = 0;
//     *(local_window_ptr + 1) = 0;
//     *(local_window_ptr + 2) = 0;
//     // Second row
//     memcpy(read_ptr, initial_address_ptr + (j - 1), 3 * sizeof(char));
//     *(local_window_ptr + 3) = *(read_ptr    );
//     *(local_window_ptr + 4) = *(read_ptr + 1);
//     *(local_window_ptr + 5) = *(read_ptr + 2);
//     // Third row
//     memcpy(read_ptr, initial_address_ptr + (IMAG_COLS + (j - 1)), 3 * sizeof(char));
//     *(local_window_ptr + 6) = *(read_ptr    );
//     *(local_window_ptr + 7) = *(read_ptr + 1);
//     *(local_window_ptr + 8) = *(read_ptr + 2);
//   }
//   else if ((i == IMAG_ROWS - 1) && (j == 0)) // Lower left corner of the image
//   {
//     // First row
//     memcpy(read_ptr, initial_address_ptr + ((IMAG_ROWS - 2) * IMAG_COLS), 2 * sizeof(char));
//     *(local_window_ptr    ) = 0;
//     *(local_window_ptr + 1) = *(read_ptr    );
//     *(local_window_ptr + 2) = *(read_ptr + 1);
//     // Second row
//     memcpy(read_ptr, initial_address_ptr + ((IMAG_ROWS - 1) * IMAG_COLS), 2 * sizeof(char));
//     *(local_window_ptr + 3) = 0;
//     *(local_window_ptr + 4) = *(read_ptr    );
//     *(local_window_ptr + 5) = *(read_ptr + 1);
//     // Third row
//     *(local_window_ptr + 6) = 0;
//     *(local_window_ptr + 7) = 0;
//     *(local_window_ptr + 8) = 0;
//   }
//   else if ((i == IMAG_ROWS - 1) && (j == IMAG_COLS - 1)) // Lower right corner of the image
//   {
//     // First row
//     memcpy(read_ptr, initial_address_ptr + (((IMAG_ROWS - 2) * IMAG_COLS) + (IMAG_COLS - 2)), 2 * sizeof(char));
//     *(local_window_ptr    ) = *(read_ptr    );
//     *(local_window_ptr + 1) = *(read_ptr + 1);
//     *(local_window_ptr + 2) = 0;
//     // Second row
//     memcpy(read_ptr, initial_address_ptr + (((IMAG_ROWS - 1) * IMAG_COLS) + (IMAG_COLS - 2)), 2 * sizeof(char));
//     *(local_window_ptr + 3) = *(read_ptr    );
//     *(local_window_ptr + 4) = *(read_ptr + 1);
//     *(local_window_ptr + 5) = 0;
//     // Third row
//     *(local_window_ptr + 6) = 0;
//     *(local_window_ptr + 7) = 0;
//     *(local_window_ptr + 8) = 0;
//   }
//   else if (i == IMAG_ROWS - 1) // Lower border of the image
//   {
//     // First row
//     memcpy(read_ptr, initial_address_ptr + (((IMAG_ROWS - 2) * IMAG_COLS) + (j - 1)), 3 * sizeof(char));
//     *(local_window_ptr    ) = *(read_ptr    );
//     *(local_window_ptr + 1) = *(read_ptr + 1);
//     *(local_window_ptr + 2) = *(read_ptr + 2);
//     // Second row
//     memcpy(read_ptr, initial_address_ptr + (((IMAG_ROWS - 1) * IMAG_COLS) + (j - 1)), 3 * sizeof(char));
//     *(local_window_ptr + 3) = *(read_ptr    );
//     *(local_window_ptr + 4) = *(read_ptr + 1);
//     *(local_window_ptr + 5) = *(read_ptr + 2);
//     // Third row
//     *(local_window_ptr + 6) = 0;
//     *(local_window_ptr + 7) = 0;
//     *(local_window_ptr + 8) = 0;
//   }
//   else if (j == 0) // Left border of the image
//   {
//     // First row
//     memcpy(read_ptr, initial_address_ptr + ((i - 1) * IMAG_COLS), 2 * sizeof(char));
//     *(local_window_ptr    ) = 0;
//     *(local_window_ptr + 1) = *(read_ptr    );
//     *(local_window_ptr + 2) = *(read_ptr + 1);
//     // Second row
//     memcpy(read_ptr, initial_address_ptr + (i * IMAG_COLS), 2 * sizeof(char));
//     *(local_window_ptr + 3) = 0;
//     *(local_window_ptr + 4) = *(read_ptr    );
//     *(local_window_ptr + 5) = *(read_ptr + 1);
//     // Third row
//     memcpy(read_ptr, initial_address_ptr + ((i + 1) * IMAG_COLS), 2 * sizeof(char));
//     *(local_window_ptr + 6) = 0;
//     *(local_window_ptr + 7) = *(read_ptr    );
//     *(local_window_ptr + 8) = *(read_ptr + 1);
//   }
//   else if (j == IMAG_COLS - 1) // Right border of the image
//   {
//     // First row
//     memcpy(read_ptr, initial_address_ptr + (((i - 1) * IMAG_COLS) + (j - 1)), 2 * sizeof(char));
//     *(local_window_ptr    ) = *(read_ptr    );
//     *(local_window_ptr + 1) = *(read_ptr + 1);
//     *(local_window_ptr + 2) = 0;
//     // Second row
//     memcpy(read_ptr, initial_address_ptr + ((i * IMAG_COLS) + (j - 1)), 2 * sizeof(char));
//     *(local_window_ptr + 3) = *(read_ptr    );
//     *(local_window_ptr + 4) = *(read_ptr + 1);
//     *(local_window_ptr + 5) = 0;
//     // Third row
//     memcpy(read_ptr, initial_address_ptr + (((i + 1) * IMAG_COLS) + (j - 1)), 2 * sizeof(char));
//     *(local_window_ptr + 6) = *(read_ptr    );
//     *(local_window_ptr + 7) = *(read_ptr + 1);
//     *(local_window_ptr + 8) = 0;
//   }
//   else // Rest of the image
//   {
//     // First row
//     memcpy(read_ptr, initial_address_ptr + (((i - 1) * IMAG_COLS) + (j - 1)), 3 * sizeof(char));
//     *(local_window_ptr    ) = *(read_ptr    );
//     *(local_window_ptr + 1) = *(read_ptr + 1);
//     *(local_window_ptr + 2) = *(read_ptr + 2);
//     // Second row
//     memcpy(read_ptr, initial_address_ptr + ((i * IMAG_COLS) + (j - 1)), 3 * sizeof(char));
//     *(local_window_ptr + 3) = *(read_ptr    );
//     *(local_window_ptr + 4) = *(read_ptr + 1);
//     *(local_window_ptr + 5) = *(read_ptr + 2);
//     // Third row
//     memcpy(read_ptr, initial_address_ptr + (((i + 1) * IMAG_COLS) + (j - 1)), 3 * sizeof(char));
//     *(local_window_ptr + 6) = *(read_ptr    );
//     *(local_window_ptr + 7) = *(read_ptr + 1);
//     *(local_window_ptr + 8) = *(read_ptr + 2);
//   }
// }

int main(void) {

  // unsigned char *local_window_ptr = new unsigned char[9];
  // unsigned char *result_ptr = new unsigned char;
  // unsigned char *filter = (unsigned char*)IMG_FILTER_KERNEL_ADDRESS_LO;
  // unsigned char *filter_output = (unsigned char*)IMG_FILTER_OUTPUT_ADDRESS_LO;
  unsigned char *img = (unsigned char*) IMG_INPUT_ADDRESS_LO;
  unsigned char *filtered_img = new unsigned char[IMAG_COLS*IMAG_ROWS];
  unsigned char *img_result = (unsigned char*) IMG_OUTPUT_ADDRESS_LO;

  memcpy(img_result, img, IMAG_COLS*IMAG_ROWS*sizeof(char));

  // for(int i = 0; i < IPS_FILTER_KERNEL_SIZE*IPS_FILTER_KERNEL_SIZE; i++) {
  //   *(local_window_ptr+i) = i;
  //   printf("Writing[%0d]: %0d \n", i, (int)*(local_window_ptr+i)); 
  // }

  //memcpy(filter, local_window_ptr, IPS_FILTER_KERNEL_SIZE*IPS_FILTER_KERNEL_SIZE*sizeof(char));
  // //Now Read
  // memcpy(result_ptr, filter, sizeof(char));
  // printf("FILTER is %0d\n", (int)*result_ptr);

  // memcpy(img_ptr, img, 289280*sizeof(char));
  // int local_count = 0;
  // int current_number_of_pixels = 0;
  // int next_target_of_completion = 10.0;
  // int local_group_count = 0;
  // int total_number_of_pixels = IMAG_COLS*IMAG_ROWS;
  // unsigned char* local_results;
  // for (int i = 0; i < 100; i++)
  // {
  //   local_group_count = 0;
  //   for (int j = 0; j < 100; j++)
  //   {
  //     unsigned char* local_window_ptr = new unsigned char[9];
  //     unsigned char* data_returned_ptr;
  //     unsigned char* write_ptr = new unsigned char[9];
  //     unsigned char* read_ptr = new unsigned char;
  //     unsigned char data_returned;

  //     extract_window(i, j, img, local_window_ptr);

  //     write_ptr = local_window_ptr;
  //     memcpy(filter, write_ptr, IPS_FILTER_KERNEL_SIZE*IPS_FILTER_KERNEL_SIZE*sizeof(char));
  //     memcpy(read_ptr, filter_output, sizeof(char));
  //     data_returned = *read_ptr;
  //     printf("Data Returned: %0d\n", data_returned);

  //     if (data_returned > 255) {
  //       *(filtered_img + (i*IMAG_COLS)+j) = 255;
  //     }
  //     else {
  //       *(filtered_img + (i*IMAG_COLS)+j) = data_returned;
  //     }

  //     if (local_count == 0)
  //     {
  //       local_results = new unsigned char[8];
  //     }

  //     if (data_returned > 255)
  //     {
  //       *(local_results + local_count) = 255;
  //     }
  //     else {
  //       *(local_results + local_count) = data_returned;
  //     }

  //     local_count++;
  //     if (local_count == 8)
  //     {
  //       memcpy(img_result + ((i * IMAG_COLS) + (local_group_count * 8 * sizeof(char))), local_results, 8 * sizeof(char));
  //       local_count = 0;
  //       local_group_count++;
  //     }

  //     current_number_of_pixels++;
  //     if(current_number_of_pixels*100/total_number_of_pixels >= next_target_of_completion) {
  //       printf("Image processing completed at %d\n", next_target_of_completion);
  //       next_target_of_completion += 10.0;
  //     }
  //   }
  // }
  // memcpy(img_result, filtered_img, total_number_of_pixels * sizeof(char));
        


  printf("FINISHED\n");
  asm volatile ("ecall");
  return 0;
}
