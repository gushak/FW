#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <unistd.h>

#include "iodefine.h"

#if 1 //network ver.
#include <QtNetwork>
#endif

//#include <QTcpServer>       //~~~(130105)

#include "syslog.h"         //(130110)
#include <QMouseEvent>
#include <QTranslator>      //(131001)

#include <QTime>
#include "u_file.h"
#include "d_setting.h"
#include "d_set_job_info.h"
#include <linux/input.h>

enum MODE
{
    euNONE = 0,
    euTRIM = 1,
    euSTITCH = 2
};

enum SELECT
{
    WORKER = 1,
    TARGET,
    MINIMUM,
    DELAY = 15
};
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    int process_num;
    int actual_stitch;
    int  motor_run_stop;
    int old_motor_run_stop;
    struct input_event key_event;
    QSocketNotifier *notRsRead_trim;
    QSocketNotifier *notRsRead_pulse;
    int loop_seconds;
    int memory_seconds;
    int operation_second;
    __u32 second;
    int count_gain;
    QTcpSocket *socket;
    QString srv_ip;
    QString op_ip;  //load ip
    bool operate_lock;

    QString worker_id;
    QString job_code;
    QString mc_code;
    int connect_state_cnt;

    __u8 display_target(QString *target);
    void display_worker_id(QString worker_id, QString *tmp);
    void display_item(QString *item, QString *tmp);
    void display_tirmstitch(QString *trim, __u8 stat);
    __u8 display_factor(__u16 *factor);
    void display_param(__u8 stat);

    void getbufferdata(QString *buffer, QString *buffer_code, QString *tmp, int index, bool is_multi);

    void setworkid(QString *work_id, QString *tmp);
    void settarget(QString *target, QString *tmp2);

    QString is_valid_data(QString data_tmp);

    void drow_network_info();
    void on_worker_scanning(QString name);
    void item_scanning();
    void process_scanning(QString job_name);

    QString socket_msg_box;
    QString worker_name;
    QString worker_id_number;
    QString selected_item;
    QString selected_process;
    QString target_cnt;

    /*    OP DATA STATIC     */
    QString MC_ITEM_NAME;    //ex. 경찰 내복
    QString MC_ITEM_CD;      //ex. 0000000001
    QString MC_PROCESS_NAME; //ex. 밴드제작
    QString MC_STYLE_CODE;   //ex. 18SHA02001-012
    QString MC_SENDER_STYLE_CODE_LEFT; //ex. 18SHA02001
    QString MC_SENDER_STYLE_CODE_RIGHT; //ex. P012
    QString MC_SENDER_MC_CODE; //ex. SH-P3A-3001

    QMessageBox msg;

    int m_Button_stat;
    QString *Group[11];
    __u8 public_laststat;

    bool is_multi;
    bool turning_chgd;
    void on_proc_data_clicked();
    bool get_time_info; //kimkt(190201) move here

    __u8 event_j;
    __u8 retry_j;

    __u8 paramindex;

    bool is_checking;
    bool event_apear;

signals:
    void sig_rcvd_socket();



public slots:
#if 1 //network add
    bool connectToHost(QString host);//0402
    bool writeData(QByteArray data);//0402
#endif

protected:
    void keyPressEvent(QKeyEvent *event);


private slots:
    void updateTime();
    void update_sewTime_pulse();
    void socketReadyRead();
    void edge_input_pulse();
    void on_b_setting_clicked();
    void on_b_file_list_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_4_clicked();
    void on_b_graph_view_clicked();
    void on_b_jobinfo_clicked();
    void on_b_worker_clicked();
    void on_b_tuning_clicked();
    void on_b_tuning_st_clicked();
    void on_b_tuning_ed_clicked();
    void on_b_home_clicked();
    void on_b_op_setting_clicked();
    void on_b_job_standard_clicked();
    void on_b_home_2_clicked();
    void on_b_monitor_clicked();
    void on_b_home_3_clicked();
    void on_b_countUp_clicked();
    void on_b_countDown_clicked();
    void on_b_countReset_clicked();
    void on_b_home_4_clicked();
    void on_b_set_network_clicked();
    void on_b_set_mc_code_clicked();
    void on_b_process_clicked();
    void on_b_workers_clicked();
    void on_b_standard_clicked();

