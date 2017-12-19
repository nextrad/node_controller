//Class:        Datetime
//For:          University of Cape Town, Dept. Elec. Eng., RRSG NeXtRAD
//Author:       Shirley Coetzee
//Created:      December 2017
//Version       1.0 (December 2017)


#include "includes.h"
#include "parameters.h"
#include "header_arm_files.h"


HeaderArmFiles::HeaderArmFiles(void)
{
    headerFileName = "NeXtRAD_Header.txt";
    headerPathName = "/home/nextrad/Documents/node_controller/NeXtRAD_Header.txt";

    armtimecfgFileName = "armtime.cfg";
    armtimecfgPathName = "/home/nextrad/Documents/node_controller/armtime.cfg";   // /home/nextrad/Desktop/NextGPSDO/armtime.cfg";
}

string HeaderArmFiles::getHeaderFileName()
{
    return headerFileName;
}

string HeaderArmFiles::getHeaderPathName()
{
    return headerPathName;
}

void HeaderArmFiles::setHeaderFileName(string newname)
{
    headerFileName =  newname;
}

string HeaderArmFiles::getArmtimecfgFileName()
{
    return armtimecfgFileName;
}

string HeaderArmFiles::getArmtimecfgPathName()
{
    return armtimecfgPathName;
}


//=============================================================================
// writeToHeaderFile()
//=============================================================================
//method to set a variable's value in the NeXtRAD header file
void HeaderArmFiles::writeToHeaderFile(string varName, string data, string section)
{
    //Read Header File
    ifstream rfile (getHeaderFileName()); //  (HEADER_FILE);
    string content [H_FILE_LENGTH];
    int pos = -1;
    //copy data from file into string array
    //some variable names are repeated so the section position must be found first
    for(int i = 0; i < H_FILE_LENGTH; i++)
    {
        getline(rfile,content[i]);
        if(content[i].find(section) != string::npos)                                //if the section's name is found
        {
            pos = i;
        }
    }
    rfile.close();
    //search for variable from start of relevant section
    while(pos < H_FILE_LENGTH - 1)
    {
        pos++;
        if(content[pos].find(varName) != string::npos)                                //if the variable's name is found
        {
            break;                                                                    //take first occurance of the variable name
        }
    }
    //replace corresponding variable with new value
    content[pos] = varName;
    content[pos].append(" = ");
    content[pos].append(data);
    content[pos].append(";\r");                       //Use \r (carriage return) because sometimes it gets forgotten
    //overwrite file with new value
    ofstream wfile (getHeaderFileName()); // (HEADER_FILE);
    for(int i = 0; i < H_FILE_LENGTH; i++)
    {
        wfile << content[i] << endl;                            //Use endl because it adds \n which gets removed in getline()
    }

    wfile.close();
    for(int i = 0; i < H_FILE_LENGTH; i++)
    {
        content [i] = '\0';
    }
    fflush(stdout);
}


//=============================================================================
// writeToArmtimecfgFile()
//=============================================================================
//method to set up the NextGPSDO armtime.cfg file
void HeaderArmFiles::writeToArmtimecfgFile(string data)
{
    //overwrite file with new value
    ofstream wfile (armtimecfgFileName);
    stringstream ss;
    ss << "Date=" << data.substr(8,2) << "/" << data.substr(5,2) << "/" << data.substr(0,4);
    wfile << ss.str() << endl;
    ss.str("");
    ss << "Arm_Time=" << data.substr(11,8) << endl;
    wfile << ss.str() << endl;
    wfile.close();

    fflush(stdout);
}


//=============================================================================
// readFromHeaderFile()
//=============================================================================
//method to return a variable's value from the NeXtRAD header file
QString HeaderArmFiles::readFromHeaderFile(string varName, string section)
{
    //Read from header file
    ifstream rfile (getHeaderFileName()); // (HEADER_FILE);
    string temp = "";
    int pos = -1;

    //find line with corresponding variable's name and find position of the variable's value on that line
    while(!rfile.eof())
    {
        getline(rfile,temp);
        if(temp.find(section) != string::npos)
        {
            pos = -2;                                                   //pos = -2 is just to indicate that the section was found
        }
        if(pos == -2)
        {
            if(temp.find(varName) != string::npos)                      //if the variable's name is found
            {
                pos = temp.find_first_of('=') + 2;                       //+2 because data starts after "= "
                break;
            }
        }
    }
    rfile.close();
    if(pos < 0)
    {
        string str = "error: can't find variable";
        return "Error";
    }
    //cout << "substring" << endl;                              //debugging

    //convert from std::string to QString
    string str = temp.substr(pos, temp.find_last_of(';') - pos);
    //cout << "substring end" << endl;                              //debugging


    return   QString::fromUtf8( str.c_str());
}

//=============================================================================
// renameHeaderFile()
//=============================================================================
//method to rename the NeXtRAD header file
int HeaderArmFiles::renameHeaderFile(string newname)
{
    int result;
    result = rename( getHeaderFileName().c_str() , newname.c_str() );
    return result;
}

/*
//=============================================================================
// saveHeaderFile()
//=============================================================================
//method to rename the NeXtRAD header file
int HeaderArmFiles::saveHeaderFile(string newname)
{
    int result;
    result = rename( getHeaderFileName().c_str() , newname.c_str() );
    return result;
}
*/
