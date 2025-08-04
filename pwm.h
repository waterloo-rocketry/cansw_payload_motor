/* 
 * File:   pwm.h
 * Author: cstubbs
 *
 * Created on August 2, 2025, 10:25 PM
 */

#ifndef PWM_H
#define	PWM_H

#define SERVO_STOP_PWM  1500            // microseconds    
#define SERVO_RIGHT_PWM 700             // microseconds
#define SERVO_LEFT_PWM  2300            // microseconds
#define PWM_PERIOD      1000            // change later, place holder value

void set_motor_pwm(uint8_t duty);

#endif	/* PWM_H */

