/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * @brief    Show how to set GPIO pin mode and use pin data input/output control.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include "stdio.h"
#include "NuMicro.h"

#include "multi_timer.h"
void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable HIRC */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);

    /* Waiting for HIRC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Switch HCLK clock source to HIRC */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    /* Set both PCLK0 and PCLK1 as HCLK/2 */
    CLK->PCLKDIV = (CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2);

    /* Switch UART0 clock source to HIRC */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* Enable UART peripheral clock */
    CLK_EnableModuleClock(UART0_MODULE);
    /* Enable TIMER module clock */
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HIRC, 0);
    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*----------------------------------------------------------------------*/
    /* Init I/O Multi-function                                              */
    /*----------------------------------------------------------------------*/
    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    /* Lock protected registers */
    SYS_LockReg();
}

/*----------------------------------------------------------------------*/
/* Init UART0                                                           */
/*----------------------------------------------------------------------*/
void UART0_Init(void)
{
    /* Reset UART0 */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}



void TMR0_IRQHandler(void)
{
    if(TIMER_GetIntFlag(TIMER0) == 1)
    {
		
        /* Clear Timer0 time-out interrupt flag */
        TIMER_ClearIntFlag(TIMER0);
	timer_ticks();     
    }
}
struct Timer timer1,timer2;
unsigned char t=0;
void timer1_callback(void)
{
printf("t1 \n\r");
	timer_init(&timer1, timer1_callback, 1000, 0,NULL); //1s loop
    timer_start(&timer1);	 
	t++;
	if (t==10)
	{
	timer_stop(&timer1);	
	}
}
void timer2_callback(void)
{
printf("t2 \n\r");
	//timer_stop(&timer1);	 
}

int32_t main(void)
{

    /* Init System, IP clock and multi-function I/O. */
    SYS_Init();

    /* Init UART0 for printf */
    UART0_Init();

    printf("\n\nCPU @ %dHz\n", SystemCoreClock);
 /* Open Timer0 in periodic mode, enable interrupt and 1 interrupt tick per second */
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER0);
	 /* Enable Timer0 NVIC */
    NVIC_EnableIRQ(TMR0_IRQn);
	    /* Start Timer0 counting */
    TIMER_Start(TIMER0);
    timer_init(&timer1, timer1_callback, 1000, 0,NULL); //1s loop
    timer_start(&timer1);	 
	 
    timer_init(&timer2, timer2_callback, 2000, 0,NULL);   //2s  delay
    timer_start(&timer2);
    
   	while(1)
	{
		timer_loop();		   
	}	 

    while(1);
}
