#ifndef CANWATCH_H
#define CANWATCH_H

#include <QThread>

/* header for network */
#include <net/if.h>
#include <sys/ioctl.h>

/* header for can */
#include <linux/can.h>
#include <linux/can/raw.h>

#include "canlib.h"  // can library header
#include "fdwatch.h" // fdwatch library header
#include "jasu.h"


class CanWatch : public QThread
{
    Q_OBJECT

public:
    CanWatch();
    ~CanWatch();


    int can_socket; /* can raw socket */
    //	int send_stitch(QList<read_data> st);

signals:
    void mc_cmd(int, unsigned int);
    void mc_cmd2(int, unsigned char *data);  //kimkt(130502)
    void design_save(unsigned char *data);
    void sig_screen_clr();
    void sig_design_save_call();
    void sig_design_clear_call();

    void signal_drawing_zoom_img();
    void sig_actual_value_moving(bool,bool);

    void sig_jasu_state(bool);


    //void sig_draw_icon();

protected:
    void run();

private:
    int  watch();
    int  send_can(int num, __u8 *data);
    void send_ready();
    void show_can_frame(struct can_frame *cf);


    //jd new
    void rcv_new_ht(__u8 *data,int data_length);





    void rcv_cs_boot(__u8 *data);
    void rcv_cs_setup(__u8 *data);
    void rcv_cs_ready(__u8 *data);
    void rcv_cs_start(__u8 *data);
    void rcv_cs_error(__u8 *data);
    void rcv_cs_init(__u8 *data);
    void rcv_cs_para(__u8 *data);
    void rcv_cs_test(__u8 *data);
    void rcv_cs_design(__u8 *data);
    void rcv_cs_format(__u8 *data);     //kimkt(121021)
    struct can_frame rcv_cf;
    __u8 send_data[8];

    //QString __data;     //test
    //__u8 __data;            //test
    unsigned char char_data_cnt; //kimkt(121106)
    unsigned char final_fg;      //kimkt(121106)
    unsigned char start_fg;      //kimkt(121106)
    bool punch_data_h_fg;   //, punch_data_sti_fg;    //kimkt(130228)
};

#endif // CANWATCH_H
