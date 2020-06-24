#include "adbwatch.h"


AdbWatch::AdbWatch()
{
    this->mTimer = nullptr;
    connect(this->mProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(AdbWatchStdOut()));
    connect(this->mProcess, SIGNAL(readyReadStandardError()), this, SLOT(AdbWatchStdErr()));
    InitTimer();

}
AdbWatch::~AdbWatch()
{
    qDebug() << "delete adbwatch";
    delete this->mTimer;
}


void AdbWatch::AdbWatchStdOut()
{
    QString tmp;
    QString remove_str = "List of devices attached";
    static bool saveAdbStatus = false;

    QString data = this->mProcess->readAllStandardOutput();
    //qDebug() << "adb watch:" << data;

    tmp = data.remove(remove_str).trimmed();
    if(tmp != "") {
        mAdbIsConnected = true;
    } else {
        mAdbIsConnected = false;
    }

    if(saveAdbStatus == false && mAdbIsConnected == true)
    {
        //通知其他对象USB已经连接成功
        emit onUsbConnect();
    }
    if(saveAdbStatus == true && mAdbIsConnected == false)
    {
        //通知其他对象USB已经断开连接
        emit onUsbDisconnect();
    }

    saveAdbStatus = mAdbIsConnected;
}

void AdbWatch::AdbWatchStdErr()
{
    QString data = this->mProcess->readAllStandardError();
    qDebug() << "adb watch:" << data;
}

void AdbWatch::InitTimer()
{
    this->mTimer = new QTimer(this);
    connect(this->mTimer, SIGNAL(timeout()), this, SLOT(SlotTimer()));
    this->mTimer->start(1000);
}

void AdbWatch::StartAdbWatch()
{
#ifdef Q_OS_WIN32
    QString cmd = "adb.exe";
#else
    QString cmd = "adb";
#endif
    QStringList opt;
    opt << "devices";

    ExeCmd(cmd, opt);
}

void AdbWatch::SlotTimer()
{
    this->mTimer->stop();
    StartAdbWatch();

    this->mTimer->start(1000);
}
