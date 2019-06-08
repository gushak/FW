#include "d_setting.h"
#include "ui_d_setting.h"
#include <QGraphicsOpacityEffect>
#include <QProcess>

D_setting::D_setting(QWidget *parent):
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::D_setting)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);
#if 0 //alpha effect
    QGraphicsOpacityEffect * effect = new QGraphicsOpacityEffect();
    effect->setOpacity(0.9);
    ui->l_bg->setGraphicsEffect(effect);
#endif

    keypad = new QButtonGroup(this);
    connect(keypad, SIGNAL(buttonClicked(int)), this, SLOT(do_keypad_clicked(int)));

    ui->b_ipsetting->setChecked(true);
    ui->b_wifi_setting->setChecked(false);
    ui->lineEdit_ip->setFocus();
    ui->b_static_ip->setChecked(true);

    /*
 *iwlist wlan0 scan|grep -A 4 "global_1"|grep Encryption|awk '{print $2}'|sed 's/key://g'
 *
 **/

    keypad->addButton(ui->b_0,0);
    keypad->addButton(ui->b_1,1);
    keypad->addButton(ui->b_2,2);
    keypad->addButton(ui->b_3,3);
    keypad->addButton(ui->b_4,4);
    keypad->addButton(ui->b_5,5);
    keypad->addButton(ui->b_6,6);
    keypad->addButton(ui->b_7,7);
    keypad->addButton(ui->b_8,8);
    keypad->addButton(ui->b_9,9);
    keypad->addButton(ui->b_bs,10);
    keypad->addButton(ui->b_comma,11);
    keypad->addButton(ui->b_enter,12);


    wwpa_flag=false;
    ui->grp_pwsetting->hide();

    ui->progressBar->hide();
    ui->l_info->hide();
    ui->grp_ipsetting->show();
    ui->grp_wifisetting->hide();

    m_qlw = new QListWidget(ui->listWidget);

    m_qlw->resize(321,341);
    m_qlw->move(0,0);
    m_nLCnt = 0;


    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,"/app/data/");
    QSettings m_settings(".","ssf.ini");


    if(m_settings.value("network_type").toInt()==1) //dhcp setting
    {
        ui->b_static_ip->setChecked(false);
        ui->b_dhcp_ip->setChecked(true);

        ui->lineEdit_ip->insert(m_settings.value("ip_addr","192.168.123.200").toString());
        ui->lineEdit_sub->insert(m_settings.value("ip_subnet","255.255.255.0").toString());
        ui->lineEdit_gate->insert(m_settings.value("ip_gate","255.255.255.0").toString());
        ui->lineEdit_srv->insert(m_settings.value("ip_srv","192.168.123.99").toString());

        ui->lineEdit_ip->setEnabled(false);
        ui->lineEdit_gate->setEnabled(false);
        ui->lineEdit_sub->setEnabled(false);

        ui->lineEdit_srv->setFocus();
    }else{ //static setting
        ui->b_static_ip->setChecked(true);
        ui->b_dhcp_ip->setChecked(false);

        ui->lineEdit_ip->insert(m_settings.value("ip_addr","192.168.123.").toString());
        ui->lineEdit_sub->insert(m_settings.value("ip_subnet","255.255.255.0").toString());
        ui->lineEdit_gate->insert(m_settings.value("ip_gate","192.168.123.1").toString());
        ui->lineEdit_srv->insert(m_settings.value("ip_srv","192.168.123.26").toString());
    }
}

D_setting::~D_setting()
{
    delete ui;
}


void D_setting::on_b_close_clicked()
{
    delete(m_qlw);
    this->close();
}

