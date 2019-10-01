#include <asf.h>
#include <board.h>
#include <gpio.h>
#include <sysclk.h>
#include "busy_delay.h"

#define CONFIG_USART_IF (AVR32_USART2)

// defines for BRTT interface
#define TEST_A      AVR32_PIN_PA31
#define RESPONSE_A  AVR32_PIN_PA30
#define TEST_B      AVR32_PIN_PA29
#define RESPONSE_B  AVR32_PIN_PA28
#define TEST_C      AVR32_PIN_PA27
#define RESPONSE_C  AVR32_PIN_PB00


__attribute__((__interrupt__)) static void interrupt_J3(void);


volatile int test_A_flag = 0;
volatile int test_B_flag = 0;
volatile int test_C_flag = 0;

void init(){
    sysclk_init();
    board_init();
    busy_delay_init(BOARD_OSC0_HZ);
    
    gpio_configure_pin(TEST_A, (GPIO_DIR_INPUT | GPIO_INTERRUPT));
    gpio_configure_pin(RESPONSE_A, (GPIO_DIR_OUTPUT | GPIO_PULL_UP));
    
    gpio_configure_pin(TEST_B, (GPIO_DIR_INPUT | GPIO_INTERRUPT));
    gpio_configure_pin(RESPONSE_B, (GPIO_DIR_OUTPUT | GPIO_PULL_UP));
     
    gpio_configure_pin(TEST_C, (GPIO_DIR_INPUT | GPIO_INTERRUPT));
    gpio_configure_pin(RESPONSE_C, (GPIO_DIR_OUTPUT | GPIO_PULL_UP));
	
    cpu_irq_disable();

    INTC_init_interrupts();
	gpio_enable_pin_interrupt(TEST_A, GPIO_FALLING_EDGE);
 	gpio_enable_pin_interrupt(TEST_B, GPIO_FALLING_EDGE);
 	gpio_enable_pin_interrupt(TEST_C, GPIO_FALLING_EDGE);

    INTC_register_interrupt(&interrupt_J3, AVR32_GPIO_IRQ_3, AVR32_INTC_INT1);

    cpu_irq_enable();

	

    stdio_usb_init(&CONFIG_USART_IF);

    #if defined(__GNUC__) && defined(__AVR32__)
        setbuf(stdout, NULL);
        setbuf(stdin,  NULL);
    #endif
}

__attribute__((__interrupt__)) static void interrupt_J3(void){ 


	
    if(gpio_get_pin_interrupt_flag(TEST_A) == 1)
	{
		gpio_clear_pin_interrupt_flag(TEST_A);
		test_A_flag = 1;
		
	}
	
	if(gpio_get_pin_interrupt_flag(TEST_B) == 1)
	{   
		gpio_clear_pin_interrupt_flag(TEST_B);
		test_B_flag = 1;
		
	}
	
	if(gpio_get_pin_interrupt_flag(TEST_C) == 1)
	{
		gpio_clear_pin_interrupt_flag(TEST_C);
		test_C_flag = 1;
	
	}
	



}


int main (void){
    init();

    while(1){

        if(test_A_flag)
		{
		    gpio_set_pin_low(RESPONSE_A);
		    busy_delay_us(5);
		    gpio_set_pin_high(RESPONSE_A);
			test_A_flag = 0;
		}
		
        if(test_B_flag)
        {   
			//busy_delay_us(100);
	        gpio_set_pin_low(RESPONSE_B);
	        busy_delay_us(5);
	        gpio_set_pin_high(RESPONSE_B);
	        test_B_flag = 0;
        }
		
        if(test_C_flag)
        {
	        gpio_set_pin_low(RESPONSE_C);
	        busy_delay_us(5);
	        gpio_set_pin_high(RESPONSE_C);
	        test_C_flag = 0;
        }				
        
		
		
    }
}
