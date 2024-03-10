#include "mainwindow.h"

#include <QContextMenuEvent>
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QMessageBox>
#include <QNetworkReply>
#include <QPainter>
#include <QTimer>
#include <QPainter>


#include "ui_mainwindow.h"
#include "weather_mapcityjson.h"

const int increament=3;//像素点，便于调整绘画高度
const int point_r=3;//点的半径
const int text_x=10;//文字偏移
const int text_y=10;


MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    //无边框设置
    setWindowFlag(Qt::FramelessWindowHint);
    setFixedSize(800,450);//设置固定大小
    //右键菜单设置
    rightclicked_menu = new QMenu(this);
    Exit = new QAction();
    Exit->setText("退出");
    Exit->setIcon(QIcon(":/res/close.png"));//设置图标
    rightclicked_menu->addAction(Exit);

    //信号与槽连接，使其可以点击退出就退出
    connect(Exit,&QAction::triggered,this,[=]{qApp->exit(0);});//第二个参数要用&，最后一个参数用的是lamada表达式，明天细看看

    //控件添加到数组
    myweeklist<<ui->lblWeek0<<ui->lblWeek1<<ui->lblWeek2<<ui->lblWeek3<<ui->lblWeek4<<ui->lblWeek5;
    mydaylist<<ui->lblDate0<<ui->lblDate1<<ui->lblDate2<<ui->lblDate3<<ui->lblDate4<<ui->lblDate5;
    mytypelist<<ui->lblType0<<ui->lblType1<<ui->lblType2<<ui->lblType3<<ui->lblType4<<ui->lblType5;
    mytypeiconlist<<ui->lblTypeIcon0<<ui->lblTypeIcon1<<ui->lblTypeIcon2<<ui->lblTypeIcon3<<ui->lblTypeIcon4<<ui->lblTypeIcon5;
    myaqilist<<ui->lblQuality0<<ui->lblQuality1<<ui->lblQuality2<<ui->lblQuality3<<ui->lblQuality4<<ui->lblQuality5;
    myfllist<<ui->lblFl0<<ui->lblFl1<<ui->lblFl2<<ui->lblFl3<<ui->lblFl4<<ui->lblFl5;
    myfxlist<<ui->lblFx0<<ui->lblFx1<<ui->lblFx2<<ui->lblFx3<<ui->lblFx4<<ui->lblFx5;


    my_manager=new QNetworkAccessManager(this);
    connect(my_manager,&QNetworkAccessManager::finished,this,&MainWindow::Replied);

    //直接构造天气信息，默认出现是北京市,城市编码101010100
    getinfo("哈尔滨");

    //在标签上绘制曲线，需要添加事件过滤器,当前窗口
    //后续重写eventFilter方法
    ui->lblHighCurve->installEventFilter(this);
    ui->lblLowCurve->installEventFilter(this);

    //key(天气类型） value（资源路径）
    mytypemap.insert("暴雪",":/res/type/BaoXue.png");
    mytypemap.insert("暴雨",":/res/type/BaoYu.png");
    mytypemap.insert("暴雨到大暴雨",":/res/type/BaoYuDaoDaBaoYu.png");
    mytypemap.insert("大暴雨到特大暴雨",":/res/type/DaBaoYuDaoTeDaBaoYu.png");
    mytypemap.insert("大到暴雪",":/res/type/DaDaoBaoXue.png");
    mytypemap.insert("大到暴雨",":/res/type/DaDaoBaoYu.png");
    mytypemap.insert("大雪",":/res/type/DaXue.png");
    mytypemap.insert("大雨",":/res/type/DaYu.png");
    mytypemap.insert("冻雨",":/res/type/DongYu.png");
    mytypemap.insert("多云",":/res/type/DuoYun.png");
    mytypemap.insert("浮沉",":/res/type/FuChen.png");
    mytypemap.insert("雷阵雨",":/res/type/LeiZhenYu.png");
    mytypemap.insert("雷阵雨伴有冰雹",":/res/type/LeiZhenYuBanYouBingBao.png");
    mytypemap.insert("霾",":/res/type/Mai.png");
    mytypemap.insert("强沙尘暴",":/res/type/QiangShaChenBao.png");
    mytypemap.insert("晴",":/res/type/Qing.png");
    mytypemap.insert("沙尘暴",":/res/type/ShaChenBao.png");
    mytypemap.insert("特大暴雨",":/res/type/TeDaBaoYu.png");
    mytypemap.insert("undefined",":/res/type/undefined.png");
    mytypemap.insert("雾",":/res/type/Wu.png");
    mytypemap.insert("小到中雪",":/res/type/XiaoDaoZhongXue.png");
    mytypemap.insert("小到中雨",":/res/type/XiaoDaoZhongYu.png");
    mytypemap.insert("小雪",":/res/type/XiaoXue.png");
    mytypemap.insert("小雨",":/res/type/XiaoYu.png");
    mytypemap.insert("雪",":/res/type/Xue.png");
    mytypemap.insert("扬沙",":/res/type/YangSha.png");
    mytypemap.insert("阴",":/res/type/Yin.png");
    mytypemap.insert("雨",":/res/type/Yu.png");
    mytypemap.insert("雨夹雪",":/res/type/YuJiaXue.png");
    mytypemap.insert("阵雪",":/res/type/DongYu.png");
    mytypemap.insert("阵雨",":/res/type/ZhenXue.png");
    mytypemap.insert("中到大雪",":/res/type/ZhongDaoDaXue.png");
    mytypemap.insert("中到大雨",":/res/type/ZhongDaoDaYu.png");
    mytypemap.insert("中雪",":/res/type/ZhongXue.png");
    mytypemap.insert("中雨",":/res/type/ZhongYu.png");

}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::getinfo(QString cityname)
{
    QString citycode=Weather_cj::getCityCode(cityname);
    if(citycode.isEmpty())
    {
        QMessageBox::warning(this,"提示","请检查输入是否正确",QMessageBox::Ok);
        return;
    }
    QUrl url("http://t.weather.itboy.net/api/weather/city/"+citycode);//citycode忘记加/就会404
    my_manager->get(QNetworkRequest(url));//发送get请求，成功后通过信号与槽关联到replied函数
}

