//Author:               Darryn Jordan
//Edited By:            David Bissett
//Class:                VideoConnectionManager
//For:                  University of Cape Town, Dept. Elec. Eng., RRSG NeXtRAD
//Created:              November 2015
//Last Edited:          April 2016

#include "video_connection_manager.h"

extern string OUTPUT_DIRECTORY;
extern string RTSP_HOST;
extern string OVERLAY_IMAGE1;
extern string OVERLAY_IMAGE2;
extern string OVERLAY_IMAGE3;
extern string OVERLAY_IMAGE4;



VideoConnectionManager::VideoConnectionManager(void)  : socket(service)
{
    socketEndpoint.address(ip::address::from_string(TELNET_HOST));
    socketEndpoint.port(TELNET_PORT);
    string cameraProfile;

    switch(CAMERA_RESOLUTION)
    {
        case 1080:
        {
            cameraOverlayConfig = "sfilter=logo{file=" + OVERLAY_IMAGE1 + "},";
            cameraProfile = "profile1";
            break;
        }

        case 720:
        {
            cameraOverlayConfig = "sfilter=logo{file=" + OVERLAY_IMAGE2 + "},";
            cameraProfile = "profile2";
            break;
        }

        case 480:
        {
            cameraOverlayConfig = "sfilter=logo{file=" + OVERLAY_IMAGE3 + "},";
            cameraProfile = "profile3";
            break;
        }

        case 240:
        {
            cameraOverlayConfig = "sfilter=logo{file=" + OVERLAY_IMAGE4 + "},";
            cameraProfile = "profile3";
            break;
        }
    }


    ipCameraAddress = (string)"rtsp://" + (string)RTSP_HOST + (string)":" + (string)RTSP_PORT + (string)"/live/video/" + cameraProfile;
}


void VideoConnectionManager::clearSocketBuffer(void)
{
    for (int i = 0; i < SOCKET_BUFFER_SIZE; i++)
    {
        socketBuffer[i] = ' ';
    }
}


void VideoConnectionManager::connectToSocket(void)
{
    int attempt = 0;

    cout << "Attempting to establish connection to VLC server..." << endl;
    try
    {
        socket.connect(socketEndpoint); //attempt to connect to the endpoint, if no exception is thrown the connection is successful
        printf("Connection Active!\n");
        writeToSocket(TELNET_PASSWORD, PRIVATE); //enter the vlc server password
    }
    catch (boost::system::system_error const& e) //exception was thrown, connection failed
    {
        if (attempt < 5)
        {
            //caught an exception
            //currently prints the error if an exception has been caught, sleeps and tries again
            cout << "Attempt: " << attempt << ", error: could not " << e.what() << endl;
            sleep(3);
            connectToSocket();
            attempt++;
        }
    }
}


void VideoConnectionManager::writeToSocket(string message, WritePrivacy privacy)
{
    socket.write_some(buffer(message + (string)"\n")); //write to the socket

    if (privacy == PUBLIC)
    {
        cout << " ~ " << message << endl;   //echo to console
    }
}


void VideoConnectionManager::readFromSocket(void)
{
    clearSocketBuffer();
    socket.read_some(buffer(socketBuffer)); //read terminal response to the buffer array
    cout << socketBuffer << endl;         	//echo to console
}


string VideoConnectionManager::getTimeAndDate(const char *format)
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer,80,format,timeinfo);
    string dateTime(buffer);
    return dateTime;
}


void VideoConnectionManager::configureVideoStream(void)
{
    printf("Configuring RTSP stream...\n");

    setRecFilePath(OUTPUT_DIRECTORY, getTimeAndDate("%d_%m_%y_%I:%M:%S"));

    writeToSocket("del stream", PUBLIC);        //delete any previous instances of 'stream' if they exist
    writeToSocket("new stream broadcast enabled", PUBLIC); //create a new instance of 'stream' and enable it
    writeToSocket("setup stream input " + ipCameraAddress, PUBLIC); //set stream input
    writeToSocket("setup stream output #transcode{" + cameraOverlayConfig + "vcodec=h264,acodec=none}:std{access=file,mux=mp4,dst=" + getRecFilePath() + ".mp4}", PUBLIC); //setup the output type, encoding format and filename
    printf("-----------------------------\n");
}


void VideoConnectionManager::startRecording()
{
    printf("\nRecording video ...\n\n");
    writeToSocket("control stream play", PUBLIC); //start video recording
    cout << endl;
}


void VideoConnectionManager::stopRecording(void)
{
    printf("Recording video stopped!\n");
    writeToSocket("control stream stop", PUBLIC); //stop video recording
    printf("-----------------------------\n\n");
}

string VideoConnectionManager::getRecFilePath()
{
    return recFilePath;
}


void VideoConnectionManager::setRecFilePath(string path, string filename)
{
    recFilePath = path + filename;
}


