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

QueueHandle_t my_queue = NULL;

static void task_1(void *pParameter) {
    
        int data = 5;
	printf("Task 1 starts\n");

	while(1) {
		printf("T1: Tick %ld\n",  xTaskGetTickCount() );
                xQueueSend(my_queue, &data, portMAX_DELAY);
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

static void task_2(void *pParameter) {
    
        int data = 7;
        
	printf("Task 2 starts\n");

	while(1) {
		printf("T2: Tick %ld\n",  xTaskGetTickCount() );
                xQueueSend(my_queue, &data, portMAX_DELAY);
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}

static void task_3(void *pParameter) {
        int data;
        
 	printf("Task 3 starts\n");

	while(1) {
                xQueueReceive(my_queue, &data, portMAX_DELAY);
		printf("T3: Tick %ld. Recv: %ld\n",  xTaskGetTickCount(), data);
		//vTaskDelay(1000 / portTICK_PERIOD_MS);
	}   
    
}

static void task_test_sobel(void *pParameter)
{
	unsigned char *sobel_input_0_ptr = (unsigned char*) SOBEL_INPUT_0_ADDRESS_LO;
	unsigned char *sobel_input_1_ptr = (unsigned char*) SOBEL_INPUT_1_ADDRESS_LO;
	short int *sobel_output_ptr = (short int*) SOBEL_OUTPUT_ADDRESS_LO;
	unsigned char local_window[3 * 3];
	short int sobel_results[2];

	printf("Starting with SOBEL testing on address %p\n", (void*)sobel_input_0_ptr);
  *(local_window + 0) = 150;
  *(local_window + 1) = 20;
  *(local_window + 2) = 38;
  *(local_window + 3) = 64;
  *(local_window + 4) = 32;
  *(local_window + 5) = 8;
  *(local_window + 6) = 16;
  *(local_window + 7) = 75;
  *(local_window + 8) = 99;
  printf("Will copy values to SOBEL on address %p\n", (void*)sobel_input_0_ptr);
  memcpy(sobel_input_0_ptr, local_window, 8 * sizeof(char));
  printf("Will copy values to SOBEL on address %p\n", (void*)sobel_input_1_ptr);
  memcpy(sobel_input_1_ptr, (local_window + 8), 1 * sizeof(char));
  memcpy(sobel_results, sobel_output_ptr, 2 * sizeof(short int));
  printf("Results of SOBEL are %d at address %p and %d at address %p\n", sobel_results[0], (void*)sobel_output_ptr, sobel_results[1], (void*)(sobel_output_ptr + 1));
}

int main( void )
{

	printf("Starting FreeRTOS test\n");

        my_queue = xQueueCreate(10, sizeof(int));
        
        /* Create tasks */
        xTaskCreate(task_1, "Task1", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);
	xTaskCreate(task_2, "Task2", 10000, NULL, tskIDLE_PRIORITY+1, NULL);
        xTaskCreate(task_3, "Task3", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);
        
	xTaskCreate(task_test_sobel, "TaskSobel", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);
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
