#include "d_progress.h"
#include "ui_d_progress.h"

extern MainWindow *w;

D_progress::D_progress(QWidget *parent) :
	QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::D_progress)
{
	ui->setupUi(this);
    this->setAttribute(Qt::WA_TranslucentBackground);
}

D_progress::~D_progress()
{
	delete ui;
}


void D_progress::on_b_close_clicked()
{
        this->close();
}


void D_progress::on_b_list_load_clicked()
{

}

void D_progress::on_b_save_clicked()
{

}
