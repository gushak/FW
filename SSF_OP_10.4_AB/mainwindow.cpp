#include <QtGui>
#include <QPixmap>
#include <QFile>            //(130303)

#include <QtGui/QApplication>
#include <QtGui>
#include <QtCore>

#if 1 //network ver.
#include <QtNetwork>
#endif

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "iodefine.h"
#include "field.h"
#include "path.h"
#include "u_file.h"
#include "d_setting.h"
#include "d_set_code.h"
#include "d_set_para.h"
#include "d_set_job_info.h"
#include "d_setting.h"
#if 1//keytest
#include <qsocketnotifier.h>
#include <sys/types.h>
#include <fcntl.h>
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QString debug;

    for(__u8 i = 0; i < 10; i++)
        Group[i] = NULL;

    for (__u8 i = 0; i < 10; i++)
        Group[i] = new QString();

    *Group[0] = "1"; //replace to QStringList
    *Group[1] = "2";
    *Group[2] = "3";
    *Group[3] = "4";
    *Group[4] = "5";
    *Group[5] = "6";
    *Group[6] = "7";
    *Group[7] = "8";
    *Group[8] = "9";
    *Group[9] = "10";

    ui->setupUi(this);

    fd_pulse = ::open("/dev/input/event1",O_RDWR);
    QSocketNotifier *notRsRead_pulse;
    notRsRead_pulse = new QSocketNotifier(fd_pulse,QSocketNotifier::Read, this);
    connect(notRsRead_pulse, SIGNAL(activated(int)),this, SLOT(edge_input_pulse()));

#ifdef  ARM_TEST
    QCursor c = cursor();
    c.setPos(mapToGlobal(QPoint(800,600)));
    c.setShape(Qt::BlankCursor);
    setCursor(c);
#endif

    get_time_info=false;
    compare_value=0;
#if 1 //network ver
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(socketReadyRead()));
#endif

    init_load_sdcard(Group);
    bool init_success = init_process(public_laststat, debug);
    qDebug()<< "init"<< init_display(ui);
    qDebug() << "init res"<< init_success;

    updateTimer = new QTimer(this);
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(update_sewTime_pulse()));
    updateTimer->start(1000);

    motor_run_stop = 0;
    old_motor_run_stop = 0;
    m_istest = 0;
    memory_seconds=0;
    loop_seconds=0;
    mouse_event_lock = false;
    operate_lock = false;
    turning_chgd = false;
    second = 0;
    operation_second = 0;
    update_second = 0;
    old_second = 0;
    before_stitch = 0;
    after_stitch = 0;
    m_iserrorsend = 0;
    m_diff = 0;
    paramindex = 0;
    is_stop = 0;
    m_ret = 0;
    server_state = false;
    connect_state_cnt=0;
    event_apear = false;
    alive = 0;
    event_j = 0;
    retry_j = 10;
    is_checking = false;
    init_try = 0;
    connect_param = 0;
}

/***************************count by worker***************************/
MainWindow::~MainWindow()
{
    delete notRsRead_pulse;
    delete notRsRead_trim;
    delete ui;
    for (int i = 0; i < 10; i ++)
        delete Group[i];
}

__u8 MainWindow::init_process(__u8 &public_laststat, QString tmp)
{
    if (!public_laststat)
    {
        qDebug() << "none process data";
        public_laststat = 1;
    }

    status_change(public_laststat);

    m_Button_Window = public_laststat;

    set_default_worker();
    draw_the_graph();
    qDebug() << "init] item"<< tmp;

    set_static_ip();
    wifi_connect();
    event_apear = true;
    is_checking = false;

    return public_laststat;
}

QString MainWindow::init_display(Ui::MainWindow *ui)
{
    ui->wifi_state_2->show();

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    ui->l_per_titile->setText("1공정당");
    ui->l_trim_title->setText("현재 사절 수");
    ui->l_stitch_title->setText("현재 땀 수");

    QTextCodec::setCodecForTr(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("eucKR"));

    ui->l_job_bg->hide();
    job_img_scale=1;

    ui->l_oper_sec->hide();
    ui->grp_setting_menu->hide();
    ui->grp_menu->hide();
    ui->grp_tuning->hide();
    ui->grp_job_info->hide();
    ui->grp_monitor->show();
    ui->old_grp->hide();
    ui->l_item_name->show();
    ui->l_wait->hide();
    ui->progressBar->hide();
    ui->l_testing->hide();
    ui->b_testing_end->hide();
    ui->l_sendingdefect->hide();
    ui->b_sendingdefect->hide();
    ui->l_stitch_to_trim->show();
    ui->l_stitch_to_trim_var->show();
    ui->l_interval->show();
    ui->l_interval_var->show();

     return "sucess";
}

__u8 MainWindow::init_load_sdcard(QString **Group)
{

    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,SPS_PATH);
    QSettings m_settings(".","ssf.ini");

    for (int i = 0; i < 10; i++)
    {
        m_settings.beginGroup(*Group[i]);
        parsedata[i].selected_item=m_settings.value("item-name","NONE").toString();                      //process
        parsedata[i].worker_id=m_settings.value("worker_id","NONE").toString();
        parsedata[i].MC_PROCESS_NAME=m_settings.value("MC_PROCESS_NAME","NONE").toString();              //process detail
        parsedata[i].MC_SENDER_STYLE_CODE_LEFT=m_settings.value("style-name","NONE").toString();
        parsedata[i].MC_SENDER_STYLE_CODE_RIGHT=m_settings.value("style-code","NONE").toString();
        parsedata[i].MC_ITEM_CD = m_settings.value("mc_item_cd","NONE").toString();
        parsedata[i].MC_STYLE_CODE=m_settings.value("style-full-name","NONE").toString();
        parsedata[i].trim_cnt = m_settings.value("trim_cnt", "0").toInt();
        parsedata[i].stitch_cnt = m_settings.value("stitch_cnt" "0").toInt();
        parsedata[i].modestat = m_settings.value("mode", "0").toInt();
        parsedata[i].process_cnt = m_settings.value("process_cnt", "0").toInt();
        parsedata[i].MC_ITEM_NAME = m_settings.value("MC_ITEM_NAME", "NONE").toString();
        parsedata[i].target_cnt = m_settings.value("target_cnt", "314").toString();
        parsedata[i].count = m_settings.value("count", "0").toInt();
        parsedata[i].gain = m_settings.value("gain", "5").toInt();
        parsedata[i].stitch_to_trim = m_settings.value("stitch_to_trim", "10").toInt();
        parsedata[i].interval = m_settings.value("interval", "0").toInt();
        m_settings.endGroup();
    }

    qDebug()<<"Load ok";

    for (int i = 0; i<10; i ++)
        qDebug()<<"process_cnt]" << parsedata[i].process_cnt;

    public_laststat = m_settings.value("public_laststat", "0").toInt();
    MC_SENDER_MC_CODE = m_settings.value("MC_SENDER_MC_CODE", "NONE").toString();

    qDebug()<<"MACHINE]"<<MC_SENDER_MC_CODE;

    if(m_settings.contains("ip_srv"))
    {
        srv_ip = m_settings.value("ip_srv").toString();
    }
    else{
        srv_ip = "192.168.123.99";
    }

    if(m_settings.contains("ip_addr"))
    {
        op_ip = m_settings.value("ip_addr").toString();
    }
    else{
        op_ip = "192.168.123.200";
    }

    qDebug() << "srv_ip" <<  srv_ip <<"op_ip" << op_ip;

    memory_seconds = m_settings.value("mem_second","0").toInt();

    if(memory_seconds==0)
    {
        ui->debug->setText("Did not tuning");
    }
    else{
        updateTimer->start(1000);
        second=memory_seconds;
    }

    count_gain=2;

    ui->test_prog->setMaximum(0);
    ui->test_prog->setMinimum(0);
    ui->test_prog->hide();

    ui->b_ftp_list->hide();
    ui->b_file_list->hide();
    ui->label_3->hide();
    ui->label_5->hide();
    ui->data->hide();
    ui->label_6->hide();
    ui->data_2->hide();

    return public_laststat; // err process need
}

void MainWindow::set_static_ip()
{

#ifdef __arm__

    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,"/app/data/");
    QSettings m_settings(".","ssf.ini");

    QString ip_command;
    QString echo_command;
    system("ifconfig wlan0 down");
    system("sync");
    system("sync");
    system("sync");

    ip_command.clear();
    ip_command = "ifconfig wlan0 ";
    ip_command.append(m_settings.value("ip_addr", "192.168.123.199").toString());
    ip_command.append(" netmask ");
    ip_command.append(m_settings.value("ip_subnet", "255.255.255.0").toString());

    echo_command="echo ";
    echo_command.append(ip_command);
    echo_command.append(" > /app/ip_setting.sh");

    system(echo_command.toLocal8Bit());
    system("sync");
    system("sync");
    system("sync");

    system("chmod 777 /app/ip_setting.sh");
    system("sync");
    system("sync");
    system("sync");

    system("sh /app/ip_setting.sh");
    system("sync");
    system("sync");
    system("sync");

    system("ifconfig wlan0 up");
    system("sync");
    system("sync");
    system("sync");

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif
}

void MainWindow::wifi_connect()
{
    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,"/app/data/");
    QSettings m_settings(".","ssf.ini");

    QString ip_command;
    QString echo_command;
#ifdef __arm__
    //*iwlist wlan0 scan|grep -A 4 "global_1"|grep Encryption|awk '{print $2}'|sed 's/key://g'

    QString ssid_command;
    echo_command.clear();
    QString pw_command;

    system("ifconfig wlan0 down");
    system("sync");

    system("killall wpa_supplicant");
    system("sync");

    system("echo ctrl_interface=/var/run/wpa_supplicant > /etc/wpa_supplicant.conf");
    system("sync");

    system("echo ap_scan=1 >> /etc/wpa_supplicant.conf");
    system("sync");

    system("echo network={ >> /etc/wpa_supplicant.conf");
    system("sync");

    system("echo key_mgmt=WPA-PSK >> /etc/wpa_supplicant.conf");
    system("sync");

    /*ssid*/
    system("echo -n ssid='\"' >> /etc/wpa_supplicant.conf");
    system("sync");

    ssid_command.clear();
    ssid_command = m_settings.value("ssid", "global_1").toString();
    ssid_command.append("'\"'");

    echo_command="echo -e \\";
    echo_command.append(ssid_command);
    echo_command.append(" >> /etc/wpa_supplicant.conf");

    system(echo_command.toLocal8Bit());
    system("sync");
    system("sync");
    system("sync");

    /*pw*/
    system("echo -n psk='\"' >> /etc/wpa_supplicant.conf");
    system("sync");

    ssid_command.clear();
    ssid_command = m_settings.value("ssid_pw", "12345678").toString();;
    ssid_command.append("'\"'");

    echo_command="echo -e \\";
    echo_command.append(ssid_command);
    echo_command.append(" >> /etc/wpa_supplicant.conf");

    system(echo_command.toLocal8Bit());
    system("sync");
    system("sync");
    system("sync");

    system("echo } >> /etc/wpa_supplicant.conf");
    system("sync");

    system("ifconfig wlan0 up");
    system("sync");
    system("sync");
    system("sync");

    system("/usr/sbin/wpa_supplicant -iwlan0 -c /etc/wpa_supplicant.conf &");
    system("sync");
    system("sync");
