#ifndef TIMEWIDGET_H
#define TIMEWIDGET_H
/*
 * 时间标签
 */
#include <QObject>
#include "customwidget.h"
#include<QTimer>
#include <QDateTime>
#include <QDebug>
class TimeWidget : public CustomWidget
{
     Q_OBJECT
public:
   explicit TimeWidget(QWidget *parent = nullptr, const StyleConfig &style = StyleConfig());
    ~TimeWidget();

private:
   QTimer *m_timer;      // 定时器
   QLabel *m_timeLabel;  // 时间显示标签
   QLabel *m_dateLabel;  // 日期显示标签
};

#endif // TIMEWIDGET_H
