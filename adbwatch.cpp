#include "adbwatch.h"
#include <QThread>
#include <QFile>
#include <QDir>

AdbWatch::AdbWatch()
{
    this->mTimer = nullptr;
    this->mServerIsStart = false;
    qDebug() << "adb watch";
    connect(this->mProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(AdbWatchStdOut()));
    connect(this->mProcess, SIGNAL(readyReadStandardError()), this, SLOT(AdbWatchStdErr()));
    connect(this->mProcess, SIGNAL(finished(int)), this, SLOT(AdbProFinished()));

    //将内置的adb文件拷贝出来
    CopyFile("adb.exe");
    CopyFile("AdbWinApi.dll");
    CopyFile("AdbWinUsbApi.dll");

    //一般电脑没使用过adb的，adb服务不会默认开启，此处要先开启服务
    StartAdbService();
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

void AdbWatch::AdbProFinished()
{
    //qDebug() << "service is start";
    this->mServerIsStart = true;
}


void AdbWatch::InitTimer()
{
    this->mTimer = new QTimer(this);
    connect(this->mTimer, SIGNAL(timeout()), this, SLOT(SlotTimer()));
    this->mTimer->start(1000);
}

void AdbWatch::CopyFile(QString filename)
{
    QString source_file(":/exe/" + filename);
    QString dest_file(filename);

    QFile f_source(source_file);
    QFile f_dest(dest_file);

    if(!f_source.exists())
    {
        qDebug() << "file no exists";
    }

    if(!f_dest.exists())
    {
        if(!QFile::copy(source_file, dest_file))
        {
            qDebug() << "copy adb file error";
        }
    }

}

bool AdbWatch::StartAdbService()
{
    QString cmd = "adb.exe";
    QStringList opt;
    opt << "start-server";

    ExeCmd(cmd, opt);
    return true;
}

void AdbWatch::StartAdbWatch()
{
    QString cmd = "adb.exe";
    QStringList opt;
    opt << "devices";
    ExeCmd(cmd, opt);

}

void AdbWatch::SlotTimer()
{
    this->mTimer->stop();
    if(this->mServerIsStart)
        StartAdbWatch();

    this->mTimer->start(1000);
}
