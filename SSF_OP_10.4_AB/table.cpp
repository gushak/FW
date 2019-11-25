
/#include "d_main2.h"
//#include "ui_d_main2.h"
#include "path.h"

/* dialog header */
//#include "d_menu.h"
//#include "d_main2.h"
//#include "d_network.h"

#include <QDebug>       //(121117)
#include <QTranslator>  //(131106)


//-------------------------------------------------------------------------
//--- indexes to access each table
//-------------------------------------------------------------------------
//#if(SPS_2516+SPS_3020)
    short code_meth_tbl[][8] = {
//#else
//    short code_meth_tbl[][8] = {
//#endif
//    int code_meth_tbl[][8] = {
        //|para|meth|min0|  max0|   min1|   max1|min2|max2|
        //|num |num |    |      |       |       |    |    |
        {	0,	0,	0,      0,      0,      0,	0,	0	},	//000 : TRIM
        {	0,	0,	0,      0,      0,      0,	0,	0	},	//001 : SEC_ORG
        {	0,	0,	0,      0,      0,      0,	0,	0	},	//002 : PAUSE
        {	0,	0,	0,      0,  	0,      0,	0,	0	},	//003 : EMPTY
        {	0,	0,	0,      0,      0,      0,	0,	0	},	//004 : JUMP
        {	0,	0,	0,      0,      0,      0,	0,	0	},	//005 : POINT
        {	0,	0,	0,      0,      0,      0,	0,	0	},	//006 : LINE/CURVE
        {	1,	0,	1,      127,	0,      0,	0,	0	},	//007 : LINE
        {	1,	0,	1,      127,	0,      0,	0,	0	},	//008 : CURVE
        {	1,	0,	1,      127,	0,      0,	0,	0	},	//009 : ARC
        {	1,	0,	1,      127,	0,      0,	0,	0	},	//010 : CIRCLE
        {	1,	0,	1000,   3000,   0,      0,	0,	0	},	//011 : JUMP   SPD
        {	1,	0,	2,      25,     0,      0,	0,	0	},	//012 : STI	SPD
        {	1,	0,	1,      127,	0,      0,	0,	0	},	//013 : STI    WIDT
        {	1,	0,	0,      999,	0,      0,	0,	0	},	//014 : PTRN 	READ
        {	1,	0,	0,      999,	0,      0,	0,	0	},	//015 : PTRN  WRITE
        {	0,	0,	0,      0,      0,      0,	0,	0	},	//016 : FORMAT
        {	0,	0,	0,      0,      0,      0,	0,	0	},	//017 : INFO	DISP
        {	1,	0,	0,      1,      0,      0,	0,	0	},	//018 : CORD	SYS
        //ZigZag Max 127 - >100  ABC_Series 100
        {	3,	0,	2,      100,	1,    100,	0,	3	},	//019 : LINE	ZIG
        {	3,	0,	2,      100,	1,    100,	0,	3	},  //020 : CURVE	ZIG
        {	3,	0,	2,      100,	1,    100,	0,	3	},	//021 : ARC	ZIG
        {	3,	0,	2,      100,	1,	100,	0,	3	},	//022 : CIRCLE	ZIG
        {	3,	0,	1,      127,	1,	100,	0,	1	},	//023 : LINE	OFST
        {   3,	0,	1,      127,	1,	100,	0,	1	},	//024 : CURVE	OFST
        {	3,	0,	1,      127,	1,	100,	0,	1	},	//025 : ARC	OFST
        {	3,	0,	1,      127,	1,	100,	0,	1	},	//026 : CIRCLE	OFST
        {	3,	0,	1,      127,	1,	100,	0,	1	},	//027 : LINE	DBL
        {	3,	0,	1,      127,	1,	100,	0,	1	},	//028 : CURVE	DBL
        {	3,	0,	1,      127,	1,	100,	0,	1	},	//029 : ARC	DBL
        {	3,	0,	1,      127,	1,	100,	0,	1	},	//030 : CIRCLE	DBL
        {	3,	0,	1,      127,	1,	100,	0,	1	},	//031 : LINE	DREV
        {	3,	0,	1,      127,	1,	100,	0,	1	},	//032 : CURVE	DREV
        {	3,	0,	1,      127,	1,	100,	0,	1	},	//033 : ARC	DREV
        {	3,	0,	1,      127,	1,	100,	0,	1	},	//034 : CIRCLE	DREV
        {	1,	0,	1,      127,	0,	0,	0,	0	},	//035 : LINE	REV
        {	1,	0,	1,      127,	0,	0,	0,	0	},	//036 : CURVE 	REV
        {	1,	0,	1,      127,	0,	0,	0,	0	},	//037 : ARC	REV
        {	1,	0,	1,      127,	0,	0,	0,	0	},	//038 : CIRCLE	REV
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//039 : PTRN	DEL
        {	2,	0,	0,      9,      0,	1,	0,	0	},	//040 : BACK	TACK
        {	3,	0,	1,      9,      1,	9,	1,	127	},	//041 : CNDNS	STI
        {	1,	0,	1,      21,     0,	0,	0,	0	},	//042 : OVLAP	STI
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//043 : SYMMETRY X
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//044 : SYMMETRY Y
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//045 : SYMMETRY P
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//046 : MOV	PTRN
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//047 : COPY	PTRN
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//048 : DEL	PTRN
        {	1,	1,	0,      1,      0,	0,	0,	0	},	//049 : REV	SET
        {	3,	0,	2,      25,     0,	999,0,	999	},	//050 : SPD	CHNG
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//051 : STITCH	DRAG
        {	1,	0,	1,      99,     0,	0,	0,	0	},	//052 : STITCH	DEL
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//053 : MOV SEWSTRT
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//054 : MOV SEWSTRT
        {	1,	0,	0,      1,      0,	0,	0,	0	},	//055 : AUTO	TRIM
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//056 : SCALE REFER
        {	0,	1,	0,   	0,      0,	0,	0,	0	},	//057 : SET	OPnn
        {	0,	1,	0,      0,      0,	0,	0,	0	},	//058 : CHK	IPnn
        {	1,	0,	1,      7500,	0,	0,	0,	0	},	//059 : TIME	DELAY
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//060 : PF Control
        {	0,	2,	0,   	0,      0,	0,	0,	0	},	//061 : EXT SET OPnn
        {	0,	2,	0,      0,      0,	0,	0,	0	},	//062 : EXT CHK	IPnn
        {	1,	1,	0,      1,      0,	0,	0,	0	},	//063 : TR3	SET
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//064 : Modify
        //09.08.31
        #if(SPS_2516+SPS_3020)		//#ifdef	SPS_F3020
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//065 :
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//066 :
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//067 :
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//068 :
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//069 :
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//070 :
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//071 :
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//072 :
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//073 :
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//074 :
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//075 :
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//076 :
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//077 :
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//078 :
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//079 :
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//080 : NULL
        {	3,	0,	1,      127,	1,	300,	1,	250	},	//081 : POCKET
        {	0,	0,	0,      0,      0,	0,	0,	0	},	//082 : GUIDE_END
        #endif
    };



