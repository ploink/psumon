/*
 * PsuMon.c
 *
 * Created: 15-05-2015 15:36:21
 *  Author: joost
 */ 

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/eeprom.h>
#include "bits.h"

const char fusedata[] __attribute__ ((section (".fuse"))) = {0xE2, 0xDC, 0xFF};
uint8_t EEMEM EE_REF3=0xff;
uint8_t EEMEM EE_REF5=0xff;
uint8_t EEMEM EE_REF12=0xff;


#define	LED_3V	BIT(PORTB,0)
#define	LED_5V	BIT(PORTB,1)
#define	LED_12V	BIT(PORTB,2)
#define LED_PWR	BIT(PORTA,7)
#define BUZZER	BIT(PORTA,3)
	
#define MUX_3V		0b10000000
#define MUX_5V		0b10000001
#define MUX_12V		0b10000010
#define StartADC(x)	ADMUX=MUX_##x;BIT(ADCSRA,ADSC)=1
#define GetADC(x)	while (BIT(ADCSRA,ADSC)); x=ADCH;


// Calculated calibration values. These should work OK if you are lucky
byte REF3	 =  3.3*(100.0/(100+270))*255/1.1;
byte REF5	 =  5.0*(100.0/(100+470))*255/1.1;
byte REF12 =  12.0*(100.0/(100+1200))*255/1.1;

static const byte RIPMAX3 = 0.050*255/1.1;
static const byte RIPMAX5 = 0.050*255/1.1;
static const byte RIPMAX12 = 0.120*255/1.1;

static const word pattern[] = {0,0xffff,0x7fff,0x5fff,0x57ff,0x55ff,0x557f};

