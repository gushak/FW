#include "d_set_job_info.h"
#include "ui_d_set_job_info.h"
#include "mainwindow.h"
#include "path.h"

#define KR_ON   set_locale(true);
#define KR_OFF   set_locale(false);

Work parsedata[12];
QString buffer[BUFFERINDEX +1];

D_set_job_info::D_set_job_info(QWidget *parent):
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::D_set_job_info)
{
    connect(w, SIGNAL(sig_rcvd_socket()),this,SLOT(rcvd_value()));

    init_display(this);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    m_qlw = new QListWidget(ui->listWidget);
    m_qlwi = new QListWidgetItem(m_qlw, QListWidgetItem::Type);

    connect(m_qlw, SIGNAL(clicked(QModelIndex)),this,SLOT(show_next_button()));

    m_qlw->resize(451,441);
    m_qlw->move(0,0);
    m_nLCnt = 0;
    cur_page=1;

    w->item_scanning();

    m_ButtonStat = 0;


    show_msg_2depth(OFF, ON);

    for(__u8 i = 0; i < 10; i++)
        jobGroup[i] = NULL;

    for (__u8 i = 0; i < 10; i++)
        jobGroup[i] = new QString();

    *jobGroup[0] = "1"; //replace to QStringList
    *jobGroup[1] = "2";
    *jobGroup[2] = "3";
    *jobGroup[3] = "4";
    *jobGroup[4] = "5";
    *jobGroup[5] = "6";
    *jobGroup[6] = "7";
    *jobGroup[7] = "8";
    *jobGroup[8] = "9";
    *jobGroup[9] = "10";
}

D_set_job_info::~D_set_job_info()
{
    delete m_qlw;
    delete ui;
    for (int i = 0; i < 10; i ++)
        delete jobGroup[i];
}

void D_set_job_info::on_b_close_clicked()
{
     m_qlw->clearSelection();
     save_process_all(jobGroup);
     this->close();
}

void D_set_job_info::save_process_all(QString **jobGroup)
{
    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,SPS_PATH);
    QSettings m_settings(".","ssf.ini");

    for (__u8 i = 1; i <= 10; i ++)
    {
        m_settings.beginGroup(*jobGroup[i - 1]);
        m_settings.setValue("item-name", parsedata[i -1].selected_item);
        m_settings.setValue("MC_PROCESS_NAME", parsedata[i -1].MC_PROCESS_NAME);
        delay_ms(10);
        parsedata[i-1].MC_SENDER_STYLE_CODE_LEFT.clear();
        parsedata[i-1].MC_SENDER_STYLE_CODE_RIGHT.clear();
        parsedata[i-1].MC_SENDER_STYLE_CODE_LEFT = parsedata[i -1].MC_STYLE_CODE.section("-",0,0);
        parsedata[i-1].MC_SENDER_STYLE_CODE_RIGHT = parsedata[i -1].MC_STYLE_CODE.section("-",1,1);
        parsedata[i-1].MC_SENDER_STYLE_CODE_RIGHT.insert(0,"");
        m_settings.setValue("MC_ITEM_NAME", parsedata[i -1].MC_ITEM_NAME);
        m_settings.setValue("style-full-name", parsedata[i -1].MC_STYLE_CODE);
        m_settings.setValue("style-name", parsedata[i-1].MC_SENDER_STYLE_CODE_LEFT);
        m_settings.setValue("style-code", parsedata[i-1].MC_SENDER_STYLE_CODE_RIGHT);
        m_settings.setValue("mc_item_cd", parsedata[i-1].MC_ITEM_CD);
        delay_ms(10);
        m_settings.setValue("trim_cnt", parsedata[i-1].trim_cnt);
        m_settings.setValue("stitch_cnt", parsedata[i-1].stitch_cnt);
        m_settings.setValue("mode", parsedata[i-1].modestat);
        m_settings.setValue("count", parsedata[i-1].count);
        m_settings.setValue("worker_id", parsedata[i-1].worker_id);
        delay_ms(10);
        if (!parsedata[i -1].target_cnt.contains("NONE"))
             m_settings.setValue("target_cnt",parsedata[i -1].target_cnt);
        m_settings.setValue("process_cnt",parsedata[i -1].process_cnt);
        m_settings.endGroup();
        delay_ms(10);
    }
}

void D_set_job_info::delay_ms(__u32 delay)
{
    for(__u32 i = 0; i < delay; i ++)
    {
         usleep(1000);
    }
}


