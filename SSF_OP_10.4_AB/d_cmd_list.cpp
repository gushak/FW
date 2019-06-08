#include "d_cmd_list.h"
#include "ui_d_cmd_list.h"
#include <QGraphicsOpacityEffect>
#include <QProcess>

#if 0 //sql
#include <QtSql/QtSql>
#endif

extern MainWindow *w;
//QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");

D_cmd_list::D_cmd_list(QWidget *parent):
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::D_cmd_list)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);
#if 0 //alpha effect
    QGraphicsOpacityEffect * effect = new QGraphicsOpacityEffect();
    effect->setOpacity(0.9);
    ui->l_bg->setGraphicsEffect(effect);
#endif

    selected_command.clear();

    m_qlw = new QListWidget(ui->listWidget);

    //connect(ui->listWidget,QListWidget::itemClicked(QListWidgetItem * item),this,item_selected(QListWidgetItem * item));
    //connect(ui->listWidget,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(item_selected(QListWidgetItem*)));
    //connect(notRsRead, SIGNAL(activated(int)),this, SLOT(edge_input()));

    //const QModelIndex &index
    //connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(cmd_selected(QListWidgetItem*)));
    connect(m_qlw, SIGNAL(clicked(const QModelIndex &)), this, SLOT(cmd_selected(const QModelIndex &)));
    //}

    //    void MyClass::onListMailItemClicked(QListWidgetItem* item)
    //    {
    //        if (ui->listMail->item(0) == item) {
    //            // This is the first item.
    //        }
    //    }

    m_qlw->resize(741,341);
    m_qlw->move(0,0);

    m_nLCnt = 0;

    list_mode();
    ui->b_dataview->hide();
    on_b_refresh_clicked();
}

D_cmd_list::~D_cmd_list()
{
    delete ui;
}


void D_cmd_list::on_b_close_clicked()
{
    //delete(m_qlwi);
    this->close();
}
#if 0

void D_cmd_list::cmd_selected(const QModelIndex &index)
{

    selected_command=index.data().toString();
}

void D_cmd_list::list_view()
{
    int num=0;
    //qDebug()<<"try connect";
    list_design.clear();
    m_nLCnt=0;

    try_connect_db();

    QSqlQuery qry;

    qry.prepare( "SELECT PLAN_NUM FROM JOB_COMMAND" );
    if( !qry.exec() )
        qDebug() << qry.lastError();
    else
    {
        //qDebug() <<  "Selected!" ;

        QSqlRecord rec = qry.record();
        int cols = rec.count();

        QString temp;
        //        for( int c=0; c<cols; c++ )
        //            temp += rec.fieldName(c) + ((c<cols-1)?"\t":"");
        //        list_design.insert(num,temp);
        //        num++;
        //        qDebug() << "cols:" <<temp;

        while( qry.next() )
        {
            temp = "";
            for( int c=0; c<cols; c++ )
            {
                temp += qry.value(c).toString();// + ((c<cols-1)?"\t":"");
            }
            list_design.insert(num,temp);
            num++;
        }
    }
    qDebug()<<list_design;

    write_command_list();
    //ui->b_connect->show();
    db.close();

}

void D_cmd_list::write_command_list()
{
    int k;
    m_qlwi = new QListWidgetItem(m_qlw, QListWidgetItem::Type);

    for(int i=0;i<m_nLCnt;i++)
    {
        delete(m_qlw->takeItem(i));
    }
    m_qlw->clear();


    m_nLCnt=0;

    QFont myfont("sunfont");
    myfont.setPointSize(25);
    m_qlw->setFont(myfont);
    // m_qlw->setLocale("utf-8");
    //m_qlw->setLocale("utf-8");

    for(k=0;k<list_design.count();k++)
    {
        m_qlw->insertItem(m_nLCnt, list_design.value(k));
        m_qlw->setCurrentRow(m_nLCnt++);
    }


    if(list_design.count()<1)
    {
        ui->b_dataview->hide();
    }else{
        ui->b_dataview->show();
    }

    m_qlw->clearSelection();





}
void D_cmd_list::try_connect_db()
{
    db.removeDatabase("QSQLITE");
    //qDebug() << QSqlDatabase::drivers();
#if __arm__
    db.setHostName(w->srv_ip);
#else
    db.setHostName( "127.0.0.1" );
#endif
    db.setDatabaseName("rsm_counter");

    db.setUserName( "root" );
    db.setPassword( "rsmglobal" );

    if( !db.open() )
    {
        qDebug() << db.lastError();
        qFatal( "Failed to connect." );
    }

    //qDebug( "Connected!" );
}

