/*----------------------------------------------------------------------------
		TIMER 0 - PWM - dutycycle 2seg
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
long				Pid;
long				Ton, Toff;
long				dutyCycle;
long				TimerDownCnt;	// timer down counter

/*----------------------------------------------------------------------------
    Timer0 Handler - 1ms
 *----------------------------------------------------------------------------*/
void TIMER0A_Handler(void) {
	  TIMER0->ICR = 1;// acknowledge timer0A timeout

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
		char 	smfr_step;
		int wait;

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

		SystemCoreClockUpdate();                       	// Get Core Clock Frequency   
		SYSCTL->RCGCTIMER |= 0x01;     									// 0) activate TIMER0
		wait = wait;									  								// wait activate... just wait 
		wait = wait;									  								// wait activate... just wait 
		wait = wait;									 								  // wait activate... just wait 

		TIMER0->CTL		= 0x00000000;     // 1) disable TIMER0A during setup
		TIMER0->CFG   = 0x00000000;     // 2) configure for 32-bit mode
		TIMER0->TAMR  = 0x00000002;     // 3) configure for periodic mode, default down-count settings
		TIMER0->TAILR = (SystemCoreClock/1000)-1;         // 4) reload value
		TIMER0->TAPR  = 0;              // 5) bus clock resolution
		TIMER0->ICR   = 0x00000001;     // 6) clear TIMER0A timeout flag
		TIMER0->IMR   = 0x00000001;     // 7) arm timeout interrupt
		NVIC_EnableIRQ(TIMER0A_IRQn);	
		TIMER0->CTL = 0x00000001;    		// 9) enable TIMER0A

		smfr_step = 0;
		TimerDownCnt = 0;

		Pid = 80; // 30 % da saida do PID 
		dutyCycle = 2000;

		/*********************** CYCLIC ***********************/
		while(1)
		{
					Ton = ((Pid * dutyCycle)/100);
					Toff = dutyCycle - Ton;
		
					switch(smfr_step)
					{
							case 0:
									if(TimerDownCnt == 0)
									{
											TimerDownCnt = Ton;
											PortN |= PN1;
											smfr_step = 1;
									}
							break;

							case 1:
									if(TimerDownCnt == 0)
									{
											TimerDownCnt = Toff;
											PortN &= ~PN1;
											smfr_step = 0;
									}
							break;
					} // switch(smfr_step)
				
		} // while(1)
}
