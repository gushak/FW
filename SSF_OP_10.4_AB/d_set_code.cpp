    #include "d_set_code.h"
#include "ui_d_set_code.h"
#include "mainwindow.h"
#include "path.h"

QString buffer_code[BUFFERINDEX +1];

D_set_code::D_set_code(QWidget *parent,int set_mode): QDialog(parent, Qt::FramelessWindowHint), ui(new Ui::D_set_code)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    qDebug()<<"set_mode"<<set_mode;
    if(set_mode==1){
        dialog_mode=set_mode;

        //ui->lineEdit->insert("SH-PC-");
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
        QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
        QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));


        QTextCodec::setCodecForTr(QTextCodec::codecForName("eucKR"));
        QTextCodec::setCodecForCStrings(QTextCodec::codecForName("eucKR"));
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("eucKR"));

    }
    else
    {
        dialog_mode=set_mode;
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
        QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
        QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));


        ui->l_title->setText("기계코드 설정");

        QTextCodec::setCodecForTr(QTextCodec::codecForName("eucKR"));
        QTextCodec::setCodecForCStrings(QTextCodec::codecForName("eucKR"));
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("eucKR"));
    }

    ui->register_box->hide();
    add_key();

    for(__u8 i = 0; i < 10; i++)
        codeGroup[i] = NULL;

    for (__u8 i = 0; i < 10; i++)
        codeGroup[i] = new QString();

    *codeGroup[0] = "1"; //replace to QStringList
    *codeGroup[1] = "2";
    *codeGroup[2] = "3";
    *codeGroup[3] = "4";
    *codeGroup[4] = "5";
    *codeGroup[5] = "6";
    *codeGroup[6] = "7";
    *codeGroup[7] = "8";
    *codeGroup[8] = "9";
    *codeGroup[9] = "10";
}

D_set_code::~D_set_code()
{
    delete ui;
    for (int i = 0; i < 10; i ++)
        delete codeGroup[i];
}

void D_set_code::add_key()
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

    keypad->addButton(ui->b_q,'Q');
    keypad->addButton(ui->b_w,'W');
    keypad->addButton(ui->b_e,'E');
    keypad->addButton(ui->b_r,'R');
    keypad->addButton(ui->b_t,'T');
    keypad->addButton(ui->b_y,'Y');
    keypad->addButton(ui->b_u,'U');
    keypad->addButton(ui->b_i,'I');
    keypad->addButton(ui->b_o,'O');
    keypad->addButton(ui->b_p,'P');

    keypad->addButton(ui->b_a,'A');
    keypad->addButton(ui->b_s,'S');
    keypad->addButton(ui->b_d,'D');
    keypad->addButton(ui->b_f,'F');
    keypad->addButton(ui->b_g,'G');
    keypad->addButton(ui->b_h,'H');
    keypad->addButton(ui->b_j,'J');
    keypad->addButton(ui->b_k,'K');
    keypad->addButton(ui->b_l,'L');

    keypad->addButton(ui->b_z,'Z');
    keypad->addButton(ui->b_x,'X');
    keypad->addButton(ui->b_c,'C');
    keypad->addButton(ui->b_v,'V');
    keypad->addButton(ui->b_b,'B');
    keypad->addButton(ui->b_n,'N');
    keypad->addButton(ui->b_m,'M');
    keypad->addButton(ui->b_slash,'/');
    keypad->addButton(ui->b_dash,'-');
    keypad->addButton(ui->b_bs,10);

}
void D_set_code::on_b_close_clicked()
{
    this->close();
}

void D_set_code::on_b_enter_clicked()
{
    QString work_id;
    QString tmp;

    if(dialog_mode==1)
    {
        w->is_multi = 0;
        buffer_code[BUFFERINDEX] = ui->lineEdit->text();
        w->getbufferdata(buffer, buffer_code, &work_id, WORKER, w->is_multi);
        w->setworkid(&parsedata[w->public_laststat -1].worker_id, &work_id);
        w->display_worker_id(parsedata[w->public_laststat -1].worker_id, &tmp);
        memory_in_worker(codeGroup);
        this->close();
    }
    else
    {
        ui->b_enter_2->hide();
        buffer[BUFFERINDEX] = ui->lineEdit->text();
        w->MC_SENDER_MC_CODE.clear();
        w->MC_SENDER_MC_CODE.append(buffer[BUFFERINDEX]);
        memory_in_mc(w->MC_SENDER_MC_CODE);

        buffer[BUFFERINDEX].clear();
        this->close();
    }
}

void D_set_code::memory_in_worker(QString **Group)
{
    if (!w->public_laststat)
        return;

    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,SPS_PATH);
    QSettings m_settings(".","ssf.ini");

    m_settings.beginGroup(*Group[w->public_laststat -1]);
    m_settings.setValue("worker_id", parsedata[w->public_laststat -1].worker_id);
    m_settings.endGroup();
}

void D_set_code::memory_in_mc(QString code)
{
    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,SPS_PATH);
    QSettings m_settings(".","ssf.ini");
    m_settings.setValue("MC_SENDER_MC_CODE", code);

}
void D_set_code::enter_event()
{
}

void D_set_code::do_keypad_clicked(int key)
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

void D_set_code::on_b_yes_clicked()
{
    w->worker_name=tmp;
    w->worker_id_number=ui->lineEdit->text();

    qDebug()<<"worker_name"<<w->worker_name;
    qDebug()<<"worker_id_number"<<w->worker_id_number;

    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,SPS_PATH);
    QSettings m_settings(".","ssf.ini");
    m_settings.setValue("worker-name",w->worker_name);
    m_settings.setValue("worker-number",w->worker_id_number);

    this->close();
}

void D_set_code::on_b_no_clicked()
{
    this->close();
}

void D_set_code::rcvd_value()
{
    #if 0
    if(w->socket_msg_box == "F")
    {

    }else
    {

        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
        QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
        QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

        ui->key_pad->hide();
        ui->register_box->show();

        tmp= w->socket_msg_box;
        tmp.replace("#W|","");

        comment.clear();
        //comment="작업자 : ";
        comment=tmp;
        comment.append(" 님 맞습니까?");

        //ui->l_content->setText(comment);
        ui->l_content->setText(comment);

        QTextCodec::setCodecForTr(QTextCodec::codecForName("eucKR"));
        QTextCodec::setCodecForCStrings(QTextCodec::codecForName("eucKR"));
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("eucKR"));
    }
    #endif
}


void D_set_code::on_b_enter_2_clicked()
{
    QString work_id;
    QString tmp;

    if (dialog_mode == 3)
    {
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
        QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
        QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

        QMessageBox msg;
        msg.setStyleSheet("QLabel{min-width:1200px; font-size: 45px;} QPushButton{ width:600px; font-size: 45px; }");
        msg.information(this, "error", "기계코드는 일괄등록을 지원하지 않습니다.");

        QTextCodec::setCodecForTr(QTextCodec::codecForName("eucKR"));
        QTextCodec::setCodecForCStrings(QTextCodec::codecForName("eucKR"));
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("eucKR"));

        return ;
    }

    w->is_multi = 1;
    buffer_code[BUFFERINDEX] = ui->lineEdit->text();
    w->getbufferdata(buffer, buffer_code, &work_id, WORKER, w->is_multi);
    w->setworkid(&parsedata[w->public_laststat-1].worker_id, &work_id);
    w->display_worker_id(work_id, &tmp);

    this->close();
}
