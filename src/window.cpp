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


//=======================================================================
// Includes
//=======================================================================

#include "window.h"
#include <QPushButton>
#include <QApplication>
#include <QDateTime>


//=======================================================================
// Constructor
//=======================================================================
Window::Window(QWidget *parent) : QWidget(parent)
{
    timMode = 0; //count down mode

    //Set size of window
    setMinimumSize(600,400); //FixedSize(600,400);

    initGUI();
    //Establish connection to camera
    printf("NeXtRAD VLC Telnet Controller\n");
    printf("-----------------------------\n");

    system("x-terminal-emulator -e \"vlc -I telnet\n\"");
    sleep(1);

    videoRecorder.connectToSocket();
    videoRecorder.configureVideoStream();

    //Set up starttimers for start and end recording and update countdown clock
    stopUnixTime = time(NULL);
    starttimer = new QTimer(this);
    starttimer->setSingleShot(true);
    connect(starttimer, SIGNAL(timeout()), this, SLOT(startRecording()));

    endtimer = new QTimer(this);
    endtimer->setSingleShot(true);
    connect(endtimer, SIGNAL(timeout()), this, SLOT(stopRecording()));

    countDownTim = new QTimer(this);
    countDownTim->setInterval(1000);
    connect(countDownTim, SIGNAL(timeout()), this, SLOT(updateCountDownLCD()));
    countDownTim->start(1000);

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
    ss.str("");         //clear stringstream
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

//    //button for starting pedestal controller program
//    startPedControlButton = new QPushButton("Start Pedestal\nController", this);
//    startPedControlButton->setGeometry(15, 120, 140, 50);
//    connect(startPedControlButton, SIGNAL (clicked(bool)), this, SLOT(startPedControlButtonClicked(void)));

    //button to show the video recording
    showVideoButton = new QPushButton("Show Video\nFeed", this);
    showVideoButton->setGeometry(15, 120, 140, 50);
    connect(showVideoButton, SIGNAL (clicked(bool)), this, SLOT(showVideoButtonClicked(void)));
/*
    //button for starting countdown starttimer for video recording
    startVideoRecButton = new QPushButton("Countdown to\nVideo Recording", this);
    startVideoRecButton->setGeometry(15, 170, 140, 50);
    connect(startVideoRecButton, SIGNAL (clicked(bool)), this, SLOT(CountDownButtonClicked(void)));
*/
    //button to abort the video recording
    abortVideoRecButton = new QPushButton("Abort\nVideo Recording", this);
    abortVideoRecButton->setGeometry(15, 220, 140, 50);
    connect(abortVideoRecButton, SIGNAL (clicked(bool)), this, SLOT(abortVideoRecordingButtonClicked(void)));
/*
    //button to start sound
    startSoundButton = new QPushButton("Start\nVideo Recording", this);
    startSoundButton->setGeometry(15, 270, 140, 50);
    connect(startSoundButton, SIGNAL (clicked(bool)), this, SLOT(startSoundButtonClicked(void)));

    //button to stop sound
    stopSoundButton = new QPushButton("Start\nSound", this);
    stopSoundButton->setGeometry(15, 320, 140, 50);
    connect(stopSoundButton, SIGNAL (clicked(bool)), this, SLOT(stopSoundButtonClicked(void)));
*/
    //    //button for starting pedestal controller program
//    startPedControlButton = new QPushButton("Start Pedestal\nController", this);
//    startPedControlButton->setGeometry(15, 310, 140, 50);
//    connect(startPedControlButton, SIGNAL (clicked(bool)), this, SLOT(startPedControlButtonClicked(void)));

//    //button to send Header file to Cobalt
//    indCompHeaderButton = new QPushButton("Send Header\nto Cobalt", this);
//    indCompHeaderButton->setGeometry(170, 250, 140, 50);
//    connect(indCompHeaderButton, SIGNAL (clicked(bool)), this, SLOT(indCompHeaderButtonClicked(void)));

//    //button for receiving node's position
//    recvGPSDetailsButton = new QPushButton("Receive Node\nDetails", this);
//    recvGPSDetailsButton->setGeometry(170, 310, 140, 50);
//    connect(recvGPSDetailsButton, SIGNAL (clicked(bool)), this, SLOT(recvGPSDetailsButtonClicked(void)));

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
// indCompHeaderButtonClicked()
//=======================================================================
/*
void Window::indCompHeaderButtonClicked(void)
{
    char* hFile [108];
    ifstream file("NeXtRAD Header.txt");

//    char* hFile [108];
//    ifstream file("NeXtRAD Header.txt");

//    string temp;
//    char* line;
//    int8_t len [108];

//    for(int i = 0; i < 108; i++)
//    {
//        getline(file,temp);
//        len[i] = temp.length();
//        line = stringToCharPntr(temp);
//        hFile [i] = line;
//    }
//    file.close();

//    client.sendIndCompFile(hFile, len);


    stringstream ss;
    ss << "scp \"NeXtRAD Header.txt\" nextradnode" << (int) NODE_ID << "@192.168.1." << (int) (NODE_ID + 1) << "5:Desktop\n"; // scp is similar to ssh but used for file transfer
    system(stringToCharPntr(ss.str()));

}
*/

//=======================================================================
// stringToCharPntr()
//=======================================================================
char* Window::stringToCharPntr(string str)
{
    char *cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    return cstr;
}


//=======================================================================
// recvGPSDetailsButtonClicked()
//=======================================================================
/*
void Window::recvGPSDetailsButtonClicked(void)
{
    if(client.isServerConnected())
    {
        client.receivePosition();
    }11
    else
    {
        statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Server Not Running");
    }
}
*/

//=======================================================================
// recvHFileButtonClicked()
//=======================================================================
/*
void Window::recvHFileButtonClicked(void)
{
    if(client.isServerConnected())
    {
        client.receiveData();
    }
    else
    {
        statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Server Not Running");
    }

}
*/


//=======================================================================
// abortVideoRecordingButtonClicked()
//=======================================================================
void Window::abortVideoRecordingButtonClicked(void)
{
    //in 'countdown to start' mode only the starttimer needs to be stopped
    if(timMode == 1)
    {
        starttimer->stop();
    }
    //in 'countdown to end' mode the starttimer must be stopped as well as the recording
    else if(timMode == 2)
    {
        stopRecording();
        endtimer->stop();
    }
    timMode = 0;
    countDownLabel->setText("Video recording aborted!");
    statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Video recording aborted!");
}

//=======================================================================
// startSoundButtonClicked()
//=======================================================================
void Window::startSoundButtonClicked(void)
{

        startRecording();

}


//=======================================================================
// stopSoundButtonClicked()
//=======================================================================
void Window::stopSoundButtonClicked(void)
{
    //in 'countdown to start' mode only the starttimer needs to be stopped
    if(timMode == 1)
    {
        starttimer->stop();
    }
    //in 'countdown to end' mode the starttimer must be stopped as well as the recording
    else if(timMode == 2)
    {
        stopRecording();
        endtimer->stop();
    }
    timMode = 0;
    countDownLabel->setText("Video recording aborted!");
    statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Video recording aborted!");
}



//=======================================================================
// updateCountDownLCD()
//=======================================================================
/*Updates the countdown LCD according.  timMode==0 - no active countdowns
                                        timMode==1 - countdown until recording starts
                                        timMode==2 - countdown until recording ends
*/
void Window::updateCountDownLCD(void)
{
    currentUnixTime = time(NULL);
    if(timMode == 0)
    {
        countDown->display("00:00:00");
    }
    else if(timMode == 1)
    {
        countDown->display(getCountDownTime(strtUnixTime - currentUnixTime));
    }
    else
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
    /*
    //Convert from unix time to hours, minutes, seconds
    int tim = (int) timeLeft;
    int hours, mins, secs;
    secs = tim % 60;
    tim -= secs;
    tim = tim / 60;
    mins = tim % 60;
    tim -= mins;
    hours = tim / 60;
*/
    //convert to QString to make easier to output
/*
    QString temp = QStringLiteral("%1:").arg(hours, 2, 10, QChar('0'));
    temp.append(QStringLiteral("%1").arg(mins, 2, 10, QChar('0')));
    temp.append(QStringLiteral(":%1").arg(secs, 2, 10, QChar('0')));
    return temp;
*/

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

//            statusBox->setTextColor("green");
//            statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      _     cnc")); // + QString::fromStdString(temp) ); //+ "CNCHub connected");
//            statusBox->append("");
        }
        else
        {
            client.connectToCNC();
            if(!client.isServerConnected())
            {
                statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Error Connecting to Server");
                statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Check Terminal for Error");
                //if connection refused then most likely the cnc server isn't running
            }
//            else
//            {
//                statusBox->setTextColor("green");
//                statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      _     cnc")); // + QString::fromStdString(temp) ); //+ "CNCHub connected");
//                statusBox->append("");
//            }
        }
    }
    else
    {
        statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Not Connected to Network");
        testConnectionButton->setText("Not connected");
        testConnectionButton->setStyleSheet("* { background-color: rgb(255,125,100) }");
    }
