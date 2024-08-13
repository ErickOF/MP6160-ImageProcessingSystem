#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

#include <stdio.h>
#include <string.h>

//Testbench Defines
#define IMAG_ROWS 452
#define IMAG_COLS 640
#define RBG_CHANNELS_NUM 3
#define GRAY_CHANNELS_NUM 1

//Testbench Includes
#include "../inc/address_map.hpp"
#include "../inc/common_func.hpp"
// #include "tb_aux_functions.c"
// #include "img_unification.c"
//#include "img_filtering.cpp"

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

static void testbench(void *pParameter) {

	//Set the pointers to memory, where images are stored
	unsigned char *img_x = (unsigned char*) IMG_INPUT_ADDRESS_LO;
    unsigned char *img_y = (unsigned char*) IMG_INPUT_ADDRESS_LO + IMAG_ROWS*IMAG_COLS;
    unsigned char *img_result = (unsigned char*) IMG_OUTPUT_ADDRESS_LO;

 	printf("Starting Testbench\n");

	printf("Starting IMG Filtering Step: \n");
	filter_img(img_x, img_result, IMAG_ROWS, IMAG_COLS);
	printf("Done IMG Filtering Step: \n");

	// printf("Starting IMG Unification Step: \n");
	// unificate_img(img_x, img_y, img_result, IMAG_ROWS*IMAG_COLS, GRAY_CHANNELS_NUM);
	// printf("Done IMG Unification Step: \n");

	printf("Testbench Done\n");
}

int main( void )
{
	BaseType_t xReturned;
	printf("Starting FreeRTOS test\n");
	
	/* Create tasks */
	xReturned = xTaskCreate(testbench, "test_unificate", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY+1, NULL);

	printf("Returned: %0d\n", xReturned);
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
