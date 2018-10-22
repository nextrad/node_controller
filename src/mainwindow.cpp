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
//Revision:             6.0 (Feb/Mar 2018)
//Edited By:            Shirley Coetzee
//Revision              7.0 (Oct 2018)



//=======================================================================
// Includes
//=======================================================================

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QApplication>
#include <QDateTime>

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

    attempt = 0;
    experiment_state = INACTIVE; //see enum for explanation

    ui->setupUi(this);

    stringstream ss;
    ss << "Node " << (int)NODE_ID;
    ui->label->setText(stringToCharPntr(ss.str()));

    ui->Countdown->display("00:00:00");

    //start vlc telnet server, connect and configure
    int status = system("x-terminal-emulator -e \"vlc -I telnet\n\"");
    if (0 != status)
    {
        cout << "x-terminal-emulator failed to start." << endl;
    }
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

    //the count down timer calls updateCountDownLCDAndPollHeaderFile() every second
    countdowntimer = new QTimer(this);
    countdowntimer->setInterval(1000);
    connect(countdowntimer, SIGNAL(timeout()), this, SLOT(updateCountDownLCDAndPollHeaderFile()));
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
// on_testConnectionButton_clicked()
// Tests the connection to NeXtRAD network
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


    testSubNetwork("1");
    testSubNetwork("2");
    testSubNetwork("3");

    ui->statusBox->setTextColor("black");
    ui->countdownLabel->setText("");
}

