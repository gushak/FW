#ifndef _WEIGHT_H_
#define _WEIGHT_H_

typedef struct {
	uint8_t name1[10];
	uint8_t name2[10];              
	uint8_t name3[10];         
	uint8_t value[50];    
} Wight_struct;
void weight_ParseData(char* Buffer);
void parse_weight(void);
void parse_weight_interrupt(uint8_t c);

#endif