#include <QDebug>   //protect overflow
void D_set_job_info::rcvd_value()
{
    if(w->socket_msg_box.contains("#I") && cur_page == 1)
    {
        w->socket_msg_box.replace("#I|","");

        QString rcvd_item_list;
        rcvd_item_list=w->socket_msg_box;

        QStringList item_name;
        int c=0;
        int n=0;
        int k=0;
        list_item.clear();
        list_processcd_item.clear();
        reference_list.clear();
        m_Cnt.clear();

        item_name = rcvd_item_list.split("|");

        while( item_name[c]!= NULL)
        {
            /*************protect overflow*************/
            if(item_name.count() - 1 > c)
            {
                list_item.insert(n,item_name[c]);
                list_processcd_item.insert(n,item_name[c+1]);
                reference_list.insert(item_name[c],n);
                c += 2;
                n++;
            }

            if( item_name.count() - 1 < c) break;
            /*************protect overflow*************/
        }


        for(int i=0;i<m_nLCnt;i++)
        {
            delete(m_qlw->takeItem(i));
        }
        m_qlw->clear();

        QFont myfont("sunfont");
        myfont.setPointSize(25);
        m_qlw->setFont(myfont);

        for(k=0;k<list_item.count();k++)
        {
            m_qlw->insertItem(m_nLCnt, list_item.value(k));
            m_qlw->setCurrentRow(m_nLCnt++);
        }
        m_qlw->clearSelection();

    }
    else if (w->socket_msg_box.contains("#J|") && cur_page == 2)
    {
        w->socket_msg_box.replace("#J|","");

        list_process.clear();
        list_style.clear();
        m_Cnt.clear();
        reference_code.clear();
        QString rcvd_prd_list;
        rcvd_prd_list=w->socket_msg_box;

        QStringList process_name;

        int c=0;
        int n=0;
        int k=0;

        qDebug()<<"2";
        process_name = rcvd_prd_list.split("|");

        while( process_name[c]!= NULL)
        {
            if (process_name.count() == 1)
            {
                qDebug() <<"The packet dont contains |";
                return ;
            }
            /*************protect overflow*************/
            if(process_name.count() - 1 > c )
            {
                list_process.insert(n,process_name[c]);
                list_style.insert(n,process_name[c+1]);
                m_Cnt.insert(n, process_name[c+2]);
                reference_code.insert(process_name[c],n);
                c += 3;
                n++;
            }

            if( process_name.count() - 1 < c) break;
            /*************protect overflow*************/
        }
        qDebug()<<"3";
        for(int i=0;i<m_nLCnt;i++)
        {
            delete(m_qlw->takeItem(i));
        }
        m_qlw->clear();
        qDebug()<<"4";
        QFont myfont("sunfont");
        myfont.setPointSize(25);
        m_qlw->setFont(myfont);
KR_ON;

        for(k=0;k<list_process.count(); k++)
        {
            m_qlw->insertItem(m_nLCnt, list_process.value(k));
            m_qlw->setCurrentRow(m_nLCnt++);
        }
KR_OFF;
        qDebug()<<"5";
        m_qlw->clearSelection();
    }
}

void D_set_job_info::init_display(D_set_job_info* const Class)
{
    ui->setupUi(Class);
    ui->b_scan->hide();
    ui->b_next->hide();
    KR_ON;
    ui->l_sub_title->setText("아이템 선택");
    ui->l_select_process_room->hide();
    ui->select_process_room->hide();
    KR_OFF;
}

void D_set_job_info::on_b_scan_clicked()
{
    w->item_scanning();
    ui->b_next->hide();
}

void D_set_job_info::show_next_button()
{
    ui->b_next->show();
}

void D_set_job_info::on_b_next_clicked()
{
    if(cur_page==1)
    {
        w->retry_j = 0;
        cur_page=2;
        selected_item=m_qlw->currentItem()->text();
        w->MC_ITEM_NAME = m_qlw->currentItem()->text();
        w->MC_ITEM_CD = list_processcd_item.value(reference_list.value(selected_item));
        w->process_scanning(w->MC_ITEM_CD);
        change_page(cur_page);
    }
    else if(cur_page==2)
    {
        QString tmp;
        w->display_item(&parsedata[w->public_laststat -1].MC_ITEM_NAME, &tmp);
        w->display_worker_id(parsedata[w->public_laststat -1].worker_id, &tmp);
        w->display_tirmstitch(&tmp, w->public_laststat);
        cur_page = 1;

KR_ON;
        qDebug()<<w->selected_process;
KR_OFF;        
        on_b_close_clicked();
    }  
}