/*
    // Poll for new Header file
    if ((connectionManager.isConnected()) && (client.isServerConnected()))
    {
        struct timeval tv;

        // Read Header file
        ifstream headerFile;
        headerFile.open("NeXtRAD_Header.txt");
        printf("Header File opened\n");
        while(!headerFile.open())
        {
            tv.tv_usec = 200; //delay
        }
        headerFile.close();
        CountDownButtonClicked ();

    }
*/
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
// startVideoRecordingCountDown()
//=======================================================================
//This method parses the start and end times for the video recording and starts the countdown starttimer
void Window::CountDownButtonClicked(void)
{
    //Read start and end times from header file
    ifstream headerFile;
    headerFile.open("NeXtRAD_Header.txt");
    printf("Header File opened\n");
    string temp;
    string timestamp;

    string cnctimestamp = client.getTimeStamp().substr(0,19); //chop off weird ASCII values
    printf("cnctimestamp = %s",cnctimestamp.c_str());
    printf("\n");
    while(!headerFile.eof())
    {
        getline(headerFile,temp);
        if(temp.substr(0,9) == "StartTime")     //looking for the "StartTime" in the header file
        {
            if (temp.substr(9,3) == " = ")
            {
                startTime = temp.substr(12,19);
            }
            else if ((temp.substr(9,2) == " =") || (temp.substr(9,2) == "= "))
            {
                startTime = temp.substr(10,19);
            }
            else if (temp.substr(9,1) == "=")
            {
                startTime = temp.substr(11,19);
            }
            printf("startTime = %s\n", startTime.c_str());
        }
        else if(temp.substr(0,7) == "EndTime")  //looking for the "EndTime" in the header file
        {
            if (temp.substr(7,3) == " = ")
            {
                endTime = temp.substr(10,19);
            }
            else if ((temp.substr(7,2) == " =") || (temp.substr(7,2) == "= "))
            {
                endTime = temp.substr(9,19);
            }
            else if (temp.substr(7,1) == "=")
            {
                endTime = temp.substr(8,19);
            }
            printf("endTime = %s\n", endTime.c_str());
		}
        else if(temp.find("TimeStamp") != string::npos)
        {
            if (temp.substr(9,3) == " = ")
            {
                timestamp = temp.substr(12,19);
            }
            else if ((temp.substr(9,2) == " =") || (temp.substr(9,2) == "= "))
            {
                timestamp = temp.substr(10,19);
            }
            else if (temp.substr(9,1) == "=")
            {
                timestamp = temp.substr(11,19);
            }
            printf("timestamp = %s\n", timestamp.c_str());
        }
    }

    headerFile.close();

    if(strcmp(stringToCharPntr(timestamp),stringToCharPntr(cnctimestamp)) == 0)
    {
        statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Header file matches CnC header file!");
    }
    else
    {
        statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "CnC must send latest Header File");
        return;
    }

    // Get Unix time (seconds since 01/01/1970)
    currentUnixTime = time(NULL);
    strtUnixTime = convertToUnixTime(startTime)+2;  // 2016-10-19 15:22:00 +2  = 146883322
    stopUnixTime = convertToUnixTime(endTime)+4;    // The camera has a 2 second buffer which is why 2 seconds were added to start and 4 to end time
    cout << "\n currentUnixTime " << currentUnixTime << "\n strtUnixTime " << strtUnixTime  << "\n stopUnixTime " << stopUnixTime << endl;

    // Checking if the start and end times are in the past or not
    if(strtUnixTime < currentUnixTime)
    {
        statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Please use a future start time");
    }
    else if(stopUnixTime < strtUnixTime)
    {
        statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Please use a future stop time");
    }
    else
    {
        starttimer->start((convertToUnixTime(startTime)+ 2 - currentUnixTime)*1000);
        //camera buffer stores 2 seconds, thus start two seconds later and record for 2 seconds longer.
        countDownLabel->setText("Video recording starts in...");
        timMode = 1;
        statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Countdown to video recording started");
    }
 

    fflush(stdout);
}

