#include "d_error.h"
#include "ui_d_error.h"



extern MainWindow *w;       //kimkt(120916)



D_error::D_error(QWidget *parent) :
	QDialog(parent, Qt::FramelessWindowHint),
	ui(new Ui::D_error)
{
	ui->setupUi(this);
	this->setAttribute(Qt::WA_TranslucentBackground);
	//connect(w, SIGNAL(sig_error_esc(int, int)), this, SLOT(do_error_esc(int, int)));  //kimkt(120916)
    ui->l_title->setFocus();     //ui->l_icon->setFocus();





//    m_camera = new QCamera("primary");
//    m_camera->setViewfinder(ui->viewFinder);
//    m_imageCapture = new QCameraImageCapture(m_camera);
//    QImageEncoderSettings encoder_setting = m_imageCapture->encodingSettings();
//    encoder_setting.setCodec("image/jpeg");
//    encoder_setting.setQuality(QtMultimediaKit::NormalQuality);
//    encoder_setting.setResolution(800,600);
//    m_imageCapture->setEncodingSettings(encoder_setting);
//    connect(m_imageCapture, SIGNAL(imageSaved(int,QString)), this, SLOT(processSavedImage(int,QString)));
//    ...
//    m_imageCapture->capture();


//    am = new QCamera(0);
//    connect(&captureTimer, SIGNAL(timeout()),
//    this, SLOT (frameUpdated()));
//    videoWidget = new QVideoWidget();
//    videoWidget->setFixedSize(640,480);
//    cam->setViewfinder(videoWidget);

    //camera = new QCamera

    //camera = new QCamera;


}

D_error::~D_error()
{
	delete ui;
}

void D_error::on_b_close_clicked()
{
	this->close();
}


void D_error::change_info(int err, QString msg)
{
	ui->l_err_no->setText("No. " + QString().setNum(err));
	ui->l_err_content->setText(msg);
}


