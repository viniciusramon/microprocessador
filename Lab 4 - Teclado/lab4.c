/************************************************************/
/*	TECLADO				 																					*/
/*																													*/
/************************************************************/

/************************	INCLUDES **************************/
#include <TM4C129.h>
#include <string.h>
#include <stdlib.h>

/************************	DEFINES ***************************/
#define PN0				(1UL << 0)
#define PN1				(1UL << 1)
#define PF0				(1UL << 0)

/************************	TECLADO	***************************/
/* Saidas */
#define PF1				(1UL << 1)
#define PF2				(1UL << 2)
#define PF3				(1UL << 3)
#define PG0				(1UL << 0)

/* Entradas */
#define PL4				(1UL << 4)
#define PL5				(1UL << 5)
#define PL0				(1UL << 0)
#define PL1				(1UL << 1)

/***********************	  OUTROS  *************************/
#define PJ0				(1ul << 0)
#define PJ1				(1ul << 1)

#define	InPJ0			(GPIOJ_AHB->DATA) & (1ul << 0)
#define	PortN			GPION->DATA
#define	PortF			GPIOF_AHB->DATA

/************************	ESTRUTURAS ************************/
typedef struct{
		char 		in;
		char		inPin[4];
		char		out;
		char		key;	
		char		*keyChar;
} key_typ;

/********************	 VARIAVEIS GLOBAIS ********************/
key_typ							Key;
char								actualKey;
int									Valor;
char								SValor[4];
char								keyStep;
char								keyCol;
int 								msTicks;                 // systick counter      
long								TimerDownCnt;

/********************	 Predefined functions ********************/
void MainTask(void);

/************************	  FUNCOES   ***********************/
/*----------------------------------------------------------------------------
    SysTick_Handler - 1ms
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
		msTicks++;
		if(TimerDownCnt>0)
		{
				TimerDownCnt--;
		}
		MainTask();
}

/*----------------------------------------------------------------------------
    delays number of tick Systicks (happens every 1 ms)
 *----------------------------------------------------------------------------*/
void Delay (long dlyTicks) {
		int curTicks;

		curTicks = msTicks;
		while ((msTicks - curTicks) < dlyTicks)
		{
			__NOP();
		}
}

	
void CheckKeyIOs(char keyColumn)
{
		int i;
	
		Key.out = (Key.out >= 4)? 0: keyColumn;
		
		/* write */
		switch(Key.out)
		{
				case 0:
					 GPIOF_AHB->DATA &= ~(1UL << 1);
					 GPIOF_AHB->DATA &= ~(1UL << 2);
					 GPIOF_AHB->DATA &= ~(1UL << 3);
					 GPIOG_AHB->DATA |=  (1UL << 0);
  			break;				
				case 1:
					 GPIOF_AHB->DATA &= ~(1UL << 1);
					 GPIOF_AHB->DATA &= ~(1UL << 2);
					 GPIOF_AHB->DATA |=  (1UL << 3);
					 GPIOG_AHB->DATA &= ~(1UL << 0);
  			break;
				case 2:
					 GPIOF_AHB->DATA &= ~(1UL << 1);
					 GPIOF_AHB->DATA |=  (1UL << 2);
					 GPIOF_AHB->DATA &= ~(1UL << 3);
					 GPIOG_AHB->DATA &= ~(1UL << 0);
  			break;
				case 3:
					 GPIOF_AHB->DATA |=  (1UL << 1);
					 GPIOF_AHB->DATA &= ~(1UL << 2);
					 GPIOF_AHB->DATA &= ~(1UL << 3);
					 GPIOG_AHB->DATA &= ~(1UL << 0);
  			break;
		}

		Delay(1);

	  /* read */		
	  Key.inPin[0] =   ((GPIOL->DATA) & (1ul << 1))? 1:0;
		Key.inPin[1] =   ((GPIOL->DATA) & (1ul << 0))? 1:0;
		Key.inPin[2] =   ((GPIOL->DATA) & (1ul << 5))? 1:0;
		Key.inPin[3] =   ((GPIOL->DATA) & (1ul << 4))? 1:0;

		Key.in = 0;
		for(i=0;i<4;i++)
		{
				if(Key.inPin[i])
				  	Key.in = i+1;
		}		
}

void TestButton()
{
		if (~InPJ0)
		{
				 PortN  ^= PN0;		// liga PN0
				 Delay(250);
		}
}

