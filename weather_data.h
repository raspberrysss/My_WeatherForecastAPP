#ifndef WEATHER_DATA_H
#define WEATHER_DATA_H

#include <QString>

class Today
{
    public:
        QString date;
        QString city;
        QString ganmao;
        QString wendu;
        QString shidu;
        double pm25;
        QString quality;
        QString type;
        QString fl;
        QString fx;
        int high;
        int low;
        Today()
        {

            date="2023-1-28";
            city="哈尔滨";
            ganmao="感冒指数";
            wendu="0";
            shidu="0%";
            pm25=0.0;
            type="多云";
            quality="无数据";
            fl="1级";
            fx="北风";
            high=20;
            low=5;
        }
};

class Day
{
    public:
        QString date;
        QString week;
        QString type;
        int high;
        int low;
        QString fl;
        QString fx;
        double aqi;
        Day()
        {
            date="2023-1-28";
            week="周六";
            type="多云";
            high=0;
            low=0;
            fl="1级";
            fx="北风";
            aqi=0.0;
        }
};

#endif // WEATHER_DATA_H
