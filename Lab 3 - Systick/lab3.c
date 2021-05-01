/*----------------------------------------------------------------------------
		GPIO - SEMAFORO - SYSTICK
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
		Includes
 *----------------------------------------------------------------------------*/
 #include <TM4C129.h>

/*----------------------------------------------------------------------------
		Defines
 *----------------------------------------------------------------------------*/
#define PN0				(1UL << 0)						// Led 2
#define PN1				(1UL << 1)						// Led 1

#define PF0				(1UL << 0)						// Led 4
#define PF4				(1UL << 4)						// Led 3

#define PJ0				(1ul << 0)						// SW1
#define PJ1				(1ul << 1)						// SW2

#define	PortN			GPION->DATA
#define	PortF			GPIOF_AHB->DATA

/*----------------------------------------------------------------------------
		Global Variables
 *----------------------------------------------------------------------------*/
char				AbSw1;				// Input  sw1 
char				AbSw2;				// Input  sw2
char				SbLed1;				// Output led1
char				SbLed2;				// Output led2
char				SbLed3;				// Output led3
char				SbLed4;				// Output led4

long				TimerDownCnt;	// timer down counter

/*----------------------------------------------------------------------------
    SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
		if(TimerDownCnt > 0)
		{
				TimerDownCnt --;
		}
}

/*----------------------------------------------------------------------------
    MAIN
 *----------------------------------------------------------------------------*/
int main()
{
		// Local variables
		char 	run;	
		char 	smfr_step;
	
		/***********************  INIT  ***********************/
		/* Enable clock and init GPIO outputs */
		SYSCTL->RCGCGPIO |= (1UL << 12)	| (1UL << 5);  	/* enable clock for GPIOs    */
		
		GPION->DIR       |= PN0 | PN1;   							 	/* PN1, PN0 is output        */
		GPION->DEN       |= PN0 | PN1;   							 	/* PN1, PN0 is digital func. */

		GPIOF_AHB->DIR   |= PF4 | PN0;   								/* PF4, PF0 is output        */
		GPIOF_AHB->DEN 	 |= PF4 | PN0;   								/* PF4, PF0 is digital func. */
		
		SYSCTL->RCGCGPIO |=  (1ul << 8); 								/* enable clock for GPIOs    */
		GPIOJ_AHB->PUR   |=  PJ0 | PJ1;  								/* PJ0, PJ1 pull-down        */
		GPIOJ_AHB->DIR   &= ~(PJ0 | PJ1);								/* PJ0, PJ1 is intput        */
		GPIOJ_AHB->DEN   |=  (PJ0 | PJ1);								/* PJ0, PJ1 is digital func. */

		SystemCoreClockUpdate();                       	 		// Get Core Clock Frequency   
		SysTick_Config(SystemCoreClock / 1000);     				// Setup SysTick for 1 msec   

		SbLed1 = 0;
		SbLed2 = 0;
		SbLed3 = 0;

		smfr_step = 0;
		TimerDownCnt = 0;
		run = 0;

		/*********************** CYCLIC ***********************/
		while(1)
		{
				// read inputs
				AbSw1 = (((GPIOJ_AHB->DATA) & PJ0))?0:1;		// pull up switch
				AbSw2 = (((GPIOJ_AHB->DATA) & PJ1))?0:1;		// pull up switch		
				
				// Start / Stop semaforo
				if(AbSw1)
				{
						run = 1;
				}

				if(AbSw2)
				{
						run = 0;
				}
				
				if(run)
				{
						switch(smfr_step)
						{
								case 0:
										if(TimerDownCnt == 0)
										{
												SbLed3 = 0;
												SbLed1 = 1;
												TimerDownCnt = 5000;
												smfr_step = 1;
										}
								break;

								case 1:
										if(TimerDownCnt == 0)
										{
												SbLed1 = 0;
												SbLed2 = 1;
												TimerDownCnt = 1000;
												smfr_step = 2;
										}
								break;

								case 2:
										if(TimerDownCnt == 0)
										{
												SbLed2 = 0;
												SbLed3 = 1;
												TimerDownCnt = 5000;
												smfr_step = 0;
										}
								break;
						} // switch(smfr_step)
				} // if(run)
				else
				{
						smfr_step = 0;
						TimerDownCnt = 0;
						SbLed1 = 0;
						SbLed2 = 0;
						SbLed3 = 0;
				} 
				
				// write outputs
				PortN = (SbLed1)? PortN | PN1: PortN & ~PN1;	
				PortN = (SbLed2)? PortN | PN0: PortN & ~PN0;				
				PortF = (SbLed3)? PortF | PF4: PortF & ~PF4;
		} // while(1)
}
