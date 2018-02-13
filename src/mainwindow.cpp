//Class:                Window
//For:                  University of Cape Town, Dept. Elec. Eng., RRSG NeXtRAD
//Created:
//Author:               David Bissett
//Date Begun:           April 2016
//Last Edited:          June 2016
//Edited By:            Shirley Coetzee and Kevin Venter
//Revision:             2.0 (October 2016)
//Edited By:            Shirley Coetzee and Darryn Jordan and Dane Du Plessis
//Revision:             3.0 (Jan/Feb 2017)
//Edited By:            Shirley Coetzee and Darryn Jordan
//Revision:             4.0 (Dec 2017)
//Edited By:            Shirley Coetzee, Darryn Jordan and Simon Lewis
//Revision:             5.0 (Jan 2018)
//Edited By:            Shirley Coetzee
//Revision:             6.0 (Feb 2018)


//=======================================================================
// Includes
//=======================================================================

#include "window.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QApplication>
#include <QDateTime>

extern int NODE_ID;
extern int EXPERIMENT_LENGTH; //in seconds
extern string RTSP_HOST;
extern string OVERLAY_IMAGE;
extern string OUTPUT_DIRECTORY;


//=======================================================================
// Constructor
//=======================================================================
MainWindow::MainWindow(QWidget *parent) :
      QMainWindow(parent),
      ui(new Ui::MainWindow)
{
    experiment_state = INACTIVE; //see enum for explanation

    ui->setupUi(this);

    stringstream ss;
    ss << "Node " << (int)NODE_ID;
    ui->label->setText(stringToCharPntr(ss.str()));

    ui->Countdown->display("00:00:00");

    //start vlc telnet server, connect and configure
    system("x-terminal-emulator -e \"vlc -I telnet\n\"");
    videoRecorder.connectToSocket();
    videoRecorder.configureVideoStream();

    //start timer calls startRecording() once it reaches the start time (happens once)
    starttimer = new QTimer(this);
    starttimer->setSingleShot(true);
    connect(starttimer, SIGNAL(timeout()), this, SLOT(startRecording()));

    //end timer calls stopRecording() once it reaches the end time (happens once)
    endtimer = new QTimer(this);
    endtimer->setSingleShot(true);
    connect(endtimer, SIGNAL(timeout()), this, SLOT(stopRecording()));

    //the count down timer calls updateCountDownLCD() every second
    countdowntimer = new QTimer(this);
    countdowntimer->setInterval(1000);
    connect(countdowntimer, SIGNAL(timeout()), this, SLOT(updateCountDownLCD()));
    countdowntimer->start(1000);

}

//=======================================================================
// Destructor
//======================================================================
MainWindow::~MainWindow()
{
    delete ui;
}


//=======================================================================
// connectionTestButtonClicked()
// Tests the connection to CNC
// and polls for Header File
//=======================================================================
void MainWindow::on_testConnectionButton_clicked()
{
    ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Testing Connection...");
    connectionManager.connectionTest();
    if(connectionManager.isConnected())
    {
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Connected to network");
        ui->testConnectionButton->setText("Connected");
        ui->testConnectionButton->setStyleSheet("* { background-color: rgb(100,255,125) }");

        if(client.isServerConnected())
        {
            ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Server Already Connected");
        }
        else
        {
            client.connectToCNC();
            if(!client.isServerConnected())
            {
                //if connection refused then most likely the cnc server isn't running
                ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Error Connecting to Server");
                ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Check Terminal for Error");
            }
        }
    }
    else
    {
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Not Connected to Network");
        ui->testConnectionButton->setText("Not connected");
        ui->testConnectionButton->setStyleSheet("* { background-color: rgb(255,125,100) }");
    }


    int attempt = 0;

    // Poll for new Header file
    if ((connectionManager.isConnected()) && (client.isServerConnected()))
    {
        // Read Header file
        ifstream headerFile (HEADER_PATH);

        while(!headerFile.is_open())
        {
            cout << "Polling for header file, attempt: " << attempt++ << endl;
            usleep(0.1e6);
            headerFile.open(HEADER_PATH);
        }
        printf("Header File opened\n");
        headerFile.close();

        // Start countdown to armtime
        CountDownButtonClicked ();
    }
}


//=============================================================================
// receiveNodePositionsButtonClicked()
// method to receive the node's positions from the GPSDO.
//=============================================================================

void MainWindow::on_receiveNodePositionsButton_clicked()
{
    ui->statusBox->append("");
    ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Fetching GPS info file from GPSDO");
    ui->statusBox->append("");

    receiveNodePosition(NODE_ID);

    ui->statusBox->append("");
    ui->statusBox->setTextColor("black");
}

