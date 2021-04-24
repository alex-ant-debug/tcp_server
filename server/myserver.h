#ifndef MYSERVER_H
#define MYSERVER_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QDataStream>
#include <QTime>
#include <QDir>


class QTcpServer;
class QTextEdit;
class QTcpSocket;

namespace Ui {
class MyServer;
}

class MyServer : public QWidget
{
    Q_OBJECT

public:
    explicit MyServer(QWidget *parent = 0);
    bool checkSettings(void);
    ~MyServer();

public slots:
    void slotNewConnection();
    void slotReadClient();

private:
    Ui::MyServer *ui;
    QTcpServer* tcpServer;
    QTextEdit*  textEdit;
    quint32     nextBlockSize;
    QVBoxLayout* layout;
    QLabel *label;
    int port;
    const uint16_t maxSizePort = 65535;
    bool isSettingsCorrect;
    const uint32_t sizeOfResponsArray = 1000000;
    const QString protocolVersion = "V1.0";
    void sendMessage(QTcpSocket* pSocket, const QString& str);
    void sendError(QTcpSocket* pSocket, const QString& str, int errorCode);
    void sendIncrementValue(QTcpSocket* pSocket, double valueToIncrement);
    bool readSettings(void);
    enum errorCodes{
        errorVersion = 1,
    };

};

#endif // MYSERVER_H

