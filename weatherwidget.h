#ifndef WEATHERWIDGET_H
#define WEATHERWIDGET_H

#include <QObject>
#include "customwidget.h"
#include "networktool.h"
#include <QLabel>
#include <QPixmap>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
class WeatherWidget : public CustomWidget
{
public:
    explicit WeatherWidget(QWidget *parent = nullptr);
    ~WeatherWidget();
private slots:
    // 处理网络请求完成信号
    void onRequestFinished(int requestId, const QByteArray &data);
    // 处理网络请求错误信号
    void onRequestError(int requestId, const QString &errorMsg);
private:
    //定位信息
    QString m_cityName;             //城市名称
    double m_lat;                   //纬度
    double m_lon;                   //经度
    //天气数据
    double m_temperature;           // 当前温度
    double m_feelsLike;             // 体感温度
    int m_weatherCode;              // 天气代码
    double m_windSpeed;             // 风速
    QString m_sunrise;              // 日出时间
    QString m_sunset;               // 日落时间
    double m_tempMax;               // 最高温度
    double m_tempMin;               // 最低温度
    //网络和图标管理
    NetworkTool *m_netTool;         // 网络管理器
    QLabel *m_weatherIconLabel;     // 天气图标标签
    // 请求ID（用于区分不同请求）
    int m_locationRequestId;        // 定位请求ID
    int m_weatherRequestId;         // 天气请求ID

    // 初始化UI
    void initUI();
    // 获取IP定位信息
    void requestLocation();
    // 根据经纬度请求天气数据
    void requestWeather(double latitude, double longitude);
    // 解析定位数据
    void parseLocationData(const QByteArray &data);
    // 解析天气数据
    void parseWeatherData(const QByteArray &data);
    // 更新天气显示
    void updateWeatherDisplay();
    // 获取天气图标路径
    QString getWeatherIconPath(int weatherCode);

};

#endif // WEATHERWIDGET_H