#endif
}

void MainWindow::set_default_worker()
{
    for (__u8 i = 0; i < 10; i++)
    {

        if (parsedata[i].worker_id.contains("NONE"))
            parsedata[i].worker_id.replace(0, 4, "SUNSTAR");
    }

}

void MainWindow::status_change(int stat)
{
    QString tmp;
    QString tmp2;

    all_button_off(tmp);

    if (!stat)
    {
        qDebug() << "error stat in change";
        return;
    }

    for (__u8 i = 0; i < 10; i++)
        Group[i] = new QString();

    *Group[0] = "1"; //replace to QStringList
    *Group[1] = "2";
    *Group[2] = "3";
    *Group[3] = "4";
    *Group[4] = "5";
    *Group[5] = "6";
    *Group[6] = "7";
    *Group[7] = "8";
    *Group[8] = "9";
    *Group[9] = "10";

    parsedata[stat -1].trim_cnt = 0;
    parsedata[stat -1].stitch_cnt = 0;

    update_stitch = 0;
    old_stitch = 0;

    getbufferdata(buffer, buffer_code, &tmp, WORKER, is_multi);
    getbufferdata(buffer, buffer_code, &tmp2, TARGET, is_multi);
    setworkid(&parsedata[stat -1].worker_id, &tmp);
    settarget(&parsedata[stat -1].target_cnt, &tmp2);
    parse_sendercode(stat);
    //handle_save(stat, &tmp, Group);
    memory_in_laststat(stat);
    memory_in_processcnt(parsedata[stat -1].process_cnt, stat, Group);
    memory_in_worker(parsedata[stat -1].worker_id, Group, stat);

    display_item(&parsedata[stat -1].MC_ITEM_NAME, &tmp);
    display_worker_id(parsedata[stat -1].worker_id, &tmp);
    display_tirmstitch(&tmp, stat);
    display_actual_cnt(&tmp, stat);
    display_target(&parsedata[stat -1].target_cnt);
    display_both(ui, parsedata[stat -1].trim_cnt, parsedata[stat -1].stitch_cnt);
    display_factor(&parsedata[stat -1].gain);
    display_param(stat);
    draw_the_graph();

    qDebug() << "status change clear";

    switch(stat)
    {
            case 1:
            if (parsedata[stat -1].modestat && !parsedata[stat -1].MC_PROCESS_NAME.contains("NONE"))
                    ui->proc_no1->setStyleSheet("border: 3px solid rgb(0,0,255); \nBorder-radius: 25px; color: rgb(0, 0, 255)");
                else
                    ui->proc_no1->setStyleSheet("border: 3px solid rgb(255,0,0); \nBorder-radius: 25px; color: rgb(255, 0, 0)");
                break;
        case 2:
            if (parsedata[stat -1].modestat && !parsedata[stat -1].MC_PROCESS_NAME.contains("NONE"))
                ui->proc_no2->setStyleSheet("border: 3px solid rgb(0,0,255); \nBorder-radius: 25px; color: rgb(0, 0, 255)");
            else
                ui->proc_no2->setStyleSheet("border: 3px solid rgb(255,0,0); \nBorder-radius: 25px; color: rgb(255, 0, 0)");
            break;
        case 3:
            if (parsedata[stat -1].modestat && !parsedata[stat -1].MC_PROCESS_NAME.contains("NONE"))
                ui->proc_no3->setStyleSheet("border: 3px solid rgb(0,0,255); \nBorder-radius: 25px; color: rgb(0, 0, 255)");
            else
                ui->proc_no3->setStyleSheet("border: 3px solid rgb(255,0,0); \nBorder-radius: 25px; color: rgb(255, 0, 0)");
            break;
        case 4:
            if (parsedata[stat -1].modestat && !parsedata[stat -1].MC_PROCESS_NAME.contains("NONE"))
                ui->proc_no4->setStyleSheet("border: 3px solid rgb(0,0,255); \nBorder-radius: 25px; color: rgb(0, 0, 255)");
            else
                ui->proc_no4->setStyleSheet("border: 3px solid rgb(255,0,0); \nBorder-radius: 25px; color: rgb(255, 0, 0)");
            break;
        case 5:
            if (parsedata[stat -1].modestat && !parsedata[stat -1].MC_PROCESS_NAME.contains("NONE"))
                ui->proc_no5->setStyleSheet("border: 3px solid rgb(0,0,255); \nBorder-radius: 25px; color: rgb(0, 0, 255)");
            else
                ui->proc_no5->setStyleSheet("border: 3px solid rgb(255,0,0); \nBorder-radius: 25px; color: rgb(255, 0, 0)");
            break;
        case 6:
            if (parsedata[stat -1].modestat && !parsedata[stat -1].MC_PROCESS_NAME.contains("NONE"))
                ui->proc_no6->setStyleSheet("border: 3px solid rgb(0,0,255); \nBorder-radius: 25px; color: rgb(0, 0, 255)");
            else
                ui->proc_no6->setStyleSheet("border: 3px solid rgb(255,0,0); \nBorder-radius: 25px; color: rgb(255, 0, 0)");
            break;
        case 7:
            if (parsedata[stat -1].modestat && !parsedata[stat -1].MC_PROCESS_NAME.contains("NONE"))
                ui->proc_no7->setStyleSheet("border: 3px solid rgb(0,0,255); \nBorder-radius: 25px; color: rgb(0, 0, 255)");
            else
                ui->proc_no7->setStyleSheet("border: 3px solid rgb(255,0,0); \nBorder-radius: 25px; color: rgb(255, 0, 0)");
            break;
        case 8:
            if (parsedata[stat -1].modestat && !parsedata[stat -1].MC_PROCESS_NAME.contains("NONE"))
                ui->proc_no8->setStyleSheet("border: 3px solid rgb(0,0,255); \nBorder-radius: 25px; color: rgb(0, 0, 255)");
            else
                ui->proc_no8->setStyleSheet("border: 3px solid rgb(255,0,0); \nBorder-radius: 25px; color: rgb(255, 0, 0)");
            break;
        case 9:
            if (parsedata[stat -1].modestat && !parsedata[stat -1].MC_PROCESS_NAME.contains("NONE"))
                ui->proc_no9->setStyleSheet("border: 3px solid rgb(0,0,255); \nBorder-radius: 25px; color: rgb(0, 0, 255)");
            else
                ui->proc_no9->setStyleSheet("border: 3px solid rgb(255,0,0); \nBorder-radius: 25px; color: rgb(255, 0, 0)");
            break;
        case 10:
            if (parsedata[stat -1].modestat && !parsedata[stat -1].MC_PROCESS_NAME.contains("NONE"))
                ui->proc_no10->setStyleSheet("border: 3px solid rgb(0,0,255); \nBorder-radius: 25px; color: rgb(0, 0, 255)");
            else
                ui->proc_no10->setStyleSheet("border: 3px solid rgb(255,0,0); \nBorder-radius: 25px; color: rgb(255, 0, 0)");
            break;
        default:
            break;
    }
}

__u8 MainWindow::memory_in_processcnt(__u8 count, __u8 localstat, QString **Group)
{
    if (!localstat)
        return 0;

    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,SPS_PATH);
    QSettings m_settings(".","ssf.ini");

    m_settings.beginGroup(*Group[localstat - 1]);
    m_settings.setValue("process_cnt", count);
    m_settings.endGroup();

    return 1;

}

__u8 MainWindow::memory_in_laststat(__u8 laststat)
{
    public_laststat = laststat;

    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,SPS_PATH);
    QSettings m_settings(".","ssf.ini");

    m_settings.setValue("public_laststat", laststat);

    if(!laststat)
    {
        qDebug()<<"error lastsat 0 where save memory process";
    }
    return laststat;
}

__u8 MainWindow::memory_in_worker(QString worker, QString **Group, __u8 localstat)
{
    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,SPS_PATH);
    QSettings m_settings(".","ssf.ini");

    m_settings.beginGroup(*Group[localstat - 1]);
    m_settings.setValue("worker_id", worker);
    m_settings.endGroup();

    return 0;
}

void MainWindow::display_actual_cnt(QString *tmp, __u8 stat)
{
    tmp->sprintf("%d", parsedata[stat -1].process_cnt);
    ui->actual->setText(*tmp);
    tmp->clear();
}

void MainWindow::display_tirmstitch(QString *trim, __u8 stat)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    if (parsedata[stat -1].modestat == 2)
    {
        trim->append("땀수");
        trim->append(trim->number(parsedata[stat -1].count));
    }
    else
    {
      trim->append("사절");
      trim->append(trim->number(parsedata[stat -1].count));
    }

    ui->setStitch->setText(*trim);

    QTextCodec::setCodecForTr(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("eucKR"));
    trim->clear();
}

void MainWindow::display_item(QString *item, QString *tmp)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    tmp->append("아이템명:");
    tmp->append(*item);

    ui->l_item_name->setText(*tmp);

    QTextCodec::setCodecForTr(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("eucKR"));
    tmp->clear();
}

void MainWindow::display_param(__u8 stat)
{
    QString tmp;

    tmp.append(QString::number(parsedata[stat -1].interval));
    ui->l_interval_var->setText(tmp);
    ui->l_stitch_to_trim_var->setText(QString::number(parsedata[stat -1].stitch_to_trim));
    tmp.clear();
}

