#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include<stdio.h>

#include "important_defines.hpp"

//For pre-loading images
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define TRACE (*(unsigned char *)0x40000000)

extern void register_timer_isr();

void transfer_window(int i, int j, unsigned long long source_address, unsigned long long target_address)
{
	unsigned char *source_ptr = (unsigned char *) source_address;
	unsigned char *target_ptr = (unsigned char *) target_address;
	unsigned char local_window[9];
	unsigned char read_ptr[3];

	if ((i == 0) && (j == 0)) // Upper left corner of the image
	{
		// First row
		*(local_window    ) = 0;
		*(local_window + 1) = 0;
		*(local_window + 2) = 0;
		// Second row
		memcpy(read_ptr, source_ptr, 2 * sizeof(char));
		*(local_window + 3) = 0;
		*(local_window + 4) = *(read_ptr    );
		*(local_window + 5) = *(read_ptr + 1);
		// Third row
		memcpy(read_ptr, source_ptr + IMAG_COLS, 2 * sizeof(char));
		*(local_window + 6) = 0;
		*(local_window + 7) = *(read_ptr    );
		*(local_window + 8) = *(read_ptr + 1);
	}
	else if ((i == 0) && (j == IMAG_COLS - 1)) // Upper right corner of the image
	{
		// First row
		*(local_window    ) = 0;
		*(local_window + 1) = 0;
		*(local_window + 2) = 0;
		// Second row
		memcpy(read_ptr, source_ptr + (IMAG_COLS - 2), 2 * sizeof(char));
		*(local_window + 3) = *(read_ptr    );
		*(local_window + 4) = *(read_ptr + 1);
		*(local_window + 5) = 0;
		// Third row
		memcpy(read_ptr, source_ptr + (IMAG_COLS + (IMAG_COLS - 2)), 2 * sizeof(char));
		*(local_window + 6) = *(read_ptr    );
		*(local_window + 7) = *(read_ptr + 1);
		*(local_window + 8) = 0;
	}
	else if (i == 0) // Upper border
	{
		// First row
		*(local_window    ) = 0;
		*(local_window + 1) = 0;
		*(local_window + 2) = 0;
		// Second row
		memcpy(read_ptr, source_ptr + (j - 1), 3 * sizeof(char));
		*(local_window + 3) = *(read_ptr    );
		*(local_window + 4) = *(read_ptr + 1);
		*(local_window + 5) = *(read_ptr + 2);
		// Third row
		memcpy(read_ptr, source_ptr + (IMAG_COLS + (j - 1)), 3 * sizeof(char));
		*(local_window + 6) = *(read_ptr    );
		*(local_window + 7) = *(read_ptr + 1);
		*(local_window + 8) = *(read_ptr + 2);
	}
	else if ((i == IMAG_ROWS - 1) && (j == 0)) // Lower left corner of the image
	{
		// First row
		memcpy(read_ptr, source_ptr + ((IMAG_ROWS - 2) * IMAG_COLS), 2 * sizeof(char));
		*(local_window    ) = 0;
		*(local_window + 1) = *(read_ptr    );
		*(local_window + 2) = *(read_ptr + 1);
		// Second row
		memcpy(read_ptr, source_ptr + ((IMAG_ROWS - 1) * IMAG_COLS), 2 * sizeof(char));
		*(local_window + 3) = 0;
		*(local_window + 4) = *(read_ptr    );
		*(local_window + 5) = *(read_ptr + 1);
		// Third row
		*(local_window + 6) = 0;
		*(local_window + 7) = 0;
		*(local_window + 8) = 0;
	}
	else if ((i == IMAG_ROWS - 1) && (j == IMAG_COLS - 1)) // Lower right corner of the image
	{
		// First row
		memcpy(read_ptr, source_ptr + (((IMAG_ROWS - 2) * IMAG_COLS) + (IMAG_COLS - 2)), 2 * sizeof(char));
		*(local_window    ) = *(read_ptr    );
		*(local_window + 1) = *(read_ptr + 1);
		*(local_window + 2) = 0;
		// Second row
		memcpy(read_ptr, source_ptr + (((IMAG_ROWS - 1) * IMAG_COLS) + (IMAG_COLS - 2)), 2 * sizeof(char));
		*(local_window + 3) = *(read_ptr    );
		*(local_window + 4) = *(read_ptr + 1);
		*(local_window + 5) = 0;
		// Third row
		*(local_window + 6) = 0;
		*(local_window + 7) = 0;
		*(local_window + 8) = 0;
	}
	else if (i == IMAG_ROWS - 1) // Lower border of the image
	{
		// First row
		memcpy(read_ptr, source_ptr + (((IMAG_ROWS - 2) * IMAG_COLS) + (j - 1)), 3 * sizeof(char));
		*(local_window    ) = *(read_ptr    );
		*(local_window + 1) = *(read_ptr + 1);
		*(local_window + 2) = *(read_ptr + 2);
		// Second row
		memcpy(read_ptr, source_ptr + (((IMAG_ROWS - 1) * IMAG_COLS) + (j - 1)), 3 * sizeof(char));
		*(local_window + 3) = *(read_ptr    );
		*(local_window + 4) = *(read_ptr + 1);
		*(local_window + 5) = *(read_ptr + 2);
		// Third row
		*(local_window + 6) = 0;
		*(local_window + 7) = 0;
		*(local_window + 8) = 0;
	}
	else if (j == 0) // Left border of the image
	{
		// First row
		memcpy(read_ptr, source_ptr + ((i - 1) * IMAG_COLS), 2 * sizeof(char));
		*(local_window    ) = 0;
		*(local_window + 1) = *(read_ptr    );
		*(local_window + 2) = *(read_ptr + 1);
		// Second row
		memcpy(read_ptr, source_ptr + (i * IMAG_COLS), 2 * sizeof(char));
		*(local_window + 3) = 0;
		*(local_window + 4) = *(read_ptr    );
		*(local_window + 5) = *(read_ptr + 1);
		// Third row
		memcpy(read_ptr, source_ptr + ((i + 1) * IMAG_COLS), 2 * sizeof(char));
		*(local_window + 6) = 0;
		*(local_window + 7) = *(read_ptr    );
		*(local_window + 8) = *(read_ptr + 1);
	}
	else if (j == IMAG_COLS - 1) // Right border of the image
	{
		// First row
		memcpy(read_ptr, source_ptr + (((i - 1) * IMAG_COLS) + (j - 1)), 2 * sizeof(char));
		*(local_window    ) = *(read_ptr    );
		*(local_window + 1) = *(read_ptr + 1);
		*(local_window + 2) = 0;
		// Second row
		memcpy(read_ptr, source_ptr + ((i * IMAG_COLS) + (j - 1)), 2 * sizeof(char));
		*(local_window + 3) = *(read_ptr    );
		*(local_window + 4) = *(read_ptr + 1);
		*(local_window + 5) = 0;
		// Third row
		memcpy(read_ptr, source_ptr + (((i + 1) * IMAG_COLS) + (j - 1)), 2 * sizeof(char));
		*(local_window + 6) = *(read_ptr    );
		*(local_window + 7) = *(read_ptr + 1);
		*(local_window + 8) = 0;
	}
	else // Rest of the image
	{
		// First row
		memcpy(read_ptr, source_ptr + (((i - 1) * IMAG_COLS) + (j - 1)), 3 * sizeof(char));
		*(local_window    ) = *(read_ptr    );
		*(local_window + 1) = *(read_ptr + 1);
		*(local_window + 2) = *(read_ptr + 2);
		// Second row
		memcpy(read_ptr, source_ptr + ((i * IMAG_COLS) + (j - 1)), 3 * sizeof(char));
		*(local_window + 3) = *(read_ptr    );
		*(local_window + 4) = *(read_ptr + 1);
		*(local_window + 5) = *(read_ptr + 2);
		// Third row
		memcpy(read_ptr, source_ptr + (((i + 1) * IMAG_COLS) + (j - 1)), 3 * sizeof(char));
		*(local_window + 6) = *(read_ptr    );
		*(local_window + 7) = *(read_ptr + 1);
		*(local_window + 8) = *(read_ptr + 2);
	}

	// if (((i == 0 || i == IMAG_ROWS - 1) && (j < 2 || j > IMAG_COLS - 3)) || ((j == 0 || j == IMAG_COLS -1) && (i < 2 || i > IMAG_ROWS - 3)))
	// {
	// 	printf("Window %0d %0d:\n\t%5d %5d %5d\n\t%5d %5d %5d\n\t%5d %5d %5d\n", i, j, local_window[0], local_window[1], local_window[2], local_window[3],local_window[4], local_window[5], local_window[6], local_window[7], local_window[8]);
	// }

	memcpy(target_ptr, local_window, 8 * sizeof(char));
	memcpy((target_ptr + 8), (local_window + 8), sizeof(char));
}

