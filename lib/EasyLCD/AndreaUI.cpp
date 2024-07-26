#include <AndreaUI.h>

LCDPrintConfig lcdPrintConfig;
int last_foregroundColor;

TextCell buffer1[ROWS][COLS];
TextCell buffer2[ROWS][COLS];
TextCell (*currentBuffer)[COLS] = buffer1;
TextCell (*nextBuffer)[COLS] = buffer2;

void InitializeBuffer(TextCell buffer[ROWS][COLS],
                      char character,
                      uint16_t fgColor,
                      uint16_t bgColor) {
	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLS; col++) {
			buffer[row][col].character = character;
			buffer[row][col].foregroundColor = fgColor;
			buffer[row][col].backgroundColor = bgColor;
		}
	}
}

void SwapBuffers() {
	TextCell(*temp)[COLS] = currentBuffer;
	currentBuffer = nextBuffer;
	nextBuffer = temp;
}

void SetChar(
    int row, int col, char character, uint16_t fgColor, uint16_t bgColor) {
	if (row >= 0 && row < ROWS && col >= 0 && col < COLS) {
		nextBuffer[row][col].character = character;
		nextBuffer[row][col].foregroundColor = fgColor;
		nextBuffer[row][col].backgroundColor = bgColor;
	}
}

void UpdateScreen() {
	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLS; col++) {
			TextCell currentCell = currentBuffer[row][col];
			TextCell nextCell = nextBuffer[row][col];

			// 仅在内容发生变化时更新屏幕
			if (currentCell.character != nextCell.character ||
			    currentCell.foregroundColor != nextCell.foregroundColor ||
			    currentCell.backgroundColor != nextCell.backgroundColor) {

				LCD_ShowChar(col * CHAR_WIDTH, row * CHAR_HEIGHT,
				             nextCell.foregroundColor, nextCell.backgroundColor,
				             nextCell.character, CHAR_HEIGHT, 0);

				// 更新当前缓冲区
				currentBuffer[row][col] = nextCell;
			}
		}
	}
}

void DisplayAlphabet() {
	uint16_t x = 0;        // 起始 x 坐标
	uint16_t y = 0;        // 起始 y 坐标
	uint16_t fc = WHITE;   // 前景色（白色）
	uint16_t bc = BLACK;   // 背景色（黑色）
	uint8_t size = 16;     // 字符大小
	uint8_t mode = 0;      // 非叠加模式
	bool uppercase = true; // 是否显示大写字母

	for (int row = 0; row < ROWS; row++) {
		char c = uppercase ? 'A' : 'a'; // 根据当前行选择大小写
		for (int col = 0; col < 26; col++) {
			SetChar(row, col, c, fc, bc);
			c++;
			if (c > (uppercase ? 'Z' : 'z'))
				c = uppercase ? 'A' : 'a'; // 循环显示字母
		}
		uppercase = !uppercase; // 切换大小写
	}

	// 切换缓冲区并更新屏幕
	// SwapBuffers();
	UpdateScreen();
}

void lcd_print_init(uint16_t fgColor,
                    uint16_t bgColor,
                    EndOfScreenBehavior behavior) {
	lcdPrintConfig.cursorX = 0;
	lcdPrintConfig.cursorY = 0;
	lcdPrintConfig.foregroundColor = fgColor;
	lcdPrintConfig.backgroundColor = bgColor;
	lcdPrintConfig.endOfScreenBehavior = behavior;
}

void lcdClearLen(int row) {
	for (int col = 0; col < COLS; col++) {
		// currentBuffer[rows][col].character = ' ';
		// currentBuffer[rows][col].foregroundColor =
		//     lcdPrintConfig.foregroundColor;
		// currentBuffer[rows][col].backgroundColor =
		//     lcdPrintConfig.backgroundColor;
		SetChar(row, col, ' ', lcdPrintConfig.foregroundColor,
		        lcdPrintConfig.backgroundColor);
	}
	// UpdateScreen();
}

void ScrollUp() {
	for (int row = 1; row < ROWS; row++) {
		for (int col = 0; col < COLS; col++) {
			nextBuffer[row - 1][col] = nextBuffer[row][col];
		}
	}
	// 清空最后一行
	lcdClearLen(ROWS - 1);
	UpdateScreen();
}

