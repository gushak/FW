/**
 * @author  Tilen Majerle
 * @email   tilen@majerle.eu
 * @website http://stm32f4-discovery.com
 * @link    http://stm32f4-discovery.com/2014/07/library-19-use-internal-rtc-on-stm32f4xx-devices/
 * @version 1.7.1
 * @ide     Keil uVision
 * @license GNU GPL v3
 * @brief   Internal RTC library for STM32F4xx devices
 *	
@verbatim
   ----------------------------------------------------------------------
    Copyright (C) Tilen Majerle, 2015
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
     
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------
@endverbatim
 */
#ifndef TM_RTC_H
#define TM_RTC_H 171

#include "RTC_to_sec.h"
	 
uint32_t Time_set(tm_struct time);
void Time_Adjust(tm_struct time);
void RTC_Configuration(void);
void RTC_Init(void);
tm_struct read_RTC(void);
uint16_t GetTime(void);
uint16_t Get_Date(void);
uint16_t GetTime_min(void);

#endif

