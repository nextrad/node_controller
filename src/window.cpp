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

//=======================================================================
// Includes
//=======================================================================

#include "window.h"
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
Window::Window(QWidget *parent) : QWidget(parent)
{
    experiment_state = INACTIVE; //see enum for explanation

    //Set size of window
    setMinimumSize(600,400); //FixedSize(600,400);

    initGUI();

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
// initGUI()
//=======================================================================
void Window::initGUI(void)
{
    //Title
    QLabel* label = new QLabel(this);
    label->setGeometry(10, 10, 160, 50);
    stringstream ss;
    ss << "Node " << (int)NODE_ID;
    label->setText(stringToCharPntr(ss.str()));
    label->setFont(QFont("Ubuntu",36));
    label->show();

    //text label above countdown LCD
    countDownLabel = new QLabel(this);
    countDownLabel->setGeometry(445, 280, 140, 60);
    countDownLabel->setText("No countdown to video\nrecording active");
    countDownLabel->setFont(QFont("Ubuntu",10));
    countDownLabel->show();

    //Status output box for text output
    statusBox = new QTextEdit(this);
    statusBox->setGeometry(170, 70, 415, 200); //180);

    //countdown LCD number set to bottom right
    countDown = new QLCDNumber(8, this);
    countDown->setGeometry(445, 335, 140, 50);
    countDown->display("00:00:00");

    //button for testing network connection to CNC
    testConnectionButton = new QPushButton("Check Network\nConnection", this);
    testConnectionButton->setGeometry(15, 70, 140, 50);
    connect(testConnectionButton, SIGNAL (clicked(bool)), this, SLOT(connectionTestButtonClicked(void)));

    //button for receiving node's position
    receiveNodePositionsButton = new QPushButton("Receive Node\nDetails", this);
    receiveNodePositionsButton->setGeometry(15, 120, 140, 50);
    connect(receiveNodePositionsButton, SIGNAL (clicked(bool)), this, SLOT(receiveNodePositionsButtonClicked(void)));

    //button to show the video recording
    showVideoButton = new QPushButton("Show Video\nFeed", this);
    showVideoButton->setGeometry(15, 170, 140, 50);
    connect(showVideoButton, SIGNAL (clicked(bool)), this, SLOT(showVideoButtonClicked(void)));

    //button to abort the video recording
    abortVideoRecButton = new QPushButton("Abort\nVideo Recording", this);
    abortVideoRecButton->setGeometry(15, 220, 140, 50);
    connect(abortVideoRecButton, SIGNAL (clicked(bool)), this, SLOT(abortVideoRecordingButtonClicked(void)));
}


//=======================================================================
// showVideoButtonClicked()
//=======================================================================
void Window::showVideoButtonClicked(void)
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
char* Window::stringToCharPntr(string str)
{
    char *cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    return cstr;
}


//=============================================================================
// receiveNodePositionsButtonClicked()
// method to receive the node's positions from the GPSDO.
//=============================================================================

void Window::receiveNodePositionsButtonClicked(void)
{
    statusBox->append("");
    statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Fetching GPS info file from GPSDO");
    statusBox->append("");

    receiveNodePosition(NODE_ID);

    statusBox->append("");
}

//=============================================================================
// receiveNodePosition()
//=============================================================================
void Window::receiveNodePosition(int node_num)
{
    stringstream ss;
    int status;
    int ret;
    string lat, lon, ht;

    ss << "ansible node" << node_num << " -m fetch -a \"src=~/Desktop/NextGPSDO/gps_info.cfg dest=~/Documents/cnc_controller/node"  << node_num << "/\"";
    cout << ss.str().c_str() << endl;

    status = system(stringToCharPntr(ss.str()));
    if (-1 != status)
    {
        ret = WEXITSTATUS(status);

        if (ret==0)
        {
            // Parse gpsinfo.ini file
            lat = headerarmfiles.readFromGPSInfoFile(node_num,"LATITUDE");
            lon = headerarmfiles.readFromGPSInfoFile(node_num,"LONGITUDE");
            ht = headerarmfiles.readFromGPSInfoFile(node_num,"ALTITUDE");

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
            statusBox->setTextColor("green");
            statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      _    ") + "node" + QString::number(node_num) + "\n " \
                        + "lat=" + QString::fromStdString(lat) + ", \tlong=" + QString::fromStdString(lon) + ", \tht=" + QString::fromStdString(ht));
        }
        else
        {
            // Display data on screen in red X per node
            statusBox->setTextColor("red");
            statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      X    ") + "node" + QString::number(node_num));
        }
    }
    ss.str("");             //clear stringstream
    statusBox->append("");
}


//=======================================================================
// abortVideoRecordingButtonClicked()
//=======================================================================
void Window::abortVideoRecordingButtonClicked(void)
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
    countDownLabel->setText("Video recording aborted!");
    statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Video recording aborted!");
}

