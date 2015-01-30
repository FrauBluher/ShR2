/*
 *  Henry Crute
 * 	hcrute@ucsc.edu
 * 	
 * contains the c code responsible for the logic behind sending,
 * storing, and receiving data.
 * 
 */

//#include "at.h"
//#include "user_interface.h"
#include "espmissingincludes.h"
#include "osapi.h"
#include "uart.h"
#include "ets_sys.h"
#include "c_types.h"

#include "send_recv_port.h"
#include "buffers.h"

//prototypes
void send_recv_store_fsm(char);
void case_idle(char);
void case_recv(char);
void case_store(char);
void case_send(char);
void case_recv_store(char);
void case_recv_send(char);


//os event queues for function priority queue
os_event_t    recv_messageQueue[recv_messageQueueLen];
os_event_t    store_send_messageQueue[store_send_messageQueueLen];

//initialize the user state to receive
rss_state user_state = receive_idle;

//flag that checks to see if it should echo rx
bool echoFlag = TRUE;
//flag that tells you if you are connected to an access point?
bool connected = FALSE;
//are we storing data?
bool storing = FALSE;
//are we sending data?
bool sending = FALSE;

/**
  * @brief  Uart receive task.
  * @param  events: contain the uart receive data
  * @retval None
  */
void ICACHE_FLASH_ATTR
recv_message(os_event_t *events) {
	//usually set temp equal to the UART character at any time
	uint8_t temp;
	//while loop getting characters while they are in the buffer
	while(READ_PERI_REG(UART_STATUS(UART0)) & (UART_RXFIFO_CNT << UART_RXFIFO_CNT_S)) {
		WRITE_PERI_REG(0X60000914, 0x73); //WTD
		temp = READ_PERI_REG(UART_FIFO(UART0)) & 0xFF;
		//echo back if flag is true
		if (echoFlag) {
			uart0_putChar(temp);
		}
		//just the fsm for sending, recv, and store
		send_recv_store_fsm(temp);
	}
	//handles uart interrupts
	if(UART_RXFIFO_FULL_INT_ST == (READ_PERI_REG(UART_INT_ST(UART0)) & UART_RXFIFO_FULL_INT_ST)) {
		WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR);
	} else if(UART_RXFIFO_TOUT_INT_ST == (READ_PERI_REG(UART_INT_ST(UART0)) & UART_RXFIFO_TOUT_INT_ST)) {
		WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_TOUT_INT_CLR);
	}
	ETS_UART_INTR_ENABLE();
}

void ICACHE_FLASH_ATTR
send_recv_store_fsm(char temp) {
	switch(user_state) {
	case receive_idle:
		case_idle(temp);
		break;
	case receive:
		case_recv(temp);
		break;
	case idle_store:
		case_store(temp);
		break;
	case idle_send:
		case_send(temp);
		break;
	//reveiving and storing at the same time
	case receive_store:
		case_recv_store(temp);
		break;
	//receiving and sending at the same time
	case receive_send:
		case_recv_send(temp);
		break;
	//default case, will never happen hopefully
	default:
		if(temp == '\n') {
			uart0_sendStr("Error State...\r\n");
		}
		break;
    }
}

/**
  * @brief  Following functions are used to handle the cases for
  * the state machine
  * @param  Temp contains the uart data received for that interval
  * @retval None!
  */

void ICACHE_FLASH_ATTR
case_idle(char temp) {
	//if the character is a dollar sign, we are no longer idle and
	//a message is beginning!
	if (temp == '$') {
		user_state = receive;
		//puts shit on the buffer
		put_buffer(temp);
	//TODO change to if is connected to access point, and send 
	//is greater than 0
	} else if (size_send_buffer() > 0) {
		sending = TRUE;
		system_os_post(store_send_messagePrio, 0, 0);
		user_state = idle_send;
	}
}

void ICACHE_FLASH_ATTR
case_recv(char temp) {
	if (temp == '\n') {
		//message terminating, storing into the send buffer
		//nmea checksum on the buffer
		if (checksum_buffer()) {
			//store it into the circular buffer
			uart0_sendStr("\r\nchecksum succeeded...\r\n");
			print_buffer();
			storing = TRUE;
			//swap and then reset current buffer
			swap_buffer();
			reset_buffer();
			system_os_post(store_send_messagePrio, 0, 0);
			user_state = idle_store;
		} else {
			uart0_sendStr("\r\nchecksum failed...\r\n");
			print_buffer();
			reset_buffer();
			user_state = receive_idle;
		}
	//if the buffer overflows, then reset the buffer and go back
	//to idle
	} else if (!put_buffer(temp)) {
		print_buffer();
		reset_buffer();
		user_state = receive_idle;
	}
}

