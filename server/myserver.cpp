#include "myserver.h"
#include "ui_myserver.h"
#include <QFile>
#include <QMessageBox>


MyServer::MyServer(QString settingsPath, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyServer),
    m_nNextBlockSize(0)
{
    readSettings();
    if(!isSettingsCorrect)
    {
        return;
    }
    ui->setupUi(this);

    m_ptcpServer = new QTcpServer(this);
    if (!m_ptcpServer->listen(QHostAddress::Any, port)) {
        QMessageBox::critical(0,
                              "Server Error",
                              "Unable to start the server:"
                              + m_ptcpServer->errorString()
                             );
        m_ptcpServer->close();
        return;
    }
    connect(m_ptcpServer, SIGNAL(newConnection()),
            this,         SLOT(slotNewConnection())
           );

    m_ptxt = new QTextEdit;
    m_ptxt->setReadOnly(true);

    //Layout setup
    QVBoxLayout* pvbxLayout = new QVBoxLayout;
    pvbxLayout->addWidget(new QLabel("<H1>Server</H1>"));
    pvbxLayout->addWidget(m_ptxt);
    setLayout(pvbxLayout);
}

void MyServer::slotReadClient()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_5_9);
    for (;;) {
        if (!m_nNextBlockSize) {
            if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (pClientSocket->bytesAvailable() < m_nNextBlockSize) {
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
        m_ptxt->append(strMessage);

        m_nNextBlockSize = 0;
    }
}

/*virtual*/ void MyServer::slotNewConnection()
{
    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();
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
    for(int i = 0; i < 10; i++)
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
        QMessageBox msgBox;
        msgBox.setText("No settings file");
        msgBox.exec();
        return false;
    }

    QTextStream stream(&file);
    QString settings = stream.readLine();
    stream.flush();
    file.close();

    port = settings.toInt();

    if (port <= 0)
    {
        QMessageBox msgBox;
        msgBox.setText("Port has wrong value");
        msgBox.exec();
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
