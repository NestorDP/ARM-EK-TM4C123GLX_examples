/*!
 * @author      Yash Bansod
 * @date        26th September 2017
 *
 * @brief       Quadrature Encoder
 * @details     The program interfaces the quadrature encoder peripheral to get the position
 *              and velocity information of a encoded DC motor. The QEI1 peripheral is used
 *              with PortC using PC4, PC5 and PC6 as Index1, PhA and PhB.
 * @note        The tm4c123ghpm_startup_ccs.c contains the vector table for the
 *              microcontroller. It was modified to execute the specified ISR on
 *              QEI1 Interrupts.
 */
/* -----------------------          Include Files       --------------------- */
#include <stdint.h>                         // Library of Standard Integer Types
#include <stdbool.h>                        // Library of Standard Boolean Types
// #include "inc/tm4c123gh6pm.h"               // Definitions for interrupt and register assignments on Tiva C
#include "inc/hw_memmap.h"                  // Macros defining the memory map of the Tiva C Series device
#include "inc/hw_types.h"                   // Defines common types and macros
#include "inc/hw_gpio.h"                    // Defines Macros for GPIO hardware
#include "inc/hw_qei.h"                     // Macros used when accessing the QEI hardware
#include "driverlib/debug.h"                // Macros for assisting debug of the driver library
#include "driverlib/sysctl.h"               // Defines and macros for System Control API of DriverLib
#include "driverlib/interrupt.h"            // Defines and macros for NVIC Controller API of DriverLib
#include "driverlib/gpio.h"                 // Defines and macros for GPIO API of DriverLib
#include "driverlib/qei.h"                  // Prototypes for the Quadrature Encoder Driver
#include "driverlib/pin_map.h"              // Mapping of peripherals to pins for all parts
#include "driverlib/rom.h"                  // Defines and macros for ROM API of driverLib

#include "stdlib.h"
#include "inc/hw_ints.h"
#include "inc/hw_timer.h"
#include "inc/hw_uart.h"

#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/pwm.h"

#include "utils/uartstdio.h"

#define VEL_INT_FREQ    7              // Macro to store the Interrupt frequency of QEI1
#define QEI1_PPR        20              // Macro to store the PPR of the QEI1

/* -----------------------      Global Variables        --------------------- */
volatile uint32_t ui32Qei1Vel;              // Variable to store the velocity of QEI1
volatile uint32_t ui32Qei1Pos;              // Variable to store the position of QEI1
volatile int32_t i32Qei1Dir;                // Variable to store the direction of QEI1
volatile uint16_t ui16Qei1Rpm;              // Variable to store the RPM of QEI1

/* -----------------------      Function Prototypes     --------------------- */
// void UARTStdioIntHandler (void);
void QEI1IntHandler(void);
void InitConsole(void);
void ligaMotor(void);

/* -----------------------          Main Program        --------------------- */
int main(void){
    // Set the System clock to 80MHz
    SysCtlClockSet(SYSCTL_SYSDIV_2_5 |SYSCTL_USE_PLL |SYSCTL_OSC_MAIN |SYSCTL_XTAL_16MHZ);

    // Enable the clock for peripherals PortC and QEI1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_QEI1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    // Configure the PC5, PC6 for QEI signals
    GPIOPinTypeQEI(GPIO_PORTC_BASE, GPIO_PIN_5 | GPIO_PIN_6);
    GPIOPinConfigure(GPIO_PC5_PHA1);
    GPIOPinConfigure(GPIO_PC6_PHB1);

    // Configure the QEI1 to increment for both PhA and PhB for quadrature input with "QEI1_PPR" PPR
    QEIConfigure(QEI1_BASE, QEI_CONFIG_CAPTURE_A | QEI_CONFIG_NO_RESET | QEI_CONFIG_CLOCK_DIR | QEI_CONFIG_NO_SWAP, QEI1_PPR);
    // Configure the QEI1 for Velocity Calculation, Predivide by 1 at "VEL_INT_FREQ" Hz
    QEIVelocityConfigure(QEI1_BASE, QEI_VELDIV_1, SysCtlClockGet() / VEL_INT_FREQ);
    QEIVelocityEnable(QEI1_BASE);

    // Enable the Interrupts for Velocity Timer Expiration of QEI1
    void (*QEI1IntHandler_ptr)(void) = &QEI1IntHandler;
    QEIIntRegister(QEI1_BASE, *QEI1IntHandler_ptr);
    QEIIntEnable(QEI1_BASE, QEI_INTTIMER);

    // Master interrupt enable API for all interrupts
    IntMasterEnable();
    // Enable the QEI1
    QEIEnable(QEI1_BASE);

	InitConsole();
	ligaMotor();
	
    while (1){
		UARTprintf("\nvelociyyyyyydade %d: ", ui32Qei1Vel);
		SysCtlDelay( 500 * (80000000 / 3 / 1000));
	}
}


void InitConsole(void)
{
    // Enable GPIO port A which is used for UART0 pins.
    // TODO: change this to whichever GPIO port you are using.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlDelay(3);

    // Configure the pin muxing for UART0 functions on port A0 and A1.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    // Enable UART0 so that we can configure the clock.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Use the internal 16MHz oscillator as the UART clock source.
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    // Select the alternate (UART) function for these pins.
    // TODO: change this to select the port/pin you are using.
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    // Initialize the UART for console I/O.
    UARTStdioConfig(0, 115200, 16000000);

}
