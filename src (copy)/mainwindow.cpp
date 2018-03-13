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

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QApplication>
#include <QDateTime>

bool newheaderfile;
int attempt;
QString dayold, hourold, minuteold, secondold;

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

    newheaderfile = false;
    attempt = 0;
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
// blah
//=======================================================================
void * checkForHeaderFile(void * threadarg)
{

    QString daynew, hournew, minutenew, secondnew;

    HeaderArmFiles *my_data = (HeaderArmFiles *)threadarg;

    cout << "Polling for header file, attempt: " << attempt++ << endl;

     // Poll for new Header file
    ifstream headerFile (HEADER_PATH);
    headerFile.open(HEADER_PATH);
    if (headerFile.is_open())
    {
        // read armtime from Header File values
        daynew = my_data->readFromHeaderFile("Timing", "DAY");
        hournew = my_data->readFromHeaderFile("Timing", "HOUR");
        minutenew = my_data->readFromHeaderFile("Timing", "MINUTE");
        secondnew = my_data->readFromHeaderFile("Timing", "SECOND");

//        cout << daynew.toStdString() << " " << hournew.toStdString() << " " << minutenew.toStdString() << " " << secondnew.toStdString() << " " << endl;
//        cout << dayold.toStdString() << " " << hourold.toStdString() << " " << minuteold.toStdString() << " " << secondold.toStdString() << " " << endl;

        headerFile.close();

        bool sametime = ((daynew == dayold) && (hournew == hourold) &&
                     (minutenew == minuteold) && (secondnew == secondold));
        //cout << "sametime = " << sametime << endl;
        if (!sametime)
        {
            cout << "Received Header File" << endl;
            newheaderfile = true;

            // Save datetime
            dayold = daynew;
            hourold = hournew;
            minuteold = minutenew;
            secondold = secondnew;
        }
    }
}


//=======================================================================
// blah
//=======================================================================
int MainWindow::createThreads()
{
    int rc;
    pthread_t thread_polling;
    pthread_attr_t attr;
    void *status;
    HeaderArmFiles headerarmfiles;

    // Initialize and set thread joinable
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    rc = pthread_create(&thread_polling, NULL, checkForHeaderFile, (void * )&headerarmfiles );
    if (rc)
    {
       cout << "Error:unable to create thread," << rc << endl;
       exit(-1);
    }

    // free attribute and wait for thread to finish
    pthread_attr_destroy(&attr);

    rc = pthread_join(thread_polling, &status);
    if (rc)
    {
      cout << "Error:unable to join," << rc << endl;
      exit(-1);
    }

    if (newheaderfile == true)
    {
        checkCountdown();
        newheaderfile = false;
    }
}

//=======================================================================
// connectionTestButtonClicked()
// Tests the connection to CnC
// and polls for Header File
//=======================================================================
void MainWindow::on_testConnectionButton_clicked()
{

    //Test if CnC is connected
    string name = "cnc";
    string address = "192.168.1.100";

    bool conn2CnC = testConnection(address);
    if (conn2CnC)
    {
        ui->statusBox->setTextColor("green");
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      _     ") + QString::fromStdString(name) );
    }
    else
    {
        ui->statusBox->setTextColor("red");
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      X     ") + QString::fromStdString(name) );
    }

    ui->statusBox->append("");
    ui->statusBox->setTextColor("black");


}

//=============================================================================
// testConnection()
// pings an address to see if it's connected to the network.
//=============================================================================
bool MainWindow::testConnection(string address)
{
    string command = "ping -c 1 ";  // where c = count
    command.append(address);

    int status = system(stringToCharPntr(command));
    if (-1 != status)
    {
        int ping_ret = WEXITSTATUS(status);

        if(ping_ret==0)
        {
            cout<<"Ping successful"<<endl; //Proceed
            return true;
        }
        else
        {
            cout<<"Ping not successful"<<endl; //Sleep and again check for ping
            return false;
        }
    }

    return false;
}

//=============================================================================
// stringToCharPntr()
// Takes in a string and converts it to char*
//=============================================================================
char* MainWindow::stringToCharPntr(string str)
{
    char *cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    return cstr;
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
    ui->countdownLabel->setText("");
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
        ui->statusBox->setTextColor("black");
        ui->countdownLabel->setText("");
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
    ui->countdownLabel->setText("");
}

//=============================================================================
// receiveBearings()

//  tardat2cc.rtf
// (*171207*)
// DTG	061855Z 1217
// Target Lat/Lon 	{-34.1813,18.46}
// n1: Range	1.82952
// n1: Bearing	46.5192
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
        ui->statusBox->setTextColor("black");
        ui->countdownLabel->setText("");
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
    ui->statusBox->setTextColor("black");
    ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Video recording aborted!");

}

//=======================================================================
// updateCountDownLCD()
// Updates the countdown LCD every second.
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

    createThreads();
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
/*
// This method opens the pedestal controller program.
// TODO At the moment this program hangs when the pedestal controller opens.
// This is because they are open on the same thread. Multithreading or forking is needed to fix this.

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
// CheckCountdown()
// Start countdown to armtime
//=======================================================================
//This method parses the start and end times for the video recording and starts the countdown starttimer
void MainWindow::checkCountdown(void)
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
        experiment_state = WAITING;
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Countdown to armtime");
        ui->countdownLabel->setText("Countdown to armtime");
    }

    ui->statusBox->append("");
    ui->statusBox->setTextColor("black");
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

    if (startTime != "")
    {
        string command;
        command = "mv " + oldRecFileName + ".mp4 " + OUTPUT_DIRECTORY + newRecFileName + ".mp4";
        system(command.c_str());
        cout << "Renamed video file to: " << newRecFileName << endl;
    }
}