void D_setting::on_b_ipsetting_clicked()
{
    ui->progressBar->hide();
    if(ui->b_wifi_setting->isChecked())
    {
        ui->b_wifi_setting->setChecked(false);
        ui->b_ipsetting->setChecked(true);
        ui->grp_wifisetting->hide();
        ui->grp_ipsetting->show();
        ui->grp_pwsetting->hide();
        ui->l_info->hide();
        repaint();

    }
    else
    {
        ui->b_wifi_setting->setChecked(false);
        ui->b_ipsetting->setChecked(true);
        ui->grp_wifisetting->hide();
        ui->grp_pwsetting->hide();
        ui->grp_ipsetting->show();
        ui->l_info->hide();
        repaint();

    }
}


void D_setting::on_b_wifi_setting_clicked()
{
    ui->progressBar->hide();

    if(ui->b_ipsetting->isChecked())
    {
        ui->b_SCANNING->hide();
        ui->b_connect->hide();

        ui->b_ipsetting->setChecked(false);
        ui->b_wifi_setting->setChecked(true);
        ui->grp_wifisetting->show();
        ui->grp_ipsetting->hide();
        ui->grp_pwsetting->hide();
        ui->l_info->hide();
        repaint();
        repaint();
        on_b_SCANNING_clicked();
        ui->b_SCANNING->show();
        ui->b_connect->show();

    }
    else
    {
        if(wwpa_flag)
        {
            ui->b_ipsetting->setChecked(false);
            ui->b_wifi_setting->setChecked(true);
            ui->grp_wifisetting->show();
            ui->grp_ipsetting->hide();
            ui->grp_pwsetting->hide();
            ui->l_info->hide();
            on_b_SCANNING_clicked();

            repaint();
            wwpa_flag=false;
        }
        else
        {
            ui->b_ipsetting->setChecked(false);
            ui->b_wifi_setting->setChecked(true);
            ui->grp_wifisetting->show();
            ui->grp_ipsetting->hide();
            ui->grp_pwsetting->hide();
            ui->l_info->hide();
            repaint();
        }
    }

}
void D_setting::on_b_connect_clicked()
{
#if 0
    int nIdx = m_qlw->currentRow();
    int nCnt = m_qlw->count();
    QString strBuf;

    m_qlw->takeItem(nIdx);
#endif

    ui->b_connect->setEnabled(false);
    repaint();

    //int nIdx = m_qlw->currentRow();
    int nCnt = m_qlw->count();

    if(nCnt==0||m_qlw->currentItem()->text()==NULL)
    {
        QMessageBox::information(this, "error", "invisible WIFI");
    }
    else
    {
#ifdef __arm__
        //*iwlist wlan0 scan|grep -A 4 "global_1"|grep Encryption|awk '{print $2}'|sed 's/key://g'

        if(w->socket->isOpen())
        {
            w->socket->close();
        }

        QStringList arguments;
        QProcess process;
        QString scan_ssid_cmd;
        scan_ssid_cmd="iwlist wlan0 scan|grep -A 4 \"";
        scan_ssid_cmd.append(m_qlw->currentItem()->text());
        scan_ssid_cmd.append("\" |grep Encryption|awk '{print $2}'|sed 's/key://g'");

        arguments << "-c" << scan_ssid_cmd;
        process.start("sh" ,arguments);
        process.waitForFinished(3000);

        QString result;
        result=process.readAllStandardOutput();

        result.simplified();

        if(result.at(1).toLatin1()=='n') //passwd on
        {
            wwpa_flag=true;
            set_wpa_pw();

        }
        else if(result.at(1).toLatin1()=='f') //passwd off
        {
            QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,"/app/data/");
            QSettings m_settings(".","ssf.ini");

            ui->grp_wifisetting->hide();
            ui->progressBar->setValue(0);
            ui->progressBar->show();
            ui->l_info->show();
            repaint();

            system("ifconfig wlan0 down");
            system("sync");
            system("echo ctrl_interface=/var/run/wpa_supplicant > /etc/wpa_supplicant.conf");
            system("sync");
            system("echo ap_scan=1 >> /etc/wpa_supplicant.conf");
            system("sync");
            system("echo network={ >> /etc/wpa_supplicant.conf");
            system("sync");
            system("echo key_mgmt=NONE >> /etc/wpa_supplicant.conf");
            system("sync");

            m_settings.setValue("ssid",m_qlw->currentItem()->text());

            QString ssid_command;
            QString echo_command;

            ssid_command.clear();
            ssid_command = "ssid=\"";
            ssid_command.append(m_qlw->currentItem()->text());
            ssid_command.append("\"");

            echo_command="echo ";
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

            ui->progressBar->setValue(100);

        }else
        {
            qDebug()<<"fail";
        }
#endif///////////////////////////////////////////////////////////////////////////////////////////////////
    }
    ui->b_connect->setEnabled(true);

}

