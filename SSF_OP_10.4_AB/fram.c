/**
 * File name : fram.c
 *
 * File Description : FRAM access example
 *
 * Author :
 * Dept :
 * Created Date :
 * Version    :
 * Related MC:
 */
/*
*Fram 저장 순서
*1. MSETPARA : 0 ~ sizeof(MsetPara) + 500
*2. DESIGNPARA :위에 이어서  ~ sizeof(DesignPara) + 500
*3. EMBPARA :위에 이어서  ~ sizeof(EmbPara) + 500
*4. MCPARA :위에 이어서  ~ sizeof(McPara) + 500
*5. ETCPARA :위에 이어서  ~ sizeof(ETCPara) + 500
*6. ATTACHPARA :위에 이어서  ~ sizeof(AttachPara) + 500
*7. INTERSECTPARA :위에 이어서  ~ sizeof(IntersectPara) + 500
*8. NEEDLEPARA :위에 이어서  ~ sizeof(NeedlePara) + 500
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>

#include <sys/stat.h>

//#include "swflib.h"
//#include "typeDefine.h"
#include "fram.h"
#include "fram_app.h"

//#define FRAM_TEST

//#ifdef FRAM_TEST

pthread_mutex_t FRAMMutexLock = PTHREAD_MUTEX_INITIALIZER;//2011.11.8 osi in
static long FramOffset = 0;

/**
 * Function Name : InitFram
 *
 * Function Description :
 * PC용 Compile 일 경우 초기에 fram 파일의 크기가 8192 값이 아닐 경우 0값으로 초기화된 8182(8KB)size dump 파일 생성
 * ARM용 Compile 일 경우 FRAM 존재 유무 판단
 * Input : int mcnum, 0일때 LEFT, 1일때 RIGHT,
 * Output : int : 생성 결과 size return
 *
 */

int FramInit(int mcnum)
{
	int fd=0;
	int fileSize=0;
	//int ret = 0;
#ifndef __arm__
	char buf[FRAM_SIZE]={0,};
#endif
   //qDebug() << "(FramInit) fileSize = ";// << fileSize;

	fd = open(DEV_FRAM, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);
	if (fd < 0) {
        fprintf(stderr, "(FramInit) Can't open FRAM.\n");
		return -1;
	}

    if(mcnum == 1)      FramOffset = 0;             //LEFT_MC
    else if(mcnum == 2) FramOffset = MC2_OFFSET;    //RIGHT_MC

    fileSize = lseek(fd, 0L, SEEK_END);
	lseek(fd,0L,SEEK_SET);
#ifndef __arm__
    if(fileSize != FRAM_SIZE){      // 32KBytes
		// fram file size 가 32KB 가 아니면 0으로 초기화된 32KB file create
		fileSize = write(fd,buf,FRAM_SIZE);
	}
#else //2011.11.8 osi in
	//ret = pthread_mutex_init(&FRAMMutexLock, NULL);//FRAM mutex initialize
#endif
    //fprintf(stderr, "(FramInit) fileSize 1.\n");      //sprintf("%d", fileSize);
	close(fd);

	return fileSize;
}


int FramRead(void* dest,size_t size)
{
	int fd;
	size_t n;

	fd = open(DEV_FRAM, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "Can't open FRAM.\n");
		return -1;
	}

	n = read(fd, dest, size);
	if (n != size) {
		fprintf(stderr, "Can't read FRAM.\n");
		close(fd);
		return -2;
	}

	close(fd);

	return n;
}

int FramWrite(void* dest,size_t size)
{
	int fd;
	size_t n;

	fd = open(DEV_FRAM, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "Can't open FRAM.\n");
		return -1;
	}

	n = write(fd, dest, size);
	if (n != size) {
		fprintf(stderr, "Can't write FRAM.\n");
		close(fd);
		return -2;
	}

	close(fd);

	return n;
}

//========================================
// Function Name :FramWritePara
// Function Description :
// fram 에 각 구조체에 해당하는 값을 기록한다..
// Input :void* dstPara,int index
// Output :int
// 기록한 크기
// Version :1.0
//========================================