void MainWindow::analy_json(QByteArray& qByte)
{
    QJsonParseError error;
    QJsonDocument doc=QJsonDocument::fromJson(qByte,&error);//若出错，把出错的信息存在error里
    if(error.error!=QJsonParseError::NoError)
    {
        return;
    }
    QJsonObject root=doc.object();
    qDebug()<<root.value("message").toString();
    //解析日期，城市
    myToday.date=root.value("date").toString();
    myToday.city=root.value("cityInfo").toObject().value("city").toString();//！！！I大写，json中cityInfo

    //解析昨天
    QJsonObject qo=root.value("data").toObject();
    QJsonObject yd=qo.value("yesterday").toObject();
    myDay[0].week=yd.value("week").toString();
    myDay[0].date=yd.value("ymd").toString();
    myDay[0].type=yd.value("type").toString();

    QString s=yd.value("high").toString().split(" ").at(1);//根据空格分割 18
    //qDebug()<<"s"<<s;
    //qDebug()<<s.left(s.length()-1);
    myDay[0].high=s.left(s.length()-1).toInt();//取最左边s.length()-2个的字符
    //qDebug()<<"high"<<myDay[0].high;
    QString s2=yd.value("low").toString().split(" ").at(1);//low
    myDay[0].low=s.left(s.length()-1).toInt();

    myDay[0].fx=yd.value("fx").toString();
    myDay[0].fl=yd.value("fl").toString();

    myDay[0].aqi=yd.value("aqi").toDouble();

    //五天数据,在forecast中选5天,forecast在data中
    QJsonArray qaf=qo.value("forecast").toArray();
    for(int i=0;i<5;++i)
    {
        QJsonObject qobjf=qaf[i].toObject();
        myDay[i+1].week=qobjf.value("week").toString();
        myDay[i+1].date=qobjf.value("ymd").toString();
        myDay[i+1].type=qobjf.value("type").toString();
        QString s=qobjf.value("high").toString().split(" ").at(1);//根据空格分割 18
        myDay[i+1].high=s.left(s.length()-1).toInt();//18
        QString s2=qobjf.value("low").toString().split(" ").at(1);//low
        myDay[i+1].low=s.left(s.length()-1).toInt();

        myDay[i+1].fx=qobjf.value("fx").toString();
        myDay[i+1].fl=qobjf.value("fl").toString();

        myDay[i+1].aqi=qobjf.value("aqi").toDouble();

    }
    //在qo的基础上继续解析今天数据
    myToday.ganmao=qo.value("ganmao").toString();
    myToday.shidu=qo.value("shidu").toString();
    myToday.pm25=qo.value("pm25").toDouble();
    myToday.quality=qo.value("quality").toString();
    myToday.wendu=qo.value("wendu").toString();///!!!json中wendu是""
    //myday[1]就是mytoday,forecast的第一个数组也有today信息
    myToday.fx=myDay[1].fx;
    myToday.fl=myDay[1].fl;
    myToday.type=myDay[1].type;
    myToday.high=myDay[1].high;
    myToday.low=myDay[1].low;

    //更新ui
    ui_update();
    //绘制温度曲线
    ui->lblHighCurve->update();
    ui->lblLowCurve->update();//调用这个方法时会触发paint事件，事件被minwindow拦截，调用eventfilte方法，才会开始绘制
}

