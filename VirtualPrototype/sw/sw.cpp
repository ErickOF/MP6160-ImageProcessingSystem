#include <string.h>
#include <stdio.h>

#define IPS_FILTER_KERNEL_SIZE 3


int main(void) {

  unsigned char *local_window_ptr = new unsigned char[9];
  unsigned char *result_ptr = new unsigned char;
  unsigned char *filter = (unsigned char*)0x40005003;

  for(int i = 0; i < IPS_FILTER_KERNEL_SIZE*IPS_FILTER_KERNEL_SIZE; i++) {
    *(local_window_ptr+i) = i;
    printf("Writing[%0d]: %0d \n", i, (int)*(local_window_ptr+i)); 
  }

  memcpy(filter, local_window_ptr, IPS_FILTER_KERNEL_SIZE*IPS_FILTER_KERNEL_SIZE*sizeof(char));
  //Now Read
  memcpy(result_ptr, filter, sizeof(char));
  printf("FILTER is %0d\n", (int)*result_ptr);

  asm volatile ("ecall");
  return 0;
}
