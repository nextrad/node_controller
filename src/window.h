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
        void receiveNodePosition(int node_num);
        void receiveBearings(int node_num);
        string replaceCharsinStr(string str_in, char ch_in, char ch_out);
        char* stringToCharPntr(string str);
        string setButtonColour(int colourno);

    private:

        int m_counter;
        int experiment_state;

        // INACTIVE is for prior to and after an experiment
        // WAITING is before start time
        // ACTIVE is during an experiment, between start and end time
        // INACTIVE -> WAITING -> ACTIVE -> INACTIVE
        enum timeState {INACTIVE=0, WAITING=1, ACTIVE=2};

        enum buttonColour {GREEN=0, GRAY=1, RED=2};

        QPushButton *testConnectionButton;
        //QPushButton *startPedControlButton;
        QPushButton *startVideoRecButton;
        QPushButton *abortVideoRecButton;
        QPushButton *showVideoButton;
        QPushButton *receiveNodePositionsButton;
        QPushButton *receiveBearingsButton;

        QLCDNumber *countDown;
        QTextEdit *statusBox;
        QLabel *countDownLabel;

        QTimer *starttimer;
        QTimer *endtimer;
        QTimer *countdowntimer;
        string startTime;
        string endTime;
        time_t strtUnixTime;
        time_t stopUnixTime;
        time_t currentUnixTime;

        ConnectionManager connectionManager;
        HeaderArmFiles headerarmfiles;
        VideoConnectionManager videoRecorder;
        NetworkManager client;

        void initGUI(void);


    private slots:
        void connectionTestButtonClicked(void);
        //void startPedControlButtonClicked(void);
        void updateCountDownLCD(void);
        void startRecording(void);
        void stopRecording(void);
        void CountDownButtonClicked(void);
        void abortVideoRecordingButtonClicked(void);
        void showVideoButtonClicked(void);
        void receiveNodePositionsButtonClicked(void);
        void receiveBearingsButtonClicked(void);

    signals:
    public slots:
};

#endif // WINDOW_H
