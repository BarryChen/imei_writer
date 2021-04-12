#include "adbpath.h"
#include <QDebug>

void SetAdbPath(QString path)
{
    qDebug() << "error: adb path is null";
    mAdbPath = path;
}

QString GetAdbPath()
{
    if(mAdbPath.isEmpty())
        qDebug() << "error: adb path is null";
    return mAdbPath;
}
