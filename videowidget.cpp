#include "VideoWidget.h"


VideoWidget::VideoWidget(QWidget *parent, const StyleConfig &style)
    : CustomWidget(parent, style), currentVideoIndex(-1), isFullScreen(false),
    videoMenu(nullptr), hideControlsTimer(nullptr), originalParent(nullptr),
    originalLayout(nullptr), originalIndex(-1)
{
    //样式
    StyleConfig temp;
    temp.bgColor="#2C2F39";
    temp.borderRadius=8;
    this->updateStyle(temp);

    initUI();

    // 初始化隐藏控制按钮的计时器
    hideControlsTimer = new QTimer(this);
    hideControlsTimer->setSingleShot(true);
    connect(hideControlsTimer, &QTimer::timeout, this, &VideoWidget::hideControls);

    loadVideoFiles();

}

VideoWidget::~VideoWidget()
{
    // 释放多媒体资源
    if (player) {
        player->stop();
        delete player;
    }
}

// 初始化UI布局与组件
void VideoWidget::initUI()
{
    this->setTitle("");
    QAudioOutput *audioOutput=new QAudioOutput(this);
    player = new QMediaPlayer(this);
    videoWidget = new QVideoWidget(this);
    player->setAudioOutput(audioOutput);
    player->setVideoOutput(videoWidget);

    // 创建控制按钮容器
    controlsWidget=new QWidget(videoWidget);
    controlsWidget->setStyleSheet("background-color: rgba(0, 0, 0, 0.6); border-radius: 0px;");
    controlsWidget->setFixedHeight(60);
    // 创建控制按钮布局
    QHBoxLayout *controlsLayout = new QHBoxLayout(controlsWidget);
    controlsLayout->setContentsMargins(10, 5, 10, 5);
    controlsLayout->setSpacing(10);

    // 视频选择按钮
    videoSelectButton = new QToolButton(controlsWidget);
    videoSelectButton->setIcon(QIcon("icons\\video_list.png"));
    videoSelectButton->setIconSize(QSize(24, 24));
    videoSelectButton->setPopupMode(QToolButton::InstantPopup);
    videoSelectButton->setStyleSheet("QToolButton { "
                                     "background-color: #3A3D49; "
                                     "color: white; "
                                     "border-radius: 4px; "
                                     "padding: 8px 12px; "
                                     "min-width: 80px; "
                                     "}");
    controlsLayout->addWidget(videoSelectButton);
    // 上一集按钮
    previousButton = new QPushButton(controlsWidget);
    previousButton->setIcon(QIcon("icons\\previous.png"));
    previousButton->setIconSize(QSize(24, 24));
    previousButton->setStyleSheet("QPushButton { "
                                  "background-color: #3A3D49; "
                                  "color: white; "
                                  "border-radius: 4px; "
                                  "padding: 8px 12px; "
                                  "min-width: 70px; "
                                  "}");
    controlsLayout->addWidget(previousButton);

    // 播放/暂停按钮
    playPauseButton = new QPushButton(controlsWidget);
    playPauseButton->setIcon(QIcon("icons\\pause.png"));
    playPauseButton->setIconSize(QSize(24, 24));
    playPauseButton->setStyleSheet("QPushButton { "
                                   "background-color: #3A3D49; "
                                   "color: white; "
                                   "border-radius: 4px; "
                                   "padding: 8px 12px; "
                                   "min-width: 70px; "
                                   "}");
    controlsLayout->addWidget(playPauseButton);

    // 下一集按钮
    nextButton = new QPushButton(controlsWidget);
    nextButton->setIcon(QIcon("icons\\next.png"));
    nextButton->setIconSize(QSize(24, 24));
    nextButton->setStyleSheet("QPushButton { "
                              "background-color: #3A3D49; "
                              "color: white; "
                              "border-radius: 4px; "
                              "padding: 8px 12px; "
                              "min-width: 70px; "
                              "}");
    controlsLayout->addWidget(nextButton);

    // 进度条
    positionSlider = new QSlider(Qt::Horizontal, controlsWidget);
    positionSlider->setStyleSheet("QSlider::groove:horizontal { "
                                  "border: 1px solid #3A3D49; "
                                  "height: 8px; "
                                  "background: #3A3D49; "
                                  "border-radius: 4px; "
                                  "}"
                                  "QSlider::handle:horizontal { "
                                  "background: white; "
                                  "border: 1px solid #5c5c5c; "
                                  "width: 18px; "
                                  "margin: -2px 0; "
                                  "border-radius: 3px; "
                                  "}"
                                  "QSlider::sub-page:horizontal { "
                                  "background: #5A9AE6; "
                                  "border-radius: 4px; "
                                  "}");
    controlsLayout->addWidget(positionSlider, 1); // 进度条占据剩余空间
    // 全屏按钮
    fullscreenButton = new QPushButton(controlsWidget);
    fullscreenButton->setIcon(QIcon("icons\\fullscreen.png"));
    fullscreenButton->setIconSize(QSize(24, 24));
    fullscreenButton->setStyleSheet("QPushButton { "
                                    "background-color: #3A3D49; "
                                    "color: white; "
                                    "border-radius: 4px; "
                                    "padding: 8px 12px; "
                                    "min-width: 70px; "
                                    "}");
    controlsLayout->addWidget(fullscreenButton);

    //初始隐藏控制按钮
    controlsWidget->hide();

    //安装事件过滤器
    videoWidget->installEventFilter(this);

    // 调整控制按钮位置
    adjustControlPosition();

    // 组合视频窗口与控制栏
    QWidget *contentWidget = new QWidget(this);
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);
    contentLayout->addWidget(videoWidget, 1);
    contentLayout->addWidget(controlsWidget);

    addContentWidget(contentWidget);

    connect(playPauseButton, &QPushButton::clicked, this, &VideoWidget::togglePlayPause);
    connect(previousButton, &QPushButton::clicked, this, &VideoWidget::playPrevious);
    connect(nextButton, &QPushButton::clicked, this, &VideoWidget::playNext);
    connect(fullscreenButton, &QPushButton::clicked, this, &VideoWidget::toggleFullscreen);
    // 播放器连接
    connect(player, &QMediaPlayer::positionChanged, this, &VideoWidget::updatePosition);
    connect(player, &QMediaPlayer::durationChanged, this, &VideoWidget::updateDuration);
    connect(positionSlider, &QSlider::sliderMoved, this, &VideoWidget::setPosition);
    // 视频选择按钮点击
    connect(videoSelectButton, &QToolButton::clicked, this, &VideoWidget::updateVideoMenu);

}

