//Author:               Darryn Jordan
//Edited By:            David Bissett
//Edited By:            Shirley Coetzee
//File:                 parameters.h
//For:                  University of Cape Town, Dept. Elec. Eng., RRSG NeXtRAD
//Created:              November 2015
//Last Edited:          April 2016
//Last Edited:          January 2017

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <string.h>

#define SOCKET_BUFFER_SIZE 	300

#define CNC_ADDRESS         "192.168.1.100"
#define SSH_PORT            22
#define TELNET_HOST 		"127.0.0.1"
#define TELNET_PORT 		4212
#define TELNET_PASSWORD		"vlc"

#define RTSP_PORT			"88"

#define CAMERA_RESOLUTION 	1080

#define IS_MOSAIC_MODE		true


// parameters that are set in main.cpp
extern int NODE_ID;
extern std::string RTSP_HOST;
extern std::string OUTPUT_DIRECTORY;
extern std::string OVERLAY_IMAGE;

#endif // PARAMETERS_H