void MainWindow::display_worker_id(QString worker_id, QString *tmp)
{

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    tmp->append("공정명 : ");
    tmp->append(parsedata[public_laststat - 1].MC_PROCESS_NAME);
    tmp->append("\n");
    tmp->append("ID : ");
    tmp->append(worker_id);

    QTextCodec::setCodecForTr(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("eucKR"));

    ui->l_job_title->setText(*tmp);
    tmp->clear();
}

void MainWindow::display_both(Ui::MainWindow *ui, __u16 trim, __u16 stitch)
{
    ui->l_trim->setText(QString::number(trim));
    ui->l_stitch->setText(QString::number(stitch));
}

__u8 MainWindow::display_target(QString *target)
{
    ui->target_count->setText(*target);
    return 1;
}

__u8 MainWindow::display_factor(__u16 *factor)
{
    ui->l_factor->setText(QString::number(*factor) +"%");
    return 0;
}

void MainWindow::getbufferdata(QString *buffer, QString *buffer_code, QString *tmp, int index, bool is_multi)
{
    tmp->clear();

    if (index == 1)
        *tmp = buffer_code[BUFFERINDEX];
    else if (index == 2)
    {
        *tmp = buffer[BUFFERINDEX];
        buffer[BUFFERINDEX].clear();
    }

    if (is_multi == 0)
         buffer_code[BUFFERINDEX].clear();
}

void MainWindow::setworkid(QString *work_id, QString *tmp)
{
    if (tmp->isEmpty())
        return;

    *work_id = *tmp;

    tmp->clear();

    return ;
}


void MainWindow::settarget(QString *target, QString *tmp2)
{
    if (tmp2->isEmpty())
        return;

    *target = *tmp2;

    tmp2->clear();

    return ;
}

void MainWindow::all_button_off(QString tmp)
{
    tmp.clear();
    ui->l_job_title->setText(tmp);
    ui->setStitch->setText(tmp);

    ui->proc_no1->setStyleSheet("border: 3px solid rgb(255,255,255); \nBorder-radius: 25px; color: rgb(255,255,255)");
    ui->proc_no2->setStyleSheet("border: 3px solid rgb(255,255,255); \nBorder-radius: 25px; color: rgb(255,255,255)");
    ui->proc_no3->setStyleSheet("border: 3px solid rgb(255,255,255); \nBorder-radius: 25px; color: rgb(255,255,255)");
    ui->proc_no4->setStyleSheet("border: 3px solid rgb(255,255,255); \nBorder-radius: 25px; color: rgb(255,255,255)");
    ui->proc_no5->setStyleSheet("border: 3px solid rgb(255,255,255); \nBorder-radius: 25px; color: rgb(255,255,255)");
    ui->proc_no6->setStyleSheet("border: 3px solid rgb(255,255,255); \nBorder-radius: 25px; color: rgb(255,255,255)");
    ui->proc_no7->setStyleSheet("border: 3px solid rgb(255,255,255); \nBorder-radius: 25px; color: rgb(255,255,255)");
    ui->proc_no8->setStyleSheet("border: 3px solid rgb(255,255,255); \nBorder-radius: 25px; color: rgb(255,255,255)");
    ui->proc_no9->setStyleSheet("border: 3px solid rgb(255,255,255); \nBorder-radius: 25px; color: rgb(255,255,255)");
    ui->proc_no10->setStyleSheet("border: 3px solid rgb(255,255,255); \nBorder-radius: 25px; color: rgb(255,255,255)");
}


void MainWindow::handle_save(int Button, QString *tmp, QString **Group)
{

    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,SPS_PATH);
    QSettings m_settings(".","ssf.ini");
    delay_ms(1);
    ui->l_wait->show();
    ui->b_home_3->hide();
    ui->progressBar->show();

    if(!Button)
        return;

    m_settings.beginGroup(*Group[Button - 1]);
    m_settings.setValue("item-name", parsedata[Button -1].selected_item);
    m_settings.setValue("MC_PROCESS_NAME", parsedata[Button -1].MC_PROCESS_NAME);
    ui->progressBar->setValue(20);
    delay_ms(10);
    m_settings.setValue("MC_ITEM_NAME", parsedata[Button -1].MC_ITEM_NAME);
    m_settings.setValue("style-full-name", parsedata[Button -1].MC_STYLE_CODE);
    m_settings.setValue("style-name", parsedata[Button-1].MC_SENDER_STYLE_CODE_LEFT);
    m_settings.setValue("style-code", parsedata[Button-1].MC_SENDER_STYLE_CODE_RIGHT);
    m_settings.setValue("mc_item_cd", parsedata[Button-1].MC_ITEM_CD);
    ui->progressBar->setValue(40);
    delay_ms(10);
    m_settings.setValue("trim_cnt", parsedata[Button-1].trim_cnt);
    m_settings.setValue("stitch_cnt", parsedata[Button-1].stitch_cnt);
    m_settings.setValue("mode", parsedata[Button-1].modestat);
    m_settings.setValue("count", parsedata[Button-1].count);
    m_settings.setValue("worker_id", parsedata[Button-1].worker_id);
    if (!parsedata[Button -1].target_cnt.contains("NONE"))
         m_settings.setValue("target_cnt",parsedata[Button -1].target_cnt);

    m_settings.setValue("process_cnt",parsedata[Button -1].process_cnt);
    m_settings.endGroup();
    ui->progressBar->setValue(60);
    delay_ms(10);
    tmp->clear();
    ui->progressBar->setValue(100);
    ui->l_wait->hide();
    ui->progressBar->hide();
    delay_ms(1);
    ui->b_home_3->show();
}

void MainWindow::handle_sava_all(QString **Group)
{

    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,SPS_PATH);
    QSettings m_settings(".","ssf.ini");
    delay_ms(10);

    ui->l_wait->show();
    ui->progressBar->show();
    ui->b_home_3->hide();

    for (__u8 i = 1; i <= 10; i ++)
    {

        m_settings.beginGroup(*Group[i - 1]);
        m_settings.setValue("item-name", parsedata[i -1].selected_item);
        m_settings.setValue("MC_PROCESS_NAME", parsedata[i -1].MC_PROCESS_NAME);
        parsedata[i-1].MC_SENDER_STYLE_CODE_LEFT.clear();
        parsedata[i-1].MC_SENDER_STYLE_CODE_RIGHT.clear();
        parsedata[i-1].MC_SENDER_STYLE_CODE_LEFT = parsedata[i -1].MC_STYLE_CODE.section("-",0,0);
        parsedata[i-1].MC_SENDER_STYLE_CODE_RIGHT = parsedata[i -1].MC_STYLE_CODE.section("-",1,1);
        parsedata[i-1].MC_SENDER_STYLE_CODE_RIGHT.insert(0,"");
        ui->progressBar->setValue(20);
        delay_ms(10);
        m_settings.setValue("MC_ITEM_NAME", parsedata[i -1].MC_ITEM_NAME);
        m_settings.setValue("style-full-name", parsedata[i -1].MC_STYLE_CODE);
        m_settings.setValue("style-name", parsedata[i-1].MC_SENDER_STYLE_CODE_LEFT);
        m_settings.setValue("style-code", parsedata[i-1].MC_SENDER_STYLE_CODE_RIGHT);
        m_settings.setValue("mc_item_cd", parsedata[i-1].MC_ITEM_CD);
        ui->progressBar->setValue(40);
        delay_ms(10);
        m_settings.setValue("trim_cnt", parsedata[i-1].trim_cnt);
        m_settings.setValue("stitch_cnt", parsedata[i-1].stitch_cnt);
        m_settings.setValue("mode", parsedata[i-1].modestat);
        m_settings.setValue("count", parsedata[i-1].count);
        m_settings.setValue("worker_id", parsedata[i-1].worker_id);
        if (!parsedata[i -1].target_cnt.contains("NONE"))
             m_settings.setValue("target_cnt",parsedata[i -1].target_cnt);
        m_settings.setValue("process_cnt",parsedata[i -1].process_cnt);
        m_settings.endGroup();
        ui->progressBar->setValue(60);
        delay_ms(10);
        ui->progressBar->setValue(100);
        delay_ms(1);
    }

    ui->l_wait->hide();
    ui->progressBar->hide();
    ui->b_home_3->show();
}

void MainWindow::parse_sendercode(__u8 Button)
{
    parsedata[Button-1].MC_SENDER_STYLE_CODE_LEFT.clear();
    parsedata[Button-1].MC_SENDER_STYLE_CODE_RIGHT.clear();
    parsedata[Button-1].MC_SENDER_STYLE_CODE_LEFT = parsedata[Button -1].MC_STYLE_CODE.section("-",0,0);
    parsedata[Button-1].MC_SENDER_STYLE_CODE_RIGHT = parsedata[Button -1].MC_STYLE_CODE.section("-",1,1);
    parsedata[Button-1].MC_SENDER_STYLE_CODE_RIGHT.insert(0,"");
}

void MainWindow::draw_the_graph()
{

    if(!parsedata[public_laststat-1].target_cnt.contains("NONE"))
    {
        ui->l_tar_bar->show();
        ui->l_cur_bar->show();
        int value = (int)(((float)parsedata[public_laststat-1].process_cnt/parsedata[public_laststat-1].target_cnt.toFloat())*100);

        if(value>=100)
            value = value/100;

        int moving_data = 260-((int)(260*((float)value/100)));
        ui->l_cur_bar->setGeometry(200,240+moving_data,71,260-moving_data);

    }else{
        ui->l_tar_bar->hide();
        ui->l_cur_bar->hide();
        ui->target_count->hide();
    }
}

//181102
void MainWindow::on_b_countUp_clicked()
{
    event_apear = true;
    is_checking = true;

    parsedata[public_laststat-1].process_cnt++;

    memory_in_processcnt(parsedata[public_laststat -1].process_cnt, public_laststat, Group);

    ui->actual->setText(QString::number(parsedata[public_laststat-1].process_cnt));

    if(!parsedata[public_laststat -1].target_cnt.contains("NONE"))
    {
        ui->l_tar_bar->show();
        ui->l_cur_bar->show();
        int value = (int)(((float)parsedata[public_laststat-1].process_cnt/parsedata[public_laststat -1].target_cnt.toFloat())*100);

        if(value>=100)
            value = value/100;

        int moving_data = 260-((int)(260*((float)value/100)));

        ui->l_cur_bar->setGeometry(200,240+moving_data,71,260-moving_data);
    }else{
        ui->l_tar_bar->hide();
        ui->l_cur_bar->hide();
        ui->target_count->hide();
    }

    if (server_state)
        send_countByUP(public_laststat -1);
    else
    {
        parsedata[public_laststat -1].sec_sum += second;
        parsedata[public_laststat -1].oper_sum += operation_second;
        parsedata[public_laststat -1].stitch_sum += parsedata[public_laststat -1].stitch_cnt;
    }

    parsedata[public_laststat -1].stitch_cnt = 0;
    parsedata[public_laststat -1].trim_cnt = 0;
    operation_second = 0;
    second = 0;

    display_both(ui, parsedata[public_laststat -1].trim_cnt, parsedata[public_laststat -1].stitch_cnt);
}


