#include "musicwidget.h"

MusicWidget::MusicWidget(QWidget *parent):CustomWidget(parent) {
    //初始化成员变量
    player=new QMediaPlayer(this);
    QAudioOutput *audioOutPut=new QAudioOutput(this);
    player->setAudioOutput(audioOutPut);
    playlist=new QStringList;
    currentPlayIndex = -1;
    connect(player, &QMediaPlayer::playbackStateChanged, this, &MusicWidget::updatePlayState);
    //样式
    StyleConfig style;
    style.bgColor="#50B8C8";
    style.titleColor="white";
    style.borderRadius=8;
    this->updateStyle(style);
    this->setFixedSize(240, 290);
    //网络工具
    netWorkTool=new NetworkTool(this,5000);
    // 连接网络信号
    connect(netWorkTool, &NetworkTool::requestFinished, this, &MusicWidget::onNetworkFinished);
    connect(netWorkTool, &NetworkTool::requestError, this, &MusicWidget::onNetworkError);
    //初始化UI
    initUI();
    //加载音乐
    requestMusic();
}

MusicWidget::~MusicWidget(){
    if (netWorkTool) {
        netWorkTool->cancalAllRequest();
        delete netWorkTool;
    }
    if (playlist) {
        delete playlist;
    }
}

void MusicWidget::initUI(){
    this->setTitle("");
    //总布局
    QWidget*contentWidget=new QWidget(this);
    QVBoxLayout *mainLayout=new QVBoxLayout(contentWidget);
    mainLayout->setSpacing(25);
    mainLayout->setContentsMargins(20,20,20,0);

    //封面
    coverLabel=new QLabel(this);
    coverLabel->setFixedSize(150,150);
    coverLabel->setAlignment(Qt::AlignCenter);
    coverLabel->setStyleSheet("border-radius: 40px; background-color: #3A3D49; color: white;");
    coverLabel->setText("加载中...");

    //添加阴影效果
    QGraphicsDropShadowEffect *shadowEffect = new QGraphicsDropShadowEffect;
    shadowEffect->setBlurRadius(50);
    shadowEffect->setColor(QColor(0, 0, 0, 100));
    shadowEffect->setOffset(0, 5);
    coverLabel->setGraphicsEffect(shadowEffect);

    mainLayout->addWidget(coverLabel, 0, Qt::AlignCenter);

    //歌曲信息区域
    QWidget *infoWidget = new QWidget(this);
    QVBoxLayout *infoLayout = new QVBoxLayout(infoWidget);
    infoLayout->setContentsMargins(0, 0, 0, 0);
    infoLayout->setSpacing(5);

    //歌名
    songNameLabel=new QLabel(this);
    songNameLabel->setText("歌曲名称");
    songNameLabel->setAlignment(Qt::AlignCenter);
    songNameLabel->setStyleSheet("color: white; font-size: 16px; font-weight: bold;");
    songNameLabel->setWordWrap(true);
    infoLayout->addWidget(songNameLabel);

    mainLayout->addWidget(infoWidget);

    //按钮设置(水平布局)
    QWidget *btnWidget=new QWidget(this);
    QHBoxLayout *btnLayout=new QHBoxLayout(btnWidget);
    btnLayout->setContentsMargins(0, 0, 0, 0);
    btnLayout->setSpacing(15);

    // 上一曲按钮 - 改为图标按钮
    prevBtn = new QPushButton(this);
    prevBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #50B8C8;
            border: none;
            border-radius: 20px;
            padding: 0px;
            min-width: 40px;
            max-width: 40px;
            min-height: 40px;
            max-height: 40px;
        }
        QPushButton:hover {
            background-color: #60C8D8;
        }
        QPushButton:disabled {
            background-color: #2A2D39;
        }
    )");
    prevBtn->setIcon(QIcon("icons\\previous-button.svg")); // 需要添加图标资源
    prevBtn->setIconSize(QSize(20, 20));
    prevBtn->setDisabled(true);
    connect(prevBtn, &QPushButton::clicked, this, &MusicWidget::onPreviousClicked);
    btnLayout->addWidget(prevBtn);

    // 播放/暂停按钮 - 改为图标按钮
    playPauseBtn = new QPushButton(this);
    playPauseBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #50B8C8;
            border: none;
            border-radius: 25px;
            padding: 0px;
            min-width: 50px;
            max-width: 50px;
            min-height: 50px;
            max-height: 50px;
        }
        QPushButton:hover {
            background-color: #60C8D8;
        }
        QPushButton:disabled {
            background-color: #2A2D39;
        }
    )");
    playPauseBtn->setIcon(QIcon("icons\\play-button.svg")); // 需要添加图标资源
    playPauseBtn->setIconSize(QSize(25, 25));
    playPauseBtn->setDisabled(true);
    connect(playPauseBtn, &QPushButton::clicked, this, &MusicWidget::onPlayPauseClicked);
    btnLayout->addWidget(playPauseBtn);

    // 下一曲按钮 - 改为图标按钮
    nextBtn = new QPushButton(this);
    nextBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #50B8C8;
            border: none;
            border-radius: 20px;
            padding: 0px;
            min-width: 40px;
            max-width: 40px;
            min-height: 40px;
            max-height: 40px;
        }
        QPushButton:hover {
            background-color: #60C8D8;
        }
        QPushButton:disabled {
            background-color: #2A2D39;
        }
    )");
    nextBtn->setIcon(QIcon("icons\\next-button.svg")); // 需要添加图标资源
    nextBtn->setIconSize(QSize(20, 20));
    nextBtn->setDisabled(true);
    connect(nextBtn, &QPushButton::clicked, this, &MusicWidget::onNextClicked);
    btnLayout->addWidget(nextBtn);

    btnLayout->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(btnWidget);

    // 将内容部件添加到自定义widget
    addContentWidget(contentWidget);
}

