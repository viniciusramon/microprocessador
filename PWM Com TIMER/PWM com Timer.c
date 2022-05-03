/*----------------------------------------------------------------------------
		LABORATÓRIO - GPIO - PWM com Timer

AUTOR: 	Vinicius Ramon		DATA:		30/05/2015					VERSAO: 1.0

*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
		Includes
 *----------------------------------------------------------------------------*/
 #include <TM4C129.h>

/*----------------------------------------------------------------------------
		Defines
 *----------------------------------------------------------------------------*/
#define PN0				(1UL << 0)						// Led 2
#define PN1				(1UL << 1)	

#define	PortN			GPION->DATA

/*----------------------------------------------------------------------------
		Global Variables
 *----------------------------------------------------------------------------*/
long TimerDownCnt, Pid, Ton, Toff, dutyCycle;

/*----------------------------------------------------------------------------
    Functions
 *----------------------------------------------------------------------------*/

void TIMER0A_Handler(void)
{
		TIMER0->ICR = 1;
		if(TimerDownCnt > 0)
		{
			TimerDownCnt--;
		}
}
/*----------------------------------------------------------------------------
    MAIN
 *----------------------------------------------------------------------------*/
int main()
{
		// Local variables
		int 	wait;	
		char step;
	
		/***********************  INIT  ***********************/
		/* Enable clock and init GPIO outputs */
		SYSCTL->RCGCGPIO |= (1UL << 12); 	/* enable clock for GPIOs    */
		
		GPION->DIR       |= PN0 | PN1;   							 	/* PN1, PN0 is output        */
		GPION->DEN       |= PN0 | PN1; 							 	/* PN1, PN0 is digital func. */
	
		SystemCoreClockUpdate();
		SYSCTL->RCGCTIMER |= (1UL << 0);
		wait = wait;
		wait = wait;
		wait = wait;
	
		TIMER0->CTL = 0; 													// 1) disable TIMER0A during setup
		TIMER0->CFG = 0;													// 2) configure for 32-bit mode
		TIMER0->TAMR = (1UL << 1); 								// 3) configure for periodic mode, default down-count settings
		TIMER0->TAILR = (SystemCoreClock/1000)-1;	// 4) reload value
		TIMER0->TAPR  = 0;             					 	// 5) bus clock resolution
		TIMER0->ICR = (1UL << 0);      						// 6) clear TIMER0A timeout flag
		TIMER0->IMR = (1UL << 0);     						// 7) arm timeout interrupt
		NVIC_EnableIRQ(TIMER0A_IRQn);	
		TIMER0->CTL = (1UL << 0);    							// 9) enable TIMER0A
		
		step = 0;
		TimerDownCnt = 0;
		
		Pid = 50; // 30 % da saida do PID 
		dutyCycle = 100;
		
		/*********************** CYCLIC ***********************/
		while(1)
		{
				Ton = ((Pid * dutyCycle)/100);
				Toff = dutyCycle - Ton;
			
				switch(step)
				{
					case 0:
						if(TimerDownCnt == 0)
						{
									TimerDownCnt = Ton;
									PortN |= PN0 | PN1;
									step = 1;
						}
						break;
						
					case 1:
						if(TimerDownCnt == 0)
						{
							TimerDownCnt = Toff;
							PortN &= ~(PN0 | PN1);
							step = 0;
						}
						break;
					}
		}
	}
