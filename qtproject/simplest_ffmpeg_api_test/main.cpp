#include <QCoreApplication>
#include <QDebug>

extern "C" {
#include "libavformat/avformat.h"
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    av_register_all();

    qDebug() << "current version is " << avformat_version();

    return a.exec();
}
