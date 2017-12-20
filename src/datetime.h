//Author:       Shirley Coetzee
//File:         datetime.h
//For:          University of Cape Town, Dept. Elec. Eng., RRSG NeXtRAD
//Created:      December 2017

#ifndef DATETIME_H
#define DATETIME_H

#include <QDateTime>
#include <iomanip>
#include "includes.h"
#include "parameters.h"

class Datetime
{
    private:


    public:
        Datetime(void);
        string getTimeAndDate(const char *format);
        string getNowPlusSecs(int secs);
        string getNowSecsPlusSecs(int secs);
        string getNowInYears(void);
        string getNowInMonths(void);
        string getNowInDays(void);
        string getNowInHours(void);
        string getNowInMinutes(void);
        string getNowInSeconds(void);
        QString getCountDownTime(time_t timeLeft);
        time_t convertToUnixTime(string timestamp);
};


#endif // DATETIME_H