void D_setting::on_b_SCANNING_clicked()
{
    m_qlwi = new QListWidgetItem(m_qlw, QListWidgetItem::Type);

    for(int i=0;i<m_nLCnt;i++)
    {
        delete(m_qlw->takeItem(i));
    }
    m_qlw->clear();

    repaint();
    repaint();
    ui->ssid_loading->show();
    ui->ssid_progressBar->setValue(0);
    ui->ssid_progressBar->show();
    ui->ssid_progressBar->setValue(20);

    ui->b_SCANNING->setEnabled(false);
    ui->ssid_progressBar->setValue(40);
    repaint();

    m_nLCnt=0;

    QStringList arguments;
    QProcess process;

    arguments << "-c" << "iwlist wlan0 scan | grep 'ESSID:'|awk '{print $1}'|sed 's/ESSID://g'|sed 's/\"//'|sed 's/\"//'|egrep -v \"^[[:space:]]*$\"";

    process.start("sh" ,arguments);

    ui->ssid_progressBar->setValue(50);

    //process.start("iwlist wlan0 scan | grep 'ESSID:'|awk '{print $1}'|sed 's/ESSID://g'|sed 's/"//'|egrep -v \"^[[:space:]]*$\"");
    process.waitForFinished(3000);
    //sed 's/key://g'

    ui->ssid_progressBar->setValue(70);

    QString result;
    QStringList wifi_list;

    result=process.readAllStandardOutput();
    //result=result.simplified();
    //qDebug()<<"result:"<<result;
    wifi_list=result.split("\n");

    int k;

    for(k=0;k<wifi_list.count();k++)
    {
        if(wifi_list.value(k)!=NULL)
        {
            list_SSID.insert(k,wifi_list.value(k));
        }
    }

    m_nLCnt=0;

    ui->ssid_progressBar->setValue(80);

#ifdef __arm__
    QFont myfont("sunfont");
    myfont.setPointSize(25);
    m_qlw->setFont(myfont);

    for(k=0;k<list_SSID.count();k++)
    {
        //m_qlwi->setText(list_SSID.value(k));
        m_qlw->insertItem(m_nLCnt, list_SSID.value(k));
        m_qlw->setCurrentRow(m_nLCnt++);
    }
#else
    //ui->listWidget->setIconsize(QSize(100,100));
    //ui->listWidget->setIconSize(QSize(100,100));
    //m_qlw->s
    //    valList =  new QListWidget(this);
    //    valList->setSizePolicy (QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
    //    valList->setMinimumSize (QSize(1111, 111));
    //m_qlw->setSizePolicy (QSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored));
    // m_qlw->setMinimumSize(QSize(300,300));
    //m_qlw->setFixedSize(m_qlw->sizeHintfo);

    m_qlwi->setText("test");

    QFont myfont("sunfont");
    myfont.setPointSize(25);

    m_qlw->insertItem(m_nLCnt,m_qlwi );
    m_qlw->setCurrentRow(m_nLCnt++);

#endif
    // QString strBuf = "test";
    //qDebug()<<"list_SSID :"<<list_SSID;
#if 0 //reference
    QString strBuf = ui->LE_TEXT->text();

    if(strBuf.length() <= 0)
    {
        QMessageBox::information(this, "error", "input text!");
        return;
    }
#endif

#if 0
    ncm->updateConfigurations();
    myList = ncm->allConfigurations();
    for(int i = 0; i < myList.size(); i ++)
    {
        qDebug() << myList.at(i).name();
    }
#endif

#if 0
    QString cmd = "iwlist wlan0 scan";


    QProcess process;
    process.start("iwlist wlan0 scan", QStringList() << "-c" << cmd);
    process.waitForFinished();
    QString output(process.readAllStandardOutput());
    if( output.contains("hostapd")){
        //return true;
    }
#endif

    ui->ssid_progressBar->setValue(100);

    ui->b_SCANNING->setEnabled(true);

    ui->ssid_loading->hide();
    ui->ssid_progressBar->hide();

}
void D_setting::do_keypad_clicked(int key)
{

    if(key==12)
    {
        QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,"/app/data");
        QSettings m_settings(".","ssf.ini");

        //dhcp? static? dhcp=1, static=2
        if(ui->b_dhcp_ip->isChecked())
        {
            if(ui->lineEdit_srv->text()!=NULL)
            {
                m_settings.setValue("network_type",1);//dhcp
                m_settings.setValue("ip_srv",ui->lineEdit_srv->text());

                if(w->socket->isOpen())
                {
                    w->socket->close();
                    w->get_time_info = false; //kimkt(190201)
                    qDebug() << "1.get_time_info = false";
                }
                set_dhcp_ip();

             }
            else
            {
                QMessageBox::information(this, "error", "input SERVER IP");
                qDebug()<<"ip error";
            }
        }
        else{   //static ip
            if(ui->lineEdit_ip->text()!=NULL && ui->lineEdit_sub->text()!=NULL && ui->lineEdit_gate->text()!=NULL && ui->lineEdit_srv->text()!=NULL)
            {
                m_settings.setValue("network_type",2); //static ip
                m_settings.setValue("ip_addr",ui->lineEdit_ip->text());
                m_settings.setValue("ip_subnet",ui->lineEdit_sub->text());
                m_settings.setValue("ip_gate",ui->lineEdit_gate->text());
                m_settings.setValue("ip_srv",ui->lineEdit_srv->text());

                if(w->socket->isOpen())
                {
                    w->socket->close();
                    w->get_time_info = false;
                }
                set_static_ip();

            }
            else{
                QMessageBox::information(this, "error", "input IP");
                qDebug()<<"ip error";
            }
        }

        w->drow_network_info();

    }
    else
    {
        if(ui->lineEdit_ip->hasFocus())
        {
            //QString
            if(0 <= key && key <= 9)
            {
                ui->lineEdit_ip->insert(QString::number(key));
            }else if(key==10)
            {
                ui->lineEdit_ip->cursorBackward(1);
                ui->lineEdit_ip->del();
            }else if(key==11)
            {
                ui->lineEdit_ip->insert(".");
            }
        }else if(ui->lineEdit_sub->hasFocus())
        {
            if(0 <= key && key <= 9)
            {
                ui->lineEdit_sub->insert(QString::number(key));
            }else if(key==10)
            {
                ui->lineEdit_sub->cursorBackward(1);
                ui->lineEdit_sub->del();
            }else if(key==11)
            {
                ui->lineEdit_sub->insert(".");
            }
        }else if(ui->lineEdit_gate->hasFocus())
        {
            if(0 <= key && key <= 9)
            {
                ui->lineEdit_gate->insert(QString::number(key));
            }else if(key==10)
            {
                ui->lineEdit_gate->cursorBackward(1);
                ui->lineEdit_gate->del();
            }else if(key==11)
            {
                ui->lineEdit_gate->insert(".");
            }
        }else if(ui->lineEdit_srv->hasFocus())
        {
            if(0 <= key && key <= 9)
            {
                ui->lineEdit_srv->insert(QString::number(key));
            }else if(key==10)
            {
                ui->lineEdit_srv->cursorBackward(1);
                ui->lineEdit_srv->del();
            }else if(key==11)
            {
                ui->lineEdit_srv->insert(".");
            }
        }
    }

}

