// Kevin Bui, 12/9/2025
// DC Motor Controller 

// include header file
#include "DCMotor.h"

// main function
int main(void){
	// initialize motor
	runMotor();
	
	// infinite loop for motor controlling and OLED display
	while(1){
		readMotor();
	}
}