#include "databaseutil.h"

dataBaseUtil::dataBaseUtil() {}

dataBaseUtil::dataBaseUtil(const QString &dataBaseName){
    connectDataBase(dataBaseName);
}

dataBaseUtil::~dataBaseUtil(){
    if(pQuery)
        delete pQuery;
    db.close();
}
bool dataBaseUtil::connectDataBase(const QString &dataBaseName){
    db=QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(dataBaseName);
    if(db.open()){
        qDebug()<<"数据库连接成功";
        pQuery=new QSqlQuery;
        return true;
    }else{
        qDebug()<<"数据库连接失败";
        return false;
    }

}
QSqlQuery* dataBaseUtil::exec(const QString &sql){
    if(pQuery->exec(sql)){
        qDebug()<<"sql执行成功";
    }else{
        qDebug()<<"sql执行失败"<<pQuery->lastError().text();
    }
    return pQuery;
}
