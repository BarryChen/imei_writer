#include "serialwatch.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>

SerialWatch::SerialWatch()
{
    this->mTimer = nullptr;
    this->mExistPort = false;

    QTimer::singleShot(1000, this, SLOT(SerialWatchLoop()));
}

void SerialWatch::SerialWatchLoop()
{
    static bool save_status = false;
    this->mExistPort = false;

    //查询是否有串口
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        //qDebug() <<"name        :" << info.portName();
        //qDebug() <<"description :" << info.description();
        if(info.description() == "LeadCore CMCC AT Interface")
        {
            this->mExistPort = true;
            this->mPort = info.portName();
        }
    }
    if(save_status != this->mExistPort)
    {
        save_status = this->mExistPort;
        if(save_status == true)
        {
            qDebug() << "emit serial connected ...";
            emit onSerialConneceted(this->mPort);
        }
        else
        {
            qDebug() << "emit serial disconnected ......";
            emit onSerialDisconnected();
        }
    }

    QTimer::singleShot(1000, this, SLOT(SerialWatchLoop()));
}
