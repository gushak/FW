#ifndef FRAM_H
#define FRAM_H

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __arm__ //2011.9.29 osi mody
#define DEV_FRAM	"/sys/bus/spi/devices/spi1.3/eeprom"
#else
#define DEV_FRAM	"./FramTest"
#endif


#define NBYTES              16
#define FRAM_OFFSET         500
#define FRAM_SIZE           (32*1024)           // 32KBytes Fram total size
#define MC2_OFFSET          (14*1024)           // DUAL MC2의 파라메터 저장은 14kbyte이후 부터 사용
#define SPECIAL_MC_OFFSET   (14*1024)           // 특종 자수기 파라메터중 특종 헤드에 관한 파라메커는 14kbyte 이후 저장


//FRAM stock para structure block address define//각 MC당 10kbyte 공간
#define FRAM_SAVE_START_ADDR        0           // Normal MC나 DUAL MC1 은 '0'부터, DUAL MC2나 특종은 '14Kbyte' 부터 시작

#define MSETPARA_START_ADDR         FRAM_SAVE_START_ADDR
#define MSETPARA_TOTAL_SPACE        100         //total space is 100byte
#define MSETPARA_USE_SPACE          MSETPARA_TOTAL_SPACE - sizeof(MSET)     // 100

#define DESIGNPARA_START_ADDR       MSETPARA_START_ADDR + MSETPARA_TOTAL_SPACE      // 0 + 100
#define DESIGNPARA_TOTAL_SPACE      300         //total space is 300byte
#define DESIGNPARA_USE_SPACE        DESIGNPARA_TOTAL_SPACE - sizeof(DESIGNPARA)     // 300 -

#define EMBPARA_START_ADDR          DESIGNPARA_START_ADDR + DESIGNPARA_TOTAL_SPACE  // 100 + 300
#define EMBPARA_TOTAL_SPACE         300         //total space is 300byte
#define EMBPARA_USE_SPACE           EMBPARA_START_ADDR - sizeof(EMBPARA)

#define MCPARA_START_ADDR           EMBPARA_START_ADDR + EMBPARA_TOTAL_SPACE
#define MCPARA_TOTAL_SPACE          300         //total space is 300byte
#define MCPARA_USE_SPACE            MCPARA_TOTAL_SPACE - sizeof(MCPARA)

#define ETCPARA_START_ADDR          MCPARA_START_ADDR + MCPARA_TOTAL_SPACE
#define ETCPARA_TOTAL_SPACE         300         //total space is 300byte
#define ETCPARA_USE_SPACE           ETCPARA_TOTAL_SPACE - sizeof(ETCPARA)

#define ATTACHPARA_START_ADDR       ETCPARA_START_ADDR + ETCPARA_TOTAL_SPACE
#define ATTACHPARA_TOTAL_SPACE      300         //total space is 300byte
#define ATTACHPARA_USE_SPACE        ATTACHPARA_TOTAL_SPACE - sizeof(ATTACHPARA)

#define INTERSECTPARA_START_ADDR    ATTACHPARA_START_ADDR + ATTACHPARA_TOTAL_SPACE
#define INTERSECTPARA_TOTAL_SPACE   300         //total space is 300byte
#define INTERSECTPARA_USE_SPACE     INTERSECTPARA_TOTAL_SPACE - sizeof(INTERSECTPARA)

#define NEEDLEPARA_START_ADDR       INTERSECTPARA_START_ADDR + INTERSECTPARA_TOTAL_SPACE
#define NEEDLEPARA_TOTAL_SPACE      8000        //total space is 8000byte
#define NEEDLEPARA_USE_SPACE        NEEDLEPARA_TOTAL_SPACE - sizeof(NEEDLEPARA)

#define COILNEEDLEPARA_START_ADDR   SPECIAL_MC_OFFSET
#define COILNEEDLEPARA_TOTAL_SPACE  5000
#define COILNEEDLEPARA_USE_SPACE    COILNEEDLEPARA_TOTAL_SPACE - sizeof(COILNEEDLE)

#define COILPARA_START_ADDR         SPECIAL_MC_OFFSET + COILNEEDLEPARA_TOTAL_SPACE
#define COILPARA_TOTAL_SPACE        500
#define COILPARA_USE_SPACE          COILPARA_TOTAL_SPACE - sizeof(COILPARA)

#define CHENNEEDLEPARA_START_ADDR   SPECIAL_MC_OFFSET
#define CHENNEEDLEPARA_TOTAL_SPACE  5000
#define CHENNEEDLEPARA_USE_SPACE    CHENNEEDLEPARA_TOTAL_SPACE - sizeof(CHENNEEDLE)

#define CHENPARA_START_ADDR         SPECIAL_MC_OFFSET + CHENNEEDLEPARA_TOTAL_SPACE
#define CHENPARA_TOTAL_SPACE        500
#define CHENPARA_USE_SPACE          CHENPARA_TOTAL_SPACE - sizeof(CHENPARA)


//#define SHM_INDEX_START         0x1000
//#define SHM_INDEX_MCSTATUS      0x1000
//#define SHM_INDEX_MSET          0x1001
//#define SHM_INDEX_DESIGNPARA    0x1002
//#define SHM_INDEX_EMBPARA       0x1003
//#define SHM_INDEX_MCPARA        0x1004
//#define SHM_INDEX_ETCPARA       0x1005
//#define SHM_INDEX_ATTACHPARA    0x1006
//#define SHM_INDEX_INTERSECTPARA 0x1007
//#define SHM_INDEX_NEEDLEPARA    0x1008
//#define SHM_INDEX_COILNEEDLEPARA 0x1009//2012.7.30 osi in
//#define SHM_INDEX_COILPARA      0x100A//2012.7.30 osi in
//#define SHM_INDEX_CHENNEEDLEPARA 0x100B// 2012-11-21 jjy
//#define SHM_INDEX_CHENPARA      0x100C// 2012-11-21 jjy
//#define SHM_INDEX_END           0x100D//2012.7.30 osi 0x1009->0x100B change// 2012-11-21 jjy 0x100B->0x100D



int FramInit(int mcnum);
int FramRead(void* dest,size_t size);
int FramWrite(void* dest,size_t size);
int FramWritePara(void* para,int index);
int FramReadPara(void* para,int index);


extern int GetParaSize(int index);
//#define ARRAY_SIZE(x)   ( sizeof((x))/sizeof((x[0])) )


#ifdef __cplusplus
};
#endif

#endif // FRAM_H
