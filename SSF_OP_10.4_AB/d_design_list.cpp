#include "d_design_list.h"
#include "ui_d_design_list.h"
#include <QGraphicsOpacityEffect>
#include <QProcess>

#if 1 //sql
#include <QtSql/QtSql>
#endif

extern MainWindow *w;

D_design_list::D_design_list(QWidget *parent):
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::D_design_list)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);
#if 0 //alpha effect
    QGraphicsOpacityEffect * effect = new QGraphicsOpacityEffect();
    effect->setOpacity(0.9);
    ui->l_bg->setGraphicsEffect(effect);
#endif

#ifdef __arm__
    system("umount /mnt");
    usb_mount();
    //system("mount /dev/sda1 /mnt");
    //    system("sync");
    //    system("sync");
    //    system("sync");

#else
    usb_mount();

#endif
    m_qlw = new QListWidget(ui->listWidget);

    m_qlw->resize(321,341);
    m_qlw->move(0,0);
    m_nLCnt = 0;
    ui->ssid_progressBar->hide();
    ui->ssid_loading->hide();
    ui->b_connect->hide();
    //    QSettings::setPath(QSettings::NativeFormat,QSettings::UserScope,"/app/data/");
    //    QSettings m_settings(".","ssf.ini");

    on_b_SCANNING_clicked();
    storage_init();
}

D_design_list::~D_design_list()
{
    delete ui;
}


void D_design_list::on_b_close_clicked()
{
    //delete(m_qlwi);
    delete(m_qlw);
    this->close();
}

#if 0
void D_design_list::on_b_SCANNING_clicked()
{
    int num=0;
    //qDebug()<<"try connect";
    list_design.clear();
    m_nLCnt=0;

    try_connect_db();

    QSqlQuery qry;

    qry.prepare( "SELECT name FROM upload" );
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
                temp += qry.value(c).toString() + ((c<cols-1)?"\t":"");
            }
            list_design.insert(num,temp);
            num++;
        }
    }
    qDebug()<<list_design;
    write_design_list();
    ui->b_connect->show();

}


void D_design_list::try_connect_db()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.removeDatabase("QSQLITE");
    qDebug() << QSqlDatabase::drivers();
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

void D_design_list::write_design_list()
{
#if 1
    int k;
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

    ui->ssid_progressBar->setValue(50);
    ui->ssid_progressBar->setValue(70);

    m_nLCnt=0;

    ui->ssid_progressBar->setValue(80);

    QFont myfont("sunfont");
    myfont.setPointSize(25);
    m_qlw->setFont(myfont);

    for(k=0;k<list_design.count();k++)
    {
        m_qlw->insertItem(m_nLCnt, list_design.value(k));
        m_qlw->setCurrentRow(m_nLCnt++);
    }


    ui->ssid_progressBar->setValue(100);

    ui->b_SCANNING->setEnabled(true);

    ui->ssid_loading->hide();
    ui->ssid_progressBar->hide();

#endif
}

void D_design_list::storage_init()
{

    model = new QDirModel(this);

    connect(ui->localView,SIGNAL(clicked(const QModelIndex &)),this, SLOT(showFileDetails(const QModelIndex &)));

    model->setReadOnly(true);
    model->setSorting(QDir::DirsFirst|QDir::IgnoreCase|QDir::Name);
    model->setFilter(QDir::Dirs|QDir::NoDotAndDotDot);

    ui->localView->setModel(model);
    ui->localView->setRootIndex(model->index("/mnt", 0));
    ui->localView->setColumnHidden(1,true);//size
    ui->localView->setColumnHidden(2,true);//type
    ui->localView->setColumnHidden(3,true);//date
}
void D_design_list::showFileDetails(const QModelIndex &index)
{

    fullpath(index);
    qDebug()<<"path->"<<full_path;

}

void D_design_list::fullpath(const QModelIndex &index)
{
    //QString path('/');
    QString path;//('/');
    QModelIndex parent = index;
    QString check_file;
    check_file=index.data().toString();

    qDebug()<<index.data();

    if(check_file.contains("."))
    {
        qDebug()<<"this is file";
    }else{
        while(parent.isValid())
        {
            path.prepend('/' + parent.data().toString());
            parent = parent.parent();
        }
        qDebug()<<"this is folder";
    }
    //return path;
    path.replace(0,2,"");
    full_path=path;
}