void D_cmd_list::view_mode()
{
    ui->grp_list->hide();
    ui->grp_view->show();
}
void D_cmd_list::list_mode()
{
    ui->grp_list->show();
    ui->grp_view->hide();
}

void D_cmd_list::on_b_refresh_clicked()
{
    selected_command.clear();
    m_qlw->clear();
    list_view();
}

void D_cmd_list::on_b_dataview_clicked()
{
    //
    //qDebug()<<ui->listWidget->currentItem();
    //qDebug()<<ui->listWidget->currentIndex();
    //qDebug()<<m_qlw->currentRow();

    if(selected_command==NULL)
    {
        qDebug()<<"please, item select";
        QMessageBox::NoIcon;
        QMessageBox::information(this, "안내", "작업지시서를 선택해주세요.");

    }else{
        //qDebug()<<selected_command;
        view_mode();
        write_command(selected_command);

    }


}
void D_cmd_list::write_command(QString name)
{
    //int num=0;
    list_design.clear();
    m_nLCnt=0;

    try_connect_db();

    QSqlQuery qry;
/*
| PLAN_NUM     | char(20) | YES  |     | NULL    |                |
| PRODUCT_NAME | char(20) | YES  |     | NULL    |                |
| JOB_NAME     | char(20) | YES  |     | NULL    |                |
| JOB_DATE     | int(11)  | YES  |     | NULL    |                |
| TARGET_CNT   | int(11)  | YES  |     | NULL    |                |
| JOB_PERSON   | char(20) | YES  |     | NULL    |                |
| Text         | text     | YES  |     | NULL    |                |
| seq          | int(11)  | NO   | PRI | NULL    | auto_increment |

*/
    QString custom_command;
    custom_command="SELECT PLAN_NUM,PRODUCT_NAME,JOB_NAME,JOB_DATE,TARGET_CNT,JOB_PERSON,Text FROM JOB_COMMAND WHERE PLAN_NUM = \"";
    custom_command.append(name);
    custom_command.append("\"");

    //qry.prepare("SELECT PLAN_NUM,PRODUCT_NAME,JOB_NAME,JOB_DATE,TARGET_CNT,JOB_PERSON,Text FROM JOB_COMMAND WHERE PLAN_NUM = ");

    //qDebug()<<"query->"<<custom_command;

    qry.prepare("set names utf8");
    qry.exec();




    qry.prepare(custom_command);

    if( !qry.exec() )
        qDebug() << qry.lastError();
    else
    {
        //qDebug() <<  "Selected!" ;

        QSqlRecord rec = qry.record();
        int cols = rec.count();

        QString temp,temp1,temp2;
        //        for( int c=0; c<cols; c++ )
        //            temp += rec.fieldName(c) + ((c<cols-1)?"\t":"");
        //        list_design.insert(num,temp);
        //        num++;
        //        qDebug() << "cols:" <<temp;
//QString::fromUtf8
        //    temp1=QString::fromUtf8("연결 안 됨");

        while( qry.next() )
        {
            temp = "";
            for( int c=0; c<cols; c++ )
            {
                temp1=qry.value(c).toString();

                qDebug()<<"-> "<<qry.value(c).toString();
            //    qDebug()<<temp1.codecForCStrings;


                //qDebug()<<"hangule!!! ->"<<qry.value(c).toString();
                //qDebug()<<"hangule!!! ->"<<temp1;

                QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
                QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
                QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));


                QTextCodec * codec = QTextCodec::codecForName("UTF-8");
                QString localeStr = codec->toUnicode(temp1.toLocal8Bit());
                //qDebug()<<localeStr;
                //qDebug()<<temp1.toUtf8();
                //temp2=temp1.toAscii();
                //qDebug()<<QString::fromLocal8Bit(temp1.toLocal8Bit());


                //this->ui->lineEdit->setText(QString::fromLocal8Bit("한글 메시지"));



                //temp += qry.value(c).toString() + ((c<cols-1)?"\t":"");
                job_instruction.insert(c, qry.value(c).toString());
            }
            //list_design.insert(num,temp);
            //qDebug()<<job_instruction;
            //num++;
        }

        ui->a_1->setText(job_instruction.value(0));
        ui->a_2->setText(job_instruction.value(1));
        ui->a_3->setText(job_instruction.value(2));
        ui->a_4->setText(job_instruction.value(3));
        ui->a_5->setText(job_instruction.value(4));
        ui->a_6->setText(job_instruction.value(5));
        ui->a_7->setText(job_instruction.value(6));
    }
    //qDebug()<<list_design;

    db.close();

}


 #if 0

QString MyClass::test()

{

    sLabel = new QLabel(toUniString(tr("큐티코리아 만세!")));

}
#endif
void D_cmd_list::on_pushButton_clicked()
{
    list_mode();
}

#endif
