char GetKey()
{
		char keyIdx;

		switch(keyStep)
		{
				case 0:
						keyCol = (keyCol >= 4)? 0 : keyCol + 1;
						CheckKeyIOs(keyCol);
						if(Key.in)
						{
								keyStep = 1;
						}
				break;

				case 1:
						CheckKeyIOs(keyCol);
						keyIdx = (Key.out * 4) + (Key.in-1);
						Key.key = Key.keyChar[keyIdx]; 
						keyStep = 2;					
				break;
				case 2:
						CheckKeyIOs(keyCol);
						if(!Key.in)
						{									
								keyStep = 0;
								return Key.key;
						}
				break;
		}		
		return '?';
}


int main()
{
		//*** variaveis locais
		char val_step, keyPressed, keyPressedCnt;
		char i;
	
		//*** inicializoes
		/* Enable clock and init GPIO outputs */
		SYSCTL->RCGCGPIO |= (1UL << 0) | (1UL << 1) | (1UL << 9) | (1UL << 12)	| (1UL << 5) | (1UL << 6) | (1UL << 8) | (1UL << 10);  /* enable clock for GPIOs  */

		/* TECLADO */
		/* OUTPUTS */
		GPIOF_AHB->DIR	 |= PF0 | PF1 | PF2 | PF3;   	
		GPIOF_AHB->DEN	 |= PF0 | PF1 | PF2 | PF3;   	

		GPIOG_AHB->DIR	 |= PG0;   	  								
		GPIOG_AHB->DEN	 |= PG0;     									
	
		/* INPUTS */
		GPIOL->PDR   		 |=  PL0 | PL1 | PL4 | PL5;  	/* PL0, PL1, PL4, PL5 pull-up          */
		GPIOL->DIR   		 &= ~(PL0 | PL1 | PL4 | PL5);	/* PL0, PL1, PL4, PL5 is intput        */
		GPIOL->DEN   		 |=  (PL0 | PL1 | PL4 | PL5);	/* PL0, PL1, PL4, PL5 is digital func. */	
	
		/* OUTROS */
		GPION->DIR      		 |= PN0 | PN1;   								/* PN1, PN0 is output        */
		GPION->DEN       		 |= PN0 | PN1;   								/* PN1, PN0 enable 				   */

		GPIOJ_AHB->PUR   		 |=  PJ0 | PJ1;  								/* PJ0, PJ1 pull-up          */
		GPIOJ_AHB->DIR   		 &= ~(PJ0 | PJ1);								/* PJ0, PJ1 is intput        */
		GPIOJ_AHB->DEN   		 |=  (PJ0 | PJ1);								/* PJ0, PJ1 is digital func. */

		SystemCoreClockUpdate();                       	 		// Get Core Clock Frequency   
		SysTick_Config(SystemCoreClock / 1000);     				// Setup SysTick for 1 msec   

		/* preset */
		Key.keyChar = "147*2580369#ABCD";
		TimerDownCnt = 0;
		memset(SValor, ' ', sizeof(SValor));

		//*** ciclica
		while(1)
		{
				switch(val_step)
				{
						case 0:
								if(GetKey() == '#')
								{								
										keyPressedCnt = 4;
										val_step = 1;
										memset(SValor, '_', sizeof(SValor));
										SValor[2] = '.';
								}
						break;
								
						case 1:
								keyPressed = GetKey();
						
								if((keyPressed != '#') && (keyPressed != '?') && (keyPressedCnt > 0 ))
								{
									  for(i=1;i<4;i++)
										{								
												if(i==3)
												{
														SValor[i-2] = SValor[i];
														SValor[2] = '.';
												}
												else
												{
														SValor[i-1] = SValor[i];
												}
										}
										SValor[3] = keyPressed;			
										keyPressedCnt--;										
								}	
								
								if(keyPressed == '#')
								{		
									  for(i=0;i<4;i++)
										{
												if(SValor[i] == '_')
												{
														SValor[i] = ' ';
												}												
										}										

									  for(i=2;i>0;i--)
										{
												SValor[i] = SValor[i-1];
										}										
										SValor[0] = ' ';										

										Valor = atoi(SValor);

										val_step = 0;
								}
						break;
							
				}				
			
				/* outros */
				TestButton();
		}
}

void MainTask()
{
		int ledTime;
		ledTime = Valor *100;
	
		TimerDownCnt = (TimerDownCnt == 0)?ledTime: TimerDownCnt;

		if(TimerDownCnt > ledTime/2)
		{
				PortN |= PN1;				
		}	
		else
		{
				PortN &= ~PN1;
		}
}
