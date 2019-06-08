#include "hw_control.h"
#include "khh_lcd.h"

/* Private */
void TM_HD44780_InitPins(void);
void TM_HD44780_Cmd(uint8_t cmd);
void TM_HD44780_Cmd4bit(uint8_t cmd);
void TM_HD44780_Data(uint8_t data);
void TM_HD44780_CursorSet(uint8_t col, uint8_t row);


static void LCD_GPIO_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	//SPI_InitTypeDef sSPI;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);

	//LCD//>º¯
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15 ;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);	

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 ;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOA, GPIO_Pin_15);	
}

void LCD_init(void)
{
	uint8_t customChar0[] = {
		0x1F,    // xxx 11111
		0x11,    // xxx 10001
		0x11,    // xxx 10001
		0x11,    // xxx 10001
		0x11,    // xxx 10001
		0x11,    // xxx 10001
		0x11,    // xxx 10001
		0x1F    // xxx 11111
	};

	uint8_t customChar1[] = {
		0x1F,    // xxx 11111
		0x1F,    // xxx 10001
		0x1F,    // xxx 10001
		0x1F,    // xxx 10001
		0x1F,    // xxx 10001
		0x1F,    // xxx 10001
		0x1F,    // xxx 10001
		0x1F    // xxx 11111
	};

	TM_HD44780_Init(16, 2);
	
    TM_HD44780_CreateChar(0, &customChar0[0]);
	TM_HD44780_CreateChar(1, &customChar1[0]);
//	TM_HD44780_PutCustom(0, 1, 0);
//	TM_HD44780_PutCustom(1, 1, 1);
//	TM_HD44780_Puts(0, 0, "1234567891234567");
//	TM_HD44780_CursorSet(0, 0);
//	TM_HD44780_CursorOn();
}


typedef struct {
	uint8_t DisplayControl;
	uint8_t DisplayFunction;
	uint8_t DisplayMode;
	uint8_t Rows;
	uint8_t Cols;
	uint8_t currentX;
	uint8_t currentY;
} TM_HD44780_Options_t;

TM_HD44780_Options_t TM_HD44780_Opts;

void TM_HD44780_Init(uint8_t cols, uint8_t rows) {
	/* Init pinout */
	TM_HD44780_InitPins();
	/* At least 20ms */
	TM_HD44780_Delay(100000);
	
	TM_HD44780_Opts.Rows = rows;
	TM_HD44780_Opts.Cols = cols;
	
	TM_HD44780_Opts.currentX = 0;
	TM_HD44780_Opts.currentY = 0;
	
	TM_HD44780_Opts.DisplayFunction = TM_HD44780_4BITMODE | TM_HD44780_5x8DOTS | TM_HD44780_1LINE;
	if (rows > 1) {
		TM_HD44780_Opts.DisplayFunction |= TM_HD44780_2LINE;
	}
	
	/* Try to set 4bit mode */
	TM_HD44780_Cmd4bit(0x03);
	TM_HD44780_Delay(4500);
	
	/* Second try */
	TM_HD44780_Cmd4bit(0x03);
	TM_HD44780_Delay(4500);
	
	/* Third goo! */
	TM_HD44780_Cmd4bit(0x03);
	TM_HD44780_Delay(4500);	
	
	/* Set 4-bit interface */
	TM_HD44780_Cmd4bit(0x02);
	TM_HD44780_Delay(100);
	
	/* Set # lines, font size, etc. */
	TM_HD44780_Cmd(TM_HD44780_FUNCTIONSET | TM_HD44780_Opts.DisplayFunction);
	TM_HD44780_Delay(100);
	
	//TM_HD44780_Cmd(0x28);
	/* Turn the display on with no cursor or blinking default */
	TM_HD44780_Opts.DisplayControl = TM_HD44780_DISPLAYON;
	TM_HD44780_DisplayOn();
	TM_HD44780_Delay(100);
	
	/* Clear lcd */
	TM_HD44780_Clear();
	TM_HD44780_Delay(100);

	/* Default font directions */
	TM_HD44780_Opts.DisplayMode = TM_HD44780_ENTRYLEFT | TM_HD44780_ENTRYSHIFTDECREMENT;
	TM_HD44780_Cmd(TM_HD44780_ENTRYMODESET | TM_HD44780_Opts.DisplayMode);
		
	/* Delay */
	TM_HD44780_Delay(10000);

}

