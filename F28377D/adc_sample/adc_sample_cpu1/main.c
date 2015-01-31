/*
 * main.c
 */
#include "F28x_Project.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "utils/uartstdio.h"
#include "inc/hw_memmap.h"
#include "driverlib/uart.h"


void ConfigureADC(void);
void SetupADCContinuous(Uint16 channel);

//buffer for storing conversion results (size must be multiple of 16)
#define RESULTS_BUFFER_SIZE 256
Uint16 AdcaResults[RESULTS_BUFFER_SIZE];
Uint16 resultsIndex;

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

    /*
    //
    // Configure and enable UART interrupts.
    //
    UARTIntClear(USB_UART_BASE, UARTIntStatus(USB_UART_BASE, false));
	UARTIntEnable(USB_UART_BASE, (UART_INT_RXERR | UART_INT_RXRDY_BRKDT)); // | UART_INT_TXRDY ));
	//UARTTXIntRegister(USB_UART_BASE, USBUARTTXIntHandler);
	UARTRXIntRegister(USB_UART_BASE, USBUARTRXIntHandler);
	*/

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
	/*
	EALLOW;
	GPIO_SetupPinOptions(31, GPIO_OUTPUT, GPIO_PUSHPULL);
	GPIO_SetupPinMux(31, GPIO_MUX_CPU2, 0);
	GPIO_SetupPinOptions(34, GPIO_OUTPUT, GPIO_PUSHPULL);
	GPIO_SetupPinMux(34, GPIO_MUX_CPU2, 0);
	EDIS;
	*/

    //
    // Set the clocking to run from the PLL at 50MHz
    // Needed for USB
    SysCtlClockSet(SYSCTL_OSCSRC_OSC2 | SYSCTL_PLL_ENABLE | SYSCTL_IMULT(10) | SYSCTL_SYSDIV(2));
    SysCtlAuxClockSet(SYSCTL_OSCSRC_OSC2 | SYSCTL_PLL_ENABLE | SYSCTL_IMULT(12) | SYSCTL_SYSDIV(2));	//60 MHz

    //
	// Initialize interrupt controller and vector table
	//
    DINT;
	InitPieCtrl();

	// Disable and clear interrupts
	IER = 0x0000;
	IFR = 0x0000;

	InitPieVectTable();

	// endable and configure the ADC
	ConfigureADC();

	//Setup the ADC for continuous conversions on channel 0
	SetupADCContinuous(0);

	EINT;  // Enable Global interrupt INTM
	ERTM;  // Enable Global realtime interrupt DBGM

	//Initialize results buffer
	//TODO: use memset instead
	for(resultsIndex = 0; resultsIndex < RESULTS_BUFFER_SIZE; resultsIndex++)
	{
		AdcaResults[resultsIndex] = 0;
	}
	resultsIndex = 0;

	// Enable Interrupts
	IntMasterEnable();

	ConfigureUART();
}

void sample_adc(void) {
	EALLOW;
	//enable ADCINT flags
	AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;
	AdcaRegs.ADCINTSEL1N2.bit.INT2E = 1;
	AdcaRegs.ADCINTSEL3N4.bit.INT3E = 1;
	AdcaRegs.ADCINTSEL3N4.bit.INT4E = 1;
	AdcaRegs.ADCINTFLGCLR.all = 0x000F;

	//initialize results index
	resultsIndex = 0;
	//software force start SOC0 to SOC7
	AdcaRegs.ADCSOCFRC1.all = 0x00FF;
	//keep taking samples until the results buffer is full
	while(resultsIndex < RESULTS_BUFFER_SIZE)
	{
		resultsIndex = 0;
		//wait for first set of 8 conversions to complete
		while(0 == AdcaRegs.ADCINTFLG.bit.ADCINT3);

		//clear both INT flags generated by first 8 conversions
		AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
		AdcaRegs.ADCINTFLGCLR.bit.ADCINT3 = 1;

		//save results for first 8 conversions
		//
		//note that during this time, the second 8 conversions have
		//already been triggered by EOC6->ADCIN1 and will be actively
		//converting while first 8 results are being saved
		AdcaResults[resultsIndex++] = AdcaResultRegs.ADCRESULT0;
		AdcaResults[resultsIndex++] = AdcaResultRegs.ADCRESULT1;
		AdcaResults[resultsIndex++] = AdcaResultRegs.ADCRESULT2;
		AdcaResults[resultsIndex++] = AdcaResultRegs.ADCRESULT3;
		AdcaResults[resultsIndex++] = AdcaResultRegs.ADCRESULT4;
		AdcaResults[resultsIndex++] = AdcaResultRegs.ADCRESULT5;
		AdcaResults[resultsIndex++] = AdcaResultRegs.ADCRESULT6;
		AdcaResults[resultsIndex++] = AdcaResultRegs.ADCRESULT7;
		int i;
		for (i = 0; i < 8; ++i) {
			UARTCharPutNonBlocking(USB_UART_BASE, AdcaResults[i]&0xFF);
			UARTCharPutNonBlocking(USB_UART_BASE, (AdcaResults[i]&0xFF00) >> 8);
		}

		//wait for the second set of 8 conversions to complete
		while(0 == AdcaRegs.ADCINTFLG.bit.ADCINT4);
		//clear both INT flags generated by second 8 conversions
		AdcaRegs.ADCINTFLGCLR.bit.ADCINT2 = 1;
		AdcaRegs.ADCINTFLGCLR.bit.ADCINT4 = 1;

		//save results for second 8 conversions
		//
		//note that during this time, the first 8 conversions have
		//already been triggered by EOC14->ADCIN2 and will be actively
		//converting while second 8 results are being saved
		AdcaResults[resultsIndex++] = AdcaResultRegs.ADCRESULT8;
		AdcaResults[resultsIndex++] = AdcaResultRegs.ADCRESULT9;
		AdcaResults[resultsIndex++] = AdcaResultRegs.ADCRESULT10;
		AdcaResults[resultsIndex++] = AdcaResultRegs.ADCRESULT11;
		AdcaResults[resultsIndex++] = AdcaResultRegs.ADCRESULT12;
		AdcaResults[resultsIndex++] = AdcaResultRegs.ADCRESULT13;
		AdcaResults[resultsIndex++] = AdcaResultRegs.ADCRESULT14;
		AdcaResults[resultsIndex++] = AdcaResultRegs.ADCRESULT15;
		for (i = 8; i < 16; ++i) {
			UARTCharPutNonBlocking(USB_UART_BASE, AdcaResults[i]&0xFF);
			UARTCharPutNonBlocking(USB_UART_BASE, (AdcaResults[i]&0xFF00) >> 8);
		}
	}

	//disable all ADCINT flags to stop sampling
	AdcaRegs.ADCINTSEL1N2.bit.INT1E = 0;
	AdcaRegs.ADCINTSEL1N2.bit.INT2E = 0;
	AdcaRegs.ADCINTSEL3N4.bit.INT3E = 0;
	AdcaRegs.ADCINTSEL3N4.bit.INT4E = 0;

	//at this point, AdcaResults[] contains a sequence of conversions
	//from the selected channel

	//software breakpoint, hit run again to get updated conversions
	//asm("   ESTOP0");
}

