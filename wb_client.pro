#-------------------------------------------------
#
# Project created by QtCreator 2020-01-13T10:25:25
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


TARGET = wb_client
TEMPLATE = app

DENABLE_PRECOMPILED_HEADERS = OFF

SOURCES += main.cpp\
    NavigationMsg.pb.cc \
    mainwindow.cpp \
    usercontrol/qmylistwidget.cpp \
    custominterface/setmapdialog.cpp \
    custominterface/setipdialog.cpp \
    custominterface/syncmapdialog.cpp \
    usercontrol/qmylabel.cpp \
    custominterface/datainitdialog.cpp \
    communication.cpp \
    custominterface/addobstaclesdialog.cpp

HEADERS  += \
    NavigationMsg.pb.h \
    mainwindow.h \
    usercontrol/qmylistwidget.h \
    custominterface/setmapdialog.h \
    custominterface/setipdialog.h \
    custominterface/syncmapdialog.h \
    usercontrol/qmylabel.h \
    custominterface/datainitdialog.h \
    communication.h \
    custominterface/addobstaclesdialog.h

FORMS    += \
    mainwindow.ui \
    custominterface/setmapdialog.ui \
    custominterface/setipdialog.ui \
    custominterface/syncmapdialog.ui \
    custominterface/datainitdialog.ui \
    custominterface/addobstaclesdialog.ui

CONFIG += console C++11

INCLUDEPATH += /usr/include/opencv \
               /usr/include/opencv2 \
               /usr/local/include/cpprest \
               ./usercontrol \
               ./custominterface

#LIBS +=/usr/lib/x86_64-linux-gnu/libopencv_highgui.so \
#        /usr/lib/x86_64-linux-gnu/libopencv_core.so    \
#        /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so \
#       -lprotobuf \
#        -lboost_system -lboost_thread\
        #-lboost_chrono \
        #-lcpprest

        #/usr/lib/x86_64-linux-gnu/libopencv_imgcodecs.so

LIBS += \
    -L/usr/local/lib \
    -lpthread \
    -lprotobuf -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_ml -lboost_system -lopencv_imgcodecs -ljsoncpp

