#include "statusmachine.h"

#include <QTimer>

StatusMachine::StatusMachine(QObject *parent) : QObject(parent)
{
    this->mStatus = STATUS_ADB_DISCONNECTED;
    this->mAdbWatch = nullptr;
    this->mSerialWatch = nullptr;
    this->mSerialWriter = nullptr;
qDebug() << "StatusMachine";
    InitAdbWatch();
    InitSerialWatch();
    this->mSerialWriter = new SerialWriter;

    QTimer::singleShot(500, this, SLOT(StatusLoop()));

}

StatusMachine::~StatusMachine()
{
    if(this->mAdbWatch != nullptr) {
        qDebug() << "will delete adbwatch";
        delete this->mAdbWatch;
        this->mAdbWatch = nullptr;
    }

    if(this->mSerialWatch != nullptr) {
        qDebug() << "will delete mSerialWatch" ;
        delete this->mSerialWatch;
        this->mSerialWatch = nullptr;
    }

    if(this->mSerialWriter != nullptr) {
        qDebug() << "will delete mSerialWriter" ;
        delete this->mSerialWriter;
        this->mSerialWriter = nullptr;
    }
}

void StatusMachine::InitAdbWatch()
{
    this->mAdbWatch = new AdbWatch();
    connect(this->mAdbWatch, SIGNAL(onUsbConnect()), this, SLOT(AdbConnect()));
    connect(this->mAdbWatch, SIGNAL(onUsbDisconnect()), this, SLOT(AdbDisconnect()));

}

void StatusMachine::InitSerialWatch()
{
    this->mSerialWatch = new SerialWatch;
    connect(this->mSerialWatch, SIGNAL(onSerialConneceted(QString)), this, SLOT(SerialConneceted(QString)));
    connect(this->mSerialWatch, SIGNAL(onSerialDisconnected()), this, SLOT(SerialDisconnected()));
}

void StatusMachine::SerialConneceted(QString port)
{
    this->mSerialPort = port;
    this->mStatus = STATUS_SERIAL_CONNECTED;
}

void StatusMachine::SerialDisconnected()
{
    this->mStatus = STATUS_SERIAL_DISCONNECTED;
}

void StatusMachine::AdbConnect()
{
    this->mStatus = STATUS_ADB_CONNECTED;
}

void StatusMachine::AdbDisconnect()
{
    this->mStatus = STATUS_ADB_DISCONNECTED;
}

void StatusMachine::StatusLoop()
{
    QString sn, imei;
    static USB_STATUS save_status = STATUS_ADB_DISCONNECTED;
    qDebug() << "usb status:" << save_status;
    if(save_status != this->mStatus)
    {
        save_status = this->mStatus;
        emit onUsbStatusChanged(save_status);
        if(save_status == STATUS_ADB_CONNECTED) {
            emit onAdbConnected();
            mAdbSwUsb.AdbSwUsb();//发送切换usb到cp的命令
        } else if(save_status == STATUS_SERIAL_CONNECTED) {
            emit onSmSerialConnected();
            //this->mSerialWriter->WriteSnImei(this->mSerialPort, this->mSn, this->mImei);//开始烧写出号
        }
    }
    QTimer::singleShot(1000, this, SLOT(StatusLoop()));
}

void StatusMachine::SMWriteSnImei(QString sn, QString imei)
{
    this->mSerialWriter->WriteSnImei(this->mSerialPort, sn, imei);
}
