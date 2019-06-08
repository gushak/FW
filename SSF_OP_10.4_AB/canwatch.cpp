#include <unistd.h>
#include <sys/errno.h>

#include "mainwindow.h"
#include "path.h"
#include "iodefine.h"       //kimkt(120905)

#include "u_file.h"

#include <QDebug>

extern MainWindow *w;


CanWatch::CanWatch()
{
    can_socket  = -1;
}

CanWatch::~CanWatch()
{
    fdwatch_del_fd(can_socket);
    fdwatch_logstats(1000);
    close(can_socket);
}

void CanWatch::run()
{
    watch();
}

void CanWatch::show_can_frame(can_frame *cf)
{
    char msg[256];

    sprintf(msg, "[RCVD] my_addr: %02x mc_addr: %02x", (cf->can_id >> 8) & 0xFF, cf->can_id & 0xFF);
    sprintf(msg, "%s [%d]", msg, cf->can_dlc);
    for (int i=0; i<cf->can_dlc; i++)   sprintf(msg, "%s %02x", msg, cf->data[i]);
    //qDebug() << msg;
}

int  CanWatch::send_can(int num, __u8 *data)
{
    return send_can_frame(can_socket, num, data, MC_ADDR);
}


void CanWatch::send_ready()
{
    qDebug()<<"send ready";
    send_data[0] = K_BOOT_ON;
    send_can_frame(can_socket, 1, send_data, MC_ADDR);
}



int CanWatch::watch()
{
    int num_ready;
    int i;
    int fd;
    char str[10];

    int nbytes;
    int max_connects;

    struct sockaddr_can addr;
    struct ifreq ifr;

    /* open socket */
    if ((can_socket = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("socket");
        return 1;
    }

    ((MainWindow *)parent())->can_socket = can_socket;

    addr.can_family = AF_CAN;

    strcpy(ifr.ifr_name, CAN_DEV);
    if (ioctl(can_socket, SIOCGIFINDEX, &ifr) < 0) {
        perror("SIOCGIFINDEX");
        return 1;
    }
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(can_socket, (struct sockaddr *)&addr, sizeof(addr)) < 0){
        perror("bind");
        return 1;
    }

    max_connects = fdwatch_get_nfiles();
    if (max_connects < 0) {
        //qDebug() << "(watch) fdwatch initialization failure";
        exit(1);
    }

    sprintf(str, CAN_DEV);
    fdwatch_add_fd(can_socket, str, FDW_READ);

    i = 0;
    //qDebug()<<"send ready";

    //send_ready();

    /* Main loop. */
    while(1){
        /* Do the fd watch. */
        num_ready = fdwatch(5000); // 5s
        if(num_ready < 0){
            if(errno == EINTR || errno == EAGAIN)	continue;   /* try again */
            return 1;
        }

//        if(!w->boot_ok)
//        {
//                i++;
//                //qDebug() << "(watch) timeout i" << i;
//                if (i == 3)
//                {
//                    qDebug()<<"boot check fail";    //mc_cmd(R_SEW_ERROR, 20);
//                    w->boot_ok=true;
//                }
//                send_ready();
//            continue;
//        }



        for(i=0; i<num_ready; i++){
            fd = fdwatch_get_fd(i);
            if(!fdwatch_check_fd(fd))  continue;

            if((nbytes = read(can_socket, &rcv_cf, sizeof(can_frame))) < 0){
                perror("read can");
                return 1;
            }

            if(rcv_cf.can_id & CAN_RTR_FLAG){
                //qDebug() << "remote request";
            }
            else{
                if(MY_ADDR != ((rcv_cf.can_id >> 8) & 0xFF)){
                    //if(MY_ADDR != ((rcv_cf.can_id) & 0xFF)){
                    //qDebug() << "my address not match";
                    continue;
                }
                //show_can_frame(&rcv_cf);
                //recived_can(rcv_cf.data);
                rcv_new_ht(rcv_cf.data,rcv_cf.can_dlc);
            }
        }
        continue;
    }
    return 0;
}