//-------------------------------------------------------------------------
//--- the order of the parameters of each code
//-------------------------------------------------------------------------
//#if(SPS_2516+SPS_3020)
//    unsigned char order_tbl[82][3] = {
//#else
    unsigned char order_tbl[][3] = {
//#endif
    //unsigned char order_tbl[][3] = {
        //- order0 -|- order1 -|- order2 -|
        {	0,		0,		0	},	//000 : TRIM
        {	0,		0,		0	},	//001 : SEC_ORG
        {	0,		0,		0	},	//002 : PAUSE
        {	0,		0,		0	},	//003 : EMPTY
        {	0,		0,		0	},	//004 : JUMP
        {	0,		0,		0	},	//005 : POINT
        {	0,		0,		0	},	//006 : LINE/CURVE
        {	3,		0,		0	},	//007 : LINE
        {	3,		0,		0	},	//008 : CURVE
        {	3,		0,		0	},	//009 : ARC
        {	3,		0,		0	},	//010 : CIRCLE
        {	4,		0,		0	},	//011 : JUMP   SPD
        {	2,		0,		0	},	//012 : STI	SPD
        {	3,		0,		0	},	//013 : STI    WIDT
        {	3,		0,		0	},	//014 : PTRN 	READ
        {	3,		0,		0	},	//015 : PTRN  WRITE
        {	0,		0,		0	},	//016 : FORMAT
        {	0,		0,		0	},	//017 : INFO	DISP
        {	1,		0,		0	},	//018 : CORD	SYS
        {	3,		3,		1	},	//019 : LINE	ZIG
        {	3,		3,		1	},  //020 : CURVE	ZIG
        {	3,		3,		1	},	//021 : ARC	ZIG
        {	3,		3,		1	},	//022 : CIRCLE	ZIG
        {	3,		3,		1	},	//023 : LINE	OFST
        {   3,		3,		1	},	//024 : CURVE	OFST
        {	3,		3,		1	},	//025 : ARC	OFST
        {	3,		3,		1	},	//026 : CIRCLE	OFST
        {	3,		3,		1	},	//027 : LINE	DBL
        {	3,	 	3,		1	},	//028 : CURVE	DBL
        {	3,		3,		1	},	//029 : ARC	DBL
        {	3,		3,		1	},	//030 : CIRCLE	DBL
        {	3,		3,		1	},	//031 : LINE	DREV
        {	3,		3,		1	},	//032 : CURVE	DREV
        {	3,		3,		1	},	//033 : ARC	DREV
        {	3,		3,		1	},	//034 : CIRCLE	DREV
        {	3,		0,		0	},	//035 : LINE	REV
        {	3,		0,		0	},	//036 : CURVE 	REV
        {	3,		0,		0	},	//037 : ARC	REV
        {	3,		0,		0	},	//038 : CIRCLE	REV
        {	0,		0,		0	},	//039 : PTRN	DEL
        {	1,		1,		0	},	//040 : BACK	TACK
        {	1,		1,		3	},	//041 : CNDNS	STI
        {	2,		0,		0	},	//042 : OVLAP	STI
        {	0,		0,		0	},	//043 : SYMMETRY X
        {	0,		0,		0	},	//044 : SYMMETRY Y
        {	0,		0,		0	},	//045 : SYMMETRY P
        {	0,		0,		0	},	//046 : MOV	PTRN
        {	0,		0,		0	},	//047 : COPY	PTRN
        {	0,		0,		0	},	//048 : DEL	PTRN
        {	1,		0,		0	},	//049 : REV	SET
        {	2,		3,		3	},	//050 : SPD	CHNG
        {	0,		0,		0	},	//051 : STITCH DRAG
        {	2,		0,		0	},	//052 : STITCH DEL
        {	0,		0,		0	},	//053 : MOV SEWSTRT
        {	0,		0,		0	},	//054 : MOV 2ndORG
        {	1,		0,		0	},	//055 : AUTO   TRIM
        {	0,		0,		0	},	//056 : SCALE REFER
        {	1,		1,		1	},	//057 : SET    OPnn
        {	1,		1,		1	},	//058 : CHK    IPnn
        {	4,		0,		0	},	//059 : TIME	DELAY
        {	0,		0,		0	},	//060 : PF Control
        {	1,		1,		1	},	//061 : EXT SET OPnn
        {	1,		1,		1	},	//062 : EXT CHK    IPnn
        {	1,		0,		0	},	//063 : TR3	SET
        {	0,		0,		0	},	//064 : MODIFY
        //09.08.31
        #if(SPS_2516+SPS_3020)		//#ifdef	SPS_F3020
        {	0,		0,		0	},	//065 :
        {	0,		0,		0	},	//066 :
        {	0,		0,		0	},	//067 :
        {	0,		0,		0	},	//068 :
        {	0,		0,		0	},	//069 :
        {	0,		0,		0	},	//070 :
        {	0,		0,		0	},	//071 :
        {	0,		0,		0	},	//072 :
        {	0,		0,		0	},	//073 :
        {	0,		0,		0	},	//074 :
        {	0,		0,		0	},	//075 :
        {	0,		0,		0	},	//076 :
        {	0,		0,		0	},	//077 :
        {	0,		0,		0	},	//078 :
        {	0,		0,		0	},	//079 :
        {	0,		0,		0	},	//080 : NULL
        {	3,		3,		3	},	//081 : POCKET
        {	0,		0,		0	},	//082 : GUIDE_END
        #endif
    };


//---------------------------------------------
//---------------------------------------------