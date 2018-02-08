//Author:       Shirley Coetzee
//File:         header.h
//For:          University of Cape Town, Dept. Elec. Eng., RRSG NeXtRAD
//Created:      December 2017

#ifndef HEADER_H
#define HEADER_H

#include <QDateTime>
#include "includes.h"
#include "parameters.h"
#include "SimpleIni.h"


class HeaderArmFiles
{
    public:
        HeaderArmFiles(void);

        void writeToHeaderFile(string section, string key, string value);
        QString readFromHeaderFile(string section, string var);
        string readFromGPSInfoFile(string var);
        string readFromBearingsFile(string var, int offset, int strsize);

    private:
};


#endif // HEADER_H