void obtain_gradients_sobel()
{
	short int *sobel_output_ptr = (short int*) SOBEL_OUTPUT_ADDRESS_LO;
	short int *output_image_X_ptr = (short int*) IMG_INPROCESS_B_ADDRESS_LO;
	short int *output_image_Y_ptr = (short int*) IMG_INPROCESS_C_ADDRESS_LO;
	short int sobel_results[2];

	int final_target = IMAG_ROWS * IMAG_COLS;
	int current_progress = 0;
	int step_percent = 2;
	int step = final_target / (100 / step_percent);
	int next_target = 1;

	printf("Starting IMG Sobel Step: \n");

	for (int i = 0; i < IMAG_ROWS; i++)
	{
		for (int j = 0; j < IMAG_COLS; j++)
		{
			transfer_window(i, j, IMG_INPROCESS_D_ADDRESS_LO, SOBEL_INPUT_0_ADDRESS_LO);
			memcpy(sobel_results, sobel_output_ptr, 2 * sizeof(short int));
			*(output_image_X_ptr + ((i * IMAG_COLS) + j)) = sobel_results[0];
			*(output_image_Y_ptr + ((i * IMAG_COLS) + j)) = sobel_results[1];

			if (current_progress == next_target * step)
			{
				printf("\tCurrent progress is at %0d%%\n", next_target * step_percent);
				next_target++;
			}
			current_progress++;
		}
	}

	printf("Done IMG Sobel Step: \n");
}