void TM_HD44780_InitPins(void) {
	/* Init all pins */
	//TM_GPIO_Init(TM_HD44780_RS_PORT, TM_HD44780_RS_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Low);
	//TM_GPIO_Init(TM_HD44780_E_PORT, TM_HD44780_E_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Low);
	//TM_GPIO_Init(TM_HD44780_D4_PORT, TM_HD44780_D4_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Low);
	//TM_GPIO_Init(TM_HD44780_D5_PORT, TM_HD44780_D5_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Low);
	//TM_GPIO_Init(TM_HD44780_D6_PORT, TM_HD44780_D6_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Low);
	//TM_GPIO_Init(TM_HD44780_D7_PORT, TM_HD44780_D7_PIN, TM_GPIO_Mode_OUT, TM_GPIO_OType_PP, TM_GPIO_PuPd_NOPULL, TM_GPIO_Speed_Low);
	LCD_GPIO_init();
	
	/* Set pins low */
	GPIO_ResetBits(TM_HD44780_RS_PORT, TM_HD44780_RS_PIN);
	GPIO_ResetBits(TM_HD44780_E_PORT,  TM_HD44780_E_PIN);
	GPIO_ResetBits(TM_HD44780_D4_PORT, TM_HD44780_D4_PIN);
	GPIO_ResetBits(TM_HD44780_D5_PORT, TM_HD44780_D5_PIN);
	GPIO_ResetBits(TM_HD44780_D6_PORT, TM_HD44780_D6_PIN);
	GPIO_ResetBits(TM_HD44780_D7_PORT, TM_HD44780_D7_PIN);
}

void TM_HD44780_Clear(void) {
	TM_HD44780_Cmd(TM_HD44780_CLEARDISPLAY);
	TM_HD44780_Delay(3000);
}

void TM_HD44780_Cmd(uint8_t cmd) {
	/* Command mode */
	TM_HD44780_RS_LOW;
	
	/* High nibble */
	TM_HD44780_Cmd4bit(cmd >> 4);
	/* Low nibble */
	TM_HD44780_Cmd4bit(cmd & 0x0F);
}

void TM_HD44780_Data(uint8_t data) {
	/* Data mode */
	TM_HD44780_RS_HIGH;
	
	/* High nibble */
	TM_HD44780_Cmd4bit(data >> 4);
	/* Low nibble */
	TM_HD44780_Cmd4bit(data & 0x0F);
}

void TM_HD44780_Cmd4bit(uint8_t cmd) {
	/* Set output port */
	//uint32_t c = (uint32_t)cmd & 0x000f;
	//GPIO_Write(GPIOE, c);
	
	if((cmd & 0x08) == 0x08) GPIO_WriteBit(TM_HD44780_D7_PORT, TM_HD44780_D7_PIN, Bit_SET); else GPIO_WriteBit(TM_HD44780_D7_PORT, TM_HD44780_D7_PIN, Bit_RESET);
	if((cmd & 0x04) == 0x04) GPIO_WriteBit(TM_HD44780_D6_PORT, TM_HD44780_D6_PIN, Bit_SET); else GPIO_WriteBit(TM_HD44780_D6_PORT, TM_HD44780_D6_PIN, Bit_RESET);
	if((cmd & 0x02) == 0x02) GPIO_WriteBit(TM_HD44780_D5_PORT, TM_HD44780_D5_PIN, Bit_SET); else GPIO_WriteBit(TM_HD44780_D5_PORT, TM_HD44780_D5_PIN, Bit_RESET);
	if((cmd & 0x01) == 0x01) GPIO_WriteBit(TM_HD44780_D4_PORT, TM_HD44780_D4_PIN, Bit_SET); else GPIO_WriteBit(TM_HD44780_D4_PORT, TM_HD44780_D4_PIN, Bit_RESET);
	
	TM_HD44780_E_BLINK;
}