//=======================================================================
// updateCountDownLCD()
// Updates the countdown LCD according.
//=======================================================================
void Window::updateCountDownLCD(void)
{
    currentUnixTime = time(NULL);
    if (experiment_state == INACTIVE)
    {
        countDown->display("00:00:00");
    }
    else if(experiment_state == WAITING)
    {
        countDown->display(getCountDownTime(strtUnixTime - currentUnixTime));
    }
    else if(experiment_state == ACTIVE)
    {
        countDown->display(getCountDownTime(stopUnixTime - currentUnixTime));
    }
}


//=======================================================================
// getCountDownTime()
//=======================================================================
//Calculates the hours minutes and seconds remaining for countdown
QString Window::getCountDownTime(time_t timeLeft)
{
    Datetime datetime;
    QString temp = datetime.getCountDownTime(timeLeft); //"%d.%m.%Y_%I:%M:%S");
    return temp;
}

//=======================================================================
// connectionTestButtonClicked()
//=======================================================================
//Tests the connection to CNC
void Window::connectionTestButtonClicked(void)
{
    statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Testing Connection...");
    connectionManager.connectionTest();
    if(connectionManager.isConnected())
    {
        statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Connected to network");
        testConnectionButton->setText("Connected");
        testConnectionButton->setStyleSheet("* { background-color: rgb(100,255,125) }");

        if(client.isServerConnected())
        {
            statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Server Already Connected");
        }
        else
        {
            client.connectToCNC();
            if(!client.isServerConnected())
            {
                //if connection refused then most likely the cnc server isn't running
                statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Error Connecting to Server");
                statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Check Terminal for Error");
            }
        }
    }
    else
    {
        statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Not Connected to Network");
        testConnectionButton->setText("Not connected");
        testConnectionButton->setStyleSheet("* { background-color: rgb(255,125,100) }");
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

string Window::replaceCharsinStr(string str_in, char ch_in, char ch_out)
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
void Window::CountDownButtonClicked(void)
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
        statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Please use a future start time");
    }
    else if(stopUnixTime < strtUnixTime)
    {
        statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Please use a future stop time");
    }
    else // start countdown to armtime
    {
        starttimer->start((strtUnixTime - currentUnixTime)*1000);
        countDownLabel->setText("Countdown to armtime");
        experiment_state = WAITING;
        statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Countdown to armtime");
        statusBox->append("");
    }
}


//=======================================================================
// startRecording()
//=======================================================================
//Method to start video recording and starts the countdown until the end of experiment
void Window::startRecording(void)
{
    experiment_state = ACTIVE;
    videoRecorder.startRecording();
    endtimer->start((stopUnixTime - currentUnixTime)*1000);
    statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Video recording started");
    countDownLabel->setText("Video recording ends in ...");
}


//=======================================================================
// stopRecording()
//=======================================================================
//This method stops the video recording
void Window::stopRecording(void)
{
    stringstream ss;

    ss << "Saving video recording ";

    videoRecorder.stopRecording();
    experiment_state = INACTIVE;
    statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Video recording stopped");
    statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + ss.str().c_str());
    countDownLabel->setText("Video recording stopped");

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
