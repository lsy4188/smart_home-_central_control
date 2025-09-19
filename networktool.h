#ifndef NETWORKTOOL_H
#define NETWORKTOOL_H

#include <QObject>
#include <QUrl>
#include <QMap>
#include <QByteArray>
#include <QNetworkReply>
#include <QTimer>
#include <QString>
#include <QDebug>
class NetworkTool : public QObject
{
    Q_OBJECT
public:
    explicit NetworkTool(QObject *parent = nullptr,int timeOut=1000);
    ~NetworkTool();
    //Get请求
    int get(const QUrl &url,const QMap<QString,QString> &header={});
    //Post请求
    int post(const QUrl &url,const QByteArray &data,const QMap<QString,QString> &header={});
    //取消请求
    void cancalAllRequest();
signals:
    void requestFinished(const int requestId,const QByteArray &requestData);
    void requestError(const int requestId,const QString &errorMsg);
    void requestProgress(int requestId, qint64 bytesReceived, qint64 bytesTotal);
public slots:
    void onReplyFinished();
    void onReplyError(QNetworkReply::NetworkError error);
    void onReplyProgress(qint64 bytesReceived, qint64 bytesTotal);
    void ontimeOut(const int requestId);
private:
    QNetworkAccessManager *netWorkManager;
    QMap<int,QNetworkReply*> repliesMap;        // 管理请求ID和对应的reply
    QMap<int, QTimer*> timeoutTimerMap;        // 管理请求超时计时器
    int nextRequestId;                       // 下一个请求ID
    int requstTimeout;                       // 超时时间(毫秒)
    // 生成唯一请求ID
    int generateRequestId();
    // 设置请求头
    void setupRequestHeaders(QNetworkRequest &request, const QMap<QString, QString> &headers);
};

#endif // NETWORKTOOL_H
