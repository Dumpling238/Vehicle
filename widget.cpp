#include "widget.h"
#include "ui_widget.h"
#include <QMessageBox>
#include <QHostAddress>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , tcpSocket(new QTcpSocket(this))
    , tcpServer(new QTcpServer(this))
    , serverClientSocket(nullptr)
{
    ui->setupUi(this);

    // Connect login button
    connect(ui->loginButton, &QPushButton::clicked, this, &Widget::onLoginButtonClicked);

    // Connect TCP socket signals for client
    connect(ui->clientConnectButton, &QPushButton::clicked, this, &Widget::onClientConnectButtonClicked);
    connect(ui->clientSendButton, &QPushButton::clicked, this, &Widget::onClientSendButtonClicked);
    connect(tcpSocket, &QTcpSocket::readyRead, this, &Widget::onClientReadyRead);
    connect(tcpSocket, &QTcpSocket::disconnected, this, &Widget::onClientDisconnected);

    // Connect TCP server signals
    connect(ui->serverStartButton, &QPushButton::clicked, this, &Widget::onServerStartButtonClicked);
    connect(ui->serverSendButton, &QPushButton::clicked, this, &Widget::onServerSendButtonClicked);
    connect(tcpServer, &QTcpServer::newConnection, this, &Widget::onNewConnection);

    // Initially, show the login page
    ui->stackedWidget->setCurrentIndex(0);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::onLoginButtonClicked()
{
    QString username = ui->usernameInput->text();
    QString password = ui->passwordInput->text();

    if (username == "admin" && password == "1234") {
        // Show main interface and hide login interface
        ui->stackedWidget->setCurrentIndex(1);
    } else {
        QMessageBox::warning(this, "Login Failed", "Invalid username or password");
    }
}

void Widget::onClientConnectButtonClicked()
{
    QString address = ui->addressInput->text();
    quint16 port = ui->portInput->text().toUShort();

    tcpSocket->connectToHost(QHostAddress(address), port);

    if (!tcpSocket->waitForConnected(3000)) {
        QMessageBox::critical(this, "Error", "Connection failed: " + tcpSocket->errorString());
    } else {
        QMessageBox::information(this, "Connected", "Connected to server.");
    }
}

void Widget::onClientSendButtonClicked()
{
    QString message = ui->clientMessageInput->text();
    tcpSocket->write(message.toUtf8());
}

void Widget::onClientReadyRead()
{
    QByteArray data = tcpSocket->readAll();
    ui->clientTextEdit->append("Client Received: " + QString::fromUtf8(data));
}

void Widget::onClientDisconnected()
{
    QMessageBox::information(this, "Disconnected", "Disconnected from server.");
}

void Widget::onServerStartButtonClicked()
{
    quint16 port = ui->serverPortInput->text().toUShort();

    if (tcpServer->listen(QHostAddress::Any, port)) {
        QMessageBox::information(this, "Server Started", "Server started on port " + QString::number(port));
    } else {
        QMessageBox::critical(this, "Error", "Server failed to start: " + tcpServer->errorString());
    }
}

void Widget::onServerSendButtonClicked()
{
    QString message = ui->serverMessageInput->text();
    if (serverClientSocket && serverClientSocket->isOpen()) {
        serverClientSocket->write(message.toUtf8());
    }
}

void Widget::onNewConnection()
{
    serverClientSocket = tcpServer->nextPendingConnection();
    connect(serverClientSocket, &QTcpSocket::readyRead, this, &Widget::onServerClientReadyRead);
    connect(serverClientSocket, &QTcpSocket::disconnected, this, &Widget::onServerClientDisconnected);
    ui->serverTextEdit->append("Server: New connection established");
}

void Widget::onServerClientReadyRead()
{
    QByteArray data = serverClientSocket->readAll();
    ui->serverTextEdit->append("Server Received: " + QString::fromUtf8(data));
}

void Widget::onServerClientDisconnected()
{
    ui->serverTextEdit->append("Server: Client disconnected");
}
