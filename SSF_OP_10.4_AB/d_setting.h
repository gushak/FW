#ifndef D_SETTING_H
#define D_SETTING_H

#include <QDialog>
#include "mainwindow.h"




namespace Ui {
    class D_setting;
}

class D_setting : public QDialog
{
    Q_OBJECT

public:
    explicit D_setting(QWidget *parent = 0);
    ~D_setting();
    int m_nLCnt;
    void set_static_ip_boot();
    void set_static_ip();


private slots:


    void on_b_close_clicked();
    void on_b_SCANNING_clicked();
    void do_keypad_clicked(int);
    void do_pw_keypad_clicked(int);

    void on_b_ipsetting_clicked();
    void on_b_wifi_setting_clicked();
    void on_b_static_ip_clicked();
    void on_b_dhcp_ip_clicked();
    void on_b_connect_clicked();

private:
    Ui::D_setting *ui;
    QHash<int, QString> list_SSID;

    QButtonGroup *keypad;
    QButtonGroup *pw_keypad;


    QListWidget* m_qlw;
    QListWidgetItem* m_qlwi;

    bool wwpa_flag;
    void try_wwpa_connect();
    void set_dhcp_ip();

    void set_wpa_pw();

};

#endif // D_SETTING_H




















#if 0
#ifndef D_SETTING_H
#define D_SETTING_H

#include <QDialog>
#include "mainwindow.h"




namespace Ui {
    class D_setting;
}

class D_setting : public QDialog
{
    Q_OBJECT

public:
    explicit D_setting(QWidget *parent = 0);
    ~D_setting();



private slots:


    void readReady(QNetworkReply *reply);

    void Request(QString requestStr);

    void on_b_close_clicked();
    void on_b_SCANNING_clicked();

private:
    Ui::D_setting *ui;

    QNetworkAccessManager *networkManager;


};

#endif // D_SETTING_H

#endif
