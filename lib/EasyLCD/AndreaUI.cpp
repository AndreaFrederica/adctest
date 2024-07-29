#include <AndreaUI.h>

LCDPrintConfig lcdPrintConfig;
int last_foregroundColor;

TextCell buffer1[ROWS][COLS];
TextCell buffer2[ROWS][COLS];
TextCell (*currentBuffer)[COLS] = buffer1;
TextCell (*nextBuffer)[COLS] = buffer2;
std::list<Spirit> spirit_bucket;
bool Spirit::isChanged = true;

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

void LCD_DrawPixel(int x, int y, uint16_t pixel) {
	LCD_SetCursor(x, y); // 设置光标位置
	Lcd_WriteData_16Bit(pixel);
}

void UpdateScreen() {
	// 绘制底部CLI
	for (int row = 0; row < ROWS; row++) {
		for (int col = 0; col < COLS; col++) {
			int x = col * CHAR_WIDTH;
			int y = row * CHAR_HEIGHT;

			// 检查该像素是否被精灵遮挡
			if (IsPixelCoveredBySprite(x, y)) {
				continue;
			}

			TextCell currentCell = currentBuffer[row][col];
			TextCell nextCell = nextBuffer[row][col];

			// 仅在内容发生变化时更新屏幕
			if (currentCell.character != nextCell.character ||
			    currentCell.foregroundColor != nextCell.foregroundColor ||
			    currentCell.backgroundColor != nextCell.backgroundColor) {

				LCD_ShowCharPlus(x, y, nextCell.foregroundColor,
				             nextCell.backgroundColor, nextCell.character,
				             CHAR_HEIGHT, 0);

				// 更新当前缓冲区
				currentBuffer[row][col] = nextCell;
			}
		}
	}

	// 绘制精灵
	DrawSprites();
}

void LCD_ShowCharPlus(uint16_t x,
                      uint16_t y,
                      uint16_t fc,
                      uint16_t bc,
                      uint8_t num,
                      uint8_t size,
                      uint8_t mode) {
	uint8_t temp;
	uint8_t pos, t;
	uint16_t colortemp = POINT_COLOR;

	num = num - ' '; // 得到偏移后的值
	LCD_SetWindows(x, y, x + size / 2 - 1,
	               y + size - 1); // 设置单个文字显示窗口
	if (!mode)                    // 非叠加方式
	{
		for (pos = 0; pos < size; pos++) {
			if (size == 12)
				temp = asc2_1206[num][pos]; // 调用1206字体
			else
				temp = asc2_1608[num][pos]; // 调用1608字体
			for (t = 0; t < size / 2; t++) {
				if (temp & 0x01)
					Lcd_WriteData_16Bit(fc);
				else
					Lcd_WriteData_16Bit(bc);
				temp >>= 1;
			}
		}
	} else // 叠加方式
	{
		for (pos = 0; pos < size; pos++) {
			if (size == 12)
				temp = asc2_1206[num][pos]; // 调用1206字体
			else
				temp = asc2_1608[num][pos]; // 调用1608字体
			for (t = 0; t < size / 2; t++) {
				POINT_COLOR = fc;
				if (temp & 0x01)
					if (IsPixelCoveredBySprite(x + t, y + pos)){
						continue;
					}else{
						LCD_DrawPoint(x + t, y + pos); // 画一个点
					}
				temp >>= 1;
			}
		}
	}
	POINT_COLOR = colortemp;
	LCD_SetWindows(0, 0, lcddev.width - 1, lcddev.height - 1); // 恢复窗口为全屏
}