void lcd_print_basic(const char* text) {
	while (*text) {
		if (*text == '\n') {
			lcdPrintConfig.cursorX = 0;
			lcdPrintConfig.cursorY += CHAR_HEIGHT;
			if (lcdPrintConfig.cursorY >= SCREEN_HEIGHT) {
				if (lcdPrintConfig.endOfScreenBehavior == SCROLL_UP) {
					ScrollUp();
					lcdPrintConfig.cursorY -= CHAR_HEIGHT;
				} else {
					lcdClearLen(0);
					lcdPrintConfig.cursorX = 0;
					lcdPrintConfig.cursorY = 0;
				}
			}
		} else if (*text == '\r') {
			lcdPrintConfig.cursorX = 0;
		} else {
			SetChar(lcdPrintConfig.cursorY / CHAR_HEIGHT,
			        lcdPrintConfig.cursorX / CHAR_WIDTH, *text,
			        lcdPrintConfig.foregroundColor,
			        lcdPrintConfig.backgroundColor);
			lcdPrintConfig.cursorX += CHAR_WIDTH;
			if (lcdPrintConfig.cursorX >= SCREEN_WIDTH) {
				lcdPrintConfig.cursorX = 0;
				if (lcdPrintConfig.cursorY / CHAR_HEIGHT <= ROWS - 1) {
					lcdClearLen((lcdPrintConfig.cursorY / CHAR_HEIGHT) + 1);
				}
				lcdPrintConfig.cursorY += CHAR_HEIGHT;
				if (lcdPrintConfig.cursorY >= SCREEN_HEIGHT) {
					if (lcdPrintConfig.endOfScreenBehavior == SCROLL_UP) {
						ScrollUp();
						lcdPrintConfig.cursorY -= CHAR_HEIGHT;
					} else {
						lcdClearLen(0);
						lcdPrintConfig.cursorX = 0;
						lcdPrintConfig.cursorY = 0;
					}
				}
			}
		}
		text++;
	}
	// SwapBuffers();
	UpdateScreen();
}

void lcd_print(const char* text, const char* end) {
	lcd_print_basic(text);
	lcd_print_basic(end);
}

void lcd_log_trace(const char* message, const char* end) {
	last_foregroundColor = lcdPrintConfig.foregroundColor;
	lcdPrintConfig.foregroundColor = GRAY;
	lcd_print_basic("Trace  | ");
	lcd_print_basic(message);
	lcd_print_basic(end);
	lcdPrintConfig.foregroundColor = last_foregroundColor;
}

void lcd_log_debug(const char* message, const char* end) {
	last_foregroundColor = lcdPrintConfig.foregroundColor;
	lcdPrintConfig.foregroundColor = BLUE;
	lcd_print_basic("Debug  | ");
	lcd_print_basic(message);
	lcd_print_basic(end);
	lcdPrintConfig.foregroundColor = last_foregroundColor;
}

void lcd_log_info(const char* message, const char* end) {
	last_foregroundColor = lcdPrintConfig.foregroundColor;
	lcdPrintConfig.foregroundColor = WHITE;
	lcd_print_basic("Info   | ");
	lcd_print_basic(message);
	lcd_print_basic(end);
	lcdPrintConfig.foregroundColor = last_foregroundColor;
}

void lcd_log_warn(const char* message, const char* end) {
	last_foregroundColor = lcdPrintConfig.foregroundColor;
	lcdPrintConfig.foregroundColor = BROWN;
	lcd_print_basic("Warn   | ");
	lcd_print_basic(message);
	lcd_print_basic(end);
	lcdPrintConfig.foregroundColor = last_foregroundColor;
}

void lcd_log_error(const char* message, const char* end) {
	last_foregroundColor = lcdPrintConfig.foregroundColor;
	lcdPrintConfig.foregroundColor = RED;
	lcd_print_basic("Error  | ");
	lcd_print_basic(message);
	lcd_print_basic(end);
	lcdPrintConfig.foregroundColor = last_foregroundColor;
}

void lcd_log_success(const char* message, const char* end) {
	last_foregroundColor = lcdPrintConfig.foregroundColor;
	lcdPrintConfig.foregroundColor = GREEN;
	lcd_print_basic("Success| ");
	lcd_print_basic(message);
	lcd_print_basic(end);
	lcdPrintConfig.foregroundColor = last_foregroundColor;
}