void MainWindow::on_b_countDown_clicked()
{
    event_apear = true;
    is_checking = true;

    parsedata[public_laststat-1].process_cnt -= 1;
    if(parsedata[public_laststat-1].process_cnt < 0)
    parsedata[public_laststat-1].process_cnt = 0;

    memory_in_processcnt(parsedata[public_laststat -1].process_cnt, public_laststat, Group);

    ui->actual->setText(QString::number(parsedata[public_laststat-1].process_cnt));

    if(!parsedata[public_laststat-1].target_cnt.contains("NONE"))
    {
        ui->l_tar_bar->show();
        ui->l_cur_bar->show();
        int value = (int)(((float)parsedata[public_laststat-1].process_cnt/parsedata[public_laststat -1].target_cnt.toFloat())*100);

        if(value>=100)
            value=110;

        int moving_data = 260-((int)(260*((float)value/100)));


        ui->l_cur_bar->setGeometry(200,240+moving_data,71,260-moving_data);
    }else{
        ui->l_tar_bar->hide();
        ui->l_cur_bar->hide();
        ui->target_count->hide();
    }
    if (server_state)
        send_countByUP(public_laststat -1);
    else
    {
        parsedata[public_laststat -1].sec_sum += second;
        parsedata[public_laststat -1].oper_sum += operation_second;
        parsedata[public_laststat -1].stitch_sum += parsedata[public_laststat -1].stitch_cnt;
    }

    parsedata[public_laststat -1].stitch_cnt = 0;
    parsedata[public_laststat -1].trim_cnt = 0;
    operation_second = 0;
    second = 0;

    display_both(ui, parsedata[public_laststat -1].trim_cnt, parsedata[public_laststat -1].stitch_cnt);
}

void MainWindow::on_b_countReset_clicked()
{
    parsedata[public_laststat -1].stitch_cnt = 0;
    parsedata[public_laststat -1].trim_cnt = 0;
    operation_second = 0;
    second = 0;

    display_both(ui, parsedata[public_laststat -1].trim_cnt, parsedata[public_laststat -1].stitch_cnt);

    int ret;
    QMessageBox msgBox;
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    msgBox.setText("현재 생산량을 0으로 초기화 하시겠습니까?");
    msgBox.setStandardButtons((QMessageBox::No | QMessageBox::Yes));
    msgBox.setDefaultButton(QMessageBox::No);

    ret = msgBox.exec();

    switch (ret)
    {
    case QMessageBox::Yes:
        parsedata[public_laststat-1].process_cnt = 0;
        memory_in_processcnt(parsedata[public_laststat -1].process_cnt, public_laststat, Group);
        ui->actual->setText(QString::number(parsedata[public_laststat-1].process_cnt));
        draw_the_graph();
        break;
    case QMessageBox::No:
        break;
    default:
        break;
    }

    QTextCodec::setCodecForTr(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("eucKR"));


}

bool MainWindow::numtostring(QString tmp, int num)
{
    tmp.sprintf("%d",num);
    ui->act_stitch->setText(tmp);
    return 1;
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    //QString fname;
    switch(event->key()){
    case Qt::Key_Escape: // LCD_SETENV -> SET_EXIT
        close();
        break;

    default:
        QWidget::keyPressEvent(event);
    }
}

void MainWindow::try_connector()
{

    if(!mouse_event_lock)
    {
        ui->wifi_state_2->show();
        bool state;
                if(socket->state() != QAbstractSocket::HostLookupState)
                {
                    state=connectToHost(srv_ip);
                    if(!state)
                    {
                        if(!get_time_info)
                        {
                            request_time();
                        }
                        qDebug()<<"connected progressing";

                    }

                }
                else if(socket->state() != QAbstractSocket::ConnectedState)
                {
                    state=connectToHost(srv_ip);
                    if(!state)
                    {
                        // need to when send ui to use korean temp1=QString::fromUtf8("연결됨");
                        if(!get_time_info)
                        {
                            request_time();
                        }
                        qDebug()<<"connected success";
                    }
                }
                else
                {
                    try_connector();
                }
    }
}
void MainWindow::socketReadyRead()
{
    QString msg;
    msg.clear();
    msg=socket->readAll();

    if (!msg.isEmpty())
    {
        QPalette pal;

        pal.setColor(QPalette::Background, Qt::green);
        ui->wifi_state_2->setPalette(pal);
        ui->l_network_state_2->setText("Connected");

        event_apear = false;
        server_state = true;
        is_checking = false;

        ui->l_ok->show();
        ui->l_ok->setText("OK");
    }

    if(msg.contains("#T"))
    {
        QStringList get_date_info;
        msg.replace("#T|","");
        get_date_info=msg.split("|");
        date=get_date_info.value(0);
        time=get_date_info.value(1);
        get_time_info=true;

        setting_date_linux();
        working_date_check();

    }
    else if(msg.contains("#U"))
    {
        qDebug()<<"rcvd URL result";
        QString result;
        result=msg.section("|",1,1);
        qDebug()<<result;
        if(result.contains("OK"))
        {
            copy_tftp_boot(MC_STYLE_CODE);
            qDebug()<<"url ok";
        }
        else
        {
            ui->ssid_progressBar->hide();
            qDebug()<<"url fail";
        }

    }
    else if(msg.contains(op_ip)) //confirm
    {
    }
    else if(msg.contains("@@@@@"))
    {
        handle_nack(txbuffer);
    }
    else if(msg.contains("#EOF"))
    {
        QPalette pal;

        pal.setColor(QPalette::Background, Qt::red);
        ui->wifi_state_2->setPalette(pal);
        ui->l_network_state_2->setText("Disconnected");

        server_state = false;
        is_checking = false;

    }
    else
    {
        socket_msg_box.clear();
        socket_msg_box=msg;

        qDebug()<<"socket_msg_box"<<socket_msg_box;

        emit sig_rcvd_socket();

    }
}

void MainWindow::copy_tftp_boot(QString temp)
{
    QString ip;
    ip=srv_ip;//srv_ip
    temp.append(".jpg");

    ui->ssid_progressBar->show();
    ui->ssid_progressBar->setValue(0);


#ifdef __arm__
    QStringList arguments,arguments1;
    QProcess getProcess;

    //system("cd /app/data");

    QString command,command1,command2,tmp;
    command="cd /app/data && /usr/bin/tftp " + srv_ip + " -r " + temp + " -g";
    command.replace("\"","");

    qDebug()<<command;

    command1 = "ls /app/data | grep -c " +temp;
    command1.replace("\"","");

    ui->ssid_progressBar->setValue(30);

    ui->ssid_progressBar->setValue(40);


    system(command.toLocal8Bit());
    ui->ssid_progressBar->setValue(50);

    system("sync");
    system("sync");
    ui->ssid_progressBar->setValue(60);

    system("sync");
    system("sync");
    system("sync");
    system("sync");
    ui->ssid_progressBar->setValue(70);
    arguments << "-c" << command1;

    ui->ssid_progressBar->setValue(75);
    getProcess.start("sh" ,arguments);
    //sleep(3);
    getProcess.waitForFinished();
    tmp = getProcess.readAll();
    tmp.remove(QRegExp("[\\n\\t\\r]"));
    tmp.replace("\"","");
    ui->ssid_progressBar->setValue(80);

    if(tmp.toInt()>=1)
    {
        qDebug()<<"down load ok"<<tmp;

        ui->ssid_progressBar->setValue(100);
        ui->ssid_progressBar->hide();

        QString fileName = "/app/data/"+MC_STYLE_CODE+".jpg";
        qDebug()<<"this load image"<<fileName;
        QImage image;
        if (!image.load(fileName)) {
            ui->l_info->show();
            //request_job_instruction();
            qDebug()<<"load img fail";
            return;
        }else{
            ui->l_img->setPixmap(QPixmap::fromImage(image));
            ui->l_info->hide();
            ui->ssid_progressBar->hide();

        }

    }else{
        qDebug()<<"down load fail";
        ui->ssid_progressBar->hide();

    }

    ui->ssid_progressBar->hide();
#else
    QStringList arguments,arguments1;
    QProcess getProcess;

    //system("cd /app/data");

    QString command,command1,command2,tmp;
    command="tftp " + srv_ip + " -r " + temp + " -g";
    command.replace("\"","");

    qDebug()<<command;

    command1 = "ls /app/data | grep -c " +temp;
    command1.replace("\"","");

    ui->ssid_progressBar->setValue(30);

    ui->ssid_progressBar->setValue(40);


    system(command.toLocal8Bit());
    ui->ssid_progressBar->setValue(50);

    system("sync");
    system("sync");
    ui->ssid_progressBar->setValue(60);

    system("sync");
    system("sync");
    system("sync");
    system("sync");
    ui->ssid_progressBar->setValue(70);
    arguments << "-c" << command1;

    ui->ssid_progressBar->setValue(75);
    getProcess.start("sh" ,arguments);
    //sleep(3);
    getProcess.waitForFinished();
    tmp = getProcess.readAll();
    tmp.remove(QRegExp("[\\n\\t\\r]"));
    tmp.replace("\"","");
    ui->ssid_progressBar->setValue(80);

    if(tmp.toInt()>=1)
    {
        qDebug()<<"down load ok"<<tmp;

        //        system(command2.toLocal8Bit());
        //        system("sync");
        //        system("sync");
        //        system("sync");

        ui->ssid_progressBar->setValue(100);
        ui->ssid_progressBar->hide();

        QString fileName = "/app/data/"+MC_STYLE_CODE+".jpg";
        qDebug()<<"this load image"<<fileName;
        QImage image;
        if (!image.load(fileName)) {
            ui->l_info->show();
            //request_job_instruction();
            qDebug()<<"load img fail";
            return;
        }else{
            ui->ssid_progressBar->hide();

            ui->l_img->setPixmap(QPixmap::fromImage(image));
            ui->l_info->hide();
        }

    }else{
        qDebug()<<"down load fail";
        ui->ssid_progressBar->hide();

    }

#endif
    ui->ssid_progressBar->hide();


}