void ICACHE_FLASH_ATTR
case_store(char temp) {
	if (storing == FALSE && temp != '$') {
		user_state = receive_idle;
	} else if (storing == FALSE && temp == '$') {
		user_state = receive;
		//puts shit on the buffer
		put_buffer(temp);
	} else if (storing == TRUE && temp == '$') {
		user_state = receive_store;
		//puts shit on the buffer
		put_buffer(temp);
	} else if (storing == TRUE && temp != '$') {
		user_state = idle_store;
	}
}

void ICACHE_FLASH_ATTR
case_send(char temp) {
	if (sending == FALSE && temp != '$') {
		user_state = receive_idle;
	} else if (sending == FALSE && temp == '$') {
		user_state = receive;
		//put shit on the buffer
		put_buffer(temp);
	} else if (sending == TRUE && temp == '$') {
		user_state = receive_send;
		//puts shit on the buffer
		put_buffer(temp);
	} else if (sending == TRUE && temp != '$') {
		user_state = idle_send;
	}
}

void ICACHE_FLASH_ATTR
case_recv_store(char temp) {
	if (storing == FALSE && temp != '\n') {
		put_buffer(temp);
		user_state = receive;
	} else if (storing == FALSE && temp == '\n') {
		if (checksum_buffer()) {
			//store it into the circular buffer
			uart0_sendStr("\r\nchecksum succeeded...\r\n");
			print_buffer();
			storing = TRUE;
			//swap and then reset buffer
			swap_buffer();
			reset_buffer();
			system_os_post(store_send_messagePrio, 0, 0);
			user_state = idle_store;
		} else {
			uart0_sendStr("\r\nchecksum failed...\r\n");
			print_buffer();			
			reset_buffer();
			user_state = receive_idle;
		}
	} else if (storing == TRUE && temp != '\n') {
		//continue to receive shit and store, in case of overflow
		if (!put_buffer(temp)) {
			print_buffer();
			reset_buffer();
			user_state = idle_store;
		}
		user_state = receive_store;
	} else if (storing == TRUE && temp == '\n') {
		//too fast for the buffers to handle, reset and drop message
		reset_buffer();
		user_state = idle_store;
	}
}

void ICACHE_FLASH_ATTR
case_recv_send(char temp) {
	if (sending == FALSE && temp != '\n') {
		if (!put_buffer(temp)) {
			print_buffer();
			reset_buffer();
			user_state = idle_store;
		}
		user_state = receive;
	} else if (sending == FALSE && temp == '\n') {
		if (checksum_buffer()) {
			//store it into the circular buffer
			uart0_sendStr("\r\nchecksum succeeded...\r\n");
			print_buffer();
			storing = TRUE;
			//swap and reset buffer
			swap_buffer();
			reset_buffer();
			system_os_post(store_send_messagePrio, 0, 0);
			user_state = idle_store;
		} else {
			uart0_sendStr("\r\nchecksum failed...\r\n");
			print_buffer();
			reset_buffer();
			user_state = receive_idle;
		}
	} else if (sending == TRUE && temp != '\n') {
		put_buffer(temp);
		user_state = receive_send;
	} else if (sending == TRUE && temp == '\n') {
		//drop message, got a message before we could send
		reset_buffer();
		user_state = idle_send;
	}
}

/**
  * @brief  Stores the messages received in a data structure
  * Sends messages if there is connectivity
  * @param  events: not used
  * @retval None
  */
void ICACHE_FLASH_ATTR
store_send_message(os_event_t *events) {
	if (user_state == idle_store || user_state == receive_store) {
		uart0_sendStr("\r\nstoring...\r\n");
		if (push_send_buffer()) {
			uart0_sendStr("\r\nsucceeded...\r\n");
		}
		storing = FALSE;
	}
	if (user_state == idle_send || user_state == receive_send) {
		uart0_sendStr("\r\nsending...\r\n");
		if (send_pop_buffer()) {
			uart0_sendStr("\r\nsucceeded...\r\n");
		}
		sending = FALSE;
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
	system_os_task(store_send_message, store_send_messagePrio,
				   store_send_messageQueue, store_send_messageQueueLen);
}

/**
  * @}
  */
