//Author:               David Bissett
//Class:                NetworkManager
//For:                  University of Cape Town, Dept. Elec. Eng., RRSG NeXtRAD
//Created:              May 2016
//Last Edited:          April 2016

#include "networkmanager.hpp"

NetworkManager::NetworkManager()
{
    serverAddress = CNC_ADDRESS; //address of server to connect to (127.0.0.1 is localhost or the current machine)
    int portNumber = 8888; //portnumber of server to connect to
    serverConnected = false;
    indCompConnected = false;

//Create client socket that will be used to communicate with the CNC
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);

    if (sock == -1)
    {
        printf("Could not create client socket");
        return;
    }
    cout << "Client socket created" << endl;

    server.sin_addr.s_addr = inet_addr(serverAddress);
    server.sin_family = AF_INET;
    server.sin_port = htons( portNumber );
//-------------------------------------------------------------------------

//Create server socket that will receive node positions from gps
    server_sock = socket(AF_INET , SOCK_STREAM , 0);
    if (server_sock == -1)
    {
        printf("Could not create server socket");
        return;
    }
    cout << "Server socket created" << endl;

    client.sin_family = AF_INET;
    client.sin_addr.s_addr = INADDR_ANY;
    client.sin_port = htons( 1234 ); //Using port 1234

    //Bind socket
    if( bind(server_sock, (struct sockaddr *)&client , sizeof(client)) < 0)
    {
        //error
        perror("bind failed. Error");
        return;
    }
    printf("Bind done on port %d\n", 1234);

    //Create listener with a maximum of 2 connection
    listen(server_sock , 2);
    c = sizeof(struct sockaddr_in);
}

NetworkManager::~NetworkManager()
{
    cout << "Closing client." << endl;
    //finally we close the connection to the server
    shutdown(sock, SHUT_RDWR);
    close(sock);
    sock = 0;
    close(server_sock);
    server_sock = 0;
}

void NetworkManager::connectToCNC()
{
    printf("Connecting to server on %s using port %d \n", serverAddress, portNumber);
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        serverConnected = false;
        return;
    }
    serverConnected = true;

    int8_t dataSize = 1;
    int8_t data = NODE_ID;
    write(sock, &dataSize, sizeof(dataSize));
    write(sock, &data, dataSize);
    cout << "Connected" << endl;
}

void NetworkManager::sendData(double nodeID, double Lat, double Lon, double Ht)
{
    //create data to send (we use a specified 64bit int to confirm compatability)
    int64_t dataSize = 4; //keep track of the size of the array
    double data [4] = {nodeID, Lat, Lon, Ht};
    int8_t sizeOfData = 4;
    cout << "Sending array..." << endl;
    if(write(sock, &sizeOfData, sizeof(sizeOfData)) < 0)
    {
        cout << "Server Disconnected\n";
        serverConnected = false;
        close(sock);
        sock = -1;
        return;
    }
    //first we send the size of the array we are sending so the server knows the size of the array
    if(write(sock, &dataSize, sizeof(dataSize)) < 0)
    {
        cout << "Server Disconnected\n";
        serverConnected = false;
        close(sock);
        sock = -1;
        return;
    }
    //next we send the actual array
    if(write(sock, data, sizeof(data)) < 0)
    {
        cout << "Server Disconnected\n";
        serverConnected = false;
        close(sock);
        sock = -1;
        return;
    }
    cout << "Array sent." << endl;

    //send 'message received' acknowledgement (The ack that I've chosen is to echo the message which also serves as error checking
    if(recv(sock, &dataSize, sizeof(dataSize), 0) == 0)
    {
        cout << "Server Disconnected" << endl;
        serverConnected = false;
        close(sock);
        sock = -1;
        return;
    }
    else
    {

        if(recv(sock, &data, sizeof(data), 0) != 0)
        {
            cout << "Acknowledge Received" << endl;
            if(data[0] == nodeID && data[1] == Lat && data[2] == Lon && data[3] == Ht)
            {
                cout << "Acknowledge Correct" << endl;
            }
            else
            {
                cout << "Acknowledge Incorrect" << endl;
                cout << "Please Resend Data" << endl;
            }
        }
        else
        {
            cout << "Server Disconnected" << endl;
            serverConnected = false;
            close(sock);
            sock = -1;
            return;
        }
    }
    puts("Sent position");
}

/*
//This method is no longer used but can be if necessary. It receives each line of the Header file as a char*
//Due to the increased network traffic, this method caused a lot of problems. I opted to use SCP instead which
//is UBUNTU's built in file transfer command. It is all done on the CNC.
void NetworkManager::receiveData()
{
    fd_set rfds;
    struct timeval tv;

    tv.tv_sec = 20; //delay for timeout
    tv.tv_usec = 0;
    int rv;

    FILE * wfile;
    wfile = fopen("NeXtRAD Header.txt","w+");
    int8_t dataSize = 0;
    bool end_of_file = false;

    while(!end_of_file)
    {
        FD_ZERO(&rfds);         //clear the file descriptor set
        FD_SET(sock, &rfds);    //add file descriptor to set
        sleep(0.05);
        rv = select(sock + 1, &rfds, NULL, NULL, &tv);
        if(rv == 0)
        {
            cout << "CNC timed out\nTry to send header file again" << endl;
            end_of_file = true;
            break;
        }
        if(recv(sock, &dataSize, sizeof(dataSize), 0) != 0)
        {
            char data[dataSize];
            if(recv(sock, &data, sizeof(data), 0) == 0)
            {
                cout << "Server Disconnected" << endl;
                serverConnected = false;
                close(sock);
                sock = -1;
                return;
            }
            if(dataSize == 2)
            {
                if(data[0] == '~' && data[1] == '~')
                {
                    cout << "End of File\n";
                    end_of_file = true;
                    cout << "Header File received\n";
                    break;
                }
            }

            if(write(sock, &dataSize,sizeof(dataSize)) < 0)
            {
                cout << "Server Disconnected" << endl;
                serverConnected = false;
                close(sock);
                sock = -1;
                return;
            }
            if(write(sock, &data, sizeof(data)) < 0)
            {
                cout << "Server Disconnected" << endl;
                serverConnected = false;
                close(sock);
                sock = -1;
                return;
            }
            for(int j =0;j<dataSize;j++)
            {
                //printf("%c",data[j]);
                fprintf(wfile,"%c",data[j]);
            }
            fprintf(wfile,"\n");
        }
        else
        {
            cout << "Server Disconnected" << endl;
            serverConnected = false;
            close(sock);
            sock = -1;
            return;
        }
    }
    fclose(wfile);
}*/

