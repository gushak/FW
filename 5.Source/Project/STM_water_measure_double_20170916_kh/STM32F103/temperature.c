#include "hw_control.h"
#include "temperature.h"
//���＼���� ���̽��� -20�� ���� 120������...

uint16_t TEMP_ADC_ARR[] =            {

        892, //-20
        863, //-15 
        829, //-10
        791, //-05
        749, //-00
        705, // 05
        657, // 10
        609, // 15
        560, // 20
        512, //25 
        465, //30
        420, //35 
        377, //40
        337, 
        300,
        267, 
        238, 
        211, 
        187, 
        166,
        146,  
        130,  
        115,  
        102,  
         91,
         81,  72,  65,  58  
  
};




uint16_t Detect_Temp_Data(uint16_t adc)
{
	int temp;
	char lp, no;
	int  low, high, dt;

        
        adc /= 4; 
	// ADC�� ���� 928���� ũ�� -20��, 31���� ������ 120���� ����
	if (adc >= TEMP_ADC_ARR[0]) temp = -200;
	else if (adc <= TEMP_ADC_ARR[28]) temp = 1200;
	else
	// ADC�� ���� TEMP_ADC_ARR[]�� ���� �ȿ� ������ ������ ��� ����
	{
		no = 0;
		// ADC�� ���� ��ġ ã��(����� � ��������?)
		for (lp = 0; lp < 29; lp++)
		if (adc < TEMP_ADC_ARR[lp]) no++;

		// ADC�� ������ ���� ���ؿµ���
		high = TEMP_ADC_ARR[no];
		no--;
		// ADC�� ������ ���� ���ؿµ���
		low = TEMP_ADC_ARR[no];
		// ���������� ���� ���
		dt = (adc - high) * 50 / (low - high);
		// ���� �µ��� ���
		temp = (no + 1) * 50 - 200 - dt;

		//temp = -200;
		//Real_Temp=temp; 
		//Temp_High20 = (temp + 200) / 10;   //-20�������� ���� ���� �ϱ����Ͽ� ..

		if(temp<0)
		{
			temp=~temp;     //1�Ǻ����� ���Ѵ�. 
			temp ++;        //2�Ǻ����� ����ġ �ϱ����Ͽ�  

		}
                         
	}

	// ����������Լ� ����� printf ��
	///Hex2_to_ASC4_Spool(temp);
	return temp;
}


