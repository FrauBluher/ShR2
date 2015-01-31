ADC Sampling Project

This project continuously samples from the ADC 1 in 16 bit differential mode.
Outputs are printed out via UART.

Pins used:
ADC-A0 ANA09
ADC-A1 ANA11

Still missing is the use of DMA to speed up the transactions.
This project also is currently not continuous. It reads in 256 samples, sends
them out, then repeats. 
