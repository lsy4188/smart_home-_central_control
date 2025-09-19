#include "customwidget.h"

CustomWidget::CustomWidget(QWidget *parent,const StyleConfig &style)
    : QWidget{parent},m_style(style)
{
    // 主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(m_style.padding, m_style.padding,
                                     m_style.padding, m_style.padding);
    m_mainLayout->setSpacing(m_style.spacing);

    // 标题区
    m_titleLabel = new QLabel(this);
    m_titleLabel->setStyleSheet(QString("color: %1; font-size: %2; font-weight: bold;")
                                    .arg(m_style.titleColor).arg(m_style.titleFontSize));
    m_mainLayout->addWidget(m_titleLabel);

    // 内容区容器
    m_contentContainer = new QWidget(this);
    m_contentContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_contentLayout = new QVBoxLayout(m_contentContainer);
    m_contentLayout->setContentsMargins(0, 0, 0, 0);
    m_contentLayout->setSpacing(0);
    m_mainLayout->addWidget(m_contentContainer, 1); // 内容区占主要空间

    // 操作区容器
    m_actionContainer = new QWidget(this);
    m_actionLayout = new QHBoxLayout(m_actionContainer);
    m_actionLayout->setContentsMargins(0, 0, 0, 0);
    m_actionLayout->setSpacing(10);
    m_mainLayout->addWidget(m_actionContainer);

    // 应用样式
    applyStyle();
}
void CustomWidget::setTitle(const QString &title)
{
    m_titleLabel->setText(title);
    m_titleLabel->setVisible(!title.isEmpty()); // 空标题则隐藏
}

void CustomWidget::clearContent()
{
    // 移除所有内容部件
    QLayoutItem *item;
    while ((item = m_contentLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
}

void CustomWidget::addContentWidget(QWidget *widget, int stretch)
{
    if (widget) {
        m_contentLayout->addWidget(widget, stretch);
    }
}

QLabel* CustomWidget::addContentLabel(const QString &text, const QString &style)
{
    QLabel *label = new QLabel(text, this);
    label->setStyleSheet(style);
    addContentWidget(label);
    return label;
}

void CustomWidget::setActionButtons(const QList<ButtonConfig> &buttons)
{
    // 清除现有按钮
    QLayoutItem *item;
    while ((item = m_actionLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    // 添加新按钮
    for (const auto &btnCfg : buttons) {
        QPushButton *btn = new QPushButton(btnCfg.text, this);

        // 设置按钮样式
        btn->setStyleSheet("QPushButton {"
                           "background-color: #3A3D49;"
                           "color: white;"
                           "border-radius: 4px;"
                           "padding: 5px 10px;"
                           "}"
                           "QPushButton:hover {"
                           "background-color: #4A4D59;"
                           "}");

        // 绑定回调
        if (btnCfg.callback) {
            connect(btn, &QPushButton::clicked, btnCfg.callback);
        }

        m_actionLayout->addWidget(btn);
    }

    // 如果没有按钮则隐藏操作区
    m_actionContainer->setVisible(!buttons.isEmpty());
}

void CustomWidget::updateStyle(const StyleConfig &style)
{
    m_style = style;
    applyStyle();
}

void CustomWidget::applyStyle()
{
    // 应用卡片样式
    setStyleSheet(QString("background-color: %1; border-radius: %2px;")
                      .arg(m_style.bgColor).arg(m_style.borderRadius));

    // 更新标题样式
    m_titleLabel->setStyleSheet(QString("color: %1; font-size: %2; font-weight: bold;")
                                    .arg(m_style.titleColor).arg(m_style.titleFontSize));

    // 更新布局间距和内边距
    m_mainLayout->setContentsMargins(m_style.padding, m_style.padding,
                                     m_style.padding, m_style.padding);
    m_mainLayout->setSpacing(m_style.spacing);
    m_contentLayout->setSpacing(m_style.spacing);
}