void D_set_job_info::show_msg_2depth(bool is_on, __u8 button)
{
    if (is_on)
    {
        ui->l_item_title->show();
        ui->l_confirm_item->show();
        ui->l_confirm_item->setText(parsedata[button -1].MC_ITEM_NAME);
        ui->l_process_title->show();
        ui->l_confirm_process->show();
        ui->l_confirm_process->setText(parsedata[button -1].MC_PROCESS_NAME);
        ui->l_confirm_message1->show();
        ui->l_confirm_message2->show();
        ui->b_replace->show();
        ui->b_doNotReplace->show();
        ui->listWidget->hide();
    }
    else
    {
        ui->l_item_title->hide();
        ui->l_confirm_item->hide();
        ui->l_process_title->hide();
        ui->l_confirm_process->hide();
        ui->l_confirm_message1->hide();
        ui->l_confirm_message2->hide();
        ui->b_replace->hide();
        ui->b_doNotReplace->hide();
        ui->listWidget->show();
    }
}

void D_set_job_info::set_locale(bool turn_on)
{
    if(turn_on)
    {
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
        QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
        QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
    }else{
        QTextCodec::setCodecForTr(QTextCodec::codecForName("eucKR"));
        QTextCodec::setCodecForCStrings(QTextCodec::codecForName("eucKR"));
        QTextCodec::setCodecForLocale(QTextCodec::codecForName("eucKR"));
    }
}

void D_set_job_info::change_page(int page)
{
    KR_ON;
    if(page==1)
    {
        ui->b_next->setText("다음");
        ui->l_sub_title->setText("아이템 선택");
        ui->select_process_room->hide();
        for(int i=0;i<m_nLCnt;i++)
        {
            delete(m_qlw->takeItem(i));
        }
        m_qlw->clear();

    }
    else if(page==2){
        btn_no_exec = true; //kimkt(190201)
        ui->b_next->setText("선택 완료");
        ui->l_sub_title->setText("공정 선택");
        ui->b_next->hide();
        ui->l_select_process_room->show();
        ui->select_process_room->show();
        for(int i=0;i<m_nLCnt;i++)
        {
            delete(m_qlw->takeItem(i));
        }
        m_qlw->clear();
        btn_no_exec = false; //kimkt(190201)
    }
    KR_OFF;
}

void D_set_job_info::handle_button(int button)
{
    QString tmp;
    all_button_off();

    int nCnt = m_qlw->count();

    if (!nCnt)
    {
        qDebug()<<"invalid acess";
        button = 0;
    }

    if (button)
    {
        parsedata[button -1].process_cnt = 0;

        if (!parsedata[button -1].MC_PROCESS_NAME.contains("NONE"))
            show_msg_2depth(ON, button);
        else
        {
            getDB(m_qlw, m_Cnt, tmp, m_ButtonStat);

            parsedata[button -1].sec_sum = 0;
            parsedata[button -1].oper_sum = 0;

        }
        button_color(button);
    }
}

void D_set_job_info::all_button_off()
{
    ui->process1->setStyleSheet("border: 2px solid rgb(112,112,112); \nBorder-radius: 25px; color: rgb(112, 112, 112)");
    ui->process2->setStyleSheet("border: 2px solid rgb(112, 112, 112); \nBorder-radius: 25px; color: rgb(112, 112, 112)");
    ui->process3->setStyleSheet("border: 2px solid rgb(112, 112, 112); \nBorder-radius: 25px; color: rgb(112, 112, 112)");
    ui->process4->setStyleSheet("border: 2px solid rgb(112, 112, 112); \nBorder-radius: 25px; color: rgb(112, 112, 112)");
    ui->process5->setStyleSheet("border: 2px solid rgb(112, 112, 112); \nBorder-radius: 25px; color: rgb(112, 112, 112)");
    ui->process6->setStyleSheet("border: 2px solid rgb(112, 112, 112); \nBorder-radius: 25px; color: rgb(112, 112, 112)");
    ui->process7->setStyleSheet("border: 2px solid rgb(112, 112, 112); \nBorder-radius: 25px; color: rgb(112, 112, 112)");
    ui->process8->setStyleSheet("border: 2px solid rgb(112, 112, 112); \nBorder-radius: 25px; color: rgb(112, 112, 112)");
    ui->process9->setStyleSheet("border: 2px solid rgb(112, 112, 112); \nBorder-radius: 25px; color: rgb(112, 112, 112)");
    ui->process10->setStyleSheet("border: 2px solid rgb(112, 112, 112); \nBorder-radius: 25px; color: rgb(112, 112, 112)");
}

