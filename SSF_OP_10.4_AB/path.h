    #ifndef PATH_H
#define PATH_H

#define IMAGE_PATH      ":/images"
#define IMAGE_BG_PATH	":/images/bg/"                  //(120825)
#define IMAGE_BU_PATH	":/images/button/"              //(120825)
#define IMAGE_ICON_PATH	"/SunStar/app/lib/"             //(130419)



#define SPS_PATH    	"/app/data"
#define SPS_LOAD_PATH   SPS_PATH"/001"



#define WIFI2_CFG       "/app/data/mac"



#define USB_PATH        "/media/sda1/SunStar/sps"

#ifdef LINUX_TEST
#define USB_OLD_PATH    "/SPS"
#else
#define USB_OLD_PATH    "/media/sda1/SPS"               //(130205)
#endif

#define ERR_FILE        "/SunStar/data/opt/err.info"
#define NET_CFG         "/SunStar/data/save/net.cfg"    //(120906)
#define NET_CMD         "/SunStar/app/bin/op_setup"
#define OPT_PATH        "/SunStar/data/opt"
#define SAVE_PATH       "/SunStar/data/save"
#define PRJ_FILE        SAVE_PATH"/work.prj"
#define DATE_CMD        "/SunStar/app/bin/date_setup"   //(120723)
#define UPDATE_CMD      "/SunStar/app/bin/update.sh"    //(120822)
#define MOV_PATH        "/SunStar/data/mov"             //(120827)
#define MOV_TRIM_PATH   "/SunStar/data/mov/trim"        //(120828)
#define MOV_PART_PATH   "/SunStar/data/mov/part"        //(120831)
#define MOV_HOOK_PATH   "/SunStar/data/mov/hook"        //(120831)
#define DATA_ROOT       "/SunStar/data"                 //(130107)

#define PATH_LINUX      "/home/kimkt/sd_card/build-sdms1_pattern/part6/mov"

#define BIN_PATH        "/SunStar/app/bin"      //(130103)
#define SPKR            BIN_PATH"/spkr"         //(130103)


//----- mode type -----     //(130221)
#define MD_PROJ         1
#define MD_MEM          0

//----- memory type -----     //(120907)
#define LOAD_MEM        2
#define LOAD_USB        3
#define LOAD_PRD        4
#define LOAD_USB_OLD    5     //(130206)


//----- current status -----
#define CS_BOOT         0xA0 // main 1 page / booting ...
#define CS_SETUP        0xA1 // main 1 page
#define CS_READY        0xA2 // main 1 page
#define CS_START        0xA3 // main 1 page / sewing ...
#define CS_ERROR        0xA4 // main 1 page
#define CS_INIT         0xA5 // menu -> init
#define CS_PARA         0xA6 // menu -> para
#define CS_TEST         0xA7 // menu -> test
//#define CS_DESIGN       0xA8 // menu -> design
#define CS_PUNCHING     0xA8 // menu -> design      //(121117)
#define CS_FORMAT       0xA9 // format

// flow status
#define F_BOOT_ON       0xC0
#define F_ACK_BOOT_ON   0xC1
#define F_RCVD_PARAM    0xC2
#define F_READY_OK      0xC3
#define F_SEND_ENTER    0xC4
#define F_ACK_ENTER     0xC5
#define F_SEND_HEADER   0xC6
#define F_SEND_STITCH   0xC7
#define F_ACK_OK        0xC8
#define F_REQUST        0xC9
#define F_UPDATE_START  0xCA
#define F_UPDATE_END    0xCB
#define F_INIT_START    0xCC
#define F_INIT_END      0xCD
#define F_ERROR         0XCE
#define F_ERROR_ESC     0xCF
#define F_ACK_SKIP      0xD0    //(121009)
#define F_ACK_SEND      0xD1
#define F_ACK_FAIL      0xD2
#define F_START_PARAM   0xD3    //(121011)
#define F_INS_START     0xD4    //(121021)
#define F_FORMAT_END    0xD5    //(121021)
#define F_UPDATE_MC_START   0xD6    //(121025)



enum _work_prj {
	_prj_prd = 0,
	_prj_nm,
	_prj_prc,
	_prj_mbr,
	_prj_wdt,
};



#endif // PATH_H
