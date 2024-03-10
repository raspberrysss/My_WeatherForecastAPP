#ifndef WEATHER_MAPCITYJSON_H
#define WEATHER_MAPCITYJSON_H
//工具类，实现citycode与cityname对应

#include <QString>
#include <QMap>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
class Weather_cj
{
    private:
       static QMap<QString,QString> mycitymap;
       static void initcitymap()
       {
           //读取文件，解析文件
           QString filepath="D:/QTfile/citycode.json";//用/
           QFile file(filepath);
           file.open(QIODevice::ReadOnly|QIODevice::Text);
           QByteArray json=file.readAll();
           //解析写入map
           QJsonParseError err;
           QJsonDocument doc= QJsonDocument::fromJson(json,&err);
           if(err.error!=QJsonParseError::NoError)
           {
               return;//出错
           }
           if(!doc.isArray())
           {
               return;//非json数组
           }
           QJsonArray city=doc.array();
           for(int i=0;i<city.size();++i)
           {
               //每个元素都是json对象
              QString cityn=city[i].toObject().value("city_name").toString();
              QString cityc=city[i].toObject().value("city_code").toString();

              if(cityc.size()>0)
              {
                  mycitymap.insert(cityn,cityc);//只添加城市，不添加省份，省份无法直接查询天气
              }

           }


       }
    public:
    static QString getCityCode(QString cityname)//静态方法，.直接调用
    {
          if(mycitymap.isEmpty())
          {
              initcitymap();
          }
          QMap<QString,QString>::Iterator it=mycitymap.find(cityname);
          if(it==mycitymap.end())//到结尾未找到
          {
              it=mycitymap.find(cityname+"市");
              if(it!=mycitymap.end())
              {
                  return it.value();//找到了
              }
              it=mycitymap.find(cityname+"县");
              if(it!=mycitymap.end())
              {
                  return it.value();//找到了
              }
          }
          if(it!=mycitymap.end())
          {
              return it.value();//找到了
          }
          else
              return "";


    }
};

QMap<QString,QString> Weather_cj::mycitymap={};//初始化静态变量

#endif // WEATHER_MAPCITYJSON_H
