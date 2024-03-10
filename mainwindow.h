#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QMouseEvent>
//网络管理，响应
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "weather_data.h"
#include <QList>
#include <QLabel>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    void getinfo(QString citycode);//获取数据
    void analy_json(QByteArray& qByte);//解析json数据
    void ui_update();//更新ui
    void paintHighCurve();
    void paintLowCurve();


   protected://重写父类方法
    void contextMenuEvent(QContextMenuEvent* event);
    void mousePressEvent(QMouseEvent* event);//带鼠标位置
    void mouseMoveEvent(QMouseEvent* event);
    bool eventFilter(QObject* watched,QEvent* event);



private slots:
    void on_btnSearch_clicked();

private:
    void Replied(QNetworkReply* reply);//槽函数,接受网络信号后调用这个函数
   private:
    Ui::MainWindow* ui;

    QMenu* rightclicked_menu;
    QAction* Exit;
    QPoint offset;//偏移距离。窗口左上角到鼠标的偏移
    QNetworkAccessManager* my_manager;

    Today myToday;
    Day myDay[6];

    QList<QLabel*> myweeklist;
    QList<QLabel*> mydaylist;
    QList<QLabel*> mytypelist;
    QList<QLabel*> mytypeiconlist;
    QList<QLabel*> myaqilist;
    QList<QLabel*> myfllist;
    QList<QLabel*> myfxlist;

    QMap<QString,QString> mytypemap;



};
#endif  // MAINWINDOW_H
