//Author:           David Bissett
//Edited By:        Shirley Coetzee
//Class:            Main
//For:              Radar Group NeXtRAD
//Created:          April 2016
//Last  Edited:     January 2017

#include <QApplication>
#include <QPushButton>
#include "window.h"
#include "includes.h"
#include "parameters.h"
#include "video_connection_manager.h"

int NODE_ID;
string RTSP_HOST;
string OUTPUT_DIRECTORY;
string OVERLAY_IMAGE;

void testNTP(void);
void setParameters(void);

int main(int argc, char **argv)
{
    testNTP();
    setParameters();

    QApplication app (argc, argv);
    QIcon icon("favicon2.ico");
    Window window;
    window.setWindowIcon(icon); //Set Icon for application
    window.show();

    app.exec();
    return 0;

}

void testNTP(void)
{
    system("sudo service ntp status > ntpStatus.txt"); //write the status of the NTP server to ntpStatus.txt

    //read status from text file
    ifstream ntpstatus ("ntpStatus.txt");
    printf("\nntpStatus.txt opened\n");
    string temp;
    getline(ntpstatus,temp);
    printf(temp.c_str());
    printf("\n");

    if(temp.find("not running") != string::npos) //if it finds the substring 'not running'
    {
        system("sudo service ntp start");
        printf("\n");
    }
    printf("\n");
}

void setParameters(void)
{
    int nodeNo;
    bool gotDetailsOK = false;


    while (!gotDetailsOK)
    {
        cout << "\nEnter Node no (0, 1 or 2) : ";
        cin >> nodeNo;
        cout << endl;

        OVERLAY_IMAGE = "/home/nextrad/Documents/Video/overlay_images/crosshair_1080p.png";
        OUTPUT_DIRECTORY = "/home/nextrad/Documents/Video/output/";

        switch (nodeNo)
        {
            case 0:
                NODE_ID = 0;
                RTSP_HOST = "192.168.1.14";
                gotDetailsOK = true;
                break;
            case 1:
                NODE_ID = 1;
                RTSP_HOST = "192.168.1.24";
                gotDetailsOK = true;
                break;
            case 2:
                NODE_ID = 2;
                RTSP_HOST = "192.168.1.34";
                gotDetailsOK = true;
                break;
            default:
                cout << "Entered value is unknown." << endl;
        }
    }
}
