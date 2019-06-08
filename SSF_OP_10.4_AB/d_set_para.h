#ifndef D_SET_PARA_H
#define D_SET_PARA_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
    class D_set_para;
}

class D_set_para : public QDialog
{
    Q_OBJECT

public:
    explicit D_set_para(QWidget *parent = 0);
    ~D_set_para();

private slots:
    void on_b_close_clicked();
    void do_keypad_clicked(int);
    void on_b_enter_clicked();

private:
    Ui::D_set_para *ui;
    QButtonGroup *keypad;
    QString tmp,comment;

    void memory_in_target(QString **Group);
    void memory_in_gain(QString **Group);
    void memory_in_stitch_to_trim(QString **Group);
    void memory_in_interval(QString **Group);

    QString *paraGroup[11];
    void add_key();
    int dialog_mode;
};

#endif // D_SET_PARA_H

