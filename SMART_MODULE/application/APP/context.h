/*

*/
#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#define CHK_TASK_EN(t)		(CTX_TASK(ulTaskEnableFlag) &  (BIT_FLAG(t)))
#define SET_TASK_EN(t)		(CTX_TASK(ulTaskEnableFlag) |= (BIT_FLAG(t)))
#define CLR_TASK_EN(t)		(CTX_TASK(ulTaskEnableFlag) &= ~(BIT_FLAG(t)))

//
// Sub Context Definition
//
typedef __PACKED_STRUCT_PRE _SUB_CONTEXT
{
	u8 		ucState;
	u8 		ucStateStep;
	u8		ucMode;
	u8 		ucModeStep;
	u8 		ucProcStep;
	u8 		ucFlag;
	u8 		ucRetry;
	u8 		reserved[1];
	u32 	ulTick;
} __PACKED_STRUCT_LAST SUB_CONTEXT, *PSUB_CONTEXT;	//size=12

#define STATE_FLAG_MASK					(0x07)
#define STATE_FLAG_WAIT					(1<<0)
#define STATE_FLAG_RETRY				(1<<1)
#define STATE_FLAG_DELAY				(1<<2)


//
// Main Context Definition
//
typedef __PACKED_STRUCT_PRE _MAIN_CONTEXT
{
	u8 					ucResetStatus; 			//ResetStatus from RCC->CSR
	u8 					ucDebugMode; 			//0=DebugMessage, 1=PC Interface
	u8					facMode;
	BOOL 				isSdMounted; 			//
	u8					facSeq;
	u8 					pwrOnSrc;
} __PACKED_STRUCT_LAST MAIN_CONTEXT, *PMAIN_CONTEXT;	//size=4

typedef __PACKED_STRUCT_PRE _BTWIFI_CONTEXT
{
	BOOL 				isHandShake;
	BOOL 				isInit;
} __PACKED_STRUCT_LAST BTWIFI_CONTEXT, *PBTWIFI_CONTEXT;	// size=4

typedef __PACKED_STRUCT_PRE _MODEM_CONTEXT
{
	BOOL 				isHandShake;
	BOOL 				isInit;
	BOOL 				isConnect;
	u8 					rsv0;
	int					ucSockHandle;
} __PACKED_STRUCT_LAST MODEM_CONTEXT, *PMODEM_CONTEXT;

typedef __PACKED_STRUCT_PRE _WIFI_CONTEXT
{

	BOOL 				isHandShake;
	BOOL 				isInit;
	BOOL 				isScanFound;
	BOOL 				isJoin;
	BOOL 				isConnect;
	int					ucSockHandle;
	u8 					reserved[2];
} __PACKED_STRUCT_LAST WIFI_CONTEXT, *PWIFI_CONTEXT;

typedef __PACKED_STRUCT_PRE _BT_CONTEXT
{
	BOOL 				isCommandMode;
	BOOL				isScanFound;
	BOOL 				isConnect;
	u8 					reserved1[1];
	u8 					btAddrObd[13+1];
	u8 					btAddrAdmin[13+1];
	u8 					reserved2[2];
} __PACKED_STRUCT_LAST BT_CONTEXT, *PBT_CONTEXT;

//
// I/O Context Definition
//
typedef __PACKED_STRUCT_PRE _IO_CONTEXT
{
	u16 				usAdKeyPower;
	u16 				usAdKey;
	u16 				usAdKeyExt;
	u8					ucKeyFlag;
	BOOL 				isKeyPressFromPOR;
	u8 					ucDevStatus;
	u8 					reserved[3];
} __PACKED_STRUCT_LAST IO_CONTEXT, *PIO_CONTEXT;	//size=12

//
// Adc Context Definition
//
typedef __PACKED_STRUCT_PRE _ADC_CONTEXT
{
	//ADC Part
	s32 				lBattSum;
	s32 				lBattAvg;
	s32 				lCpuTempSum;
	s32 				lCpuTempAvg;
	u8 					ucAdcSampleCount;
	u8 					reserved[3];
} __PACKED_STRUCT_LAST ADC_CONTEXT, *PADC_CONTEXT;		//size=20

//
// Admin Context Definition
//
typedef __PACKED_STRUCT_PRE _ADMIN_CONTEXT
{
	BOOL 				isAdminWatchdogReset;
	u8 					reserved[3];
} __PACKED_STRUCT_LAST ADMIN_CONTEXT, *PADMIN_CONTEXT;	//size=4

