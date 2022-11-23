#include <string.h>
#include <stdio.h>
#include "main.h"
#include "pca9538.h"
#include "kb.h"
#include "usart.h"
#include "menu.h"

#define KBRD_ADDR 0xE2

uint8_t lastNKey = 0;
uint8_t mode = 1;

HAL_StatusTypeDef Set_Keyboard( void ) {
	HAL_StatusTypeDef ret = HAL_OK;
	uint8_t buf;

	buf = 0;
	ret = PCA9538_Write_Register(KBRD_ADDR, POLARITY_INVERSION, &buf);
	if( ret != HAL_OK ) {
		uartTransmit((uint8_t *) "Error write polarity\n", 21);
		goto exit;
	}

	buf = 0;
	ret = PCA9538_Write_Register(KBRD_ADDR, OUTPUT_PORT, &buf);
	if( ret != HAL_OK ) {
		uartTransmit((uint8_t *) "Error write output\n", 19);
	}

exit:
	return ret;
}

uint8_t Check_Row( uint8_t  Nrow ) {
	uint8_t Nkey = 0;
	HAL_StatusTypeDef ret = HAL_OK;
	uint8_t buf;
	uint8_t kbd_in;

	ret = Set_Keyboard();
	if( ret != HAL_OK ) {
		uartTransmit((uint8_t *) "Error write init\n", 17);
	}

	buf = Nrow;
	ret = PCA9538_Write_Register(KBRD_ADDR, CONFIG, &buf);
	if( ret != HAL_OK ) {
		uartTransmit((uint8_t *) "Error write config\n", 19);
	}

	ret = PCA9538_Read_Inputs(KBRD_ADDR, &buf);
	if( ret != HAL_OK ) {
		uartTransmit((uint8_t *) "Read error\n", 11);
	}

	uint8_t pressed = 0;
	kbd_in = buf & 0x70;
	Nkey = kbd_in;
	if( kbd_in != 0x70) {
		if( !(kbd_in & 0x10) ) {
			Nkey = 1;
			pressed++;
		}
		if( !(kbd_in & 0x20) ) {
			Nkey = 2;
			pressed++;
		}
		if( !(kbd_in & 0x40) ) {
			Nkey = 3;
			pressed++;
		}
	} else Nkey = 0;

	if (pressed > 1) {
		Nkey = lastNKey;
		pressed = 0;
	}
	return Nkey;
}

void kbKeyExecute(char* buf) {
	if (mode == 0) {
		const char *commandResult = menuExecuteCommand(buf);
		uartTransmit((uint8_t *) "\n", 1);
		uartTransmit((uint8_t *) commandResult, strlen(commandResult));
	}
	else {
		uartTransmit((uint8_t *) buf, strlen(buf));
		uartTransmit((uint8_t *) "\n", 1);
	}
}

uint8_t getLastNKey(uint8_t keyNumber) {
	if (keyNumber % 3 == 0) return 3;
	if (keyNumber % 3 == 1) return 1;
	if (keyNumber % 3 == 2) return 2;
}

uint32_t clickStarts[12] = {0};
uint32_t releaseStarts[12] = {0};
uint8_t handled[12] = {0};
uint8_t pressed = 0;
void keyPressHandle(uint8_t keyNumber) {
	if (handled[keyNumber-1] == 1 || pressed == 1) return;
	char buf[2];
	uint32_t time = HAL_GetTick();
	if (clickStarts[keyNumber-1] > 0) {
		if ((time - clickStarts[keyNumber-1]) > 0) {
			sprintf(buf, "%d", keyNumber);
			kbKeyExecute(buf);
			handled[keyNumber-1] = 1;
			pressed = 1;
			lastNKey = getLastNKey(keyNumber); //TODO
			clickStarts[keyNumber-1] = 0;
		}
	} else {
		clickStarts[keyNumber-1] = time;
	}
}

void keyReleaseHandle(uint8_t keyNumber) {
	uint32_t time = HAL_GetTick();
	if (releaseStarts[keyNumber-1] > 0) {
		if ((time - releaseStarts[keyNumber-1]) > 0) {
			if (handled[keyNumber-1] == 1) {
				handled[keyNumber-1] = 0;
				pressed = 0;
			}
		}
	} else {
		releaseStarts[keyNumber-1] = time;
	}
}

void kbChangeMode() {
	if (mode == 0) {
		mode = 1;
		uartTransmit((uint8_t *) "Test mode\n", 9);
	} else {
		mode = 0;
		uartTransmit((uint8_t *) "Work mode\n", 9);
	}
}