void D_set_job_info::button_color(__u8 button)
{
    switch(button)
    {
    case 1:
        if (!parsedata[button -1].MC_PROCESS_NAME.contains("NONE"))
            ui->process1->setStyleSheet("border: 2px solid rgb(255,0,0); \nBorder-radius: 25px; color: rgb(255, 0, 0)");
        else
            ui->process1->setStyleSheet("border: 2px solid rgb(0,0,255); \nBorder-radius: 25px; color: rgb(0, 0, 255)");
        break;
    case 2:
        if (!parsedata[button -1].MC_PROCESS_NAME.contains("NONE"))
            ui->process2->setStyleSheet("border: 2px solid rgb(255,0,0); \nBorder-radius: 25px; color: rgb(255, 0, 0)");
        else
            ui->process2->setStyleSheet("border: 2px solid rgb(0,0,255); \nBorder-radius: 25px; color: rgb(0, 0, 255)");
        break;
    case 3:
        if (!parsedata[button -1].MC_PROCESS_NAME.contains("NONE"))
            ui->process3->setStyleSheet("border: 2px solid rgb(255,0,0); \nBorder-radius: 25px; color: rgb(255, 0, 0)");
        else
            ui->process3->setStyleSheet("border: 2px solid rgb(0,0,255); \nBorder-radius: 25px; color: rgb(0, 0, 255)");
        break;
    case 4:
        if (!parsedata[button -1].MC_PROCESS_NAME.contains("NONE"))
            ui->process4->setStyleSheet("border: 2px solid rgb(255,0,0); \nBorder-radius: 25px; color: rgb(255, 0, 0)");
        else
            ui->process4->setStyleSheet("border: 2px solid rgb(0,0,255); \nBorder-radius: 25px; color: rgb(0, 0, 255)");
        break;
    case 5:
        if (!parsedata[button -1].MC_PROCESS_NAME.contains("NONE"))
            ui->process5->setStyleSheet("border: 2px solid rgb(255,0,0); \nBorder-radius: 25px; color: rgb(255, 0, 0)");
        else
            ui->process5->setStyleSheet("border: 2px solid rgb(0,0,255); \nBorder-radius: 25px; color: rgb(0, 0, 255)");
        break;
    case 6:
        if (!parsedata[button -1].MC_PROCESS_NAME.contains("NONE"))
            ui->process6->setStyleSheet("border: 2px solid rgb(255,0,0); \nBorder-radius: 25px; color: rgb(255, 0, 0)");
        else
            ui->process6->setStyleSheet("border: 2px solid rgb(0,0,255); \nBorder-radius: 25px; color: rgb(0, 0, 255)");
        break;
    case 7:
        if (!parsedata[button -1].MC_PROCESS_NAME.contains("NONE"))
            ui->process7->setStyleSheet("border: 2px solid rgb(255,0,0); \nBorder-radius: 25px; color: rgb(255, 0, 0)");
        else
            ui->process7->setStyleSheet("border: 2px solid rgb(0,0,255); \nBorder-radius: 25px; color: rgb(0, 0, 255)");
        break;
    case 8:
        if (!parsedata[button -1].MC_PROCESS_NAME.contains("NONE"))
            ui->process8->setStyleSheet("border: 2px solid rgb(255,0,0); \nBorder-radius: 25px; color: rgb(255, 0, 0)");
        else
            ui->process8->setStyleSheet("border: 2px solid rgb(0,0,255); \nBorder-radius: 25px; color: rgb(0, 0, 255)");
        break;
    case 9:
        if (!parsedata[button -1].MC_PROCESS_NAME.contains("NONE"))
            ui->process9->setStyleSheet("border: 2px solid rgb(255,0,0); \nBorder-radius: 25px; color: rgb(255, 0, 0)");
        else
            ui->process9->setStyleSheet("border: 2px solid rgb(0,0,255); \nBorder-radius: 25px; color: rgb(0, 0, 255)");
        break;
    case 10:
        if (!parsedata[button -1].MC_PROCESS_NAME.contains("NONE"))
            ui->process10->setStyleSheet("border: 2px solid rgb(255,0,0); \nBorder-radius: 25px; color: rgb(255, 0, 0)");
        else
            ui->process10->setStyleSheet("border: 2px solid rgb(0,0,255); \nBorder-radius: 25px; color: rgb(0, 0, 255)");
        break;
    default:
        break;
    }
}