void MainWindow::working_date_check()
{
    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,SPS_PATH);
    QSettings m_settings(".","ssf.ini");

    if(date.toInt() > m_settings.value("date",0).toInt())
    {

        m_settings.setValue("actual_count","0");
        parsedata[public_laststat -1].process_cnt = 0;

        //all delete job infomation
        m_settings.setValue("date",date.toInt());
        m_settings.setValue("time",time.toInt());
        //must remove job date
        qDebug()<<"new working day";
    }else{
        qDebug()<<"working.....";
        //not remove job infomation
    }
}

void MainWindow::setting_date_linux()
{
    QString system_command,command_date,command_time;

    command_date = QString("%1%2%3%4-%5%6-%7%8").arg(date.at(0),date.at(1),date.at(2),date.at(3),date.at(4),date.at(5),date.at(6),date.at(7));

    if(time.count()==6){
        command_time = QString("%1%2:%3%4:%5%6").arg(time.at(0),time.at(1),time.at(2),time.at(3),time.at(4),time.at(5));
    }else{
        command_time = QString("0%1:%2%3:%4%5").arg(time.at(0),time.at(1),time.at(2),time.at(3),time.at(4));
    }
    system_command="date -s \"";
    system_command.append(command_date);
    system_command.append(" ");
    system_command.append(command_time);
    system_command.append("\"");

#ifdef  ARM_TEST
    system(system_command.toLocal8Bit());
#endif
}

bool MainWindow::connectToHost(QString host)
{
    //socket->hostFound();
    socket->connectToHost(host, 3000);
    if (socket->waitForConnected(1000))
    {
        return 0;
    }else{
        return 1;
    }
    //return socket->waitForConnected();
}
QByteArray IntToArray(qint32 source) //Use qint32 to ensure that the number have 4 bytes
{
    //Avoid use of cast, this is the Qt way to serialize objects
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}

bool MainWindow::writeData(QByteArray data)
//bool MainWindow::writeData(QString data)
{
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->write(IntToArray(data.size())); //write size of data
        socket->write(data,qstrlen(data)); //write the data itself
        return socket->waitForBytesWritten();
    }
    else
        return false;
}

void MainWindow::send_count(__u8 index)
{

    event_apear = true;
    is_checking = true;

    QString data_tmp;

    if (parsedata[index].sec_sum && parsedata[index].oper_sum && parsedata[index].stitch_sum)
    {
        second = parsedata[index].sec_sum + second;
        operation_second = parsedata[index].oper_sum + operation_second;
        parsedata[index].stitch_cnt = parsedata[index].stitch_sum + parsedata[index].stitch_cnt;
        parsedata[index].stitch_sum = parsedata[index].sec_sum = parsedata[index].oper_sum = 0;
    }


    data_tmp = QString("$D|-|%1|%2|%3|%4|%5|%6|A|%7|%8|%9|#").arg(parsedata[index].worker_id,//2se
                                                                    parsedata[index].target_cnt,//3
                                                                    QString::number(parsedata[index].process_cnt),
                                                                    QString::number(second),
                                                                    QString::number(parsedata[index].stitch_cnt),
                                                                    QString::number(operation_second),
                                                                    MC_SENDER_MC_CODE,//10 //MC_MODEL ex. 18SHA02001
                                                                    parsedata[index].MC_SENDER_STYLE_CODE_LEFT,//11          //LINE_NM   ex. SH-P3A-3001
                                                                    parsedata[index].MC_SENDER_STYLE_CODE_RIGHT
                                                                        ); //MC_CODE  ex. P012

    qDebug()<<"data_tmp"<<data_tmp;

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    data_tmp = is_valid_data(data_tmp);

    QTextCodec::setCodecForTr(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("eucKR"));

    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->write(data_tmp.toLocal8Bit());
    }
    else{
    }

    txbuffer.clear();
    txbuffer.append(data_tmp);
}


void MainWindow::send_init()
{
    int state;

    QString data_tmp;
    data_tmp.sprintf("%d",0);

    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->write("$D");
        state=1;
    }
    else{
        state=0;
    }
}

void MainWindow::updateTime()
{
    if(!operate_lock)
    {
        QString temp;
        if(old_motor_run_stop != motor_run_stop )
        {
            old_motor_run_stop = motor_run_stop;
            ui->l_oper_sec->setText(temp.sprintf("%d",++operation_second));
        }
    }

    if(connect_state_cnt > 30)
    {
        if(socket->state() != QAbstractSocket::ConnectedState)
        {
            try_connector();
        }else{
            qDebug()<<"network state ok!";
            ui->l_network_state->setText("Connected");
            ui->l_network_state_2->setText("Connected");
            QPalette pal;
            pal.setColor(QPalette::Background, Qt::green);
            ui->wifi_state_2->setPalette(pal);

        }
        connect_state_cnt=0;
    }else
    {
        connect_state_cnt++;
    }
}

#include<QMap>
void MainWindow::update_sewTime_pulse()
{
        if(operate_lock)
        {
            return;
        }

        if (!public_laststat)
        {
            return;
        }

        if (!m_istest)
        {
            if (second < 300)
            {
                m_ret = 0;
                second++;
            }
            if (second == 300 && !is_stop && m_ret == 0)
                is_stop = 1;


            if (!is_stop && second >= 300)
                second++;

            if (second % 2 == 0)
            {
                ui->l_ok->hide();
            }


            loop_seconds++;
            update_second++;

            if (is_checking)
                alive++;

            if (server_state == false)
                alive++;
        }

        if (init_try < 3)
        {
            alive_checking();
            init_try++;
        }


        if (server_state == false && alive >= 10)
        {
            QPalette pal;

            get_time_info = false;

            pal.setColor(QPalette::Background, Qt::red);
            ui->wifi_state_2->setPalette(pal);
            ui->l_network_state_2->setText("Disconnected");
            ui->l_ok->setText("NG");

            try_connector();

            alive = 0;
        }
        else if(server_state == true)
        {
            QPalette pal;

            pal.setColor(QPalette::Background, Qt::green);
            ui->wifi_state_2->setPalette(pal);
            ui->l_network_state_2->setText("Connected");
        }

        if (event_apear == true && is_checking == true)
        {
            if (alive > 1)
            {
                server_state = false;
                is_checking = false;
                alive = 0;
            }
        }

        if(retry_j < 3)
        {
            if (event_j ==true && socket_msg_box.count() == 0)
            {
                retry_j++;
                process_scanning(MC_ITEM_CD);
            }
        }

        m_diff = (before_stitch - after_stitch) / ((update_second - old_second) +1);

        if (m_diff)
        {
            after_stitch = before_stitch;
            old_second = update_second;
            if (m_iserrorsend)
            {
                ui->l_sendingdefect->hide();
                ui->b_sendingdefect->hide();
                ui->b_home_3->show();
            }
        }


        if(old_motor_run_stop != motor_run_stop )
        {
            if (is_stop == 1)
            {
                m_ret = 1;
                is_stop = 0;
            }
            old_motor_run_stop = motor_run_stop;
            operation_second++;
        }

        if (parsedata[public_laststat - 1].modestat == euSTITCH && !m_diff && update_second - old_second > parsedata[public_laststat - 1].interval)
        {
            if (parsedata[public_laststat -1].stitch_cnt >= (int)(parsedata[public_laststat - 1].count * (100 - parsedata[public_laststat -1].gain)) / 100)
            {

                ui->actual->setText(QString::number(++parsedata[public_laststat -1].process_cnt));

                memory_in_processcnt(parsedata[public_laststat -1].process_cnt, public_laststat, Group);

                if(server_state == true)
                    send_count(public_laststat -1);

                if(server_state == false)
                {
                    parsedata[public_laststat -1].sec_sum += second;
                    parsedata[public_laststat -1].oper_sum += operation_second;
                    parsedata[public_laststat -1].stitch_sum += parsedata[public_laststat -1].stitch_cnt;
                }

                parsedata[public_laststat -1].stitch_cnt = 0;
                parsedata[public_laststat -1].trim_cnt = 0;
                update_second = old_second = before_stitch = after_stitch = 0;
                operation_second=0;
                second=0;
                motor_run_stop = 0;
                old_motor_run_stop = 0;
                display_both(ui, parsedata[public_laststat -1].trim_cnt, parsedata[public_laststat -1].stitch_cnt);
            }

            loop_seconds=0;
        }
        else if (parsedata[public_laststat -1].modestat == euTRIM && !m_diff && update_second - old_second > parsedata[public_laststat - 1].interval)
        {

            if (m_is_valid_trim)
            {
                parsedata[public_laststat -1].trim_cnt++;
                m_is_valid_trim = 0;
            }

            if ((parsedata[public_laststat -1].trim_cnt >= parsedata[public_laststat - 1].count) && parsedata[public_laststat - 1].count )
            {

                 ui->actual->setText(QString::number(++parsedata[public_laststat -1].process_cnt));
                 memory_in_processcnt(parsedata[public_laststat -1].process_cnt, public_laststat, Group);

                 if (!(int)m_diff)
                 {
                     if(server_state == true)
                        send_count(public_laststat -1);

                     if(server_state == false)
                     {
                         parsedata[public_laststat -1].sec_sum += second;
                         parsedata[public_laststat -1].oper_sum += operation_second;
                         parsedata[public_laststat -1].stitch_sum += parsedata[public_laststat -1].stitch_cnt;
                     }
                 }

                 parsedata[public_laststat -1].trim_cnt = 0;
                 parsedata[public_laststat -1].stitch_cnt = 0;
                 update_second = old_second = before_stitch = after_stitch = 0;
                 operation_second=0;
                 second=0;

                 display_both(ui, parsedata[public_laststat -1].trim_cnt, parsedata[public_laststat -1].stitch_cnt);
            }

            loop_seconds=0;
        }

        draw_the_graph();

}

