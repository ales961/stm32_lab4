#ifndef HARDWARE_UART_H
#define HARDWARE_UART_H

#include <stdint.h>

extern void uartReceiveIntCallback(uint8_t data);
extern void uartTransmitIntCallback();

uint8_t uartHardwareTransmitPolling(uint8_t data, uint16_t timeOut);
uint8_t uartHardwareReceivePolling(uint8_t * data, uint16_t timeOut);
void uartHardwareDisableInterrupts();

#endif