void TM_HD44780_CursorSet(uint8_t col, uint8_t row) {
	uint8_t row_offsets[] = {0x00, 0x40, 0x14, 0x54};
	
	/* Go to the beginning */
	if (row >= TM_HD44780_Opts.Rows) {
		row = 0;
	}
	
	/* Save colums and row */
	TM_HD44780_Opts.currentX = col;
	TM_HD44780_Opts.currentY = row;
	
	/* Set location address */
	TM_HD44780_Cmd(TM_HD44780_SETDDRAMADDR | (col + row_offsets[row]));
}

void TM_HD44780_Put(uint8_t x, uint8_t y, char str) {
	TM_HD44780_CursorSet(x, y);
	TM_HD44780_Data(str);
}

void TM_HD44780_Puts(uint8_t x, uint8_t y, char* str) {
  
	TM_HD44780_CursorSet(x, y);
	while (*str) {
		if (TM_HD44780_Opts.currentX >= TM_HD44780_Opts.Cols) {
			TM_HD44780_Opts.currentX = 0;
			TM_HD44780_Opts.currentY++;
			TM_HD44780_CursorSet(TM_HD44780_Opts.currentX, TM_HD44780_Opts.currentY);
		}
		if (*str == '\n') {
			TM_HD44780_Opts.currentY++;
			TM_HD44780_CursorSet(TM_HD44780_Opts.currentX, TM_HD44780_Opts.currentY);
		} else if (*str == '\r') {
			TM_HD44780_CursorSet(0, TM_HD44780_Opts.currentY);
		} else {
			TM_HD44780_Data(*str);
			TM_HD44780_Opts.currentX++;
		}
		str++;
	}
}

void TM_HD44780_DisplayOn(void) {
	TM_HD44780_Opts.DisplayControl |= TM_HD44780_DISPLAYON;
	TM_HD44780_Cmd(TM_HD44780_DISPLAYCONTROL | TM_HD44780_Opts.DisplayControl);
}

void TM_HD44780_DisplayOff(void) {
	TM_HD44780_Opts.DisplayControl &= ~TM_HD44780_DISPLAYON;
	TM_HD44780_Cmd(TM_HD44780_DISPLAYCONTROL | TM_HD44780_Opts.DisplayControl);
}

void TM_HD44780_BlinkOn(void) {
	TM_HD44780_Opts.DisplayControl |= TM_HD44780_BLINKON;
	TM_HD44780_Cmd(TM_HD44780_DISPLAYCONTROL | TM_HD44780_Opts.DisplayControl);
}

void TM_HD44780_BlinkOff(void) {
	TM_HD44780_Opts.DisplayControl &= ~TM_HD44780_BLINKON;
	TM_HD44780_Cmd(TM_HD44780_DISPLAYCONTROL | TM_HD44780_Opts.DisplayControl);
}

void TM_HD44780_CursorOn(void) {
	TM_HD44780_Opts.DisplayControl |= TM_HD44780_CURSORON;
	TM_HD44780_Cmd(TM_HD44780_DISPLAYCONTROL | TM_HD44780_Opts.DisplayControl);
}

void TM_HD44780_CursorOff(void) {
	TM_HD44780_Opts.DisplayControl &= ~TM_HD44780_CURSORON;
	TM_HD44780_Cmd(TM_HD44780_DISPLAYCONTROL | TM_HD44780_Opts.DisplayControl);
}

void TM_HD44780_ScrollLeft(void) {
	TM_HD44780_Cmd(TM_HD44780_CURSORSHIFT | TM_HD44780_DISPLAYMOVE | TM_HD44780_MOVELEFT);
}

void TM_HD44780_ScrollRight(void) {
	TM_HD44780_Cmd(TM_HD44780_CURSORSHIFT | TM_HD44780_DISPLAYMOVE | TM_HD44780_MOVERIGHT);
}

void TM_HD44780_CreateChar(uint8_t location, uint8_t *data) {
	uint8_t i;
	/* We have 8 locations available for custom characters */
	location &= 0x07;
	TM_HD44780_Cmd(TM_HD44780_SETCGRAMADDR | (location << 3));
	
	for (i = 0; i < 8; i++) {
		TM_HD44780_Data(data[i]);
	}
}

void TM_HD44780_PutCustom(uint8_t x, uint8_t y, uint8_t location) {
	TM_HD44780_CursorSet(x, y);
	TM_HD44780_Data(location);
}