void MainWindow::ui_update()
{
    ui->lblDate->setText(QDateTime::fromString(myToday.date,"yyyyMMdd").toString("yyyy/MM/dd")+" "+myDay[1].week);//格式改变一下加上/
    ui->lblCity->setText(myToday.city);//为什么没显示出来，json写入出错
    ui->lblTemp->setText((myToday.wendu)+"°");//！一直显示不出来，json中wendu是string 加上°c格式不太好
    ui->lblTypeIcon->setPixmap(mytypemap[myToday.type]);
    ui->lblType->setText(myToday.type);
    ui->lblLowHigh->setText(QString::number(myToday.low)+"~"+QString::number(myToday.high)+"°C");//number方法转换
    ui->lblGanMao->setText("感冒指数:"+myToday.ganmao);
    ui->lblWindFx->setText(myToday.fx);
    ui->lblWindFl->setText(myToday.fl);
    ui->lblPM25->setText(QString::number(myToday.pm25));
    ui->lblShiDu->setText(myToday.shidu);
    ui->lblQuality->setText(myToday.quality);

    for(int i=0;i<6;++i)
    {
        myweeklist[i]->setText("周"+myDay[i].week.right(1));//周+取最后一个字符
        ui->lblWeek0->setText("昨天");
        ui->lblWeek1->setText("今天");
        ui->lblWeek2->setText("明天");

        QStringList qsl=myDay[i].date.split("-");
        mydaylist[i]->setText(qsl[1]+"/"+qsl[2]);
        mytypelist[i]->setText(myDay[i].type);
        mytypeiconlist[i]->setPixmap(mytypemap[myDay[i].type]);
        if(myDay[i].aqi>=0&&myDay[i].aqi<=50)
        {
            myaqilist[i]->setText("优");
            myaqilist[i]->setStyleSheet("background-color:rgb(121,184,0);");
        }
        else if(myDay[i].aqi>50&&myDay[i].aqi<=100)
        {
            myaqilist[i]->setText("良");
            myaqilist[i]->setStyleSheet("background-color:rgb(255,187,23);");
        }
        else if(myDay[i].aqi>100&&myDay[i].aqi<=150)
        {
            myaqilist[i]->setText("轻度");
            myaqilist[i]->setStyleSheet("background-color:rgb(255,87,97);");
        }
        else if(myDay[i].aqi>150&&myDay[i].aqi<=200)
        {
            myaqilist[i]->setText("中度");
            myaqilist[i]->setStyleSheet("background-color:rgb(235,17,27);");
        }
        else if(myDay[i].aqi>200&&myDay[i].aqi<=250)
        {
            myaqilist[i]->setText("重度");
            myaqilist[i]->setStyleSheet("background-color:rgb(170,0,0);");
        }
        else
        {
            myaqilist[i]->setText("严重");
            myaqilist[i]->setStyleSheet("background-color:rgb(110,0,0);");
        }
        myfxlist[i]->setText(myDay[i].fx);
        myfllist[i]->setText(myDay[i].fl);
    }


}

void MainWindow::paintHighCurve()
{
    QPainter painter(ui->lblHighCurve);//绘制到这里
    painter.setRenderHint(QPainter::Antialiasing,true);//抗锯齿
    //获取x，y坐标 x位于控件中心
    int pointx[6]={0};
    for(int i=0;i<6;++i)
    {
        pointx[i]=myweeklist[i]->pos().x()+myweeklist[i]->width()/2;//中心点再偏移才正好
    }
    //取平均值画中间，高低多少度相应移动
    int sum_tmp=0;
    int ave_tmp=0;
    for(int i=0;i<6;++i)
    {
        sum_tmp+=myDay[i].high;
    }
    ave_tmp=sum_tmp/6;
    int pointy[6]={0};
    int yCenter=ui->lblHighCurve->height()/2;
    for(int i=0;i<6;++i)
    {
        pointy[i]=yCenter-((myDay[i].high-ave_tmp)*increament);
    }
    //绘制，连起来
    QPen pen=painter.pen();
    pen.setWidth(1);
    pen.setColor(QColor(255,170,0));
    painter.setPen(pen);
    painter.setBrush(QColor(255,170,0));//内部填充颜色
    for(int i=0;i<6;++i)
    {
        painter.drawEllipse(QPoint(pointx[i],pointy[i]),point_r,point_r);//圆心x轴y轴
        //重新点击才会刷新？？？需要手动出发变化！！！
        //qDebug()<<myDay[i].high<<endl;
        painter.drawText(pointx[i]-text_x,pointy[i]-text_y,QString::number(myDay[i].high)+"°");
    }
    for(int i=0;i<5;++i)
    {
        if(i==0)
        {
            pen.setStyle(Qt::DotLine);//虚线
            painter.setPen(pen);
        }
        else {
            pen.setStyle(Qt::SolidLine);
            painter.setPen(pen);
        }
        painter.drawLine(pointx[i],pointy[i],pointx[i+1],pointy[i+1]);
    }

}