// 加载项目路径下videos目录的视频（支持mp4/avi/mov/mkv格式）
void VideoWidget::loadVideoFiles(const QString &directory)
{
    //查看是否存在目录
    QDir dir(directory);
    if(!dir.exists()){
        qDebug() << "目录不存在，尝试创建:" << directory;
        if (!dir.mkpath(".")) {
            qDebug() << "无法创建目录:" << directory;
            return;
        }
    }
    //获取视频文件
    QStringList filters;
    filters << "*.mp4" << "*.avi" << "*.mkv" << "*.mov" << "*.wmv";
    QStringList fileList=dir.entryList(filters,QDir::Files);
    if (fileList.isEmpty()) {
        qDebug() << "在目录" << directory << "中未找到视频文件";
        return;
    }
    videoFiles.clear();
    for (const auto &item : fileList) {
        QString filePath = dir.absoluteFilePath(item);
        videoFiles.append(filePath);
    }
    if (!videoFiles.isEmpty()) {
        // playVideo(0);
        createVideoMenu();
    }
}

void VideoWidget::playVideo(int index){
    if (index < 0 || index >= videoFiles.size()) return;
    currentVideoIndex = index;
    // 检查文件是否存在
    QFileInfo fileInfo(videoFiles[index]);
    if (!fileInfo.exists()) {
        qDebug() << "视频文件不存在:" << videoFiles[index];
        return;
    }
    player->setSource(QUrl::fromLocalFile(videoFiles[index]));
    player->play();
    // 更新视频菜单
    createVideoMenu();
}

void VideoWidget::togglePlayPause(){
    if(player->playbackState()==QMediaPlayer::PlayingState){
        player->pause();
        playPauseButton->setIcon(QIcon("icons\\pause.png"));
    }else{
        player->play();
        playPauseButton->setIcon(QIcon("icons\\play.png"));
    }
}

void VideoWidget::playNext(){
    playVideo((currentVideoIndex+1)%videoFiles.size());
}

void VideoWidget::playPrevious(){
    playVideo((currentVideoIndex-1+videoFiles.size())%videoFiles.size());
}
void VideoWidget::updatePosition(qint64 position){
    positionSlider->setValue(position);
}

void VideoWidget::updateDuration(qint64 duration){
    positionSlider->setRange(0, duration);
}