//=============================================================================
// receiveNodePosition()
//=============================================================================
void MainWindow::receiveNodePosition(int node_num)
{
    string lat, lon, ht;

    try
    {
        // Parse gpsinfo.ini file
        lat = headerarmfiles.readFromGPSInfoFile("LATITUDE");
        lon = headerarmfiles.readFromGPSInfoFile("LONGITUDE");
        ht = headerarmfiles.readFromGPSInfoFile("ALTITUDE");

        if ((lat == "Fault") || (lon == "Fault") || (ht == "Fault"))
        {
            // Display data on screen in red X per node
            ui->statusBox->setTextColor("red");
            ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      X    ") + "node" + QString::number(node_num));
        }
        else
        {
            // Update Header file
            switch (node_num)
            {
                case 0: headerarmfiles.writeToHeaderFile("GeometrySettings", "NODE0_LOCATION_LAT", lat);
                        headerarmfiles.writeToHeaderFile("GeometrySettings", "NODE0_LOCATION_LON", lon);
                        headerarmfiles.writeToHeaderFile("GeometrySettings", "NODE0_LOCATION_HT", ht);
                        break;
                case 1: headerarmfiles.writeToHeaderFile("GeometrySettings", "NODE1_LOCATION_LAT", lat);
                        headerarmfiles.writeToHeaderFile("GeometrySettings", "NODE1_LOCATION_LON", lon);
                        headerarmfiles.writeToHeaderFile("GeometrySettings", "NODE1_LOCATION_HT", ht);
                        break;
                case 2: headerarmfiles.writeToHeaderFile("GeometrySettings", "NODE2_LOCATION_LAT", lat);
                        headerarmfiles.writeToHeaderFile("GeometrySettings", "NODE2_LOCATION_LON", lon);
                        headerarmfiles.writeToHeaderFile("GeometrySettings", "NODE2_LOCATION_HT", ht);
                        break;
            }

            // Display data on screen in green values per node
            ui->statusBox->setTextColor("green");
            ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      _    ") + "node" + QString::number(node_num) + "\n " \
                        + "lat=" + QString::fromStdString(lat) + ", \tlong=" + QString::fromStdString(lon) + ", \tht=" + QString::fromStdString(ht));
        }

        ui->statusBox->append("");
    }
    catch(exception &e)
    {
        cout << "receiveNodePosition exception: " << e.what() << endl;
    }
}


//=============================================================================
// receiveBearingsButtonClicked()
// method to receive the bearings from the GPSDO.
//=============================================================================
void MainWindow::on_receiveBearingsButton_clicked()
{
    ui->statusBox->append("");
    ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Fetching node bearings file from Mission Control");
    ui->statusBox->append("");

    receiveBearings(NODE_ID);

    ui->statusBox->append("");
    ui->statusBox->setTextColor("black");
}

//=============================================================================
// receiveBearings()

