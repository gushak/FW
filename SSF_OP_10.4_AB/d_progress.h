#ifndef D_PROGRESS_H
#define D_PROGRESS_H

#include <QDialog>
#include "mainwindow.h"     //(120916)


namespace Ui {
    class D_progress;
}

class D_progress : public QDialog
{
    Q_OBJECT

public:
    explicit D_progress(QWidget *parent = 0);
    ~D_progress();


private slots:


    void on_b_close_clicked();

    void on_b_list_load_clicked();

    void on_b_save_clicked();

private:
    Ui::D_progress *ui;
};

#endif // D_PROGRESS_H
