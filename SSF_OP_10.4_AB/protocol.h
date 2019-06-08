#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "iodefine.h"   //(120901)

/// OP -> M/C
//############################################
//############################################
/// Level 1
#define K_BOOT_ON           0xF8
#define K_CHK_NUM           0x01
#define MAIN_MODE           0x02
#define LOAD_MODE           0x03
#define MODE_MENU           0x04
#define K_INSTALL           0x05
#define TEST_MODE           0x06
#define MODE_PUNCH          0x07
#define K_GETPARA           0xA0

/// Level 2
#define K_J_INIT            0x0A
#define K_J_SETUP           0x00
#define K_J_READY           0x01
#define K_J_START           0x02
#define K_J_ERROR           0x03
#define K_HEADER            0x04
#define K_HEADER_STI        0x05
#define K_INS_FORMAT        0x06    //(121021)
#define K_INS_UPDATE        0x07    //(121021)
#define K_INS_VER_EXCH      0x08    //(121021)
#define K_J_WARN            0x09    //(130226)
#define K_CHK_NUM1          0x0F    //(130227)


/// Level 3
#define K_MENU              0x00
#define K_NEXT_PAGE         0x01
#define K_NETWORK           0x02
#define K_DESIGN_NO         0x03
#define K_INFO              0x04
#define K_SPEED_UP          0x05
#define K_SPEED_DN          0x06
#define K_FORWARD           0x07
#define K_BACKWARD          0x08
#define K_ORIGIN        	0x09
#define K_THREAD        	0x0A
#define K_MOV_UP        	0x0B
#define K_MOV_DN        	0x0C
#define K_MOV_LEFT      	0x0D
#define K_MOV_RIGHT     	0x0E
#define K_EXPAND        	0x0F
#define K_REDUCE        	0x10
#define K_PRE_PAGE      	0x11
#define K_B_COUNT       	0x12
#define K_P_COUNT           0x13
#define K_X_SCALE       	0x14
#define K_Y_SCALE       	0x15
#define K_BOBBIN        	0x16
#define K_SPEED             0x17
#define K_TENSION           0x18
#define K_PTRN_NO           0x19
#define K_RELEASE           0x1A    //0x20	//(121005)
#define K_P_CHAIN_MODE      0x1B    //0x21	//(121008)
#define K_MD_PRJ_MEM        0x1C    //0x22	//(130221)
#define K_MENU1             0x1D    //(130315)


//----- Level 4 -----
#define K_M_INIT            0x01
#define K_M_PARA            0x02
#define K_M_TEST            0x03
#define K_M_DESIGN          0x04
//#define K_P_CHAIN           0x40
#define K_PTRN_DATA         0xA0	//(121005)
#define K_PTRN_DELETE		0xA1	//(121014)
#define K_M_BOBBIN			0x16	//(121109)


/// Level 5
#define K_D_MOV_UP          0x00
#define K_D_MOV_LEFT        0x01
#define K_D_MOV_DN          0x02
#define K_D_MOV_RIGHT       0x03
#define K_D_EXPAND          0x04
#define K_D_REDUCE          0x05
#define K_D_DEL             0x06
#define K_I_VERSION         0x07
#define K_I_PARA_INIT       0x08
#define K_I_UPDATE          0x09
#define K_I_PARA_RW         0x0A
#define K_P_TRIM            0x0B
#define K_P_PF              0x0C
#define K_P_WP              0x0D
#define K_P_CLAMP           0x0E
#define K_P_SPEED           0x0F
#define K_P_THREAD          0x10
#define K_P_COOL            0x11
#define K_P_PEDAL           0x12
#define K_P_HOOK            0x13
#define K_P_HEAD            0x14
#define K_P_SAFETY          0x15
//#define K_P_UPSTOP_POS      0x16
#define K_T_X_Y             0x17
#define K_T_AREA            0x18
#define K_T_DESIGN          0x19
#define K_T_ETC_FUNC        0x1A
#define K_T_ENCODER         0x1B
#define K_T_XY_MAIN         0x1C
#define K_T_MAIN_MOTOR      0x1D
#define K_T_XY_JOG          0x1E
#define K_T_PF_JOG          0x1F
#define K_T_TURN_HEAD       0x20
#define K_T_OUTPUT          0x21
#define K_T_INPUT           0x22
#define K_T_KEY             0x23    //(120411)
#define K_FUNC_CODE         0x24	//(120508)
#define K_JUMP          	0x25	//(120508)
#define K_LINE              0x26	//(120508)
#define K_CURVE             0x27	//(120508)
#define K_TRIM              0x28	//(120508)
#define K_PTRN_DEL          0x29	//(120508)
#define K_DISK_READ         0x2A	//(120508)
#define K_DISK_WRITE		0x2B	//(120508)
#define K_TEST              0x2C	//(120508)
#define K_T_SOL             0x2D	//(120929)
#define K_T_THREADSOL		0x2E	//(121003)
#define K_POINT_SET         0x2F    //(121215)
#define K_EXE               0x30    //(121215)
#define K_EXIT              0x31    //(130607)
#define K_VAL_SEND          0x32    //(130731)
#define K_D_MOV_LUP         0x33    //(130822)
#define K_D_MOV_LDN         0x34    //(130822)
#define K_D_MOV_RUP         0x35    //(130822)
#define K_D_MOV_RDN         0x36    //(130822)

//#define K_P_CHAIN_NO		0x40
//#define K_CHAIN_VAL		0x41	//(121014)
#define K_P_CHAIN			0x40	//(121008)

