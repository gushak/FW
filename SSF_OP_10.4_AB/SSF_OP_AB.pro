#-------------------------------------------------
#
# Project created by QtCreator 2015-07-10T11:32:51
#
# 2018-11-29
# 코오롱 샘플 설치 관련
# 1. 메인화면 display관련 l_information 출력부 수정(mainwindow.cpp)
# 2. 작업자 무작위 write 수정(d_set_codd.cpp, 작업자 확인 및 #W 호출 부분 주석처리+입력부 기계코드와 동일하게
# 3. 통신프로토콜 worker_id_number -> worker_id로 변경
#
#-------------------------------------------------

QT       += core gui network


TARGET = rsm_6040_ht
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp \
    fdwatch.cpp \
    u_file.cpp \
    d_set_code.cpp \
    d_set_job_info.cpp \
    d_set_para.cpp \
    UserInfo.cpp \
    d_setting.cpp

HEADERS  += mainwindow.h \
    path.h \
    fdwatch.h \
    protocol.h \
    u_file.h \
    field.h \
    iodefine.h \
    d_set_code.h \
    d_set_job_info.h \
    d_set_para.h \
    UserInfo.h \
    d_setting.h

FORMS    += mainwindow.ui \
    d_setting.ui \
    d_set_code.ui \
    d_set_job_info.ui \
    d_set_para.ui

RESOURCES += \
    #images.qrc \
    #jquery.qrc
    images.qrc \
    jquery.qrc


INCLUDEPATH +=  $$PWD/
DEPENDPATH += $$PWD/





CONFIG(release, debug|release){
#unix:!macx:!symbian: LIBS += -L/usr/lib/mysql  -lrt
}
CONFIG(debug, debug|release){
#unix:!macx:!symbian: LIBS += -L$$PWD/lib/ -lrt
}


