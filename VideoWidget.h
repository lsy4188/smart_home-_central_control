
#ifndef VIDEOPLAYERWIDGET_H
#define VIDEOPLAYERWIDGET_H

#include "customwidget.h"
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>
#include <QSlider>
#include <QPushButton>
#include <QToolButton>
#include <QListWidget>
#include <QProgressDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QEvent>
#include <QKeyEvent>
#include <QStyle>
#include <QTimer>
#include <QDir>
#include<QMenu>

class VideoWidget : public CustomWidget
{
    Q_OBJECT
public:
    explicit VideoWidget(QWidget *parent = nullptr, const StyleConfig &style = StyleConfig());
    ~VideoWidget() override;
    void loadVideoFiles(const QString &directory = "videos\\"); // 加载视频文件

protected:
    // 事件过滤器：监听鼠标进入/离开视频区域
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void playVideo(int index); // 播放指定索引的视频
    void togglePlayPause();    // 切换播放/暂停
    void playNext();           // 播放下一集
    void playPrevious();       // 播放上一集
    void updatePosition(qint64 position); // 更新播放位置
    void updateDuration(qint64 duration); // 更新视频时长
    void setPosition(int position);       // 设置播放位置
    void updateVideoMenu();               // 更新视频菜单
    void toggleFullscreen();   // 切换全屏
    void hideControls();                  // 隐藏控制按钮
    void showControls();                  // 显示控制按钮
private:

    void initUI(); // 初始化UI
    void adjustControlPosition(); // 调整控制按钮位置
    void enterFullscreen();      // 进入全屏
    void exitFullscreen();       // 退出全屏
    void createVideoMenu();  // 创建视频选择菜单

    QMediaPlayer *player;      // 媒体播放器
    QVideoWidget *videoWidget; // 视频显示部件
    QWidget *controlsWidget;   // 控制按钮容器
    QPushButton *playPauseButton;
    QPushButton *previousButton;
    QPushButton *nextButton;
    QPushButton *fullscreenButton;
    QSlider *positionSlider;
    QToolButton *videoSelectButton; // 视频选择按钮

    QMenu *videoMenu;               // 视频选择菜单

    QStringList videoFiles;    // 视频文件列表
    int currentVideoIndex;     // 当前播放的视频索引
    bool isFullScreen;         // 是否全屏状态
    QTimer *hideControlsTimer; // 隐藏控制按钮的计时器

    QWidget *originalParent;   // 记录原始父部件
    QLayout *originalLayout;   // 记录原始布局
    int originalIndex;         // 记录原始布局索引
};

#endif // VIDEOPLAYERWIDGET_H
