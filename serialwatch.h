#ifndef SERIALWATCH_H
#define SERIALWATCH_H

#include <QObject>
#include <QTimer>
#include "baseprocess.h"

class SerialWatch : public BaseProcess
{
    Q_OBJECT
public:
    explicit SerialWatch();

signals:
   void onSerialConneceted(QString port);
   void onSerialDisconnected();

public slots:
    void SerialWatchLoop();

private:
    bool    mExistPort;
    QString mPort;
    QTimer  *mTimer;
};

#endif // SERIALWATCH_H
