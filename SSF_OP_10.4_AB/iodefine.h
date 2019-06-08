#ifndef IODEFINE_H
#define IODEFINE_H


//#ifndef __arm__
#ifdef __arm__
    #define ARM_TEST 1
#else
    #define LINUX_TEST 1
#endif

#define WINDOWS 1




//--------- test -----------
#define CD_DATA_TEST          1     //(121108)
//#define SCALE_TEST          1     //(121110)
#define PUNCHING_             1     //(1211120)
#define CHATTING              1     //(121124)
#define SEND_WORK_CMD         1     //(121128)
#define NEW_OP                1     //(130115)
#define NEW_WORK_CMD          1     //(130118)
#define FORW_BACK_KEY         1

//--------- debug ----------
//#define DEBUG_CD_DATA         1     //(121116)
//#define DEBUG_PUNCHING        1     //(121116)


//=================================
//#define OP_VER        "20131227"
//=================================


//----- E series Pattern -----
//#define	SPS_1306      	1	 	//SPS-1306
#define 	SPS_1507      	2      	//SPS_1507
//#define	SPS_1811      	3  		//SPS-1811
//#define 	SPS_2211        11      //SPS-2211		//(120903)
//#define	SPS_2516        4	 	//SPS_2516
//#define 	SPS_3020        10      //SPS-3020		//(120903)
//#define	SPS_5030      	5	 	//SPS-5030
//#define	SPS_5050        6	 	//SPS-5050
//#define 	SPS_8050      	7       //SPS-8050
//#define 	SPS_50200    	8       //SPS-5002000
//----- LargePatternMC -----
//#define SPS_80100         1
//-----------------------------

//#define	HT_SPS_3030     12	 	//SPS-3030
//#define	HT_SPS_5050     13	 	//SPS-5050

//---------------------------------------------
//#define NULL                0


//---------------------------------------------
#define THREADBOARD         1   //(120905)
#define NEW_PARA            1   //(120905)

//---------------- chain ----------------------
#define MAX_CHN             16  //(130117)


//---------- Coordination Type ----------------
#define ABSXY               0x00    // absolute coordination
#define RELXY               0x01    // relative coordination

//---------------- language -------------------
#define KOREA               0
#define ENG                 1
#define CHINA               2
#define SPAIN               3

//---------------- Qt color -------------------
#define QT_WHITE            3
#define QT_BLACK            2
#define QT_RED              7
#define QT_DARKRED         13
#define QT_GREEN            8
#define QT_DARKGREEN       14
#define QT_BLUE             9
#define QT_DARKBLUE        15
#define QT_CYAN            10
#define QT_DARKCYAN        16
#define QT_MAGENTA         11
#define QT_DARKMAGENTA     17
#define QT_YELLOW          12
#define QT_DARKYELLOW      19
#define QT_GRAY             5
#define QT_DARKGRAY         4
#define QT_LIGHTGRAY        6

//#define T_PINK              20  //(130219)
//#define T_PURPLE            21  //(130219)

