
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

#define VEL_INT_FREQ    7
#define QEI1_PPR        40

volatile uint32_t ui32Qei1Vel;              // Variable to store the velocity of QEI1
volatile uint32_t ui32Qei1Pos;              // Variable to store the position of QEI1
volatile int32_t i32Qei1Dir;                // Variable to store the direction of QEI1
volatile uint16_t ui16Qei1Rpm;              // Variable to store the RPM of QEI1

void QEI1IntHandler(void);

void InitConsole(void);
void ligaMotor(void);


int main(void){
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 |SYSCTL_USE_PLL |SYSCTL_OSC_MAIN |SYSCTL_XTAL_16MHZ);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    GPIOPinTypeQEI(GPIO_PORTC_BASE, GPIO_PIN_5 | GPIO_PIN_6);
    GPIOPinConfigure(GPIO_PC5_PHA1);
    GPIOPinConfigure(GPIO_PC6_PHB1);

    QEIConfigure(QEI1_BASE, QEI_CONFIG_CAPTURE_A | QEI_CONFIG_NO_RESET | QEI_CONFIG_CLOCK_DIR | QEI_CONFIG_NO_SWAP, QEI1_PPR);
    QEIVelocityConfigure(QEI1_BASE, QEI_VELDIV_1, SysCtlClockGet() / VEL_INT_FREQ);
    QEIVelocityEnable(QEI1_BASE);

    void (*QEI1IntHandler_ptr)(void) = &QEI1IntHandler;
    QEIIntRegister(QEI1_BASE, *QEI1IntHandler_ptr);
    QEIIntEnable(QEI1_BASE, QEI_INTTIMER);

    IntMasterEnable();
    QEIEnable(QEI1_BASE);

	InitConsole();
	ligaMotor();
	
    while (1){
		UARTprintf("\nvelocidade %d ", ui16Qei1Rpm);
		SysCtlDelay( 500 * (80000000 / 3 / 1000));
	}
}


void QEI1IntHandler(void) {
    QEIIntClear(QEI1_BASE, QEIIntStatus(QEI1_BASE, true));
    ui32Qei1Vel = QEIVelocityGet(QEI1_BASE);
    ui32Qei1Pos = QEIPositionGet(QEI1_BASE);
    i32Qei1Dir = QEIDirectionGet(QEI1_BASE);

    ui16Qei1Rpm = ui32Qei1Vel * VEL_INT_FREQ * 60 / QEI1_PPR;
}

void InitConsole(void) {
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlDelay(3);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    UARTStdioConfig(0, 115200, 16000000);
}


void ligaMotor(void) {
	SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_1);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

	GPIOPinConfigure(GPIO_PB4_M0PWM2);
	GPIOPinConfigure(GPIO_PB5_M0PWM3);
	GPIOPinConfigure(GPIO_PD0_M0PWM6);
	GPIOPinConfigure(GPIO_PD1_M0PWM7);

	GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_5);
	GPIOPinTypePWM(GPIO_PORTB_BASE, GPIO_PIN_4);
	GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_1);
	GPIOPinTypePWM(GPIO_PORTD_BASE, GPIO_PIN_0);

	PWMGenConfigure(PWM0_BASE, PWM_GEN_1, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
	PWMGenConfigure(PWM0_BASE, PWM_GEN_3, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

	PWMGenPeriodSet(PWM0_BASE, PWM_GEN_1, 25000);	// 3.2KHz
	PWMGenPeriodSet(PWM0_BASE, PWM_GEN_3, 25000);	// 3.2KHz

	PWMOutputState(PWM0_BASE, PWM_OUT_7_BIT | PWM_OUT_6_BIT | PWM_OUT_3_BIT | PWM_OUT_2_BIT, true);

	PWMGenEnable(PWM0_BASE, PWM_GEN_1);
	PWMGenEnable(PWM0_BASE, PWM_GEN_3);

	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_2, 23000);
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_3, 0);
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_6, 0);
	PWMPulseWidthSet(PWM0_BASE, PWM_OUT_7, 23000);
}
