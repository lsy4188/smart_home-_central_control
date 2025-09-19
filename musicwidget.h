#ifndef MUSICWIDGET_H
#define MUSICWIDGET_H

#include <QObject>
#include "customwidget.h"
#include "networktool.h"
#include <QMediaPlayer>
#include <QLabel>
#include <QPushButton>
#include <QAudioOutput>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QGraphicsDropShadowEffect>
class MusicWidget : public CustomWidget
{
    Q_OBJECT
public:
    explicit MusicWidget(QWidget *parent = nullptr);
    ~MusicWidget();

private slots:
    //音乐控制
    void onPlayPauseClicked();
    void onPreviousClicked();
    void onNextClicked();
    void updatePlayState(QMediaPlayer::PlaybackState state);
    // 网络回调（处理歌曲列表/封面请求）
    void onNetworkFinished(int requestId, const QByteArray &data);
    void onNetworkError(int requestId, const QString &errorMsg);

private:

    //初始化UI
    void initUI();
    // 获取音乐数据
    void requestMusic();
    // 解析接口返回的JSON数据（多歌曲）
    void parseSongList(const QByteArray &jsonData);
    // 加载指定索引的歌曲封面
    void loadSongCover(int songIndex);
    // 切换到指定索引的歌曲并播放
    void playSongByIndex(int songIndex);

    //UI控件
    QLabel* coverLabel;//封面
    QLabel* songNameLabel;//歌曲名
    QPushButton *prevBtn;  // 上一曲按钮
    QPushButton *playPauseBtn; // 播放/暂停按钮
    QPushButton *nextBtn;  // 下一曲按钮
    // 媒体播放
    QMediaPlayer *player;
    QStringList *playlist;
    int currentPlayIndex;
    bool isPlaying = false;

    // 网络请求
    NetworkTool *netWorkTool;
    int songListRequestId = -1;  // 歌曲列表请求ID
    int coverRequestId = -1;     // 封面请求ID

    // 歌曲信息缓存（索引与playlist一一对应）
        struct SongInfo {
        QString songId;       // 歌曲ID（构造播放地址）
        QString mvid;         // mvid（拼接显示）
        QString songName;     // 歌曲名
        QString coverUrl;     // 封面URL
    };
    QList<SongInfo> songInfoList; // 存储所有解析出的歌曲信息
};

#endif // MUSICWIDGET_H