//------------------------ error number define -------------------------//
//#define	NO_ERR				0
//#define	MAIN_MT_ERR         1
//#define SYNC_ERR			2
//#define NO_PTRN_ERR   		3
//#define FDD_EMPTY_ERR		4
//#define DSK_RD_ERR			5
//#define	DSK_WR_ERR          6
//#define	DSK_FMT_ERR         7
//#define	DSK_FULL_ERR		8
//#define SCL_OVR_ERR         9
//#define STI_OVR_ERR         10
//#define CNT_RST_ERR         11
//#define CMB_ERR             12
//#define	NDL_POS_ERR         13
//#define	LMT_OVR_ERR         14
//#define	CAL_ERR             15
//#define	DATA_BAD_ERR		16
//#define	EM_STOP_ERR         17
//#define THRD_CUT_ERR		18
//#define XY_ERR				19
//#define	NO_SYS_ERR			20
//#define	MEM_ERR             21
//#define	WR_PRTCT_ERR		22
//#define	INSUF_MEM_ERR		23
//#define LOW_PRESSURE		24
//#define	DRAG_ERR			25
//#define	LOWFEED_ERR         26
//#define	PALETTE_ERR         27
//#define	EM_SW_PRESSED		28	//emergency pressed
//#define	ST_SW_PRESSED		29	//start pedal pressed
//#define	FF_SW_PRESSED		30	//right clamp switch pressed
//#define	FFL_SW_PRESSED      31	//left clamp switch pressed
//#define	TS_SW_PRESSED		32	//two stage clamp switch pressed
//#define	SER_COM_ERR         33
//#define	FOUND				34
//#define	NOT_FOUND			35
//#define	WZ_ERR				36
//#define LOW_MT_ERR          37
//#define Y_MT_ERR            38
//#define X_MT_ERR			39
//#define TIMER_ERR			40
//#define MOTOR_TYPE_ERR      41      //9999.Motro type dismatched
//#define ASIPM_ERR			42      //133. Over current/Over tem/arm Short...
//#define OVC_ERR     		43      //131. Over current or abnormal current control
//#define OVL_ERR         	44      //129. Over Load
//#define ENCRST_ERR			45      //128. Encoder RST Error
//#define	ENCAB_ERR			46      //127. Encoder AB Error
//#define SYNCIN_ERR			47      //60.  Synchro connector inserted
//#define	SYNCOUT_ERR     	48      //61.  Synchro connector pulled out
//#define	REVCOM_ERR			49      //126. Reverse Communication Error
//#define EEPR_ERR			50      //EEPR.EEPROM access error(check-sum error)
//#define	MTINFO_SER_COM_ERR  51
//#define BOB_STITCH_RESET	52
//#define SCL_EN_RE_ERR      	53
//#define P_MT_ERR			54
//#define MC_OPEN_ERR        	55
//#define PF_CLAMP_ORG_ERR	56
//#define FAN_ERR             57
//#define AC_CHK_ERR			58
//#define OVER_VOLT_ERR		59
//#define UNDER_VOLT_ERR      60
//#define CAN_COMM_ERR		61


///----- Main Motor Error Command-------
//#define PWROFF_ERR          62	//Power Off
//#define	PWRON_ERR			63	//Power On
//#define	RXCOM_ERR			64	//Rx Com Error
//#define	TXCOM_ERR			65	//Tx Com Error
//#define	PU0_ERR             66	//Externa P/U Connection Error
//#define	PU1_ERR             67	//Built-in P/U Connection Error
//#define	MSYNC_ERR			68	//Synchro signal Error
//#define	SF22_ERR			69	//Safety switch Error
//#define	RATE_ERR			70	//Motor rated over error

//#define	USB_ERR             71


#if(SPS_2516+SPS_3020)          //(120920)
#define	SENS70_ERR			72
#define	SENS71_ERR			73
#define SENS72_ERR			74
#define SENS73_ERR   		75
#define SENS74_ERR			76
#define SENS75_ERR			77
#endif

//#ifdef	TOUCH_OP
#define	PARA_CNT_ERR		80     //(120131)
#define	CODE_PAUSE          81     //(121007)
#define DATA_SEND1_ERR      82     //(121010)
#define DATA_SEND2_ERR      83     //(121010)
#define MC_COM_ERR          84     //(121018)
#define NO_PATTERN          85
#define PTRN_OVERWTR        86
#define DESIGN_DATA_H_ERR   87     //(130228)
#define DESIGN_DATA_STI_ERR 88     //(130228)
//#endif


//-------------------------------------------------
//-----              TABLE.C                  -----
//-------------------------------------------------
//#if(SPS_2516+SPS_3020)
    extern short code_meth_tbl[][8];
//#else
//    extern short code_meth_tbl[][8];
//#endif


//#if(SPS_2516+SPS_3020)
    extern unsigned char order_tbl[][3];
//#else
//    extern unsigned char order_tbl[][3];
//#endif
    #if(SPS_2516 + SPS_3020)
        #define PUNCH_CODE_MAX      83
    #else
        #define PUNCH_CODE_MAX      68
    #endif
    //extern static const char * const punch_code[];
    //extern static char * const punch_code[];

//#define	PARA_NUM		0
//#define	METH_NUM		1
//#define	PARA0_MIN		2
//#define	PARA0_MAX		3
//#define	PARA1_MIN		4
//#define	PARA1_MAX		5
//#define	PARA2_MIN      	6
//#define	PARA2_MAX		7


//#define	ORDER0          0
//#define	ORDER1          1
//#define	ORDER2          2

//---------------- punching code -------------------

//------------- function code define : start or middle points ------------
#define	TRIM				0		//trimming-code


#define	SEC_ORG        		1       //2nd-origin
#define PAUSE				2       //temporary-stop
#define	EMPTY				3       //machine 1 revolution without x-y moving
#define	JUMP				4		//jump-stitching code
#define	POINT				5		//point-stitching code
#define LINE_CURVE			6		//line/curve-stitching code


