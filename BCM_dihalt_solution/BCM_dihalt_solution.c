//Clock Config
#define F_CPU 16000000L

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <avrlibtypes.h>
//#include <avrlibdefs.h>
//#include <avr/pgmspace.h>
//#include <avr/wdt.h>
#include <util/delay.h>
//#include <stdlib.h>
#include <math.h>


#define ROL(x) ((x << 1) | (x >> 7))
#define ROR(x) ((x >> 1) | (x << 7))

#define COPYBIT(dst,bit,src) 					\
do {										    \
	(dst) &=~(1<<(bit));						\
	(dst) |=((   (src) & OCR2  )?1:0)<<(bit);	\
} while(0)

typedef unsigned char byte;
typedef unsigned char boolean;


// Brightness values are stored here:
//unsigned char BRG[8]= {1,33,65,97,129,161,193,225};
unsigned char BRG[8] = {15,255,255,0,0,255,255,0};
	
unsigned char BRG0[8] = {0,0,0,0,0,0,0,0};
	
byte _COL;
	
	
unsigned char BRIGHNESS_ARR[8][8]={
	{1,1,1,1,1,1,1,1},
	{33,33,33,33,33,33,33,33},
	{65,65,65,65,65,65,65,65},
	{97,97,97,97,97,97,97,97},
		
	{129,129,129,129,129,129,129,129},
	{161,161,161,161,161,161,161,161},
	{193,193,193,193,193,193,193,193},
	{255,255,255,255,255,255,255,255},
	};
	
unsigned char BRG2[8] = {1,1,1,1,1,1,1,1};

ISR(TIMER2_COMP_vect)
{
	OCR2 = ROL(OCR2); // That thing, where we  moving 1bit in OCR with each clock cycle
	PORTC=0xff;
	PORTB=0xff;
	
	//COPYBIT(PORTD,0,BRG0[0]);
	//COPYBIT(PORTD,1,BRG0[1]);
	//COPYBIT(PORTD,2,BRG0[2]);
	//COPYBIT(PORTD,3,BRG0[3]);
	//COPYBIT(PORTD,4,BRG0[4]);
	//COPYBIT(PORTD,5,BRG0[5]);
	//COPYBIT(PORTD,6,BRG0[6]);
	//COPYBIT(PORTD,7,BRG0[7]);

	for(int i = 0; i<8; i++)
	{
		COPYBIT(PORTD,i,BRG0[i]);
		
			if(_COL>5) PORTB &= ~(1<<(_COL-5));
			if(_COL<=5) PORTC &= ~(1<<(_COL));
		


	}



}


void SetOne(byte LED, byte VAL) { BRG0[LED]=VAL; }
	
void SetOneDot(byte ROW, byte COL, byte VAL)
{
	_COL=COL;
	BRG0[ROW]=VAL;
}

void DimOneDot(byte ROW, byte COL, int delay, byte VAL)
{
	OCR1A = delay;
	
		int topVal=255;
		
		TCNT1 = 0;
		for(int i=0; i<topVal; i++) // Rolling Brightness!
		{
			_COL = COL;
			while(!(TIFR & (1 << OCF1A))) if(OCR2==64)  SetOne(ROW,i);
			TIFR |= (1 << OCF1A);
		}

}
	
void SetFew(byte LEDS, byte VAL) 
{
	for (byte i = 0; i < 8; i++)
	{
		if((LEDS >> i)&1)  BRG0[i]=VAL;
	}
}

void SetAll(byte VAL)
{
	for (byte i = 0; i<8; i++)
	{
		BRG0[i]=VAL;
	}
}


void DimFew(byte LEDS, int delay, byte topVal, boolean  dir)
{
	TCCR1B |= (1<<WGM12)|(0<<CS12)|(1<<CS11)|(1<<CS10);
	OCR1A = delay;
		
	if (dir == 1)
	{
		TCNT1 = 0;
		for(int i=0; i<topVal; i++) // Rolling Brightness!
		{
			while(!(TIFR & (1 << OCF1A))) SetFew(LEDS,i);
			TIFR |= (1 << OCF1A);
		}
	}
	else
	{
		SetFew(LEDS,topVal);

		TCNT1 = 0;
		for (int i = topVal; i>0; i--)
		{
			while(!(TIFR & (1 << OCF1A)))  SetFew(LEDS,i);
			TIFR |= (1 << OCF1A);
		}
		SetFew(LEDS,0);
	}
}

