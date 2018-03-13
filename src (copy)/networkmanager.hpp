//Author:               David Bissett
//Class:                NetworkManager
//For:                  University of Cape Town, Dept. Elec. Eng., RRSG NeXtRAD
//Created:              May 2016
//Last Edited:          April 2016

#ifndef NETWORKMANAGER_HPP
#define NETWORKMANAGER_HPP

#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include <stdint.h> //int16_t
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <QWidget>
#include <sstream>
#include "parameters.h"

using namespace std;

class NetworkManager
{
    private:
        char* serverAddress;
        int portNumber;
        struct sockaddr_in server, client, cnc;
        int sock, server_sock, client_sock, c;
        bool serverConnected, indCompConnected;
        std::thread recvPosThread;
        double gpsLat, gpsLon, gpsHt;
        int nodeID;

    public:
        NetworkManager();
        ~NetworkManager();
        void sendData(double nodeID, double Lat, double Lon, double Ht);
        void sendIndCompFile(char* hFile[], int8_t len[]);
        void receiveData();
        char* stringToCharPntr(string str);
        void connectToCNC();
        bool isServerConnected();
        void receivePosition(void);
        bool isIndCompConnected();
        string getTimeStamp();
};

#endif // NETWORKMANAGER_HPP
