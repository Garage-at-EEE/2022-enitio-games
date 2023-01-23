#include "motorControl.h"

bool isMovingForward = false;

void motors_init(void) { 
    
    // sets the pins as outputs:
    pinMode(motor1Pin1, OUTPUT);
    pinMode(motor1Pin2, OUTPUT);
    pinMode(enable1Pin, OUTPUT);

    pinMode(motor2Pin1, OUTPUT);
    pinMode(motor2Pin2, OUTPUT);
    pinMode(enable2Pin, OUTPUT);

    // configure LED PWM functionalitites
    ledcSetup(pwmChannel1, freq, resolution);
    ledcSetup(pwmChannel0, freq, resolution);
  
    // attach the channel to the GPIO to be controlled
    ledcAttachPin(enable1Pin, pwmChannel1);
    ledcAttachPin(enable2Pin, pwmChannel0);
}


void move_forward(uint16_t Cycle){
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);

    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH);

    ledcWrite(pwmChannel1, Cycle);
    ledcWrite(pwmChannel0, Cycle);
}

void move_backward(uint16_t dCycle){
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);

    digitalWrite(motor2Pin1, HIGH);
    digitalWrite(motor2Pin2, LOW);

    ledcWrite(pwmChannel1, dCycle);
    ledcWrite(pwmChannel0, dCycle);
}

void move_left(uint16_t dCycle){
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);

    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH);

    ledcWrite(pwmChannel1, dCycle);
    ledcWrite(pwmChannel0, dCycle);
}

void left_motor_forward(uint16_t dCycle){
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);

    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, HIGH);

    ledcWrite(pwmChannel1, dCycle);
    ledcWrite(pwmChannel0, dCycle);
}

void left_motor_backward(uint16_t dCycle){
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);

    digitalWrite(motor2Pin1, HIGH);
    digitalWrite(motor2Pin2, LOW);

    ledcWrite(pwmChannel1, dCycle);
    ledcWrite(pwmChannel0, dCycle);
}

void right_motor_forward(uint16_t dCycle){
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);

    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW);

    ledcWrite(pwmChannel1, dCycle);
    ledcWrite(pwmChannel0, dCycle);
}

void right_motor_backward(uint16_t dCycle){
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);

    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW);

    ledcWrite(pwmChannel1, dCycle);
    ledcWrite(pwmChannel0, dCycle);
}

void move_right(uint16_t dCycle){
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);

    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW);

    ledcWrite(pwmChannel1, dCycle);
    ledcWrite(pwmChannel0, dCycle);
}

void stop(void){
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, LOW);
    digitalWrite(motor2Pin1, LOW);
    digitalWrite(motor2Pin2, LOW);

    ledcWrite(pwmChannel1, 0);
    ledcWrite(pwmChannel0, 0);
}
uint16_t cycle;

void read_y_axis_adc(uint32_t dCycle){ //left joystick

    if (dCycle < 1500){
        cycle = map(dCycle, 0, 2000, 4095, 0);
        right_motor_backward(cycle);
        
        //move_backward(cycle);
        //isMovingForward = false;
    }

    else if (dCycle > 1501 && dCycle < 2000){
        stop();
        isMovingForward = false;
    }

    else{
        isMovingForward = true;

        if (dCycle > 4000)
            dCycle = 2000;

        cycle = map(dCycle, 2000, 4095, 0, 4000);
        //move_forward(cycle);
        right_motor_forward(cycle);
    }
}

void read_x_axis_adc(uint16_t dCycle, uint16_t limits){ //right joystick
   /* if (isMovingForward && dCycle > 1701 && dCycle < 1930){
        move_left(random(949,950));
    }
    
    else {
        if (dCycle < 1700){
            cycle = map(dCycle, 0, 1880, 3000, 0);
            move_right(cycle);
        }
        else if (dCycle > 1701 && dCycle < 1930){
            stop();
        }
        else{
            cycle = map(dCycle, 1931, 4095, 0, 3000);
            move_left(cycle);
        }
    }*/
    Serial.println(dCycle);

    if (dCycle < 1500){
            cycle = map(dCycle, 0, 2000, 4000, 0);
            left_motor_backward(cycle);
            //Serial.println("x move right");
    }
    else if (dCycle > 1501 && dCycle < 2000){
        stop();
    }
    else{
        cycle = map(dCycle, 2000, 4095, 0, limits);
        left_motor_forward(cycle);
        //Serial.println("x move left");
    }

}

void disconnected_state(void){
    stop();
}