/*  tardat2cc.rtf
(*171207*)

DTG	061855Z 1217
Target Lat/Lon 	{-34.1813,18.46}
n1: Range	1.82952
n1: Bearing	46.5192
*/
//=============================================================================
void MainWindow::receiveBearings(int node_num)
{
    string lat, lon, dtg, baseline_bisector;
    string n0range, n0bearing, n1range, n1bearing, n2range, n2bearing;

    try
    {
        // Parse tardat2cc.rtf file
        dtg = headerarmfiles.readFromBearingsFile("DTG", 4, 12);
        lat = headerarmfiles.readFromBearingsFile("Lat", 11, 8);
        lon = headerarmfiles.readFromBearingsFile("Lon", 16, 5);
        baseline_bisector = headerarmfiles.readFromBearingsFile("BASELINE_BISECTOR", 16, 5);
        n0range = headerarmfiles.readFromBearingsFile("n0: Range", 10, 7);
        n0bearing = headerarmfiles.readFromBearingsFile("n0: Bearing", 12, 7);
        n1range = headerarmfiles.readFromBearingsFile("n1: Range", 10, 7);
        n1bearing = headerarmfiles.readFromBearingsFile("n1: Bearing", 12, 7);
        n2range = headerarmfiles.readFromBearingsFile("n2: Range", 10, 7);
        n2bearing = headerarmfiles.readFromBearingsFile("n2: Bearing", 12, 7);

        if ((lat == "Fault") || (lon == "Fault") || (dtg == "Fault"))
        {
            // Display data on screen in red X per node
            ui->statusBox->setTextColor("red");
            ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      X    ") + "\tnode" + QString::number(node_num));
        }
        else
        {
            // Update Header file
            headerarmfiles.writeToHeaderFile("Bearings", "DTG", dtg);
            headerarmfiles.writeToHeaderFile("Bearings", "BASELINE_BISECTOR", baseline_bisector);
            headerarmfiles.writeToHeaderFile("TargetSettings", "TGT_LOCATION_LAT", lat);
            headerarmfiles.writeToHeaderFile("TargetSettings", "TGT_LOCATION_LON", lon);
            headerarmfiles.writeToHeaderFile("TargetSettings", "TGT_LOCATION_HT", "0.00");

            switch (node_num)
            {
                case 0: headerarmfiles.writeToHeaderFile("Bearings", "NODE0_RANGE", n0range);
                        headerarmfiles.writeToHeaderFile("Bearings", "NODE0_BEARING", n0bearing);
                        break;
                case 1: headerarmfiles.writeToHeaderFile("Bearings", "NODE1_RANGE", n1range);
                        headerarmfiles.writeToHeaderFile("Bearings", "NODE1_BEARING", n1bearing);
                        break;
                case 2: headerarmfiles.writeToHeaderFile("Bearings", "NODE2_RANGE", n2range);
                        headerarmfiles.writeToHeaderFile("Bearings", "NODE2_BEARING", n2bearing);
                        break;
            }

            // Display data on screen in green values per node
            ui->statusBox->setTextColor("green");
            ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      _    ") + "node" + QString::number(node_num) + "\n" \
                        + "lat=" + QString::fromStdString(lat) + "\tlong=" + QString::fromStdString(lon) + "\n" \
                        + "DTG=" + QString::fromStdString(dtg) + "\tbaseline_bisector=" + QString::fromStdString(baseline_bisector));

            switch(node_num)
            {
            case 0: ui->statusBox->append("n0 range=" + QString::fromStdString(n0range) + "\tn0 bearing=" + QString::fromStdString(n0bearing));
                    break;
            case 1: ui->statusBox->append("n1 range=" + QString::fromStdString(n1range) + "\tn1 bearing=" + QString::fromStdString(n1bearing));
                    break;
            case 2: ui->statusBox->append("n2 range=" + QString::fromStdString(n2range) + "\tn2 bearing=" + QString::fromStdString(n2bearing));
                    break;
            }
        }

        ui->statusBox->append("");
    }
    catch(exception &e)
    {
        cout << "receiveBearings exception: " << e.what() << endl;
    }
}


//=======================================================================
// showVideoButtonClicked()
//=======================================================================
void MainWindow::on_showVideoButton_clicked()
{
    stringstream ss;
    ss << "x-terminal-emulator -x -e \"cvlc --sub-filter logo --logo-file ";
    ss << OVERLAY_IMAGE;
    ss << " --width=500 rtsp://";
    ss << RTSP_HOST;
    ss << "/live/video/profile1\"";
    system(stringToCharPntr(ss.str()));
}

//=======================================================================
// stringToCharPntr()
//=======================================================================
char* MainWindow::stringToCharPntr(string str)
{
    char *cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    return cstr;
}


//=======================================================================
// abortVideoRecordingButtonClicked()
//=======================================================================
void MainWindow::on_abortVideoRecButton_clicked()
{
    //in 'countdown to start' mode only the starttimer needs to be stopped
    if(experiment_state == WAITING)
    {
        starttimer->stop();
        endtimer->stop();
    }
    //in 'countdown to end' mode the starttimer must be stopped as well as the recording
    else if(experiment_state == ACTIVE)
    {
        stopRecording();
        endtimer->stop();
    }
    experiment_state = INACTIVE;
    ui->countdownLabel->setText("Video recording aborted!");
    ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Video recording aborted!");
}

//=======================================================================
// updateCountDownLCD()
// Updates the countdown LCD according.
//=======================================================================
void MainWindow::updateCountDownLCD(void)
{
    currentUnixTime = time(NULL);
    if (experiment_state == INACTIVE)
    {
        ui->Countdown->display("00:00:00");
    }
    else if(experiment_state == WAITING)
    {
        ui->Countdown->display(getCountDownTime(strtUnixTime - currentUnixTime));
    }
    else if(experiment_state == ACTIVE)
    {
        ui->Countdown->display(getCountDownTime(stopUnixTime - currentUnixTime));
    }
}


//=======================================================================
// getCountDownTime()
//=======================================================================
//Calculates the hours minutes and seconds remaining for countdown
QString MainWindow::getCountDownTime(time_t timeLeft)
{
    Datetime datetime;
    QString temp = datetime.getCountDownTime(timeLeft); //"%d.%m.%Y_%I:%M:%S");
    return temp;
}