void DimOne(byte LED, int delay, byte topVal, boolean  dir) // Delay works best in range of 10 - 600
{

	 OCR1A = delay;
	//int x[] = {1,2,3,4,50,60,90,255};//{1,23,48,78,109,141,193,255}; 
	// y = 4.646464646 * 10^-1 x3 - 3.260822511 x2 + 31.79834055 x - 29.21428571
	
	 float y;
		if (dir == 1)
		{
			TCNT1 = 0;
			for(int i=0; i<topVal; i++) // Rolling Brightness!
			{	
				y = 0.4646464646 * pow(i,3) - 3.260822511 * pow(i,2) + 31.79834055 * i - 29.21428571;
				//y=pow(i,2);	
				while(!(TIFR & (1 << OCF1A))) if(OCR2==64)  SetOne(LED,i);		
				TIFR |= (1 << OCF1A);
			}		
		}
		
		//int x[] = {1,2,3,4,50,60,90,255};//{1,23,48,78,109,141,193,255};
		//
		//if(dir==1){
			//TCNT1=0;
			//int i=0;
			//while(i<length(x)){
				//while(!(TIFR & (1 << OCF1A))) if(OCR2==64)  SetOne(LED,x[i]);
				//TIFR |= (1 << OCF1A);
				//i++;				
			//}
		//}
		
		else
		{
			SetOne(LED,topVal);

			TCNT1 = 0;
			for (int i = topVal; i>0; i--)
			{
				while(!(TIFR & (1 << OCF1A))) if(OCR2==64) SetOne(LED,i);
				TIFR |= (1 << OCF1A);
			}
			SetOne(LED,0);
		}
}

void init_timer_BCM()
{
	sei(); // Allow interrupts. Yeah, because why not, interrupt me, interrupt me everybody!!!1

	TCNT2 = 0;  //Timer 2 initial value
	OCR2 = 0b10000000; // Starting upper limit of timer 2

	TIMSK |= 1<<OCF2; // Timer 2
	TCCR2 |= 1<<WGM21 | 1<<CS22 | 1<<CS21;// | 1<< CS20; // Setting Timer 2 to CTC mode, and to prescaler of 256
}

void init_timer_DIM()
{
	TCCR1B |= (1<<WGM12)|(0<<CS12)|(1<<CS11)|(1<<CS10); // initializing timer 1, that is to contol dimming timing
	
}


void main(void)
{
	volatile unsigned char a = 1;

	init_timer_BCM();
	init_timer_DIM();

	DDRD = 0xFF;  //
	PORTD = 0x00; // Row Port 
	
	// C&B both form column port
	DDRC = 0x3F;   // 0b00111111
	PORTC = ~0xC0;  // 0b11000000

	DDRB |= (1<<PB1)|(1<<PB2);	
	PORTB |= (1<<PB1)|(1<<PB2);



	while(1)
	{
	
		DimOneDot(1,1,250,128);
		_delay_ms(2000);
		
//		a=ROL(a);
		byte flip = 0;
		for (int i = 0; i<=8; i++)
		{
			
			for (int j = 0; j<=8; j++)
			{
				if(flip==0){
				SetOneDot(j-1, i-1, 0);
				SetOneDot(j, i, 128);
				_delay_ms(75);}
				if(flip==1){
									SetOneDot(i-1, j-1, 0);
									SetOneDot(i, j, 128);
									_delay_ms(75);
				}
			}
			
			flip=!flip;


	}	
		//for (int i = 0; i<=8; i++)
	//{
		//
		//
		//
		//SetOneDot(i-1, 7, 0);
		//SetOneDot(i, 7, 128);
		//_delay_ms(50);
//
	//}		

		//SetOneDot(0,6,64);

		//DimFew(0x55,700,250,1);
		//DimOne(0,3000,250,1);
		_delay_ms(25);
		//DimFew(0x55,700,250,0);
		//DimOne(0,3000,250,0);
		_delay_ms(25);

		//DimFew(a,600,50,1);
		//
		//_delay_ms(25);
//
		//DimFew(a,600,50,0);
//
		//_delay_ms(25);

		
	}

}