//
// Global Context Definition
//
typedef __PACKED_STRUCT_PRE _GLOBAL_CONTEXT
{
	u32 				reserved;
} __PACKED_STRUCT_LAST GLOBAL_CONTEXT, *PGLOBAL_CONTEXT;	//size=4

//< for PC Protocol, 4 Bytes Aligned Order (element order different communication structure)
typedef __PACKED_STRUCT_PRE _RESULT_DATA
{
	s32 				num;
	double 				startSpeed;
	double 				endSpeed;
	double 				avgSpeed;
	double 				distance;
	double 				orientedAngle;
	double 				deviationAngle;
	double 				deviationMeter;
} __PACKED_STRUCT_LAST RESULT_DATA, *PRESULT_DATA; 		//size=60

typedef __PACKED_STRUCT_PRE _CURRENT_DATA
{
	s8					szTime[12];
	u8 					svNum;
	u8 					fixedNum;
	u8 					startTriggerFlag; 	//ucTrigger: Off, On
	u8 					orientAngleFlag; 	//ucOrient:  Off, On
	double 				lat;
	double 				lon;
	double 				vel; 				//speed
	double 				headingAngle;
	double 				orientAngle;
	double 				centerDeviation;
	double 				distance;
	s16 				steeringAngle;
	s16 				steeringToque;
//  u8 					steeringSpeed;
	u8 					tirePressureUnit;
	u8 					tirePressureFL;
	u8 					tirePressureFR;
	u8 					tirePressureRL;
	u8 					tirePressureRR;
	u8 					reserved[3];
} __PACKED_STRUCT_LAST CURRENT_DATA, *PCURRENT_DATA; 	//size=84
//> 

typedef __PACKED_STRUCT_PRE _GPS_CONTEXT
{
	//==========================>
	// Message ID $PTNL,AVR
	uint32_t		utc;		//UTC of vector fix,  except .ss
	float			yaw;		//yaw angle in degrees	
	float			tilt;		//tilt angle in degrees
	double			range;		//Range in meters
	float			pdop;
	u8				fix_type;	//0: fix not available or invalid, 1: autonomous GPS fix, 2: differential carrier phase solution RTK(Float), 3: differential carrier phase solution RTK(Fix), 4: differential code-based solution, DGPS
	u8				inuse_sat;
	u8				rsvd[2];
	// Message ID $PTNL,AVR
	//<========================= 28 bytes

	//==========================>
	// Message ID $RMC
	double			latitude;
	double			longitude;
	float			speed;		//Speed over the ground in knots
	float			track;		//track angle in degrees
	uint32_t		date;
	float			magnetic;	//Magnetic variation in degrees
	// Message ID $RMC
	//<========================= 32 bytes
} __PACKED_STRUCT_LAST GPS_CONTEXT, *PGPS_CONTEXT;	//size=60

typedef __PACKED_STRUCT_PRE _EXF_CONTEXT
{
	uint32_t		wp;		//write-pointer in a sector of External Flash, based on 32bytes unit
	uint32_t		wp_idx;	//the index of write-pointer array on Internal Flash
	uint32_t		rp;		//read-pointer  in a sector of External Flash, based on 32bytes unit
	uint32_t		rp_idx;	//the index of read-pointer array on Internal Flash
	uint32_t		wn;		//work number
	uint32_t		wn_idx;	//the index of work number array on Internal Flash
} __PACKED_STRUCT_LAST EXF_CONTEXT, *PEXF_CONTEXT;	//size=12

//
// Main Context Definition
//
typedef __PACKED_STRUCT_PRE _CONTEXT
{
	//Flash ===================================>
	CONFIG_INFO			Config;					// 512
	//Sram  ===================================>
	u8					mcuId[12];				//12
	MAIN_CONTEXT 		Main; 					// 6
	EXF_CONTEXT			Exf;					// 12
} __PACKED_STRUCT_LAST CONTEXT, *PCONTEXT;		//size=542

//
// Macro Definition
//

#define CTX(member)							pgContext->member

// Macro's for Config
#define GET_CONFIG_CONTEXT()				&pgContext->Config
#define CFG_HDR(member)						pgContext->Config.hdr.member
#define CFG(member)							pgContext->Config.member

// macro's for Main Context
#define GET_MAIN_CONTEXT()					&pgContext->Main
#define CTX_MAIN(member)					pgContext->Main.member

// macro's for GPS Context
#define GET_EXF_CONTEXT()					&pgContext->Exf
#define CTX_EXF(member)						pgContext->Exf.member

//
// Context Global
//
extern PCONTEXT		pgContext;

#endif //__CONTEXT_H__
