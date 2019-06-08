
#ifndef _RTC_TO_SEC_H_
#define _RTC_TO_SEC_H_
// rtc.h

struct _tm_struct {
int tm_sec; /* seconds after the minute - [0,59] */
int tm_min; /* minutes after the hour - [0,59] */
int tm_hour; /* hours since midnight - [0,23] */
int tm_mday; /* day of the month - [1,31] */
int tm_mon; /* months since January - [0,11] */
int tm_year; /* years since 1900 */
int tm_wday; /* days since Sunday - [0,6] */
int tm_yday; /* days since January 1 - [0,365] */
int tm_isdst; /* daylight savings time flag */ // NOT USED
};

typedef struct _tm_struct tm_struct;
u32 RTC_DateToBinary(tm_struct *datetime);
void RTC_BinaryToDate(u32 binary,tm_struct *datetime) ;

#endif