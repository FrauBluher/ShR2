/*
 * Henry Crute
 * hcrute@ucsc.edu
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
#include "http_request.h"

//prototypes
void send_recv_store_fsm(char);
void case_idle(char);
void case_recv(char);
void case_store(char);
void case_send(char);
void case_recv_store(char);
void case_recv_send(char);

//default sending timer value to be possibly modified later
uint16_t sending_timer = 4000;

//os event queues for function priority queue
os_event_t    recv_messageQueue[recv_messageQueueLen];
os_event_t    store_send_messageQueue[store_send_messageQueueLen];

//initialize the user state to receive
rss_state user_state = IDLE;

//flag that checks to see if it should echo rx
bool echoFlag = TRUE;
//are we storing data?
bool storing = FALSE;
//are we sending data?
bool sending = FALSE;
//do we have config?
bool config = FALSE;

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
	//handles uart interrupt flags
	if(UART_RXFIFO_FULL_INT_ST == (READ_PERI_REG(UART_INT_ST(UART0)) & UART_RXFIFO_FULL_INT_ST)) {
		WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR);
	} else if(UART_RXFIFO_TOUT_INT_ST == (READ_PERI_REG(UART_INT_ST(UART0)) & UART_RXFIFO_TOUT_INT_ST)) {
		WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_TOUT_INT_CLR);
	}
	ETS_UART_INTR_ENABLE();
}

/**
  * @brief  Continuously called state machine, for message processing
  * @param  contains the uart character received
  * @retval None
  */
