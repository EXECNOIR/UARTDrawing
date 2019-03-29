#include "stm32f7xx_hal.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include <stdlib.h>
#include <string.h>

#define PRINT_UART int __io_putchar(int ch)

static void SystemClock_Config(void);
static void Error_Handler(void);
static void MPU_Config(void);
static void CPU_CACHE_Enable(void);

#define LCD_FRAME_BUFFER SDRAM_DEVICE_ADDR
#define RGB565_BYTE_PER_PIXEL 4
#define ARBG8888_BYTE_PER_PIXEL 2

static void Init_UART(void);

UART_HandleTypeDef uart_handle;
TS_StateTypeDef ts_state;

typedef enum state {

	START, THICKNESS, COLOR, ERASER, FEATURE

} state_t;

void drawUI();
void thicknessUI();
void colorUI();
void clearColorUI();

void fingerDrawing();
void sendX();
void sendY();
void sendBoth();

void clearEraser();

int thickness = 4;
int previousThickness = 4;

int eraserState = 0;

uint32_t color = LCD_COLOR_WHITE;
uint32_t previousColor = LCD_COLOR_WHITE;

int main(void) {

	MPU_Config();
	CPU_CACHE_Enable();
	HAL_Init();
	SystemClock_Config();

	BSP_LCD_Init();
	BSP_TS_Init(BSP_LCD_GetXSize, BSP_LCD_GetYSize);
	Init_UART();

	BSP_LCD_LayerDefaultInit(1, LCD_FRAME_BUFFER);
	BSP_LCD_SelectLayer(1);
	BSP_LCD_SetLayerVisible(1, ENABLE);
	BSP_LCD_Clear(LCD_COLOR_BLACK);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
	BSP_LCD_SetFont(&LCD_DEFAULT_FONT);
	BSP_LCD_DisplayOn();

	state_t state = START;
	drawUI();

	while (1) {
		BSP_TS_GetState(&ts_state);

		if (ts_state.touchDetected && ts_state.touchY[0] < 220
				&& ts_state.touchX[0] > 3 && ts_state.touchX[0] < 477
				&& ts_state.touchY[0] > 3) {
			sendBoth();
			fingerDrawing();
		}

		if (!eraserState && state != THICKNESS && state != COLOR
				&& ts_state.touchDetected && ts_state.touchY[0] > 225
				&& ts_state.touchX[0] < 96) {
			sendBoth();
			previousColor = color;
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			drawUI();
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			thicknessUI();
			HAL_Delay(250);
			ts_state.touchX[0] = 1;
			ts_state.touchY[0] = 1;
			state = THICKNESS;

		} else if (!eraserState && state != THICKNESS && state != COLOR
				&& ts_state.touchDetected && ts_state.touchY[0] > 225
				&& ts_state.touchX[0] > 96 && ts_state.touchX[0] < 192) {
			sendBoth();
			BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
			drawUI();
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			colorUI();
			HAL_Delay(250);
			ts_state.touchX[0] = 1;
			ts_state.touchY[0] = 1;
			state = COLOR;
			HAL_Delay(250);

		} else if (state != THICKNESS && state != COLOR
				&& ts_state.touchDetected && ts_state.touchY[0] > 225
				&& ts_state.touchX[0] > 192 && ts_state.touchX[0] < 288) {
			sendBoth();

			if (!eraserState) {
				sendBoth();
				state = ERASER;
				previousColor = color;

				BSP_LCD_SetTextColor(LCD_COLOR_GRAY);
				BSP_LCD_FillRect(195, 228, 93, 39);
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

				char text[10];
				sprintf(text, "ERASER");
				BSP_LCD_DisplayStringAt(220, 242, (uint8_t *) text, LEFT_MODE);
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);

				eraserState = 1;
				previousThickness = thickness;

				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				thickness = 10;
				HAL_Delay(250);
			} else {
				sendBoth();
				clearEraser();
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				drawUI();
				color = previousColor;
				BSP_LCD_SetTextColor(color);
				HAL_Delay(250);
			}

		} else if (!eraserState && state != THICKNESS && state != COLOR
				&& ts_state.touchDetected && ts_state.touchY[0] > 225
				&& ts_state.touchX[0] > 288 && ts_state.touchX[0] < 384) {
			sendBoth();
			state = FEATURE;
		} else if (!eraserState && state != THICKNESS && state != COLOR
				&& ts_state.touchDetected && ts_state.touchY[0] > 225
				&& ts_state.touchX[0] > 384) {
			sendBoth();
			previousColor = color;
			previousThickness = thickness;

			BSP_LCD_Clear(LCD_COLOR_BLACK);
			drawUI();
			BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
			drawUI();

			color = previousColor;
			BSP_LCD_SetTextColor(color);
			thickness = previousThickness;

			HAL_Delay(250);
		}

		switch (state) {

		case START:

			break;

		case THICKNESS:
			if (ts_state.touchDetected && ts_state.touchY[0] > 225
					&& ts_state.touchX[0] < 96) {
				sendBoth();
				thickness = 2;
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				thicknessUI();
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				drawUI();
				color = previousColor;
				BSP_LCD_SetTextColor(color);
				HAL_Delay(250);
				state = START;

			} else if (ts_state.touchDetected && ts_state.touchY[0] > 225
					&& ts_state.touchX[0] > 96 && ts_state.touchX[0] < 192) {
				sendBoth();
				thickness = 4;
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				thicknessUI();
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				drawUI();
				color = previousColor;
				BSP_LCD_SetTextColor(color);
				HAL_Delay(250);
				state = START;

			} else if (ts_state.touchDetected && ts_state.touchY[0] > 225
					&& ts_state.touchX[0] > 192 && ts_state.touchX[0] < 288) {
				sendBoth();
				thickness = 6;
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				thicknessUI();
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				drawUI();
				color = previousColor;
				BSP_LCD_SetTextColor(color);
				HAL_Delay(250);
				state = START;

			} else if (ts_state.touchDetected && ts_state.touchY[0] > 225
					&& ts_state.touchX[0] > 288 && ts_state.touchX[0] < 384) {
				sendBoth();
				thickness = 8;
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				thicknessUI();
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				drawUI();
				color = previousColor;
				BSP_LCD_SetTextColor(color);
				HAL_Delay(250);
				state = START;

			} else if (ts_state.touchDetected && ts_state.touchY[0] > 225
					&& ts_state.touchX[0] > 384) {
				sendBoth();
				thickness = 10;
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				thicknessUI();
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				drawUI();
				color = previousColor;
				BSP_LCD_SetTextColor(color);
				HAL_Delay(250);
				state = START;
			}

			break;
		case COLOR:
			if (ts_state.touchDetected && ts_state.touchY[0] > 225
					&& ts_state.touchX[0] < 96) {
				sendBoth();
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				clearColorUI();
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				drawUI();
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				color = LCD_COLOR_WHITE;
				HAL_Delay(250);
				state = START;

			} else if (ts_state.touchDetected && ts_state.touchY[0] > 225
					&& ts_state.touchX[0] > 96 && ts_state.touchX[0] < 192) {
				sendBoth();
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				clearColorUI();
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				drawUI();
				BSP_LCD_SetTextColor(LCD_COLOR_RED);
				color = LCD_COLOR_RED;
				HAL_Delay(250);
				state = START;

			} else if (ts_state.touchDetected && ts_state.touchY[0] > 225
					&& ts_state.touchX[0] > 192 && ts_state.touchX[0] < 288) {
				sendBoth();
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				clearColorUI();
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				drawUI();
				BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
				color = LCD_COLOR_BLUE;
				HAL_Delay(250);
				state = START;

			} else if (ts_state.touchDetected && ts_state.touchY[0] > 225
					&& ts_state.touchX[0] > 288 && ts_state.touchX[0] < 384) {
				sendBoth();
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				clearColorUI();
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				drawUI();
				BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
				color = LCD_COLOR_GREEN;
				HAL_Delay(250);
				state = START;

			} else if (ts_state.touchDetected && ts_state.touchY[0] > 225
					&& ts_state.touchX[0] > 384) {
				sendBoth();
				BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
				clearColorUI();
				BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
				drawUI();
				BSP_LCD_SetTextColor(LCD_COLOR_MAGENTA);
				color = LCD_COLOR_MAGENTA;
				HAL_Delay(250);
				state = START;
			}
			break;
		case ERASER:
			break;
		case FEATURE:
			break;
		}

	}
}

