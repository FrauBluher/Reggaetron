/* 
 * File:   PMSMBoard.h
 * Author: Pavlo
 *
 * Created on October 29, 2013, 4:55 PM
 */


#ifndef PMSMBOARD_H
#define	PMSMBOARD_H

#define LED1_TRIS   TRISCbits.TRISC10
#define LED1        LATCbits.LATC10
#define LED2_TRIS   TRISBbits.TRISB7
#define LED2        LATBbits.LATB7
#define LED3_TRIS   TRISCbits.TRISC13
#define LED3        LATCbits.LATC13
#define LED4_TRIS   TRISBbits.TRISB8
#define LED4        LATBbits.LATB8

#define HALLA_TRIS  TRISCbits.TRISC0
#define HALLB_TRIS  TRISCbits.TRISC1
#define HALLC_TRIS  TRISCbits.TRISC2
#define HALLA       PORTCbits.RC0
#define HALLB       PORTCbits.RC1
#define HALLC       PORTCbits.RC2

#include <xc.h>
#include <stdint.h>

void UART2Init();
void ClockInit();
void MotorInit();
void TimersInit();
void PinInit();
void ADCInit(void);
uint16_t *ADCRead(void);

#endif	/* PMSMBOARD_H */

