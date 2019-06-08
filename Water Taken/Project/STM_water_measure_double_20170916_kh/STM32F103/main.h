#ifndef __MAIN_H__
#define __MAIN_H__

extern void lcd_view_init(void);
extern void setting(void);
extern void timer_10ms_loop(void);
extern void timer_100ms_loop(void);
extern void main_view(void);
extern void lcd_view_print(void);
extern void loop(void);
extern void Operation_Rtn(void);
extern uint8_t loop_Stable_ch_check();
/* R */
extern uint8_t R_IR_check();
extern uint8_t R_IR_check2();
extern void R_setting_menu();
extern uint16_t R_rfid_check(void);
extern uint8_t R_MaxWater_check();
extern void R_rfid_clear(void);
extern void lora_init(void);
extern volatile uint32_t FET_sum[2];
extern volatile uint16_t FET_value[10];
#endif