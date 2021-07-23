﻿#include "mainwindow.h"
#include "ui_mainwindow.h"
#include    <QtNetwork>
#include "message.h"
#include <cstring>
QString MainWindow::getLocalIP()
{//获取本机IPv4地址
    QString hostName=QHostInfo::localHostName();//本地主机名
    QHostInfo   hostInfo=QHostInfo::fromName(hostName);
    QString   localIP="";

    QList<QHostAddress> addList=hostInfo.addresses();//

    if (!addList.isEmpty())
    for (int i=0;i<addList.count();i++)
    {
        QHostAddress aHost=addList.at(i);
        if (QAbstractSocket::IPv4Protocol==aHost.protocol())
        {
            localIP=aHost.toString();
            break;
        }
    }
    return localIP;
}

void MainWindow::closeEvent(QCloseEvent *event)
{//关闭窗口时停止监听
    if (tcpServer->isListening())
        tcpServer->close();;//停止网络监听
    event->accept();
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    LabListen=new QLabel("监听状态:");
    LabListen->setMinimumWidth(150);
    ui->statusBar->addWidget(LabListen);

    LabSocketState=new QLabel("Socket状态：");//
    LabSocketState->setMinimumWidth(200);
    ui->statusBar->addWidget(LabSocketState);

    QString localIP=getLocalIP();//本机IP
    this->setWindowTitle(this->windowTitle()+"----本机IP："+localIP);
    ui->comboIP->addItem(localIP);

    tcpServer=new QTcpServer(this);
    connect(tcpServer,SIGNAL(newConnection()),this,SLOT(onNewConnection()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onNewConnection()
{
//    ui->plainTextEdit->appendPlainText("有新连接");
    tcpSocket = tcpServer->nextPendingConnection(); //创建socket

    connect(tcpSocket, SIGNAL(connected()),
            this, SLOT(onClientConnected()));
    onClientConnected();//

    connect(tcpSocket, SIGNAL(disconnected()),
            this, SLOT(onClientDisconnected()));

    connect(tcpSocket,SIGNAL(stateChanged(QAbstractSocket::SocketState)),
            this,SLOT(onSocketStateChange(QAbstractSocket::SocketState)));
    onSocketStateChange(tcpSocket->state());

    connect(tcpSocket,SIGNAL(readyRead()),
            this,SLOT(onSocketReadyRead()));
}

void MainWindow::onSocketStateChange(QAbstractSocket::SocketState socketState)
{//socket状态变化时
    switch(socketState)
    {
    case QAbstractSocket::UnconnectedState:
        LabSocketState->setText("scoket状态：UnconnectedState");
        break;
    case QAbstractSocket::HostLookupState:
        LabSocketState->setText("scoket状态：HostLookupState");
        break;
    case QAbstractSocket::ConnectingState:
        LabSocketState->setText("scoket状态：ConnectingState");
        break;

    case QAbstractSocket::ConnectedState:
        LabSocketState->setText("scoket状态：ConnectedState");
        break;

    case QAbstractSocket::BoundState:
        LabSocketState->setText("scoket状态：BoundState");
        break;

    case QAbstractSocket::ClosingState:
        LabSocketState->setText("scoket状态：ClosingState");
        break;

    case QAbstractSocket::ListeningState:
        LabSocketState->setText("scoket状态：ListeningState");
    }
}

void MainWindow::onClientConnected()
{//客户端接入时
    ui->plainTextEdit->appendPlainText("**client socket connected");
    ui->plainTextEdit->appendPlainText("**peer address:"+
                                   tcpSocket->peerAddress().toString());
    ui->plainTextEdit->appendPlainText("**peer port:"+
                                   QString::number(tcpSocket->peerPort()));
}

void MainWindow::onClientDisconnected()
{//客户端断开连接时
    ui->plainTextEdit->appendPlainText("**client socket disconnected");
    tcpSocket->deleteLater();
    //    deleteLater();//QObject::deleteLater();
}
#include <iostream>
using namespace std;
void MainWindow::onSocketReadyRead()
{//读取缓冲区行文本
//    QStringList   lines;
//    while(tcpSocket->canReadLine())
 //       ui->plainTextEdit->appendPlainText("[in] "+tcpSocket->readLine());
    //QString msg = tcpSocket->readAll();
//    char data_length_buf[1024] = { 0 };
//    int data_lenth = 0;
//    tcpSocket->read(data_length_buf,4);
//    data_lenth = atoi(data_length_buf);
//    QString msg = tcpSocket->read(data_lenth);
//    qDebug() << data_lenth << msg.utf16()<< endl;
//     ui->plainTextEdit->appendPlainText(msg);
//   // msg.c
//    if(!msg.isEmpty())
//    {
//        //PACK1 A;
//        //memcpy((void *)&A,(void *)&msg,msg.length());
//        //ui->plainTextEdit->appendPlainText(A.data);
//        //printf("%s\n",(msg+8).c_str);
//        qDebug() << msg.data();
//       // ui->plainTextEdit->appendPlainText(msg + 4);
//    }
//        lines.append(clientConnection->readLine());
    QString msg = tcpSocket->readAll();
    int ii = 0;
    for(ii = 0;ii <10;ii++)
        cout << msg.at(ii).unicode() << " " ;
    cout << endl;
}

void MainWindow::on_actStart_triggered()
{//开始监听
    QString     IP=ui->comboIP->currentText();//IP地址
    quint16     port=ui->spinPort->value();//端口
    QHostAddress    addr(IP);
    tcpServer->listen(addr,port);//
//    tcpServer->listen(QHostAddress::LocalHost,port);// Equivalent to QHostAddress("127.0.0.1").
    ui->plainTextEdit->appendPlainText("**开始监听...");
    ui->plainTextEdit->appendPlainText("**服务器地址："
                       +tcpServer->serverAddress().toString());
    ui->plainTextEdit->appendPlainText("**服务器端口："
                       +QString::number(tcpServer->serverPort()));

    ui->actStart->setEnabled(false);
    ui->actStop->setEnabled(true);

    LabListen->setText("监听状态：正在监听");
}

void MainWindow::on_actStop_triggered()
{//停止监听
    if (tcpServer->isListening()) //tcpServer正在监听
    {
        tcpServer->close();//停止监听
        ui->actStart->setEnabled(true);
        ui->actStop->setEnabled(false);
        LabListen->setText("监听状态：已停止监听");
    }
}

void MainWindow::on_actClear_triggered()
{
    ui->plainTextEdit->clear();
}
QByteArray intToByte(unsigned int number)
{
    QByteArray abyte0;
    abyte0.resize(4);
    abyte0[0] = (uchar) (0x000000ff & number);
    abyte0[1] = (uchar) ((0x0000ff00 & number) >> 8);
    abyte0[2] = (uchar) ((0x00ff0000 & number) >> 16);
    abyte0[3] = (uchar) ((0xff000000 & number) >> 24);
    return abyte0;
}


void MainWindow::on_btnSend_clicked()
{//发送一行字符串，以换行符结束
    QString  msg=ui->editMsg->text();
    ui->plainTextEdit->appendPlainText("[out] "+msg);
    ui->editMsg->clear();
    ui->editMsg->setFocus();
   PACK1 A;
   A.total_data_size = msg.length();
    QByteArray  str = intToByte(A.total_data_size);
    //char msg1[512] = { 0 };

    QByteArray  str1 = msg.toUtf8();
    QByteArray  str2  = str + str1;
     //itoa(msg.length(), msg1, 512);
    // tcpSocket->write(msg1);
   // memcpy(msg1+4,(msg.toStdString()).c_str(),512-4);
    //tcpSocket->write(msg1);
    // str=msg.toUtf8();
    //str.append('\n');//添加一个换行符
    tcpSocket->write(str2,(str.length()+str1.length()));
}

void MainWindow::on_actHostInfo_triggered()
{//获取本机地址
    QString hostName=QHostInfo::localHostName();//本地主机名
    ui->plainTextEdit->appendPlainText("本机主机名："+hostName+"\n");
    QHostInfo   hostInfo=QHostInfo::fromName(hostName);

    QList<QHostAddress> addList=hostInfo.addresses();//
    if (!addList.isEmpty())
    for (int i=0;i<addList.count();i++)
    {
        QHostAddress aHost=addList.at(i);
        if (QAbstractSocket::IPv4Protocol==aHost.protocol())
        {
            QString IP=aHost.toString();
            ui->plainTextEdit->appendPlainText("本机IP地址："+aHost.toString());
            if (ui->comboIP->findText(IP)<0)
                ui->comboIP->addItem(IP);
        }
    }

}
