#include "smartplug_it.h"
#include "smartplug_hw.h"
#include "smartplug_impl.h"
#include "SM_NMS.h"
#include "SM_V9871.h"
#include "stm32f1xx.h"
#include <assert.h>
#include <stdio.h>
#include <time.h>

#define MAX_SIZE 9 
#define SWAP(x, y, temp) ((temp) = (x), (x)=(y), (y)=(temp))

// max threshold milliseconds of no main loop response
#define NO_RESPOSE_THRESHOLD_MILLS 1000

static uint16_t main_loop_no_response_count_;

RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;

extern RTC_HandleTypeDef hrtc;
extern bool_t g_relay_is_on;

void smartplug_set_main_loop_run()
{
        main_loop_no_response_count_ = 0;
}

void UserRTC_GetTimer()
{
        HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
        HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);
      
        if(g_RtcConfig.RtcAct.day == sDate.Date && g_RtcConfig.RtcAct.hour == sTime.Hours && g_RtcConfig.RtcAct.min == sTime.Minutes && g_RtcConfig.RtcAct.sec == sTime.Seconds && g_relay_is_on == kTrue)
        {
              hw_relay_func(kTrue);
              printf("d] %02x, h] %02x, m] %02x, s] %02x \r\n",sDate.Date, sTime.Hours, sTime.Minutes, sTime.Seconds);
        }
        else if(g_RtcConfig.RtcAct.day == sDate.Date && g_RtcConfig.RtcAct.hour == sTime.Hours && g_RtcConfig.RtcAct.min == sTime.Minutes && g_RtcConfig.RtcAct.sec == sTime.Seconds && g_relay_is_on == kFalse)
        {
              hw_relay_func(kFalse);
              printf("d] %02x, h] %02x, m] %02x, s] %02x \r\n",sDate.Date, sTime.Hours, sTime.Minutes, sTime.Seconds);
        }
}

void UserRTC_SetTimer(uint8_t* Buf)
{
        sDate.Year = Buf[0];
        sDate.Month = Buf[1];
        sDate.WeekDay = Buf[2];
        sDate.Date = Buf[3];
        if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
        {
          _Error_Handler(__FILE__, __LINE__);
        }
        
        sTime.Hours = Buf[4];
        sTime.Minutes = Buf[5];
        sTime.Seconds = Buf[6];
        if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
        {
          _Error_Handler(__FILE__, __LINE__);
        }        
        
}

int partition(int list[], int left, int right)
{
      int pivot, temp;
      int low, high;
      
      low = left;
      high = right + 1;
      pivot = list[left];
      
      do{
            do{
                low++;
            } while(low <= right && list[low] < pivot);
          
            do{
                high--;
            } while(high >= left && list[high] > pivot);
          
            if (low<high)
            {
                  SWAP(list[low], list[high], temp);
            }
      } while (low<high);
      
      SWAP(list[left], list[high], temp);
      
      return high;
}

void quick_sort(int list[], int left, int right)
{
    if (left < right)
    {
          int q = partition(list, left, right);
          
          quick_sort(list, left , q-1);
          quick_sort(list, q+1, right);
    }
}
