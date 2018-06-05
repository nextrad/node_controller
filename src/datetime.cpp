//Class:        Datetime
//For:          University of Cape Town, Dept. Elec. Eng., RRSG NeXtRAD
//Author:       Shirley Coetzee
//Created:      December 2017
//Version       1.0 (December 2017)

#include "datetime.h"
#include "includes.h"
#include "parameters.h"

Datetime::Datetime(void)
{

}
//=============================================================================

string Datetime::getTimeAndDate(const char *format)
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];
    time (&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer,80,format,timeinfo);
    string dateTime(buffer);
    return dateTime;
}
//=============================================================================
string Datetime::getNowPlusSecs(int secs)
{
    QDateTime eTime = QDateTime::currentDateTime();
    QDateTime t = eTime.addSecs(secs);
    string now = t.toString("yyyy-MM-dd hh:mm:ss").toUtf8().constData();
    return now.c_str();
}
//=============================================================================
string Datetime::getNowSecsPlusSecs(int secs)
{
    QDateTime eTime = QDateTime::currentDateTime();
    QDateTime t = eTime.addSecs(secs);
    string now = t.toString("ss").toUtf8().constData();
    return now.c_str();
}
//=============================================================================
//"yyyy-MM-dd hh:mm:ss"  Local time (SAST)
string Datetime::getNowInYears(void)
{
    QDateTime eTime = QDateTime::currentDateTime();
    string now = eTime.toString("yyyy").toUtf8().constData();
    return now.c_str();
}
//=============================================================================
//"yyyy-MM-dd hh:mm:ss"  Local time (SAST: UTC+2)
string Datetime::getNowInMonths(void)
{
    QDateTime eTime = QDateTime::currentDateTime();
    string now = eTime.toString("MM").toUtf8().constData();
    return now.c_str();
}
//=============================================================================
//"yyyy-MM-dd hh:mm:ss"  Local time (SAST: UTC+2)
string Datetime::getNowInDays(void)
{
    QDateTime eTime = QDateTime::currentDateTime();
    string now = eTime.toString("dd").toUtf8().constData();
    return now.c_str();
}
//=============================================================================
//"yyyy-MM-dd hh:mm:ss"  Local time (SAST)
string Datetime::getNowInHours(void)
{
    QDateTime eTime = QDateTime::currentDateTime();
    string now = eTime.toString("hh").toUtf8().constData();
    return now.c_str();
}
//=============================================================================
//"yyyy-MM-dd hh:mm:ss"  Local time (SAST: UTC+2)
string Datetime::getNowInMinutes(void)
{
    QDateTime eTime = QDateTime::currentDateTime();
    string now = eTime.toString("mm").toUtf8().constData();
    return now.c_str();
}
//=============================================================================
//"yyyy-MM-dd hh:mm:ss"  Local time (SAST: UTC+2)
string Datetime::getNowInSeconds(void)
{
    QDateTime eTime = QDateTime::currentDateTime();
    string now = eTime.toString("ss").toUtf8().constData();
    return now.c_str();
}

//=============================================================================
// getCountDownTime()
//=============================================================================
//Calculates the hours minutes and seconds remaining for countdown
QString Datetime::getCountDownTime(time_t timeLeft)
{
    //convert unix time to hours, minutes, seconds
    int tim = (int)timeLeft;

    int hours, mins, secs;

    hours = tim / 3600;
    tim -= hours * 3600;
    mins = tim / 60;
    tim -= mins * 60;
    secs = tim;

    stringstream ss_countdown;
    ss_countdown << std::setfill('0') << std::setw(2) << hours << ":" << std::setfill('0') << std::setw(2) << mins << ":" << std::setfill('0') << std::setw(2) << secs;
    return QString::fromStdString(ss_countdown.str());
}
//=============================================================================
// convertToUnixTime()
//=============================================================================
//Method to convert time to 'yyyy-MM-dd hh:mm:ss' format then to data type time_t
time_t Datetime::convertToUnixTime(string timestamp)
{
    string str = timestamp;
    string s   = "";

    // Convert the date from 'dd-MM-yyyy hh:mm:ss' to 'yyyy-MM-dd hh:mm:ss' format
    if ((str.substr(2,1) == "-") && (str.substr(5,1) == "-") && (str.substr(10,1) == " "))
    {
        s.assign(str,6,4);  // yyyy
        s.append(str,2,4);  // yyyy-MM-
        s.append(str,0,2);  // yyyy-MM-dd
        s.append(str,10,9); // yyyy-MM-dd hh:mm:ss
    }
    // Take as is if the date in 'yyyy-MM-dd hh:mm:ss' format
    else if ((str.substr(4,1) == "-") && (str.substr(7,1) == "-") && (str.substr(10,1) == " "))
    {
        s = str;
    }

    // Convert the date from 'yyyy-MM-dd hh:mm:ss' format to get correct Unix time!
    struct tm tm;
    strptime(s.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
    return mktime(&tm);
}