void ICACHE_FLASH_ATTR
send_recv_store_fsm(char temp) {
	switch(user_state) {
	case IDLE:
		case_idle(temp);
		break;
	case RECEIVE:
		case_recv(temp);
		break;
	case STORE:
		case_store(temp);
		break;
	case RECEIVE_STORE:
		case_recv_store(temp);
		break;
	//default case, will never happen
	default:
		if(temp == '\n') {
			uart0_sendStr("ERR, RESET PLS\r\n");
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
//what happens when we are not receiving anything interesting
void ICACHE_FLASH_ATTR
case_idle(char temp) {
	//if the character is a dollar sign, we are no longer idle and
	//a message is beginning!
	if (temp == '$') {
		user_state = RECEIVE;
		//puts the char on the buffer
		put_buffer(temp);
	}
}

/**
  * @brief  The state when we are in the middle of receiving a message
  * @param  Temp contains the uart data received for that interval
  * @retval None
  */
void ICACHE_FLASH_ATTR
case_recv(char temp) {
	if (temp == '\n') {
		//message terminating, storing into the send buffer
		//nmea checksum on the buffer
		if (checksum_buffer()) {
			//store it into the circular buffer
			//os_printf("\r\nchecksum succeeded\r\n");
			//print_buffer();
			storing = TRUE;
			//swap and then reset current buffer
			swap_buffer();
			reset_buffer();
			system_os_post(store_send_messagePrio, 0, 0);
			user_state = STORE;
		} else {
			//os_printf("\r\nchecksum failed\r\n");
			//print_buffer();
			reset_buffer();
			user_state = IDLE;
		}
	} else if (!put_buffer(temp)) {
		//if the buffer overflows, then reset the buffer and go back
		//to idle. This is the case that puts things into the buffer
		os_printf("\r\noverflow\r\n");
		//print_buffer();
		reset_buffer();
		user_state = IDLE;
	}
}


/**
  * @brief  The state when we are done receiving a message (store it)
  * @param  Temp contains the uart data received for that interval
  * @retval None
  */
void ICACHE_FLASH_ATTR
case_store(char temp) {
	//send stuff right after storing?
	if (storing == FALSE && temp != '$' && size_send_buffer() > 0) {
		user_state = IDLE;
	} else if (storing == FALSE && temp != '$') {
		user_state = IDLE;
	} else if (storing == FALSE && temp == '$' && size_send_buffer() > 0) {
		user_state = RECEIVE;
		//puts shit on the buffer
		put_buffer(temp);
	} else if (storing == FALSE && temp == '$') {
		user_state = RECEIVE;
		//puts shit on the buffer
		put_buffer(temp);
	} else if (storing == TRUE && temp == '$') {
		user_state = RECEIVE_STORE;
		//puts shit on the buffer
		put_buffer(temp);
	} else if (storing == TRUE && temp != '$') {
		user_state = STORE;
	}
}

/**
  * @brief  The state when we are storing and received a new message
  * 		beginning
  * @param  Temp contains the uart data received for that interval
  * @retval None
  */
void ICACHE_FLASH_ATTR
case_recv_store(char temp) {
	if (storing == FALSE && temp != '\n') {
		put_buffer(temp);
		user_state = RECEIVE;
	} else if (storing == FALSE && temp == '\n') {
		if (checksum_buffer()) {
			//store it into the circular buffer
			//uart0_sendStr("\r\nchecksum succeeded\r\n");
			//print_buffer();
			storing = TRUE;
			//swap and then reset buffer
			swap_buffer();
			reset_buffer();
			system_os_post(store_send_messagePrio, 0, 0);
			user_state = STORE;
		} else {
			//uart0_sendStr("\r\nchecksum failed\r\n");
			//print_buffer();			
			reset_buffer();
			user_state = IDLE;
		}
	} else if (storing == TRUE && temp != '\n') {
		//continue to receive shit and store, in case of overflow
		if (!put_buffer(temp)) {
			os_printf("\r\noverflow\r\n");
			//print_buffer();
			reset_buffer();
			user_state = STORE;
		} else {
			user_state = RECEIVE_STORE;
		}
	} else if (storing == TRUE && temp == '\n') {
		//too fast for the buffers to handle, reset and drop message
		os_printf("\r\nmessage dropped\r\n");
		reset_buffer();
		user_state = STORE;
	}
}

/**
  * @brief  Stores the messages received in a data buffer
  * 		Sends messages if there is connectivity
  * @param  events: not used
  * @retval None
  */
void ICACHE_FLASH_ATTR
store_send_message(os_event_t *events) {
	//always store before we send, if this happens twice
	if (user_state == STORE || user_state == RECEIVE_STORE) {
		//os_printf("\r\nstoring\r\n");
		if (push_send_buffer()) {
			//os_printf("\r\nsucceeded\r\n");
		} else {
			//os_printf("\r\nfailed\r\n");
		}
		storing = FALSE;
	}
}

/**
  * @brief  Timer driven function call for checking to see if we have
  * 		data to format and send out to the server
  * @param  None
  * @retval None
  */
static void ICACHE_FLASH_ATTR
send_timer_cb(void *arg) {
	//sending timer
	static ETSTimer send_timer;
	//getting intial config
	if (config == FALSE) {
		if (get_http_config()) {
			uart0_sendStr("configuring\r\n");
		}
	//checks to see if we should send
	} else if (sending == FALSE && size_send_buffer() > 0) {
		uart0_sendStr("\r\nsending\r\n");
		sending = TRUE;
		//we can't tell when it's done sending after this, so 
		//the sending flag will be false after the command
		//to send the data has been issued
		if (!send_pop_buffer()) {
			sending = FALSE;
		}
	}
	os_timer_disarm(&send_timer);
	os_timer_setfn(&send_timer, send_timer_cb, NULL);
	os_timer_arm(&send_timer, sending_timer, 0);
}

/**
  * @brief  Sets sending timer variable for sending callback function
  * @param  None
  * @retval None
  */
void ICACHE_FLASH_ATTR
set_rate(uint16_t rate) {
	sending_timer = rate;
}

/**
  * @brief  Sets config flag to true
  * @param  None
  * @retval None
  */
void ICACHE_FLASH_ATTR
done_config(void) {
	config = TRUE;
}

/**
  * @brief  Sets sending flag to false
  * @param  None
  * @retval None
  */
void ICACHE_FLASH_ATTR
done_sending(void) {
	sending = FALSE;
}

/**
  * @brief  Initializes receiving, storing, and sending function queues
  * @param  None
  * @retval None
  */
void ICACHE_FLASH_ATTR
send_recv_init(void) {
	//indicates the uart has first priority, and a queue length of 64
	system_os_task(recv_message, recv_messagePrio,
				   recv_messageQueue, recv_messageQueueLen);
	//storing messages have second priority
	system_os_task(store_send_message, store_send_messagePrio,
				   store_send_messageQueue, store_send_messageQueueLen);
	send_timer_cb(0);
}

/**
  * @}
  */
