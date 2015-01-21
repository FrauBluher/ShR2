/*
 *  Henry Crute
 * 	hcrute@ucsc.edu
 * 	
 * contains the c code responsible for the logic behind sending,
 * storing, and receiving data.
 * 
 */

//#include "at.h"
#include "user_interface.h"
#include "osapi.h"
#include "driver/uart.h"

#include "send_recv_port.h"
#include "buffers.h"

//os event queues for function priority queue
os_event_t    recv_messageQueue[recv_messageQueueLen];
os_event_t    store_messageQueue[store_messageQueueLen];
os_event_t    send_messageQueue[send_messageQueueLen];

//initialize the user state to receive
rss_state user_state = receive_idle;

//flag that checks to see if it should echo rx
bool echoFlag = FALSE;
//flag that tells you if you are connected to an access point?
bool connected = FALSE;

/**
  * @brief  Uart receive task.
  * @param  events: contain the uart receive data
  * @retval None
  */
void ICACHE_FLASH_ATTR
recv_message(os_event_t *events) {
	//init variables
	
	//usually set temp equal to the UART character at any time
	uint8_t temp;
	//while loop getting characters while they are in the buffer
	while(READ_PERI_REG(UART_STATUS(UART0)) & (UART_RXFIFO_CNT << UART_RXFIFO_CNT_S)) {
	WRITE_PERI_REG(0X60000914, 0x73); //WTD
	
	//if it is not storing from the buffer. don't worry because storing is
	//and should be fast
	if(user_state != store) {
		temp = READ_PERI_REG(UART_FIFO(UART0)) & 0xFF;
		//echo back if flag is true
		if (echoFlag) {
			uart_tx_one_char(temp);
		}
    }
    switch(user_state) {
	case receive_idle:
		//if the character is a dollar sign, we are no longer idle and
		//a message is beginning!
		if (temp == '$') {
			user_state = receive_message;
			//puts shit on the buffer
			put_buffer(temp);
		}
		break;
	case receive_message:
		if (temp == '\n') {
			//message terminating
			user_state = store;
		} else if (!put_buffer(temp)) { // buffer overflow
			reset_buffer();
			user_state = receive_idle;
		}
		break;
	//todo
	//can use two uart buffers for simultaneous storing and receiving?
	case store:
		//nmea checksum on the buffer
		if (checksum_buffer()) {
			//insert into send buffer
			put_send_buffer();
			reset_buffer();
			user_state = receive_idle;
		} else {
			reset_buffer();
			user_state = receive_idle;
		}
		break;
	//must not send and store at the same time!
	case send:
		;
		break;
	default:
		if(temp == '\n') {
		}
		break;
    }
    
    //end of the while loop
	}
	
	//handles uart interrupt, and calling business from the uart.c driver
	if(UART_RXFIFO_FULL_INT_ST == (READ_PERI_REG(UART_INT_ST(UART0)) & UART_RXFIFO_FULL_INT_ST)) {
		WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR);
	} else if(UART_RXFIFO_TOUT_INT_ST == (READ_PERI_REG(UART_INT_ST(UART0)) & UART_RXFIFO_TOUT_INT_ST)) {
		WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_TOUT_INT_CLR);
	}
	ETS_UART_INTR_ENABLE();
}

/**
  * @brief  Stores the messages received in a data structure
  * @param  events: not used
  * @retval None
  */
  /*
void ICACHE_FLASH_ATTR
store_message(os_event_t *events) {
	if (user_state == send) {
		uart0_sendStr("\r\nstoring...\r\n");
	}
}*/

/**
  * @brief  Sends the messages if we have connectivity
  * @param  events: not used
  * @retval None
  */
void ICACHE_FLASH_ATTR
send_message(os_event_t *events) {
	if (user_state == send) {
		uart0_sendStr("\r\nsending...\r\n");
	}
}

/**
  * @brief  Initializes receiving, storing, and sending function queues
  * @param  None
  * @retval None
  */
void ICACHE_FLASH_ATTR
send_recv_init(void) {
	//indicates the uart has first priority, and a queue length of 64
	//bytes to execute
	system_os_task(recv_message, recv_messagePrio,
				   recv_messageQueue, recv_messageQueueLen);
	//storing messages have second priority
	//system_os_task(store_message, store_messagePrio,
				   //store_messageQueue, store_messageQueueLen);
	//sending messages have third priority
	system_os_task(send_message, send_messagePrio,
			       send_messageQueue, send_messageQueueLen);
}

/**
  * @}
  */