//=======================================================================
// startPedControlButtonClicked()
//=======================================================================
/*This method opens the pedestal controller program.
TODO At the moment this program hangs when the pedestal controller opens.
This is because they are open on the same thread. Multithreading or forking is needed to fix this.
*//*
void Window::startPedControlButtonClicked(void)
{
    stringstream ss;
    ss << "x-terminal-emulator -e \"sudo python /home/node" << (int)NODE_ID << "/Documents/node_controller/SkywatcherTxRxUBUNTU/client.py\n\"";
    system(stringToCharPntr(ss.str()));
}
*/
//=======================================================================

string MainWindow::replaceCharsinStr(string str_in, char ch_in, char ch_out)
{
    for (int i = 0; i < str_in.length(); i++)
    {
        if (str_in[i] == ch_in)
        {
            str_in[i] = ch_out;
        }
    }
    return str_in;
}


//=======================================================================
// CountDownButtonClicked()
// Start countdown to armtime
//=======================================================================
//This method parses the start and end times for the video recording and starts the countdown starttimer
void MainWindow::CountDownButtonClicked(void)
{
    Datetime datetime;
    stringstream ss_unixtime;
    HeaderArmFiles headerarmfiles;

    // read armtime from Header File values
    QString year = headerarmfiles.readFromHeaderFile("Timing", "YEAR");
    QString month = headerarmfiles.readFromHeaderFile("Timing", "MONTH");
    QString day = headerarmfiles.readFromHeaderFile("Timing", "DAY");
    QString hour = headerarmfiles.readFromHeaderFile("Timing", "HOUR");
    QString minute = headerarmfiles.readFromHeaderFile("Timing", "MINUTE");
    QString second = headerarmfiles.readFromHeaderFile("Timing", "SECOND");

    // calculate ENDTIMESECS from Header File values
    int num_pris = atoi(headerarmfiles.readFromHeaderFile("PulseParameters", "NUM_PRIS").toStdString().c_str());
    int pri = atoi(headerarmfiles.readFromHeaderFile("PulseParameters", "PRI").toStdString().c_str());    // microseconds
    EXPERIMENT_LENGTH = num_pris * pri * 1e-6;  // = 60000 * 1000/1000000 = 60

    //required format: YYYY-MM-DD HH:MM:SS
    ss_unixtime << year.toStdString() << "-" << month.toStdString() << "-" << day.toStdString() << " ";
    ss_unixtime << hour.toStdString() << ":" << minute.toStdString() << ":" << second.toStdString();

    //change times to Unix time format
    strtUnixTime = datetime.convertToUnixTime(ss_unixtime.str());
    stopUnixTime = strtUnixTime + EXPERIMENT_LENGTH;
    currentUnixTime = time(NULL);

    //check if the start/end times are in the past
    if(strtUnixTime < currentUnixTime)
    {
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Please use a future start time");
    }
    else if(stopUnixTime < strtUnixTime)
    {
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Please use a future stop time");
    }
    else // start countdown to armtime
    {
        starttimer->start((strtUnixTime - currentUnixTime)*1000);
        ui->countdownLabel->setText("Countdown to armtime");
        experiment_state = WAITING;
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Countdown to armtime");
        ui->statusBox->append("");
    }
}


//=======================================================================
// startRecording()
// Method to start video recording and starts the countdown until the end of experiment
//=======================================================================
void MainWindow::startRecording(void)
{
    experiment_state = ACTIVE;
    videoRecorder.startRecording();
    endtimer->start((stopUnixTime - currentUnixTime)*1000);
    ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Video recording started");
    ui->countdownLabel->setText("Video recording ends in ...");
}


//=======================================================================
// stopRecording()
// This method stops the video recording
//=======================================================================
void MainWindow::stopRecording(void)
{
    stringstream ss;

    ss << "Saving video recording ";

    videoRecorder.stopRecording();
    experiment_state = INACTIVE;
    ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Video recording stopped");
    ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + ss.str().c_str());
    ui->countdownLabel->setText("Video recording stopped");

    // Rename video recording filename to startTime

    string oldRecFileName = videoRecorder.getRecFilePath();

    string newRecFileName = startTime;
    newRecFileName = replaceCharsinStr(newRecFileName, '-', '_');
    newRecFileName = replaceCharsinStr(newRecFileName, ' ', '_');

    //cout << "oldRecFileName = " + oldRecFileName + ", newrecFileName = " << newRecFileName << endl;

   if (startTime != "")
    {
        string command;
        command = "mv " + oldRecFileName + ".mp4 " + OUTPUT_DIRECTORY + newRecFileName + ".mp4";
        system(command.c_str());
        cout << "Renamed video file to: " << newRecFileName << endl;
    }
}
