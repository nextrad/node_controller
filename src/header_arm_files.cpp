//Class:        Datetime
//For:          University of Cape Town, Dept. Elec. Eng., RRSG NeXtRAD
//Author:       Shirley Coetzee
//Created:      December 2017
//Version       1.0 (December 2017)
//Edited By:    Shirley Coetzee, Darryn Jordan and Simon Lewis
//Revision:     2.0 (Jan 2018)
//Edited By:    Shirley Coetzee
//Revision      3.0 (Oct 2018)


#include "header_arm_files.h"



HeaderArmFiles::HeaderArmFiles(void)
{

}

//=============================================================================
// writeToHeaderFile()
//=============================================================================
//method to set a variable's value in the NeXtRAD header file
void HeaderArmFiles::writeToHeaderFile(string section, string key, string value)
{
    std::ifstream check (HEADER_PATH);
    if (!check.good())
    {
        printf("Please check location of header file and try again.\n");
        exit(EXIT_FAILURE);
    }

    CSimpleIniA ini;

    ini.LoadFile(HEADER_PATH);

    ini.SetValue(section.c_str(),key.c_str(),value.c_str());

    ini.SaveFile(HEADER_PATH);

    check.close();
}


//=============================================================================
// readFromHeaderFile()
//=============================================================================
//method to return a variable's value from the NeXtRAD header file
QString HeaderArmFiles::readFromHeaderFile(string section, string var)
{
    //Read from header file
    std::ifstream check(HEADER_PATH);
    if (!check.good())
    {
        printf("Please check location of header file and try again.\n");
        exit(EXIT_FAILURE);
    }

    CSimpleIniA ini;

    ini.LoadFile(HEADER_PATH);

    std::string value = (ini.GetValue(section.c_str(), var.c_str()));

    check.close();

    return  QString::fromUtf8(value.c_str());
}

//=============================================================================
// readFromGPSInfoFile()
//=============================================================================
//method to return a variable's value from a GPSInfo file
string HeaderArmFiles::readFromGPSInfoFile(string var)
{
    string path;

    path = NODE_GPS_INFO_PATH;

    //Read from header file
    std::ifstream check (path);
    if (check.good() != 1)
    {
        printf("Please check location of gps info file and try again.\n");
        return "Fault";
    }

    CSimpleIniA ini;

    ini.LoadFile(path.c_str());

    std::string value = (ini.GetValue("", var.c_str()));

    check.close();

    return value;
}