// Level 6
#define K_READ              0x00
#define K_WRITE             0x01

#define K_P_CHAIN_NO		0x40	//(121014)
#define K_P_CHAIN_VAL		0x41	//(121014)
#define K_P_CHAIN_NUM		0x42	//(121014)

// Parameter -------------------------
#if 1
//-----  -----(120102)
#define K_SET_JOG           0x00
#define K_JOG_MODE          0x01
#define K_MC_ORG_MODE_1     0x02
#define K_MC_ORG_MODE_2     0x03
#define K_RETURN_MODE       0x04
#define K_SET_BOBBIN_CNT    0x05
//#define 	0x06
#define K_READ_MODE			0x07
#define K_SET_TRIM_AFT_STOP	0x08	//(120110)
#define K_SLOW_START        0x09
#define User_Slow_1         0x0A
#define User_Slow_2         0x0B
#define User_Slow_3         0x0C
#define User_Slow_4         0x0D
#define User_Slow_5         0x0E
#define User_Slow_6         0x0F
#define User_Slow_7         0x10
#define User_Slow_8         0x11
#define User_Slow_9         0x12
#define User_Slow_10        0x13
#define K_MAX_SPEED         0x14
#define K_FEED_POS          0x15
#define K_FF_OPERATION      0x16    //K_FEED_OPERATION	0x16
#define K_SET_DOWN          0x17
#define K_PEDAL_MODE_1		0x18
#define K_PEDAL_MODE_2  	0x19
#define K_PF_OPERATION  	0x1A
//#define K_PAUSE             0x1B // add
#define K_SET_WIPER         0x1C
#define K_CONTROL_HEAD      0x1D // add
#define K_THREAD_DETECT     0x1E
#define K_DETECT_STI_1  	0x1F
#define K_DETECT_STI_2  	0x20
#define K_SET_TRIM			0x21		//(120110)
//#define K_OIL_ON_TM         0x22 // add
//#define K_OIL_OFF_TM        0x23 // add
//#define K_SET_NEEDLE_COOL	0x24 // add
//#define K_COOL_ON_TM    	0x25 // add
//#define K_COOL_OFF_TM   	0x26 // add
//#define K_SET_PF_MOTOR  	0x27 // add
//#define K_HOOK_ORG_MODE     0x28 // add
//#define K_DESIGN_OPEN_MODE  0x29
//#define K_START_ANGLE       0x2A // add
//#define K_TRIM_SPM      	0x2B // add
//#define K_READ_MODE         0x2C // add
//#define K_THREAD_HOLD       0x2D // add
#define K_PF_FULL_ON_TIME	0x2E
#define K_FF_FULL_ON_TIME	0x2F
#define K_TT_FULL_ON_TIME	0x30
#define K_TH_FULL_ON_TIME	0x31
#define K_WP_FULL_ON_TIME	0x32
//#define K_THREAD_HOLD_TM	0x33
//#define K_UPSTOPPOS     	0x34 // add
//#define 	0x35
#define K_PF_DUTY           0x36
#define K_FF_DUTY       	0x37
#define K_TT_DUTY           0x38
#define K_TH_DUTY       	0x39
#define K_WP_DUTY       	0x3A
//#define 	0x3B
//#define 	0x3C
//#define 	0x3D
#define K_DESIGN_OPEN_MODE	0x3E	// para 062
#define K_SCALE_MODE		0x3F    //(120905)
#define K_CHAIN_NO          0x40
#define K_CHAIN_MODE		0x41
#define K_CHAIN_CLAMP		0x42
#define K_DECEL_STI 		0x43
#define K_TRIM_SPM  		0x44	//(120522)
#define K_TRIM_DELAY     	0x45
#define K_LOW_AIR       	0x46
#define K_FF_NO             0x47
#define K_FF_PAUSE_CTRL		0x48
#define K_THRDHOLD_EN       0x49    //73
//#define 	0x4A
//#define 	0x4B
#define K_SCALE_REF         0x4C
//#define         0x4D // add
#define K_SET_LIMIT         0x4E
#define K_X_P_LIMIT         0x4F
#define K_X_M_LIMIT         0x50
#define K_Y_P_LIMIT         0x51
#define K_Y_M_LIMIT         0x52
//#define 	0x53
#define K_REVERSE_AFT_TRIM  0x54
#define K_REVERSE_ANGLE     0x55
//#define 	0x56
//#define 	0x57
#define K_SAFETY_MODE       0x58
#define K_JUMP_SPEED        0x59
#define K_STOP_JUMP_EM      0x5A
//#define 	0x5B
//#define 	0x5C
#define K_SET_CLAMP         0x5D
#define K_CLAMP_DATA        0x5E
#define K_SET_PF            0x5F
#define K_CLAMP_RANGE       0x60
#define K_PF_RANGE          0x61
#define K_AUTO_CALL         0x62
#define K_AUTO_READY        0x63
#define K_SET_ATTACH        0x64
#define K_AUTO_CALL_TM      0x65
#define K_SET_AUTO_NO       0x66
#define K_SET_EX_BOARD      0x67
//#define K_TRIM_SPM          0x68
//#define K_TRIM_DELAY        0x69

