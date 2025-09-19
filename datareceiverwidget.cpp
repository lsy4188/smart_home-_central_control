#include "datareceiverwidget.h"

DataReceiverWidget::DataReceiverWidget(QWidget *parent , QTcpSocket* socket)
    :CustomWidget(parent),m_tcpSocket(socket)
{
    this->setTitle("");
    this->setMinimumHeight(100);
    //创建数据显示标签
    m_dataLabel=new QLabel("等待数据连接中。。。",this);
    m_dataLabel->setStyleSheet("color: white; font-size: 14px;");
    m_dataLabel->setAlignment(Qt::AlignCenter|Qt::AlignVCenter);
    this->addContentWidget(m_dataLabel);

    //数据实时更新
    m_updateTimer=new QTimer();
    connect(m_updateTimer,&QTimer::timeout,this,&DataReceiverWidget::updateDataDisplay);
    m_updateTimer->start(1000);

    //是否连接设备
    if(m_tcpSocket){
        connect(m_tcpSocket,&QTcpSocket::readyRead,this,&DataReceiverWidget::onReadyRead);
        connect(m_tcpSocket,&QTcpSocket::disconnected,this,&DataReceiverWidget::onDisconnected);
        m_dataLabel->setText("已接到设备");
    }

}

DataReceiverWidget::~DataReceiverWidget() {
    if(m_updateTimer){
        m_updateTimer->stop();
        m_updateTimer->deleteLater();
    }
}

void DataReceiverWidget::setSocket(QTcpSocket* socket) {
    // 断开旧连接信号
    if (m_tcpSocket) {
        disconnect(m_tcpSocket, &QTcpSocket::readyRead, this, &DataReceiverWidget::onReadyRead);
        disconnect(m_tcpSocket, &QTcpSocket::disconnected, this, &DataReceiverWidget::onDisconnected);
    }
    m_tcpSocket = socket;
    // 连接新socket信号
    if (m_tcpSocket) {
        connect(m_tcpSocket, &QTcpSocket::readyRead, this, &DataReceiverWidget::onReadyRead);
        connect(m_tcpSocket, &QTcpSocket::disconnected, this, &DataReceiverWidget::onDisconnected);
        m_dataLabel->setText("已连接设备");
    } else {
        m_dataLabel->setText("设备断开连接");
    }
}

void DataReceiverWidget::onReadyRead(){
    if(m_tcpSocket&&m_tcpSocket->bytesAvailable()>0){
        QByteArray data= m_tcpSocket->readAll();
        m_receivedData=QString::fromUtf8(data);
    }
}
void DataReceiverWidget::onDisconnected(){
    m_dataLabel->setText("设备断开连接");
}
void DataReceiverWidget::updateDataDisplay(){
    if(!m_receivedData.isEmpty()){
        m_dataLabel->setText(m_receivedData);
    }
}