#define LINE				7		//line-stitching code
#define	CURVE				8		//curve-stitching code


#define	ARC					9		//arc-stitching code
#define	CIRCLE				10		//circle-stitching code
#define	JUMP_SPD			11		//jump-speed-change code
#define	STI_SPD				12		//stitching-speed-change code
#define STI_WIDTH			13		//stitch-width-change code
#define	PTRN_READ			14		//read pattern data from FDD
#define	PTRN_WRITE			15		//write pattern data from FDD
#define	FORMAT				16		//FDD formatting
#define INFO_DISP			17		//display information of pattern
#define	CORDINATE			18		//set cordination-system
#define	LINE_ZIGZAG         19		//zig-zag-line-stitching code
#define	CURVE_ZIGZAG		20		//zig-zag-curve-stitching code
#define	ARC_ZIGZAG			21		//zig-zag-arc-stitching code
#define	CIRCLE_ZIGZAG		22		//zig-zag-circle-stitching code
#define	LINE_OFFSET         23		//offset-line-stitching code
#define	CURVE_OFFSET		24		//offset-curve-stitching code
#define	ARC_OFFSET			25		//offset-arc-stitching code
#define	CIRCLE_OFFSET		26		//offset-circle-stitching code
#define LINE_DBL			27	    //double-line-stitching code
#define	CURVE_DBL			28		//double-curve-stitching code
#define	ARC_DBL             29		//double-arc-stitching code
#define	CIRCLE_DBL			30		//double-circle-stitching code
#define LINE_DBL_REV		31		//double-reverse-line-stitching code
#define	CURVE_DBL_REV		32		//double-reverse-curve-stitching code
#define	ARC_DBL_REV         33		//double-reverse-arc-stitching code
#define	CIRCLE_DBL_REV		34		//double-reverse-circle-stitching code
#define LINE_REV			35		//reverse-line-stitching code
#define	CURVE_REV			36		//reverse-curve-stitching code
#define	ARC_REV             37		//reverse-arc-stitching code
#define	CIRCLE_REV			38		//reverse-circle-stitching code
#define PTRN_DEL			39		//delete pattern elememt
#define	BACK_TACK			40		//add back-tack code at start & end of pattern
#define CONDENSE_STI		41		//add condensed stitch code at start & end of pattern
#define	OVERLAP_STI         42		//add over-lapping sttich code bet. start & end of pattern
#define	SYMMETRY_X          43		//add stitch code symmetrical to x-axis
#define	SYMMETRY_Y          44		//add stitch code symmetrical to y-axis
#define SYMMETRY_P          45		//add stitch code symmetrical to any point
#define MOV_PTRN			46		//move pattern data to any point
#define	COPY_PTRN			47		//copy pattern data to any point
#define DELETE				48		//delete pattern data on memory
#define REVERSE_SET         49		//reverse device setting
#define	SPD_CHNG			50		//limit maximum speed of pattern
#define	STITCH_DRAG         51		//stitch dragging
#define	STITCH_DEL			52		//delete the given stitch
#define	MOV_SEWSTRT         53		//move sewing start point to the new point
#define	MOV_2ndORG          54		//move 2nd origin point to the new point
#define	TRIM_INS			55		//atuomatically insert trimming-code or not when code deletion
#define	SCL_REF				56		//set scale reference point
#define	SET_OPnn			57		//turn on/off OP3.0 ~ OP3.7
                                    //  OP4.0 ~ OP4.7
                                    //  OP5.0 ~ OP5.7
#define	CHK_IPnn			58		//wait for low/high of IP1.0 ~ IP1.7
                                    //	IP2.0 ~ IP2.7
                                    //	IP3.0 ~ IP3.7
#define	TIME_DELAY			59		//delay for the given time
#define PF_CONTROL			60
#define CAN_OPnn       		61
#define CAN_IPnn       		62
#define	TR3_SET             63		//the 3rd TR solenoid setting
#define	Edit_Func			64		//Design Edit function

#ifdef SPS_1507				//(121210)
    #define	DEV_BACK_S1507	66
#endif

#define JUMPLINE			70

#if(SPS_2516+SPS_3020)		//#ifdef	SPS_F3020
    #define	POCKET				81
    #define	GUIDE_END			82
#endif

#define	END					0x100		//mark end of code
#define	START				0xFF		//mark start of code



#endif // IODEFINE_H


