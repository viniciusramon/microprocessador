/*----------------------------------------------------------------------------
		AULA 08/05 - Display LCD

AUTOR:  Vinicius Ramon					DATA:	08/05/2015						VERSAO: 1.1

*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
		Includes
 *----------------------------------------------------------------------------*/
 #include <TM4C129.h>

/*----------------------------------------------------------------------------
		Defines
 *----------------------------------------------------------------------------*/

#define GrupoA (1UL << 0) // Habilita o grupo das portas A
#define GrupoB (1UL << 1) // Habilita o grupo das portas B
#define GrupoK (1UL << 9) // Habilita o grupo das portas K

#define PA4			(1UL << 4) // Habilita a porta PA4
#define PA5			(1UL << 5) // Habilita a porta PA5

#define PB5			(1UL << 5) // Habilita a porta PB5

#define PK0			(1UL << 0) // Habilita a porta PK0
#define PK1			(1UL << 1) // Habilita a porta PK1
#define PK2			(1UL << 2) // Habilita a porta PK2
#define PK3			(1UL << 3) // Habilita a porta PK3

#define PORTA		GPIOA_AHB->DATA
#define PORTB		GPIOB_AHB->DATA
#define PORTK		GPIOK->DATA

/*----------------------------------------------------------------------------
		Global Variables
 *----------------------------------------------------------------------------*/
	int TimerUp, Init;
	long ticks;
/*----------------------------------------------------------------------------
    Functions
 *----------------------------------------------------------------------------*/
void LCD_DATA(char value)
{
	if(value & 0x01)
	{
		PORTK |= PK2;
	}
	else
	{
		PORTK &= ~PK2;
	}
	
	if(value & 0x02)
	{
		PORTK |= PK3;
	}
	else
	{
		PORTK &= ~PK3;
	}
	
	if(value & 0x04)
	{
		PORTA |= PA4;
	}
	else
	{
		PORTA &= ~PA4;
	}
	
	if(value & 0x08)
	{
		PORTA |= PA5;
	}
	else
	{
		PORTA &= ~PA5;
	}
}

void SysTick_Handler()
{
			ticks++;
}

void Delay(long tempo)
{
			long start=ticks;
	while((ticks-start)<tempo)
	{
	}
			
}

void Enable()
{
				Delay(5);
				GPIOK->DATA &= ~PK1;
				Delay(5);
				GPIOK->DATA |= PK1;
				Delay(5);
				GPIOK->DATA &= ~PK1;
				Delay(5);
}


/*----------------------------------------------------------------------------
    MAIN
 *----------------------------------------------------------------------------*/
int main()
{
	char Letra, nibble;
	
		/***********************  INIT  ***********************/
		/* Enable clock and init GPIO outputs */
		SYSCTL->RCGCGPIO |= GrupoA | GrupoB | GrupoK;  /* enable clock for GPIOs  */

		/* OUTPUTS */
		GPIOA_AHB->DIR	 |= PA4 | PA5;  /* direcao: 0 = entrada ou 1 = saida */ 	
		GPIOA_AHB->DEN	 |= PA4 | PA5;  /* habilita o pino */ 	
	
		GPIOB_AHB->DIR	 |= PB5;  /* direcao: 0 = entrada ou 1 = saida */ 	
		GPIOB_AHB->DEN	 |= PB5;  /* habilita o pino */ 	
	
		GPIOK->DIR	 |= PK0 | PK1 | PK2 | PK3;  /* direcao: 0 = entrada ou 1 = saida */ 	
		GPIOK->DEN	 |= PK0 | PK1 | PK2 | PK3;  /* habilita o pino */ 	
	
		SystemCoreClockUpdate();
		SysTick_Config(SystemCoreClock/1000); // base de tempo 
	
		/*********************** CYCLIC ***********************/				
			
			
		Delay(500);		//Delay de 500ms
				
		GPIOB_AHB->DATA &= ~PB5; // COLOCA RS=0
		GPIOK->DATA &= ~PK0; // COLOCA RW=0
		Enable();
		
		LCD_DATA(2);	//Manda o valor 2
		Enable();	// Faz o Enable
		LCD_DATA(2);	//Manda o valor 2
		Enable();
		
		LCD_DATA(8); //Manda o valor 8
		Enable();

		LCD_DATA(0);
		Enable();
		
		LCD_DATA(0x0F);		
		Enable();
		
		LCD_DATA(0);
		Enable();
		
		LCD_DATA(1);
		Enable();
		
		GPIOB_AHB->DATA |= PB5; // COLOCA RS=1
		
		Letra='A';
		nibble=(Letra&0xF0);
		nibble=nibble>>4;
		LCD_DATA(nibble);
		Enable();
		
		Letra='A';
		nibble=(Letra&0x0F);
		LCD_DATA(nibble);
		Enable();
	
		
		while(1)
		{			
			

		} 
}
