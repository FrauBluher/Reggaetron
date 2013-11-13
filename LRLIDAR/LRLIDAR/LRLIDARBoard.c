#include <xc.h>
#include <stdint.h>
#include "LRLIDARBoard.h"
#include "hspwm.h"
#include "dma.h"
#include "adc.h"
#include "pps.h"

static volatile uint16_t adcDmaBuffer[16] __attribute__((aligned(32)));

void UART2Init(void)
{
	U2MODEbits.UARTEN = 0; // Disable the port
	U2MODEbits.USIDL = 0; // Stop on idle
	U2MODEbits.IREN = 0; // No IR decoder
	U2MODEbits.RTSMD = 0; // Ready to send mode (irrelevant)
	U2MODEbits.UEN = 0; // Only RX and TX#ifdef __dsPIC33FJ128MC802__
	U2MODEbits.WAKE = 1; // Enable at startup
	U2MODEbits.LPBACK = 0; // Disable loopback
	U2MODEbits.ABAUD = 0; // Disable autobaud
	U2MODEbits.URXINV = 0; // Normal operation (high is idle)
	U2MODEbits.PDSEL = 0; // No parity 8 bit
	U2MODEbits.STSEL = 0; // 1 stop bit
	U2MODEbits.BRGH = 0;

	IPC7 = 0x4400;
	// U2STA Register
	// ==============
	U2STAbits.URXISEL = 0; // RX interrupt on every character
	U2STAbits.OERR = 0; // clear overun error

	// U2BRG Register
	// ==============
	U2BRG = 26; //115200

	// Enable the port;
	U2MODEbits.UARTEN = 1; // Enable the port
	U2STAbits.UTXEN = 1; // Enable TX

	RPINR19bits.U2RXR = 97;		//RX
	RPOR7bits.RP56R = 0x0005;	//TX
}

void MotorInit()
{
	/* Set PWM Period on Primary Time Base */
	PTPER = 1000;
	/* Set Phase Shift */
	PHASE1 = 0;
	SPHASE1 = 0;
	PHASE2 = 0;
	SPHASE2 = 0;
	PHASE3 = 0;
	SPHASE3 = 0;
	/* Set Duty Cycles */
	PDC1 = 0;
	SDC1 = 1000;
	PDC2 = 0;
	SDC2 = 1000;
	PDC3 = 0;
	SDC3 = 1000;
	/* Set Dead Time Values */
	DTR1 = DTR2 = DTR3 = 0;
	ALTDTR1 = ALTDTR2 = ALTDTR3 = 0;
	/* Set PWM Mode to Independent */
	IOCON1 = IOCON2 = IOCON3 = 0xCC00;
	/* Set Primary Time Base, Edge-Aligned Mode and Independent Duty Cycles */
	PWMCON1 = PWMCON2 = PWMCON3 = 0x0000;
	/* Configure Faults */
	FCLCON1 = FCLCON2 = FCLCON3 = 0x0003;
	/* 1:1 Prescaler */
	PTCON2 = 0x0000;
	/* Enable PWM Module */
	PTCON = 0x8000;

	TRISCbits.TRISC1 = 0;
	TRISCbits.TRISC2 = 0;
	LATCbits.LATC1 = 1;
	LATCbits.LATC2 = 0;
}

void ClockInit(void)
{
	// 140.03 MHz VCO  -- 70 MIPS
	PLLFBD = 74;
	CLKDIVbits.PLLPRE = 0;
	CLKDIVbits.PLLPOST = 0;

	// Initiate Clock Switch to FRC oscillator with PLL (NOSC=0b001)
	__builtin_write_OSCCONH(0x01);

	__builtin_write_OSCCONL(OSCCON | 0x01);

	// Wait for Clock switch to occur
	while (OSCCONbits.COSC != 0b001);

	// Wait for PLL to lock
	while (OSCCONbits.LOCK != 1);
}

void PinInit(void)
{
	TRISCbits.TRISC8 = 0;
	LATCbits.LATC8 = 1;
	TRISDbits.TRISD6 = 0;
	LATDbits.LATD6 = 1;

	LED1_TRIS = 0;
	LED2_TRIS = 0;
	LED3_TRIS = 0;

	HALLA_TRIS = 1;
	HALLB_TRIS = 1;
	HALLB_TRIS = 1;

	ANSELCbits.ANSC0 = 0;
	ANSELCbits.ANSC1 = 0;
	ANSELCbits.ANSC2 = 0;

}

void TimersInit(void)
{
	//Timer1 Init 500Hz.
	T1CONbits.TON = 0;
	T1CONbits.TCS = 0;
	T1CONbits.TGATE = 0;
	T1CONbits.TCKPS = 0b11; // Select 1:256 Prescaler
	TMR1 = 0x00;
	PR1 = 400;
	IPC0bits.T1IP = 0x01;
	IFS0bits.T1IF = 0;
	IEC0bits.T1IE = 1;
	T1CONbits.TON = 1;

	T2CON = 0; // Ensure Timer 2 is in reset
	IFS0bits.T2IF = 0; // Clear Timer2 interrupt flag status bit
	IPC1bits.T2IP2 = 1; // Set interrupt priority
	IEC0bits.T2IE = 1; // Enable Timer 2 interrupt
	PR2 = 1000; // Load the timer period value with the prescalar value
	T2CON = 0x8030; // Set Timer2 control register TON = 1, TCKPS1:TCKPS0 = 0b11 (1:256 prescaler)
	T2CONbits.TCKPS = 3; // Set Timer2 control register TON = 1, TCKPS1:TCKPS0 = 0b11 (1:256 prescaler)
	// Use system clock Fosc/2 = 40 MHz. Result is a 156,250 Hz clock.
}

void ADCInit(void)
{
	AD1CON1 = 0; //Clear CON1
	AD1CON1bits.SSRC = 0b111; //Internal counter (auto-convert)
	AD1CON1bits.ASAM = 1; //Sampling begins immediately after last conversion
	AD1CON2 = 0; //Clear CON2
	AD1CON3 = 0; //Clear CON3
	AD1CON3bits.SAMC = 0x0F; //Auto-sampling time: TAD = 15
	AD1CON4 = 0; //Clear CON4
	ANSELA = ANSELB = 0x0000; //Clear ANSEL registers, set to digital I/O
	ANSELAbits.ANSA0 = 1; //Set AN3/RB1 to analog
	AD1CHS0 = 0; //Clear Channel 0 select
	AD1CHS0bits.CH0SA = 0x0; //Set input to AN3 (RB1)
	AD1CSSLbits.CSS0 = 1; //Scan AN3 (RB1)

	ConfigIntADC1(ADC_INT_ENABLE & ADC_INT_PRI_3); //Configure ADC1 interrupt (priority 3)

	AD1CON1bits.ADON = 1; //Enable ADC1
}
