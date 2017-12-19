//Author:       Shirley Coetzee
//File:         header_arm_files.h
//For:          University of Cape Town, Dept. Elec. Eng., RRSG NeXtRAD
//Created:      December 2017

#ifndef HEADERARMFILES_H
#define HEADERARMFILES_H

#include <QDateTime>
#include "includes.h"
#include "parameters.h"

class HeaderArmFiles
{
    public:
        HeaderArmFiles(void);
        void setHeaderFileName(string newname);
        string getHeaderFileName();
        string getHeaderPathName();
        string getArmtimecfgFileName();
        string getArmtimecfgPathName();
        void writeToHeaderFile(string varName, string data, string section);
        QString readFromHeaderFile(string varName, string section);
        int renameHeaderFile(string newname);
        void writeToArmtimecfgFile(string data);

    private:
        string headerFileName;
        string headerPathName;
        string armtimecfgFileName;
        string armtimecfgPathName;


};


#endif // HEADERARMFILES_H
