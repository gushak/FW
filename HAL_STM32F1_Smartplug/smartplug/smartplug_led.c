/*****************************************************************************
* Copyright(C) 2018 Enernet Information & Communication Corp.
* All right reserved.
*
* Description	: LED management
*
* History
* Date			Description
* 2018/09/28	file created
*****************************************************************************/

#include "smartplug_led.h"
#include "smartplug_hw.h"

typedef void(*LED_onoff_func)(bool_t is_on);

struct LED_PARAM
{
        bool_t blink;
        bool_t repeat;
        bool_t is_on;
        uint32_t counter;
        uint32_t duration;
        LED_onoff_func onoff_func;
};

struct LED_CONTEXT
{
        struct LED_PARAM params[LED_MAX];
};

static struct LED_CONTEXT led_;

void led_init()
{
        memset(&led_, 0, sizeof(led_));
        led_.params[kLED_RG].onoff_func = hw_led_rg_onoff;
        led_.params[kLED_DLINK].onoff_func = hw_led_dlink_onoff;
}

void led_on_tick(uint32_t tick)
{
      size_t i;
        
      for(i = 0; i < LED_MAX; ++i)
      {
                struct LED_PARAM* param = &led_.params[i];
                
                if (!param->blink)
                        continue;
                        
                param->counter++;
                if (param->counter > param->duration)
                {
                        param->is_on = param->is_on ? kFalse : kTrue;
                        param->onoff_func(param->is_on);
                        if (param->repeat)
                        {
                                param->counter = 0;
                        }
                        else
                        {
                                param->blink = kFalse;
                        }
                }
        }
}

void led_blink(LED_Type_t led_type, bool_t start_onoff_state, uint32_t duration, bool_t repeat)
{
      struct LED_PARAM* param = &led_.params[led_type];
      
      param->is_on = start_onoff_state;
      param->onoff_func(param->is_on);
      param->duration = duration;
      param->repeat = repeat;
      param->counter = 0;
      param->blink = kTrue;
}

void led_onoff(LED_Type_t led_type, bool_t is_on)
{
      struct LED_PARAM* param= &led_.params[led_type];
      
      param->is_on = is_on;
      param->onoff_func(param->is_on);
      param->duration = 0;
      param->repeat = kFalse;
      param->counter = 0;
      param->blink = kFalse;
}

void led_onoff_all(bool_t is_on)
{
	int i;

	for (i = 0; i < LED_MAX; ++i)
	{
		led_onoff((LED_Type_t)i, is_on);
	}
}



