/*
 * main.c
 */
#include "F28x_Project.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
#include "inc/hw_memmap.h"
#include "driverlib/uart.h"

//*****************************************************************************
//
// Defines required to redirect UART0 via USB.
//
//*****************************************************************************
#define USB_UART_BASE           UARTA_BASE
#define USB_UART_PERIPH         SYSCTL_PERIPH_SCI1
#define USB_UART_INT            INT_SCIRXINTA

extern void UARTStdioIntHandler(void);

//*****************************************************************************
//
// Interrupt handler for the UART TX
//
//*****************************************************************************
__interrupt void
USBUARTTXIntHandler(void)
{
    uint32_t ui32Ints;

    ui32Ints = UARTIntStatus(USB_UART_BASE, true);
    //
    // Handle transmit interrupts.
    //
    if(ui32Ints & UART_INT_TXRDY)
    {
        //
        // Move as many bytes as possible into the transmit FIFO.
        //
        //USBUARTPrimeTransmit(USB_UART_BASE);
    	//UARTCharPutNonBlocking(USB_UART_BASE, 'T');

        //
        // If the output buffer is empty, turn off the transmit interrupt.
        //
    	/*
        if(!USBBufferDataAvailable(&g_sRxBuffer))
        {
            UARTIntDisable(USB_UART_BASE, UART_INT_TXRDY);
        }
        */
    }

    PieCtrlRegs.PIEACK.all = 0x100;
}

//*****************************************************************************
//
// Interrupt handler for the UART RX which is being redirected via USB.
//
//*****************************************************************************
__interrupt void
USBUARTRXIntHandler(void)
{
    uint32_t u3i2Ints;

    u3i2Ints = UARTIntStatus(USB_UART_BASE, true);
    //
    // Handle receive interrupts.
    //
    if(u3i2Ints & UART_INT_RXRDY_BRKDT)
    {
        //
        // Read the UART's characters into the buffer.
        //
        //ReadUARTData();
    	// Blocking code in an interrupt = bad
    	while(UARTCharsAvail(USB_UART_BASE)) {
			//
			// Read a character from the UART FIFO if no
			// errors are reported.
			//
			uint32_t i32Char = UARTCharGetNonBlocking(USB_UART_BASE);
			if(!(i32Char & ~0xFF)) {
				uint8_t ui8Char = (uint8_t)(i32Char & 0xFF);
				UARTCharPutNonBlocking(USB_UART_BASE, ui8Char);
			}
			else {
				// error
			}
    	}
    }
    else if(u3i2Ints & UART_INT_RXERR)
    {
        //
        //Notify Host of our error
        //
        //CheckForSerialStateChange(&g_sCDCDevice, UARTRxErrorGet(USB_UART_BASE));

        //
        //Clear the error and continue
        //
        UARTRxErrorClear(USB_UART_BASE);
    }

    PieCtrlRegs.PIEACK.all = 0x100;

}

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

    //
    // Configure and enable UART interrupts.
    //
    UARTIntClear(USB_UART_BASE, UARTIntStatus(USB_UART_BASE, false));
	UARTIntEnable(USB_UART_BASE, (UART_INT_RXERR | UART_INT_RXRDY_BRKDT)); // | UART_INT_TXRDY ));
	//UARTTXIntRegister(USB_UART_BASE, USBUARTTXIntHandler);
	UARTRXIntRegister(USB_UART_BASE, USBUARTRXIntHandler);

}

void init(void) {
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

/*
 * Need to add proper linker configuration for this
#ifdef _FLASH
// Copy time critical code and Flash setup code to RAM
// This includes the following functions:  InitFlash();
// The  RamfuncsLoadStart, RamfuncsLoadSize, and RamfuncsRunStart
// symbols are created by the linker. Refer to the device .cmd file.
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
#endif
*/

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
}

int main(void) {
	init();

	for(;;) {
		UARTprintf("\n\nHello UART\n");
		DELAY_US(1000 * 500);
	}
}
