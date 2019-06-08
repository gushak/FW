#include "d_ftp_list.h"
#include "ui_d_ftp_list.h"
#include <QTextCodec>

extern MainWindow *w;

D_ftp_list::D_ftp_list(QWidget *parent):
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::D_ftp_list)
{
    ui->setupUi(this);


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

D_ftp_list::~D_ftp_list()
{
    delete ui;
}


void D_ftp_list::on_b_close_clicked()
{
    this->close();
}



void D_ftp_list::on_b_ok_clicked()
{
    qDebug()<<full_path;
}

void D_ftp_list::showFileDetails(const QModelIndex &index)
{

    fullpath(index);
    qDebug()<<"path->"<<full_path;

}

void D_ftp_list::fullpath(const QModelIndex &index)
{
    //QString path('/');
    QString path;//('/');
    QModelIndex parent = index;
    while(parent.isValid())
    {
        path.prepend('/' + parent.data().toString());
        parent = parent.parent();
    }
    //return path;
    path.replace(0,2,"");
    full_path=path;
}





















