#ifndef TB_AUX_FUNCTIONS_CPP
#define TB_AUX_FUNCTIONS_CPP

#include <stdlib.h>
#include <string.h>

//Extract Window: A function to extract a 3x3 window from an image
void extract_window(int i, int j, unsigned char *initial_address_ptr, unsigned char* local_window_ptr, int img_width, int img_height)
{
  unsigned char* read_ptr = malloc(3*sizeof(char));
  if ((i == 0) && (j == 0)) // Upper left corner of the image
  {
    // First row
    *(local_window_ptr    ) = 0;
    *(local_window_ptr + 1) = 0;
    *(local_window_ptr + 2) = 0;
    // Second row
    memcpy(read_ptr, initial_address_ptr, 2 * sizeof(char));
    *(local_window_ptr + 3) = 0;
    *(local_window_ptr + 4) = *(read_ptr    );
    *(local_window_ptr + 5) = *(read_ptr + 1);
    // Third row
    memcpy(read_ptr, initial_address_ptr + img_height, 2 * sizeof(char));
    *(local_window_ptr + 6) = 0;
    *(local_window_ptr + 7) = *(read_ptr    );
    *(local_window_ptr + 8) = *(read_ptr + 1);
  }
  else if ((i == 0) && (j == img_height - 1)) // Upper right corner of the image
  {
    // First row
    *(local_window_ptr    ) = 0;
    *(local_window_ptr + 1) = 0;
    *(local_window_ptr + 2) = 0;
    // Second row
    memcpy(read_ptr, initial_address_ptr + (img_height - 2), 2 * sizeof(char));
    *(local_window_ptr + 3) = *(read_ptr    );
    *(local_window_ptr + 4) = *(read_ptr + 1);
    *(local_window_ptr + 5) = 0;
    // Third row
    memcpy(read_ptr, initial_address_ptr + (img_height + (img_height - 2)), 2 * sizeof(char));
    *(local_window_ptr + 6) = *(read_ptr    );
    *(local_window_ptr + 7) = *(read_ptr + 1);
    *(local_window_ptr + 8) = 0;
  }
  else if (i == 0) // Upper border
  {
    // First row
    *(local_window_ptr    ) = 0;
    *(local_window_ptr + 1) = 0;
    *(local_window_ptr + 2) = 0;
    // Second row
    memcpy(read_ptr, initial_address_ptr + (j - 1), 3 * sizeof(char));
    *(local_window_ptr + 3) = *(read_ptr    );
    *(local_window_ptr + 4) = *(read_ptr + 1);
    *(local_window_ptr + 5) = *(read_ptr + 2);
    // Third row
    memcpy(read_ptr, initial_address_ptr + (img_height + (j - 1)), 3 * sizeof(char));
    *(local_window_ptr + 6) = *(read_ptr    );
    *(local_window_ptr + 7) = *(read_ptr + 1);
    *(local_window_ptr + 8) = *(read_ptr + 2);
  }
  else if ((i == img_width - 1) && (j == 0)) // Lower left corner of the image
  {
    // First row
    memcpy(read_ptr, initial_address_ptr + ((img_width - 2) * img_height), 2 * sizeof(char));
    *(local_window_ptr    ) = 0;
    *(local_window_ptr + 1) = *(read_ptr    );
    *(local_window_ptr + 2) = *(read_ptr + 1);
    // Second row
    memcpy(read_ptr, initial_address_ptr + ((img_width - 1) * img_height), 2 * sizeof(char));
    *(local_window_ptr + 3) = 0;
    *(local_window_ptr + 4) = *(read_ptr    );
    *(local_window_ptr + 5) = *(read_ptr + 1);
    // Third row
    *(local_window_ptr + 6) = 0;
    *(local_window_ptr + 7) = 0;
    *(local_window_ptr + 8) = 0;
  }
  else if ((i == img_width - 1) && (j == img_height - 1)) // Lower right corner of the image
  {
    // First row
    memcpy(read_ptr, initial_address_ptr + (((img_width - 2) * img_height) + (img_height - 2)), 2 * sizeof(char));
    *(local_window_ptr    ) = *(read_ptr    );
    *(local_window_ptr + 1) = *(read_ptr + 1);
    *(local_window_ptr + 2) = 0;
    // Second row
    memcpy(read_ptr, initial_address_ptr + (((img_width - 1) * img_height) + (img_height - 2)), 2 * sizeof(char));
    *(local_window_ptr + 3) = *(read_ptr    );
    *(local_window_ptr + 4) = *(read_ptr + 1);
    *(local_window_ptr + 5) = 0;
    // Third row
    *(local_window_ptr + 6) = 0;
    *(local_window_ptr + 7) = 0;
    *(local_window_ptr + 8) = 0;
  }
  else if (i == img_width - 1) // Lower border of the image
  {
    // First row
    memcpy(read_ptr, initial_address_ptr + (((img_width - 2) * img_height) + (j - 1)), 3 * sizeof(char));
    *(local_window_ptr    ) = *(read_ptr    );
    *(local_window_ptr + 1) = *(read_ptr + 1);
    *(local_window_ptr + 2) = *(read_ptr + 2);
    // Second row
    memcpy(read_ptr, initial_address_ptr + (((img_width - 1) * img_height) + (j - 1)), 3 * sizeof(char));
    *(local_window_ptr + 3) = *(read_ptr    );
    *(local_window_ptr + 4) = *(read_ptr + 1);
    *(local_window_ptr + 5) = *(read_ptr + 2);
    // Third row
    *(local_window_ptr + 6) = 0;
    *(local_window_ptr + 7) = 0;
    *(local_window_ptr + 8) = 0;
  }
  else if (j == 0) // Left border of the image
  {
    // First row
    memcpy(read_ptr, initial_address_ptr + ((i - 1) * img_height), 2 * sizeof(char));
    *(local_window_ptr    ) = 0;
    *(local_window_ptr + 1) = *(read_ptr    );
    *(local_window_ptr + 2) = *(read_ptr + 1);
    // Second row
    memcpy(read_ptr, initial_address_ptr + (i * img_height), 2 * sizeof(char));
    *(local_window_ptr + 3) = 0;
    *(local_window_ptr + 4) = *(read_ptr    );
    *(local_window_ptr + 5) = *(read_ptr + 1);
    // Third row
    memcpy(read_ptr, initial_address_ptr + ((i + 1) * img_height), 2 * sizeof(char));
    *(local_window_ptr + 6) = 0;
    *(local_window_ptr + 7) = *(read_ptr    );
    *(local_window_ptr + 8) = *(read_ptr + 1);
  }
  else if (j == img_height - 1) // Right border of the image
  {
    // First row
    memcpy(read_ptr, initial_address_ptr + (((i - 1) * img_height) + (j - 1)), 2 * sizeof(char));
    *(local_window_ptr    ) = *(read_ptr    );
    *(local_window_ptr + 1) = *(read_ptr + 1);
    *(local_window_ptr + 2) = 0;
    // Second row
    memcpy(read_ptr, initial_address_ptr + ((i * img_height) + (j - 1)), 2 * sizeof(char));
    *(local_window_ptr + 3) = *(read_ptr    );
    *(local_window_ptr + 4) = *(read_ptr + 1);
    *(local_window_ptr + 5) = 0;
    // Third row
    memcpy(read_ptr, initial_address_ptr + (((i + 1) * img_height) + (j - 1)), 2 * sizeof(char));
    *(local_window_ptr + 6) = *(read_ptr    );
    *(local_window_ptr + 7) = *(read_ptr + 1);
    *(local_window_ptr + 8) = 0;
  }
  else // Rest of the image
  {
    // First row
    memcpy(read_ptr, initial_address_ptr + (((i - 1) * img_height) + (j - 1)), 3 * sizeof(char));
    *(local_window_ptr    ) = *(read_ptr    );
    *(local_window_ptr + 1) = *(read_ptr + 1);
    *(local_window_ptr + 2) = *(read_ptr + 2);
    // Second row
    memcpy(read_ptr, initial_address_ptr + ((i * img_height) + (j - 1)), 3 * sizeof(char));
    *(local_window_ptr + 3) = *(read_ptr    );
    *(local_window_ptr + 4) = *(read_ptr + 1);
    *(local_window_ptr + 5) = *(read_ptr + 2);
    // Third row
    memcpy(read_ptr, initial_address_ptr + (((i + 1) * img_height) + (j - 1)), 3 * sizeof(char));
    *(local_window_ptr + 6) = *(read_ptr    );
    *(local_window_ptr + 7) = *(read_ptr + 1);
    *(local_window_ptr + 8) = *(read_ptr + 2);
  }

  //printf("Window is: [%0d, %0d, %0d,%0d, %0d, %0d,%0d,%0d,%0d]\n", *(local_window_ptr), *(local_window_ptr + 1),  *(local_window_ptr + 2), *(local_window_ptr + 3), *(local_window_ptr + 4), *(local_window_ptr + 5), *(local_window_ptr + 6), *(local_window_ptr + 7), *(local_window_ptr + 8));
}

#endif //TB_AUX_FUNCTIONS_CPP