void CanWatch::rcv_new_ht(__u8 *data,int data_length)
{
    __u8 var;
    int  val;
    //int temp_val;

    val=data_length;
    //qDebug()<<"data length : "<<data_length;
    var = data[0];
    //val = 0;

    // qDebug()<<"design_mode : "<<w->design_mode;

    if(w->design_mode && var != 0xB3)
    {
        if(data_length==8)
        {
            w->st.can_value1 = data[0];
            w->st.can_value2 = data[1];
            w->st.can_value3 = data[2];
            w->st.can_value4 = data[3];
            w->stitch_list.append(w->st);
            w->st.can_value1 = data[4];
            w->st.can_value2 = data[5];
            w->st.can_value3 = data[6];
            w->st.can_value4 = data[7];
            w->stitch_list.append(w->st);
        }else if(data_length==4){
            w->st.can_value1 = data[0];
            w->st.can_value2 = data[1];
            w->st.can_value3 = data[2];
            w->st.can_value4 = data[3];
            w->stitch_list.append(w->st);
        }
    }else{
        //qDebug()<<"var:"<<var<<"data[1]"<<data[1];
        switch((int)var)
        {
        case R_SCREEN_CLR:      // 0xB3
            if(data[1]==100) //clear and receive start  0xB3
            {
                w->design_mode=1;
                w->custom_zoom_level=1;
                //qDebug()<<"start receive design";
                emit sig_screen_clr();
            }else if(data[1]==101){  // design receive stop 0xB3
                //qDebug()<<"ending receive design";
                //qDebug()<<"start save";
                w->design_mode=0;
                emit sig_design_save_call();
            }else if(data[1]==102){
                //w->job_running=true;
                //color paint reset signal
                //w->move_origin_point_cursor();
                w->history_stitch=0;
                w->job_running=true;
                emit sig_design_clear_call();

                while(w->job_running)
                {
                    //qDebug()<<"while"<<w->job_running;
                    usleep(100);
                }



                if(w->custom_zoom_level==1) //normal size
                {
                    w->init_design_pattern();
                }else if(w->custom_zoom_level==2){//zoom
                    w->job_running=true;
                    emit signal_drawing_zoom_img();

                    while(w->job_running)
                    {
                        //qDebug()<<"while"<<w->job_running;
                        usleep(100);
                    }
                    //jasu->zoom_set_sewing_area(400,400,400,400,custom_zoom_level,event->x(),event->y());

                    //add action
                    //zoom_save_fg=true;
                    //w->enable_f_jasu(false);
                    emit sig_jasu_state(false);
                    usleep(100);
                    w->init_design_pattern();
                    emit sig_jasu_state(true);
                    //w->enable_f_jasu(true);
                    //zoom_save_fg=false;

                    //jasu->move_zoom_point(0,0);

                    //jasu->move_origin_point();
                }
                //w->job_running=false;
                //qDebug()<<"color paint clear!";
            }else{
                //qDebug()<<"nothing 0xB3";
            }
            break;
        case R_MOVING_CURSOR:
            w->color_paint_design((data[1] * 256) + data[2]);
            break;

        case R_COUNT_VALUE:
            if(data[1]==0){ //total product count init
                //qDebug()<<"clear count";
                emit    sig_actual_value_moving(true,true);
            }else{
                //qDebug()<<"count +1";
                emit    sig_actual_value_moving(false,true);
            }
            //qDebug()<<"thisthis";

            break;
        case R_MOVE_JUMP:
            w->color_paint_jump((data[1] * 256) + data[2]);
            break;
        case R_WARNING:
            if(data[1]==1)
            {
                w->do_drow_icon();
            }
            break;
        case R_OPERATE:
            if(data[1]==1)
            {
                w->operate=true;
            }else{
                w->operate=false;
            }
            break;
        case R_PATTERN_NUM:
            w->write_pttr_number((data[1] * 256) + data[2]);
            break;

        case R_SEND_READY:
            w->boot_ok=true;

            break;
        default:
            //qDebug()<<"invalid value";
            break;
        }
    }
}


