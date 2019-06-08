#include "d_init.h"
#include "ui_d_init.h"
#include <QGraphicsOpacityEffect>

extern MainWindow *w;

D_init::D_init(QWidget *parent):
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::D_init)
{
    ui->setupUi(this);

#if 1 //background alpha value
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint);

    QGraphicsOpacityEffect * effect = new QGraphicsOpacityEffect();
    effect->setOpacity(0.9);
    ui->l_bg->setGraphicsEffect(effect);
#endif

    ui->l_err_content->hide();

#if 0
    QGraphicsOpacityEffect* opa = new QGraphicsOpaictyEffect();
    sub_widget->setGraphicsEffect(opa);
    opa->setOpaicty(0.5);

    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::FramelessWindowHint);


    QGraphicsOpacityEffect * effect = new QGraphicsOpacityEffect(ui->l_bg);
    effect->setOpacity(90);
    ui->l_bg->setGraphicsEffect(effect);

    //this->setAttribute(Qt::WA_TranslucentBackground);

    //this->setWindowOpacity(0.1);
    //ui->l_bg->setStyleSheet("background-color: rgba(255, 255, 255, 10);");
    //ui->l_bg->setStyleSheet("background-color: rgba(20, 0, 0, 70%);");
    //ui->l_bg->setWindowOpacity(0.5);
#endif
}

D_init::~D_init()
{
    delete ui;
}


void D_init::on_b_close_clicked()
{
    this->close();
}



void D_init::on_b_yes_clicked()
{
    w->actual_cnt=0;
    this->close();
}

void D_init::on_b_no_clicked()
{

    this->close();
}
