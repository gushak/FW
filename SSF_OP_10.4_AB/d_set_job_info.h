#ifndef D_SET_JOB_INFO_H
#define D_SET_JOB_INFO_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
    class D_set_job_info;
}

struct Work
{
    QString MC_PROCESS_NAME;
    QString selected_item;
    QString MC_SENDER_STYLE_CODE_LEFT;
    QString MC_SENDER_STYLE_CODE_RIGHT;
    QString MC_ITEM_CD;
    QString MC_STYLE_CODE;
    QString workername;
    QString MC_CODE;
    QString worker_id;
    QString MC_ITEM_NAME;
    QString target_cnt;
    int modestat;
    __u16 trim_cnt;
    __u16 stitch_cnt;
    __u16 count;
    int process_cnt;
    __u16 gain;
    __u8 stitch_to_trim;
    __u8 interval;
    __u32 sec_sum;
    __u32 oper_sum;
    __u32 stitch_sum;
};

enum index
{
    BUFFERINDEX = 0,
    ERR = 0,
};

enum is_on
{
    OFF = 0,
    ON = 1,
};

class D_set_job_info : public QDialog, public QPalette
{
    Q_OBJECT

public:
    explicit D_set_job_info(QWidget *parent = 0);
    ~D_set_job_info();
private slots:
    void on_b_close_clicked();
    void rcvd_value();
    void on_b_scan_clicked();
    void on_b_next_clicked();
    void show_next_button();
    void on_process1_clicked();
    void on_process2_clicked();
    void on_process3_clicked();
    void on_process4_clicked();
    void on_process5_clicked();
    void on_process6_clicked();
    void on_process7_clicked();
    void on_process8_clicked();
    void on_process9_clicked();
    void on_process10_clicked();
    void on_b_replace_clicked();
    void on_b_doNotReplace_clicked();

    void on_b_scan_2_clicked();

private:
    Ui::D_set_job_info *ui;

    QButtonGroup *process_button;

    void init_display(D_set_job_info* const Class);
    void show_msg_2depth(bool is_on, __u8 button);
    void change_page(int);
    void set_process_num();
    int m_nLCnt;
    int cur_page;
    int m_GroupIndex;

    void all_button_off();
    void button_color(__u8 button);
    void handle_button(int button);

    void save_process_all(QString **jobGroup);

    void delay_ms(__u32 delay);

    QString *jobGroup[11];

    void set_locale(bool turn_on);

    QHash<int, QString> list_item;
    QHash<QString, int> reference_list;
    QHash<int, QString> list_processcd_item;

    QHash<int, QString> list_process;
    QHash<QString, int> reference_code;
    QHash<int, QString> list_style;
    QHash<int, QString> m_Cnt;

    QListWidget* m_qlw;
    QListWidgetItem* m_qlwi;

    QString getDB(QListWidget* m_qlw, QHash<int, QString> m_Cnt, QString tmp, __u8 button);

    QString selected_item;
    QString selected_process;

    int m_ButtonStat;
    bool btn_no_exec;  //kimkt(190201)
};
extern Work parsedata[12];
extern QString buffer[BUFFERINDEX + 1];
#endif // D_set_info_H