void MainWindow::edge_input_pulse()
{
    read(fd_pulse, &key_event, sizeof(key_event));

    if (m_istest)
        return ;

    if(key_event.type == EV_KEY)
    {
        if(key_event.value==1)
        {
            if(key_event.code == KEY_BACK)
            {
                if (m_istune == 2)
                    return ;

                if (m_istune == 1)
                {
                    ui->act_stitch->setText(QString::number(++m_cntbuff));
                    motor_run_stop ++;
                }
                else if (m_istune == 0)
                {
                    loop_seconds=0;

                    display_both(ui, parsedata[public_laststat -1].trim_cnt, ++parsedata[public_laststat -1].stitch_cnt);
                    ++update_stitch;
                    ++before_stitch;
                    motor_run_stop ++;
                }
            }
            else if (key_event.code == KEY_UP)
            {
                __u32 diff = (update_stitch - old_stitch + 0x7FFFFFFF) % 0x7FFFFFFF;

                if (diff >= parsedata[public_laststat - 1].stitch_to_trim)
                {
                    if (parsedata[public_laststat -1].trim_cnt >= parsedata[public_laststat -1].count -1)
                    {
                        m_is_valid_trim = 1;
                        old_stitch = update_stitch;
                    }
                    else
                    {
                        display_both(ui, ++parsedata[public_laststat -1].trim_cnt, parsedata[public_laststat -1].stitch_cnt);
                        old_stitch = update_stitch;
                    }
                }
            }
        }
    }
}

void MainWindow::on_b_setting_clicked()
{
    ui->test_prog->show();

    operate_lock=true;
    mouse_event_lock=true;
    //qDebug()<<"count init";


    D_setting d_setting(this);
    //d_init.
    d_setting.move(0,0);
    d_setting.exec();

    ui->test_prog->hide();

    operate_lock=false;
    mouse_event_lock=false;

}
#if 0
void MainWindow::on_b_set_code_clicked()
{
    ui->test_prog->show();


    operate_lock=true;
    mouse_event_lock=true;
    //qDebug()<<"count init";
    D_set_code d_set_code(this);
    //d_init.
    d_set_code.move(0,0);
    d_set_code.exec();

    if(worker_id!=NULL)
        ui->l_worker->setText(worker_id);

    if(mc_code!=NULL)
        ui->l_mc->setText(mc_code);

    if(job_code!=NULL)
        ui->l_job->setText(job_code);

    operate_lock=false;
    mouse_event_lock=false;

    ui->test_prog->hide();
}

void MainWindow::on_b_design_list_clicked()
{
    operate_lock=true;
    mouse_event_lock=true;



    D_design_list d_design_list(this);
    //d_init.
    d_design_list.move(0,0);
    d_design_list.exec();

    operate_lock=false;
    mouse_event_lock=false;
    //qDebug() << QSqlDatabase::drivers();
}

#endif

void MainWindow::on_b_file_list_clicked()
{

    QString select_dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),"/mnt",QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks
                                                           |QFileDialog::DontUseSheet|QFileDialog::DontUseNativeDialog
                                                           |QFileDialog::ShowDirsOnly);
    qDebug()<<"path:"<<select_dir;

}

#if 0
void MainWindow::on_b_ftp_list_clicked()
{

    operate_lock=true;
    mouse_event_lock=true;

    D_ftp_list d_ftp_list(this);
    //d_init.
    d_ftp_list.move(0,0);
    d_ftp_list.exec();

    operate_lock=false;
    mouse_event_lock=false;
}
#endif
void MainWindow::drow_network_info()
{
    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,"/app/data/");
    QSettings m_settings(".","ssf.ini");

    ui->l_srv_ip->setText(m_settings.value("ip_srv","192.168.123.99").toString());
    ui->l_op_ip->setText(m_settings.value("ip_addr","192.168.123.200").toString());
}


#if 0
void MainWindow::on_b_work_instruction_list_clicked()
{
    operate_lock=true;
    mouse_event_lock=true;

    D_cmd_list d_cmd_list(this);
    //d_init.
    d_cmd_list.move(0,0);
    d_cmd_list.exec();

    qDebug()<<"this";
    operate_lock=false;
    mouse_event_lock=false;
}
#endif
void MainWindow::on_pushButton_2_clicked()
{
#if 0
    QGraphicsScene* scene = new QGraphicsScene;
    ui->l_graphic->setScene(scene);


    //this->graphicsView->setScene(scene);

    QPixmap pixmap;
    pixmap.load("/SunStar/data/232.JPG");

    QGraphicsPixmapItem * item = scene->addPixmap(pixmap);
    ui->l_graphic->fitInView (item);
    qDebug()<<"ok";
#endif

#if 0
    QPixmap jpg_img;
    jpg_img.load("/SunStar/data/232.JPG","JPG");
    //ui->l_test->setPixmap(jpg_img.scaled(ui->l_test->size(),Qt::KeepAspectRatio));
    ui->l_test->setPixmap(jpg_img.scaled(QSize(800,600),Qt::KeepAspectRatio));
    qDebug()<<ui->l_test->size();
#endif
#if 0
    void CustomGraphicsView::mousePressEvent(QMouseEvent* event)
    {
        if (e->button() == Qt::MiddleButton)
        {
            // Store original position.
            m_originX = event->x();
            m_originY = event->y();
        }
    }

    void CustomGraphicsView::mouseMoveEvent(QMouseEvent* event)
    {
        if (e->buttons() & Qt::MidButton)
        {
            QPointF oldp = mapToScene(m_originX, m_originY);
            QPointF newP = mapToScene(event->pos());
            QPointF translation = newp - oldp;

            translate(translation.x(), translation.y());

            m_originX = event->x();
            m_originY = event->y();
        }
    }
#endif
    ui->l_job_bg->show();

    QGraphicsScene *scene = new QGraphicsScene(QRect(0, 0, 800, 600));

    QPixmap image = QPixmap::fromImage(QImage("232.JPG"));
    //QPixmap image = QPixmap::fromImage(QImage("ssf_bg.png"));
    qDebug()<<"image is null?"<<image.isNull();  ui->test_prog->show();


    operate_lock=true;
    mouse_event_lock=true;


    D_set_code d_set_code(this, 0);
    d_set_code.move(0,0);
    d_set_code.exec();

    if(worker_id!=NULL)
        ui->l_worker->setText(worker_id);

    if(mc_code!=NULL)
        ui->l_mc->setText(mc_code);

    if(job_code!=NULL)
        ui->l_job->setText(job_code);

    operate_lock=false;
    mouse_event_lock=false;

    ui->test_prog->hide();
    QGraphicsPixmapItem * pixMapitem = new QGraphicsPixmapItem();
    pixMapitem->setPixmap(image);
    scene->addItem(pixMapitem);

    QGraphicsView * view = new QGraphicsView(ui->l_test);
    view->setScene(scene);
    view->setGeometry(QRect(0,0,800,600));
    view->scale(job_img_scale,job_img_scale);


    qDebug()<<job_img_scale;

    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->show();

}


void MainWindow::on_pushButton_3_clicked()
{
}

void MainWindow::on_pushButton_5_clicked()
{

    QGraphicsScene *scene = new QGraphicsScene(QRect(0, 0, 800, 600));

    //QPixmap image = QPixmap::fromImage(QImage("232.JPG"));
    QPixmap image = QPixmap::fromImage(QImage("ssf_bg.png"));
    qDebug()<<"image is null?"<<image.isNull();
    QGraphicsPixmapItem * pixMapitem = new QGraphicsPixmapItem();
    pixMapitem->setPixmap(image);
    scene->addItem(pixMapitem);

    QGraphicsView * view = new QGraphicsView(ui->l_test);
    view->setScene(scene);
    view->setGeometry(QRect(0,0,800,600));
    job_img_scale+=0.5;
    view->scale(job_img_scale,job_img_scale);

    qDebug()<<job_img_scale;



    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->show();

}

void MainWindow::on_pushButton_4_clicked()
{

    QGraphicsScene *scene = new QGraphicsScene(QRect(0, 0, 800, 600));

    QPixmap image = QPixmap::fromImage(QImage("/app/232.JPG"));
    //QPixmap image = QPixmap::fromImage(QImage("ssf_bg.png"));
    qDebug()<<"image is null?"<<image.isNull();
    QGraphicsPixmapItem * pixMapitem = new QGraphicsPixmapItem();
    pixMapitem->setPixmap(image);
    scene->addItem(pixMapitem);

    QGraphicsView * view = new QGraphicsView(ui->l_test);
    view->setScene(scene);
    view->setGeometry(QRect(0,0,800,600));
    if(job_img_scale==1)
    {
        job_img_scale=1;
    }else{
        job_img_scale-=0.5;
    }

    view->scale(job_img_scale,job_img_scale);


    qDebug()<<job_img_scale;


    view->setDragMode(QGraphicsView::ScrollHandDrag);
    //view->centerOn();
    view->show();

}

void MainWindow::on_b_graph_view_clicked()
{

}


void MainWindow::on_worker_scanning(QString name)
{
    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->write(name.toLocal8Bit());
        //socket->write(data_temp.toLocal8Bit());
        //state=1;
        //return socket->waitForBytesWritten();
    }
    else{
        //state=0;
    }

    txbuffer.clear();
    txbuffer.append(name);
}
void MainWindow::item_scanning()
{
    event_apear = true;
    is_checking = true;

    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->write("#I");
    }
    else{
        //state=0;
    }

    txbuffer.clear();
    txbuffer.append("#I");
}
void MainWindow::process_scanning(QString job_name)
{
    event_apear = true;
    is_checking = true;
    event_j = true;

    QString send_msg;

    send_msg="#J|";
    send_msg.append(job_name);

    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        qDebug() <<"socketwirte] "<<socket->write(send_msg.toLocal8Bit());
    }
    else{

    }

    txbuffer.clear();
    txbuffer.append(send_msg);
}

void MainWindow::alive_checking()
{

    is_checking = true;

    QString data_tmp = "#R";

    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->write(data_tmp.toLocal8Bit());
    }
    else{
    }
}

void MainWindow::handle_nack(QString msg)
{

    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        qDebug() <<"send alive] "<<socket->write(msg.toLocal8Bit());
    }
}

