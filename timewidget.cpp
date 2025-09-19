#include "timewidget.h"

TimeWidget::TimeWidget(QWidget *parent, const StyleConfig &style):CustomWidget(parent,style) {
    this->setTitle("");
    this->setMinimumSize(200, 100);
    //初始化时间
    m_dateLabel=addContentLabel("");
    m_dateLabel->setStyleSheet("color: white; font-size: 14px; text-align: center;");
    m_dateLabel->setAlignment(Qt::AlignCenter);

    m_timeLabel = addContentLabel("");
    m_timeLabel->setStyleSheet("color: white; font-size: 18px; font-weight: bold; text-align: center;");
    m_timeLabel->setAlignment(Qt::AlignCenter);
    //初始化定时器
    m_timer=new QTimer(this);
    connect(m_timer,&QTimer::timeout,this,[&](){   
        QDateTime current=QDateTime::currentDateTime();
        QString week;
        switch (current.date().dayOfWeek()) {
        case 1:week="星期一";break;
        case 2:week="星期二";break;
        case 3:week="星期三";break;
        case 4:week="星期四";break;
        case 5:week="星期五";break;
        case 6:week="星期六";break;
        case 7:week="星期日";break;
        }
        // 日期格式：2024年05月20日 星期一
        QString dateText = current.toString("yyyy年MM月dd日 ")+week;
        // 时间格式：15:30:45
        QString timeText = current.toString("HH:mm:ss");
        m_timeLabel->setText(timeText);
        m_dateLabel->setText(dateText);
    });
    m_timer->start(1000);
}

TimeWidget::~TimeWidget(){
    if (m_timer) {
        m_timer->stop();
    }
}
