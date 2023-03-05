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

#include "utils/uartstdio.h"

// Functions prototypes 
void Timer0BIntHandler(void);

uint32_t flag = 0x0;

int main(void){
    // Set the System clock to 80MHz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 |SYSCTL_USE_PLL |SYSCTL_OSC_MAIN |SYSCTL_XTAL_16MHZ);

    // Configure LED green as output
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {}
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_3);

    // Configure Timer0B as a 16-bit periodic timer.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PERIODIC);
    TimerPrescaleSet(TIMER0_BASE, TIMER_B, 60); 
    TimerIntEnable(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
    TimerEnable(TIMER0_BASE, TIMER_B);

    // Master interrupt enable API for all interrupts
    IntMasterEnable();
    IntEnable(INT_TIMER0B);
	
    while (1){
	}
}

void Timer0BIntHandler(void) {
    TimerIntClear(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
    flag = ~flag;
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, flag & GPIO_PIN_3);
}
