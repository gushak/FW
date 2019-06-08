#ifndef CANLIB_H
#define CANLIB_H


#ifdef LINUX_TEST   //TEST
    #define CAN_DEV     "vcan0"
#else
    #define CAN_DEV     "can0"
#endif

#define MY_ADDR     0x22
#define MC_ADDR     0x02
#define TARGET_MASK 0x01FFFFFFU // high 7bit ignore.

void show_can_frame(can_frame *cf);
void make_can_frame(can_frame *cf, int num, __u8 *data, char target);
int  send_can_frame(int can_socket, int num, __u8 *data, char target);

#endif //CANLIB.H
