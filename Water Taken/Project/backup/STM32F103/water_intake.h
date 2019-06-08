#ifndef _WATER_INTAKE_H_
#define _WATER_INTAKE_H_

#define MRAM_INTAKE_MAX_ADDRESS	200
#define RFID_UNKNOWN 9999

extern void Gate_open(uint16_t m_cnt);

typedef struct Water_Intake_Table_Struct WATER_ITAKE_TABLE_STRUCT;

#pragma pack(push,1)
struct Water_Intake_Table_Struct
{
	uint16_t index;				//�Ϸù�ȣ 
	uint8_t status;				//�޿��� ����		//���� ���̺��� ����Ǿ��ٸ� �ش� �������ͽ� ���� ���� ���泻���� �����Ҽ� �ֵ��� �մϴ�. 
	uint16_t rfid_num;		//��ü �ĺ���ȣ RFID
	uint16_t date;				//����� ��¥ �����(��ȯ) 
	uint16_t enter_time;		//����ð�
	uint16_t visit_time;		//ü���ð�
	uint16_t water_cnt1;		//pcnt1//qt1;				//1��� ���뷮
	uint16_t water_cnt2;		 //pcnt2//qt2;				//2��� ���뷮  
	uint16_t water_temp;	 //weight//temp//qt3;				//3��� ���뷮
	uint16_t aqt1;					//÷����1 ���뷮
	uint16_t aqt2;					//÷����2 ���뷮
	uint16_t intake_weight;			//���뷮 ���� //��ü ����
	uint8_t feeder_num;			//������ ���̱� ��ȣ
};
#pragma pack(pop)

extern WATER_ITAKE_TABLE_STRUCT W_Intake;

extern void intake_Run();


#endif