unsigned char compute_gray_value(unsigned char r_val, unsigned char g_val, unsigned char b_val)
{
	return (unsigned char)((299 * (int)r_val + 587 * (int)g_val + 114 * (int)b_val) / 1000);
}

void convert_to_grayscale()
{
	unsigned char *source_ptr = (unsigned char *) IMG_INPUT_ADDRESS_LO;
	unsigned char *target_ptr = (unsigned char *) IMG_INPROCESS_A_ADDRESS_LO;
	unsigned char rgb_val[3];
	unsigned char gray_val;

	int final_target = IMAG_ROWS * IMAG_COLS;
	int current_progress = 0;
	int step_percent = 2;
	int step = final_target / (100 / step_percent);
	int next_target = 1;

	printf("Starting IMG Grayscale Step: \n");

	for (int i = 0; i < IMAG_ROWS; i++)
	{
		for (int j = 0; j < IMAG_COLS; j++)
		{
			memcpy(rgb_val, source_ptr + ((i * 3 * IMAG_COLS) + (j * 3)), 3 * sizeof(char));
			// if (i == 0 && j < 10)
			// {
			// 	printf("On pixel %0d %0d r %0d g %0d b %0d\n", i, j, rgb_val[0], rgb_val[1], rgb_val[2]);
			// }
			gray_val = compute_gray_value(rgb_val[2], rgb_val[1], rgb_val[0]);
			// if (i == 0 && j < 10)
			// {
			// 	printf("On pixel %0d %0d gray value %0d\n", i, j, gray_val);
			// }
			*(target_ptr + ((i * IMAG_COLS) + j)) = gray_val;

			if (current_progress == next_target * step)
			{
				printf("\tCurrent progress is at %0d%%\n", next_target * step_percent);
				next_target++;
			}
			current_progress++;
		}
	}

	printf("Done IMG Grayscale Step: \n");
}

void filter_image()
{
	unsigned char *filter_output_ptr = (unsigned char*) IMG_FILTER_OUTPUT_ADDRESS_LO;
	unsigned char *output_image_ptr = (unsigned char*) IMG_INPROCESS_D_ADDRESS_LO;

	int final_target = IMAG_ROWS * IMAG_COLS;
	int current_progress = 0;
	int step_percent = 2;
	int step = final_target / (100 / step_percent);
	int next_target = 1;

	printf("Starting IMG Filtering Step: \n");

	for (int i = 0; i < IMAG_ROWS; i++)
	{
		for (int j = 0; j < IMAG_COLS; j++)
		{
			transfer_window(i, j, IMG_INPROCESS_A_ADDRESS_LO, IMG_FILTER_KERNEL_ADDRESS_LO);
			*(output_image_ptr + ((i * IMAG_COLS) + j)) = *(filter_output_ptr);

			if (current_progress == next_target * step)
			{
				printf("\tCurrent progress is at %0d%%\n", next_target * step_percent);
				next_target++;
			}
			current_progress++;
		}
	}

	printf("Done IMG Filtering Step: \n");
}

int intSqrt(int x) 
{
  unsigned int s = 0;
  for (unsigned int i = (1 << 15); i > 0; i >>= 1){
    if (((s+i) * (s+i)) <= x){
      s += i;
    }
  }
  return s;
}

int norm(int a, int b) 
{
  int norm_result = 0;

  norm_result = intSqrt(a*a+b*b); //sqrt(pow(a, 2) + pow(b, 2));


  return norm_result;
}

