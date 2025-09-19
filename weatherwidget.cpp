#include "weatherwidget.h"
WeatherWidget::WeatherWidget(QWidget *parent):CustomWidget(parent) {
    // 初始化数据变量
    m_lat = 0.0;
    m_lon = 0.0;
    m_temperature = 0.0;
    m_feelsLike = 0.0;
    m_weatherCode = -1;
    m_windSpeed = 0.0;
    StyleConfig style;
    style.bgColor="#3A9FEA";
    style.titleColor="white";
    this->updateStyle(style);
    this->setFixedSize(240,285);
    //初始化网络工具
    m_netTool=new NetworkTool(this,5000);
    // 连接网络信号
    connect(m_netTool, &NetworkTool::requestFinished, this, &WeatherWidget::onRequestFinished);
    connect(m_netTool, &NetworkTool::requestError, this, &WeatherWidget::onRequestError);
    //初始化组件并定位
    initUI();
    requestLocation();
}

WeatherWidget::~WeatherWidget(){
    if (m_netTool) {
        m_netTool->cancalAllRequest();  // 取消所有未完成请求
        delete m_netTool;
    }
}

void WeatherWidget::initUI(){
    this->setTitle("");
    //主布局
    QWidget*contentWidget=new QWidget(this);
    QVBoxLayout *contentLayout=new QVBoxLayout(contentWidget);
    contentLayout->setSpacing(8);
    contentLayout->setContentsMargins(10,10,10,10);
    //图标+温度布局
    QWidget *topWidget=new QWidget(this);
    QHBoxLayout *topLayout=new QHBoxLayout(topWidget);
    topLayout->setSpacing(10);
    topLayout->setContentsMargins(0,0,0,0);
    //天气图标
    m_weatherIconLabel=new QLabel(this);
    m_weatherIconLabel->setFixedSize(60,60);
    m_weatherIconLabel->setStyleSheet("background: transparent;");
    m_weatherIconLabel->setAlignment(Qt::AlignCenter);
    topLayout->addWidget(m_weatherIconLabel);
    //温度
    QLabel*tempLabel=new QLabel("--- °C",this);
    tempLabel->setStyleSheet("color: white; font-size: 28px; font-weight: bold; background: transparent;");
    tempLabel->setAlignment(Qt::AlignVCenter);
    topLayout->addWidget(tempLabel);
    topLayout->addStretch();//自适应

    contentLayout->addWidget(topWidget);

    //详细信息
    QLabel*cityLabel=new QLabel("城市：--",this);
    QLabel*feelLabel=new QLabel("体感温度：--",this);
    QLabel*statusLabel=new QLabel("天气状况：--",this);
    QLabel*windLabel=new QLabel("风速：--",this);
    QLabel*tempRangeLabel=new QLabel("温度范围：--",this);
    QLabel*sunLabel=new QLabel("日出/日落：--",this);
    // 设置标签样式
    QString labelStyle = "color: white; font-size: 13px; background: transparent;";
    cityLabel->setStyleSheet(labelStyle);
    feelLabel->setStyleSheet(labelStyle);
    statusLabel->setStyleSheet(labelStyle);
    windLabel->setStyleSheet(labelStyle);
    tempRangeLabel->setStyleSheet(labelStyle);
    sunLabel->setStyleSheet(labelStyle);

    // 添加到布局
    contentLayout->addWidget(cityLabel);
    contentLayout->addWidget(feelLabel);
    contentLayout->addWidget(statusLabel);
    contentLayout->addWidget(windLabel);
    contentLayout->addWidget(tempRangeLabel);
    contentLayout->addWidget(sunLabel);

    // 将内容部件添加到自定义widget
    addContentWidget(contentWidget);

}

//定位
void WeatherWidget::requestLocation(){
    QUrl url("http://ip-api.com/json/?fields=status,message,city,lat,lon");
    m_locationRequestId =m_netTool->get(url);
}
//天气
void WeatherWidget::requestWeather(double latitude, double longitude){
    QString urlStr = QString("https://api.open-meteo.com/v1/forecast?"
                             "latitude=%1&longitude=%2&timezone=auto&"
                             "current=temperature_2m,apparent_temperature,weather_code,wind_speed_10m&"
                             "daily=temperature_2m_max,temperature_2m_min,sunrise,sunset&"
                             "forecast_days=1")
                         .arg(latitude)
                         .arg(longitude);

    QUrl url(urlStr);
    m_weatherRequestId =m_netTool->get(url);
}

void WeatherWidget::onRequestFinished(int requestId, const QByteArray &data){
    if(requestId==m_locationRequestId){
         parseLocationData(data);
    }else{
        parseWeatherData(data);
    }
}

void WeatherWidget::onRequestError(int requestId, const QString &errorMsg){
    if(requestId==m_locationRequestId){
        qDebug()<<"定位请求错误："<<errorMsg;
    }else{
        qDebug()<<"天气请求错误："<<errorMsg;
    }
}