void VideoWidget::setPosition(int position)
{
    player->setPosition(position);
}

void VideoWidget::hideControls()
{
    controlsWidget->hide();
}

void VideoWidget::toggleFullscreen(){
    if(isFullScreen){
        exitFullscreen();
    }else{
        enterFullscreen();
    }
}

void VideoWidget::enterFullscreen()
{
    if (isFullScreen) return;
    // 记录原始状态
    originalParent = videoWidget->parentWidget();
    originalLayout = qobject_cast<QLayout*>(m_contentLayout);
    // 找到原始布局中的索引
    for (int i = 0; i < m_contentLayout->count(); ++i) {
        if (m_contentLayout->itemAt(i)->widget() == videoWidget) {
            originalIndex = i;
            break;
        }
    }
    // 将视频控件提升到主窗口级别
    QWidget *mainWindow = this->window();
    videoWidget->setParent(mainWindow);
    videoWidget->setGeometry(mainWindow->rect());
    videoWidget->raise();
    videoWidget->show();
    // 隐藏其他控件
    this->hide();
    isFullScreen = true;
    // 安装主窗口事件过滤器以处理ESC键
    mainWindow->installEventFilter(this);
}

void VideoWidget::exitFullscreen()
{
    if (!isFullScreen) return;
    // 恢复原始状态
    videoWidget->setParent(originalParent);
    // 如果找到了原始布局和索引，则恢复
    if (originalLayout && originalIndex >= 0) {
        m_contentLayout->insertWidget(originalIndex, videoWidget);
    } else {
        m_contentLayout->insertWidget(0, videoWidget);
    }
    // 显示其他控件
    this->show();
    isFullScreen = false;
    // 移除主窗口事件过滤器
    this->window()->removeEventFilter(this);
}

void VideoWidget::updateVideoMenu()
{
    createVideoMenu();
}

void VideoWidget::createVideoMenu(){
    if(videoMenu){
        videoMenu->deleteLater();
    }
    videoMenu=new QMenu(this);
    videoMenu->setStyleSheet("QMenu { "
                             "background-color: #2C2F39; "
                             "color: white; "
                             "min-width: 120px; "
                             "}"
                             "QMenu::item:selected { "
                             "background-color: #5A9AE6; "
                             "}");
    for(int i=0;i<videoFiles.size();i++){
        QFileInfo fileInfo(videoFiles[i]);
        QAction *action=new QAction(fileInfo.fileName(),videoMenu);
        action->setData(i);//存储视频索引
        //设置选中状态
        if(currentVideoIndex==i){
            QFont font = action->font();
            font.setBold(true);
            action->setFont(font);
        }
        connect(action, &QAction::triggered, [=]() {
            playVideo(i);
        });
        videoMenu->addAction(action);
    }
    videoSelectButton->setMenu(videoMenu);
    videoSelectButton->setPopupMode(QToolButton::InstantPopup);
}

void VideoWidget::adjustControlPosition(){
    if (controlsWidget && videoWidget) {
        // 确保控制按钮容器已经计算了正确的尺寸
        controlsWidget->adjustSize();
        // 计算居中位置
        int x = (videoWidget->width() - controlsWidget->width()) / 2;
        int y = videoWidget->height() - controlsWidget->height() - 10;
        // 确保位置不会超出边界
        if (x < 0) x = 0;
        if (y < 0) y = 0;
        controlsWidget->move(x, y);
    }
}


void VideoWidget::showControls()
{
    // 停止隐藏计时器
    hideControlsTimer->stop();
    // 显示控制按钮
    controlsWidget->show();
    // 重新启动隐藏计时器
    hideControlsTimer->start(3000); // 3秒后隐藏控制按钮
}

// 事件过滤器：鼠标进入视频区域显示控制栏，离开隐藏
bool VideoWidget::eventFilter(QObject *watched, QEvent *event)
{

    if (watched == videoWidget) {
        if (event->type() == QEvent::Enter) {
            showControls();
        } else if (event->type() == QEvent::Leave) {
            hideControlsTimer->start(2000); // 2秒后隐藏控制按钮
        } else if (event->type() == QEvent::MouseButtonPress) {
            // 点击视频区域切换播放/暂停
            togglePlayPause();
        } else if (event->type() == QEvent::Resize) {
            // 调整控制按钮位置
            adjustControlPosition();
        }
    }else if (isFullScreen && watched == this->window() && event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Escape) {
            exitFullscreen();
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
}
