#include <QtGui/QApplication>
#include <QTextCodec>
#include "mainwindow.h"
#include "d_set_job_info.h"
//#include <QTranslator>      //(130927) test
#include <unistd.h>
//#define QT_NO_CURSOR
MainWindow *w;

#include <QFontDatabase>

int main(int argc, char *argv[])
{
    // Q_INIT_RESOURCE(lang);  //(131001)
    QApplication a(argc, argv);

    w = new MainWindow();

//    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
//    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
//    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    w->setWindowFlags(Qt::FramelessWindowHint);
QTextCodec::setCodecForTr(QTextCodec::codecForName("eucKR"));
QTextCodec::setCodecForCStrings(QTextCodec::codecForName("eucKR"));
QTextCodec::setCodecForLocale(QTextCodec::codecForName("eucKR"));

    //    int id = QFontDatabase::addApplicationFont(qApp->applicationDirPath()+"/data/fonts/sunfont.ttf");
    //    QString family = QFontDatabase::applicationFontFamilies(id).at(0);

    //    QFont SerifFont(family);
    //    a.setFont(SerifFont);
    //    QApplication::setFont(SerifFont);    //20120906
    //    qDebug()<<"import font ->  "<<id;

    //QFontDatabase::addApplicationFont("/app/data/fonts/sunfont.ttf");
    //QFont myfont("swiss");            //20120906

    //QFont myfont("sunfont");
    //QFont myfont("/app/data/fonts/sunfont.ttf");
    //QApplication::setFont(myfont);      //20120902

    //QFontInfo info(myfont);
    //QFont realFont(info.family());
    //QString rawName = realFont.rawName();

    //qDebug()<<"font"<<rawName;
    //QApplication::setFont(myfont);    //20120906
    //qApp->processEvents();F
#ifdef LINUX_TEST       //(130628)
    //QFont myfont("morden_gothic.ttf");  //20121013
    //QFont myfont("gulim.ttc");          //20131001
    //QFont myfont("UnBatang");          //20131001


    //    int newfontId = QFontDatabase::addApplicationFont("/app/data/fonts/sunfont.ttf");
    //            QString fontname = QFontDatabase::applicationFontFamilies(newfontId).at(0);
    //            QFont font(fontname);
    //            font.setPointSize(42);
    //            font.setStyleStrategy(QFont::PreferAntialias);
    //            //ui->label_font->setFont(font);
    //            QApplication::setFont(font);

    //    QFontDatabase fontDB;
    //    fontDB.addApplicationFont("/app/data/fonts/sunfont.ttf");
    //    QFont myfont("sunfont.ttf");
    //    QApplication::setFont(myfont);    //20120906
    //    qApp->processEvents();
#else

    //    int newfontId = QFontDatabase::addApplicationFont("/app/data/fonts/sunfont.ttf");
    //            QString fontname = QFontDatabase::applicationFontFamilies(newfontId).at(0);
    //            QFont font(fontname);
    //            font.setPointSize(42);
    //            font.setStyleStrategy(QFont::PreferAntialias);
    //            //ui->label_font->setFont(font);
    //            QApplication::setFont(font);


    //    QFontDatabase fontDB;
    //    fontDB.addApplicationFont("/app/data/fonts/sunfont.ttf");

    //        QFont myfont("sunfont");
    //        QApplication::setFont(myfont);    //20120906
    //        qApp->processEvents();

    //QFontDatabase::addApplicationFont("/app/data/fonts/sunfont.ttf");

    QFont myfont("sunfont");
    QApplication::setFont(myfont);
    w->setWindowIcon(QIcon(":/icon"));
    //app.setWindowIcon(QIcon(":/icon");



    qApp->processEvents();

    //QFont myfont("gulim");            //20120902
    //QFont myfont("morden_gothic.ttf");  //(131122)
    //    QFont myfont("sunfont.ttf");
    //myfont.setPixelSize(20);
    //    QApplication::setFont(myfont);      //20120902
    //------- test -------
    //m_font = QFont("gulim",20);
    //setFont(m_font);
#endif


    //QApplication::setOverrideCursor(Qt::BlankCursor);

    w->move(0, 0);
    w->show();

    return a.exec();
}



//QFont myfont("gulim");            //20120902
//QApplication::setFont(myfont);    //20120902
//myfont.setPixelSize(11);
#if 0
void InstallProc::playlogo()
{
    QLabel *logo;
    QMovie *logoPlayer;

    this->setGeometry(0,0,800,600); // size change

    logoPlayer = new QMovie();
    logoPlayer->setCacheMode(QMovie::CacheAll);

    logo = new QLabel(tr("no gif"),this);
    logo->setAlignment(Qt::AlignCenter);
    logo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    logoPlayer->stop();
    logo->setMovie(logoPlayer);
    logo->setGeometry(0,0,800,600);
    logo->show();

    //logo display
    //logoPlayer->setFileName(":/logo/INTRO");
    logoPlayer->setFileName(":/images/icon/opening.gif");
    logoPlayer->start();
}
#endif
