#include<p18f4520.h>
#pragma config WDT=OFF
#pragma config OSC=HS
#pragma config LVP=OFF

void Check(void);
//ISR
#pragma interrupt Check
#pragma code High_Vec=0x0008
void High_Vec(void)
{
	Check();
}

//Init. Functions
void EnableInterrupt(void);

//Timer Functions
void StartTimer(void);
void StopTimer(void);
void SetTimer(unsigned char high,unsigned char low);

//UART Functions
void SetUart(void);
void Send(unsigned char buf);
void SendString(unsigned char* buf);
void SendInt(unsigned int Val);

//ADC Functions
void StartAdc(void);
void SetAdc(void);
void StopAdc(void);
unsigned int ReadAdc(void);

unsigned char temp=0; //UART Receive byte
unsigned int adcVal=0; //value from adc
unsigned char cha=0; //ADC Channel
unsigned char cmd=0; 

unsigned char iF[]="  \r\n";

void main()
{
	EnableInterrupt();
	SetUart();
	SetAdc();
	while(1);
}

void Check()
{
	if(PIR1bits.TMR1IF)
	{
/* 		StopTimer();

		SetTimer(0xF6,0x36);		
		StartTimer();	 */	
	}
	if (PIR1bits.ADIF)
	{
		/* StopAdc();
		adcVal=ReadAdc();
		
		StartAdc(); */
		
	}
	if(PIR1bits.RCIF)
	{
		/* temp=RCREG;
		 */
	}
}

void SetTimer(unsigned char high,unsigned char low)
{
	T1CON=0x00;
	TMR1L=low;
	TMR1H=high;
}

void StartTimer()
{
	T1CONbits.TMR1ON=1;
}

void StopTimer()
{
	PIR1bits.TMR1IF=0;
	T1CONbits.TMR1ON=0;
}

void SetUart()
{
	TRISCbits.TRISC7=1;
	TRISCbits.TRISC6=1;
	SPBRG=0x0F;
	RCSTA=0x90;
	TXSTA=0x20;
}

void EnableInterrupt()
{
	INTCONbits.GIE=1;
	INTCONbits.PEIE=1;
	PIE1bits.TMR1IE=1;
	PIE1bits.RCIE=1;
	PIR1bits.ADIF = 0;
	PIE1bits.ADIE = 1;
	PIR1bits.TMR2IF = 0;
	PIE1bits.TMR2IE = 1;
}

void StartAdc()
{
	ADCON0=0x00;
	//Can be optimized
	if(cha==0)
	{
		ADCON0bits.CHS3=0;
		ADCON0bits.CHS2=0;
		ADCON0bits.CHS1=0;
		ADCON0bits.CHS0=0;
	}
	else if(cha==1)
	{
		ADCON0bits.CHS3=0;
		ADCON0bits.CHS2=0;
		ADCON0bits.CHS1=0;
		ADCON0bits.CHS0=1;
	}
	else
	{
		ADCON0bits.CHS3=0;
		ADCON0bits.CHS2=0;
		ADCON0bits.CHS1=1;
		ADCON0bits.CHS0=0;
	}
	ADCON0bits.ADON = 1; 
	ADCON0bits.GO_DONE=1;
}

void StopAdc()
{
	PIR1bits.ADIF = 0;
	ADCON0bits.ADON = 0;
	ADCON0bits.GO_DONE=0;
}

void SetAdc()
{
	TRISAbits.TRISA0 = 1; 
	TRISAbits.TRISA1 = 1;
	TRISAbits.TRISA2=1;
	TRISAbits.TRISA3=1;
	ADCON1 = 0x0C;
	ADCON0 = 0x00;
	ADCON2 = 0xAD;
}

void Send(unsigned char buf)
{
	TXREG=buf;
	while(!TXSTAbits.TRMT);
}

unsigned int ReadAdc()
{
	return (unsigned int)((((unsigned int)ADRESH&0x0003))<<8 |  ADRESL);
} 

void SendInt(unsigned int adcVal)
{
	iF[1]=(unsigned char)adcVal+'0';
	iF[0]=(unsigned char)(adcVal>>8)+'0';
	SendString(iF);
}

void SendString(unsigned char* buf)
{
	while(*buf!='\0')
	{
		Send(*buf);
		buf++;
	}
}