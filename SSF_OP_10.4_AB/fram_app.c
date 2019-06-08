#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include "fram_app.h"
#include "fram.h"
//#include "typeDefine.h"
//#include "sharestuff.h"
//#include "crc.h"
//#include "swflib.h"



#define POSIX_SHM

#ifdef POSIX_SHM
#define SHM_MCSTATUS_PATH "shmMCSTATUS"

// shared memory 이름
static const char* _ShmPath[]=
{
    "MCSTATUS",
    "MSET",
    "DESIGNPARA",
    "EMBPARA",
    "MCPARA",
    "ETCPARA",
    "ATTACHPARA",
    "INTERSECTPARA",
    "NEEDLEPARA",
    "COILNEEDLEPARA",   //2012.7.30 osi in
    "COILPARA",         //2012.7.30 osi in
    "CHENNEEDLEPARA",   // 2012-11-21 jjy
    "CHENPARA"          // 2012-11-21 jjy
};

// sempore 이름
static const char* _SemPath[]=
{
    "MCSTATUS",
    "MSET",
    "DESIGNPARA",
    "EMBPARA",
    "MCPARA",
    "ETCPARA",
    "ATTACHPARA",
    "INTERSECTPARA",
    "NEEDLEPARA",
    "COILNEEDLEPARA",   //2012.7.30 osi in
    "COILPARA",         //2012.7.30 osi in
    "CHENNEEDLEPARA",   // 2012-11-21 jjy
    "CHENPARA"          // 2012-11-21 jjy
};

static int Shmfd[SHM_INDEX_END-SHM_INDEX_START];   // shared memory descriptor   0x100D - 0x1000
static sem_t* Semfd[SHM_INDEX_END-SHM_INDEX_START]; // semaphore descriptor
#if 0
// lock 기능 multi process 에서 동작 구현 필요
static pthread_rwlock_t _RWLock[SHM_INDEX_END-SHM_INDEX_START];
#endif
#else
static Share* _Pshare[SHM_INDEX_END-SHM_INDEX_START];
#endif
static int initSHM(int index);


static int initSHM(int index)
{
    int rtn=-1;
    void *data;
    int dataSize = GetParaSize(index);
    data = (void*)malloc(dataSize);
    memset(data,0,dataSize);
    SHMWrite(data, index);
    free(data);
    return rtn;
}


static int lockSHM(int index)
{
    int ret;
#if 0
    pthread_rwlock_rdlock(&_RWLock[index-SHM_INDEX_START]);
#endif
    /* need to enter critical section */
    //printf("-- waiting to enter critical section\n");
    /* looping until the lock is acquired */
    do {
_RETRY_LOCK:
        ret = sem_wait(Semfd[index-SHM_INDEX_START]);
        if (ret != 0) {
            if(errno == EINTR){
                goto _RETRY_LOCK;
            }
            /* the lock wasn't acquired */
            if (errno != EINVAL) {
                //perror("-- Error in sem_wait. terminating -> ");
                pthread_exit(NULL);
            }
            else {
                /* sem_wait() has been interrupted by a signal: looping again */
                //printf("-- sem_wait interrupted. Trying again for the lock...\n");
            }
        }
    } while (ret != 0);
    //printf("-- lock acquired. Enter critical section\n");
    /* requesting the shared segment    --  mmap() */
    return ret;
}


static int unlockSHM(int index)
{
    int ret = 0;
    // lock release
_RETRY_UNLOCK:
    ret = sem_post(Semfd[index-SHM_INDEX_START]);
    if (ret != 0) {
        if(errno == EINTR){
            goto _RETRY_UNLOCK;
        }
        perror("-- Error in sem_post");
    }
#if 0
    pthread_rwlock_unlock(&_RWLock[index-SHM_INDEX_START]);
#endif
    return ret;
}


static int initSHMlock(void)
{
    int ret = 0;
    int i=0 ;
    //create named, local semaphore, and init vilue 1(mxa concurrency 1)
    for(i = 0; i < (SHM_INDEX_END-SHM_INDEX_START) ;i++){
        Semfd[i] = sem_open(_SemPath[i], O_CREAT | O_RDWR, S_IRWXU | S_IRWXG,1);
        if (Semfd[i] == SEM_FAILED ) {
            /* error. errno has been set */
            perror("In init_semaphore()");
            return -1;
        }
    }
#if 0
    // using pthread_rwlock
    for(i = 0; i < (SHM_INDEX_END-SHM_INDEX_START-1) ;i++){
        rtn = pthread_rwlock_init(&_RWLock[i], NULL);
        if(rtn != 0) {
            perror("pthread rwlock init()");
            return -1;
        }
    }
#endif
    return ret;
}


