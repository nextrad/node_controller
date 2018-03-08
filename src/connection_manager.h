//Author:               Darryn Jordan
//Edited By:            David Bissett
//Class:                ConnectionManager
//For:                  University of Cape Town, Dept. Elec. Eng., RRSG NeXtRAD
//Created:              November 2015
//Last Edited:          April 2016

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include "includes.h"
#include "parameters.h"
#include "video_connection_manager.h"

//enum WritePrivacy {PUBLIC, PRIVATE};

class ConnectionManager
{
    private:
        char socketBuffer[SOCKET_BUFFER_SIZE];
        io_service service;
        ip::tcp::socket sockets;
        ip::tcp::endpoint remoteEndpoint;
        string password;
        bool connection;

    public:
        ConnectionManager(void);
        void writeToSocket(string, WritePrivacy);
        void readFromSocket(void);
        void clearSocketBuffer(void);
        void connectToSocket(void);
        void connectionTest(void);
        string getTimeAndDate(const char *);
        bool isConnected();
};

#endif // CONNECTIONMANAGER_H