void D_error::change_err_list(int err)
{
    qDebug() << "change_err_list";

    static const char * const err_list[] = {
        QT_TR_NOOP("NO_ERR"),       //0
        QT_TR_NOOP("MAIN_MT_ERR"),  //1
        QT_TR_NOOP("SYNC_ERR"),     //2
        QT_TR_NOOP("NO_PTRN_ERR"),  //3
        QT_TR_NOOP("USB_EMPTY_ERR"),//4
        QT_TR_NOOP("DSK_RD_ERR"),   //5
        QT_TR_NOOP("DSK_WR_ERR"),   //6
        QT_TR_NOOP("DSK_FMT_ERR"),  //7
        QT_TR_NOOP("DSK_FULL_ERR"), //8
        QT_TR_NOOP("SCL_OVR_ERR"),  //9
        QT_TR_NOOP("STI_OVR_ERR"),  //10

        QT_TR_NOOP("CNT_RST_ERR"),    //11
        QT_TR_NOOP("CMB_ERR"),        //12
        QT_TR_NOOP("LMT_OVR_ERR"),    //13
        QT_TR_NOOP("NDL_POS_ERR"),    //14
        QT_TR_NOOP("CAL_ERR"),        //15
        QT_TR_NOOP("DATA_BAD_ERR"),   //16
        QT_TR_NOOP("EM_STOP_ERR"),    //17
        QT_TR_NOOP("THRD_CUT_ERR"),   //18
        QT_TR_NOOP("XY_ERR"),         //19
        QT_TR_NOOP("NO_SYS_ERR"),     //20

        QT_TR_NOOP("MEM_ERR"),          //21
        QT_TR_NOOP("WR_PRTCT_ERR"),     //22
        QT_TR_NOOP("INSUF_MEM_ERR"),    //23
        QT_TR_NOOP("LOW_PRESSURE"),     //24
        QT_TR_NOOP("DRAG_ERR"),         //25
        QT_TR_NOOP("LOWFEED_ERR"),      //26
        QT_TR_NOOP("PALETTE_ERR"),      //27
        QT_TR_NOOP("EM_SW_PRESSED"),    //28
        QT_TR_NOOP("ST_SW_PRESSED"),    //29
        QT_TR_NOOP("FF_SW_PRESSED"),    //30

        QT_TR_NOOP("FFL_SW_PRESSED"),   //31
        QT_TR_NOOP("TS_SW_PRESSED"),    //32
        QT_TR_NOOP("SER_COM_ERR"),      //33
        QT_TR_NOOP("FOUND"),            //34
        QT_TR_NOOP("NOT_FOUND"),        //35
        QT_TR_NOOP("Hook Origin_ERR"),  //36
        QT_TR_NOOP("LOW_MT_ERR"),       //37
        QT_TR_NOOP("Y_MT_ERR"),         //38
        QT_TR_NOOP("X_MT_ERR"),         //39
        QT_TR_NOOP("TIMER_ERR"),        //40

        QT_TR_NOOP("MOTOR_TYPE_ERR"),   //41
        QT_TR_NOOP("ASIPM_ERR"),        //42
        QT_TR_NOOP("OVC_ERR"),          //43
        QT_TR_NOOP("OVL_ERR"),          //44
        QT_TR_NOOP("ENCRST_ERR"),       //45
        QT_TR_NOOP("ENCAB_ERR"),        //46
        QT_TR_NOOP("SYNCIN_ERR"),       //47
        QT_TR_NOOP("SYNCOUT_ERR"),      //48
        QT_TR_NOOP("REVCOM_ERR"),       //49
        QT_TR_NOOP("EEPR_ERR"),         //50

        QT_TR_NOOP("MTINFO_SER_COM_ERR"),//51
        QT_TR_NOOP("BOB_STITCH_RESET"),  //52
        QT_TR_NOOP("SCL_EN_RE_ERR"),     //53
        QT_TR_NOOP("P_MT_ERR"),         //54
        QT_TR_NOOP("MC_OPEN_ERR"),      //55
        QT_TR_NOOP("PF_CLAMP_ORG_ERR"), //56
        QT_TR_NOOP("FAN_ERR"),          //57
        QT_TR_NOOP("AC_CHK_ERR"),       //58
        QT_TR_NOOP("OVER_VOLT_ERR"),    //59
        QT_TR_NOOP("UNDER_VOLT_ERR"),   //60

        QT_TR_NOOP("MTINFO_SER_COM_ERR"),//51
        QT_TR_NOOP("BOB_STITCH_RESET"),  //52
        QT_TR_NOOP("SCL_EN_RE_ERR"),    //53
        QT_TR_NOOP("P_MT_ERR"),         //54
        QT_TR_NOOP("MC_OPEN_ERR"),      //55
        QT_TR_NOOP("PF_CLAMP_ORG_ERR"), //56
        QT_TR_NOOP("FAN_ERR"),          //57
        QT_TR_NOOP("AC_CHK_ERR"),       //58
        QT_TR_NOOP("OVER_VOLT_ERR"),    //59
        QT_TR_NOOP("UNDER_VOLT_ERR"),   //60

        QT_TR_NOOP("CAN_COMM_ERR"),     //61
        QT_TR_NOOP("PWROFF_ERR"),       //62
        QT_TR_NOOP("PWRON_ERR"),        //63
        QT_TR_NOOP("RXCOM_ERR"),        //64
        QT_TR_NOOP("TXCOM_ERR"),        //65
        QT_TR_NOOP("PU0_ERR"),          //66
        QT_TR_NOOP("PU1_ERR"),          //67
        QT_TR_NOOP("MSYNC_ERR"),        //68
        QT_TR_NOOP("SF22_ERR"),         //69
        QT_TR_NOOP("RATE_ERR"),         //70

        QT_TR_NOOP("USB_ERR"),     //71
        QT_TR_NOOP("NULL"),     //72
        QT_TR_NOOP("NULL"),     //73
        QT_TR_NOOP("NULL"),     //74
        QT_TR_NOOP("NULL"),     //75
        QT_TR_NOOP("NULL"),     //76
        QT_TR_NOOP("NULL"),     //77
        QT_TR_NOOP("NULL"),     //78
        QT_TR_NOOP("NULL"),     //79
        QT_TR_NOOP("PARA_CNT_ERR"),     //80

        QT_TR_NOOP("CODE_PAUSE"),         //81
        QT_TR_NOOP("DATA_SEND1_ERR"),     //82
        QT_TR_NOOP("DATA_SEND2_ERR"),     //83
        QT_TR_NOOP("MC_COM_ERR"),         //84
        QT_TR_NOOP("NO_PATTERN"),         //85
        QT_TR_NOOP("PTRN_OVERWTR"),       //86
        QT_TR_NOOP("DESIGH_DATA1_ERR"),   //87
        QT_TR_NOOP("DESIGN_DATA2_ERR"),   //88
        0,
    };
    //ui->l_err_no->setText("No. " + QString().setNum(err));
    ui->l_err_no->setText(tr("No. ") + QString().setNum(err));
    //ui->l_err_no->setText(6, QString::setNum(value));
    ui->l_err_code->setText("");
    ui->l_err_content->setText(tr(err_list[err]));   //(msg);
    //qDebug() << "err_list[THRD_CUT_ERR]:" << err_list[THRD_CUT_ERR];
}