#if(SPS_5050+SPS_8050+SPS_80100+SPS_50200)
    #define K_HOOK_ORG_MODE         0x69 	// 105
    //unsigned char head_dis_en;    		// 106
    #define	K_OIL_ON_TM          	0x6B	// 107
    #define	K_OIL_OFF_TM			0x6C   	// 108
    #define K_THREAD_HOLD           0x6D 	// 109
    #define K_THREAD_HOLD_TM        0x6E    // 110
    #define	K_SET_NEEDLE_COOL		0x6F    // 111
    #define K_COOL_ON_TM			0x70	// 112
    #define K_COOL_OFF_TM			0x71	// 113
    //unsigned char SetTrimmer;          	// 114
    #define K_SET_HEAD_SENS     	0x73	// 115
    //unsigned char ClampSensor;           	// 116
    #define K_BCODE_ORG         	0x75	// 117
    #define K_BCODE_TYPE        	0x76	// 118
    #define K_START_ANGLE         	0x77	// 119
    //unsigned char TrimHoldAngle;      	// 120
    #define K_SET_PF_MTR			0x79	// 121
    #define K_JUMP_TENSION      	0x7A	// 122
    #define K_PROGRAM_LOCK      	0x7B	// 123
    #define K_POSITION_REF          0x7C    // 124
    #define K_SET_AIRBAG			0x7D	// 125
    //unsigned char bobbin_num;             // 126
    #ifdef THREADBOARD				//(121121)
        #define K_THRD_BRD_EN		0x7F	// 127
        #define K_THRD_SOL_DUTY     0x80	// 128
        //#define R_ThreadSol2_duty;	//0x81	// 129
        #define K_PARA_UPSTOPPOS	0x82	// 130 UpStop position(UDCUpStop)
    #else
        #define K_PARA_UPSTOPPOS	0x7F	// 127 UpStop position(UDCUpStop)
    #endif

#elif(HT_SPS_3030+HT_SPS_5050)
    #define K_HOOK_ORG_MODE         0x69 	// 105
    //unsigned char head_dis_en;    		// 106
    #define	K_OIL_ON_TM          	0x6B	// 107
    #define	K_OIL_OFF_TM			0x6C   	// 108
    #define K_THREAD_HOLD           0x6D 	// 109
    #define K_THREAD_HOLD_TM        0x6E    // 110
    #define	K_SET_NEEDLE_COOL		0x6F    // 111
    #define K_COOL_ON_TM			0x70	// 112
    #define K_COOL_OFF_TM			0x71	// 113
    //unsigned char SetTrimmer;          	// 114
    #define K_SET_HEAD_SENS     	0x73	// 115
    //unsigned char ClampSensor;           	// 116
    #define K_BCODE_ORG         	0x75	// 117
    #define K_BCODE_TYPE        	0x76	// 118
    #define K_START_ANGLE         	0x77	// 119
    //unsigned char TrimHoldAngle;      	// 120
    #define K_SET_PF_MTR			0x79	// 121
    #define K_JUMP_TENSION      	0x7A	// 122
    #define K_PROGRAM_LOCK      	0x7B	// 123
    #define K_POSITION_REF          0x7C    // 124
    #define K_SET_AIRBAG			0x7D	// 125
    //unsigned char bobbin_num;             // 126
    //int	      UDCUpStop;				// 127 UpStop position(UDCUpStop)
    #ifdef THREADBOARD				//(121121)
        #define K_THRD_BRD_EN		0x7F	// 127
        #define K_THRD_SOL_DUTY     0x80	// 128
        //#define R_ThreadSol2_duty;	//0x81	// 129
        #define K_PARA_UPSTOPPOS	0x82	// 130 UpStop position(UDCUpStop)
    #else
        #define K_PARA_UPSTOPPOS	0x7F	// 127 UpStop position(UDCUpStop)
    #endif
#else			//SmallPatternMC
    #define K_BCODE_ORG             0x6A
    #define K_BCODE_TYPE            0x6B
    #define K_JUMP_TENSION          0x6C
    #define K_PROGRAM_LOCK          0x6D
    #define K_POSITION_REF          0x6E    // 110
    //unsigned char TrAirType;          	// 111
    #define K_SET_AIRBAG            0x70	// 112
    #define K_START_ANGLE         	0x71	// 113 (120522)
    //unsigned char bobbin_num;     0x72    // 114
    #if SPS_1507                    		//(121210)
        #define	K_CLAMP_DEV     	0x73    //115
        #define	K_CLAMP_REV_PUNCH	0x74    //116
        #ifdef THREADBOARD
            #define K_THRD_BRD_EN		0x75	// 117
            #define K_THRD_SOL_DUTY     0x76	// 118
            //#define R_ThreadSol2_duty;0x77    // 119
            #define K_PARA_UPSTOPPOS	0x78	// 120 UpStop position(UDCUpStop)
        #else
            #define K_PARA_UPSTOPPOS	0x75	// 117 UpStop position(UDCUpStop)
        #endif
    #elif(SPS_3020+SPS_2516)
        #define K_USE_POCKET		0x73	// 115 (F3020)		//09.08.31
        //#define R_PocketBacktack;			// 116 (F3020)
        //#define R_BarTackSpeed;			// 117 (F3020)
        //#define R_Set_Inner_Pos_Fix;		// 118 (F3020)
        #ifdef THREADBOARD
            #define K_THRD_BRD_EN		0x77	// 119
            #define K_THRD_SOL_DUTY     0x78	//
            //#define R_ThreadSol2_duty;        // 121(120406)
            #define K_PARA_UPSTOPPOS	0x7A	// 122 UpStop position(UDCUpStop)
        #else
            #define K_PARA_UPSTOPPOS	0x77	// 119 UpStop position(UDCUpStop)
        #endif
    #else
        #ifdef THREADBOARD
            #define K_THRD_BRD_EN		0x73	// 115
            #define K_THRD_SOL_DUTY     0x74	// 116(110915)
            //#define R_ThreadSol2_duty;        // 117(111013)
            #define K_PARA_UPSTOPPOS	0x76	// 126 UpStop position(UDCUpStop)
        #else
            #define K_PARA_UPSTOPPOS	0x73	// 115 UpStop position(UDCUpStop)
        #endif
    #endif
