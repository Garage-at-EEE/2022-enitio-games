#include <Arduino.h>

// Motor A
#define motor1Pin1  27 
#define motor1Pin2  26 
#define enable1Pin  14 

#define motor2Pin1  25 
#define motor2Pin2  33 
#define enable2Pin  32 

const uint16_t freq = 5000;
const uint8_t pwmChannel0 = 0;
const uint8_t pwmChannel1 = 1;
const int resolution = 12;
//uint16_t cycle;


void motors_init(void);
void disconnected_state(void);
void move_forward(uint16_t Cycle);
void move_backward(uint16_t dCycle);
void move_left(uint16_t dCycle);
void move_right(uint16_t dCycle);
void read_y_axis_adc(uint32_t dCycle);
void read_x_axis_adc(uint16_t dCycle, uint16_t limits);
void stop(void);
