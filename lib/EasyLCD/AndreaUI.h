/*
 * @Author: AndreaFrederica andreafrederica@outlook.com
 * @Date: 2024-07-26 18:50:12
 * @LastEditors: AndreaFrederica andreafrederica@outlook.com
 * @LastEditTime: 2024-07-30 11:26:27
 * @FilePath: \adctest\lib\EasyLCD\AndreaUI.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置
 * 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __AndreaUI_H
#define __AndreaUI_H

#include "EasyLCD.h"
#include "GUI.h"
#include "stm32f4xx_hal.h"
#include <EasyUART.h>
#include <list>
#include "FONT.H"
// TODO 横屏模式和精灵图支持没做

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
void LCD_ShowCharPlus(uint16_t x,
                      uint16_t y,
                      uint16_t fc,
                      uint16_t bc,
                      uint8_t num,
                      uint8_t size,
                      uint8_t mode);

void lcd_print_basic(const char* text);
void lcd_setString(int y,int x,uint16_t fg_color, uint16_t bg_color, const char* text);


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

//? 静态的默认调色盘 不可更改
static uint16_t DefaultPalette[16] = {
    WHITE,    BLACK,      BLUE,  BRED, GRED, RED,     GREEN, YELLOW,
    DARKBLUE, LIGHTGREEN, BROWN, GRAY, CYAN, MAGENTA, BRRED, LIGHTBLUE};

void LCD_DrawPixel(int x, int y, uint16_t pixel);

typedef enum { STATIC, DYNAMIC } SpiritType;
typedef enum { C1BIT, C16BIT } SpiritColorDeep;
class Spirit {
public:
	static bool compareByLayer(const Spirit& a, const Spirit& b);
	static bool isChanged;
	Spirit();
	void init(SpiritType type,
	          SpiritColorDeep color_deep,
	          int width,
	          int height,
	          int pos_x,
	          int pos_y,
	          int layer);
	//? layer 从0开始 越高在越上层 cli默认固定为-1
	void setStaticPixels(uint8_t* p1bit);
	void setStaticPixels(uint16_t* p16bit);
	void pixelsInit();
	void setAllChanged();
	void setPixel(int x, int y, bool pixel);
	void setPixel(int x, int y, uint16_t pixel);
	void setPalette(int key, uint16_t color);
	void drawSelf();
	void forceDrawSelf();
	void setX(int x);
	void setY(int y);
	SpiritType type;
	SpiritColorDeep color_deep;
	int width;
	int height;
	int pos_x;
	int pos_y;
	int layer;
	uint16_t palette[16];

	// bool* P_1BIT;
	uint8_t* P_1BIT;
	uint16_t* P_16BIT;
	// bool* A_changed_pixels;
	uint8_t* A_changed_pixels;

private:
	void set1Bit(int x, int y, bool key);
	bool get1Bit(int x, int y);
	void set1BitChange(int x, int y, bool key);
	bool get1BitChange(int x, int y);
};
bool IsPixelCoveredBySprite(int x, int y);
struct CharBlock {
    int x;
    int y;

    // 构造函数声明
    CharBlock(int xCoord, int yCoord);
};
// bool PixelCoveredBySprite[SCREEN_WIDTH][SCREEN_HEIGHT];//TODO 精灵绘制加速
extern std::list<Spirit> spirit_bucket;
extern std::list<CharBlock> changed_block;
void DrawSprites();

#endif