#endif


#endif

//-----------------------------------
#define K_T_SPM			0x53
#define K_T_X_WIDTH		0x54
#define K_T_Y_WIDTH		0x55
#define K_T_START		0x56
#define K_T_STOP		0x57
#define K_T_SPEED_UP	0x58
#define K_T_SPEED_DN	0x59
#define K_T_TRIM		0x5A
#define K_T_LT			0x5B
#define K_T_T			0x5C
#define K_T_RT			0x5D
#define K_T_L			0x5E
#define K_T_R			0x5F
#define K_T_LB			0x60
#define K_T_B			0x61
#define K_T_RB			0x62
#define K_T_ORIGIN		0x63
#define K_T_PF_UP		0x64
#define K_T_PF_DN		0x65
#define K_T_PF_ORG		0x66
#define K_T_TH_L		0x67
#define K_T_TH_R		0x68
#define K_T_PF_UPDN		0x69
#define K_T_OP00		0x6A
#define K_T_OP01		0x6B
#define K_T_OP02		0x6C
#define K_T_OP03		0x6D
#define K_T_OP04		0x6E
#define K_T_OP05		0x6F
#define K_T_OP06		0x70
#define K_T_OP07		0x71
#define K_T_OP08		0x72
#define K_T_OP09		0x73
#define K_T_OP10		0x74
#define K_T_OP11		0x75
#define K_T_IP00		0x76
#define K_T_IP01		0x77
#define K_T_IP02		0x78
#define K_T_IP03		0x79
#define K_T_IP04		0x7A
#define K_T_IP05		0x7B
#define K_T_IP06		0x7C
#define K_T_IP07		0x7D
#define K_T_IP08		0x7E
#define K_T_IP09		0x7F
#define K_T_IP10		0x80
#define K_T_IP11		0x81

/// level 7
#define K_0     		0x00
#define K_1         	0x01
#define K_2         	0x02
#define K_3         	0x03
#define K_4         	0x04
#define K_5         	0x05
#define K_6         	0x06
#define K_7         	0x07
#define K_8         	0x08
#define K_9         	0x09
#define K_10			0x0A
#define K_11			0x0B
#define K_12			0x0C
#define K_13			0x0D
#define K_14			0x0E
#define K_15			0x0F
//	0x10
//	0x11
//	0x12
//	0x13
#define K_BIT_0			0x14
#define K_BIT_1			0x15
#define K_BIT_2			0x16
#define K_BIT_3			0x17
#define K_BIT_4			0x18
#define K_BIT_5			0x19
#define K_BIT_6			0x1A
#define K_BIT_7			0x1B

// COMMON Command
#define K_NO_0			0xE0
#define K_NO_1			0xE1
#define K_NO_2			0xE2
#define K_NO_3			0xE3
#define K_NO_4			0xE4
#define K_NO_5			0xE5
#define K_NO_6			0xE6
#define K_NO_7			0xE7
#define K_NO_8			0xE8
#define K_NO_9			0xE9
#define K_BACK_SPACE    0xEA
#define K_ESC			0xEB
#define K_ENTER			0xEC
#define K_YES           0xED
#define K_NO            0xEF

/// EOF
#define K_EOF			0xF9



//############################################
//############################################
/// M/C -> OP
//############################################
//############################################
//----- level1 -----
#define R_MC_DATA			0x10	//(120905)
#define R_PARAMETER			0x11	//(120518)

//----- punching ----- (121117) -----
#define R_CUR_CROSS             0x50    //(121117)
#define R_CUR_CIRCLE            0x51    //(121117)
#define R_SETCOLOR              0x52    //(121117)
#define R_XY_DATA               0x53    //(121117)
#define R_CUR_CROSS2            0x54    //(121222)
#define R_OP_TEXT               0x55    //(130226)
#define R_PUNCH_DATA_H          0x56    //(130228)
#define R_PUNCH_DATA_STI        0x57    //(130228)
#define R_PUNCH_DATA_CHK        0x58    //(130228)
#define R_PUNCH_DATA_NO         0x59    //(130305)
#define R_PUNCH_TEXT            0x5A    //(130604)
#define R_PUNCH_CHN_ICON        0x5B    //(130605)
#define R_PUNCH_PF_CONTROL      0x5C    //(130610)

//--------------------------------------
#define R_CHK_NUM               0x60
#define R_SEW_START             0x61
#define R_SEW_END               0x62
#define R_STI_CNT               0x63
#define R_SEW_ERROR             0x64
#define R_END                   0x65
#define R_START                 0x66
#define R_GO                    0x67
#define R_ERR_ESC               0x68    //(120916)
#define R_UPDATE                0x6E
#define R_INIT                  0x6F
#define R_J_SETUP               0x70
#define R_J_READY               0x71
#define R_J_START               0x72
#define R_PARA                  0x73
#define R_TEST                  0x74
#define R_DESIGN                0x75
#define R_FORMAT                0x76
#define R_SETUP_MENU            0x77	//(120923)
#define R_TRIM                  0x78	//(121018)
#define R_FORMAT_VER_EXCH       0x79
#define R_FORMAT_DISP           0x7A    //(121114) //0x80
#define R_CALIBRATE             0x7B    //(121114)
#define R_WARN                  0x7C    //(130226)

