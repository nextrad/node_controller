//Author:               Darryn Jordan
//Edited By:            David Bissett
//Class:                VideoConnectionManager
//For:                  University of Cape Town, Dept. Elec. Eng., RRSG NeXtRAD
//Created:              November 2015
//Last Edited:          April 2016

#ifndef VIDEOConnectionManager_H
#define VIDEOConnectionManager_H

#include "includes.h"
#include "parameters.h"
#include <QTimer>

enum WritePrivacy {PUBLIC, PRIVATE};

class VideoConnectionManager
{

    private:
        char socketBuffer[SOCKET_BUFFER_SIZE];
        io_service service;
        ip::tcp::socket socket;
        ip::tcp::endpoint socketEndpoint;
        string password;
        string ipCameraAddress;
        string cameraOverlayConfig;
        string recFilePath;

    public:
        VideoConnectionManager(void);
        void writeToSocket(string, WritePrivacy);
        void readFromSocket(void);
        void clearSocketBuffer(void);
        void connectToSocket(void);
        void configureVideoStream(void);
        void startRecording();
        string getTimeAndDate(const char *);
        string getRecFilePath();
        void setRecFilePath(string path, string filename);

    public slots:
        void stopRecording(void);

};

#endif // VIDEOConnectionManager_H