void MusicWidget::requestMusic(){
    QUrl url("http://music.163.com/api/search/get/web?s=free&type=1&offset=0&limit=100&total=true");
    songListRequestId =netWorkTool->get(url);
}

void MusicWidget::parseSongList(const QByteArray &jsonData){
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (doc.isNull()) {
        songNameLabel->setText("数据格式错误");
        return;
    }
    QJsonObject root = doc.object();
    if (root["code"].toInt() != 200) {
        QString errMsg = root["msg"].toString().isEmpty() ? "接口请求失败" : root["msg"].toString();
        songNameLabel->setText("错误：" + errMsg);
        return;
    }
    // 提取songs数组
    QJsonArray songs = root["result"].toObject()["songs"].toArray();
    if (songs.isEmpty()) {
        songNameLabel->setText("未找到歌曲");
        return;
    }
    // 清空原有数据（避免重复加载）
    playlist->clear();
    songInfoList.clear();
    currentPlayIndex = -1;
    for(int i=0;i<songs.size();i++){
       QJsonObject  songObj= songs[i].toObject();
        if(songObj["fee"].toInt()!=0||songObj["id"].toInt()==0) continue;
        SongInfo songInfo;
        songInfo.songId=QString::number(songObj["id"].toInt());
        songInfo.songName=songObj["name"].toString();
        songInfo.mvid= QString::number(songObj["mvid"].toInt());
        QJsonArray artistsArray = songObj["artists"].toArray();
        QString coverUrl = "";
        if (!artistsArray.isEmpty()) { // 确保 artists 数组非空
            QJsonObject firstArtist = artistsArray[0].toObject();
            coverUrl = firstArtist["img1v1Url"].toString();
        }
        songInfo.coverUrl = coverUrl;
        songInfoList.append(songInfo);
        QString playUrl = QString("http://music.163.com/song/media/outer/url?id=%1.mp3").arg(songInfo.songId);
        playlist->append(playUrl);
    }
    // 解析完成后启用按钮
    if (!songInfoList.isEmpty()) {
        prevBtn->setEnabled(true);
        playPauseBtn->setEnabled(true);
        nextBtn->setEnabled(true);
        // 自动加载第一首歌的封面
        currentPlayIndex = 0;
        // 显示第一首歌的信息
        SongInfo curInfo = songInfoList[currentPlayIndex];
        songNameLabel->setText(QString("%1").arg(curInfo.songName));
        loadSongCover(currentPlayIndex);   // 加载第一首歌的封面
    } else {
        songNameLabel->setText("无免费歌曲可播放");
    }
}
void MusicWidget::playSongByIndex(int songIndex){
    // 边界检查
        if (songIndex < 0 || songIndex >= playlist->size()) return;

    // 更新当前索引
    currentPlayIndex = songIndex;

    // 加载并播放歌曲
    QString playUrl = playlist->at(currentPlayIndex);
    player->setSource(QUrl(playUrl));
    player->play();

    // 更新歌曲名显示
    SongInfo curInfo = songInfoList[currentPlayIndex];
    songNameLabel->setText(QString("%1").arg(curInfo.songName));
}

