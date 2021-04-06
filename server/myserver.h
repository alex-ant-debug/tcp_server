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
    QTcpServer* m_ptcpServer;
    QTextEdit*  m_ptxt;
    quint16     m_nNextBlockSize;
    int port;
    bool isSettingsCorrect;
    const QString protocolVersion = "V1.0";
    void sendMessage(QTcpSocket* pSocket, const QString& str);
    void sendError(QTcpSocket* pSocket, const QString& str, int errorCode);
    void sendIncrementValue(QTcpSocket* pSocket, double valueToIncrement);
    bool readSettings(void);

 private:
    enum errorCodes{
        errorVersion = 1,
    };

};

#endif // MYSERVER_H

