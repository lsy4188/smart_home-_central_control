#ifndef MAININTERFACE_H
#define MAININTERFACE_H
/*
 * 中控主界面
 */
#include <QMainWindow>
#include <QObject>
#include <QFrame>
#include<QMouseEvent>
#include <QCloseEvent>
#include <QVBoxLayout>//垂直布局
#include <QHBoxLayout> //水平布局
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
#include <QList>  // 用于管理子组件


class DataReceiverWidget;
class NetworkSwitchWidget;

class MainInterface : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainInterface(QWidget *parent = nullptr, QTcpServer* server=nullptr, QTcpSocket* socket=nullptr);
    ~MainInterface();
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent*event);
    void closeEvent(QCloseEvent* event);
    void setSocket(QTcpSocket* socket);
private:
    // 初始化顶部区域
    void initTopArea();
    // 初始化中间区域
    void initMiddleArea();
    // 初始化底部区域
    void initBottomArea();

    QWidget *centralWidget;  // 中央部件
    QVBoxLayout *mainLayout; // 主布局
    QPoint windowPoint;//记录上一次窗口位置
    QTcpServer* pTcpServer;
    QTcpSocket* pSocket;

    // 子组件实例列表（用于同步socket）
    QList<DataReceiverWidget*> dataReceiverWidgets;
    QList<NetworkSwitchWidget*> networkSwitchWidgets;

signals:
    void closed();
};

#endif // MAININTERFACE_H
