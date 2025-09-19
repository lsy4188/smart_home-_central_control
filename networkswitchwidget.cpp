#include "networkswitchwidget.h"

NetworkSwitchWidget::NetworkSwitchWidget(QWidget *parent,QTcpSocket *tcpSocket,const QString &controlName,const QString &onCommand,const QString &offCommand,bool initIsOn)
    :CustomWidget(parent),m_tcpSocket(tcpSocket),m_controlName(controlName),m_onCommand(onCommand),m_offCommand(offCommand),m_isOn(initIsOn)
{
    StyleConfig style;
    style.bgColor = "#3A3D49";
    style.borderRadius = 6;
    style.padding = 10;
    style.spacing = 0;
    style.titleColor = "white";
    style.titleFontSize = "14px";
    this->updateStyle(style);

    initSwitchUI();

    updateSwitchStyle();

    updateConnectionStatus();

    if (m_tcpSocket) {
        connect(m_tcpSocket, &QTcpSocket::disconnected, this, &NetworkSwitchWidget::onDisconnected);
    }
}

NetworkSwitchWidget::~NetworkSwitchWidget(){
    if (m_tcpSocket) {
        m_tcpSocket->disconnectFromHost();
        m_tcpSocket->deleteLater();
    }
}

void NetworkSwitchWidget::initSwitchUI(){
    this->setTitle("");
    //设置布局
    QWidget *contentWidget=new QWidget(this);
    QHBoxLayout *mainLayout=new QHBoxLayout(contentWidget);
    mainLayout->setContentsMargins(5, 0, 5, 0);
    mainLayout->setSpacing(10);
    //设置名字标签
    QLabel*nameLabel=new QLabel(m_controlName,this);
    nameLabel->setStyleSheet("color: white; font-size: 14px; font-weight: 500;");
    nameLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    mainLayout->addWidget(nameLabel);
    //连接状态标签
    m_statusLabel = new QLabel(this);
    m_statusLabel->setStyleSheet("color: #CCCCCC; font-size: 12px;");
    m_statusLabel->setAlignment(Qt::AlignLeft|Qt::AlignVCenter);
    mainLayout->addWidget(m_statusLabel);
    // 添加伸缩项（将开关推到右侧）
    mainLayout->addStretch(1);
    //添加开关按钮（右对齐，固定尺寸）
    m_switchBtn = new QCheckBox(this);
    m_switchBtn->setObjectName("HardwareSwitch");
    m_switchBtn->setFixedSize(50, 26);  // 开关尺寸（宽50px，高26px）
    m_switchBtn->setChecked(m_isOn);    // 初始状态同步
    m_switchBtn->setText("");
    //将开关添加到布局
    mainLayout->addWidget(m_switchBtn);
    //将内容区添加到父类CustomWidget
    addContentWidget(contentWidget);
    connect(m_switchBtn, &QCheckBox::clicked, this, [=](bool checked) {
        // 校验连接状态
        if (!m_tcpSocket || m_tcpSocket->state() != QTcpSocket::ConnectedState) {
            m_switchBtn->setChecked(!checked);  // 恢复状态
            qWarning() << "[开关][" << m_controlName << "] 设备未连接，操作无效！";
            return;
        }
        m_isOn = checked;
        onSwitchClicked();
    });
}

void NetworkSwitchWidget::setSocket(QTcpSocket* socket) {
    // 断开旧连接信号
    if (m_tcpSocket) {
        disconnect(m_tcpSocket, &QTcpSocket::disconnected, this, &NetworkSwitchWidget::onDisconnected);
    }
    m_tcpSocket = socket;
    // 连接新socket信号
    if (m_tcpSocket) {
        connect(m_tcpSocket, &QTcpSocket::disconnected, this, &NetworkSwitchWidget::onDisconnected);
    }
    updateConnectionStatus();
}

//处理连接断开
void NetworkSwitchWidget::onDisconnected() {
    updateConnectionStatus();
}

//更新连接状态显示
void NetworkSwitchWidget::updateConnectionStatus() {
    if (m_tcpSocket && m_tcpSocket->state() == QTcpSocket::ConnectedState) {
        m_statusLabel->setText("已连接");
        m_statusLabel->setStyleSheet("color: #4CAF50; font-size: 12px;"); // 绿色表示连接
        m_switchBtn->setEnabled(true);
    } else {
        m_statusLabel->setText("未连接");
        m_statusLabel->setStyleSheet("color: #FF5252; font-size: 12px;"); // 红色表示未连接
        m_switchBtn->setEnabled(false); // 未连接时禁用开关
    }
}
void NetworkSwitchWidget::updateSwitchStyle(){
    QString baseStyle = R"(
        QCheckBox#HardwareSwitch {
            border: none;
            outline: none;
            background-color: #555555;
            border-radius: 13px;
            min-width: 50px;
            max-width: 50px;
            min-height: 26px;
            max-height: 26px;
            transition: background-color 0.2s ease;
        }
        QCheckBox#HardwareSwitch::indicator {
            width: 22px;
            height: 22px;
            border-radius: 11px;
            background-color: white;
            margin: 2px;
            transition: transform 0.2s ease;
        }
    )";

    if (m_isOn) {
        m_switchBtn->setStyleSheet(baseStyle + R"(
            QCheckBox#HardwareSwitch {
                background-color: #4CAF50;
            }
            QCheckBox#HardwareSwitch::indicator {
                transform: translateX(24px);
            }
        )");
    } else {
        m_switchBtn->setStyleSheet(baseStyle + R"(
            QCheckBox#HardwareSwitch {
                background-color: #555555;
            }
            QCheckBox#HardwareSwitch::indicator {
                transform: translateX(0px);
            }
        )");
    }
}

void NetworkSwitchWidget::onSwitchClicked(){
    updateSwitchStyle();

    QString sendCmd = m_isOn ? m_onCommand : m_offCommand;
    QByteArray cmdData = (sendCmd + "\r\n").toUtf8();
    if (!m_tcpSocket || m_tcpSocket->state() != QTcpSocket::ConnectedState) {
        qCritical() << "[开关][" << m_controlName << "] 未连接，发送失败：" << sendCmd;
        return;
    }

    qint64 bytesSent = m_tcpSocket->write(cmdData);
    if (bytesSent == -1) {
        qCritical() << "[开关][" << m_controlName << "] 命令发送失败：" << sendCmd;
        m_isOn = !m_isOn;
        m_switchBtn->setChecked(m_isOn);
        updateSwitchStyle();
    } else {
        qInfo() << "[开关][" << m_controlName << "] 命令发送成功：" << sendCmd;
    }
}

void NetworkSwitchWidget::setIsOn(bool isOn){
    if (m_isOn != isOn) {
        m_isOn = isOn;
        m_switchBtn->setChecked(isOn);
        updateSwitchStyle();
    }
}