void MainWindow::paintLowCurve()
{
    //画笔失效，不知道为什么画不出来
    QPainter painter2(ui->lblLowCurve);//绘制到这里
    painter2.setRenderHint(QPainter::Antialiasing,true);//抗锯齿
    //获取x，y坐标 x位于控件中心
    int pointx[6]={0};
    for(int i=0;i<6;++i)
    {
        pointx[i]=myweeklist[i]->pos().x()+myweeklist[i]->width()/2;//中心点再偏移才正好
    }
    //取平均值画中间，高低多少度相应移动
    int sum_tmp=0;
    int ave_tmp=0;
    for(int i=0;i<6;++i)
    {
        sum_tmp+=myDay[i].low;
        qDebug()<<myDay[i].low<<endl;
    }
    ave_tmp=sum_tmp/6;
    int pointy[6]={0};
    int yCenter=ui->lblLowCurve->height()/2;
    for(int i=0;i<6;++i)
    {
        pointy[i]=yCenter-((myDay[i].low-ave_tmp)*increament);
    }
    //绘制，连起来
    QPen pen2=painter2.pen();
    pen2.setWidth(1);
    pen2.setColor(QColor(0,255,255));
    painter2.setPen(pen2);
    painter2.setBrush(QColor(0,255,255));//内部填充颜色
    for(int i=0;i<6;++i)
    {
        painter2.drawEllipse(QPoint(pointx[i],pointy[i]),point_r,point_r);//圆心x轴y轴
        //重新点击才会刷新？？？
        //qDebug()<<myDay[i].high<<endl;
        painter2.drawText(pointx[i]-text_x,pointy[i]-text_y,QString::number(myDay[i].low)+"°");
    }
    for(int i=0;i<5;++i)
    {
        if(i==0)
        {
            pen2.setStyle(Qt::DotLine);//虚线
            painter2.setPen(pen2);
        }
        else {
            pen2.setStyle(Qt::SolidLine);
            painter2.setPen(pen2);
        }
        painter2.drawLine(pointx[i],pointy[i],pointx[i+1],pointy[i+1]);
    }
    painter2.end();
}

void MainWindow::contextMenuEvent(QContextMenuEvent* event) {
    //右键出现菜单
    rightclicked_menu->exec(QCursor::pos());//传给菜单鼠标右键单机的地方，使其在这个位置出现
    //事件已经处理，无需向前传递？？？
    //event->accept();
}

void MainWindow::mousePressEvent(QMouseEvent *event)//按下记录位置
{
    //获取窗口位置
    offset=event->globalPos()-this->pos();//gol是全局左上角
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)//移动改变位置
{
    this->move(event->globalPos()-offset);//设置窗口左上角的位置
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    //调用绘制高低温曲线方法,捕获标签绘制事件才开始画，课设音乐管理系统没有解决的问题！！！棒
    if(watched==ui->lblHighCurve&&event->type()==QEvent::Paint)
    {
        paintHighCurve();
    }
    if(watched==ui->lblLowCurve&&event->type()==QEvent::Paint)//一直没画出来是这里忘改了
    {
        paintLowCurve();
    }

    return QWidget::eventFilter(watched,event);//其余控件调用父类方法
}

void MainWindow::Replied(QNetworkReply *reply)
{
    //qDebug()<<"right finished"<<endl;//请求成功
    //所有数据都封装在reply里
    int status_code=reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();//响应码200为成功
    qDebug()<<status_code<<endl;
    //可打印各种信息
    if(reply->error()!=QNetworkReply::NoError||status_code!=200)
    {
        //出错
        qDebug()<<reply->errorString().toLatin1().data();
        QMessageBox::warning(this,"提示","请求数据失败，请检查网络连接",QMessageBox::Ok);
    }
    else {
        QByteArray byteArray=reply->readAll();
        analy_json(byteArray);
        //qDebug()<<"readAll"<<byteArray.data();
    }
    reply->deleteLater();//!!!把堆上的内存释放，否则会造成内存泄露，内存管理，还没看

}

void MainWindow::on_btnSearch_clicked()
{
    QString cityn=ui->leCity->text();
    getinfo(cityn);
}
