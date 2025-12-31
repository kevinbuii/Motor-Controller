// include header
#include "DCMotor.h"

// global variables
volatile int counter = 0;
volatile int sensor_counter = 0;
volatile double RPM = 0;
volatile char dutyCycle = 0;
volatile char lastInput = 0;
int count_int = 0;

double dc_adj = 0.1;
int n = 1;

// ISR for timer0 overflow for RPM calculations
ISR(TIMER0_COMPA_vect){
	// increment counter 
	counter++;
	// number of wheel openings = 2, 
	int k = 2;
	if (counter >= 200){
		// replace w/ RPM calculations
		RPM = ((sensor_counter/((256*0.0625*0.000001*1024)*200))*60)/k;
		sensor_counter = 0;
		// reset timer0 
		counter = 0;
	}
}

// ISR for falling edge of PD2
ISR(INT0_vect, ISR_BLOCK){
	sensor_counter++;
}

// ISR for timer1A interrupt, turns off PE2
ISR(TIMER1_COMPA_vect){
	PORTE &= ~(1<<DCmotor);
}

// ISR for time1B interrupt, turns on PE2
ISR(TIMER1_COMPB_vect){
	PORTE |= (1<<DCmotor);
}

// OLED display
void OLED_display(void){
	OLED_SetCursor(0,0);
	OLED_Printf("Kevin Bui");
	OLED_SetCursor(2,0);
	OLED_Printf("Level: ");
	OLED_DisplayNumber(C_DECIMAL_U8,count_int,2);
	OLED_SetCursor(4,0);
	OLED_Printf("Duty Cycle: ");
	OLED_DisplayNumber(C_DECIMAL_U8,dutyCycle,3);
	OLED_Printf("%%");
	OLED_SetCursor(6,0);
	OLED_Printf("RPM: ");
	OLED_DisplayNumber(C_DECIMAL_U8,RPM,5);
}

// motor initialization 
void runMotor(void){
	// set up PORTB to be input and pin-up resistors
	DDRB = 0x00;
	PORTB = 0xA0; 
	
	// set motor pin to output
	DDRE |= (1<<DCmotor);
	OCR1A = 0xFFFF;
	OCR1B = 0xFFFF;
	
	// initialize timer1
	TCCR1A |= (1<<COM1A0) | (1<<COM1B0);
	TCCR1B |= (1<<CS10); // no prescaler
	TIMSK1 |= (1<<OCIE1A) | (1<<OCIE1B);
	
	sei();
	
	// sensor setup
	DDRD &= ~(1<<SensorPin);
	PORTD |= (1<<SensorPin);
	
	// initialize timer0
	TCCR0A |= (1<<WGM01);
	TCCR0B |= (1<<CS00) | (1<<CS02); // 1024 prescaler
	TIMSK0 |= (1<<OCIE0A);
	OCR0A = 0xFF;
	
	sei();
	
	// initialize int0
	EIMSK |= (1<<INT0);
	EICRA |= (1<<ISC01);
	
	sei();
	
	OLED_Init();
	OLED_Clear();
}

// motor controller 
void readMotor(void){
	// initialize joystick
	char joystick = (0b11110000&PINB);
	joystick = joystick ^ 0xF0; 
	
	if (lastInput != joystick){
		if (joystick & (1<<up_count)){
			if (count_int < 10) count_int++;
		}
		if (joystick & (1<<down_count)){
			if (count_int > 0) count_int--;
		}
		dutyCycle = count_int * 10;
		dc_adj = dutyCycle/100.0;
		lastInput = joystick;
	}
	
	// reassigning timer1 value for on-time pulse
	OCR1A = 0xFFFF * dc_adj/n;
	
	// if its less than 10% then turn off motor
	if (dc_adj < 0.1){
		TIMSK1 &= ~((1 << OCIE1A) | (1 << OCIE1B)); // disable interrupts
		PORTE &= ~(1<<DCmotor);
	}
	else {
		TIMSK1 |= (1 << OCIE1A) | (1 << OCIE1B); // enable interrupts 
		OCR1A = 0xFFFF * dc_adj/n;
	}
	
	OLED_display();
}