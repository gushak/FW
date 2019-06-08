/*


*/

#include "common.h"
#include <string.h>

u8 g_days[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// 31 -> 0x31
inline u8 BIN2BCD(u8 bin_value)
{
	u8 bcd_value;

	bcd_value  = (bin_value / 10)<<4;
	bcd_value |= (bin_value % 10)<<0;

	return bcd_value;
}

// 0x31 -> 31
inline u8 BCD2BIN(u8 bcd_value)
{
	u8 bin_value;

	bin_value  = ((bcd_value & 0xf0)>>4) * 10;
	bin_value += ((bcd_value & 0xf) >>0);

	return bin_value;
}


// 0x31 -> "31"
void BIN2HEX(u8* pBin, u8* pHex, u16 usLenBin)
{
	char data, hex, *p;
	u16 i;

	if( (pBin==NULL) || (pHex==NULL) ) return;

	p = (char *)pHex;

	for(i=0; i<usLenBin; i++)
	{
		data = (pBin[i] & 0xf0)>>4;
		hex = (data >= 10) ? (data + 'A' - 10) : (data + '0');
		*p++ = hex;

		data = (pBin[i] & 0xf);
		hex = (data >= 10) ? (data + 'A' - 10) : (data + '0');
		*p++ = hex;
	}
}

// "31" -> 0x31
void HEX2BIN(u8* pHex, u8* pBin, u16 usLenHex)
{
	char data, bin;
	u8 *p;
    u16 i;

	if( (pHex==NULL) || (pBin==NULL) ) return;

	usLenHex -= (usLenHex%2) ? 1 : 0;
	p = pBin;

	for(i=0; i<usLenHex; i+=2)
	{
		data = pHex[i];
		bin  = ((data >= 'a') ? (data - 'a' + 10) : (data >= 'A') ? (data - 'A' + 10) : (data - '0')) * 16;
		data = pHex[i+1];
		bin += ((data >= 'a') ? (data - 'a' + 10) : (data >= 'A') ? (data - 'A' + 10) : (data - '0'));
		*p++ = bin;
	}
}

// 0x3132 -> 12  (for not terminated ascii-numeric string conversion)
u32 BIN2DEC(u8* pBin, u16 usLenBin)
{
	u32 ulRet = 0;
	u16 i;

	if( (pBin==NULL) || (usLenBin==0) ) 
	{
		return 0;
	}

	for(i=0; i<usLenBin; i++)
	{
		if( (pBin[i]>='0') && (pBin[i]<='9') )
		{
			ulRet *= 10;
			ulRet += (pBin[i]-'0');
		}
		else
		{
			return 0;
		}
	}

	return ulRet;
}

// 12 -> 0x3132  (for not terminated ascii-numeric string conversion)
void DEC2BIN(u32 ulDec, u8* pBin, u16 usLenDec)
{
	u32 ulTemp, ulDiv;
    u16 i;

	if(pBin==NULL) return;

	ulDiv = (u32)pow(10,usLenDec);
	ulDec = (ulDec % ulDiv);		//ÀÚ¸®¼ö¸¦ ¸ÂÃã, ex)1234¿¡ 2ÀÚ¸® º¯È¯Àº 34, 3ÀÚ¸®º¯È¯Àº 234

	for(i=0; i<usLenDec; i++)
	{
		ulDiv = (u32)pow(10,usLenDec-i-1);
		ulTemp = (ulDec  / ulDiv);
		ulDec -= (ulTemp * ulDiv);
		pBin[i] = ulTemp + '0';
	}
}


// DBJ2 Hash Funtion : http://www.cse.yorku.ca/~oz/hash.html

u32 STR2HASH(const s8* pstr, u16 size)
{
    u32 hash = 5381;
	s8 *p = (s8 *)pstr;

	if(pstr == NULL) return 0;

	while(size--)
	{
		hash = ((hash << 5) + hash) + *p++;
	}

    return hash;
}

u16 GetCmdHandlerIndex(CMD_MAP* pMap, u16 usMapCount, s8* buffer)
{
	u32 hash;
	u16 idx, len;
	s8 *p1, *p2;

	if(pMap == NULL) return 0;
	if(buffer == NULL) return 0;

	// CR/LF Á¦°Å
	if(buffer[strlen((char *)buffer)-1]==_LF)
	{
		buffer[strlen((char *)buffer)-1] = 0;
	}
	if(buffer[strlen((char *)buffer)-1]==_CR)
	{
		buffer[strlen((char *)buffer)-1] = 0;
	}

	p1 = (s8*)strchr((char *)buffer, ':');	//for Modem Response
	p2 = (s8*)strchr((char *)buffer, ',');	//for Admin Command
	if(p2 == NULL)
	{
		p2 = (s8*)strchr((char *)buffer, '=');	//for Legacy Admin Command (ex. #TSN=xxx)
	}

	if(p1 && p2)
	{
		if(p1 > p2)
			len = (s16)(p2 - &buffer[0]);
		else
			len = (s16)(p1 - &buffer[0]);
	}
	else if(p1)
	{
		len = (s16)(p1 - &buffer[0]);
	}
	else if(p2)
	{
		len = (s16)(p2 - &buffer[0]);
	}
	else
	{
		len = (s16)strlen((char *)buffer);
	}

	hash = STR2HASH(buffer, len);

	for(idx=1; idx<usMapCount; idx++)
	{
		if(GET_CMD_HASH(pMap, idx) == hash)
		{
			return idx;
		}
	}

	return 0;
}

// 1. 4·Î ³ª´©¾î ¶³¾îÁö¸é¼­ 100À¸·Î ³ª´©¾î ¶³¾îÁöÁö ¾Ê´Â ÇØ
// 2. 400À¸·Î ³ª´©¾î ¶³¾îÁö´Â ÇØ
BOOL isLeapYear(u16 year)
{
	BOOL isDiv4, isDiv100, isDiv400, isLeapYear=FALSE;

	isDiv4   = ((year%4)==0)   ? TRUE : FALSE;
	isDiv100 = ((year%100)==0) ? TRUE : FALSE;
	isDiv400 = ((year%400)==0) ? TRUE : FALSE;

	isLeapYear = (isDiv4 && (isDiv100==FALSE)) || (isDiv400);

	return isLeapYear;
}

u8 GetDayOfWeek(PDATETIME pDateTime)
{
	u32 day_count;
	u16 yy;
	u8  mm;

	if(pDateTime==NULL) return 0;

	//Àü³â±îÁöÀÇ ÀÏ¼ö °è»ê
	yy = pDateTime->year - 1;
	day_count = yy * 365;
	day_count += (u16)(yy/4) - (u16)(yy/100) + (u16)(yy/400);

	//¿ÃÇØ 1¿ù1ÀÏºÎÅÍ Áö³­´Þ±îÁöÀÇ ÀÏ¼ö°è»ê
	for(mm=1; mm<pDateTime->month; mm++)
	{
		day_count += g_days[mm-1];
	}
	if((pDateTime->month>2) && isLeapYear(pDateTime->year))
	{
		day_count++;
	}
	//ÀÌ´Þ 1ÀÏºÎÅÍ ¿À´Ã±îÁöÀÇ ÀÏ¼ö°è»ê
	day_count += pDateTime->day;

	return (u8)(day_count%7);
}

void ConvTimeStampToDateTime(u32 ulTimeStamp, PDATETIME pDateTime)
{
	u32 ulDays, ulSeconds;
	u16 yy, days_of_year;
	u8  mm;

	if(pDateTime==NULL) return;

	ulDays		= ulTimeStamp / SECONDS_OF_DAY + 1;		//´çÀÏ
	ulSeconds	= ulTimeStamp % SECONDS_OF_DAY;

	// ³¯Â¥º¯È¯
	for(yy=1970;  ; yy++)
	{
		days_of_year = isLeapYear(yy) ? 366 : 365;
		if(ulDays <= days_of_year) 
		{
			pDateTime->year = yy;
			break;
		}
		ulDays -= days_of_year;
	}

	pDateTime->month = 1;
	for(mm=1; mm<=12; mm++)
	{
		if(ulDays <= g_days[mm-1])
		{
			pDateTime->month = mm;
			break;
		}
		ulDays -= g_days[mm-1];
	}

	pDateTime->day = ulDays;

	// ½Ã°£º¯È¯
	pDateTime->hour		= ulSeconds / 3600;
	ulSeconds = (ulSeconds % 3600);
	pDateTime->minute	= ulSeconds / 60;
	pDateTime->second	= ulSeconds % 60;

//	DBGMSG(2, "%s: %lu --> %04d/%02d/%02d %02d:%02d:%02d\r\n",
//			__func__, ulTimeStamp,
//			pDateTime->year, pDateTime->month, pDateTime->day,
//			pDateTime->hour, pDateTime->minute, pDateTime->second);

	return;
}

u32 ConvDateTimeToTimeStamp(PDATETIME pDateTime)
{
	u32 timestamp = 0;
	u16 yy;
	u8  mm;

	if(pDateTime==NULL) return 0;
	if(pDateTime->year < 1970) return 0;

	//1970/01/01ºÎÅÍ ÀÛ³â±îÁöÀÇ ÀÏ¼ö°è»ê
	for(yy=1970; yy<pDateTime->year; yy++)
	{
		timestamp += (isLeapYear(yy)) ? 366 : 365;
	}
	//¿ÃÇØ 1¿ùºÎÅÍ Àü¿ù±îÁöÀÇ ÀÏ¼ö°è»ê
	for(mm=1; mm<pDateTime->month; mm++)
	{
		timestamp += g_days[mm-1];
	}
	if((pDateTime->month>2) && isLeapYear(pDateTime->year))
	{
		timestamp++;
	}
	//ÀÌ´Þ 1ÀÏºÎÅÍ ¾îÁ¦±îÁöÀÇ ÀÏ¼ö°è»ê
	timestamp += (pDateTime->day-1);

	//ÀÏ¼ö->ÃÊ½Ã°£À¸·Î º¯È¯: 1ÀÏ=86400ÃÊ
	timestamp = (timestamp * SECONDS_OF_DAY);

	//±ÝÀÏ °æ°ú½Ã°£ÀÇ ÃÊ¼ö¸¦ ´õÇÔ
	if(pDateTime->hour)
	{
		timestamp += (pDateTime->hour*3600);
	}
	if(pDateTime->minute)
	{
		timestamp += (pDateTime->minute*60);
	}
	timestamp += pDateTime->second;

//	DBGMSG(2, "%s: %04d/%02d/%02d %02d:%02d:%02d --> %lu\r\n",
//			__func__,
//			pDateTime->year, pDateTime->month, pDateTime->day,
//			pDateTime->hour, pDateTime->minute, pDateTime->second,
//			timestamp);

	return timestamp;
}

void ConvGpsDateTime(u32 ulDateUtc, u32 ulTimeUtc, PDATETIME pDateTime, BOOL isConvKST)
{
	u32 timestamp;

	if(pDateTime==NULL) return;

//	GPS Date: ddmmyy
	pDateTime->day		= (u8)(ulDateUtc / 10000);
	pDateTime->month	= (u8)((ulDateUtc % 10000) / 100);
	pDateTime->year		= (ulDateUtc % 100) + 2000;

//	GPS Time: hrmise (UTC)
	pDateTime->hour		= (u8)(ulTimeUtc / 10000);
	pDateTime->minute	= (u8)((ulTimeUtc % 10000) / 100);
	pDateTime->second	= (ulTimeUtc % 100);

	if(isConvKST)
	{
		timestamp = ConvDateTimeToTimeStamp(pDateTime) + KST_OFFSET;
		ConvTimeStampToDateTime(timestamp, pDateTime);
	}
}

char* GetTimeStringFromTimeStamp(u32 ulTimeStamp)
{
	static char szDateTime[20];
	DATETIME datetime;

	memset(szDateTime, 0, 20);

	ConvTimeStampToDateTime(ulTimeStamp, &datetime);

	sprintf(szDateTime, "%04d/%02d/%02d %02d:%02d:%02d\0",
						datetime.year, datetime.month,  datetime.day,
						datetime.hour, datetime.minute, datetime.second);

	return szDateTime;
}

// hhmmss format, ulTime1=from, ulTime2=to
u32 GpsTimeDiffInt(u32 ulTime1, u32 ulTime2)
{
	u32 ulTime1s, ulTime2s;

	if(ulTime2 < ulTime1) ulTime2 += 240000;

	ulTime1s = ((u32)(ulTime1 / 10000)) * 3600;
	ulTime1s += (((u32)(ulTime1 / 100)) % 100) * 60;
	ulTime1s += (ulTime1 % 100);

	ulTime2s = ((u32)(ulTime2 / 10000)) * 3600;
	ulTime2s += (((u32)(ulTime2 / 100)) % 100) * 60;
	ulTime2s += (ulTime2 % 100);

	return (ulTime2s - ulTime1s);
}

// hhmmss.ss format, dTime1=from, dTime2=to
double GpsTimeDiffDouble(double dTime1, double dTime2)
{
	u32 ulTime1, ulTime2;
	double dTime1s, dTime2s;

	ulTime1 = (u32)(dTime1 * 100.0);
	ulTime2 = (u32)(dTime2 * 100.0);
	if(ulTime2 < ulTime1) ulTime2 += 24000000;

	dTime1s = ((u32)(ulTime1 / 1000000)) * 3600;
	dTime1s += (((u32)(ulTime1 / 10000)) % 100) * 60;
	dTime1s += (ulTime1 % 10000) / 100.0;

	dTime2s = ((u32)(ulTime2 / 1000000)) * 3600;
	dTime2s += (((u32)(ulTime2 / 10000)) % 100) * 60;
	dTime2s += (ulTime2 % 10000) / 100.0;

	return (dTime2s - dTime1s);
}

//NMEA»óÀÇ ÁÂÇ¥¸¦ ¸Ê»óÀÇ ÁÂÇ¥·Î º¯È¯ (WGS84)
s32 Conv_DMM2DDD_long(double dmm)
{
	s32 ddd, d;
	double mm;

	d  = (s32)(dmm / 100);		//D
	mm = (dmm - d*100);			//MM

	ddd = (s32)( (d + mm/60) * 10000000 );

	return ddd;
}

//¸Ê»óÀÇ ÁÂÇ¥¸¦ NMEAÁÂÇ¥·Î º¯È¯
s32 Conv_DDD2DMM_long(double ddd)
{
	s32 dmm, d;
	double dd;

#if 0 	// 10000000 °öÇØÁø Á¤¼öÀÔ·Â½Ã
	d = (s32)(ddd / 10000000);		//D
	dd = (ddd / 10000000.0 - d);	//MM

	dmm = (s32)((d*100 + dd*60) * 100000);
#else
	d = (s32)ddd;
	dd = (ddd - d);

	dmm = (s32)((d*100 + dd*60) * 100000);
#endif

	return dmm;
}

double Conv_DMM2DDD_double(double dmm)
{
	s32 d;
	double mm, ddd;

	d  = (s32)(dmm / 100);		//D
	mm = (dmm - d*100);			//MM

	ddd = d + mm/60.0;

	return ddd;
}

double Conv_DDD2DMM_double(double ddd)
{
	s32 d;
	double dd, dmm;

	d = (s32)ddd;
	dd = (ddd - d);

	dmm = d*100 + dd*60;

	return dmm;
}

//Àû¿ë°ø½Ä: Greate Circle Distance Calculation
double CalcGpsDistance(s32 lat1, s32 lon1, s32 lat2, s32 lon2)
{
	double dlat1, dlon1, dlat2, dlon2;
	double theta, dist=0.0;
//	u32 dist_meter;

	if( (lat1==lat2) && (lon1==lon2) ) return 0;

	//Á¤¼öÇü ÁÂÇ¥¸¦ ½Ç¼ö·Î ¹Ù²Þ
#if 0
	dlat1 = lat1 / 10000000.0;
	dlon1 = lon1 / 10000000.0;
	dlat2 = lat2 / 10000000.0;
	dlon2 = lon2 / 10000000.0;

	theta = dlon1 - dlon2;
	dist = sin(dlat1*DEG2RAD) * sin(dlat2*DEG2RAD) + cos(dlat1*DEG2RAD) * cos(dlat2*DEG2RAD) * cos(theta*DEG2RAD);
#else
	dlat1 = lat1 / 10000000.0 * DEG2RAD;
	dlon1 = lon1 / 10000000.0 * DEG2RAD;
	dlat2 = lat2 / 10000000.0 * DEG2RAD;
	dlon2 = lon2 / 10000000.0 * DEG2RAD;

	theta = dlon1 - dlon2;
	dist = sin(dlat1) * sin(dlat2) + cos(dlat1) * cos(dlat2) * cos(theta);
#endif

#if 0
	dist = acos(dist)*RAD2DEG*60*1.1515*MILE_TO_KM;

	dist_meter = (u32)(dist*1000);

	return dist_meter;
#else
	dist = acos(dist)*RAD2DEG*60*1.1515*MILE_TO_KM*1000;	//km->m

	return dist;
#endif
}

u32 CalcGpsDistanceSimple(s32 lat1, s32 lon1, s32 lat2, s32 lon2)
{
	float lat, lon;
	u32 dist_meter;

	if( (lat1==lat2) && (lon1==lon2) ) return 0;

	lat = (lat2-lat1)*1.0/LAT_PER_METER;
	lon = (lon2-lon1)*1.0/LON_PER_METER;

//	dist_meter = (u32)sqrt(lat^2 + lon^2);
	dist_meter = (u32)sqrt(lat*lat + lon*lon);

	return dist_meter;
}

BOOL CheckGpsRegion(s32 latGps, s32 lonGps, s32 latChk, s32 lonChk, u16 radChk)
{
	u32 dist;

	dist = CalcGpsDistanceSimple(latGps, lonGps, latChk, lonChk);
	
	return (dist <= radChk) ? TRUE : FALSE;
}

u16  GetByteCheckSum(u16 cs_init, u8* buffer, u32 len)
{
	u16 cs=cs_init;
	u8 *p = buffer;

	if(p)
	{
		while(len--) cs += *p++;
	}

	return cs;
}

u8  GetByteXor(u8 cs_init, u8* buffer, u32 len)
{
	u8 cs=cs_init, *p = buffer;

	if(p)
	{
		while(len--) cs ^= *p++;
	}

	return cs;
}

u16 GetFletcherCheckSum(u16 cs_init, u8* buffer, u32 len)
{
	u8 *p = buffer, ck_a, ck_b;
	u16 cs = cs_init;

	ck_a = (cs>>0) & 0xff;
	ck_b = (cs>>8) & 0xff;

	if(p)
	{
		while(len--)
		{
			ck_a += *p++;
			ck_b += ck_a;
		}
	}

	cs = (ck_b<<8) | ck_a;

	return cs;
}

u32 ConvIpStr2Inet(u8 *pIpStr)
{
	u32 ulInet = 0;
#if 0
	char *token, *save_ptr;

	if(pIpStr==NULL) return 0;

	token = strtok_r((char*)pIpStr, ".\r\n", &save_ptr);
	if(token)
	{
		ulInet = strtoul(token, NULL, 10);
		*(save_ptr-1) = '.';
		token = strtok_r(NULL, ".\r\n", &save_ptr);
	}
	if(token)
	{
		ulInet |= (strtoul(token, NULL, 10) << 8);
		*(save_ptr-1) = '.';
		token = strtok_r(NULL, ".\r\n", &save_ptr);
	}
	if(token)
	{
		ulInet |= (strtoul(token, NULL, 10) << 16);
		*(save_ptr-1) = '.';
		token = strtok_r(NULL, ".\r\n", &save_ptr);
	}
	if(token)
	{
		ulInet |= (strtoul(token, NULL, 10) << 24);
	}
#endif

	return ulInet;
}

char* ConvInet2IpStr(u32 ulInet)
{
	static char szIpStr[16] = {0,};

	sprintf(szIpStr, "%d.%d.%d.%d\0", (ulInet>>0)&0xff, (ulInet>>8)&0xff, (ulInet>>16)&0xff, (ulInet>>24)&0xff);

	return szIpStr;
}

#if 1 	//CAUTION: DATA LENGTH MUST BE 1,2,4
inline void ByteOrderChange(void* pDest, void* pSrc, u8 size)
{
	u8 i, *pdest = (u8*)pDest, *psrc = (u8*)pSrc;

	for(i=0; i<size; i++)
	{
		pdest[size-1-i] = psrc[i];
	}
}
#else	//inline assembly version for ARM
inline void ByteOrderChange(void* pDest, void* pSrc, u8 size)
{
	if(size==4)
	{
		asm volatile("REV %0, %1" : "=r" (*(u32*)pDest) : "r" (*(u32*)pSrc));
	}
	else if(size==2)
	{
		asm volatile("REV16 %0, %1" : "=r" (*(u16*)pDest) : "r" (*(u16*)pSrc));
	}
	else
	{
		u8 i, *pdest = (u8*)pDest, *psrc = (u8*)pSrc;
		for(i=0; i<size; i++) 
		{
			pdest[size-1-i] = psrc[i];
		}
	}
}
#endif

#if 0
inline float VSQRT(float arg)
{
	float result;

	asm volatile("VSQRT.F32 %0, %1" : "=t" (result) : "t" (arg));

	return result;
}
#endif

#if 0
#define MAX_FLOAT_PRECISION 	8
char* ConvFloatFormatStr(double dValue, u8 upper_dec_point, u8 below_dec_point)
{
	char szTempStr[MAX_FLOAT_PRECISION+1] = {0,};
	static char szFreqStr[MAX_LCD_COL+1] = {0,};	//MAX_LCD_COL=16
	u32 ulValue;
	u8 i, idx_r=0, idx_w=0, digit;
	double dTemp = dValue;

	if(dValue < 0) dValue = 0.0; 	//minus not support
	if(upper_dec_point > MAX_FLOAT_PRECISION) upper_dec_point = MAX_FLOAT_PRECISION;
	if(below_dec_point > MAX_FLOAT_PRECISION) below_dec_point = MAX_FLOAT_PRECISION;

	ulValue = (u32)(float)(dValue);
	sprintf(szTempStr, "%08lu\0", ulValue);

	idx_r = MAX_FLOAT_PRECISION - upper_dec_point;
	digit = upper_dec_point;

	for(i=idx_r; i<MAX_FLOAT_PRECISION; i++)
	{
		if( (digit != upper_dec_point) && ((digit%3)==0) )
		{
			szFreqStr[idx_w++] = ' ';	
		}
		szFreqStr[idx_w++] = szTempStr[i];
		digit--;
	}

	szFreqStr[idx_w++] = '.';

	sprintf(szTempStr, "%.8f\0", fmod(dValue, 1.0));

	digit = (below_dec_point > MAX_FLOAT_PRECISION) ? MAX_FLOAT_PRECISION : below_dec_point;

	for(i=0; i<digit; i++)
	{
		if( (i>0) && ((i%3)==0) )
		{
			szFreqStr[idx_w++] = ' ';	
		}
		szFreqStr[idx_w++] = szTempStr[2+i];
	}

	szFreqStr[idx_w++] = 0;

//	DBGMSG(0, "%s: dValue=%.8f, upper_dec_point=%d, below_dec_point=%d -> [%s]\r\n",
//					__func__, dValue, upper_dec_point, below_dec_point, szFreqStr);

	return szFreqStr;
}

char* ConvFloatFormatStrEx(double dValue, u8 upper_dec_point, u8 below_dec_point, u8 round_point)
{
	char szTempStr[MAX_FLOAT_PRECISION+1] = {0,};
	static char szFreqStr[MAX_LCD_COL+1] = {0,};	//MAX_LCD_COL=16
	u32 ulValue;
	u8 i, idx_r=0, idx_w=0, digit;
	double dTemp = dValue;

	if(dValue < 0) dValue = 0.0; 	//minus not support
	if(upper_dec_point > MAX_FLOAT_PRECISION) upper_dec_point = MAX_FLOAT_PRECISION;
	if(below_dec_point > MAX_FLOAT_PRECISION) below_dec_point = MAX_FLOAT_PRECISION;
	if(round_point > (upper_dec_point+below_dec_point)) round_point = 0;
	if(round_point == 1) round_point = 2;

//	DBGMSG(0, "Conv: %.8f, point=%d : ", dValue, round_point);

	if(round_point <= upper_dec_point)
	{
		ulValue = (u32)(dValue / pow(10, upper_dec_point - round_point + 1) + 0.5);
		dValue = ulValue * pow(10, upper_dec_point - round_point + 1);
	}
	else
	{
		ulValue = (u32)(dValue * pow(10, round_point - upper_dec_point - 1) + 0.5);
		dValue = ulValue / pow(10, round_point - upper_dec_point - 1);
	}

//	DBGMSG(0, "%lu, %.8f\r\n", ulValue, dValue);

	ulValue = (u32)(float)(dValue);
	sprintf(szTempStr, "%08lu\0", ulValue);

	idx_r = MAX_FLOAT_PRECISION - upper_dec_point;
	digit = upper_dec_point;

	if(round_point <= upper_dec_point)
	{
		memset(&szTempStr[idx_r+round_point-1], '-', upper_dec_point - round_point + 1);
	}

	for(i=idx_r; i<MAX_FLOAT_PRECISION; i++)
	{
		if( (digit != upper_dec_point) && ((digit%3)==0) )
		{
			szFreqStr[idx_w++] = ' ';	
		}
		szFreqStr[idx_w++] = szTempStr[i];
		digit--;
	}

//	DBGMSG(0, "1) szTempStr=%s, szFreqStr=%s\r\n", szTempStr, szFreqStr);

	szFreqStr[idx_w++] = '.';

	sprintf(szTempStr, "%.8f\0", fmod(dValue, 1.0));

	digit = (below_dec_point > MAX_FLOAT_PRECISION) ? MAX_FLOAT_PRECISION : below_dec_point;

	if(round_point <= upper_dec_point)
	{
		memset(&szTempStr[2], '-', digit);
	}
	else
	{
		memset(&szTempStr[2 + round_point - upper_dec_point - 1], '-', digit - (round_point - upper_dec_point - 1));
	}

	for(i=0; i<digit; i++)
	{
		if( (i>0) && ((i%3)==0) )
		{
			szFreqStr[idx_w++] = ' ';	
		}
		szFreqStr[idx_w++] = szTempStr[2+i];
	}

	szFreqStr[idx_w++] = 0;

//	DBGMSG(0, "2) szTempStr=%s, szFreqStr=%s\r\n", szTempStr, szFreqStr);

//	DBGMSG(0, "%s: dValue=%.8f, upper_dec_point=%d, below_dec_point=%d -> [%s]\r\n",
//					__func__, dValue, upper_dec_point, below_dec_point, szFreqStr);

	return szFreqStr;
}

#if 0
#define MAX_LONG_DIGIT 		11
char* ConvLongFormatStr(u32 ulValueHigh, u32 ulValueLow, u8 ucDigit)
{
	char szTempStr[MAX_LONG_DIGIT+1] = {0,};
	static char szFreqStr[MAX_LCD_COL+1] = {0,};
	u8 i, idx_r=0, idx_w=0, digit;

	if(ucDigit > MAX_LONG_DIGIT) ucDigit = MAX_LONG_DIGIT;

	sprintf(&szTempStr[0], "%02lu\0", ulValueHigh%100);
	sprintf(&szTempStr[2], "%09lu\0", ulValueLow);

	idx_r = MAX_LONG_DIGIT - ucDigit;
	digit = ucDigit;

	for(i=idx_r; i<MAX_LONG_DIGIT; i++)
	{
		if( (digit != ucDigit) && ((digit%3)==0) )
		{
			szFreqStr[idx_w++] = ' ';	
		}
		szFreqStr[idx_w++] = szTempStr[i];
		digit--;
	}

	szFreqStr[idx_w++] = 0;

//	DBGMSG(0, "%s: ulValueHigh=%lu ulValueLow=%lu, ucDigit=%d -> [%s]\r\n", 
//				__func__, ulValueHigh, ulValueLow, ucDigit, szFreqStr);

	return szFreqStr;
}
#else
#define MAX_LONG_DIGIT 		11
char* ConvLongFormatStr(u32 ulValue, u8 ucDigit)
{
	char szTempStr[MAX_LONG_DIGIT+1] = {0,};
	static char szFreqStr[MAX_LCD_COL+1] = {0,};
	u8 i, idx_r=0, idx_w=0, digit;

	if(ucDigit > MAX_LONG_DIGIT) ucDigit = MAX_LONG_DIGIT;

	szTempStr[0] = '0';
	sprintf(&szTempStr[1], "%010lu\0", ulValue);

	idx_r = MAX_LONG_DIGIT - ucDigit;
	digit = ucDigit;

	for(i=idx_r; i<MAX_LONG_DIGIT; i++)
	{
		if( (digit != ucDigit) && ((digit%3)==0) )
		{
			szFreqStr[idx_w++] = ' ';	
		}
		szFreqStr[idx_w++] = szTempStr[i];
		digit--;
	}

	szFreqStr[idx_w++] = 0;

//	DBGMSG(0, "%s: ulValueHigh=%lu ulValueLow=%lu, ucDigit=%d -> [%s]\r\n", 
//				__func__, ulValueHigh, ulValueLow, ucDigit, szFreqStr);

	return szFreqStr;
}
#endif
#endif
