/*
 * command.c
 *
 *  Created on: 2020. 11. 11.
 *      Author: USER
 */
#include <string.h>
#include "usart.h"

int GetCommand(char *command, int len)
{
	char c;
	int i;
	int numRead;
	int maxRead = len - 1;

	for(numRead = 0, i = 0; numRead < maxRead;)
	{
		/* try to get a byte from the serial port */
//		c = Uart_Getch() & 0xff;
//		c = RX1_Char();
		c = DebugUart_GetChar();
//		HAL_UART_Receive(&huart1, &c, 1, 0xFFFF);

		/* check for errors */
		if(c < 0) {
			command[i++] = '\0';
//			Uart_SendByte('\n');
//			TX1_Char('\n');
//			TX1_Char('\r');
			DebugUart_PutChar('\n');
			DebugUart_PutChar('\r');
//			HAL_UART_Transmit(&huart1, '\n', 1, 0xFFFF);
//			HAL_UART_Transmit(&huart1, '\r', 1, 0xFFFF);
			return c;
		}
		if((c == '\r') || (c == '\n')) {
			command[i++] = '\0';

			/* print newline */
//			Uart_SendByte('\n');
//			TX1_Char('\n');
//			TX1_Char('\r');
			DebugUart_PutChar('\n');
			DebugUart_PutChar('\r');
//			HAL_UART_Transmit(&huart1, "\n", 1, 0xFFFF);
//			HAL_UART_Transmit(&huart1, "\r", 1, 0xFFFF);
			return(numRead);
		}
		else if(c == '\b') { /* FIXME: is this backspace? */
			if(i > 0) {
				i--;
				numRead--;
				/* cursor one position back. */
//				Uart_Printf("\b \b");
//				TX1_Str("\b \b");
				DebugUart_PutStr("\b \b");
//				HAL_UART_Transmit(&huart1, "\b \b", 3, 0xFFFF);
			}
		}
		else {
			command[i++] = c;
			numRead++;

			/* print character */
//			Uart_SendByte(c);
//			TX1_Char(c);
			DebugUart_PutChar(c);
//			HAL_UART_Transmit(&huart1, &c, 1, 0xFFFF);
		}
	}
	return(numRead);
}

int MyStrNCmp(const char *s1, const char *s2, int maxlen)
{
	int i;

	for(i = 0; i < maxlen; i++) {
		if(s1[i] != s2[i])
			return ((int) s1[i]) - ((int) s2[i]);
		if(s1[i] == 0)
			return 0;
	}

	return 0;
}