void D_setting::set_static_ip()
{
    //qDebug()<<"seting static ip";
    //sudo ifconfig eth0 192.168.2.5 netmask 255.255.255.0 broadcast 192.168.2.7
    ui->grp_ipsetting->hide();

    //    m_settings.setValue("network_type",2); //static ip
    //    m_settings.setValue("ip_addr",ui->linfeEdit_ip->text());
    //    m_settings.setValue("ip_subnet",ui->lineEdit_sub->text());
    //    m_settings.setValue("ip_gate",ui->lineEdit_gate->text());
    //    m_settings.setValue("ip_srv",ui->lineEdit_srv->text());
    ui->progressBar->setValue(0);
    ui->progressBar->show();

    //ui->l_info->setText("네트워크 설정을 변경중 입니다.");
    ui->l_info->show();
    repaint();

    w->srv_ip.clear();
    w->srv_ip = ui->lineEdit_srv->text();

    w->op_ip.clear();
    w->op_ip = ui->lineEdit_ip->text();

#ifdef __arm__

    system("ifconfig wlan0 down");
    system("sync");
    system("sync");
    system("sync");

    //QString gate_command;
    QString ip_command;
    QString echo_command;
    ip_command.clear();
    ip_command = "ifconfig wlan0 ";
    ip_command.append(ui->lineEdit_ip->text());
    ip_command.append(" netmask ");
    ip_command.append(ui->lineEdit_sub->text());

    ui->progressBar->setValue(30);
    repaint();
    echo_command="echo ";
    echo_command.append(ip_command);
    echo_command.append(" > /app/ip_setting.sh");

    system(echo_command.toLocal8Bit());
    system("sync");
    system("sync");
    system("sync");

    ui->progressBar->setValue(70);
    repaint();
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

    qDebug()<<"ipsetting ok";
#else
    qDebug()<<"this is host pc";
#endif
    ui->l_info->show();
    //ui->l_info->setText("네트워크 설정이 완료 되었습니다.");
    ui->progressBar->setValue(100);
    repaint();
}