//add 120410
#define R_ENCODER               0x90
#define R_ENC_1                 0xF1
#define R_ENC_2                 0xF2
#define R_ENC_3                 0xF3
#define R_PULYSIZE              0x91
#define R_KEYVAL                0x92
#define R_MAINMTR               0x93    //(20120414)
#define R_TEST_INPUT            0x94    //(20120926)
#define R_UPSTOP                0xA0    //(20120830) para
#define R_BOBBIN                0xA1    //(20121109)
#define R_PRINFT                0xB0    //(20121101)
#define R_PRINFT1               0xB1    //(20121122)
#define R_POINTSET              0xB2    //(20130409)
#define R_SCREEN_CLR            0xB3    //(20130425)
#define R_WARNING               0xB4    //(20130531)
#define R_OPERATE               0xB5    //(20130425)



#define R_PATTERN_NUM           0xB6    //(20130425)



#define R_MOVE_JUMP             0xF0    //(20130531)

#define R_MOVING_CURSOR         0xF4

//----- EOF ------ //RECEIVE
#define R_SEND_READY            0xF7
//------------------
#define USB_VER_ERR             0x64    //(120822)

//--- Level 2 ---		//----- Group1 (MC data) -----
#define R_MODEL_1               0x00
#define R_MODEL_2               0x01
#define R_M_TYPE                0x02
#define R_VERSION_Y             0x03
#define R_VERSION_M             0x04
#define R_VERSION_D             0x05
#define R_X_SCALE               0x06
#define R_Y_SCALE               0x07
#define R_SPEED                 0x08
#define R_B_COUNT               0x09
//#define R_CHN_NO              0x0A
#define R_UPSTOPPOS             0x0B
#define R_X_PLUS                0x0C
#define R_X_MINUS               0x0D
#define R_Y_PLUS                0x0E
#define R_Y_MINUS               0x0F
//--------------------------------------
#define R_DATA_SEND         0x12	//(121009)
#define R_PTRN_NO			0x13	//0x10	//(120905)
#define R_P_COUNT			0x14	//0x11	//(120916)
#define R_CHN_NO			0x15	//(130117)
#define R_CHN_ARY0			0x16	//(130117)
#define R_CHN_ARY1			0x17	//(130117)
#define R_CHN_ARY2			0x18	//(130117)
#define R_CHN_ARY3			0x19	//(130117)
#define R_CHN_ARY4			0x1A	//(130117)
#define R_CHN_ARY5			0x1B	//(130117)
#define R_CHN_ARY6			0x1C	//(130117)
#define R_CHN_ARY7			0x1D	//(130117)
#define R_CHN_ARY8			0x1E	//(130117)
#define R_CHN_ARY9			0x1F	//(130117)
#define R_CHN_ARY10			0x20	//(130117)
#define R_CHN_ARY11			0x21	//(130117)
#define R_CHN_ARY12			0x22	//(130117)
#define R_CHN_ARY13			0x23	//(130117)
#define R_CHN_ARY14			0x24	//(130117)
#define R_CHN_ARY15			0x25	//(130117)
#define R_MD_PRJ_MEM        0x26    //(130221)