//#if 0
int FramWritePara(void* para,int index)
{
    int fd = 0;
	int size = 0;   // parameter size
	int rtn=-1;
	int offset = 0;

#ifdef __arm__ //2011.11.8 osi in
	rtn = pthread_mutex_lock(&FRAMMutexLock);//Mutex Lock
#endif
	size = GetParaSize(index);
	// fram save offset calculate
    switch(index){      //2011.9.29 osi mody
		case SHM_INDEX_MSET:
            offset = MSETPARA_START_ADDR + FramOffset;      //FRAM_SAVE_START_ADDR (0 + 0)
			break;
		case SHM_INDEX_DESIGNPARA:
            offset = DESIGNPARA_START_ADDR + FramOffset;    // DESIGNPARA_START_ADDR (0 + 100)
			break;
		case SHM_INDEX_EMBPARA:
            offset = EMBPARA_START_ADDR + FramOffset;       // 100 + 300
			break;
		case SHM_INDEX_MCPARA:
            offset = MCPARA_START_ADDR + FramOffset;        //EMBPARA_START_ADDR(100 + 300) + EMBPARA_TOTAL_SPACE(300)
			break;
		case SHM_INDEX_ETCPARA:
			offset = ETCPARA_START_ADDR + FramOffset;
			break;
		case SHM_INDEX_ATTACHPARA:
			offset = ATTACHPARA_START_ADDR + FramOffset;
			break;
		case SHM_INDEX_INTERSECTPARA:
			offset = INTERSECTPARA_START_ADDR + FramOffset;
			break;
		case SHM_INDEX_NEEDLEPARA:
			offset = NEEDLEPARA_START_ADDR + FramOffset;
			break;
		case SHM_INDEX_COILNEEDLEPARA:
			offset = COILNEEDLEPARA_START_ADDR + FramOffset;
			break;
		case SHM_INDEX_COILPARA:
			offset = COILPARA_START_ADDR + FramOffset;
			break;
		case SHM_INDEX_CHENNEEDLEPARA:			// 2012-11-21 jjy
			offset = CHENNEEDLEPARA_START_ADDR + FramOffset;
			break;
		case SHM_INDEX_CHENPARA:				// 2012-11-21 jjy
			offset = CHENPARA_START_ADDR + FramOffset;
			break;
		default:
			break;
	}
	//    offset = FRAM_OFFSET*(index-SHM_INDEX_START) ;//2011.9.29 osi del
	fd = open(DEV_FRAM, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "Can't open FRAM.\n");
		rtn = -1;
	}
	else{
_RETRY_WRITE:
		lseek(fd,offset,SEEK_SET);
		rtn = write(fd, para, size);
		if (rtn != size) {
			if(errno == EINTR){
				goto _RETRY_WRITE;
			}
			else{
				fprintf(stderr, "Can't write FRAM.\n");
				rtn = -2;
			}
		}
	}

	close(fd);
#ifdef __arm__ //2011.11.8 osi in
	rtn = pthread_mutex_unlock(&FRAMMutexLock);//mutex unlock
#endif
	return rtn;    
}
//#endif

//========================================
// Function Name :FramReadPara
// Function Description : fram 에서 각 구조체에 해당하는 값을 읽어온다.
// Input :void* dstPara,int index
// Output :int
// 읽은 크기
// Version :1.0
//========================================
//#if 0
int FramReadPara(void* dstPara,int index)
{
	int fd =0;
	int size = 0;   // parameter size
	int rtn=-1;
	int offset = 0;

#ifdef __arm__ //2011.11.8 osi in
    rtn = pthread_mutex_lock(&FRAMMutexLock);   //Mutex Lock
#endif
	size = GetParaSize(index);
	// fram save offset calculate

	switch(index){//2011.9.29 osi mody
        case SHM_INDEX_MSET:        // 0x1001
            offset = MSETPARA_START_ADDR + FramOffset;
			break;
        case SHM_INDEX_DESIGNPARA:  // 0x1002
			offset = DESIGNPARA_START_ADDR + FramOffset;
			break;
        case SHM_INDEX_EMBPARA:     // 0x1003
			offset = EMBPARA_START_ADDR + FramOffset;
			break;
        case SHM_INDEX_MCPARA:      // 0x1004
			offset = MCPARA_START_ADDR + FramOffset;
			break;
        case SHM_INDEX_ETCPARA:     // 0x1005
			offset = ETCPARA_START_ADDR + FramOffset;
			break;
        case SHM_INDEX_ATTACHPARA:  // 0x1006
			offset = ATTACHPARA_START_ADDR + FramOffset;
			break;
		case SHM_INDEX_INTERSECTPARA:
			offset = INTERSECTPARA_START_ADDR + FramOffset;
			break;
		case SHM_INDEX_NEEDLEPARA:
			offset = NEEDLEPARA_START_ADDR + FramOffset;
			break;
		case SHM_INDEX_COILNEEDLEPARA:
			offset = COILNEEDLEPARA_START_ADDR + FramOffset;
            break;
		case SHM_INDEX_COILPARA:
			offset = COILPARA_START_ADDR + FramOffset;
			break;
		case SHM_INDEX_CHENNEEDLEPARA:
			offset = CHENNEEDLEPARA_START_ADDR + FramOffset;
			break;
		case SHM_INDEX_CHENPARA:
			offset = CHENPARA_START_ADDR + FramOffset;
			break;
		default:
			break;
	}
	//offset = FRAM_OFFSET*(index-SHM_INDEX_START) ;//2011.9.29 osi del

	//    fd = open(DEV_FRAM, O_RDWR);//2011.11.8 osi del
	fd = open(DEV_FRAM, O_RDONLY);//2011.11.8 osi mody
	if (fd < 0) {
		fprintf(stderr, "Can't open FRAM.\n");
		rtn = -1;
	}
	else{
_RETRY_READ:
		lseek(fd,offset,SEEK_SET);
		rtn = read(fd, dstPara, size);

		if (rtn != size) {
			if(errno == EINTR){
				goto _RETRY_READ;
			}
			else{
				fprintf(stderr, "Can't read FRAM.\n");
				rtn =  -2;
			}
		}
	}

	close(fd);
#ifdef __arm__ //2011.11.8 osi in
    rtn = pthread_mutex_unlock(&FRAMMutexLock);//mutex unlock
#endif
	return rtn;
}
//#endif





