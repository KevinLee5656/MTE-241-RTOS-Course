#include <lpc17xx.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#define partNumber 2

// Part 1 -- Write a program to continually read the value of the pushbutton, and turn the topmost
// (or leftmost) LED on when the button is down and off when the button is up.
#if partNumber == 1
int main(void) {
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

	while(true) {
		int button_pushed = (LPC_GPIO2->FIOPIN & 0x400); // Port 2, Pin 10
		if(button_pushed == 0) {
			printf("Button is pushed.\n");
			LPC_GPIO2->FIOSET |= (1 << 6); // turn leftmost LED on
		}
		else {
			printf("Button is not pushed.\n");
			LPC_GPIO2->FIOCLR |= (1 << 6); // turn leftmost LED off
		}
	}
}

// Part 2 -- Write a program to continually read the joystick and print a message out the serial port
// that indicates whether the joystick position is North, South, East, West or Centre, and whether 
// or not it�s pressed in.
#elif partNumber == 2
int main(void) {
	while(true) {
		int joystick_pressed = (LPC_GPIO1->FIOPIN & 0x100000); // Port 1, Pin 20 (Cntr)
		
		int joystick_north = (LPC_GPIO1->FIOPIN & 0x800000); // Port 1, Pin 23 (A)
		int joystick_east = (LPC_GPIO1->FIOPIN & 0x1000000); // Port 1, Pin 24 (B)
		int joystick_south = (LPC_GPIO1->FIOPIN & 0x2000000); // Port 1, Pin 25 (D)
		int joystick_west = (LPC_GPIO1->FIOPIN & 0x4000000); // Port 1, Pin 26 (C)
		
		if (joystick_pressed == 0) printf("pressed in.\n");
		else printf("not pressed in.\n");
		
		if (joystick_north == 0) printf("North and ");
		else if (joystick_east == 0) printf("East and ");
		else if (joystick_south == 0) printf("South and ");
		else if (joystick_west == 0) printf("West and ");
		else printf("Centre and ");	
	}
}

// Part 3 -- Write a program to repeatedly read a string that�s typed by the user into the serial port, 
// convert that string into a number, and display the number in binary on the LEDs. The number 
// can be in the range of 0 to 255. When entering text through the serial port, you should end the
// input by pressing ctrl+J which is the ASCII newline character. So for example, if the user enters
// 75 and presses ctrl+J, the LEDs should display 1001011
#elif partNumber == 3
int main(void) {
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
	
	while(true) {		
		char str[10];
		printf( "Enter a number from 0 to 255 then press ctrl+J: ");
		scanf("%s", str);
		
		// clear LED pins as a reset
		LPC_GPIO1->FIOCLR |= (1 << 28);
		LPC_GPIO1->FIOCLR |= (1 << 29);
		LPC_GPIO1->FIOCLR |= (1u << 31);
		LPC_GPIO2->FIOCLR |= (1 << 2);
		LPC_GPIO2->FIOCLR |= (1 << 3);
		LPC_GPIO2->FIOCLR |= (1 << 4);
		LPC_GPIO2->FIOCLR |= (1 << 5);
		LPC_GPIO2->FIOCLR |= (1 << 6);
		
		int num = atoi(str);
		if (num < 0 || num > 255){
			printf("Number out of range!\n");
			continue;
		}
		
		for(int i = 0; num > 0; i++){
			if(num % 2 == 1) {
				if(i == 0) LPC_GPIO1->FIOSET |= (1 << 28); // 2^0
				else if(i == 1) LPC_GPIO1->FIOSET |= (1 << 29); // 2^1
				else if(i == 2) LPC_GPIO1->FIOSET |= (1u << 31); // 2^2
				else if(i == 3) LPC_GPIO2->FIOSET |= (1 << 2); // 2^3
				else if(i == 4) LPC_GPIO2->FIOSET |= (1 << 3); // 2^4
				else if(i == 5) LPC_GPIO2->FIOSET |= (1 << 4); // 2^5
				else if(i == 6) LPC_GPIO2->FIOSET |= (1 << 5); // 2^6
				else LPC_GPIO2->FIOSET |= (1 << 6);
			}
			num /= 2; // rounding down
		}
	}
}

// Part 4 -- Write a program to continually read the value of the analog to digital converter, and
// print the value out the serial port as a decimal number
#else
int main(void) {
	// Setup PCONP
	LPC_SC->PCONP |= (1 << 12); 

	// Setup PINSEL
	LPC_PINCON->PINSEL1 &= ~(0x3 << 18); // clear bits 18 and 19
	LPC_PINCON->PINSEL1 |=  (0x1 << 18); // set bit 18 to 01 = AD0.2

	// Setup ADC control register
	LPC_ADC->ADCR = (1 <<  2) |  // turn on analog input bit 2
	                (4 <<  8) |  // bit 8..15 set clock divisor: 25MHz / (4+1) = 5MHz     
	                (1 << 21);   // enable ADCR circuitry
	
	while(true) {
		LPC_ADC->ADCR |= (1 << 24); // start conversion
    while((LPC_ADC->ADGDR & 0x80000000) == 0); // checking if conversion is complete
    uint16_t ADC_count = ((LPC_ADC->ADGDR & 0xFFF0) >> 4); // extract bits 4..15
    printf("ADC Value: %f\n", roundf((ADC_count * 3.3/4095) * 100) / 100);
	}
}

#endif
