// motor controlling header file
#ifndef DCMOTOR_H_
#define DCMOTOR_H_

// defines
#define F_CPU 16000000
#define SensorPin PD2
#define DCmotor PE2
#define up_count PB5
#define down_count PB7

// includes
#include <avr/io.h>
#include <avr/interrupt.h>
#include "i2c.h"
#include "SSD1306.h"

// function include
void runMotor(void);
void readMotor(void);

#endif /* DCMOTOR_H_ */