#if 0
int mainFram()
{
	int fd;
	int i,n;
	char buf[NBYTES];

	fd = open(DEV_FRAM, O_RDWR);
	if (fd < 0) {
		fprintf(stderr, "Can't open FRAM.\n");
		return -1;
	}

	n = read(fd, &buf, NBYTES);
	if (n != NBYTES) {
		fprintf(stderr, "Can't read FRAM.\n");
		close(fd);
		return -2;
	}

	for (i = 0; i < NBYTES; i++) {
		printf("%02x ", buf[i]);
	}
	printf("\n");

	srand(time(NULL));

	for (i = 0; i < NBYTES; i++) {
		buf[i] = rand() & 0xff;
	}

	lseek(fd, 0, SEEK_SET);

	n = write(fd, &buf, 16);
	if (n != NBYTES) {
		fprintf(stderr, "Can't write FRAM.\n");
		close(fd);
		return -3;
	}

	lseek(fd, 0, SEEK_SET);

	read(fd, &buf, NBYTES);

	for (i = 0; i < NBYTES; i++) {
		printf("%02x ", buf[i]);
	}
	printf("\n");

	close(fd);

	return 0;
}
#endif


int GetParaSize(int index)
{
    int rtn=-1;
    switch(index){
        case SHM_INDEX_MCSTATUS:
            rtn = 50;    //rtn = sizeof(MCSTATUS);
            break;
        case SHM_INDEX_MSET:
            rtn = 1;    //rtn = sizeof(MSET);
            break;
        case SHM_INDEX_DESIGNPARA:
            rtn = 1;    //rtn = sizeof(DESIGNPARA);
            break;
        case SHM_INDEX_EMBPARA:
            rtn = 1;    //rtn = sizeof(EMBPARA);
            break;
        case SHM_INDEX_MCPARA:
            rtn = 1;    //rtn = sizeof(MCPARA);
            break;
        case SHM_INDEX_ETCPARA:
            rtn = 1;    //rtn = sizeof(ETCPARA);
            break;
        case SHM_INDEX_ATTACHPARA:
            rtn = 1;    //rtn = sizeof(ATTACHPARA);
            break;
        case SHM_INDEX_INTERSECTPARA:
            rtn = 1;    //rtn = sizeof(INTERSECTPARA);
            break;
        case SHM_INDEX_NEEDLEPARA:
            rtn = 1;    //rtn = sizeof(NEEDLEPARA);
            break;
        case SHM_INDEX_COILNEEDLEPARA://2012.7.30 osi in
            rtn = 1;    //rtn = sizeof(COILNEEDLE);
            break;
        case SHM_INDEX_COILPARA://2012.7.30 osi in
            rtn = 1;    //rtn = sizeof(COILPARA);
            break;
        case SHM_INDEX_CHENNEEDLEPARA:// 2012-11-21 jjy
            rtn = 1;    //rtn = sizeof(CHENNEEDLE);
            break;
        case SHM_INDEX_CHENPARA:		// 2012-11-21 jjy
            rtn = 1;    //rtn = sizeof(CHENPARA);
            break;
    }
    return rtn;
}

