#include "d_set_para.h"
#include "ui_d_set_para.h"
#include "d_set_code.h"
#include "d_set_job_info.h"
#include "mainwindow.h"
#include "path.h"

D_set_para::D_set_para(QWidget *parent):
    QDialog(parent, Qt::FramelessWindowHint), ui(new Ui::D_set_para)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    add_key();

    if(w->target_cnt.contains("0"))
    {
    }else{
        ui->lineEdit->insert(w->target_cnt);
    }

    for(__u8 i = 0; i < 10; i++)
        paraGroup[i] = NULL;

    for (__u8 i = 0; i < 10; i++)
        paraGroup[i] = new QString();

    *paraGroup[0] = "1"; //replace to QStringList
    *paraGroup[1] = "2";
    *paraGroup[2] = "3";
    *paraGroup[3] = "4";
    *paraGroup[4] = "5";
    *paraGroup[5] = "6";
    *paraGroup[6] = "7";
    *paraGroup[7] = "8";
    *paraGroup[8] = "9";
    *paraGroup[9] = "10";
}

D_set_para::~D_set_para()
{
    delete ui;
    for (int i = 0; i < 10; i ++)
        delete paraGroup[i];
}

void D_set_para::add_key()
{
    keypad = new QButtonGroup(this);
    connect(keypad, SIGNAL(buttonClicked(int)), this, SLOT(do_keypad_clicked(int)));
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

}

void D_set_para::on_b_close_clicked()
{
    this->close();
}

void D_set_para::on_b_enter_clicked()
{
    QString temp;

    qDebug()<<"enter";
    buffer[BUFFERINDEX] = ui->lineEdit->text();

    if (w->paramindex == 0)
    {
        w->getbufferdata(buffer, buffer_code, &temp, TARGET, 0);
        w->settarget(&parsedata[w->public_laststat -1].target_cnt, &temp);
        w->display_target(&parsedata[w->public_laststat -1].target_cnt);
        memory_in_target(paraGroup);
    }
    else if(w->paramindex == 1)
    {
        parsedata[w->public_laststat -1].gain = buffer[BUFFERINDEX].toInt();
        w->display_factor(&parsedata[w->public_laststat -1].gain);
        memory_in_gain(paraGroup);
    }
    else if(w->paramindex == 2)
    {
        parsedata[w->public_laststat -1].stitch_to_trim = buffer[BUFFERINDEX].toInt();
        memory_in_stitch_to_trim(paraGroup);
    }
    else if(w->paramindex == 3)
    {
        parsedata[w->public_laststat -1].interval = buffer[BUFFERINDEX].toInt();
        memory_in_interval(paraGroup);
    }

    buffer[BUFFERINDEX].clear();
    w->display_param(w->public_laststat);

   this->close();
}

void D_set_para::memory_in_target(QString **Group)
{
    if (!w->public_laststat)
        return;

    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,SPS_PATH);
    QSettings m_settings(".","ssf.ini");

    m_settings.beginGroup(*Group[w->public_laststat -1]);
    m_settings.setValue("target_cnt", parsedata[w->public_laststat -1].target_cnt);
    m_settings.endGroup();
}

void D_set_para::memory_in_gain(QString **Group)
{
    if (!w->public_laststat)
        return;

    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,SPS_PATH);
    QSettings m_settings(".","ssf.ini");

    m_settings.beginGroup(*Group[w->public_laststat -1]);
    m_settings.setValue("gain", parsedata[w->public_laststat -1].gain);
    m_settings.endGroup();
}

void D_set_para::memory_in_stitch_to_trim(QString **Group)
{
    if (!w->public_laststat)
        return;

    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,SPS_PATH);
    QSettings m_settings(".","ssf.ini");

    m_settings.beginGroup(*Group[w->public_laststat -1]);
    m_settings.setValue("stitch_to_trim", parsedata[w->public_laststat -1].stitch_to_trim);
    m_settings.endGroup();
}

void D_set_para::memory_in_interval(QString **Group)
{
    if (!w->public_laststat)
        return;

    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,SPS_PATH);
    QSettings m_settings(".","ssf.ini");

    m_settings.beginGroup(*Group[w->public_laststat -1]);
    m_settings.setValue("interval", parsedata[w->public_laststat -1].interval);
    m_settings.endGroup();
}


void D_set_para::do_keypad_clicked(int key)
{
    QString temp;

    if(key>=0 && key<=9) //input number
    {
        ui->lineEdit->insert(QString::number(key));
    }else if(key==10)
    {
        ui->lineEdit->cursorBackward(1);
        ui->lineEdit->del();
    }else{
        temp.sprintf("%c",key);
        ui->lineEdit->insert(temp);
    }

}


