void MainWindow::on_b_jobinfo_clicked()
{
    D_set_job_info d_set_job_info(this);
    d_set_job_info.move(0,0);
    d_set_job_info.exec();

    QString info;

    info.append(QString::fromUtf8("공정 : "));
    info.append(parsedata[process_num].selected_item);
    info.append(",   ");
    info.append(QString::fromUtf8("세부공정 : "));
    info.append(parsedata[process_num].MC_PROCESS_NAME);
    info.append("\n");
    info.append(QString::fromUtf8("이름 : "));
    info.append(parsedata[process_num].workername + ", ");
    info.append(QString::fromUtf8("ID : "));
    info.append(", "+ parsedata[process_num].worker_id);
    info.append(",   MC_CODE : ");
    info.append(parsedata[process_num].MC_CODE);

    ui->l_infomation->setText(info);
}
void MainWindow::on_b_process_clicked()
{
    D_set_job_info d_set_job_info(this);
    d_set_job_info.move(0,0);
    d_set_job_info.exec();

    QString info;

    info.append(QString::fromUtf8("공정 : "));
    info.append(selected_item);
    info.append(",   ");
    info.append(QString::fromUtf8("세부공정 : "));
    info.append(MC_PROCESS_NAME);
    info.append("\n");
    info.append(QString::fromUtf8("이름 : "));
    info.append(worker_name + ", ");
    info.append(QString::fromUtf8("ID : "));
    info.append(", "+ worker_id);
    info.append(",   MC_CODE : ");
    info.append(MC_SENDER_MC_CODE);

    ui->l_infomation->setText(info);
}

void MainWindow::on_b_worker_clicked()
{
    operate_lock=true;

    D_set_code d_set_code(this,1);
    d_set_code.move(0,0);
    d_set_code.exec();

    QString info;

    info.append(QString::fromUtf8("공정 : "));
    info.append(selected_item);
    info.append(",   ");
    info.append(QString::fromUtf8("세부공정 : "));
    info.append(MC_PROCESS_NAME);
    info.append("\n");
    info.append(QString::fromUtf8("이름 : "));
    info.append(worker_name + ", ");
    info.append(QString::fromUtf8("ID : "));
    info.append(", "+ worker_id);
    info.append(",   MC_CODE : ");
    info.append(MC_SENDER_MC_CODE);

    ui->l_infomation->setText(info);

    operate_lock=false;

    ui->test_prog->hide();
}
void MainWindow::on_b_workers_clicked()
{
    //strWorkerName = "";   181106
    operate_lock=true;
    //qDebug()<<"count init";
    D_set_code d_set_code(this,1);
    //d_init.
    d_set_code.move(0,0);
    /***************************count by worker***************************/
    d_set_code.exec();

    QString info;

    info.append(QString::fromUtf8("공정 : "));
    info.append(selected_item);
    info.append(",   ");
    info.append(QString::fromUtf8("세부공정 : "));
    info.append(MC_PROCESS_NAME);
    info.append("\n");
    info.append(QString::fromUtf8("이름 : "));
    info.append(worker_name + ", ");
    info.append(QString::fromUtf8("ID : "));
//    info.append("( " /*+ worker_id +*/ " )");
    info.append(", "+ worker_id);
    info.append(",   MC_CODE : ");
    info.append(MC_SENDER_MC_CODE);

    ui->l_infomation->setText(info);

    operate_lock=false;
}

void MainWindow::on_b_tuning_clicked()
{

    m_cntbuff = 0;
    operation_second = 0;
    motor_run_stop = 0;
    old_motor_run_stop = 0;

    m_istune = 2;

    ui->grp_monitor->hide();
    ui->grp_tuning->show();

    ui->act_stitch->setText(QString::number(m_cntbuff));
    ui->l_oper_sec->show();
    ui->l_oper_sec->setText(QString::number(operation_second));
    ui->b_tuning_st->setEnabled(true);
    ui->b_tuning_ed->setEnabled(false);
    ui->l_tune_process->setText(QString::number(public_laststat));
}

void MainWindow::on_b_tuning_st_clicked()
{

    ui->b_tuning_st->setEnabled(false);
    ui->b_tuning_ed->setEnabled(true);

    m_istune = 1;

    ui->b_home->hide();

    second=0;
    updateTimer->stop();
    disconnect(updateTimer, SIGNAL(timeout()), this, SLOT(update_sewTime_pulse()));
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(updateTime()));
    updateTimer->start(1000);
}

void MainWindow::on_b_tuning_ed_clicked()
{
    if (m_cntbuff)
    {
        if (parsedata[public_laststat -1].modestat == euTRIM)
        {
            m_cntbuff = 0;
            return ;
        }
        parsedata[public_laststat -1].count = m_cntbuff;
        m_cntbuff = 0;
        QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,SPS_PATH);
        QSettings m_settings(".","ssf.ini");
        if (!public_laststat)
            return;

        m_settings.beginGroup(*Group[public_laststat -1]);
        m_settings.setValue("cnt", parsedata[public_laststat -1].count);
        m_settings.endGroup();
    }

    ui->b_tuning_st->setEnabled(true);
    ui->b_tuning_ed->setEnabled(false);
    m_istune = 2;
    ui->b_home->show();

    updateTimer->stop();
    disconnect(updateTimer, SIGNAL(timeout()), this, SLOT(updateTime()));
    connect(updateTimer, SIGNAL(timeout()), this, SLOT(update_sewTime_pulse()));

    updateTimer->start(1000);


    qDebug()<<actual_stitch<<second;

}

void MainWindow::on_b_home_clicked()
{
    QString tmp;
    m_istune = 0;
    motor_run_stop = 0;
    old_motor_run_stop = 0;
    display_tirmstitch(&tmp, public_laststat);
    ui->grp_setting_menu->hide();
    ui->grp_monitor->show();
    ui->grp_tuning->hide();

}

void MainWindow::on_b_op_setting_clicked()
{
    ui->grp_tuning->hide();
    ui->grp_job_info->hide();
    ui->grp_monitor->hide();
    ui->grp_setting_menu->show();
#if 0
    ui->test_prog->show();


    QPalette pal;
    pal.setColor(QPalette::Background, Qt::red);
    ui->wifi_state_2->setPalette(pal);
    repaint();
    operate_lock=true;
    mouse_event_lock=true;
    //qDebug()<<"count init";


    D_setting d_setting(this);
    //d_init.
    d_setting.move(0,0);
    d_setting.exec();

    ui->test_prog->hide();

    operate_lock=false;
    mouse_event_lock=false;
#endif

}

void MainWindow::show_job_instruction()
{
    ui->l_img->clear();
    //QString fileName = "/home/iamrsm/pattern_qt/SSF-10.4-AB/SSF_OP_10.4_AB/new_images/1old_bg.png";
    QString fileName = "/app/data/"+MC_STYLE_CODE+".jpg";
    QImage image;
    if (!image.load(fileName)) {
        //  ui.label->setText(tr("Selected file is not an image, please select another."));
        ui->l_info->show();
        request_job_instruction();
        qDebug()<<"load img fail";
        return;
    }else{
        ui->l_img->setPixmap(QPixmap::fromImage(image));
        ui->l_info->hide();
    }
    //ui.label->setPixmap(QPixmap::fromImage(image));
    //ui->l_img->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::request_job_instruction()
{

    QString process_cd = MC_ITEM_CD;
    QString line_code = MC_SENDER_MC_CODE;
    QString mc_code = MC_STYLE_CODE;

    QString cmd;

    cmd="#U|";
    cmd.append(process_cd);
    cmd.append("|");
    cmd.append(line_code);
    cmd.append("|");
    cmd.append(mc_code);
    cmd.append("|");
    qDebug()<<"process_cd"<<process_cd<<"line_code"<<line_code<<"mc_code"<<mc_code;

    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->write(cmd.toLocal8Bit());
    }
    else{
        //state=0;
    }
}


void MainWindow::on_b_job_standard_clicked()
{
    ui->grp_menu->hide();
    ui->grp_tuning->hide();
    ui->grp_job_info->show();
    ui->ssid_progressBar->hide();

    show_job_instruction();
}
void MainWindow::on_b_standard_clicked()
{
    ui->grp_monitor->hide();
    ui->grp_job_info->show();
    ui->ssid_progressBar->hide();

    show_job_instruction();
}

void MainWindow::on_b_home_2_clicked()
{
    ui->grp_menu->hide();
    ui->grp_tuning->hide();
    ui->grp_job_info->hide();
    ui->grp_monitor->show();
}

void MainWindow::on_b_monitor_clicked()
{

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    if(!parsedata[public_laststat-1].MC_ITEM_NAME.isNull() && !parsedata[public_laststat-1].MC_PROCESS_NAME.isNull())
    {
        QString tmp;
        tmp.append("아이템명 : ");
        tmp.append(parsedata[public_laststat-1].MC_ITEM_NAME);
        tmp.append("\n");
        tmp.append("공정명 : ");
        tmp.append(parsedata[public_laststat-1].MC_PROCESS_NAME);
        tmp.append("\n");
        tmp.append("ID: ");
        tmp.append(worker_id);
        ui->l_job_title->setText(tmp);
    }

    ui->grp_menu->hide();
    ui->grp_tuning->hide();
    ui->grp_job_info->hide();
    ui->grp_monitor->show();
    ui->l_send->show();
    ui->l_send->setText("카운트 전송");

    QTextCodec::setCodecForTr(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("eucKR"));

}


void MainWindow::on_b_home_3_clicked()
{
    ui->grp_monitor->hide();
    ui->grp_setting_menu->show();
}

void MainWindow::request_time()
{

    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->write("#T");
    }
    else{
    }
}

void MainWindow::on_b_home_4_clicked()
{

    ui->grp_setting_menu->hide();
    ui->grp_menu->hide();
    ui->grp_tuning->hide();
    ui->grp_job_info->hide();
    ui->grp_monitor->show();
}

void MainWindow::on_b_set_network_clicked()
{
    event_apear = true;
    is_checking = true;
    operate_lock=true;
    mouse_event_lock=true;

    D_setting d_setting(this);
    d_setting.move(0,0);
    d_setting.exec();

    ui->test_prog->hide();

    operate_lock=false;
    mouse_event_lock=false;
}

void MainWindow::on_b_set_mc_code_clicked()
{
    operate_lock=true;

    D_set_code d_set_code(this,3);
    d_set_code.move(0,0);
    d_set_code.exec();


    QString info;

    info.append(QString::fromUtf8("공정 : "));
    info.append(selected_item);
    info.append(",   ");
    info.append(QString::fromUtf8("세부공정 : "));
    info.append(MC_PROCESS_NAME);
    info.append("\n");
    info.append(QString::fromUtf8("이름 : "));
    info.append(worker_name + ", ");
    info.append(QString::fromUtf8("ID : "));
    info.append(", "+ worker_id);
    info.append(",   MC_CODE : ");
    info.append(MC_SENDER_MC_CODE);

    ui->l_infomation->setText(info);


    operate_lock=false;
}

