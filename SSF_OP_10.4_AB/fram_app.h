#ifndef __FRAM_APP_H__
#define __FRAM_APP_H__

#define SHM_INDEX_START         0x1000
#define SHM_INDEX_MCSTATUS      0x1000
#define SHM_INDEX_MSET          0x1001
#define SHM_INDEX_DESIGNPARA    0x1002
#define SHM_INDEX_EMBPARA       0x1003
#define SHM_INDEX_MCPARA        0x1004
#define SHM_INDEX_ETCPARA       0x1005
#define SHM_INDEX_ATTACHPARA    0x1006
#define SHM_INDEX_INTERSECTPARA 0x1007
#define SHM_INDEX_NEEDLEPARA    0x1008
#define SHM_INDEX_COILNEEDLEPARA 0x1009//2012.7.30 osi in
#define SHM_INDEX_COILPARA      0x100A//2012.7.30 osi in
#define SHM_INDEX_CHENNEEDLEPARA 0x100B// 2012-11-21 jjy
#define SHM_INDEX_CHENPARA      0x100C// 2012-11-21 jjy
#define SHM_INDEX_END           0x100D//2012.7.30 osi 0x1009->0x100B change// 2012-11-21 jjy 0x100B->0x100D

#ifdef __cplusplus
extern "C" {
#endif
int SHMWrite(void *data, int index);
int SHMRead(void *data, int index);
int SHMInit(void);
int SHMOpen(void);
#ifdef __cplusplus
};
#endif

#endif
