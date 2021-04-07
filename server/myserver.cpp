#include "myserver.h"
#include "ui_myserver.h"
#include <QFile>


MyServer::MyServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyServer),
    nextBlockSize(0)
{
    ui->setupUi(this);
    textEdit = new QTextEdit;
    textEdit->setReadOnly(true);

    //Layout setup
    QVBoxLayout* pvbxLayout = new QVBoxLayout;
    pvbxLayout->addWidget(new QLabel("<H1>Server</H1>"));
    pvbxLayout->addWidget(textEdit);
    setLayout(pvbxLayout);

    readSettings();
    if(!isSettingsCorrect)
    {
        return;
    }


    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen(QHostAddress::Any, port)) {
        textEdit->append("Unable to start the server:" + tcpServer->errorString());
        tcpServer->close();
        return;
    }
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}

void MyServer::slotReadClient()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_5_9);
    for (;;) {
        if (!nextBlockSize) {
            if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> nextBlockSize;
        }

        if (pClientSocket->bytesAvailable() < nextBlockSize) {
            break;
        }
        QTime   time;
        double valueToIncrement;
        QString version;
        in >> version >> time >> valueToIncrement;

        if(version != protocolVersion)
        {
            QString errorMessage = "Client has wrong version, curent version is " + protocolVersion;
            sendError(pClientSocket, errorMessage, errorVersion);
        }
        else
        {
            sendIncrementValue(pClientSocket, valueToIncrement);
        }

        QString str = QString::number(valueToIncrement);
        QString strMessage = time.toString() + " " + version + " " + "Client has sended - " + str;
        textEdit->append(strMessage);

        nextBlockSize = 0;
    }
}

void MyServer::slotNewConnection()
{
    QTcpSocket* pClientSocket = tcpServer->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()), pClientSocket, SLOT(deleteLater()));
    connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));

    sendMessage(pClientSocket, "Server Response: Connected!");
}

void MyServer::sendMessage(QTcpSocket* pSocket, const QString& str)
{
    QString messageType = "message";
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_9);
    out << quint16(0) << QTime::currentTime() << messageType <<str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pSocket->write(arrBlock);
}

void MyServer::sendError(QTcpSocket* pSocket, const QString& str, int errorCode)
{
    QString messageType = "error";
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_9);
    out << quint16(0) << QTime::currentTime() << messageType <<errorCode<<str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pSocket->write(arrBlock);
}

void MyServer::sendIncrementValue(QTcpSocket* pSocket, double valueToIncrement)
{
    QVector<double> responseValues;
    for(int i = 0; i < 1000; i++)
    {
        responseValues.push_back(valueToIncrement+i);
    }
    QString messageType = "incremented_value";
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_9);
    out << quint16(0) << QTime::currentTime() <<messageType<< responseValues;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pSocket->write(arrBlock);
}

bool MyServer::readSettings(void)
{
    isSettingsCorrect = false;
    QFile file(QDir::currentPath() + "/settings");

    if (!file.open(QIODevice::ReadOnly ))
    {
        textEdit->append("No settings file");
        return false;
    }

    QTextStream stream(&file);
    QString settings = stream.readLine();
    stream.flush();
    file.close();

    port = settings.toInt();

    if (port <= 0 || port > 65535)
    {
        textEdit->append("Port has wrong value");
        return false;
    }

    isSettingsCorrect = true;
    return true;
}

bool MyServer::checkSettings(void)
{
    return isSettingsCorrect;
}


MyServer::~MyServer()
{
    delete ui;
}