void unificate_img()
{
	short int *x_img = (short int *) IMG_INPROCESS_B_ADDRESS_LO;
	short int *y_img = (short int *) IMG_INPROCESS_C_ADDRESS_LO;
	unsigned char *unificated_img = (unsigned char *) IMG_INPROCESS_A_ADDRESS_LO;

	int final_target = IMAG_ROWS * IMAG_COLS;
	int current_progress = 0;
	int step_percent = 2;
	int step = final_target / (100 / step_percent);
	int next_target = 1;

	printf("Starting IMG Unification Step: \n");

	//Iterate over image
	for (int i = 0; i < IMAG_ROWS * IMAG_COLS; i++)
	{
		int pixel_magnitude;
		int pixel_x = (int) *x_img;
		int pixel_y = (int) *y_img;

		pixel_magnitude = norm(pixel_x, pixel_y);

		if (pixel_magnitude > 255)
		{
			pixel_magnitude = 255;
		}
		*unificated_img = (unsigned char) pixel_magnitude;

		x_img++;
		y_img++;
		unificated_img++;

		if (current_progress == next_target * step)
		{
			printf("\tCurrent progress is at %0d%%\n", next_target * step_percent);
			next_target++;
		}
		current_progress++;
	}

	printf("Done IMG Unification Step: \n");
}

void transmit_data(unsigned int data_length)
{
	unsigned int *data_length_ptr = (unsigned int *) IMG_OUTPUT_SIZE_ADDRESS_LO;
	unsigned char *start_transmission = (unsigned char *) IMG_OUTPUT_DONE_ADDRESS_LO;
	unsigned char *send_status = (unsigned char *) IMG_OUTPUT_STATUS_ADDRESS_LO;
	unsigned char send_status_result;

	const TickType_t xDelay = 200;

	printf("Preparing to send %0d bytes\n", data_length);

	*data_length_ptr = data_length;
	*start_transmission = 1;

	printf("Sent start signal to transmiter\n");

	memcpy(&send_status_result, send_status, sizeof(char));
	while(send_status_result == 1)
	{
		printf("\tTransmiter still sending data\n");
		vTaskDelay(xDelay);
		memcpy(&send_status_result, send_status, sizeof(char));
	}
	printf("Send process finished in the transmiter\n");
}

void copy_data()
{
	unsigned char *source_img = (unsigned char *) IMG_INPROCESS_A_ADDRESS_LO;
	unsigned char *output_img = (unsigned char *) IMG_OUTPUT_ADDRESS_LO;

	memcpy(output_img, source_img, IMAG_ROWS * IMAG_COLS);
}

void save_image_from_mem(int image_id)
{
	int *img_id_ptr = (int *) IMG_SAVER_ID_ADDRESS_LO;
	unsigned char *save_img_ptr = (unsigned char *) IMG_SAVER_START_ADDRESS_LO;

	*img_id_ptr = image_id;
	*save_img_ptr = 1;
	printf("Saved image with id %0d\n", image_id);
}

void receive_image()
{
	unsigned char *start_reception = (unsigned char *) IMG_INPUT_START_ADDRESS_LO;
	unsigned char *receive_status = (unsigned char *) IMG_INPUT_DONE_ADDRESS_LO;
	unsigned char receive_status_result;

	const TickType_t xDelay = 200;

	printf("Preparing to receive image from VGA\n");

	*start_reception = 1;

	printf("Sent start signal to receiver\n");

	memcpy(&receive_status_result, receive_status, sizeof(char));
	while(receive_status_result == 0)
	{
		printf("\tReceiver still receiving data\n");
		vTaskDelay(xDelay);
		memcpy(&receive_status_result, receive_status, sizeof(char));
	}
	printf("Receive process finished in the receiver\n");
}

static void testbench(void *pParameter) {
	receive_image();

	save_image_from_mem(0);

	convert_to_grayscale();

	save_image_from_mem(1);

	filter_image();

	save_image_from_mem(2);

	obtain_gradients_sobel();

	save_image_from_mem(3);

	unificate_img();

	save_image_from_mem(4);

	copy_data();

	save_image_from_mem(5);

	transmit_data(IMAG_ROWS * IMAG_COLS);

	save_image_from_mem(6);
}

int main( void )
{

	printf("Starting FreeRTOS test\n");

	xTaskCreate(testbench, "testbench", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);
	/* Start the kernel.  From here on, only tasks and interrupts will run. */
	vTaskStartScheduler();

	/* Exit FreeRTOS */
	return 0;
}

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();

	TRACE='M';
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	TRACE = 'S';
	for( ;; );
}
/*-----------------------------------------------------------*/