//QT_TRANSLATE_NOOP("err_list","NO_ERR"),       //0
//QT_TRANSLATE_NOOP("err_list","MAIN_MT_ERR"),  //1
//QT_TRANSLATE_NOOP("err_list","SYNC_ERR"),     //2
//QT_TRANSLATE_NOOP("err_list","NO_PTRN_ERR"),  //3
//QT_TRANSLATE_NOOP("err_list","USB_EMPTY_ERR"),//4
//QT_TRANSLATE_NOOP("err_list","DSK_RD_ERR"),   //5
//QT_TRANSLATE_NOOP("err_list","DSK_WR_ERR"),   //6
//QT_TRANSLATE_NOOP("err_list","DSK_FMT_ERR"),  //7
//QT_TRANSLATE_NOOP("err_list","DSK_FULL_ERR"), //8
//QT_TRANSLATE_NOOP("err_list","SCL_OVR_ERR"),  //9
//QT_TRANSLATE_NOOP("err_list","STI_OVR_ERR"),  //10

//QT_TRANSLATE_NOOP("err_list","CNT_RST_ERR"),    //11
//QT_TRANSLATE_NOOP("err_list","CMB_ERR"),        //12
//QT_TRANSLATE_NOOP("err_list","LMT_OVR_ERR"),    //13
//QT_TRANSLATE_NOOP("err_list","NDL_POS_ERR"),    //14
//QT_TRANSLATE_NOOP("err_list","CAL_ERR"),        //15
//QT_TRANSLATE_NOOP("err_list","DATA_BAD_ERR"),   //16
//QT_TRANSLATE_NOOP("err_list","EM_STOP_ERR"),    //17
//QT_TRANSLATE_NOOP("err_list","THRD_CUT_ERR"),   //18
//QT_TRANSLATE_NOOP("err_list","XY_ERR"),         //19
//QT_TRANSLATE_NOOP("err_list","NO_SYS_ERR"),     //20

//QT_TRANSLATE_NOOP("err_list","MEM_ERR"),          //21
//QT_TRANSLATE_NOOP("err_list","WR_PRTCT_ERR"),     //22
//QT_TRANSLATE_NOOP("err_list","INSUF_MEM_ERR"),    //23
//QT_TRANSLATE_NOOP("err_list","LOW_PRESSURE"),     //24
//QT_TRANSLATE_NOOP("err_list","DRAG_ERR"),         //25
//QT_TRANSLATE_NOOP("err_list","LOWFEED_ERR"),      //26
//QT_TRANSLATE_NOOP("err_list","PALETTE_ERR"),      //27
//QT_TRANSLATE_NOOP("err_list","EM_SW_PRESSED"),    //28
//QT_TRANSLATE_NOOP("err_list","ST_SW_PRESSED"),    //29
//QT_TRANSLATE_NOOP("err_list","FF_SW_PRESSED"),    //30

//QT_TRANSLATE_NOOP("err_list","FFL_SW_PRESSED"),   //31
//QT_TRANSLATE_NOOP("err_list","TS_SW_PRESSED"),    //32
//QT_TRANSLATE_NOOP("err_list","SER_COM_ERR"),      //33
//QT_TRANSLATE_NOOP("err_list","FOUND"),            //34
//QT_TRANSLATE_NOOP("err_list","NOT_FOUND"),        //35
//QT_TRANSLATE_NOOP("err_list","Hook Origin_ERR"),  //36
//QT_TRANSLATE_NOOP("err_list","LOW_MT_ERR"),       //37
//QT_TRANSLATE_NOOP("err_list","Y_MT_ERR"),         //38
//QT_TRANSLATE_NOOP("err_list","X_MT_ERR"),         //39
//QT_TRANSLATE_NOOP("err_list","TIMER_ERR"),        //40

//QT_TRANSLATE_NOOP("err_list","MOTOR_TYPE_ERR"),   //41
//QT_TRANSLATE_NOOP("err_list","ASIPM_ERR"),        //42
//QT_TRANSLATE_NOOP("err_list","OVC_ERR"),          //43
//QT_TRANSLATE_NOOP("err_list","OVL_ERR"),          //44
//QT_TRANSLATE_NOOP("err_list","ENCRST_ERR"),       //45
//QT_TRANSLATE_NOOP("err_list","ENCAB_ERR"),        //46
//QT_TRANSLATE_NOOP("err_list","SYNCIN_ERR"),       //47
//QT_TRANSLATE_NOOP("err_list","SYNCOUT_ERR"),      //48
//QT_TRANSLATE_NOOP("err_list","REVCOM_ERR"),       //49
//QT_TRANSLATE_NOOP("err_list","EEPR_ERR"),         //50

