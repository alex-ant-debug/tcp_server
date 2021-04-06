#include "myserver.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MyServer server;
    if(server.checkSettings())
    {
        server.show();
    }

    return a.exec();
}
