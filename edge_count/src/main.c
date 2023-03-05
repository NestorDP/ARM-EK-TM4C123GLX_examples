#include <stdint.h>                         
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/qei.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_timer.h"
#include "inc/hw_uart.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"

// Functions prototypes 
void Timer0AIntHandler(void);

uint32_t flag = 0x0;

int main(void){
    // Set the System clock to 80MHz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 |SYSCTL_USE_PLL |SYSCTL_OSC_MAIN |SYSCTL_XTAL_16MHZ);

    // Configure LED green as output
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {}
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

    // Timer count
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    GPIOUnlockPin(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOPinConfigure(GPIO_PF0_T0CCP0);
    GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    TimerConfigure(TIMER0_BASE, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_COUNT));
    TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
    TimerLoadSet(TIMER0_BASE, TIMER_A, 10);
    TimerMatchSet(TIMER0_BASE, TIMER_A, 0);

    // Master interrupt enable API for all interrupts
    IntMasterEnable();
    IntEnable(INT_TIMER0A);
    TimerIntEnable(TIMER0_BASE, TIMER_CAPA_MATCH);

    // Enable the timer.
    TimerEnable(TIMER0_BASE, TIMER_A);
	
    while (1){

	}
}

void Timer0AIntHandler(void) {
    TimerIntClear(TIMER0_BASE, TIMER_CAPA_MATCH);
    flag = ~flag;
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, flag & GPIO_PIN_3);
    TimerEnable(TIMER0_BASE, TIMER_A);
}