//private:

//    struct Worker
//    {
//        QString strId;
//        int nCount;
//        Worker(const QString& idName, int count)
//        {
//            strId = idName;
//            nCount = count;
//        }
//        Worker()
//        {
//            strId = "";
//            nCount = 0;
//        }
//    };181106

    void on_proc_no1_pressed();
    void on_proc_no2_pressed();
    void on_proc_no3_pressed();
    void on_proc_no4_pressed();
    void on_proc_no5_pressed();
    void on_proc_no6_pressed();    
    void on_proc_no7_clicked();
    void on_proc_no8_clicked();
    void on_proc_no9_clicked();
    void on_proc_no10_clicked();
    void on_b_test_clicked();
    void on_pushButton_6_clicked();
    void on_b_testing_end_clicked();
    void on_b_sendingdefect_clicked();
    void on_b_senddefect_init_clicked();

    void on_proc_no10_2_clicked();

    void on_proc_no10_3_clicked();

    void on_pushButton_clicked();

    void on_b_stitch_to_trim_clicked();

    void on_b_interval_clicked();

private:
    __u8 init_process(__u8 &laststat, QString tmp);
    QString init_display(Ui::MainWindow *ui);
    __u8 init_load_sdcard(QString **Group);

    void set_default_worker();
    void  all_button_off(QString tmp);

    void handle_save(int Button, QString *tmp, QString **Group);
    void handle_sava_all(QString **Group);

    void parse_sendercode(__u8 Button);

    __u8 memory_in_processcnt(__u8 count, __u8 localstat, QString **Group);
    __u8 memory_in_laststat(__u8 laststat);
    __u8 memory_in_worker(QString worker, QString **Group, __u8 localstat);

    void status_change(int stat);
    void display_actual_cnt(QString *tmp,__u8 stat);
    void display_both(Ui::MainWindow *ui, __u16 trim, __u16 stitch);

    void wifi_connect();
    void set_static_ip();
    bool numtostring(QString tmp, int num);

    int  m_Button_Window;

    bool m_istest;
    __u8 m_istune;

    __u16 m_cntbuff;

    void updateDate();
    int compare_value;
    //void wait_ack(__u8 ack);
    QTimer *updateTimer;
    QString date,time;

    Ui::MainWindow *ui;

    void send_count(__u8 index);
    void send_init();
    void try_connector();
    void showFtpTreeViewMenu(const QPoint &point );
    void show_job_instruction();
    void request_job_instruction();
    void copy_tftp_boot(QString);

    void request_time();
    void working_date_check();
    void setting_date_linux();
    void draw_the_graph();
    void send_countByUP(__u8 index);
    void send_countByDN(__u8 index);
    void handle_nack(QString msg);

    void delay_ms(__u32 delay);

    void alive_checking();

    //bool get_time_info;   //delete kimkt(190201)
    bool mouse_event_lock;
    float job_img_scale;

    __u16 fd_pulse;
    __u32 old_stitch;
    __u32 update_stitch;

    double update_second;
    double old_second;
    double m_diff;

    double after_stitch;
    double before_stitch;

    bool m_iserrorsend;
    bool m_is_valid_trim;
    __u8 retry_state;

    bool is_stop;
    bool m_ret;

    bool server_state;
    __u8 alive;

    __u8 init_try;

    bool is_recv;

    QStringList ListCnt;

    QString txbuffer;

    bool connect_param;
    //    QList<Worker> workerList; 181106
//    UserInfo* _userInfo;    //count by worker

};
extern MainWindow *w;
#endif // MAINWINDOW_H