//===== Punching code ===== Start  (121121)
#define	 K_CODE_TRIM				0		//trimming-code
#define	 K_CODE_SEC_ORG        		1       //2nd-origin
#define  K_CODE_PAUSE				2       //temporary-stop
#define	 K_CODE_EMPTY				3       //machine 1 revolution without x-y moving
#define	 K_CODE_JUMP				4		//jump-stitching code
#define	 K_CODE_POINT				5		//point-stitching code
#define  K_CODE_LINE_CURVE			6		//line/curve-stitching code
#define  K_CODE_LINE				7		//line-stitching code
#define	 K_CODE_CURVE				8		//curve-stitching code
#define	 K_CODE_ARC					9		//arc-stitching code
#define	 K_CODE_CIRCLE				10		//circle-stitching code
#define	 K_CODE_JUMP_SPD			11		//jump-speed-change code
#define	 K_CODE_STI_SPD             12		//stitching-speed-change code
#define  K_CODE_STI_WIDTH			13		//stitch-width-change code
#define	 K_CODE_PTRN_READ			14		//read pattern data from FDD
#define	 K_CODE_PTRN_WRITE          15		//write pattern data from FDD
#define	 K_CODE_FORMAT				16		//FDD formatting
#define  K_CODE_INFO_DISP			17		//display information of pattern
#define	 K_CODE_CORDINATE			18		//set cordination-system
#define	 K_CODE_LINE_ZIGZAG         19		//zig-zag-line-stitching code
#define	 K_CODE_CURVE_ZIGZAG		20		//zig-zag-curve-stitching code
#define	 K_CODE_ARC_ZIGZAG          21		//zig-zag-arc-stitching code
#define	 K_CODE_CIRCLE_ZIGZAG		22		//zig-zag-circle-stitching code
#define	 K_CODE_LINE_OFFSET         23		//offset-line-stitching code
#define	 K_CODE_CURVE_OFFSET		24		//offset-curve-stitching code
#define	 K_CODE_ARC_OFFSET          25		//offset-arc-stitching code
#define	 K_CODE_CIRCLE_OFFSET		26		//offset-circle-stitching code
#define  K_CODE_LINE_DBL			27	    //double-line-stitching code
#define	 K_CODE_CURVE_DBL			28		//double-curve-stitching code
#define	 K_CODE_ARC_DBL             29		//double-arc-stitching code
#define	 K_CODE_CIRCLE_DBL			30		//double-circle-stitching code
#define  K_CODE_LINE_DBL_REV		31		//double-reverse-line-stitching code
#define	 K_CODE_CURVE_DBL_REV		32		//double-reverse-curve-stitching code
#define	 K_CODE_ARC_DBL_REV         33		//double-reverse-arc-stitching code
#define	 K_CODE_CIRCLE_DBL_REV		34		//double-reverse-circle-stitching code
#define  K_CODE_LINE_REV			35		//reverse-line-stitching code
#define	 K_CODE_CURVE_REV			36		//reverse-curve-stitching code
#define	 K_CODE_ARC_REV             37		//reverse-arc-stitching code
#define	 K_CODE_CIRCLE_REV			38		//reverse-circle-stitching code
#define  K_CODE_PTRN_DEL			39		//delete pattern elememt
#define	 K_CODE_BACK_TACK			40		//add back-tack code at start & end of pattern
#define  K_CODE_CONDENSE_STI		41		//add condensed stitch code at start & end of pattern
#define  K_CODE_OVERLAP_STI     	42		//add over-lapping sttich code bet. start & end of pattern
#define	 K_CODE_SYMMETRY_X          43		//add stitch code symmetrical to x-axis
#define	 K_CODE_SYMMETRY_Y   		44		//add stitch code symmetrical to y-axis
#define  K_CODE_SYMMETRY_P      	45		//add stitch code symmetrical to any point
#define  K_CODE_MOV_PTRN			46		//move pattern data to any point
#define	 K_CODE_COPY_PTRN			47		//copy pattern data to any point
#define  K_CODE_DELETE				48		//delete pattern data on memory
#define  K_CODE_REVERSE_SET     	49		//reverse device setting
#define	 K_CODE_SPD_CHNG         	50		//limit maximum speed of pattern
#define	 K_CODE_STITCH_DRAG      	51		//stitch dragging
#define	 K_CODE_STITCH_DEL			52		//delete the given stitch
#define	 K_CODE_MOV_SEWSTRT      	53		//move sewing start point to the new point
#define	 K_CODE_MOV_2ndORG       	54		//move 2nd origin point to the new point
#define	 K_CODE_TRIM_INS         	55		//atuomatically insert trimming-code or not when code deletion
#define	 K_CODE_SCL_REF          	56		//set scale reference point
#define	 K_CODE_SET_OPnn         	57		//turn on/off OP3.0 ~ OP3.7
                                            //		  OP4.0 ~ OP4.7
                                            //		  OP5.0 ~ OP5.7
#define	 K_CODE_CHK_IPnn         	58		//wait for low/high of IP1.0 ~ IP1.7
                                            //		   IP2.0 ~ IP2.7
                                            //		   IP3.0 ~ IP3.7
#define	 K_CODE_TIME_DELAY       	59		//delay for the given time
#define  K_CODE_PF_CONTROL       	60
#define  K_CODE_CAN_OPnn         	61
#define  K_CODE_CAN_IPnn         	62
#define	 K_CODE_TR3_SET          	63		//the 3rd TR solenoid setting
#define	 K_CODE_Edit_Func        	64		//Design Edit function

#define  K_CODE_JUMPLINE            70

//09.08.31
#if(SPS_2516+SPS_3020)		//#ifdef	SPS_F3020
#define	 K_CODE_POCKET				81
#define	 K_CODE_GUIDE_END			82
#endif



//COUNTE RECEIVER
#define R_COUNT_VALUE              0xF6




#define K_DESIGN_SEND_TEST         0xF5

//===== Punching code ===== End  (121121)