void D_setting::set_dhcp_ip()
{

    QString del_ip_cmd;

    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,"/app/data/");
    QSettings m_settings(".","ssf.ini");

    ui->grp_ipsetting->hide();
    //ui->l_info->setText("네트워크 설정을 변경중 입니다.");
    ui->l_info->show();
    ui->progressBar->show();
    ui->progressBar->setValue(10);
    repaint();

    qDebug()<<"seting dhcp ip";
    w->srv_ip.clear();
    w->srv_ip = ui->lineEdit_srv->text();

#ifdef __arm__

    system("ifconfig wlan0 down");
    system("sync");
    system("sync");
    system("sync");


    del_ip_cmd = "ip addr del ";
    del_ip_cmd.append(m_settings.value("ip_addr","192.168.123.210").toString());
    del_ip_cmd.append("/24 dev wlan0");

    system(del_ip_cmd.toLocal8Bit());
    system("sync");
    system("sync");


    ui->progressBar->setValue(30);
    repaint();

    //QString gate_command;
    QString ip_command;
    QString echo_command;
    ip_command.clear();
    ip_command = "ifconfig wlan0 up";

    echo_command="echo ";
    echo_command.append(ip_command);
    echo_command.append(" > /app/ip_setting.sh");

    system(echo_command.toLocal8Bit());
    system("sync");
    system("sync");
    system("sync");

    system("echo sync >> /app/ip_setting.sh");
    system("sync");
    system("sync");

    system("echo sync >> /app/ip_setting.sh");
    system("sync");
    system("sync");

    system("echo \"dhcpcd wlan0 &\" >> /app/ip_setting.sh");
    system("sync");
    system("sync");
    //qDebug()<<"dhcp echo!!";


    //qDebug()<<"command"<<ip_command;
    //qDebug()<<"echo"<<echo_command;


    //    system("route delete default");

    //    gate_command="route add default gw ";
    //    gate_command.append(ui->lineEdit_gate->text());

    //    system(gate_command.toLocal8Bit());

    //    system("sync");
    //    system("sync");
    //    system("sync");

    system("chmod 777 /app/ip_setting.sh");
    system("sync");
    system("sync");
    system("sync");


    system("ifconfig wlan0 up");
    system("sync");
    system("sync");
    system("sync");


    ui->progressBar->setValue(50);
    repaint();
    QString ssid_name;

    ssid_name = "iwconfig wlan0 essid ";
    ssid_name.append(m_settings.value("ssid","global_1").toString());
    //ssid_name.append(m_settings.value("ssid_pw","13572468").toString());  //load pwd

    system(ssid_name.toLocal8Bit());
    system("sync");
    system("sync");
    system("sync");


    system("dhcpcd wlan0 &");
    system("sync");
    system("sync");
    system("sync");


    ui->progressBar->setValue(60);
    repaint();

    sleep(3);
    ui->progressBar->setValue(70);
    repaint();
    sleep(3);
    ui->progressBar->setValue(80);
    repaint();
    sleep(2);

    //app/data/ip_setting.sh"
    //qDebug()<<"ipsetting ok";

    ui->grp_ipsetting->hide();
    ui->l_info->show();

    //    process.start("ifconfig wlan0 up");
    //    process.waitForFinished(-1);
    //arguments << "-c" << "iwlist wlan0 scan | grep ESSID";

    //ifconfig | grep -A 1 wlan0 | grep addr: | awk '{print $2}' | sed 's/addr://g'
    //arguments << "-c" << "ifconfig wlan0|grep addr:|awk '{print $2}'|sed 's/addr://g'"; // can't use command

    ui->progressBar->setValue(90);
    repaint();
    QStringList arguments;
    arguments << "-c" << "ifconfig | grep -A 1 wlan0 | grep addr: | awk '{print $2}' | sed 's/addr://g'";

    QProcess getProcess;
    getProcess.start("sh" ,arguments);
    sleep(3);
    getProcess.waitForFinished();
    QString tmp = getProcess.readAll();
    tmp.remove(QRegExp("[\\n\\t\\r]"));


    tmp.replace("\"","");
    //tmp.simplified();
    qDebug() <<"result"<<tmp;

    m_settings.setValue("ip_addr",tmp);


    ui->progressBar->setValue(100);
    //ui->progressBar->setFormat("OK");
    //ui->l_info->setText("네트워크 설정이 완료 되었습니다.");
    repaint();

    //getProcess.start(arguments);
    //getProcess.waitForFinished(3000);
    //111111111111111111111111111111111111

    //    if(getProcess.exitCode() == 0)
    //    {
    //        if(getProcess.readAllStandardOutput()==0)
    //        {
    //            qDebug()<<"ip setting ok1 : ip addr"<<getProcess.readAll();
    //        }else
    //        {
    //            qDebug()<<"ip setting ok 2: ip addr"<<getProcess.readAll();
    //        }
    //    }else{
    //        qDebug()<<"process fail";
    //        qDebug()<<getProcess.readAllStandardError();
    //    }

    //qDebug()<<message;

