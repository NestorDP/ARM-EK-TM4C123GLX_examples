#include <stdint.h>                         
#include <stdbool.h>                        
#include "inc/hw_memmap.h"                  
#include "inc/hw_types.h"                   
#include "inc/hw_gpio.h"                    
#include "inc/hw_qei.h"                     
#include "driverlib/debug.h"                
#include "driverlib/sysctl.h"               
#include "driverlib/interrupt.h"            
#include "driverlib/gpio.h"                 
#include "driverlib/qei.h"                  
#include "driverlib/pin_map.h"              
#include "driverlib/rom.h"                  

#include "stdlib.h"
#include "inc/hw_ints.h"
#include "inc/hw_timer.h"
#include "inc/hw_uart.h"

#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/pwm.h"

#include "utils/uartstdio.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"

#include "utils/uartstdio.h"

//-----ISR prototypes-----
void UARTStdioIntHandler (void);
void Timer2AIntHandler(void);

//-----Functions prototypes-----
void UARTinit(void);
// void Timer0init(void);     // 
void Timer2Init(void);     // Edge count

uint32_t ui32flag = 0;


// TimerValueGet(TIMER0_BASE, TIMER_A)

int main(void){
    // Set the System clock to 80MHz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 |SYSCTL_USE_PLL |SYSCTL_OSC_MAIN |SYSCTL_XTAL_16MHZ);

    // Configure LED green as output
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF)) {}
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
	
    // Master interrupt enable API for all interrupts
    IntMasterEnable();
    Timer2Init();
	
    while (1){
	}
}

//-------Functions-------
// void Timer0Binit(void){
//     SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
//     TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PERIODIC);
//     TimerPrescaleSet(TIMER0_BASE, TIMER_B, 60); 
//     // TimerIntEnable(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
//     TimerEnable(TIMER0_BASE, TIMER_B);
// }

void Timer2Init(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    GPIOPinTypeTimer(GPIO_PORTB_BASE, GPIO_PIN_0);
    GPIOPinConfigure(GPIO_PB0_T2CCP0);
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

    TimerConfigure(TIMER2_BASE, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_A_CAP_COUNT));
    TimerControlEvent(TIMER2_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
    TimerLoadSet(TIMER2_BASE, TIMER_A, 10);
    TimerMatchSet(TIMER2_BASE, TIMER_A, 0);

    IntEnable(INT_TIMER2A);
    TimerIntEnable(TIMER2_BASE, TIMER_CAPA_MATCH);

    TimerEnable(TIMER2_BASE, TIMER_A);
}

void UARTinit(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlDelay(3);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioConfig(0, 115200, 16000000);
}


// -------ISR------
void Timer2AIntHandler(void) {
    TimerIntClear(TIMER2_BASE, TIMER_CAPA_MATCH);
    ui32flag = ~ui32flag;
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, ui32flag & GPIO_PIN_1);
    TimerEnable(TIMER2_BASE, TIMER_A);
}
