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

/*
time_t now = time(0);
struct tm tstruct;
char buf[80];
tstruct = *localtime(&now);
strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);
*/

string Datetime::nowPlusSecs(int secs)
{
    QDateTime eTime = QDateTime::currentDateTime();
    QDateTime t = eTime.addSecs(secs);
    string now = t.toString("yyyy-MM-dd hh:mm:ss").toUtf8().constData();
    return now.c_str();
}


//=============================================================================
// getCountDownTime()
//=============================================================================
//Calculates the hours minutes and seconds remaining for countdown
QString Datetime::getCountDownTime(time_t timeLeft)
{
    //convert unix time to hours, minutes, seconds
    int tim = (int) timeLeft;
    int hours, mins, secs;
    secs = tim % 60;
    tim -= secs;
    tim = tim / 60;
    mins = tim % 60;
    tim -= mins;
    hours = tim / 60;
/*
    //return as QString so that it's easily outputted
    QString temp = QStringLiteral("%1:").arg(hours, 2, 10, QChar('0'));
    temp.append(QStringLiteral("%1").arg(mins, 2, 10, QChar('0')));
    temp.append(QStringLiteral(":%1").arg(secs, 2, 10, QChar('0')));
    return temp;
*/
    string s   = "";
    s += hours; // hh
    s += ":";
    s += mins; // mm
    s += ":";
    s += secs; // ss
    s += ":";

    cout << "CountDownTime = " << s << endl;
    QString temp = QString::fromStdString(s);
    return temp;

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
    cout << "s is : " << s << endl;

    // Convert the date from 'yyyy-MM-dd hh:mm:ss' format to get correct Unix time!
    struct tm tm;
    strptime(s.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
    return mktime(&tm);
}





