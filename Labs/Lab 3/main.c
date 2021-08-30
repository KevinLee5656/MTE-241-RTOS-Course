#include <LPC17xx.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <cmsis_os2.h>

bool button_press_LED_on = true;
bool button_prev_state_pushed = false;

/*
Continuously read the joystick, and display a number in binary on the bottom four LEDs
that indicates the direction of the joystick (1 for North, 2 for East, 3 for South, 4 for West,
0 for no direction) and turn on LED 5 if (and only if) the joystick is pressed in
*/
void read_joystick(void *arg) {
	while(true) {
		// clear LED pins
		LPC_GPIO1->FIOCLR |= (1 << 28);
		LPC_GPIO1->FIOCLR |= (1 << 29);
		LPC_GPIO1->FIOCLR |= (1u << 31);
		LPC_GPIO2->FIOCLR |= (1 << 3);
		
		// assign joystick input values into variables
		int joystick_pressed = (LPC_GPIO1->FIOPIN & 0x100000); // Port 1, Pin 20 (Cntr)
		int joystick_north = (LPC_GPIO1->FIOPIN & 0x800000); // Port 1, Pin 23 (A)
		int joystick_east = (LPC_GPIO1->FIOPIN & 0x1000000); // Port 1, Pin 24 (B)
		int joystick_south = (LPC_GPIO1->FIOPIN & 0x2000000); // Port 1, Pin 25 (D)
		int joystick_west = (LPC_GPIO1->FIOPIN & 0x4000000); // Port 1, Pin 26 (C)
		
		// LED response
		if(joystick_pressed == 0) {
			LPC_GPIO2->FIOSET |= (1 << 3); // turn on LED 5
		}
		else {
			LPC_GPIO2->FIOCLR |= (1 << 3); //turn off LED 5
		}
		
		if(joystick_north == 0) {
			LPC_GPIO1->FIOSET |= (1 << 28); // 0b 0001
		}
		else if(joystick_east == 0) {
			LPC_GPIO1->FIOSET |= (1 << 29); // 0b 0010
		}
		else if(joystick_south == 0) {
			LPC_GPIO1->FIOSET |= (1 << 28); // 0b 0010
			LPC_GPIO1->FIOSET |= (1 << 29); // 0b 0011
		}
		else if(joystick_west == 0) {
			LPC_GPIO1->FIOSET |= (1u << 31); // 0b 0100
		}
		else {
		}
		osDelay(50);
	}
}

/*
Continuously read values from the ADC and print them out the serial port.
*/
void read_ADC(void *arg) {
	// Setup PCONP
	LPC_SC->PCONP |= (1 << 12); 

	// Setup PINSEL
	LPC_PINCON->PINSEL1 &= ~(0x3 << 18); // clear bits 18 and 19
	LPC_PINCON->PINSEL1 |=  (0x1 << 18); // set bit 18 to 01 = AD0.2

	// Setup ADC control register
	LPC_ADC->ADCR = (1 <<  2) |  // turn on analog input bit 2
	                (4 <<  8) |  // bit 8..15 set clock divisor: 25MHz / (4+1) = 5MHz     
	                (1 << 21);   // enable ADCR circuitry
	
	while(true){
		LPC_ADC->ADCR |= (1 << 24); // start conversion
    while((LPC_ADC->ADGDR & 0x80000000) == 0); // checking if conversion is complete
    uint16_t ADC_count = ((LPC_ADC->ADGDR & 0xFFF0) >> 4); // extract bits 4..15
    printf("ADC Value: %f\n", roundf((ADC_count * 3.3/4095) * 100) / 100);
		osThreadYield();
	}
}

/*
Toggle the top LED on or off each time the pushbutton is pressed and released. So each
sequence of button press, button release should change the LED from off to on or from
on to off.
*/
void toggle_LED(void *arg) {
	while(true) {
		int button_pushed = (LPC_GPIO2->FIOPIN & 0x400); // Port 2, Pin 10
		
		if(button_pushed == 0) {
			button_prev_state_pushed = true;
		}
		else if(button_press_LED_on && button_prev_state_pushed) { // pre-requisite: button was pushed AND it's time to turn LED on
			LPC_GPIO2->FIOSET |= (1 << 6); // turn on LED
			button_press_LED_on = false;
			button_prev_state_pushed = false;
		}
		else if(!button_press_LED_on && button_prev_state_pushed) { // pre-requisite: button was pushed AND it's time to turn LED off
			LPC_GPIO2->FIOCLR |= (1 << 6); // turn off LED
			button_press_LED_on = true;
			button_prev_state_pushed = false;
		}
		else {
		}
		osDelay(50);
	}
}


int main() {
	// set LED pins to output
	LPC_GPIO1->FIODIR |= (1 << 28);
	LPC_GPIO1->FIODIR |= (1 << 29);
	LPC_GPIO1->FIODIR |= (1u << 31);
	LPC_GPIO2->FIODIR |= (1 << 2);
	LPC_GPIO2->FIODIR |= (1 << 3);
	LPC_GPIO2->FIODIR |= (1 << 4);
	LPC_GPIO2->FIODIR |= (1 << 5);
	LPC_GPIO2->FIODIR |= (1 << 6);
	
	// clear LED pins
	LPC_GPIO1->FIOCLR |= (1 << 28);
	LPC_GPIO1->FIOCLR |= (1 << 29);
	LPC_GPIO1->FIOCLR |= (1u << 31);
	LPC_GPIO2->FIOCLR |= (1 << 2);
	LPC_GPIO2->FIOCLR |= (1 << 3);
	LPC_GPIO2->FIOCLR |= (1 << 4);
	LPC_GPIO2->FIOCLR |= (1 << 5);
	LPC_GPIO2->FIOCLR |= (1 << 6);
	
	osKernelInitialize();
	osThreadNew(read_joystick, NULL, NULL);
	osThreadNew(read_ADC, NULL, NULL);
	osThreadNew(toggle_LED, NULL, NULL);
	osKernelStart();
}
