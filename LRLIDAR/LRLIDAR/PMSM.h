/*
 * The MIT License (MIT)
 * 
 * Copyright (c) 2013 Pavlo Milo Manovi
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file	PMSM.h
 * @author 	Pavlo Manovi
 * @date 	July, 2013
 * @brief 	Provides methods for controlling PMSM motors sinusoidally.
 * 
 * This library provides methods for controlling torque, position, and air
 * gap flux linkages of permemant magnet synchronous motors, sinusoidally.
 * 
 */

#ifndef PMSM_H
#define PMSM_H

#include <stdint.h>

#if defined(__dsPIC33FJ128MC802__)
#define FCY 40000000
#warning The 33F PMSM implementation is unsupported at this time.
#endif
#if defined(__dsPIC33EP256MC506__)
#define FCY 70000000
#endif

#define MOTOR_PWM_FREQ 20000

//Provided for implementation.  Use floats.
#define RADTODEG(x) ((x) * 57.29578)
#define DEGTORAD(x) ((x) * 0.0174532925))

/**
 * @brief A structure which holds information about the motor, and driver.
 *
 * A variable of type MotorInfo must be declared in a scope that allows its address to
 * be passed to PMSM_Init.
 */
typedef struct {
    double t1;
    double t2;
    double t3;
    uint8_t newData;
} MotorInfo;

/**
 * @brief PMSM initialization call. Sets up hardware and timers.
 * @param *information a pointer to the MotorInfo struct that will be updated.
 * @return Returns 1 if successful, returns 0 otherwise.
 *
 * This needs to be called after all other hardware peripherals have been initialized.
 * It is up to you to set newData to zero, it is set to one every time a new position is commanded.
 * Setting newData to zero isn't required and newData itself is provided only for implementation purposes.
 * 
 * PMSM_Init uses Timer2 and sets up the processor.
 */
uint8_t PMSM_Init(MotorInfo *information);

/**
 * @brief calculates PMSM vectors and updates the init'd MotorInfo struct with duty values.
 *
 * This should be called after one is done updating position, field weakening, and torque.
 * Call only once after all three are updated, not after setting each individual parameter.
 */
void PMSM_Update(void);

/**
 * @brief Sets the commanded position of the motor.
 * @param pos The position of the rotor in radians from 0 - 2pi / n-poles.
 * 
 * This will set the position of the rotor to the commanded angle divided by the number
 * of poles in the motor divided by three.  If the motor has six poles, it will take two
 * commanded rotations from zero to 2*pi radians to complete one full rotation of the rotor.
 * The passed value will be updated at 100Hz.
 */
void SetPosition(float pos);

/**
 * @brief Sets the commanded torque of the motor.
 * @param power Percentage of torque requested from 0 - 100%.
 * 
 * Scales the pulse-width-modulation duty cycles output from the PMSM algorithm
 * by zero to 100% of generated.  Torque is continuous throughout the rotation of
 * the motor.
 * The passed value will be updated at 100Hz.
 */
void SetTorque(uint8_t power);

/**
 * @brief Sets the air gap flux linkage value.
 * @param Id Air gap flux linkage timing advance.  ~0 to -2.5.
 * 
 * Field weakening changes the air gap flux linkage, this can cunteract BEMF and allow
 * a motor to spin faster than its rated speed.  Make sure you know what you are doing
 * with this value or you can demagnetize your motor magnets, melt windings, or rip your
 * motor apart.
 *
 * Suggested reading for air-gap flux linkage control:
 * http://ww1.microchip.com/downloads/en/AppNotes/01078B.pdf
 *
 * The passed value will be updated at 100Hz.
 */
void SetAirGapFluxLinkage(float Id);


#endif    /*PMSM_H  */