#if 0 //sample
    QString prog = "/bin/bash";//shell
    QStringList arguments;
    arguments << "-c" << "ifconfig eth0 | grep ‘inet ‘ | awk ‘{print $2}’ | sed ’s/addr://’";
    QProcess* process = new QProcess(this);
    process->start(prog , arguments);
    process->waitForFinished();
    QString tmp = process->readAll();
    qDebug() << tmp;
#endif

#else
    qDebug()<<"this is host pc";
#endif
}

void D_setting::on_b_static_ip_clicked()
{
    if(ui->b_dhcp_ip->isChecked())
    {
        ui->b_dhcp_ip->setChecked(false);
        ui->lineEdit_ip->setEnabled(true);
        ui->lineEdit_gate->setEnabled(true);
        ui->lineEdit_sub->setEnabled(true);

        ui->lineEdit_ip->setFocus();

    }else{
        ui->b_static_ip->setChecked(true);
        ui->lineEdit_ip->setFocus();

    }
}

void D_setting::on_b_dhcp_ip_clicked()
{
    if(ui->b_static_ip->isChecked())
    {
        ui->b_static_ip->setChecked(false);
        ui->lineEdit_ip->setEnabled(false);
        ui->lineEdit_gate->setEnabled(false);
        ui->lineEdit_sub->setEnabled(false);
        ui->lineEdit_srv->setFocus();
        ui->lineEdit_srv->setSelection(ui->lineEdit_srv->cursorPosition(),ui->lineEdit_srv->cursorPosition());
    }else{
        ui->b_dhcp_ip->setChecked(true);
        ui->lineEdit_srv->setFocus();

    }
}