#ifndef POSIX_SHM
static Share* getShare(int index)
{
    Share* share;

    share=_Pshare[index-SHM_INDEX_START];

    return share;
}
#endif


int SHMInit(void)
{
    int rtn = 0 ;
    int i;

    for(i = 0; i < (SHM_INDEX_END-SHM_INDEX_START) ;i++){
        /* Set the size of the SHM to be the size of the struct. */
        initSHM(SHM_INDEX_START+i);
    }

    return rtn;
}


int SHMOpen(void)
{
    int rtn = 0 ;
#ifdef POSIX_SHM
    int i;
    //int first=0;

    for(i = 0; i < (SHM_INDEX_END-SHM_INDEX_START) ;i++){
        Shmfd[i] = shm_open(_ShmPath[i], O_CREAT | O_RDWR, S_IRWXU | S_IRWXG);
        if (Shmfd[i] < 0) {
            perror("In shm_open()");
            return -1;
        }
    }
//    fprintf(stderr, "Created shared memory object %s pid=%d\n", SHM_MCSTATUS_PATH,getpid());
//    LOG("Created shared memory object %s pid=%d\n", SHM_MCSTATUS_PATH,getpid());

    for(i = 0; i < (SHM_INDEX_END-SHM_INDEX_START) ;i++){
        /* Set the size of the SHM to be the size of the struct. */
        rtn = ftruncate(Shmfd[i], GetParaSize(SHM_INDEX_START+i));
        if(rtn != 0) {
            perror("pthread ftruncate()");
            return -1;
        }
    }
    // sharedmemory lock init
    // using semapore for lock
    initSHMlock();
#else
    int i;
    for(i = 0; i < (SHM_INDEX_END-SHM_INDEX_START) ;i++){
        _Pshare[i] = new_share(i+SHM_INDEX_START,GetParaSize(i+SHM_INDEX_START)+sizeof( Header ),0666|IPC_CREAT);
    }
#endif
    return rtn;
}


int SHMRead(void *data, int index)
{
#ifdef POSIX_SHM
    int ret=0;
    void *shared_msg;
    int shmSegSize = (1 * GetParaSize(index));
    shmSegSize = 50;        //kimkt(131224)

    // lock sharedmemory
    ret = lockSHM(index);
    // atache sharedmemory
    // PROT_READ 해당 페이지 읽기 가능
    // MAP_SHARED 공유 메모리 맵 방식
    shared_msg = (void *)mmap(NULL, shmSegSize, PROT_READ, MAP_SHARED, Shmfd[index-SHM_INDEX_START], 0);
    if (shared_msg == MAP_FAILED){
        perror("In mmap()");
        // unlock sharedmemory
        ret = unlockSHM(index);
        return -1;
    }
    // critical section
    memcpy(data,shared_msg,shmSegSize);

    // deatch shared memory
    munmap(shared_msg,shmSegSize);
    // unlock sharedmemory
    ret = unlockSHM(index);

    return ret;
#else
    return read_share( getShare(index), (char*)data );
#endif
}


int SHMWrite(void *data, int index)
{
    int ret;
    unsigned short size = 0;
#ifdef POSIX_SHM
    void *shared_msg;
    int shmSegSize = (1 * GetParaSize(index));
#if 0
    pthread_rwlock_wrlock(&_RWLock[index-SHM_INDEX_START]);
#endif
    // printf(stderr, "Shared memory segment allocated correctly (%d bytes).\n", shmSegSize);
    // lock sharedmemory
    ret = lockSHM(index);

    /* requesting the shared segment    --  mmap() */
    shared_msg = (void *)mmap(NULL, shmSegSize, PROT_WRITE, MAP_SHARED, Shmfd[index-SHM_INDEX_START], 0);
    if (shared_msg == NULL) {
        perror("In mmap()");
        // unlock sharedmemory
        ret = unlockSHM(index);
        return -1;
    }
    //-----------------------2011.9.17 osi crc calcuration start
    size = GetParaSize(index);
//    CalAndSaveCRC(data,size);
    if(index != SHM_INDEX_MCSTATUS)     FramWritePara(data,index);  // 0x1000
    //-----------------------2011.9.17 osi crc calcuration end
    //++ critical section
    memcpy(shared_msg,data,shmSegSize);
    // ?? nsync,mummap를 lock 내부에 넣을지 말지 고민중 일단 write 부분에는 삾입
    msync(shared_msg,shmSegSize,MS_SYNC);
    munmap(shared_msg,shmSegSize);
    //-- critical section
    // lock release
    // unlock sharedmemory
    ret = unlockSHM(index);
#else
    sharelite_lock(getShare(index),LOCK_EX);
    ret = write_share(getShare(index),(char*)data, GetParaSize(index));
    sharelite_unlock(getShare(index));
#endif
    return ret;
}

