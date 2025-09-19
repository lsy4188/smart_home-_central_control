#include "networktool.h"

NetworkTool::NetworkTool(QObject *parent,int timeOut)
    : QObject{parent},nextRequestId(1),requstTimeout(timeOut)
{
    netWorkManager=new QNetworkAccessManager();
}

 NetworkTool::~NetworkTool(){
     cancalAllRequest();
 }


 //Get请求
 int NetworkTool::get(const QUrl &url,const QMap<QString,QString> &header){
     //校验url的合法性
     if(!url.isValid()){
         emit requestError(-1,"无效的url");
         return -1;
     }
     //发送Get请求
     QNetworkRequest requestUrl(url);
     this->setupRequestHeaders(requestUrl,header);
     QNetworkReply * reply=netWorkManager->get(requestUrl);
     int requestId=generateRequestId();
     //信号关联
     connect(reply,&QNetworkReply::finished,this,&NetworkTool::onReplyFinished);
     connect(reply,&QNetworkReply::errorOccurred,this,&NetworkTool::onReplyError);
     connect(reply,&QNetworkReply::downloadProgress,this,&NetworkTool::onReplyProgress);
     //请求和超时管理
     repliesMap.insert(requestId,reply);
     QTimer *timer=new QTimer(this);
     timer->setSingleShot(true);
     timer->start(requstTimeout);
     timeoutTimerMap.insert(requestId,timer);
     connect(timer,&QTimer::timeout,this,[=](){
         ontimeOut(requestId);
     });
     return requestId;
 }
 //Post请求
 int NetworkTool::post(const QUrl &url,const QByteArray &data,const QMap<QString,QString> &header){
     //校验url的合法性
     if(!url.isValid()){
         emit requestError(-1,"无效的url");
         return -1;
     }
     //发送Post请求
     QNetworkRequest requestUrl(url);
     this->setupRequestHeaders(requestUrl,header);
     if(!header.contains("Content-Type")){
         requestUrl.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
     }
     QNetworkReply * reply=netWorkManager->post(requestUrl,data);
     int requestId=generateRequestId();
     // 关联信号槽
     connect(reply, &QNetworkReply::finished, this, &NetworkTool::onReplyFinished);
     connect(reply, &QNetworkReply::errorOccurred, this, &NetworkTool::onReplyError);
     connect(reply, &QNetworkReply::uploadProgress, this, &NetworkTool::onReplyProgress);
     //请求和超时管理
     repliesMap.insert(requestId,reply);
     QTimer *timer=new QTimer(this);
     timer->setSingleShot(true);
     timer->start(requstTimeout);
     timeoutTimerMap.insert(requestId,timer);
     connect(timer,&QTimer::timeout,this,[=](){
         ontimeOut(requestId);
     });
     return requestId;
 }

 //取消请求
 void NetworkTool::cancalAllRequest(){
     for (auto &reply : repliesMap) {
         if(reply&&reply->isRunning()){
             reply->abort();
         }
     }

     for (auto &timer : timeoutTimerMap) {
         if(timer){
             timer->stop();
             timer->deleteLater();
         }
     }
     repliesMap.clear();
     timeoutTimerMap.clear();
 }

 /*************槽函数*****************/
 void NetworkTool::onReplyFinished(){
     //获取信号发送者
     QNetworkReply* reply=qobject_cast<QNetworkReply*>(sender());
     if(!reply) return;
     //获取请求id
     int requestId=-1;
     for (auto it = repliesMap.begin(); it != repliesMap.end(); ++it) {
         if (it.value() == reply) {
             requestId = it.key();
             break;
         }
     }
     if(requestId==-1){
         reply->deleteLater();
         return;
     }
     //处理数据
     if(reply->error()==QNetworkReply::NoError){
         emit requestFinished(requestId,reply->readAll());
     }
     //清理资源
     reply->deleteLater();
     repliesMap.remove(requestId);
     if(timeoutTimerMap.contains(requestId)){
         timeoutTimerMap.find(requestId).value()->stop();
         timeoutTimerMap.find(requestId).value()->deleteLater();
         timeoutTimerMap.remove(requestId);
     }

 }
 void NetworkTool::onReplyError(QNetworkReply::NetworkError error){
     //获取信号发送者
     QNetworkReply* reply=qobject_cast<QNetworkReply*>(sender());
     if(!reply) return;
     // 查找请求ID
     int requestId = -1;
     for (auto it = repliesMap.begin(); it != repliesMap.end(); ++it) {
         if (it.value() == reply) {
             requestId = it.key();
             break;
         }
     }

     if (requestId != -1) {
         emit requestError(requestId, reply->errorString());
     }
 }
 void NetworkTool::onReplyProgress(qint64 bytesReceived, qint64 bytesTotal){
     //获取信号发送者
     QNetworkReply* reply=qobject_cast<QNetworkReply*>(sender());
     if(!reply) return;
     // 查找请求ID
     int requestId = -1;
     for (auto it = repliesMap.begin(); it != repliesMap.end(); ++it) {
         if (it.value() == reply) {
             requestId = it.key();
             break;
         }
     }

     if (requestId != -1) {
         emit requestProgress(requestId, bytesReceived, bytesTotal);
     }
 }
 void NetworkTool::ontimeOut(const int requestId){
     if(repliesMap.contains(requestId)){
         QNetworkReply* reply=repliesMap.find(requestId).value();
         if(reply->isRunning()){
             reply->abort();
             emit requestError(requestId,"连接超时");
         }
         // 清理资源
         if (reply) {
             reply->deleteLater();
         }
         repliesMap.remove(requestId);

         if (timeoutTimerMap.contains(requestId)) {
             timeoutTimerMap[requestId]->deleteLater();
             timeoutTimerMap.remove(requestId);
         }
     }
 }

 /***************私有函数***************/
//生成请求id
 int NetworkTool::generateRequestId(){
     return nextRequestId++;
 }
 //设置请求头
 void NetworkTool::setupRequestHeaders(QNetworkRequest &request, const QMap<QString, QString> &headers){
     // 设置默认User-Agent
     if(!headers.contains("User-Agent")){
         request.setHeader(QNetworkRequest::UserAgentHeader,"Mozilla/5.0 (compatible; NetworkTool/1.0)");
     }
     // 设置自定义请求头
     for (auto it = headers.begin(); it != headers.end(); ++it) {
         request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
     }
 }


