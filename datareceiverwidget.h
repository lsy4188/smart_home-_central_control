#ifndef DATARECEIVERWIDGET_H
#define DATARECEIVERWIDGET_H
/*
 * 设备采集数据标签
*/
#include <QObject>
#include "customwidget.h"
#include <QTcpSocket>
#include <QTimer>
class DataReceiverWidget : public CustomWidget
{
    Q_OBJECT
public:
   explicit DataReceiverWidget(QWidget *parent = nullptr, QTcpSocket* socket = nullptr);
    ~DataReceiverWidget();
    void setSocket(QTcpSocket* socket);

private slots:
    void onReadyRead();
    void onDisconnected();
    void updateDataDisplay();

private:
   QTcpSocket* m_tcpSocket;
   QTimer* m_updateTimer;
   QLabel* m_dataLabel;
   QString m_receivedData;
};

#endif // DATARECEIVERWIDGET_H
