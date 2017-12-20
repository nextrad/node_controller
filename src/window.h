//Author:               David Bissett
//Class:                Window
//For:                  University of Cape Town, Dept. Elec. Eng., RRSG NeXtRAD
//Created:              April 2016
//Last Edited:          June 2016
//Edited By:            Shirley Coetzee and Darryn Jordan
//Revision:             4.0 (Dec 2017)

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
    private:
        int m_counter;
        enum timeState {inactive=0, running=1, stopped=2};
        QPushButton *testConnectionButton;
        //QPushButton *startPedControlButton;
        QPushButton *startVideoRecButton;
        QPushButton *abortVideoRecButton;
        QPushButton *showVideoButton;
        QPushButton *recvGPSDetailsButton;
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
        int timeMode;
        void initGUI(void);
        NetworkManager client;

    private slots:
        void connectionTestButtonClicked(void);
        //void startPedControlButtonClicked(void);
        void updateCountDownLCD(void);
        void startRecording(void);
        void stopRecording(void);
        void CountDownButtonClicked(void);
        void abortVideoRecordingButtonClicked(void);
        void showVideoButtonClicked(void);
        void startSoundButtonClicked(void);
        void stopSoundButtonClicked(void);
        void recvGPSDetailsButtonClicked(void);
        char* stringToCharPntr(string str);
        string replaceCharsinStr(string str_in, char ch_in, char ch_out);

    signals:
    public slots:
};

#endif // WINDOW_H