void DrawSprites() {
	if (Spirit::isChanged) {
		spirit_bucket.sort(Spirit::compareByLayer);
		for (auto& spirit : spirit_bucket) {
			spirit.drawSelf();
		}
	}
	Spirit::isChanged = false;
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

Spirit::Spirit() {}
void Spirit::init(SpiritType type,
                  SpiritColorDeep color_deep,
                  int width,
                  int height,
                  int pos_x,
                  int pos_y,
                  int layer) {
	this->type = type;
	this->color_deep = color_deep;
	this->width = width;
	this->height = height;
	this->pos_x = pos_x;
	this->pos_y = pos_y;
	this->layer = layer;
	std::copy(std::begin(DefaultPalette), std::end(DefaultPalette),
	          this->palette);
	uart_log_debug("spirit init done");
}
void Spirit::pixelsInit() {
	A_changed_pixels = new uint8_t[width * height / 8];
	setAllChanged();
	switch (type) {
	case DYNAMIC:
		switch (color_deep) {
		case C1BIT:
			// P_1BIT = new bool[width * height]{false};
			P_1BIT = new uint8_t[(width * height) / 8];
			P_16BIT = nullptr;
			break;
		case C16BIT:
			P_16BIT = new uint16_t[width * height];
			P_1BIT = nullptr;
		default:
			break;
		}
		break;
	// TODO 静态精灵
	default:
		break;
	}
	uart_log_debug("pix init done");
}
void Spirit::setAllChanged() {
	// for (int x = 0; x < width; x++) {
	// 	for (int y = 0; y < height; y++) {
	// 		// sprintf(buf,"x %d y%d",x,y);
	// 		set1BitChange(x, y, true);
	// 	}
	// }
	for (int pos = 0; pos < width * height / 8; pos++) {
		A_changed_pixels[pos] = 255;
	}
}
void Spirit::set1Bit(int x, int y, bool key) {
	int pos = x * (height - 1) + y;
	int bit = pos / 8;
	uint8_t bit_addr = pos % 8;
	P_1BIT[bit] |= ((int)key << bit_addr);
}

bool Spirit::get1Bit(int x, int y) {
	int pos = x * (height - 1) + y;
	int bit = pos / 8;
	uint8_t bit_addr = pos % 8;
	return P_1BIT[bit] & (1 << bit_addr);
}

void Spirit::set1BitChange(int x, int y, bool key) {
	int pos = x * (height - 1) + y;
	int bit = pos / 8;
	uint8_t bit_addr = pos % 8;
	A_changed_pixels[bit] |= ((int)key << bit_addr);
}

bool Spirit::get1BitChange(int x, int y) {
	int pos = x * (height - 1) + y;
	int bit = pos / 8;
	uint8_t bit_addr = pos % 8;
	return A_changed_pixels[bit] & (1 << bit_addr);
}

void Spirit::setPixel(int x, int y, bool pixel) {
	//! 坐标计算方式
	// int pos = x * (height - 1) + y;
	//  P_1BIT[pos] = pixel;
	set1Bit(x, y, pixel);
	// A_changed_pixels[pos] = true;
	set1BitChange(x, y, true);
	isChanged = true;
}

void Spirit::setPixel(int x, int y, uint16_t pixel) {
	int pos = x * (height - 1) + y;
	P_16BIT[pos] = pixel;
	// A_changed_pixels[pos] = true;
	set1BitChange(x, y, true);
	isChanged = true;
}

bool Spirit::compareByLayer(const Spirit& a, const Spirit& b) {
	return a.layer < b.layer;
}

void Spirit::setPalette(int key, uint16_t color) { palette[key] = color; }

void Spirit::drawSelf() {
	// uart_log_debug("0000");
	int x, y;
	if (color_deep == C1BIT) {
		int len = height * width;
		int pos;
		for (int y = 0; y < height; y++) {
			// uart_log_debug("11111");
			for (int x = 0; x < width; x++) {
				pos = x * (height - 1) + y;
				if (get1BitChange(x, y)) {
					LCD_DrawPixel(pos_x + x, pos_y + y,
					              palette[(int)get1Bit(x, y)]);
					set1BitChange(x, y, false);
				}
			}
		}
	} else if (color_deep == C16BIT) {
		int len = height * width;
		int pos;
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				pos = x * (height - 1) + y;
				if (get1BitChange(x, y)) {
					//? 16Bit直接写入保存的颜色
					LCD_DrawPixel(pos_x + x, pos_y + y, P_16BIT[pos]);
					set1BitChange(x, y, false);
				}
			}
		}
	}
}

void Spirit::setStaticPixels(uint8_t* p1bit) {
	uart_log_debug("setOUT");
	isChanged = true;
	P_1BIT = p1bit;
	P_16BIT = nullptr;
	// std::fill(A_changed_pixels, A_changed_pixels + width * height, true);
	// for (int i = 0; i < width * height / 8; i++) {
	// 	this->A_changed_pixels[i] = true;
	// 	uart_log_debug("setOUTx");

	// }
	setAllChanged();
}

void Spirit::setStaticPixels(uint16_t* p16bit) {
	isChanged = true;
	P_16BIT = p16bit;
	P_1BIT = nullptr;
	// std::fill(A_changed_pixels, A_changed_pixels + width * height, true);
	// for (int i = 0; i < width * height; i++) {
	// 	this->A_changed_pixels[i] = true;
	// }
	setAllChanged();
}

bool IsPixelCoveredBySprite(int x, int y) {
	for (auto& spirit : spirit_bucket) {
		if (x >= spirit.pos_x && x < (spirit.pos_x + spirit.width) &&
		    y >= spirit.pos_y && y < (spirit.pos_y + spirit.height)) {
			return true;
		}
	}
	return false;
}