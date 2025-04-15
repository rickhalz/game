#ifndef ILI9488_H
#define ILI9488_H

#include "stm32f4xx_hal.h"
#include <stdio.h>
#include "main.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define RST_LOW() HAL_GPIO_WritePin(LCD_RESET_GPIO_Port,LCD_RESET_Pin,GPIO_PIN_RESET)
#define RST_HIGH() HAL_GPIO_WritePin(LCD_RESET_GPIO_Port,LCD_RESET_Pin,GPIO_PIN_SET)
#define CS_LOW() HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin,GPIO_PIN_RESET)
#define CS_HIGH() HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin,GPIO_PIN_SET)
#define DC_COMMAND() HAL_GPIO_WritePin(LCD_DC_GPIO_Port,LCD_DC_Pin,GPIO_PIN_RESET)
#define DC_DATA() HAL_GPIO_WritePin(LCD_DC_GPIO_Port,LCD_DC_Pin,GPIO_PIN_SET)

#define ILI9488_WIDTH 			320
#define ILI9488_HEIGHT 			480
#define ILI9488_PIXEL				153600

// Power & Sleep command
#define ILI9488_RESET				0x01
#define ILI9488_SLPOUT			0x11

// Display on/off
#define ILI9488_GAMMASET		0x26
#define ILI9488_DISOFF			0x28
#define ILI9488_DISON				0x29

// Memory access
#define ILI9488_CASET				0x2A		// X-axis range
#define ILI9488_PASET				0x2B		// Y-axis range
#define ILI9488_WR					0x2C

// Display & Orientation
#define ILI9488_MAC					0x36
#define ILI9488_PXFMT				0x3A
#define ILI9488_WDB					0x51
#define ILI9488_WCD					0x53

// Frame Rate & Interface
#define ILI9488_RGBITF			0xB0
#define ILI9488_FRC					0xB1		// Normal mode - Full color
#define ILI9488_BPC					0xB5
#define ILI9488_DFC					0xB6

// Power Control
#define ILI9488_PWCTR1  		0xC0
#define ILI9488_PWCTR2  		0xC1
#define ILI9488_PWCTR3  		0xC2
#define ILI9488_PWCTR4  		0xC3
#define ILI9488_PWCTR5  		0xC4
#define ILI9488_VMCTR1  		0xC5		// VCOM helps balance brightness and contrast

// Additional Display Tuning
#define ILI9488_PGAMCTL			0xE0
#define ILI9488_NGAMCTL			0xE1
#define ILI9341_3GAMMA_EN		0xF8

// Color definitions
#define ILI9488_BLACK      	0x0000      /*   0,   0,   0 */
#define ILI9488_NAVY				0x000F      /*   0,   0, 128 */
#define ILI9488_DARKGREEN   0x03E0      /*   0, 128,   0 */
#define ILI9488_DARKCYAN   	0x03EF      /*   0, 128, 128 */
#define ILI9488_MAROON      0x7800      /* 128,   0,   0 */
#define ILI9488_PURPLE     	0x780F      /* 128,   0, 128 */
#define ILI9488_OLIVE      	0x7BE0      /* 128, 128,   0 */
#define ILI9488_LIGHTGREY  	0xC618      /* 192, 192, 192 */
#define ILI9488_DARKGREY   	0x7BEF      /* 128, 128, 128 */
#define ILI9488_BLUE   			0x001F      /*   0,   0, 255 */
#define ILI9488_GREEN      	0x07E0      /*   0, 255,   0 */
#define ILI9488_CYAN      	0x07FF      /*   0, 255, 255 */
#define ILI9488_RED       	0xF800      /* 255,   0,   0 */
#define ILI9488_MAGENTA    	0xF81F      /* 255,   0, 255 */
#define ILI9488_YELLOW    	0xFFE0      /* 255, 255,   0 */
#define ILI9488_WHITE     	0xFFFF      /* 255, 255, 255 */
#define ILI9488_ORANGE    	0xFD20      /* 255, 165,   0 */
#define ILI9488_GREENYELLOW	0xAFE5      /* 173, 255,  47 */
#define ILI9488_PINK     		0xF81F

#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

// Initialize hardware
void ILI9488_Init();

// Write command to LCD
void ILI9488_SendCommand(uint8_t cmd);

// Write data to LCD
void ILI9488_SendData(uint8_t data);

void fillScreen(uint16_t color);
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void setRotation(uint8_t r);
void ILI9488_SendData_Multi(uint8_t *buff, size_t buff_size);

#endif
