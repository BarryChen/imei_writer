#ifndef ADBPATH_H
#define ADBPATH_H

#include <QDebug>


static QString mAdbPath = "adb.exe";
extern void SetAdbPath(QString path);
extern QString GetAdbPath();

#endif // ADBPATH_H
