#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include <sys/socket.h> /* for sa_family_t */
#include <linux/can.h>
#include <linux/can/error.h>

#include <QDebug>
#include "canlib.h"

#define CANID_DELIM '#'
#define DATA_SEPERATOR '.'

#define MAX_CANFRAME      "12345678#01.23.45.67.89.AB.CD.EF"
#define MAX_LONG_CANFRAME_SIZE 256


const int Extended_mode = 1;


void show_can_frame(can_frame *cf)
{
	char msg[256];

	sprintf(msg, "<SEND> my_addr: %02x mc_addr: %02x",
			cf->can_id & 0xFF, (cf->can_id >> 8) & 0xFF);
	sprintf(msg, "%s [%d]", msg, cf->can_dlc);
    for (int i=0; i<cf->can_dlc; i++)	sprintf(msg, "%s %02x", msg, cf->data[i]);
    qDebug() << msg;
}

void make_can_frame(can_frame *cf, int num, __u8 *data, char target)
{
	__u32 tmp;

	memset(cf, 0, sizeof(*cf));
	if (Extended_mode) {
		tmp = (((target & 0xFF) << 8) | (MY_ADDR & 0xFF)) & TARGET_MASK;
		cf->can_id |= CAN_EFF_FLAG;
		cf->can_id |= tmp & CAN_EFF_MASK;
		cf->can_dlc = num;

        for (int i=0; i<num; i++)		cf->data[i] = *data++;
	} else {
		// Standard_mode
	}
}

int send_can_frame(int can_socket, int num, __u8 *data, char target)
{
	int nbytes;
	struct can_frame cf;
    //qDebug()<<"can send";
	make_can_frame(&cf, num, data, target);
    //show_can_frame(&cf);

    if ((nbytes = write(can_socket, &cf, sizeof(can_frame))) != sizeof(can_frame))
    {
		perror("write");
		return 1;
	}
	return 0;
}



