/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Pavlo Milo Manovi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/**
 * @file	DAQ.c
 * @author 	Pavlo Milo Manovi
 * @date	April, 2014
 * @brief 	Main loop for the DAQ.
 */


#include "ADCModuleBoard.h"
#include "SB_Computation.h"

#include <plib.h>

SampleBuffer BufferA;
SampleBuffer BufferB;

static MCP391x_Info ADCInfo;

/**
  * @brief  Takes the buffer not used, and does useful computations on it
  * @param  None
  * @retval None
  */
void ComputeBuffer(void)
{
        uint32_t RMS_Value = 0;
        uint8_t currentBuffer = CurrentBuffer();
        if (currentBuffer == BUFFER_A) {
                //compute things on the buffer!
                RMS_Value = SB_RMS(&BufferB);
                //TODO: DMA these variables, instead of the whole SampleBuffer s
                        uint8_t send_buf[20];
                        uint8_t write_len = sprintf(send_buf, "rms=%014d\r\n", RMS_Value);
                        BufferToPMP_Transfer(send_buf, write_len);
        } else if (currentBuffer == BUFFER_B) {
                        //compute things on the buffer!
                        RMS_Value = SB_RMS(&BufferA);
                        //TODO: DMA these variables, instead of the whole SampleBuffer s
                        uint8_t send_buf[20];
                        uint8_t write_len = sprintf(send_buf, "rms=%014d\r\n", RMS_Value);
                        BufferToPMP_Transfer(send_buf, write_len);
        }
}


int main(void)
{
	ADCModuleBoard_Init(&BufferA, &BufferB, &ADCInfo);
        uint8_t done = FALSE;
        uint8_t prevBuffer = CurrentBuffer();
	while (1) {
                //grabs the buffer that isn't being DMA'D at that point
		//does copmutations with it
                uint8_t currentBuffer = CurrentBuffer();
                if (currentBuffer != prevBuffer) {
                        done = FALSE;
                        prevBuffer = currentBuffer;
                }
                if (done == FALSE) {
                        ComputeBuffer();
                        done = TRUE;
                }
	}
}