void D_design_list::on_b_connect_clicked()
{
    // ui->ssid_loading->show();
    ui->ssid_progressBar->show();
    ui->ssid_progressBar->setValue(0);
    QStringList arguments,arguments1;
    QProcess getProcess;

    //system("cd /app/data");

    QString command,command1,command2,tmp;
    command="cd /app/data && /usr/bin/tftp " + w->srv_ip + " -r " + m_qlw->currentItem()->text() + " -g";
    command.replace("\"","");

    command1 = "ls /app/data | grep -c " +m_qlw->currentItem()->text();
    command1.replace("\"","");

    if(full_path==NULL)
    {
        command2 = "mv /app/data/"+m_qlw->currentItem()->text()+" /mnt";
        command2.replace("\"","");
    }else{

        command2 = "mv /app/data/"+m_qlw->currentItem()->text()+" "+full_path;
        command2.replace("\"","");
    }
    ui->ssid_progressBar->setValue(30);
    qDebug()<<"move command "<<command2;

#ifdef __arm__
    //system("cd /app/data");
    //system("sync");
    ui->ssid_progressBar->setValue(40);


    system(command.toLocal8Bit());
    //system(command.toLatin1());
    ui->ssid_progressBar->setValue(50);

    system("sync");
    system("sync");
    //arguments1 << "-c" << command;
    //arguments1 << "-c" << "tftp" <<w->srv_ip<<" -r "<<m_qlw->currentItem()->text()<<" -g";
    //getProcess.start("sh" ,arguments1);
    //sleep(1);
    //    getProcess.waitForFinished();
    //    QString tmp = getProcess.readAll();
    //    tmp.remove(QRegExp("[\\n\\t\\r]"));
    //    tmp.replace("\"","");
    //    qDebug()<<"result:"<<arguments1;
    //sleep(1);

#if 0

    arguments<< "tftp";
    arguments<< "-i";
    arguments<< "192.168.2.1";
    arguments<< "get";
    arguments<< "configINPUT.txt";
    process.setWorkingDirectory(temp_path);
    process.start(program,arguments);
    qDebug()<<arguments;
    process.waitForFinished();

#endif
    ui->ssid_progressBar->setValue(60);

    system("sync");
    system("sync");
    system("sync");
    system("sync");
    ui->ssid_progressBar->setValue(70);
#if 1
    arguments << "-c" << command1;

    ui->ssid_progressBar->setValue(75);
    getProcess.start("sh" ,arguments);
    sleep(3);
    getProcess.waitForFinished();
    tmp = getProcess.readAll();
    tmp.remove(QRegExp("[\\n\\t\\r]"));
    tmp.replace("\"","");
    ui->ssid_progressBar->setValue(80);

    if(tmp.toInt()>=1)
    {
        qDebug()<<"down load ok"<<tmp;

        qDebug()<<"file moving start";

        system(command2.toLocal8Bit());
        system("sync");
        system("sync");
        system("sync");

        ui->ssid_progressBar->setValue(100);


#if 0
        QMessageBox about_box(this);
        //QMessageBox::NoIcon;
        //about_box.QMessageBox::windowTitle()
        about_box.setWindowFlags(Qt::FramelessWindowHint);//|QMessageBox::NoIcon);
        about_box.setStyleSheet("border: 1px solid rgb(112, 112, 112); Border-radius: 25px;Border-color: rgb(255, 255, 255);background: grey;");
        //about_box.setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint|QMessageBox::NoIcon);
        about_box.QMessageBox::NoIcon;
        about_box.setText("DOWNLOAD SUCCESS");

        // about_box.setIconPixmap(":/new_images/bg.png");
        about_box.setParent(this);

        about_box.exec();
#else
        QMessageBox::NoIcon;
        QMessageBox::information(this, "", "DOWNLOAD SUCCESS");



        //QMessageBox::setWindowFlags(this,Qt::FramelessWindowHint);
        //  w->setWindowFlags(Qt::FramelessWindowHint);



#endif
        //        Here is my resource file:

        //        <RCC>
        //            <qresource prefix="/images">
        //                <file>logo.png</file>
        //            </qresource>
        //        </RCC>



        //QMessageBox::information(this, "info", "DOWNLOAD SUCCESS");



        //storage_init();

        //showFileDetails(full_path.toStdString());
        //ui->localView->setLocale(full_path);
        //ui->localView->setCurrentIndex(full_path);
        //ui->localView->setRootIndex(model->index(full_path, 0));
    }else{
        qDebug()<<"down load fail";
    }
#endif
    ui->ssid_progressBar->hide();
#endif

}


void D_design_list::usb_mount()
{
    QString mount_name;
    QStringList arguments;
    QProcess process;
    QString scan_usb_cmd;
    QString command;
    //cat /proc/partitions | grep sd | awk '{print $4}'|grep 1

    scan_usb_cmd = "cat /proc/partitions | grep sd | awk '{print $4}'|grep 1";

    arguments << "-c" << scan_usb_cmd;
    process.start("sh" ,arguments);
    process.waitForFinished(3000);

    QString result;
    result=process.readAllStandardOutput();

    result.simplified();


    if(result.toLatin1()==NULL) //passwd on
    {
        qDebug()<<"usb is not visible";
        ui->localView->setEnabled(false);
        ui->b_usb_disconnect->hide();

    }else{
        ui->localView->setEnabled(true);

        ui->b_usb_disconnect->show();
        mount_name.clear();
        mount_name.append(result.at(0));
        mount_name.append(result.at(1));
        mount_name.append(result.at(2));
        mount_name.append(result.at(3));
        qDebug()<<"usb mounted :"<<mount_name;
        command = "mount /dev/" + mount_name + " /mnt";
        //command.append()
        system(command.toLatin1());
    }
}

void D_design_list::on_b_usb_disconnect_clicked()
{
    qDebug()<<system("umount /mnt");

    ui->b_usb_disconnect->hide();
    storage_init();

    ui->listWidget->setEnabled(false);
    ui->localView->setEnabled(false);


}
#endif
