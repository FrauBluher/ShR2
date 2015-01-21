/*
 * main.c
 */
#include "F28x_Project.h"

int main(void) {
	InitSysCtrl();

	EALLOW;
	//TODO Add code to configure GPADIR through IPC
	GPIO_WritePin(31, 1);
	GPIO_WritePin(34, 1);
	InitSysCtrl();
// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
	DINT;

// Initialize the PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.
// This function is found in the F2837xD_PieCtrl.c file.
	InitPieCtrl();

// Disable CPU interrupts and clear all CPU interrupt flags:
	IER = 0x0000;
	IFR = 0x0000;

// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in F2837xD_DefaultIsr.c.
// This function is found in F2837xD_PieVect.c.
	InitPieVectTable();



// Enable global Interrupts and higher priority real-time debug events:
	EINT;  // Enable Global interrupt INTM
	ERTM;  // Enable Global realtime interrupt DBGM

// Step 6. IDLE loop. Just sit and loop forever (optional):
	for(;;)
	{
		//
		// Turn on LED
		//
		GPIO_WritePin(34, 0);
		GPIO_WritePin(31, 1);
		//
		// Delay for a bit.
		//
		DELAY_US(1000 * 250);

		//
		// Turn off LED
		//
		GPIO_WritePin(34, 1);
		GPIO_WritePin(31, 0);
		//
		// Delay for a bit.
		//
		DELAY_US(1000 * 250);


	}
}