void MusicWidget::loadSongCover(int songIndex){
    if(songIndex<0||songIndex>=songInfoList.size()) return;
    QString curUrl= songInfoList.at(songIndex).coverUrl;
    //qDebug()<<songInfoList.at(songIndex).songId<<" "<<songInfoList.at(songIndex).coverUrl;
    if (curUrl.isEmpty()) {
        coverLabel->setText("无封面");
        coverLabel->setPixmap(QPixmap());
        return;
    }
    // 取消之前未完成的封面请求（避免覆盖当前封面）
    if (coverRequestId != -1) {
        netWorkTool->cancalAllRequest(); // 简化处理：取消所有未完成请求
        coverRequestId = -1;
    }
    // 发起封面请求（带Referer头）
    coverRequestId = netWorkTool->get(QUrl(curUrl));
}

void MusicWidget::onNetworkFinished(int requestId, const QByteArray &data){
    // 1. 处理歌曲列表请求结果
    if (requestId == songListRequestId) {
        parseSongList(data);
        songListRequestId = -1;
    }
    // 2. 处理封面请求结果
    else if (requestId == coverRequestId) {
        QPixmap coverPix;
        if (coverPix.loadFromData(data)) {
            // 缩放封面（保持比例，适应标签尺寸）
            QPixmap scaledPix = coverPix.scaled(coverLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
            coverLabel->setPixmap(scaledPix);
            coverLabel->setText(""); // 清空加载提示
        } else {
            coverLabel->setText("封面加载失败");
        }
        coverRequestId = -1; // 重置请求ID
    }
}

// 网络请求错误回调
void MusicWidget::onNetworkError(int requestId, const QString &errorMsg) {
    qDebug() << "Network Error (ID:" << requestId << "):" << errorMsg;
    if (requestId == songListRequestId) {
        songNameLabel->setText("歌曲列表加载失败");
    } else if (requestId == coverRequestId) {
        coverLabel->setText("封面加载失败");
    }
}

// 播放/暂停按钮点击
void MusicWidget::onPlayPauseClicked() {
    if (currentPlayIndex == -1) return;
    if (player->playbackState() == QMediaPlayer::PlayingState) {
        player->pause();
    } else {
        player->play();
    }
}

// 上一曲按钮点击
void MusicWidget::onPreviousClicked() {
    if (playlist->isEmpty()) return;

    currentPlayIndex = (currentPlayIndex - 1 + playlist->size()) % playlist->size();
    playSongByIndex(currentPlayIndex);
    loadSongCover(currentPlayIndex);
}

// 下一曲按钮点击
void MusicWidget::onNextClicked() {
    if (playlist->isEmpty()) return;

    currentPlayIndex = (currentPlayIndex + 1) % playlist->size();
    playSongByIndex(currentPlayIndex);
    loadSongCover(currentPlayIndex);
}

// 更新播放状态（同步按钮文本）
void MusicWidget::updatePlayState(QMediaPlayer::PlaybackState state) {
    isPlaying = (state == QMediaPlayer::PlayingState);
    playPauseBtn->setIcon(isPlaying ? QIcon("icons\\pause-button.svg") : QIcon("icons\\play-button.svg"));
}