void D_setting::set_wpa_pw()
{

    //PW SETTING
    pw_keypad = new QButtonGroup(this);

    connect(pw_keypad, SIGNAL(buttonClicked(int)), this, SLOT(do_pw_keypad_clicked(int)));

    pw_keypad->addButton(ui->b__0,0);
    pw_keypad->addButton(ui->b__1,1);
    pw_keypad->addButton(ui->b__2,2);
    pw_keypad->addButton(ui->b__3,3);
    pw_keypad->addButton(ui->b__4,4);
    pw_keypad->addButton(ui->b__5,5);
    pw_keypad->addButton(ui->b__6,6);
    pw_keypad->addButton(ui->b__7,7);
    pw_keypad->addButton(ui->b__8,8);
    pw_keypad->addButton(ui->b__9,9);
    pw_keypad->addButton(ui->b__bs,10);
    pw_keypad->addButton(ui->b__try_connect,11);

    ui->grp_ipsetting->hide();
    ui->grp_pwsetting->show();
    ui->grp_wifisetting->hide();
}


void D_setting::try_wwpa_connect()
{
    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,"/app/data/");
    QSettings m_settings(".","ssf.ini");

    QString ssid_command;
    QString echo_command;
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

    m_settings.setValue("ssid",m_qlw->currentItem()->text());
    m_settings.setValue("ssid_pw",ui->lineEdit_pw->text());

    /*ssid*/
    system("echo -n ssid='\"' >> /etc/wpa_supplicant.conf");
    system("sync");

    ssid_command.clear();
    ssid_command = m_qlw->currentItem()->text();
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
    ssid_command = ui->lineEdit_pw->text();
    //ssid_command = m_qlw->currentItem()->text();
    ssid_command.append("'\"'");

    echo_command="echo -e \\";
    echo_command.append(ssid_command);
    //echo_command.append(pw_command);    //load pwd
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


    ui->grp_pwsetting->hide();

    //ui->l_info->setText("변경 완료");
    ui->progressBar->setValue(100);
    ui->l_info->show();
    repaint();
}

void D_setting::do_pw_keypad_clicked(int key)
{

    if(0 <= key && key <= 9)
    {
        ui->lineEdit_pw->insert(QString::number(key));
    }else if(key==10)
    {
        ui->lineEdit_pw->cursorBackward(1);
        ui->lineEdit_pw->del();
    }else if(key==11)
    {
        qDebug()<<"try connect";
        if(ui->lineEdit_srv->text()!=NULL)
        {
            ui->b__try_connect->setEnabled(false);
            ui->lineEdit_pw->setEnabled(false);

            repaint();
            try_wwpa_connect();
            ui->b__try_connect->setEnabled(true);
            ui->lineEdit_pw->setEnabled(true);
        }else{

            ui->l_wwpa_info->setText("패스워드를 입력해주세요.");
            ui->l_wwpa_info->show();
            sleep(2);
            ui->l_wwpa_info->hide();
        }

    }
}