void WeatherWidget::parseLocationData(const QByteArray &data){
    QJsonDocument doc=QJsonDocument::fromJson(data);
    if(doc.isNull()){
        qDebug()<<"定位信息解析失败";
        return;
    }
    QJsonObject obj=doc.object();
    if(obj["status"].toString()!="success"){
        qDebug()<<"定位失败："<<obj["message"].toString();
        return;
    }
    //提取经纬度
    m_cityName=obj["city"].toString()=="Changsha"?"长沙":obj["city"].toString();//后期需要维护城市拼音-城市中文名的数据库表
    m_lat=obj["lat"].toDouble();
    m_lon=obj["lon"].toDouble();
    qDebug() << "定位成功:" << m_cityName << "(" << m_lat << "," << m_lon << ")";

    // 定位成功后请求天气数据
    if (m_lat != 0 && m_lon != 0) {
        requestWeather(m_lat, m_lon);
    }
}

void WeatherWidget::parseWeatherData(const QByteArray &data){
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) {
        qDebug() << "解析天气数据失败";
        return;
    }

    QJsonObject root = doc.object();

    // 解析当前天气数据
    QJsonObject current = root["current"].toObject();
    m_temperature = current["temperature_2m"].toDouble();
    m_feelsLike = current["apparent_temperature"].toDouble();
    m_weatherCode = current["weather_code"].toInt();
    m_windSpeed = current["wind_speed_10m"].toDouble();

    // 解析每日天气数据
    QJsonObject daily = root["daily"].toObject();
    m_tempMax = daily["temperature_2m_max"].toArray()[0].toDouble();
    m_tempMin = daily["temperature_2m_min"].toArray()[0].toDouble();
    m_sunrise = daily["sunrise"].toArray()[0].toString().split("T").last();
    m_sunset = daily["sunset"].toArray()[0].toString().split("T").last();

    // 更新显示
    updateWeatherDisplay();
}

void WeatherWidget::updateWeatherDisplay(){
    // 获取内容区的所有标签
    QList<QLabel*> labelList;
    QWidget *contentWidget = m_contentLayout->itemAt(0)->widget();
    if (contentWidget) {
        // 从内容部件中查找所有标签
        for (QWidget *child : contentWidget->findChildren<QWidget*>()) {
            if (QLabel *label = qobject_cast<QLabel*>(child)) {
                labelList.append(label);
            }
        }
    }
    // 设置天气图标
    QString iconPath = getWeatherIconPath(m_weatherCode);
    QPixmap weatherIcon(iconPath);
    if (!weatherIcon.isNull()) {
        m_weatherIconLabel->setPixmap(weatherIcon.scaled(
            m_weatherIconLabel->size(),
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation
            ));
    } else {
        m_weatherIconLabel->setText("?");
        m_weatherIconLabel->setStyleSheet("color: white; font-size: 24px; background: transparent;");
    }

    // 更新温度和详细信息（标签索引对应initUI中的添加顺序，但是注意索引0为空要从1开始）
    if (labelList.size() >= 7) {
        // 温度标签（索引1）
        labelList.at(1)->setText(QString("%1 °C").arg(m_temperature));

        // 城市标签（索引2）
        labelList.at(2)->setText(QString("城市: %1").arg(m_cityName));

        // 体感温度（索引3）
        labelList.at(3)->setText(QString("体感温度: %1 °C").arg(m_feelsLike));

        // 天气状况（索引4）
        QString weatherDesc;
        if (m_weatherCode >= 0 && m_weatherCode <= 3) weatherDesc = "晴朗/少云/多云";
        else if (m_weatherCode >= 45 && m_weatherCode <= 48) weatherDesc = "雾/轻雾";
        else if ((m_weatherCode >= 51 && m_weatherCode <= 67) || (m_weatherCode >= 80 && m_weatherCode <= 86)) weatherDesc = "降雨";
        else if (m_weatherCode >= 71 && m_weatherCode <= 77) weatherDesc = "降雪";
        else if (m_weatherCode >= 95 && m_weatherCode <= 99) weatherDesc = "雷暴";
        else weatherDesc = "未知天气";
        labelList.at(4)->setText(QString("天气状况: %1").arg(weatherDesc));

        // 风速（索引5）
        labelList.at(5)->setText(QString("风速: %1 km/h").arg(m_windSpeed));

        // 温度范围（索引6）
        labelList.at(6)->setText(QString("温度范围: %1 ~ %2 °C")
                               .arg(m_tempMin).arg(m_tempMax));

        // 日出日落（索引7）
        labelList.at(7)->setText(QString("日出/日落: %1 / %2").arg(m_sunrise).arg(m_sunset));
    }

}

QString WeatherWidget::getWeatherIconPath(int weatherCode)
{
    // 天气图标路径映射（请替换为实际图标资源路径）
    if (weatherCode >= 0 && weatherCode <= 3) {
        return "icons\\clear.svg";        // 晴朗/少云/多云
    } else if (weatherCode >= 45 && weatherCode <= 48) {
        return "icons\\fog.svg";          // 雾/轻雾
    } else if ((weatherCode >= 51 && weatherCode <= 67) ||
               (weatherCode >= 80 && weatherCode <= 82)) {
        return "icons\\rain.svg";         // 降雨
    } else if (weatherCode >= 83 && weatherCode <= 86) {
        return "icons\\heavy-rain.svg";   // 强降雨
    } else if (weatherCode >= 71 && weatherCode <= 77) {
        return "icons\\snow.svg";         // 降雪
    } else if (weatherCode >= 95 && weatherCode <= 99) {
        return "icons\\thunderstorm.svg"; // 雷暴
    } else {
        return "icons\\unknown.svg";      // 未知天气
    }
}
