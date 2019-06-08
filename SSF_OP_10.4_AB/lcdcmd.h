#ifndef LCDCMD_H
#define LCDCMD_H

/*
   lcdcmd.h
   written by KMH on Dec. 05, 2006
*/

// Coordination Type
//#define ABSXY           0x00    // absolute coordination
//#define RELXY           0x01    // relative coordination

enum _lcd_color{
    _WHITE = 0,         // 0
    _BLACK,             // 1
    _GRAY,              // 2
    _RED,               // 3
    _GREEN,             // 4
    _BLUE,              // 5
    _YELLOW,            // 6
    _TRIM,              // 7
    _SET_ORG,           // 8
    _PAUSE,             // 9
    _STI_SPD,           // 10
    _SCL_REF,           // 11
    _SET_OPnn,          // 12
    _CHK_IPnn,          // 13
    _TIME_DELAY,        // 14
    _PF_CONTROL,        // 15
    _CAN_OPnn,          // 16
    _CAN_IPnn,          // 17
    _TRANSPARENCY,      // 18
    _JUMP,              // 19
    _REV,               // 20   //(121212)
}; // mc color

enum _lcd_cmd {
	LCD_SETENV = 0,
	LCD_SETAREA, // 1
	LCD_TEXTOUT, // 2
	LCD_CLEAR,   // 3
	DRAW_DESIGN, // 4
	DRAW_STITCH, // 5
	MOVE_CURSOR, // 6
	LCD_LIST,    // 7
	LCD_PROGRAM, // 8
	LCD_ACK,     // 9
	LCD_READY,   // 10
	LCD_TOUCH,   // 11
	OPEN_POPUP,  // 12
	CLOSE_POPUP, // 13
}; // current 14, max 16

enum _set_env {
	SET_WIN = 0,
	SET_XY,
	SET_COL,
	SET_FONT,
	SET_CLS,
	SET_EXIT,
	SET_UFMT,
}; // current 7, max 8

enum _prg_mode {
	PRG_ZOOM = 0,
	PRG_GRID,
	PRG_CENTER,
}; // current 3, max 8

enum _cursor {
	CSS_CROSS = 0,
	CSS_CIRCLE,
}; // current 2, max 8

enum _uformat {
	UF_ASCII = 0,
	UF_ASCII_CP,
	UF_UNICODE,
	UF_UTF8,
};

#endif // LCDCMD_H
