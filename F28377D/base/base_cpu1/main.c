/*
 * main.c
 */
#include "F28x_Project.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"

extern void UARTStdioIntHandler(void);

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void)
{

    //
    // Enable UART0
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SCI1);

    //
    // Configure GPIO Pins for UART mode.
    //
    EALLOW;
    GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;
    GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;
    GpioCtrlRegs.GPADIR.bit.GPIO28 = 0;

    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO29 = 1;
    EDIS;

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, SysCtlLowSpeedClockGet(SYSTEM_CLOCK_SPEED));

}

int main(void) {
	InitSysCtrl();

	// self booting
#ifdef _STANDALONE
#ifdef _FLASH
// Send boot command to allow the CPU2 application to begin execution
IPCBootCPU2(C1C2_BROM_BOOTMODE_BOOT_FROM_FLASH);
#else
// Send boot command to allow the CPU2 application to begin execution
IPCBootCPU2(C1C2_BROM_BOOTMODE_BOOT_FROM_RAM);
#endif
#endif

// TODO: put copy from flash to RAM here

// Call Flash Initialization to setup flash waitstates
// This function must reside in RAM
#ifdef _FLASH
   InitFlash();
#endif

	// init GPIO so CPU2 can access the LEDs
	InitGpio();
	EALLOW;
	GPIO_SetupPinOptions(31, GPIO_OUTPUT, GPIO_PUSHPULL);
	GPIO_SetupPinMux(31, GPIO_MUX_CPU2, 0);
	GPIO_SetupPinOptions(34, GPIO_OUTPUT, GPIO_PUSHPULL);
	GPIO_SetupPinMux(34, GPIO_MUX_CPU2, 0);
	EDIS;
	
    //
    // Set the clocking to run from the PLL at 50MHz
    // Needed for USB
    SysCtlClockSet(SYSCTL_OSCSRC_OSC2 | SYSCTL_PLL_ENABLE | SYSCTL_IMULT(10) | SYSCTL_SYSDIV(2));
    SysCtlAuxClockSet(SYSCTL_OSCSRC_OSC2 | SYSCTL_PLL_ENABLE | SYSCTL_IMULT(12) | SYSCTL_SYSDIV(2));	//60 MHz

    //
	// Initialize interrupt controller and vector table
	//
	InitPieCtrl();

	// Disable and clear interrupts
	IER = 0x0000;
	IFR = 0x0000;

	InitPieVectTable();

	EINT;  // Enable Global interrupt INTM
	ERTM;  // Enable Global realtime interrupt DBGM

	// Enable Interrupts
	IntMasterEnable();

	ConfigureUART();

	for(;;) {
		UARTprintf("\n\nHello UART\n");
		DELAY_US(1000 * 500);
	}
}
