#ifndef DATABASEUTIL_H
#define DATABASEUTIL_H
/*
 * 数据库连接工具类
 */
#include <QMessageBox>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include<QString>
class dataBaseUtil
{
public:
    dataBaseUtil();
    dataBaseUtil(const QString &dataBaseName);
    ~dataBaseUtil();
    bool connectDataBase(const QString &dataBaseName);
    QSqlQuery* exec(const QString &sql);
private:
    QSqlDatabase db;
    QSqlQuery *pQuery;
};

#endif // DATABASEUTIL_H
