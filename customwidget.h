#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H
/*
 * 自定义窗口
*/
#include <QObject>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QList>
#include <QVariant>
#include <functional>
class CustomWidget : public QWidget
{
    Q_OBJECT
public:
    //样式配置
    struct StyleConfig {
        QString bgColor;       // 背景色
        QString titleColor;      // 标题颜色
        QString titleFontSize;    // 标题字体大小
        int borderRadius;              // 圆角半径
        int padding ;                  // 内边距
        int spacing ;                   // 内部间距
        StyleConfig()
            : bgColor("#2C2F39"),
            titleColor("white"),
            titleFontSize("16px"),
            borderRadius(8),
            padding(10),
            spacing(8)
        {}
    };

    // 按钮配置
    struct ButtonConfig {
        QString text;                      // 按钮文本
        QString icon;                      // 图标路径(可选)
        std::function<void()> callback;    // 点击回调
    };
    explicit CustomWidget(QWidget *parent = nullptr,const StyleConfig &style = StyleConfig());
    // 设置卡片标题
    void setTitle(const QString &title);

    // 清空内容区
    void clearContent();

    // 向内容区添加自定义部件
    void addContentWidget(QWidget *widget, int stretch = 0);

    // 向内容区添加文本标签
    QLabel* addContentLabel(const QString &text, const QString &style = "color: white;");

    // 设置底部操作按钮
    void setActionButtons(const QList<ButtonConfig> &buttons);

    // 修改卡片样式
    void updateStyle(const StyleConfig &style);

protected:
    StyleConfig m_style;                  // 样式配置
    QVBoxLayout *m_mainLayout;            // 主布局
    QLabel *m_titleLabel;                 // 标题标签
    QWidget *m_contentContainer;          // 内容容器
    QWidget *m_actionContainer;           // 操作区容器
    QHBoxLayout *m_actionLayout;          // 操作区布局
    QVBoxLayout *m_contentLayout;         // 内容布局
    // 应用样式到部件
    void applyStyle();
};

#endif // CUSTOMWIDGET_H
