TEMPLATE = app
TARGET = node_controller

QT = core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

LIBS += -L/usr/lib/x86_64-linux-gnu -lboost_system -pthread

QMAKE_CXXFLAGS += -std=c++0x -pthread   #using c++11 and multithreading

SOURCES += \
    main.cpp \
	mainwindow.cpp \
    connection_manager.cpp \
    video_connection_manager.cpp \
    networkmanager.cpp \
    datetime.cpp \
    header_arm_files.cpp

HEADERS += \
    mainwindow.h \
    connection_manager.h \
    parameters.h \
    includes.h \
    video_connection_manager.h \
    networkmanager.hpp \
    header_arm_files.h \
    datetime.h \
    SimpleIni.h

FORMS    += mainwindow.ui