void MainWindow::send_countByUP(__u8 index)
{

    QString data_tmp;

    if (parsedata[index].sec_sum && parsedata[index].oper_sum && parsedata[index].stitch_sum)
    {
        second = parsedata[index].sec_sum + second;
        operation_second = parsedata[index].oper_sum + operation_second;
        parsedata[index].stitch_cnt = parsedata[index].stitch_sum + parsedata[index].stitch_cnt;
        parsedata[index].stitch_sum = parsedata[index].sec_sum = parsedata[index].oper_sum = 0;
    }


    data_tmp = QString("$D|-|%1|%2|%3|%4|%5|%6|P|%7|%8|%9|#").arg(parsedata[index].worker_id,//2se
                                                                    parsedata[index].target_cnt,//3
                                                                    QString::number(parsedata[index].process_cnt),
                                                                    QString::number(second),
                                                                    QString::number(parsedata[index].stitch_cnt),
                                                                    QString::number(operation_second),
                                                                    MC_SENDER_MC_CODE,//10 //MC_MODEL ex. 18SHA02001
                                                                    parsedata[index].MC_SENDER_STYLE_CODE_LEFT,//11          //LINE_NM   ex. SH-P3A-3001
                                                                    parsedata[index].MC_SENDER_STYLE_CODE_RIGHT
                                                                        ); //MC_CODE  ex. P012


    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("eucKR"));

    data_tmp = is_valid_data(data_tmp);


    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->write(data_tmp.toLocal8Bit());
    }
    else{
    }


    txbuffer.clear();
    txbuffer.append(data_tmp);

}

void MainWindow::send_countByDN(__u8 index)
{
    QString data_tmp;

    if (parsedata[index].sec_sum && parsedata[index].oper_sum && parsedata[index].stitch_sum)
    {
        second = parsedata[index].sec_sum + second;
        operation_second = parsedata[index].oper_sum + operation_second;
        parsedata[index].stitch_cnt = parsedata[index].stitch_sum + parsedata[index].stitch_cnt;
        parsedata[index].stitch_sum = parsedata[index].sec_sum = parsedata[index].oper_sum = 0;
    }


    data_tmp = QString("$D|-|%1|%2|%3|%4|%5|%6|M|%7|%8|%9|#").arg(parsedata[index].worker_id,//2se
                                                                    parsedata[index].target_cnt,//3
                                                                    QString::number(parsedata[index].process_cnt),
                                                                    QString::number(second),
                                                                    QString::number(parsedata[index].stitch_cnt),
                                                                    QString::number(operation_second),
                                                                    MC_SENDER_MC_CODE,//10 //MC_MODEL ex. 18SHA02001
                                                                    parsedata[index].MC_SENDER_STYLE_CODE_LEFT,//11          //LINE_NM   ex. SH-P3A-3001
                                                                    parsedata[index].MC_SENDER_STYLE_CODE_RIGHT
                                                                        ); //MC_CODE  ex. P012

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("eucKR"));

    data_tmp = is_valid_data(data_tmp);

    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->write(data_tmp.toLocal8Bit());
    }
    else{
    }

    txbuffer.clear();
    txbuffer.append(data_tmp);

}

QString MainWindow::is_valid_data(QString data_tmp)
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    QMessageBox msg;
    msg.setStyleSheet("QLabel{min-width:1200px; font-size: 45px;} QPushButton{ width:600px; font-size: 45px; }");
    if (parsedata[public_laststat-1].MC_ITEM_NAME.contains("NONE") || parsedata[public_laststat-1].worker_id.contains("NONE") || (parsedata[public_laststat-1].MC_PROCESS_NAME.contains("NONE")))
    {
        data_tmp.clear();
        data_tmp = QString("$D|-|%1|#").arg(parsedata[public_laststat -1].worker_id);
    }
    if (parsedata[public_laststat-1].MC_ITEM_NAME.contains("NONE"))
    {
        data_tmp.append("MC_ITEM_NAME ERROR");
        msg.information(this, "error", "아이템 선택이 안되었습니다.");
    }
    if(parsedata[public_laststat-1].worker_id.contains("NONE"))
    {
        data_tmp = "worker_id ERROR";
        msg.information(this, "error", "작업자 이름 입력이 안되었습니다.");
    }
    if(parsedata[public_laststat-1].MC_PROCESS_NAME.contains("NONE"))
    {
        data_tmp = "MC_PROCESS_NAME ERROR";
        msg.information(this, "error", "공정 선택이 안되었습니다.");
    }

    QTextCodec::setCodecForTr(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("eucKR"));

    return data_tmp;
}

void MainWindow::on_b_test_clicked()
{
    ui->l_testing->show();
    ui->b_testing_end->show();
    ui->b_home_3->hide();
    m_istest = 1;
}

void MainWindow::on_pushButton_6_clicked()
{
    paramindex = 0;
    D_set_para d_set_para(this);
    d_set_para.move(0,0);
    d_set_para.exec();
}

void MainWindow::on_proc_no1_pressed()
{
    if(m_Button_Window == 1)
        m_Button_Window = 0;
    else
        m_Button_Window = 1;

    status_change(m_Button_Window);
}

void MainWindow::on_proc_no2_pressed()
{

    if(m_Button_Window == 2)
        m_Button_Window = 0;
    else
        m_Button_Window = 2;

    status_change(m_Button_Window);
}

void MainWindow::on_proc_no3_pressed()
{

    if(m_Button_Window == 3)
        m_Button_Window = 0;
    else
        m_Button_Window = 3;

    status_change(m_Button_Window);
}

void MainWindow::on_proc_no4_pressed()
{
    if(m_Button_Window == 4)
        m_Button_Window = 0;
    else
        m_Button_Window = 4;

    status_change(m_Button_Window);
}

void MainWindow::on_proc_no5_pressed()
{
    if(m_Button_Window == 5)
        m_Button_Window = 0;
    else
        m_Button_Window = 5;

    status_change(m_Button_Window);
}

void MainWindow::on_proc_no6_pressed()
{
    if(m_Button_Window == 6)
        m_Button_Window = 0;
    else
        m_Button_Window = 6;

    status_change(m_Button_Window);
}

void MainWindow::on_proc_no7_clicked()
{
    if(m_Button_Window == 7)
        m_Button_Window = 0;
    else
        m_Button_Window = 7;

    status_change(m_Button_Window);
}

void MainWindow::on_proc_no8_clicked()
{
    if(m_Button_Window == 8)
        m_Button_Window = 0;
    else
        m_Button_Window = 8;

    status_change(m_Button_Window);
}

void MainWindow::on_proc_no9_clicked()
{
    if(m_Button_Window == 9)
        m_Button_Window = 0;
    else
        m_Button_Window = 9;

    status_change(m_Button_Window);
}

void MainWindow::on_proc_no10_clicked()
{
    if(m_Button_Window == 10)
        m_Button_Window = 0;
    else
        m_Button_Window = 10;
        status_change(m_Button_Window);
}

void MainWindow::delay_ms(__u32 delay)
{
    for(__u32 i = 0; i < delay; i ++)
    {
         usleep(1000);
    }
}

void MainWindow::on_b_testing_end_clicked()
{
    ui->l_testing->hide();
    ui->b_testing_end->hide();
    ui->b_home_3->show();
    m_istest = 0;
}

void MainWindow::on_b_sendingdefect_clicked()
{
    ui->l_sendingdefect->hide();
    ui->b_sendingdefect->hide();
    ui->b_home_3->show();
    m_istest = 0;
}

void MainWindow::on_b_senddefect_init_clicked()
{
    int state;

    m_iserrorsend = 1;

    ui->l_sendingdefect->show();
    ui->b_sendingdefect->show();
    ui->b_home_3->hide();


    QString data_tmp;
    data_tmp = QString("$D|-|%1|%2|%3|%4|%5|%6|R|%7|%8|%9|#").arg(parsedata[public_laststat -1].worker_id,//2
                                                                    parsedata[public_laststat -1].target_cnt,
                                                                    QString::number(parsedata[public_laststat -1].process_cnt),
                                                                    QString::number(second),
                                                                    QString::number(parsedata[public_laststat -1].stitch_cnt),
                                                                    QString::number(operation_second),
                                                                    MC_SENDER_MC_CODE,
                                                                    parsedata[public_laststat - 1].MC_SENDER_STYLE_CODE_LEFT,
                                                                    parsedata[public_laststat - 1].MC_SENDER_STYLE_CODE_RIGHT //12
                                                                    );

    parsedata[public_laststat -1].stitch_cnt = 0;
    parsedata[public_laststat -1].trim_cnt = 0;
    operation_second = 0;
    second = 0;

    display_both(ui, parsedata[public_laststat -1].trim_cnt, parsedata[public_laststat -1].stitch_cnt);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    qDebug()<<"data_tmp"<<data_tmp;

    qDebug()<<"process_cnt"<<parsedata[public_laststat -1].process_cnt;
    qDebug()<<"second"<<second;
    qDebug()<<"stitch_cnt"<<parsedata[public_laststat -1].stitch_cnt;

    qDebug()<<data_tmp;

    QTextCodec::setCodecForTr(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("eucKR"));
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("eucKR"));


    if(socket->state() == QAbstractSocket::ConnectedState)
    {
        socket->write(data_tmp.toLocal8Bit());
        //QByteArray bArray = data_tmp.toLocal8Bit();   //181128_$D 전송
        state=1;
        //return socket->waitForBytesWritten();
    }
    else
    {
        state=0;
    }
    //   return false;


    ui->label_2->show();
    if(state)
    {
        ui->label_2->setText("send ok");
    }
    else
    {
        ui->label_2->setText("send fail");
    }
}


void MainWindow::on_proc_data_clicked()
{
    handle_sava_all(Group);
}

void MainWindow::on_proc_no10_2_clicked()
{
    handle_sava_all(Group);
}

void MainWindow::on_proc_no10_3_clicked()
{
    buffer_code[BUFFERINDEX].clear();
}


void MainWindow::on_pushButton_clicked()
{

    paramindex = 1;

    D_set_para d_set_para(this);
    d_set_para.move(0,0);
    d_set_para.exec();
}

void MainWindow::on_b_stitch_to_trim_clicked()
{

    paramindex = 2;

    D_set_para d_set_para(this);
    d_set_para.move(0,0);
    d_set_para.exec();
}

void MainWindow::on_b_interval_clicked()
{

    paramindex = 3;

    D_set_para d_set_para(this);
    d_set_para.move(0,0);
    d_set_para.exec();
}
