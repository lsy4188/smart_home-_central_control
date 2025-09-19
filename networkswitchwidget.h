#ifndef NETWORKSWITCHWIDGET_H
#define NETWORKSWITCHWIDGET_H

#include <QObject>
#include "customwidget.h"
#include <QTcpSocket>
#include <QCheckBox>
#include <QTimer>
class NetworkSwitchWidget : public CustomWidget
{
    Q_OBJECT
public:
    explicit NetworkSwitchWidget(QWidget *parent = nullptr,
                                 QTcpSocket *tcpSocket = nullptr,  // TCP通信对象
                                 const QString &controlName = "",   // 控件显示名称
                                 const QString &onCommand = "",     // 开状态命令
                                 const QString &offCommand = "",    // 关状态命令
                                 bool initIsOn = false);            // 初始状态（默认关闭）

    ~NetworkSwitchWidget();

    void setSocket(QTcpSocket* socket);
    // 外部更新开关状态（用于硬件反馈同步）
    void setIsOn(bool isOn);
    // 获取当前开关状态
    bool getIsOn() const { return m_isOn; }

private slots:
    // 开关点击事件处理
    void onSwitchClicked();
    //处理连接断开
    void onDisconnected();

private:
    // 初始化UI布局
    void initSwitchUI();
    // 根据状态更新开关样式
    void updateSwitchStyle();
    //更新连接状态显示
    void updateConnectionStatus();

    // 成员变量
    QTcpSocket *m_tcpSocket;    // TCP通信套接字
    QString m_controlName;      // 控件显示名称
    QString m_onCommand;        // 开命令
    QString m_offCommand;       // 关命令
    bool m_isOn;                // 当前开关状态（true=开，false=关）
    QCheckBox *m_switchBtn;   // 开关按钮
    QLabel *m_statusLabel;
};

#endif // NETWORKSWITCHWIDGET_H