//-----------------------------
//-----     level 2nd     -----
//-----------------------------
//--- Level 2 ---		//----- Group2 (para data) -----
//----- 1. Parameter  -----
#define R_SET_JOG				0x00
#define R_JOG_MODE				0x01
#define R_MC_ORG_MODE_1         0x02
#define R_MC_ORG_MODE_2         0x03
#define R_RETURN_MODE			0x04
#define R_SET_BOBBIN_CNT		0x05
//#define 	0x06
#define R_READ_MODE             0x07
#define R_SET_TRIM_AFT_STOP     0x08	//(120110)
#define R_SLOW_START			0x09
#define R_User_Slow_1			0x0A
#define R_User_Slow_2			0x0B
#define R_User_Slow_3			0x0C
#define R_User_Slow_4			0x0D
#define R_User_Slow_5			0x0E
#define R_User_Slow_6			0x0F
#define R_User_Slow_7			0x10
#define R_User_Slow_8			0x11
#define R_User_Slow_9			0x12
#define R_User_Slow_10			0x13
#define R_MAX_SPEED             0x14
#define R_FEED_ENDPOS			0x15
#define R_FF_OPERATION			0x16	// para.022
#define R_SET_DOWN				0x17
#define R_PEDAL_MODE_1			0x18
#define R_PEDAL_MODE_2			0x19
#define R_PF_OPERATION			0x1A
//#define 	0x1B
#define R_SET_WIPER				0x1C
#define R_CONTROL_HEAD      	0x1D
#define R_THREAD_DETECT         0x1E
#define R_DETECT_STI_1			0x1F	// para 031
#define R_DETECT_STI_2			0x20
#define R_SET_TRIM				0x21	//(120110)
//#define 	0x22
//#define 	0x23
//#define 	0x24
//#define 	0x25
//#define 	0x26
//#define 	0x27
//#define 	0x28
//#define 	0x29
//#define 	0x2A
//#define 	0x2B
//#define 	0x2C
//#define 	0x2D
#define R_PF_FULL_ON_TIME		0x2E
#define R_FF_FULL_ON_TIME		0x2F
#define R_TT_FULL_ON_TIME		0x30
#define R_TH_FULL_ON_TIME		0x31
#define R_WP_FULL_ON_TIME		0x32
//#define 	0x33
//#define 	0x34
//#define 	0x35
#define R_PF_DUTY				0x36
#define R_FF_DUTY				0x37
#define R_TT_DUTY				0x38
#define R_TH_DUTY				0x39
#define R_WP_DUTY				0x3A
//#define 	0x3B
//#define 	0x3C
//#define 	0x3D
#define R_DESIGN_OPEN_MODE      0x3E	// para 062
#define R_SCALE_MODE			0x3F	// para 063
#define R_CHAIN_NO				0x40
#define R_CHAIN_MODE			0x41
#define R_CHAIN_CLAMP			0x42
#define R_DECEL_STI				0x43
#define R_TRIM_SPM				0x44	//(120522)
#define R_TRIM_DELAY         	0x45
#define R_LOW_AIR       		0x46
#define R_FF_NO         		0x47
#define R_FF_PAUSE_CTRL         0x48
#define R_THRDHOLD_EN			0x49		// para 073
//#define 	0x4A
//#define 	0x4B
#define R_SCALE_REF         	0x4C
//#define 	0x4D
#define R_SET_LIMIT         	0x4E		// para 078
#define R_X_P_LIMIT         	0x4F		// para 079
#define R_X_M_LIMIT         	0x50		// para 080
#define R_Y_P_LIMIT         	0x51		// para 081
#define R_Y_M_LIMIT         	0x52		// para 082
//#define 	0x53
#define R_REVERSE_AFT_TRIM      0x54
#define R_REVERSE_ANGLE     	0x55
//#define 	0x56
//#define 	0x57
#define R_SAFETY_MODE       	0x58
#define R_JUMP_SPEED        	0x59
#define R_STOP_JUMP_EM      	0x5A
//#define 	0x5B
//#define 	0x5C
#define R_SET_CLAMP         	0x5D	// para 093
#define R_CLAMP_DATA        	0x5E	// para 094
#define R_SET_PF            	0x5F
#define R_CLAMP_RANGE       	0x60	// para 096
#define R_PF_RANGE          	0x61
#define R_AUTO_CALL         	0x62
#define R_AUTO_READY        	0x63	// para 099
#define R_SET_ATTACH        	0x64	// para 100
#define R_AUTO_CALL_TM      	0x65
#define R_SET_AUTO_NO       	0x66
#define R_SET_EX_BOARD      	0x67
//#define R_TRIM_SPM              0x68    //(130806)
//#define R_TRIM_DELAY            0x69    //(130806)

//-----------------------------------
//#define R_SET_HEAD_SENS     		0x73	// 115
//#define R_TURN_HEAD         		0x7E
//#define R_DOOR_SENS         		0x7F

#if(SPS_5050+SPS_8050+SPS_80100+SPS_50200)
    #define R_HOOK_ORG_MODE         0x69 	// 105
    //unsigned char head_dis_en;    		//106
    #define	R_OIL_ON_TM          	0x6B	//107
    #define	R_OIL_OFF_TM			0x6C    //108
    #define R_THREAD_HOLD           0x6D 	//109
    #define R_THREAD_HOLD_TM        0x6E    //110
    #define	R_SET_NEEDLE_COOL		0x6F
    #define R_COOL_ON_TM			0x70	// 112
    #define R_COOL_OFF_TM			0x71	// 113
    //unsigned char SetTrimmer;          	// 114
    #define R_SET_HEAD_SENS     	0x73    // 115
    //unsigned char ClampSensor;            // 116
    #define R_BCODE_ORG         	0x75	// 117
    #define R_BCODE_TYPE        	0x76	// 118
    #define R_START_ANGLE         	0x77	// 119 (121123)
    //unsigned char TrimHoldAngle;          // 120
    #define R_SET_PF_MTR			0x79	// 121
    #define R_JUMP_TENSION      	0x7A	// 122
    #define R_PROGRAM_LOCK      	0x7B	// 123
    #define R_POSITION_REF      	0x7C	// 124
    #define R_SET_AIRBAG			0x7D	// 125
    //unsigned char bobbin_num;     //0x7E  // 126
    //int	      UDCUpStop;				// 127 UpStop position(UDCUpStop)
#ifdef THREADBOARD                          //(121121)
    #define R_THRD_BRD_EN           0x7F	// 127
    #define R_THRD_SOL_DUTY         0x80	// 128
    //#define R_ThreadSol2_duty;	//0x81	// 129
    #define R_PARA_UPSTOPPOS        0x82	// 130 UpStop position(UDCUpStop)
#else
    #define R_PARA_UPSTOPPOS        0x7F	// 127 UpStop position(UDCUpStop)
#endif