//=======================================================================
// startRecording()
//=======================================================================
//Method to start video recording and starts the countdown until the end of experiment
void Window::startRecording(void)
{
    timMode = 2;
    videoRecorder.startRecording();
    endtimer->start((stopUnixTime - currentUnixTime)*1000);
    statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Video recording started");
    countDownLabel->setText("Video recording ends in...");
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
    timMode = 0;
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


//=======================================================================
// convertToUnixTime()
//=======================================================================
//Method to convert time to 'yyyy-MM-dd hh:mm:ss' format then to data type time_t
time_t Window::convertToUnixTime(string time)
{
    string str = time;
    string s   = "";

    // Convert the date from 'dd-MM-yyyy hh:mm:ss' to 'yyyy-MM-dd hh:mm:ss' format
    if ((str.substr(2,1) == "-") && (str.substr(5,1) == "-") && (str.substr(10,1) == " "))
    {
        s.assign(str,6,4);  // yyyy
        s.append(str,2,4);  // yyyy-MM-
        s.append(str,0,2);  // yyyy-MM-dd
        s.append(str,10,9); // yyyy-MM-dd hh:mm:ss
    }
    // Take as is if the date in 'yyyy-MM-dd hh:mm:ss' format
    else if ((str.substr(4,1) == "-") && (str.substr(7,1) == "-") && (str.substr(10,1) == " "))
    {
        s = str;
    }
    //cout << "s is : " << s << endl;
    
    // Convert the date from 'yyyy-MM-dd hh:mm:ss' format to get correct Unix time!
    struct tm tm;
    strptime(s.c_str(), "%F %T", &tm);   //%F = yyyy-MM-dd; %T = hh:mm:ss
    return mktime(&tm);
}