QString D_set_job_info::getDB(QListWidget* m_qlw, QHash<int, QString> m_Cnt, QString tmp,__u8 button)
{
    if(!button)
        return parsedata[button -1].MC_PROCESS_NAME;

    selected_process=m_qlw->currentItem()->text();
    parsedata[button -1].MC_ITEM_NAME = w->MC_ITEM_NAME;
    parsedata[button -1].MC_PROCESS_NAME = selected_process;
    parsedata[button -1].MC_STYLE_CODE = list_style.value(reference_code.value(selected_process));
    tmp = m_Cnt.value(reference_code.value(selected_process));

    if (tmp.contains("H"))
    {
        tmp.replace(0, 1,"");
        parsedata[button -1].modestat = 1;
        parsedata[button -1].count = tmp.toInt();
    }
    else
    {
        tmp.replace(0, 1,"");
        parsedata[button -1].modestat = 2;
        parsedata[button -1].count = tmp.toInt();
    }

    return parsedata[button -1].MC_PROCESS_NAME;
}

void D_set_job_info::on_process1_clicked()
{
    if(btn_no_exec)  return;    //kimkt(190201)
    if(m_ButtonStat == 1)
    {
        m_ButtonStat = 0;
    }
    else
         m_ButtonStat = 1;

    handle_button(m_ButtonStat);
}

void D_set_job_info::on_process2_clicked()
{
    if(btn_no_exec)  return;    //kimkt(190201)
    if(m_ButtonStat == 2)
    {
        m_ButtonStat = 0;
    }
    else
         m_ButtonStat = 2;

    handle_button(m_ButtonStat);
}

void D_set_job_info::on_process3_clicked()
{
    if(btn_no_exec)  return;    //kimkt(190201)
    if(m_ButtonStat == 3)
    {
        m_ButtonStat = 0;
    }
    else
         m_ButtonStat = 3;

    handle_button(m_ButtonStat);
}

void D_set_job_info::on_process4_clicked()
{
    if(btn_no_exec)  return;    //kimkt(190201)
    if(m_ButtonStat == 4)
    {
        m_ButtonStat = 0;
    }
    else
         m_ButtonStat = 4;

    handle_button(m_ButtonStat);
}

void D_set_job_info::on_process5_clicked()
{
    if(btn_no_exec)  return;    //kimkt(190201)
    if(m_ButtonStat == 5)
    {
        m_ButtonStat = 0;
    }
    else
         m_ButtonStat = 5;

    handle_button(m_ButtonStat);
}

void D_set_job_info::on_process6_clicked()
{
    if(btn_no_exec)  return;    //kimkt(190201)
    if(m_ButtonStat == 6)
    {
        m_ButtonStat = 0;
    }
    else
         m_ButtonStat = 6;

    handle_button(m_ButtonStat);
}

void D_set_job_info::on_process7_clicked()
{
    if(btn_no_exec)  return;    //kimkt(190201)
    if(m_ButtonStat == 7)
    {
        m_ButtonStat = 0;
    }
    else
         m_ButtonStat = 7;

    handle_button(m_ButtonStat);
}

void D_set_job_info::on_process8_clicked()
{
    if(btn_no_exec)  return;    //kimkt(190201)
    if(m_ButtonStat == 8)
    {
        m_ButtonStat = 0;
    }
    else
         m_ButtonStat = 8;

    handle_button(m_ButtonStat);
}

void D_set_job_info::on_process9_clicked()
{
    if(btn_no_exec)  return;    //kimkt(190201)
    if(m_ButtonStat == 9)
    {
        m_ButtonStat = 0;
    }
    else
         m_ButtonStat = 9;

    handle_button(m_ButtonStat);
}

void D_set_job_info::on_process10_clicked()
{
    if(btn_no_exec)  return;    //kimkt(190201)
    if(m_ButtonStat == 10)
    {
        m_ButtonStat = 0;
    }
    else
         m_ButtonStat = 10;

    handle_button(m_ButtonStat);
}

void D_set_job_info::on_b_replace_clicked()
{
    QString tmp;
    getDB(m_qlw, m_Cnt, tmp, m_ButtonStat);
    parsedata[m_ButtonStat -1].sec_sum = 0;
    parsedata[m_ButtonStat -1].oper_sum = 0;
    show_msg_2depth(OFF, ON);
    qDebug() << "get a DB";
}

void D_set_job_info::on_b_doNotReplace_clicked()
{
    show_msg_2depth(OFF, ON);
    qDebug() << "do not get DB";
}

void D_set_job_info::on_b_scan_2_clicked()
{
    if (cur_page == 1)
    {
        w->item_scanning();
        ui->b_next->hide();
    }
    else if (cur_page == 2)
    {
        w->process_scanning(w->MC_ITEM_CD);
        ui->b_next->hide();
    }
}