int main(void) {
	init();

	for(;;) {
		// sample one round until buffer is full
		sample_adc();
		// copy the buffer over UART
		//UARTwrite((char *)AdcaResults, sizeof(AdcaResults));
	}
}

//TODO: move these somewhere better

//Write ADC configurations and power up the ADC for both ADC A and ADC B
void ConfigureADC(void)
{
	EALLOW;

	//write configurations
	AdcaRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_16BIT, ADC_SIGNALMODE_DIFFERENTIAL);

	//Set pulse positions to late
	AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;

	//power up the ADC
	AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;

	//delay for 1ms to allow ADC time to power up
	DELAY_US(1000);

	EDIS;
}

//setup the ADC to continuously convert on one channel
void SetupADCContinuous(Uint16 channel)
{
	Uint16 acqps;

	//determine minimum acquisition window (in SYSCLKS) based on resolution
	if(ADC_RESOLUTION_12BIT == AdcaRegs.ADCCTL2.bit.RESOLUTION){
		acqps = 14; //75ns
	}
	else { //resolution is 16-bit
		acqps = 63; //320ns
	}

	EALLOW;
	AdcaRegs.ADCSOC0CTL.bit.CHSEL  = channel;  //SOC will convert on channel
	AdcaRegs.ADCSOC1CTL.bit.CHSEL  = channel;  //SOC will convert on channel
	AdcaRegs.ADCSOC2CTL.bit.CHSEL  = channel;  //SOC will convert on channel
	AdcaRegs.ADCSOC3CTL.bit.CHSEL  = channel;  //SOC will convert on channel
	AdcaRegs.ADCSOC4CTL.bit.CHSEL  = channel;  //SOC will convert on channel
	AdcaRegs.ADCSOC5CTL.bit.CHSEL  = channel;  //SOC will convert on channel
	AdcaRegs.ADCSOC6CTL.bit.CHSEL  = channel;  //SOC will convert on channel
	AdcaRegs.ADCSOC7CTL.bit.CHSEL  = channel;  //SOC will convert on channel
	AdcaRegs.ADCSOC8CTL.bit.CHSEL  = channel;  //SOC will convert on channel
	AdcaRegs.ADCSOC9CTL.bit.CHSEL  = channel;  //SOC will convert on channel
	AdcaRegs.ADCSOC10CTL.bit.CHSEL = channel;  //SOC will convert on channel
	AdcaRegs.ADCSOC11CTL.bit.CHSEL = channel;  //SOC will convert on channel
	AdcaRegs.ADCSOC12CTL.bit.CHSEL = channel;  //SOC will convert on channel
	AdcaRegs.ADCSOC13CTL.bit.CHSEL = channel;  //SOC will convert on channel
	AdcaRegs.ADCSOC14CTL.bit.CHSEL = channel;  //SOC will convert on channel
	AdcaRegs.ADCSOC15CTL.bit.CHSEL = channel;  //SOC will convert on channel

	AdcaRegs.ADCSOC0CTL.bit.ACQPS  = acqps;    //sample window is acqps + 1 SYSCLK cycles
	AdcaRegs.ADCSOC1CTL.bit.ACQPS  = acqps;    //sample window is acqps + 1 SYSCLK cycles
	AdcaRegs.ADCSOC2CTL.bit.ACQPS  = acqps;    //sample window is acqps + 1 SYSCLK cycles
	AdcaRegs.ADCSOC3CTL.bit.ACQPS  = acqps;    //sample window is acqps + 1 SYSCLK cycles
	AdcaRegs.ADCSOC4CTL.bit.ACQPS  = acqps;    //sample window is acqps + 1 SYSCLK cycles
	AdcaRegs.ADCSOC5CTL.bit.ACQPS  = acqps;    //sample window is acqps + 1 SYSCLK cycles
	AdcaRegs.ADCSOC6CTL.bit.ACQPS  = acqps;    //sample window is acqps + 1 SYSCLK cycles
	AdcaRegs.ADCSOC7CTL.bit.ACQPS  = acqps;    //sample window is acqps + 1 SYSCLK cycles
	AdcaRegs.ADCSOC8CTL.bit.ACQPS  = acqps;    //sample window is acqps + 1 SYSCLK cycles
	AdcaRegs.ADCSOC9CTL.bit.ACQPS  = acqps;    //sample window is acqps + 1 SYSCLK cycles
	AdcaRegs.ADCSOC10CTL.bit.ACQPS = acqps;    //sample window is acqps + 1 SYSCLK cycles
	AdcaRegs.ADCSOC11CTL.bit.ACQPS = acqps;    //sample window is acqps + 1 SYSCLK cycles
	AdcaRegs.ADCSOC12CTL.bit.ACQPS = acqps;    //sample window is acqps + 1 SYSCLK cycles
	AdcaRegs.ADCSOC13CTL.bit.ACQPS = acqps;    //sample window is acqps + 1 SYSCLK cycles
	AdcaRegs.ADCSOC14CTL.bit.ACQPS = acqps;    //sample window is acqps + 1 SYSCLK cycles
	AdcaRegs.ADCSOC15CTL.bit.ACQPS = acqps;    //sample window is acqps + 1 SYSCLK cycles

	AdcaRegs.ADCINTSEL1N2.bit.INT1E = 0; //disable INT1 flag
	AdcaRegs.ADCINTSEL1N2.bit.INT2E = 0; //disable INT2 flag
	AdcaRegs.ADCINTSEL3N4.bit.INT3E = 0; //disable INT3 flag
	AdcaRegs.ADCINTSEL3N4.bit.INT4E = 0; //disable INT4 flag

	AdcaRegs.ADCINTSEL1N2.bit.INT1CONT = 0;
	AdcaRegs.ADCINTSEL1N2.bit.INT2CONT = 0;
	AdcaRegs.ADCINTSEL3N4.bit.INT3CONT = 0;
	AdcaRegs.ADCINTSEL3N4.bit.INT4CONT = 0;

	AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 6;  //end of SOC6 will set INT1 flag
	AdcaRegs.ADCINTSEL1N2.bit.INT2SEL = 14; //end of SOC14 will set INT2 flag
	AdcaRegs.ADCINTSEL3N4.bit.INT3SEL = 7;  //end of SOC7 will set INT3 flag
	AdcaRegs.ADCINTSEL3N4.bit.INT4SEL = 15; //end of SOC15 will set INT4 flag

	//ADCINT2 will trigger first 8 SOCs
   	AdcaRegs.ADCINTSOCSEL1.bit.SOC0 = 2;
   	AdcaRegs.ADCINTSOCSEL1.bit.SOC1 = 2;
   	AdcaRegs.ADCINTSOCSEL1.bit.SOC2 = 2;
   	AdcaRegs.ADCINTSOCSEL1.bit.SOC3 = 2;
   	AdcaRegs.ADCINTSOCSEL1.bit.SOC4 = 2;
   	AdcaRegs.ADCINTSOCSEL1.bit.SOC5 = 2;
   	AdcaRegs.ADCINTSOCSEL1.bit.SOC6 = 2;
   	AdcaRegs.ADCINTSOCSEL1.bit.SOC7 = 2;

	//ADCINT1 will trigger second 8 SOCs
   	AdcaRegs.ADCINTSOCSEL2.bit.SOC8 = 1;
   	AdcaRegs.ADCINTSOCSEL2.bit.SOC9 = 1;
   	AdcaRegs.ADCINTSOCSEL2.bit.SOC10 = 1;
   	AdcaRegs.ADCINTSOCSEL2.bit.SOC11 = 1;
   	AdcaRegs.ADCINTSOCSEL2.bit.SOC12 = 1;
   	AdcaRegs.ADCINTSOCSEL2.bit.SOC13 = 1;
   	AdcaRegs.ADCINTSOCSEL2.bit.SOC14 = 1;
   	AdcaRegs.ADCINTSOCSEL2.bit.SOC15 = 1;
}