bool NetworkManager::isIndCompConnected()
{
    return indCompConnected;
}

bool NetworkManager::isServerConnected()
{
    return serverConnected;
}

void NetworkManager::receivePosition(void)
{
    puts("Waiting for incoming client...");
    client_sock = accept(server_sock, (struct sockaddr * )&cnc, (socklen_t*)&c);

    if(client_sock < 0)
    {
        perror("Accept failed");
        return;     //May need better error handling
    }

    int64_t dataSize = 0;

    recv(client_sock, &dataSize, sizeof(dataSize), 0);

    double data [dataSize];

    recv(client_sock, &data, sizeof(data), 0);
    nodeID = data[0];
    gpsLat = data[1];
    gpsLon = data[2];
    gpsHt = data[3];
    puts("Sending Ack...");

    write(client_sock, &dataSize, sizeof(dataSize));
    write(client_sock, data, sizeof(dataSize));
    close(client_sock);
    puts("Ack Sent");
    sendData(nodeID, gpsLat, gpsLon, gpsHt);        //send to cnc
}

string NetworkManager::getTimeStamp()
{
    int8_t dataSize = 6;
    char data[6] = {};
    for(int i=0;i<6;i++)
    {
        data[i] = '~';
    }
    write(sock, &dataSize, sizeof(dataSize));
    write(sock, &data, dataSize);

    recv(sock, &dataSize, sizeof(dataSize), 0);

    char timestamp[dataSize];
    recv(sock, &timestamp[0], dataSize, 0);

    return std::string(timestamp);
}





void NetworkManager::sendIndCompFile(char* hFile [], int8_t len[])
{
    /*
    //accept connection from an incoming client
    //the code will wait on this line until a client connects...
    client_sock = accept(server_sock, (struct sockaddr *)&client, (socklen_t*)&c);
    puts("Client connecting!...");

    //if there's a problem the accept function return -1 so we check for that
    if (client_sock < 0)
    {
        perror("accept failed");
        return;
    }
    puts("Connection accepted");

    bool is_equal = false;
    std::vector<char> data;
    std::vector<char> ack;
    int8_t dataSize = 1;
    int error = 0;
    int n = 0;

    fd_set rfds;
    struct timeval tv;

    tv.tv_sec = 5;
    tv.tv_usec = 0;
    int rv;

    for(int i=0;i<108 && error != 4;i++)
    {
        is_equal = false;
        //char* data [len[i]];
        //while(!is_equal && error != 4)
        //{
            dataSize = len[i];
            for(int k=0;k<len[i];k++)
            {
                data.push_back(hFile[i][k]);
            }
            puts("vector populated");               //debugging
            n = write(client_sock, &dataSize, sizeof(dataSize));
            if(n<0)
            {
                puts("error writing to client");
                return;
            }
            n = write(client_sock, &data[0], len[i]);

            FD_ZERO(&rfds);
            FD_SET(client_sock, &rfds);
            sleep(0.05);
            rv = select(client_sock + 1, &rfds, NULL, NULL, &tv);
            if(rv == 0)
            {
                puts("client timed out");
                error = 4;
                break;
            }
            puts("no timeout");               //debugging

            n = recv(client_sock, &dataSize, sizeof(dataSize), 0);
            if(n==0)
            {
                puts("client disconnected");
                return;
            }
            else
            {
                puts("received datasize");
                n = recv(client_sock, &ack, dataSize, 0);
                puts("received line");               //debugging

                if(data.size() < ack.size())
                {
                    puts("checked difference");               //debugging
                    is_equal = std::equal(data.begin(), data.end(), ack.begin());
                    puts("checked is_equal");               //debugging
                }
                else
                {
                    puts("checked for difference");               //debugging
                    is_equal = std::equal(ack.begin(), ack.end(), data.begin());
                }
                is_equal = true;
                puts("checked is_equal");               //debugging
            }

            data.clear();
            ack.clear();
            puts("vectors cleared");               //debugging
        //}
    }
    dataSize = (int8_t)2;

    is_equal = false;
    while(!is_equal)
    {
        write(client_sock, &dataSize, sizeof(dataSize));
        write(client_sock, "~~", 2);

        FD_ZERO(&rfds);
        FD_SET(client_sock, &rfds);
        sleep(0.05);
        rv = select(client_sock + 1, &rfds, NULL, NULL, &tv);
        if(rv == 0)
        {
            puts("client timed out");
            error = 4;
            continue;
        }
        is_equal = true;
        n = recv(client_sock, &dataSize, sizeof(dataSize), 0);
        n = recv(client_sock, &data, dataSize, 0);
        puts("received eof");
    }


    puts("sent eof");               //debugging
    fflush(stdout);
    */


}

char* NetworkManager::stringToCharPntr(string str)
{
    char *cstr = new char[str.length() + 1];
    strcpy(cstr, str.c_str());
    return cstr;
}