int main(void)
{	
	// Set System Clock prescaler to 1, just in case CKDIV8 fuse is programmed.
	CLKPR=128; CLKPR=0;


	// Init I/O
	DDRA=0b10001000; PORTA=0;
	DDRB=0b00000111; PORTB=0;
	DIDR0=0b00000111; // Disable digital input buffers on analog pins.	

	LED_PWR=1;		// Enable power led immediately on boot.

	byte b; byte c;  // General purpose vars.
	byte calibrate;
	word blink3=0; word blink5=0; word blink12=0; word blinkPWR=0xffff; word beep=0; word bitpos=0x8000;
	byte rip3=0; byte rip5=0; byte rip12=0;
	byte dev3=0; byte dev5=0; byte dev12=0;
	byte counter=0;

	// Read calibration values from EEPROM
	b=eeprom_read_byte(&EE_REF3);
	if (b==255) calibrate=1;
	else {
		REF3=b;
		REF5=eeprom_read_byte(&EE_REF5);
		REF12=eeprom_read_byte(&EE_REF12);
		calibrate=0;
	}
	
	// Init ADC
	ADCSRA=0b10000101;	// Enable with prescaler 32. 
	// Conversion time: 14 ADC cycles * 32 system cyles / 8MHz = 56us
	ADCSRB=0b00010000;	// ADC result left adjusted.	
	StartADC(3V);		// First conversion to init ADC.
	//GetADC(b);			// Wait for ADC to complete. Not needed because following delay.

	// PSU must settle in 500ms. When MCU starts, The 12V rail passed 4.3V brownout plus some 7805 voltage drop.
	// We guess it should be up in 300ms. If not, this results in a fault indication.
	_delay_ms(300);
	
	// Wait longer if we connect it for calibration.
    if (calibrate) _delay_ms(2000);
	
    while(1)
    {		
		counter++;

		// Init min/max values.		
		byte min3 = 255; byte min5 = 255; byte min12 = 255;
		byte max3 =   0; byte max5 =   0; byte max12 =   0;
		word sum3 =   0; word sum5 =   0; word sum12 =   0;
		
		// loop 256 times using only 8-bit counter. Sample all three voltages and get min/max/avg values.
		// Total time = 3 * conversion time (56us) * 256 = 43ms
		byte i=0; do {
			StartADC(3V);  GetADC(b);
			if (b<min3) min3=b; 
			if (b>max3) max3=b; 
			sum3+=b;
			StartADC(5V);  GetADC(b);
			if (b<min5) min5=b; 
			if (b>max5) max5=b; 
			sum5+=b;
			StartADC(12V); GetADC(b);
			if (b<min12) min12=b; 
			if (b>max12) max12=b; 
			sum12+=b;
		} while (++i);
			
		sum3>>=8; sum5>>=8;	sum12>>=8;
			
		// calculate maximum peak-peak ripple from min/max values.
		b=max3-min3;   if (b>rip3)    rip3=b;
		b=max5-min5;   if (b>rip5)    rip5=b;
		b=max12-min12; if (b>rip12)   rip12=b;
		
		// calculate deviation form reference values
  		if (sum3>REF3)   b=sum3-REF3;   else b=REF3-sum3;   if (b>dev3) dev3=b;
		if (sum5>REF5)   b=sum5-REF5;   else b=REF5-sum5;   if (b>dev5) dev5=b;
		if (sum12>REF12) b=sum12-REF12; else b=REF12-sum12; if (b>dev12) dev12=b;					

  	    if (calibrate) {
			// Device not calibrated. We should do it now if values are plausible.
			// If not then signal with 5V LED and freeze. 
			// It probably means we just programmed the chip, otherwise a hardware fault.
			if (dev3>20 || dev5>20 || dev12>20) { 
				LED_5V=1; 
				while (1); 
			}
			// Save calibration values.
			REF3=sum3; REF5=sum5; REF12=sum12;
			eeprom_write_byte(&EE_REF3,sum3);
			eeprom_write_byte(&EE_REF5,sum5);
			eeprom_write_byte(&EE_REF12,sum12);
			LED_3V=1; 
			while (1);
	    }

		if (counter==4) {
		  counter=0;		  		  
 		  // Do blinky thing
		  if (blink3&bitpos)	LED_3V=1; else LED_3V=0;
		  if (blink5&bitpos)	LED_5V=1; else LED_5V=0;
		  if (blink12&bitpos)	LED_12V=1; else LED_12V=0;
		  if (blinkPWR&bitpos)	LED_PWR=1; else LED_PWR=0;
		  if (beep&bitpos)		BUZZER=1; else BUZZER=0;
		  bitpos=bitpos>>1;
		  
		  // after end of blink sequence then ....
		  if (bitpos==0) {
			  bitpos=0x8000;

			  // calculate new blink patterns
			  b=dev12/(byte)(REF12*5/100);		// blink once for every 5% deviation.
			  c=rip12/RIPMAX12;
			  if (c>b) b=c;
			  if (b>5) b=5;
			  byte peak=b;
			  blink12=pattern[b+1];

			  b=dev5/(byte)(REF5*5/100);
			  c=rip5/RIPMAX5;
			  if (c>b) b=c;
			  if (b>5) b=5;
			  if (b>peak) peak=b;
			  blink5=pattern[b+1];

			  if (dev3>(REF3>>1)) blink3=pattern[0]; // Led off if there is no 3.3V
			  else {				  
				  b=dev3/(byte)(REF3*5/100);		 
				  c=rip3/RIPMAX3;
				  if (c>b) b=c;
				  if (b>5) b=5;
				  if (b>peak) peak=b;
				  blink3=pattern[b+1];				  
			  }
			  
			  // "peak" contains worst measurement of all
			  blinkPWR=pattern[peak+1];

		  	  if (peak<2) peak=0;
			  else peak-=2;			  			  	
			  beep=~pattern[peak+1];		  	  
			    		  
			  // reset data
			  dev3=dev5=dev12=0;
			  rip3=rip5=rip12=0;			  
		  }
		}		
    }
}