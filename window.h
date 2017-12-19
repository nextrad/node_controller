//Author:               David Bissett
//Class:                Window
//For:                  University of Cape Town, Dept. Elec. Eng., RRSG NeXtRAD
//Created:              April 2016
//Last Edited:          June 2016

#ifndef WINDOW_H
#define WINDOW_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QLCDNumber>
#include <QTextEdit>
#include "connection_manager.h"
#include "video_connection_manager.h"
#include "networkmanager.hpp"
#include "includes.h"
#include "parameters.h"
#include "datetime.h"
#include "header_arm_files.h"

class QPushButton;
class Window : public QWidget
{
    Q_OBJECT
    public:
        explicit Window(QWidget *parent = 0);
        QString getCountDownTime(time_t timeLeft);
        time_t convertToUnixTime(string time);
    private:
        int m_counter;
        QPushButton *testConnectionButton;
        QPushButton *startPedControlButton;
        QPushButton *startVideoRecButton;
        QPushButton *abortVideoRecButton;
        QPushButton *showVideoButton;
        QPushButton *recvGPSDetailsButton;
        QPushButton *indCompHeaderButton;
        QPushButton *startSoundButton;
        QPushButton *stopSoundButton;
        QLCDNumber *countDown;
        QTextEdit *statusBox;
        QLabel *countDownLabel;
        ConnectionManager connectionManager;
        VideoConnectionManager videoRecorder;
        QTimer *starttimer;
        QTimer *endtimer;
        QTimer *countDownTim;
        string startTime;
        string endTime;
        time_t strtUnixTime;
        time_t stopUnixTime;
        time_t currentUnixTime;
        int timMode;
        void initGUI(void);
        NetworkManager client;

    private slots:
        void connectionTestButtonClicked(void);
        //void startPedControlButtonClicked(void);
        void updateCountDownLCD(void);
        void startRecording(void);
        void stopRecording(void);
        void CountDownButtonClicked(void);  //startVideoRecordingCountDownButtonClicked
        void abortVideoRecordingButtonClicked(void);
        void showVideoButtonClicked(void);
        void startSoundButtonClicked(void);
        void stopSoundButtonClicked(void);
        //void recvHFileButtonClicked(void);
        //void recvGPSDetailsButtonClicked(void);
        //void indCompHeaderButtonClicked(void);
        char* stringToCharPntr(string str);
        string replaceCharsinStr(string str_in, char ch_in, char ch_out);


    signals:
    public slots:
};

#endif // WINDOW_H
