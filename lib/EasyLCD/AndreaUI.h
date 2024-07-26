#ifndef __AndreaUI_H
#define __AndreaUI_H

#include "EasyLCD.h"
#include "GUI.h"
#include "stm32f4xx_hal.h"
//TODO 横屏模式和精灵图支持没做

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 480
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 16
#define ROWS (SCREEN_HEIGHT / CHAR_HEIGHT)
#define COLS (SCREEN_WIDTH / CHAR_WIDTH)

typedef struct {
	char character;
	uint16_t foregroundColor;
	uint16_t backgroundColor;
} TextCell;

typedef enum {
	SCROLL_UP, //! 很慢
	RETURN_TO_ORIGIN
} EndOfScreenBehavior;

typedef struct {
	uint16_t cursorX;
	uint16_t cursorY;
	uint16_t foregroundColor;
	uint16_t backgroundColor;
	EndOfScreenBehavior endOfScreenBehavior;
} LCDPrintConfig;

extern LCDPrintConfig lcdPrintConfig;
extern int last_foregroundColor;

extern TextCell buffer1[ROWS][COLS];
extern TextCell buffer2[ROWS][COLS];

void lcd_print_init(uint16_t fgColor,
                    uint16_t bgColor,
                    EndOfScreenBehavior behavior);

void lcdClearLen(int row);

void ScrollUp();

void lcd_print_basic(const char* text);

void lcd_print(const char* text, const char* end = "\r\n");
void lcd_log_trace(const char* message, const char* end = "\r\n");
void lcd_log_debug(const char* message, const char* end = "\r\n");
void lcd_log_info(const char* message, const char* end = "\r\n");
void lcd_log_warn(const char* message, const char* end = "\r\n");
void lcd_log_error(const char* message, const char* end = "\r\n");
void lcd_log_success(const char* message, const char* end = "\r\n");

void InitializeBuffer(TextCell buffer[ROWS][COLS],
                      char character,
                      uint16_t fgColor,
                      uint16_t bgColor);

void SwapBuffers();

void SetChar(
    int row, int col, char character, uint16_t fgColor, uint16_t bgColor);

void UpdateScreen();

void DisplayAlphabet();

#endif