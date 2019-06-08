#include "hw_control.h"
#include "temperature.h"
//서울세미텍 서미스터 -20도 부터 120도까지...

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
	// ADC의 값이 928보다 크면 -20도, 31보다 작으면 120도로 간주
	if (adc >= TEMP_ADC_ARR[0]) temp = -200;
	else if (adc <= TEMP_ADC_ARR[28]) temp = 1200;
	else
	// ADC의 값이 TEMP_ADC_ARR[]의 범위 안에 있으면 보간법 계산 시작
	{
		no = 0;
		// ADC의 값의 위치 찾기(몇도에서 몇도 범위인지?)
		for (lp = 0; lp < 29; lp++)
		if (adc < TEMP_ADC_ARR[lp]) no++;

		// ADC의 값보다 높은 기준온도값
		high = TEMP_ADC_ARR[no];
		no--;
		// ADC의 값보다 높은 기준온도값
		low = TEMP_ADC_ARR[no];
		// 보간법으로 편차 계산
		dt = (adc - high) * 50 / (low - high);
		// 실제 온도값 계산
		temp = (no + 1) * 50 - 200 - dt;

		//temp = -200;
		//Real_Temp=temp; 
		//Temp_High20 = (temp + 200) / 10;   //-20도까지의 값을 갖게 하기위하여 ..

		if(temp<0)
		{
			temp=~temp;     //1의보수를 취한다. 
			temp ++;        //2의보수를 원위치 하기위하여  

		}
                         
	}

	// 보간법계산함수 시험용 printf 문
	///Hex2_to_ASC4_Spool(temp);
	return temp;
}


