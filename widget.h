#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
    void onLoginButtonClicked();
    void onClientConnectButtonClicked();
    void onClientSendButtonClicked();
    void onClientReadyRead();
    void onClientDisconnected();

    void onServerStartButtonClicked();
    void onServerSendButtonClicked();
    void onNewConnection();
    void onServerClientReadyRead();
    void onServerClientDisconnected();

private:
    Ui::Widget *ui;
    QTcpSocket *tcpSocket;
    QTcpServer *tcpServer;
    QTcpSocket *serverClientSocket;
};

#endif // WIDGET_H