#elif(HT_SPS_3030 + HT_SPS_5050)
    #define R_HOOK_ORG_MODE         0x69 	// 105
    //unsigned char head_dis_en;    		//106
    #define	R_OIL_ON_TM          	0x6B	//107
    #define	R_OIL_OFF_TM			0x6C    //108
    #define R_THREAD_HOLD           0x6D 	//109
    #define R_THREAD_HOLD_TM        0x6E    //110
    #define	R_SET_NEEDLE_COOL		0x6F
    #define R_COOL_ON_TM			0x70	// 112
    #define R_COOL_OFF_TM			0x71	// 113
    //unsigned char SetTrimmer;          	// 114
    //unsigned char HeadSensor;             // 115
    //unsigned char ClampSensor;            // 116
    #define R_BCODE_ORG         	0x75	// 117
    #define R_BCODE_TYPE        	0x76	// 118
    #define R_START_ANGLE         	0x77	// 119 (121123)
    //unsigned char TrimHoldAngle;          // 120
    #define R_SET_PF_MTR			0x79	// 121
    #define R_JUMP_TENSION      	0x7A	// 122
    #define R_PROGRAM_LOCK      	0x7B	// 123
    #define R_POSITION_REF      	0x7C	// 124
    #define R_SET_AIRBAG			0x7D	// 125
    //unsigned char bobbin_num;             // 126
    #ifdef THREADBOARD                          //(121121)
        #define R_THRD_BRD_EN           0x7F	// 127
        #define R_THRD_SOL_DUTY         0x80	// 128
        //#define R_ThreadSol2_duty;	//0x81	// 129
        #define R_PARA_UPSTOPPOS        0x82	// 130 UpStop position(UDCUpStop)
    #else
        #define R_PARA_UPSTOPPOS        0x7F	// 127 UpStop position(UDCUpStop)
    #endif

#else			//SmallPatternMC
    //unsigned char ClampSensor;            // 105
    #define R_BCODE_ORG         	0x6A	// 106
    #define R_BCODE_TYPE        	0x6B	// 107
    #define R_JUMP_TENSION      	0x6C	// 108
    #define R_PROGRAM_LOCK      	0x6D	// 109
    #define R_POSITION_REF      	0x6E	// 110
    //unsigned char TrAirType;          	// 111
    #define R_SET_AIRBAG		 	0x70	// 112
    #define R_START_ANGLE         	0x71	// 113 (120522)
    //unsigned char bobbin_num;     0x72    // 114

    #if SPS_1507                    		//(121210)
        #define	R_CLAMP_DEV     	0x73    //115
        #define	R_CLAMP_REV_PUNCH	0x74    //116
        #ifdef THREADBOARD
            #define R_THRD_BRD_EN		0x75	// 117
            #define R_THRD_SOL_DUTY     0x76	// 118
            //#define R_ThreadSol2_duty;0x77    // 119
            #define R_PARA_UPSTOPPOS	0x78	// 120 UpStop position(UDCUpStop)
        #else
            #define R_PARA_UPSTOPPOS	0x75	// 117 UpStop position(UDCUpStop)
        #endif
    #elif(SPS_3020+SPS_2516)        //#if(SPS_3020+SPS_2516)
        #define R_USE_POCKET		0x73	// 115(F3020)		//09.08.31
        //#define R_PocketBacktack;			// 116(F3020)
        //#define R_BarTackSpeed;			// 117(F3020)
        //#define R_Set_Inner_Pos_Fix;		// 118(F3020)
        #ifdef THREADBOARD
            #define R_THRD_BRD_EN		0x77	// 119
            #define	R_THRD_SOL_DUTY     0x78	// 120 (120406)
            //#define R_ThreadSol2_duty;        // 121 (120406)
            #define R_PARA_UPSTOPPOS	0x7A	// 122 UpStop position(UDCUpStop)
        #else
            #define R_PARA_UPSTOPPOS	0x77	// 119 UpStop position(UDCUpStop)
        #endif
    #else
        #ifdef THREADBOARD
            #define R_THRD_BRD_EN		0x73	// 115
            #define	R_THRD_SOL_DUTY     0x74	// 116 (110915)
            //#define R_ThreadSol2_duty;        // 117 (111013)
            #define R_PARA_UPSTOPPOS	0x76	// 118 UpStop position(UDCUpStop)
        #else
            #define R_PARA_UPSTOPPOS	0x73	// 115 UpStop position(UDCUpStop)
        #endif
    #endif
#endif
//------------------------------------

//----- 2. ver exchage -----
#define R_USB_NOT_FOUND         0x01    //(20121106)
#define R_USB_Diret_NOT_FOUND   0x02    //(20121106)
#define R_USB_ROMFILE_ERR       0x03    //(20121106)
#define R_SURE                  0x04    //(20121106)
#define R_MD5_NOT_FOUND         0x05    //(20121106)
#define R_ROMI_NOT_FOUND        0x06    //(20121106)
#define R_VER_UPDATE            0x07    //(20121106)
#define R_VER_UPDATE_SUC		0x08    //(20121107)
#define R_VER_COPY_MD5			0x09    //(20121107)
#define R_VER_COPY_ROMI         0x0A    //(20121107)
#define R_VER_COPY_CHK			0x0B    //(20121107)
#define R_VER_COPY_DIFF         0x0C    //(20121107)
#define R_VER_COPY_3			0x0D    //(20121107)
#define R_VER_COPY_4			0x0E    //(20121107)
#define R_VER_COPY_FAIL			0x0F    //(20121107)
#define R_POWEROFF				0x10    //(20121107)
#define R_VER_FORMAT			0x11    //(20121107)
//-----------------------

#endif // PROTOCOL_H



