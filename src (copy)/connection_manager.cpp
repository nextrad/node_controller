//Author:               Darryn Jordan
//Edited By:            David Bissett
//Class:                NetworkManager
//For:                  University of Cape Town, Dept. Elec. Eng., RRSG NeXtRAD
//Created:              November 2015
//Last Edited:          April 2016

#include "connection_manager.h"
#include "includes.h"
#include "parameters.h"
#include <boost/asio.hpp>

ConnectionManager::ConnectionManager(void)  : sockets(service)
{
    remoteEndpoint.address(ip::address::from_string(CNC_ADDRESS));
    remoteEndpoint.port(SSH_PORT);
    connection = false;
}

void ConnectionManager::connectionTest(void)
{
    u_short port;                /* user specified port number */
    char *addr;                  /* will be a pointer to the address */
    struct sockaddr_in address;  /* the libc network address data structure */
    short int sock = -1;         /* file descriptor for the network socket */
    fd_set fdset;
    struct timeval tv;

    port = SSH_PORT;
    addr = CNC_ADDRESS;

    address.sin_family = AF_INET;               //Set to IP_V4
    address.sin_addr.s_addr = inet_addr(addr); /* assign the address */
    address.sin_port = htons(port);            /* translate int2port num */

    sock = socket(AF_INET, SOCK_STREAM, 0);
    //For some reason there's no default way to set the timeout on socket connections.
    //To get around this you have unblock it so that you can set a shorter timeout using select()
    fcntl(sock, F_SETFL, O_NONBLOCK);

    connect(sock, (struct sockaddr *)&address, sizeof(address));

    FD_ZERO(&fdset);
    FD_SET(sock, &fdset);
    tv.tv_sec = 2;             /* 2 second timeout */
    tv.tv_usec = 0;

    if (select(sock + 1, NULL, &fdset, NULL, &tv) == 1)
    {
        int so_error;
        socklen_t len = sizeof so_error;
        getsockopt(sock, SOL_SOCKET, SO_ERROR, &so_error, &len);

        if (so_error == 0) {
            connection = true;
        }
        else
        {
            connection = false;
        }
    }
    close(sock);
}

bool ConnectionManager::isConnected()
{
    return connection;
}
