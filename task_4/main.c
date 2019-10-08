#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gpio.h>
#include <power_clocks_lib.h>
#include <sysclk.h>
#include <compiler.h>
#include <board.h>
#include <stdio_usb.h>


#define CONFIG_USART_IF (AVR32_USART2)

#include "FreeRTOS.h"
#include "task.h"

#define TEST_A      AVR32_PIN_PA31
#define RESPONSE_A  AVR32_PIN_PA30
#define TEST_B      AVR32_PIN_PA29
#define RESPONSE_B  AVR32_PIN_PA28
#define TEST_C      AVR32_PIN_PA27
#define RESPONSE_C  AVR32_PIN_PB00


//#define TASK_A
//#define TASK_B
#define TASK_C
//#define TASK_D

uint8_t response_c_wait;

struct responseTaskArgs {
	struct {
		uint32_t test;
		uint32_t response;
	} pin;
	uint32_t wait_time_ms;
};

void busy_delay_ms(int delay){
    for(; delay != 0; delay--){
        for(int i = 0; i < 2108; i++){
            asm volatile ("" ::: "memory");
        }
    }
}

void busy_delay_short(void){
    for(int i = 0; i < 10; i++){
        asm volatile ("" ::: "memory");
    }
}

void init(){
	board_init();
	
    gpio_configure_pin(TEST_A, GPIO_DIR_INPUT);
    gpio_configure_pin(TEST_B, GPIO_DIR_INPUT);
    gpio_configure_pin(TEST_C, GPIO_DIR_INPUT);
    gpio_configure_pin(RESPONSE_A, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(RESPONSE_B, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);
    gpio_configure_pin(RESPONSE_C, GPIO_DIR_OUTPUT | GPIO_INIT_HIGH);

	pcl_switch_to_osc(PCL_OSC0, FOSC0, OSC0_STARTUP);
	
	stdio_usb_init(&CONFIG_USART_IF);

    #if defined(__GNUC__) && defined(__AVR32__)
	    setbuf(stdout, NULL);
	    setbuf(stdin,  NULL);
    #endif
}

static void vBasicTask(void *pvParameters){
	const portTickType xDelay = 500;
	
	while(1){
		gpio_toggle_pin(LED0_GPIO);
		printf("tick\n");
		
		vTaskDelay(xDelay);
		//busy_delay_ms(500);
	}
}

// TODO: Parameterize these
static void vTask_LED0(void *pvParameters){
	const portTickType xDelay = 200;
	while(1){
		gpio_toggle_pin(LED0_GPIO);
		vTaskDelay(xDelay);
	}
}

static void vTask_LED1(void *pvParameters){
	const portTickType xDelay = 500;
	while(1){
		gpio_toggle_pin(LED1_GPIO);
		vTaskDelay(xDelay);
	}
}


static void responseTask(void* args){
	struct responseTaskArgs a = *(struct responseTaskArgs*)args;
	while(1){
		if(gpio_pin_is_low(a.pin.test)){
			if(a.wait_time_ms > 0)
			{
				busy_delay_ms(a.wait_time_ms);
			}
			gpio_set_pin_low(a.pin.response);
			vTaskDelay(1);
			//busy_delay_short();
			gpio_set_pin_high(a.pin.response);
		}
		vTaskDelay(1);
	}
}




int main(){
	// initialize
	init();

	#	ifdef TASK_A
	xTaskCreate(vTask_LED0, "LED0", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(vTask_LED1, "LED1", 1024, NULL, tskIDLE_PRIORITY + 1, NULL);
	#	else
	#		ifdef TASK_C
	response_c_wait = 3;
	#		else
	response_c_wait = 0;
	#		endif
	xTaskCreate(responseTask, "A", 1024,(&(struct responseTaskArgs){{TEST_A, RESPONSE_A}, 0}),tskIDLE_PRIORITY + 3, NULL);
	xTaskCreate(responseTask, "B", 1024,(&(struct responseTaskArgs){{TEST_B, RESPONSE_B}, 0}),tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(responseTask, "C", 1024,(&(struct responseTaskArgs){{TEST_C, RESPONSE_C}, response_c_wait}),tskIDLE_PRIORITY + 1, NULL);
	
	#	endif

	// Start the scheduler, anything after this will not run.
	vTaskStartScheduler();
	
}

