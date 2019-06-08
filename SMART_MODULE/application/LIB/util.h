/*

*/
#ifndef __UTIL_H__
#define __UTIL_H__

#define SECONDS_OF_DAY					86400		//1��=86400��
#define MINUTES_OF_DAY					1440		//1��=14��0��
#define KST_OFFSET						32400		//9�ð�=32400

//
// Flag Manipulation Macro
//
#define SET_FLAG(v, b)					(v |= (b))
#define CLR_FLAG(v, b)					(v &= ~(b))
#define CHK_FLAG(v, b)					((v & (b))==(b))
#define MSK_FLAG(v, b)					(v & (b))
#define BIT_FLAG(b)						(1<<(b))

//
// Nibble Manipulation Macro
//
#define LOW_NIBBLE(v) 					((v)&0xf)
#define HIGH_NIBBLE(v) 					(((v)&0xf0)>>4)
#define MEKE_HIGH_NIBBLE(v) 			(((v)&0xf)<<4)
//
// Convert Constant
//
#define KNOTS_TO_KM						(1.852)
#define MILE_TO_KM						(1.609344)

#define DEG2RAD							( 0.0174532925)		//PI/180	[0.01745329251]994329576923690768489
#define RAD2DEG							(57.2957795130)		//180/PI	[57.2957795130]82320876798154814105

//간략계산을 위한 상수
#define LAT_PER_METER					90					//0.00000901082, 미터당 위도 증감
#define LON_PER_METER					112					//0.00001123453, 미터당 경도 증감

//
// Command Handler Map
//
typedef s16 (*PFN_CMD_HANDLER)(u8* pCommand);
typedef __PACKED_STRUCT_PRE _CMD_MAP
{
	u32						ulCommandHash;
	PFN_CMD_HANDLER			pfnHandler;
} __PACKED_STRUCT_LAST CMD_MAP;

#define SET_CMD_TABLE(tbl,idx,cmd,func)		tbl[idx].ulCommandHash = STR2HASH(cmd, strlen(cmd)); \
											tbl[idx].pfnHandler	 = func
#define RUN_CMD_HANDLER(tbl,idx, param)		tbl[idx].pfnHandler(param)
#define GET_CMD_HASH(tbl,idx)				tbl[idx].ulCommandHash

//
// Export Variable
//
extern u8 g_days[12];

//
// Common Function Prototype
//
u8 BIN2BCD(u8 bin_value);
u8 BCD2BIN(u8 bcd_value);

void BIN2HEX(u8* pBin, u8* pHex, u16 usLenBin);
void HEX2BIN(u8* pHex, u8* pBin, u16 usLenHex);

u32 BIN2DEC(u8* pBin, u16 usLenBin);
void DEC2BIN(u32 ulDec, u8* pBin, u16 usLenDec);

u32 STR2HASH(const s8* pstr, u16 size);
u16 GetCmdHandlerIndex(CMD_MAP* pMap, u16 usMapCount, s8* buffer);

BOOL isLeapYear(u16 year);
u8 GetDayOfWeek(PDATETIME pDateTime);

void ConvTimeStampToDateTime(u32 ulTimeStamp, PDATETIME pDateTime);
u32 ConvDateTimeToTimeStamp(PDATETIME pDateTime);
void ConvGpsDateTime(u32 ulDateUtc, u32 ulTimeUtc, PDATETIME pDateTime, BOOL isConvKST);
char* GetTimeStringFromTimeStamp(u32 ulTimeStamp);

u32 GpsTimeDiffInt(u32 ulTime1, u32 ulTime2);
double GpsTimeDiffDouble(double dTime1, double dTime2);

s32 Conv_DMM2DDD_long(double dmm);
s32 Conv_DDD2DMM_long(double ddd);

double Conv_DMM2DDD_double(double dmm);
double Conv_DDD2DMM_double(double ddd);

double CalcGpsDistance(s32 lat1, s32 lon1, s32 lat2, s32 lon2);
u32 CalcGpsDistanceSimple(s32 lat1, s32 lon1, s32 lat2, s32 lon2);
BOOL CheckGpsRegion(s32 latGps, s32 lonGps, s32 latChk, s32 lonChk, u16 radChk);

u16 GetByteCheckSum(u16 cs_init, u8* buffer, u32 len);
u8  GetByteXor(u8 cs_init, u8* buffer, u32 len);
u16 GetFletcherCheckSum(u16 cs_init, u8* buffer, u32 len);

u32 ConvIpStr2Inet(u8 *pIpStr);
char* ConvInet2IpStr(u32 ulInet);

void ByteOrderChange(void* pDest, void* pSrc, u8 size);

float VSQRT(float arg);

#if 0
char* ConvFloatFormatStr(double fValue, u8 upper_dec_point, u8 below_dec_point);
char* ConvFloatFormatStrEx(double fValue, u8 upper_dec_point, u8 below_dec_point, u8 round_point);
#if 0
char* ConvLongFormatStr(u32 ulValueHigh, u32 ulValueLow, u8 digit);
#else
char* ConvLongFormatStr(u32 ulValue, u8 digit);
#endif
#endif

#endif //__UTIL_H__