//QT_TRANSLATE_NOOP("err_list","MTINFO_SER_COM_ERR"),//51
//QT_TRANSLATE_NOOP("err_list","BOB_STITCH_RESET"),  //52
//QT_TRANSLATE_NOOP("err_list","SCL_EN_RE_ERR"),     //53
//QT_TRANSLATE_NOOP("err_list","P_MT_ERR"),         //54
//QT_TRANSLATE_NOOP("err_list","MC_OPEN_ERR"),      //55
//QT_TRANSLATE_NOOP("err_list","PF_CLAMP_ORG_ERR"), //56
//QT_TRANSLATE_NOOP("err_list","FAN_ERR"),          //57
//QT_TRANSLATE_NOOP("err_list","AC_CHK_ERR"),       //58
//QT_TRANSLATE_NOOP("err_list","OVER_VOLT_ERR"),    //59
//QT_TRANSLATE_NOOP("err_list","UNDER_VOLT_ERR"),   //60

//QT_TRANSLATE_NOOP("err_list","MTINFO_SER_COM_ERR"),//51
//QT_TRANSLATE_NOOP("err_list","BOB_STITCH_RESET"),  //52
//QT_TRANSLATE_NOOP("err_list","SCL_EN_RE_ERR"),    //53
//QT_TRANSLATE_NOOP("err_list","P_MT_ERR"),         //54
//QT_TRANSLATE_NOOP("err_list","MC_OPEN_ERR"),      //55
//QT_TRANSLATE_NOOP("err_list","PF_CLAMP_ORG_ERR"), //56
//QT_TRANSLATE_NOOP("err_list","FAN_ERR"),          //57
//QT_TRANSLATE_NOOP("err_list","AC_CHK_ERR"),       //58
//QT_TRANSLATE_NOOP("err_list","OVER_VOLT_ERR"),    //59
//QT_TRANSLATE_NOOP("err_list","UNDER_VOLT_ERR"),   //60

//QT_TRANSLATE_NOOP("err_list","CAN_COMM_ERR"),     //61
//QT_TRANSLATE_NOOP("err_list","PWROFF_ERR"),       //62
//QT_TRANSLATE_NOOP("err_list","PWRON_ERR"),        //63
//QT_TRANSLATE_NOOP("err_list","RXCOM_ERR"),        //64
//QT_TRANSLATE_NOOP("err_list","TXCOM_ERR"),        //65
//QT_TRANSLATE_NOOP("err_list","PU0_ERR"),          //66
//QT_TRANSLATE_NOOP("err_list","PU1_ERR"),          //67
//QT_TRANSLATE_NOOP("err_list","MSYNC_ERR"),        //68
//QT_TRANSLATE_NOOP("err_list","SF22_ERR"),         //69
//QT_TRANSLATE_NOOP("err_list","RATE_ERR"),         //70

//QT_TRANSLATE_NOOP("err_list","USB_ERR"),     //71
//QT_TRANSLATE_NOOP("err_list","NULL"),     //72
//QT_TRANSLATE_NOOP("err_list","NULL"),     //73
//QT_TRANSLATE_NOOP("err_list","NULL"),     //74
//QT_TRANSLATE_NOOP("err_list","NULL"),     //75
//QT_TRANSLATE_NOOP("err_list","NULL"),     //76
//QT_TRANSLATE_NOOP("err_list","NULL"),     //77
//QT_TRANSLATE_NOOP("err_list","NULL"),     //78
//QT_TRANSLATE_NOOP("err_list","NULL"),     //79
//QT_TRANSLATE_NOOP("err_list","PARA_CNT_ERR"),     //80

//QT_TRANSLATE_NOOP("err_list","CODE_PAUSE"),         //81
//QT_TRANSLATE_NOOP("err_list","DATA_SEND1_ERR"),     //82
//QT_TRANSLATE_NOOP("err_list","DATA_SEND2_ERR"),     //83
//QT_TRANSLATE_NOOP("err_list","MC_COM_ERR"),         //84
//QT_TRANSLATE_NOOP("err_list","NO_PATTERN"),         //85
//QT_TRANSLATE_NOOP("err_list","PTRN_OVERWTR"),       //86
//QT_TRANSLATE_NOOP("err_list","DESIGH_DATA1_ERR"),   //87
//QT_TRANSLATE_NOOP("err_list","DESIGN_DATA2_ERR"),   //88
//0,