//=============================================================================
// testSubNetwork()
// Tests the connections to CNC
//=============================================================================
void MainWindow::testSubNetwork(QString NetID)
{
    string temp, name;
    string address = "192.168.1.";
    address.append(NetID.toUtf8().constData());
    NetID = QString::number(NetID.toInt() - 1);    //This is because the node numbering actually starts from zero.

    //Test if Access Point bullet is connected
    name = "ap";
    name.append(NetID.toUtf8().constData());
    temp = address;
    temp.append("2");
    if(!testConnection(temp))
    {
        ui->statusBox->setTextColor("red");
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      X     ") + QString::fromStdString(name) );
    }
    else
    {
        ui->statusBox->setTextColor("green");
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      _     ") + QString::fromStdString(name) );
    }

    //Test if STAtion bullet is connected
    name = "sta";
    name.append(NetID.toUtf8().constData());
    temp = address;
    temp.append("3");
    if(!testConnection(temp))
    {
        ui->statusBox->setTextColor("red");
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      X     ") + QString::fromStdString(name) );
    }
    else
    {
        ui->statusBox->setTextColor("green");
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      _     ") + QString::fromStdString(name) );
    }

    //Test if PoE Switch is connected
    name = "switch";
    name.append(NetID.toUtf8().constData());
    temp = address;
    temp.append("0");
    if(!testConnection(temp))
    {
        ui->statusBox->setTextColor("red");
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      X     ") + QString::fromStdString(name) );
    }
    else
    {
        ui->statusBox->setTextColor("green");
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      _     ") + QString::fromStdString(name) );
    }

    //Test if Node Laptop is connected
    name = "node";
    name.append(NetID.toUtf8().constData());
    temp = address;
    temp.append("1");
    if(!testConnection(temp))
    {
        ui->statusBox->setTextColor("red");
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      X     ") + QString::fromStdString(name) );
    }
    else
    {
        ui->statusBox->setTextColor("green");
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      _     ") + QString::fromStdString(name) );
    }

    //Test if IP camera is connected
    name = "cam";
    name.append(NetID.toUtf8().constData());
    temp = address;
    temp.append("4");
    if(!testConnection(temp))                   //Cameras have no ssh port and more security so it's easier to ping them
    {
        ui->statusBox->setTextColor("red");
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      X     ") + QString::fromStdString(name) );
    }
    else
    {
        ui->statusBox->setTextColor("green");
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      _     ") + QString::fromStdString(name) );
    }

    //Test if Cobalt is connected
    name = "cobalt";
    name.append(NetID.toUtf8().constData());
    temp = address;
    temp.append("5");
    if(!testConnection(temp))
    {
        ui->statusBox->setTextColor("red");
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      X     ") + QString::fromStdString(name) );
    }
    else
    {
        ui->statusBox->setTextColor("green");
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      _     ") + QString::fromStdString(name) );
    }

    //Test if TCUs is connected
    name = "tcu";
    name.append(NetID.toUtf8().constData());
    temp = address;
    temp.append("6");
    if(!testConnection(temp))
    {
        ui->statusBox->setTextColor("red");
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      X     ") + QString::fromStdString(name) );
    }
    else
    {
        ui->statusBox->setTextColor("green");
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      _     ") + QString::fromStdString(name) );
    }

    ui->statusBox->append("");

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
            ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      X     ") + "node" + QString::number(node_num));
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
            ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      _     ") + "node" + QString::number(node_num) + "\n" \
                        + "lat=" + QString::fromStdString(lat) + "                    long=" + QString::fromStdString(lon) + "\n" \
                        + "ht=" + QString::fromStdString(ht));
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
// on_calcBearingsButton_clicked()
// Checks if received latest Header File
// If so, calls calcBearings(NODE_ID);
//=============================================================================
void MainWindow::on_calcBearingsButton_clicked()
{
    ui->statusBox->append("");
    ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Reading Header File for target and node locations");
    ui->statusBox->append("");

    if (experiment_state != INACTIVE)
    {
        calcBearings(NODE_ID);
    }
    else
    {
        ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Header File not recent");
        ui->statusBox->append("");
    }

    ui->statusBox->append("");
    ui->statusBox->setTextColor("black");
    ui->countdownLabel->setText("");
}

//=============================================================================
// calcBearings()
// Calculate bearing from node position and target position.
// e.g.
// p1 = new LatLon(-34.19269, 18.44571);
// p2 = new LatLon(-34.18126, 18.46009);
// b2 = p1.bearingTo(p2); // 46.14°
//
//=============================================================================
void MainWindow::calcBearings(int node_num)
{
    QString nodelat, nodelon, tgtlat, tgtlon;
    double brg;

    try
    {
        // Get node position from Header file
        switch (node_num)
        {
            case 0: nodelat = headerarmfiles.readFromHeaderFile("GeometrySettings", "NODE0_LOCATION_LAT");
                    nodelon = headerarmfiles.readFromHeaderFile("GeometrySettings", "NODE0_LOCATION_LON");
                    break;
            case 1: nodelat = headerarmfiles.readFromHeaderFile("GeometrySettings", "NODE1_LOCATION_LAT");
                    nodelon = headerarmfiles.readFromHeaderFile("GeometrySettings", "NODE1_LOCATION_LON");
                    break;
            case 2: nodelat = headerarmfiles.readFromHeaderFile("GeometrySettings", "NODE2_LOCATION_LAT");
                    nodelon = headerarmfiles.readFromHeaderFile("GeometrySettings", "NODE2_LOCATION_LON");
                    break;
        }

        // Get node position from Header file
        tgtlat = headerarmfiles.readFromHeaderFile("TargetSettings", "TGT_LOCATION_LAT");
        tgtlon = headerarmfiles.readFromHeaderFile("TargetSettings", "TGT_LOCATION_LON");

        if ((nodelat == "Fault") || (nodelon == "Fault") || (tgtlat == "Fault") || (tgtlon == "Fault"))
        {
            // Display data on screen in red X per node
            ui->statusBox->setTextColor("red");
            ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      X     ") + "\tnode" + QString::number(node_num));
        }
        else
        {
            // Display data on screen in green values per node
            ui->statusBox->setTextColor("green");
            ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      _     ") + "node" + QString::number(node_num) + "\n" \
                        + "lat=" + nodelat + "                    long=" + nodelon);
            ui->statusBox->append("");
            ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      _     ") + "target" + "\n" \
                        + "lat=" + tgtlat + "                    long=" + tgtlon);
            ui->statusBox->append("");

            Point node, target;
            node.lat = nodelat.toDouble();
            node.lon = nodelon.toDouble();
            target.lat = tgtlat.toDouble();
            target.lon = tgtlon.toDouble();

            brg = bearingTo(node, target);

            ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm      _     \n") + "bearing=" + QString::number(brg, 'f', 4));        }

        ui->statusBox->append("");
        ui->statusBox->setTextColor("black");
        ui->countdownLabel->setText("");
    }
    catch(exception &e)
    {
        cout << "calcBearings exception: " << e.what() << endl;
    }
}


//=======================================================================
// Returns the radians from the input measured in degrees.
//=======================================================================

double MainWindow::toRadians (double degs) {
        return degs * M_PI / 180;
}


//=======================================================================
// Returns the degrees from the input measured in radians.
//=======================================================================

double MainWindow::toDegrees (double rads) {
    return rads * 180 / M_PI;
}


//=======================================================================
//
// Returns the (initial) bearing from ‘this’ point to destination point.
//
// @param   {LatLon} point - Latitude/longitude of destination point.
// @returns {number} Initial bearing in degrees from north.
//
// @example
//     var p1 = new LatLon(52.205, 0.119);
//     var p2 = new LatLon(48.857, 2.351);
//     var b1 = p1.bearingTo(p2); // 156.2°
//
//=======================================================================

double MainWindow::bearingTo(Point here, Point there)
{
    // tanθ = sinΔλ⋅cosφ2 / cosφ1⋅sinφ2 − sinφ1⋅cosφ2⋅cosΔλ
    // see mathforum.org/library/drmath/view/55417.html for derivation

    double ang1 = toRadians(here.lat);
    double ang2 = toRadians(there.lat);
    double londiff = toRadians(there.lon-here.lon);
    double y = sin(londiff) * cos(ang2);
    double x = cos(ang1)*sin(ang2) -
            sin(ang1)*cos(ang2)*cos(londiff);
    double brg = atan2(y, x);

    return std::fmod((toDegrees(brg)+360), 360);
}


//=======================================================================
// Returns final bearing arriving at destination destination point from ‘this’ point; the final bearing
// will differ from the initial bearing by varying degrees according to distance and latitude.
//
// @param   {LatLon} point - Latitude/longitude of destination point.
// @returns {number} Final bearing in degrees from north.
//
// @example
//     var p1 = new LatLon(52.205, 0.119);
//     var p2 = new LatLon(48.857, 2.351);
//     var b2 = p1.finalBearingTo(p2); // 157.9°
//=======================================================================

double MainWindow::finalBearingTo (Point here, Point there)
{
    // get initial bearing from destination point to this point & reverse it by adding 180°
    return std::fmod(( bearingTo(here, there)+180 ), 360);
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
    int status = system(stringToCharPntr(ss.str()));
    if (-1 != status)
    {
        cout << "Failed to show video." << endl;
    }
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
// checkForNewHeaderFile
// Checks if received new Header File
//=======================================================================
bool MainWindow::checkForNewHeaderFile(void)
{

    HeaderArmFiles headerarmfiles;
    QString daynew, hournew, minutenew, secondnew;

    attempt++;

    // Poll for new Header file
    ifstream headerFile (HEADER_PATH);
    headerFile.open(HEADER_PATH);
    if (headerFile.is_open())
    {
        // read armtime from Header File values
        daynew = headerarmfiles.readFromHeaderFile("Timing", "DAY");
        hournew = headerarmfiles.readFromHeaderFile("Timing", "HOUR");
        minutenew = headerarmfiles.readFromHeaderFile("Timing", "MINUTE");
        secondnew = headerarmfiles.readFromHeaderFile("Timing", "SECOND");

        headerFile.close();

        if ((daynew != dayold) || (hournew != hourold) ||
            (minutenew != minuteold) || (secondnew != secondold))
        {
            if (attempt > 1)
            {
                cout << "Received new Header File" << endl;
            }

            // Save datetime
            dayold = daynew;
            hourold = hournew;
            minuteold = minutenew;
            secondold = secondnew;

            return true;
        }
    }

    return false;
}

//=======================================================================
// updateCountDownLCDAndPollHeaderFile()
// Updates the countdown LCD every second depending on experiment_state.
// Also checks if received latest Header File
// If so, calls checkCountdown()
//=======================================================================
void MainWindow::updateCountDownLCDAndPollHeaderFile(void)
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


    if ((checkForNewHeaderFile() == true) && (attempt > 1))
    {
        checkCountdown();
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
    for (int i = 0; i < (int)str_in.length(); i++)
    {
        if (str_in[i] == ch_in)
        {
            str_in[i] = ch_out;
        }
    }
    return str_in;
}


//=============================================================================
// calcExperimentLength()
//
// Calc ExperimentLength from the NUM_PRIS, and the summing of each specific block in PULSES
//=============================================================================
int MainWindow::calcExperimentLength(void)
{
    int num_pris = atoi(headerarmfiles.readFromHeaderFile("PulseParameters", "NUM_PRIS").toStdString().c_str());

    string pulses_str = headerarmfiles.readFromHeaderFile("PulseParameters", "PULSES").toStdString();
    // e.g. PULSES = "5.0,1000.0,0,1300.0|10.0,2000.0,1,1300.0|10.0,3000.0,2,1300.0|10.0,4000.0,3,1300.0";

    // ====================================================

    // Split into blocks separated by "|", put into pulses_arr

    std::string s = pulses_str;
    std::string delimiter = "|";

    // Get pulses_arr
    vector <string> pulses_arr;

    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        pulses_arr.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    pulses_arr.push_back(s);

    int num_pulses_in_block = pulses_arr.size();

    for (int i=0; i<num_pulses_in_block; i++)
        cout << pulses_arr[i] << "\n";

    cout << "num_pulses_in_block = " << num_pulses_in_block << std::endl  << std::endl;

    // =======================================================

    // Split into values separated by ",", put into block_arr

    std::string::size_type sz;   // alias of size_t
    int tblock = 0;
    int i_dec;

    for (int i=0; i<num_pulses_in_block; i++)
    {

        std::string s = pulses_arr[i];
        std::string delimiter = ",";

        // Get block
        vector <string> block_arr;

        size_t pos = 0;
        std::string token;
        while ((pos = s.find(delimiter)) != std::string::npos) {
            token = s.substr(0, pos);
            block_arr.push_back(token);
            s.erase(0, pos + delimiter.length());
        }
        block_arr.push_back(s);

        int num_values_in_block = block_arr.size();

        for (int i=0; i<num_values_in_block; i++)
            cout << block_arr[i] << "\n";

        cout << "num_values_in_block = " << num_values_in_block << endl;


        i_dec = std::stoi (block_arr[1],&sz);
        cout << "i_dec = " << i_dec << endl << endl;

        tblock += i_dec;
    }

    cout << "tblock = " << tblock << endl;

    // =======================================================

    int num_rpts = num_pris / num_pulses_in_block;

    cout << "num_pris =" << num_pris << "\nnum_rpts = " << num_rpts << endl;

    int texperiment = tblock * num_rpts * 1e-6;  //  secs
                                                // num_pris * pri * 1e-6;  // = 60000 * 1000/1000000 = 60

    cout << "texperiment = " << texperiment << " secs" << endl;

    return texperiment;
}


//=============================================================================
// checkCountdown()
// This method parses the start and end times for the video recording,
// converts the times from dd-MM-yyyy hh:mm:ss to yyyy-MM-dd hh:mm:ss formats for timer and NTP
// If countdown time is valid, this method starts the countdown timer
//=============================================================================
bool MainWindow::checkCountdown(void)
{
    Datetime datetime;
    stringstream ss_unixtime;
    HeaderArmFiles headerarmfiles;

    try
    {
        // read armtime from Header File values
        QString year = headerarmfiles.readFromHeaderFile("Timing", "YEAR");
        QString month = headerarmfiles.readFromHeaderFile("Timing", "MONTH");
        QString day = headerarmfiles.readFromHeaderFile("Timing", "DAY");
        QString hour = headerarmfiles.readFromHeaderFile("Timing", "HOUR");
        QString minute = headerarmfiles.readFromHeaderFile("Timing", "MINUTE");
        QString second = headerarmfiles.readFromHeaderFile("Timing", "SECOND");

        cout << "checkCountdown1() = " << year.toStdString() << "-" << month.toStdString()<< "-" << day.toStdString();
        cout << " " << hour.toStdString() << ":" << minute.toStdString() << ":" << second.toStdString() << endl;

        // calculate EXPERIMENT_LENGTH from Header File values
        EXPERIMENT_LENGTH = calcExperimentLength();

        //required format: YYYY-MM-DD HH:MM:SS
        ss_unixtime << year.toStdString() << "-" << setfill('0') << setw(2) << month.toStdString() << "-" << setfill('0') << setw(2) << day.toStdString() << " ";
        ss_unixtime << hour.toStdString() << ":" << setfill('0') << setw(2) << minute.toStdString() << ":" << setfill('0') << setw(2) << second.toStdString();

        cout << "checkCountdown2() = " << ss_unixtime.str() << endl;

        struct tm tm1;
        tm1 = datetime.convertToStructTm(ss_unixtime.str());

        // Check for validity
        int iyear, imonth, iday;

        iyear = tm1.tm_year + 1900;
        imonth = tm1.tm_mon + 1;
        iday = tm1.tm_mday;

        cout << "checkCountdown3() = " << iyear << "-" << imonth << "-" << iday << endl;

        if (((imonth == 2) && (iday > 28) && (remainder (iyear, 4) != 0)) ||
            ((imonth == 2) && (iday > 29) && (remainder (iyear, 4) == 0)) ||
            (((imonth == 4) || (imonth == 6) || (imonth == 11))  && (iday > 30)))
        {
            ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Header File date is invalid, please correct it.");
            return false;
        }

        strtUnixTime = datetime.convertToUnixTime(tm1);
        stopUnixTime = strtUnixTime + EXPERIMENT_LENGTH;
        currentUnixTime = time(NULL);

        //check if the start/end times are in the past
        if(strtUnixTime < currentUnixTime)
        {
            ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Please use a future start time");
            return false;
        }
        else if(stopUnixTime < strtUnixTime)
        {
            ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Please use a future stop time");
            return false;
        }
        else // return true
        {
            // start countdown to armtime
            starttimer->start((strtUnixTime - currentUnixTime)*1000);
            cout << "armtime = " << (strtUnixTime - currentUnixTime)*1000 << endl;
            ui->countdownLabel->setText("Countdown to armtime");
            experiment_state = WAITING;
            ui->statusBox->append(QDateTime::currentDateTime().toString("dd-MM-yyyy hh:mm   ") + "Countdown to armtime");

            return true;

        }
    }
    catch(exception &e)
    {
        cout << "checkCountdown exception: " << e.what() << endl;
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

    if (startTime != "")
    {
        string command;
        command = "mv " + oldRecFileName + ".mp4 " + OUTPUT_DIRECTORY + newRecFileName + ".mp4";
        int status = system(command.c_str());
        cout << "Renamed video file to: " << newRecFileName << ".mp4, status = " << status << endl;
    }
}


