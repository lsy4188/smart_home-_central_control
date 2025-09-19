#ifndef ENTERINTERFACE_H
#define ENTERINTERFACE_H
/*
 * 连接界面
*/
#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QMessageBox>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDebug>
class MainInterface;

class EnterInterface : public QWidget
{
    Q_OBJECT
public:
    explicit EnterInterface(QWidget *parent = nullptr);
    ~EnterInterface();
    void mouseMoveEvent(QMouseEvent*event);
    void mousePressEvent(QMouseEvent*event);
    void paintEvent(QPaintEvent*event);

private:
    QLabel *avatarLabel;
    QLineEdit *ipEdit;
    QLineEdit *portEdit;
    QPushButton *enterButton;
    QPushButton *closeButton;
    QPoint windowPosition;
    QTcpServer* pTcpServer;
    QTcpSocket* pSocket;
    MainInterface* mainInterface;

    void initUI();
    void setWindowStyle();

signals:
    void connected(QTcpServer* server, QTcpSocket* socket);

private slots:
    void connectSTM32();
    void showMainInterface(); // 显示主界面
    void onNewConnection();
};

#endif // ENTERINTERFACE_H