static void SystemClock_Config(void) {
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_OscInitTypeDef RCC_OscInitStruct;

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 25;
	RCC_OscInitStruct.PLL.PLLN = 432;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK) {
		Error_Handler();
	}
}

static void Error_Handler(void) {
	while (1) {
	}
}

static void MPU_Config(void) {
	MPU_Region_InitTypeDef MPU_InitStruct;

	HAL_MPU_Disable();

	MPU_InitStruct.Enable = MPU_REGION_ENABLE;
	MPU_InitStruct.BaseAddress = 0x20010000;
	MPU_InitStruct.Size = MPU_REGION_SIZE_256KB;
	MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
	MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
	MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
	MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
	MPU_InitStruct.Number = MPU_REGION_NUMBER0;
	MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
	MPU_InitStruct.SubRegionDisable = 0x00;
	MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;

	HAL_MPU_ConfigRegion(&MPU_InitStruct);

	HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

static void CPU_CACHE_Enable(void) {
	/* Enable I-Cache */
	SCB_EnableICache();

	/* Enable D-Cache */
	SCB_EnableDCache();
}

static void Init_UART(void) {

	uart_handle.Instance = USART1;
	uart_handle.Init.BaudRate = 115200;
	uart_handle.Init.WordLength = UART_WORDLENGTH_8B;
	uart_handle.Init.StopBits = UART_STOPBITS_1;
	uart_handle.Init.Parity = UART_PARITY_NONE;
	uart_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart_handle.Init.Mode = UART_MODE_TX_RX;

	BSP_COM_Init(COM1, &uart_handle);
}

PRINT_UART {
	HAL_UART_Transmit(&uart_handle, (uint8_t*) &ch, 1, 0xFFFF);
	return ch;
}

void drawUI() {

	BSP_LCD_FillRect(0, 0, 3, 225);
	BSP_LCD_FillRect(477, 0, 3, 225);
	BSP_LCD_FillRect(0, 0, 480, 3);

	BSP_LCD_FillRect(0, 225, 480, 3);
	BSP_LCD_FillRect(0, 267, 480, 3);

	BSP_LCD_FillRect(0, 225, 3, 45);
	BSP_LCD_FillRect(96, 225, 3, 45);
	BSP_LCD_FillRect(192, 225, 3, 45);
	BSP_LCD_FillRect(288, 225, 3, 45);
	BSP_LCD_FillRect(384, 225, 3, 45);
	BSP_LCD_FillRect(477, 225, 3, 45);

	char text[10];
	sprintf(text, "THICKNESS");
	BSP_LCD_DisplayStringAt(17, 242, (uint8_t *) text, LEFT_MODE);

	sprintf(text, "COLOR");
	BSP_LCD_DisplayStringAt(127, 242, (uint8_t *) text, LEFT_MODE);

	sprintf(text, "ERASER");
	BSP_LCD_DisplayStringAt(220, 242, (uint8_t *) text, LEFT_MODE);

	sprintf(text, "FEATURE");
	BSP_LCD_DisplayStringAt(314, 242, (uint8_t *) text, LEFT_MODE);

	sprintf(text, "CLEAR");
	BSP_LCD_DisplayStringAt(415, 242, (uint8_t *) text, LEFT_MODE);
}

void thicknessUI() {

	BSP_LCD_FillRect(0, 0, 3, 225);
	BSP_LCD_FillRect(477, 0, 3, 225);
	BSP_LCD_FillRect(0, 0, 480, 3);

	BSP_LCD_FillRect(0, 225, 480, 3);
	BSP_LCD_FillRect(0, 267, 480, 3);

	BSP_LCD_FillRect(0, 225, 3, 45);
	BSP_LCD_FillRect(96, 225, 3, 45);
	BSP_LCD_FillRect(192, 225, 3, 45);
	BSP_LCD_FillRect(288, 225, 3, 45);
	BSP_LCD_FillRect(384, 225, 3, 45);
	BSP_LCD_FillRect(477, 225, 3, 45);

	BSP_LCD_FillCircle(50, 246, 2);
	BSP_LCD_FillCircle(145, 246, 4);
	BSP_LCD_FillCircle(240, 246, 6);
	BSP_LCD_FillCircle(340, 246, 8);
	BSP_LCD_FillCircle(430, 246, 10);
}

void colorUI() {

	BSP_LCD_FillRect(0, 0, 3, 225);
	BSP_LCD_FillRect(477, 0, 3, 225);
	BSP_LCD_FillRect(0, 0, 480, 3);

	BSP_LCD_FillRect(0, 225, 480, 3);
	BSP_LCD_FillRect(0, 267, 480, 3);

	BSP_LCD_FillRect(0, 225, 3, 45);
	BSP_LCD_FillRect(96, 225, 3, 45);
	BSP_LCD_FillRect(192, 225, 3, 45);
	BSP_LCD_FillRect(288, 225, 3, 45);
	BSP_LCD_FillRect(384, 225, 3, 45);
	BSP_LCD_FillRect(477, 225, 3, 45);

	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	BSP_LCD_FillRect(3, 228, 93, 39);
	BSP_LCD_SetTextColor(LCD_COLOR_RED);
	BSP_LCD_FillRect(99, 228, 93, 39);
	BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
	BSP_LCD_FillRect(195, 228, 93, 39);
	BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
	BSP_LCD_FillRect(291, 228, 93, 39);
	BSP_LCD_SetTextColor(LCD_COLOR_MAGENTA);
	BSP_LCD_FillRect(387, 228, 93, 39);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
}

void clearColorUI() {
	BSP_LCD_FillRect(3, 228, 93, 39);
	BSP_LCD_FillRect(99, 228, 93, 39);
	BSP_LCD_FillRect(195, 228, 93, 39);
	BSP_LCD_FillRect(291, 228, 93, 39);
	BSP_LCD_FillRect(387, 228, 93, 39);
}

void fingerDrawing() {
	if (ts_state.touchDetected && ts_state.touchX[0] > 10
			&& ts_state.touchY[0] > 10 && ts_state.touchX[0] < 470
			&& ts_state.touchY[0] < 262) {
		BSP_LCD_FillCircle(ts_state.touchX[0], ts_state.touchY[0], thickness);
	}
}

void sendX() {

	int size = 5;
	char infoX[size];

	if (ts_state.touchX[0] < 10) {
		sprintf(infoX, "xQQ%dE", ts_state.touchX[0]);
	} else if (ts_state.touchX[0] > 9 && ts_state.touchX[0] < 100) {
		sprintf(infoX, "xQ%dE", ts_state.touchX[0]);
	} else {
		sprintf(infoX, "x%dE", ts_state.touchX[0]);
	}

	HAL_UART_Transmit(&uart_handle, infoX, (uint8_t*) size, 0xFFFF);
}

void sendY() {

	int size = 5;
	char infoY[size];

	if (ts_state.touchY[0] < 10) {
		sprintf(infoY, "yQQ%dE", ts_state.touchY[0]);
	} else if (ts_state.touchY[0] > 9 && ts_state.touchY[0] < 100) {
		sprintf(infoY, "yQ%dE", ts_state.touchY[0]);
	} else {
		sprintf(infoY, "y%dE", ts_state.touchY[0]);
	}

	HAL_UART_Transmit(&uart_handle, infoY, (uint8_t*) size, 0xFFFF);
}

void sendBoth() {
	sendX();
	HAL_Delay(5);
	sendY();
	HAL_Delay(5);
}

void clearEraser() {
	BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
	BSP_LCD_FillRect(195, 228, 93, 39);
	BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
	eraserState = 0;
	thickness = previousThickness;
	color = previousColor;
	char text[10];
	sprintf(text, "ERASER");
	BSP_LCD_DisplayStringAt(220, 242, (uint8_t *) text, LEFT_MODE);
}
