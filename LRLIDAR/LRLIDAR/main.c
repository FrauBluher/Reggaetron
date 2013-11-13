/*
 * File:   example_main.c
 * Author: Pavlo Milo Manovi
 *
 *
 * If being used with the CAN enabled PMSM Board obtained at either pavlo.me or off of
 * http://github.com/FrauBluher/ as of v 1.6 the following pins are reserved on the
 * dsPIC33EP256MC506:
 *
 * THIS NEEDS TO BE UPDATED FOR v1.6
 */

//#define SINUSOIDAL

#include <xc.h>
#include <string.h>
#include <stdlib.h>
#include <uart.h>
#include "PMSM.h"
#include "LRLIDARBoard.h"
#include "adc.h"
#include "PMSM_Sinusoidal_Drive.h"

_FOSCSEL(FNOSC_FRC & IESO_OFF & PWMLOCK_OFF);
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF & POSCMD_NONE);
_FWDT(FWDTEN_OFF);
_FICD(ICS_PGD1 & JTAGEN_OFF);

MotorInfo motorInformation;
uint16_t CDARRaw[180];
uint8_t CDARString[1024];
uint16_t torque = 15;
uint16_t adcBuffer;

extern uint16_t adcDmaBuffer[16];
uint8_t updateFlag = 0;
uint8_t direction = 0;
float position = 0;
char counter;

int main(void)
{
	PinInit();
	ClockInit();
	ADCInit();
	PMSM_Init(&motorInformation);
	TimersInit();
	MotorInit();
	SetTorque(25);
	uint16_t test;
	while (1) {
		char i = 0;
		if (updateFlag) {
			for(i = 0; i < 90; i++) {
				itoa(CDARString, CDARRaw[i], 10);
			}
			for(i = 0; i < 512; i++) {
				WriteUART2(CDARString[i]);
				while(BusyUART2());
			}
			updateFlag = 0;
		}
	};
	//Sit and Spin
}

void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void)
{
	IFS0bits.T1IF = 0; // Clear Timer1 Interrupt Flag
}

void __attribute__((__interrupt__, no_auto_psv)) _T2Interrupt(void)
{
	if (direction) {
		counter++;
		position += .19;
		SetPosition(position);
		if (position >= 17) {
			direction = 0;
			updateFlag = 1;
		}
	} else {
		counter--;
		position -= .19;
		SetPosition(position);
		if (position <= -17) {
			direction = 1;
		}
	}

	CDARRaw[counter] = adcBuffer;

	PMSM_Update();

	PDC1 = motorInformation.t1 / 10;
	PDC2 = motorInformation.t2 / 10;
	PDC3 = motorInformation.t3 / 10;



	updateFlag = 0;
	IFS0bits.T2IF = 0; // Clear Timer1 Interrupt Flag
}

void __attribute__((interrupt, no_auto_psv))_AD1Interrupt(void)
{
	adcBuffer = ADC1BUF0;
	;
	IFS0bits.AD1IF = 0;
}