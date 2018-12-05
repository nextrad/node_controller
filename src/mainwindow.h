#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "video_connection_manager.h"
#include "includes.h"
#include "parameters.h"
#include "datetime.h"
#include "header_arm_files.h"

#include <math.h>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_testConnectionButton_clicked();

    void on_receiveNodePositionsButton_clicked();

    void on_calcBearingDistanceButton_clicked();

    void on_showVideoButton_clicked();

    void on_abortVideoRecButton_clicked();

    void startRecording(void);
    void stopRecording(void);
    void updateCountDownLCDAndPollHeaderFile(void);


private:
    Ui::MainWindow *ui;

    int m_counter;
    int experiment_state;


    // INACTIVE is for prior to and after an experiment
    // WAITING is before start time
    // ACTIVE is during an experiment, between start and end time
    // INACTIVE -> WAITING -> ACTIVE -> INACTIVE
    enum timeState {INACTIVE=0, WAITING=1, ACTIVE=2};

    enum buttonColour {GREEN=0, GRAY=1, RED=2};

    QTimer *starttimer;
    QTimer *endtimer;
    QTimer *countdowntimer;
    string startTime;
    string endTime;
    time_t strtUnixTime;
    time_t stopUnixTime;
    time_t currentUnixTime;

    class Point
    {
        public:
            double lat;
            double lon;
            double ht;
    };

    HeaderArmFiles headerarmfiles;
    VideoConnectionManager videoRecorder;

    bool checkForNewHeaderFile(void);

    bool testConnection(string address);
    void testSubNetwork(QString NetID);

    char* stringToCharPntr(string str);

    void receiveNodePosition(int node_num);


    void calcBearingDistance(int node_num);

    double toRadians (double degs);
    double toDegrees (double rads);
    double bearingTo(Point node, Point target);

    double calcDistance(Point node, Point target);


    QString getCountDownTime(time_t timeLeft);

    string replaceCharsinStr(string str_in, char ch_in, char ch_out);

    int calcExperimentLength(void);
    bool checkCountdown(void);



};

#endif // MAINWINDOW_H
