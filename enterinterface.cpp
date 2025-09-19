#include "enterinterface.h"
#include "maininterface.h"
EnterInterface::EnterInterface(QWidget *parent)
    : QWidget{parent}
{
    this->setWindowFlag(Qt::FramelessWindowHint);//无边框
    this->setAttribute(Qt::WA_TranslucentBackground);//设置透明度
    this->setFixedSize(350,500);
    pTcpServer=new QTcpServer();
    pSocket=nullptr;
    initUI();
    setWindowStyle();
    connect(pTcpServer,&QTcpServer::newConnection,this,&EnterInterface::onNewConnection);
}

EnterInterface::~EnterInterface(){
    if(mainInterface){
        mainInterface->deleteLater();
    }
}

void EnterInterface::onNewConnection()
{
    pSocket = pTcpServer->nextPendingConnection();
    qDebug() << "新客户端连接：" << pSocket;
    if (mainInterface) {
        mainInterface->setSocket(pSocket);
    }
}

void EnterInterface::mouseMoveEvent(QMouseEvent*event){
   if(event->buttons()==Qt::LeftButton){
        move((event->globalPos()-windowPosition).x(),(event->globalPos()-windowPosition).y());
    }
}

void EnterInterface::mousePressEvent(QMouseEvent*event){
    if(event->button()==Qt::LeftButton){
        /*
         *event->globalPos():获取鼠标事件发生时，鼠标在全局屏幕坐标中的位置。
         *frameGeometry().topLeft():获取窗口框架的左上角在全局屏幕坐标中的位置
         *
        */
        windowPosition=event->globalPos()-frameGeometry().topLeft();//获取窗口内的坐标
    }
}

void EnterInterface::connectSTM32(){
   bool ret=pTcpServer->listen(QHostAddress(ipEdit->text()),portEdit->text().toInt());
    if(ret){
        qDebug() << "服务器启动成功";
        showMainInterface();
    }else{
        QMessageBox::warning(this,"警告","连接失败",QMessageBox::Button::Ok,QMessageBox::Button::Cancel);
    }
}

void EnterInterface::showMainInterface(){
    //创建主界面并传递TCP对象
    mainInterface=new MainInterface(nullptr,pTcpServer,pSocket);
    connect(mainInterface,&MainInterface::closed,this,[&](){
        qDebug()<<"收到了来自主界面的关闭信号";
        this->show();
        mainInterface->deleteLater();
        mainInterface=nullptr;
        pTcpServer->close();
        if(pSocket){
            pSocket->deleteLater();
            pSocket=nullptr;
        }
    });
    this->hide();
    mainInterface->show();
}

void EnterInterface::paintEvent(QPaintEvent*event){
    QPainter painter(this);
    QLinearGradient gradient(0, 0, width(), height());//创建渐变效果
    gradient.setColorAt(0, QColor(44, 62, 80)); // 深灰色
    gradient.setColorAt(1, QColor(26, 35, 126)); // 蓝黑色
    // 绘制带圆角的背景
    QRect rect(0, 0, width(), height());
    painter.setBrush(gradient);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect, 10, 10);
}

void EnterInterface::initUI(){
    //主布局
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30,30,30,30);//设置布局周围边距
    mainLayout->setSpacing(30);//设置间距
    //顶布局
    QHBoxLayout*topLayout=new QHBoxLayout();
    topLayout->setAlignment(Qt::AlignRight);
    topLayout->setSpacing(10);
    //创建关闭按钮并添加到顶部
    closeButton=new QPushButton();
    closeButton->setFixedSize(24,24);
    closeButton->setObjectName("closeButton");
    closeButton->setIcon(QIcon("icons\\close.png"));
    topLayout->addWidget(closeButton);
    //标签设置
    avatarLabel=new QLabel();
    avatarLabel->setFixedSize(100,100);
    avatarLabel->setObjectName("avatarLabel");
    avatarLabel->setAlignment(Qt::AlignCenter);
    //图标标志设置
    QPixmap avatar("icons\\title.png");
    if(avatar.isNull()){
        avatar = QPixmap(100,100);
        avatar.fill(QColor(240,240,240));
    }
    avatarLabel->setPixmap(avatar.scaled(avatarLabel->size(),Qt::KeepAspectRatio,Qt::SmoothTransformation));
    // 创建ip输入框
    ipEdit = new QLineEdit();
    ipEdit->setObjectName("ipEdit");
    ipEdit->setPlaceholderText("输入IP地址");
    ipEdit->setAlignment(Qt::AlignCenter);
    // 创建端口输入框
    portEdit = new QLineEdit();
    portEdit->setObjectName("portEdit");
    portEdit->setPlaceholderText("输入端口号");
    portEdit->setAlignment(Qt::AlignCenter);
    //创建连接按钮
    enterButton=new QPushButton("连接");
    enterButton->setObjectName("enterButton");
    enterButton->setFixedHeight(40);

    // 将所有控件添加到主布局
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(avatarLabel, 0, Qt::AlignCenter);
    mainLayout->addWidget(ipEdit);
    mainLayout->addWidget(portEdit);
    mainLayout->addWidget(enterButton);
    mainLayout->addStretch();
    // 连接信号槽
    connect(closeButton, &QPushButton::clicked, this, [&](){
        this->close();
        pTcpServer->close();
    });
    connect(enterButton,&QPushButton::clicked,this,&EnterInterface::connectSTM32);
}

void EnterInterface::setWindowStyle(){
    setStyleSheet(R"(
        #closeButton {
            background-color: transparent;
            background-repeat: no-repeat;
            background-position: center;
            border: none;
            border-radius: 40px;
        }
        #closeButton:hover {
             background-color: rgba(255, 255, 255, 0.1);
        }
        #closeButton:pressed {
            background-color: rgba(255, 255, 255, 0.2);
        }
        #avatarLabel {
            background-color: #34495e;
            border: 2px solid #455a64;
            border-radius: 50px;
            width: 100px;
            height: 100px;
        }
        #ipEdit, #portEdit {
            background-color: #34495e;
            border: none;
            border-radius: 20px;
            padding: 0 15px;
            height: 40px;
            color: #ecf0f1;
            font-size: 14px;
        }
        #ipEdit::placeholder,#portEdit::placeholder {
            color: #bdc3c7;
        }
        #ipEdit:focus, #portEdit:focus {
            border: 1px solid #3498db;
        }
        #enterButton {
            background-color: #2d8cf0;
            color: white;
            border: none;
            border-radius: 20px;
            font-size: 16px;
        }
        #enterButton:hover {
            background-color: #2b85e4;
        }
        #enterButton:pressed {
            background-color: #2879d0;
        }
        #enterButton:disabled {
            background-color: #a0cfff;
        }
    )");
}
