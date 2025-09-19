#include "maininterface.h"
#include "timewidget.h"
#include "datareceiverwidget.h"
#include "weatherwidget.h"
#include "musicwidget.h"
#include "videowidget.h"
#include "networkswitchwidget.h"
MainInterface::MainInterface(QWidget *parent, QTcpServer* server, QTcpSocket* socket)
    : QMainWindow{parent},pTcpServer(server),pSocket(socket)
{
    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground);
    this->setMaximumSize(1200,700);
    this->setMinimumSize(1200,700);
    centralWidget=new QWidget(this);
    this->setCentralWidget(centralWidget);
    mainLayout=new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(12);//区域间间距
    mainLayout->setContentsMargins(15,15,15,15);//设置窗口内边框
    centralWidget->setStyleSheet("background-color: #1E2129;");
    initTopArea();
    initMiddleArea();
    initBottomArea();
}
MainInterface::~MainInterface(){
    dataReceiverWidgets.clear();
    networkSwitchWidgets.clear();
}

// 初始化顶部区域
void MainInterface::initTopArea(){
    QWidget *topWidget=new QWidget(this);
    topWidget->setFixedHeight(100);
    QHBoxLayout *topLayout=new QHBoxLayout(topWidget);
    topLayout->setSpacing(15);

    // 语音提示卡片
    QWidget *voiceCard = new QWidget(topWidget);
    voiceCard->setFixedSize(160,70);
    voiceCard->setStyleSheet("background-color: #2C2F39; border-radius: 8px;");
    QVBoxLayout *voiceVLayout = new QVBoxLayout(voiceCard);
    voiceVLayout->setContentsMargins(20, 20, 20, 20);
    voiceVLayout->setSpacing(0);
    //嵌套水平布局
    QHBoxLayout *voiceHLayout = new QHBoxLayout();
    voiceHLayout->setSpacing(0);
    // 退出按钮
    QPushButton *exitBtn = new QPushButton("退出", voiceCard);
    exitBtn->setStyleSheet(R"(
        QPushButton {
            background-color: #FF5252; /* 红色背景（匹配需求） */
            color: white;             /* 白色文字，高对比度 */
            border: none;
            border-radius: 6px;       /* 圆角，适配居中视觉 */
            font-size: 14px;          /* 字体大小适中 */
            padding: 8px 20px;        /* 内边距，控制按钮宽高 */
            min-width: 80px;          /* 最小宽度，避免按钮过窄 */
        }
        QPushButton:hover {
            background-color: #FF7070; /*  hover加深，增强交互反馈 */
        }
        QPushButton:pressed {
            background-color: #E04040; /* 按下时进一步加深，反馈明确 */
        }
    )");
    // 按钮点击触发主窗口关闭（复用原有closeEvent，确保资源释放）
    connect(exitBtn, &QPushButton::clicked, this, &MainInterface::close);
    // 水平居中
    voiceHLayout->addStretch(1);  // 左侧伸缩项（占满剩余空间）
    voiceHLayout->addWidget(exitBtn); // 中间按钮
    voiceHLayout->addStretch(1);  // 右侧伸缩项（占满剩余空间）
    // 垂直居中
    voiceVLayout->addStretch(1);  // 上方伸缩项（占满剩余空间）
    voiceVLayout->addLayout(voiceHLayout); // 中间水平布局（含按钮）
    voiceVLayout->addStretch(1);  // 下方伸缩项（占满剩余空间）
    topLayout->addWidget(voiceCard, 1); // voiceCard在顶部布局的占比不变（1份宽度）


    // 传感器卡片
    qDebug() << "传感器卡片：" << pSocket;
    DataReceiverWidget *sensorData=new DataReceiverWidget(topWidget,pSocket);
    dataReceiverWidgets.append(sensorData);
    topLayout->addWidget(sensorData, 2);

    // 时间卡片
    TimeWidget *timeCard=new TimeWidget(topWidget);
    topLayout->addWidget(timeCard,1);

    mainLayout->addWidget(topWidget);
}
// 初始化中间区域
void MainInterface::initMiddleArea(){
    //左侧容器
    QWidget *middleWidget=new QWidget(this);
    QHBoxLayout *middleLayout=new QHBoxLayout(middleWidget);
    middleLayout->setSpacing(15);
    middleLayout->setContentsMargins(0, 0, 0, 0);
    middleWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 天气卡片（固定尺寸）
   WeatherWidget *weatherWidget=new WeatherWidget(middleWidget);
    middleLayout->addWidget(weatherWidget);

    // 音乐卡片（固定尺寸）
    MusicWidget *musicCard = new MusicWidget(this);
    middleLayout->addWidget(musicCard);

    //右侧总容器
    QWidget *rightContainer=new QWidget(this);
    rightContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    QVBoxLayout *rightLayout=new QVBoxLayout(rightContainer);
    rightLayout->setSpacing(0); // 移除间距
    rightLayout->setContentsMargins(0, 0, 0, 0); // 移除内边距

    // 视频信息卡片
    VideoWidget *videoCard=new VideoWidget(this);
    rightLayout->addWidget(videoCard, 1);

    middleLayout->addWidget(rightContainer,1);

    mainLayout->addWidget(middleWidget,1);
}
// 初始化底部区域
void MainInterface::initBottomArea(){
    QWidget *bottomWidget=new QWidget(this);
    bottomWidget->setFixedHeight(70);
    bottomWidget->setStyleSheet("background-color: #2C2F39; border-radius: 8px;");

    QHBoxLayout* bottomLayout=new QHBoxLayout(bottomWidget);
    bottomLayout->setSpacing(15);
    bottomLayout->setContentsMargins(15,0,15,0);

    qDebug() << "准备传递给NetworkSwitchWidget的socket：" << pSocket;
    struct SwitchConfig{
        QString name;
        QString onCmd;
        QString offCmd;
        bool initIsOn;
    };

    QList<SwitchConfig> switchConfigs = {
        {"灯光系统", "CTRL:led open OK", "CTRL:led close OK", false},
        {"通风系统", "CTRL:fan open OK", "CTRL:fan close OK", false},
        {"音响系统", "CTRL:beep open OK", "CTRL:beep close OK", false}
    };
    qDebug()<<"主设备上："<<pSocket;
    for (const auto &config : switchConfigs) {
        NetworkSwitchWidget *hardwareSwitch = new NetworkSwitchWidget(
            bottomWidget,
            pSocket,
            config.name,
            config.onCmd,
            config.offCmd,
            config.initIsOn
            );
        // 设置开关尺寸策略：宽度自适应，高度充满底部区域
        hardwareSwitch->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        networkSwitchWidgets.append(hardwareSwitch);
        bottomLayout->addWidget(hardwareSwitch);
    }
    mainLayout->addWidget(bottomWidget);
}

void MainInterface::setSocket(QTcpSocket* socket) {
    pSocket = socket;
    // 更新数据接收组件
    for (auto receiver : dataReceiverWidgets) {
        receiver->setSocket(socket);
    }
    // 更新开关组件
    for (auto switchWidget : networkSwitchWidgets) {
        switchWidget->setSocket(socket);
    }
}

void MainInterface::mousePressEvent(QMouseEvent*event){
    if(event->button()==Qt::LeftButton){
        windowPoint=event->globalPos()-frameGeometry().topLeft();
    }
}

void MainInterface::mouseMoveEvent(QMouseEvent*event){
    if(event->buttons()==Qt::LeftButton){
        move(event->globalX()-windowPoint.x(),event->globalY()-windowPoint.y());
    }
}

void MainInterface::closeEvent(QCloseEvent* event){
    emit closed();
}
