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
	OCR2 = ROR(OCR2); // That thing, where we  moving 1bit in OCR with each clock cycle

	COPYBIT(PORTD,0,BRG0[0]);
	COPYBIT(PORTD,1,BRG0[1]);
	COPYBIT(PORTD,2,BRG0[2]);
	COPYBIT(PORTD,3,BRG0[3]);
	COPYBIT(PORTD,4,BRG0[4]);
	COPYBIT(PORTD,5,BRG0[5]);
	COPYBIT(PORTD,6,BRG0[6]);
	COPYBIT(PORTD,7,BRG0[7]);

	//for(int i = 0; i<8; i++)
	//{
		//COPYBIT(PORTD,i,BRG0[i]);
	//}



}


void SetOne(unsigned char LED, unsigned char VAL)
{
	//if(OCR2 == 64)
	//{
		BRG0[LED]=VAL;
	//}
}

void SetFew(byte LEDS, byte VAL)
{
	if (OCR2 == 1)
	{
		for (byte i = 0; i<8; i++)
		{
			if (LEDS)
			{
			}
		}
	}
}

void DimOne(byte LED,int VAL, boolean  dir)
{
	
	
	TCCR1A |= 0;
	TCCR1B |= (0<<CS12)|(1<<CS11)|(1<<CS10);
	TCCR1B |= (1<<WGM12);

	//TIMSK |= (1<<TOIE1);
	
	OCR1A = VAL;
		 
		if (dir == 1)
		{

			TCNT1 = 0;

			for(int i=0; i<255; i++) // Rolling Brightness!
			{
				//while(TCNT1 < VAL)
				//{
					//if (i!=127 || i!=64)
					//{
						
				while(!(TIFR & (1 << OCF1A))) if(OCR2==64)  SetOne(LED,i);		

				TIFR |= (1 << OCF1A);
					//}
				//}
				//TCNT1 = 0;
				
				
			}
			
		}
		
		else
		{
			TCNT1H = 0;
			TCNT1L = 0;
			SetOne(LED,VAL);
			for (int i = VAL; i>0; i--)
			{
				while(TCNT1L < VAL)
				{
					//if (i!=127 || i!=64)
					//{
						if(OCR2==64) SetOne(LED,i);
					//}
					//_delay_us(7);
				}
				//SetOne(LED,0);
				TCNT1L = 0;
			}
			SetOne(LED,0);
		}

}


void SetAll(unsigned char VAL)
{
	//if(OCR2 == 64)
	//{
		for (unsigned char i = 0; i<8; i++)
		{
			BRG0[i]=VAL;
		}
	//}
}


void main(void)
{
	volatile unsigned char a;

	sei(); // Allow interrupts. Yeah, because why not, interrupt me, interrupt me everybody!!!1

	TCNT2 = 0;  //Timer 2 initial value
	OCR2 = 0b00000001; // Ehm, that thing, which is comaprd to. Something, something..

	TIMSK |= 1<<OCF2; // Timer 2 
	TCCR2 |= 1<<WGM21 | 1<<CS22 | 1<<CS21;// | 1<< CS20; // Setting Timer 2 to CTC mode, and to prescaler of 256

	PORTD = 0x00; // initalizig our port
	DDRD = 0xFF; // Setting directions as outputs

		
    //DimOne(7,255,1,1);

	while(1)
	{
		
		//SetAll(10);
		
		//SetOne(3,127);
		
		//SetOne(4,128);
		

		//for (int i = 0; i<255; i++)
		//{
			//SetOne(0,i);
			//_delay_ms(5);
		//}

		//SetAll(0);
		
		//SetOne(7,90);
		//_delay_ms(1000);
		
		//_delay_ms(1000);
		
		//DimOne(6,255,1);
		//for (int i = 0; i<8;i++)
		//{
			//DimOne(i,255,100,1);
			////SetOne((i-1),0);
			//SetAll(0);
		//}
		
		//SetAll(1);
		DimOne(7,130,1);
		//SetOne(7,255);
		_delay_ms(1000);
		DimOne(7,255,0);
		
		SetOne(7,0);
		_delay_ms(1000);
		SetOne(7,0);
		
		
		//_delay_ms(10);

		//SetOne(7,255);
		
		//for (int i = 0; i<255; i++)
		//{
			//for (int j = 0; j<1; j++)
			//{
				//BRG0[j]=i;
				//_delay_ms(10);
			//}
			//for (int j = 1; j<8; j++)
			//{
				//BRG0[j]=0;
//
			//}
//
		//}
		//
		//for (int i = 255; i>0; i--)
		//{
			//for (int j = 0; j<1; j++)
			//{
				//BRG0[j]=i;
				////_delay_ms(10);
			//}
			//for (int j = 1; j<8; j++)
			//{
				//BRG0[j]=0;
//
			//}
//
		//}
		
		//for (int i = 0; i<8; i++)
		//{
			//for (int j = 0; j<8; j++)
			//{
				//BRG2[j]=BRIGHNESS_ARR[j][i];
				//_delay_ms(200);
			//}
		//}
		
		//for (int j = 0; j<8; j++)
			//{
				//BRG2[j]=j+32;
			//}
			
			
	}

}
