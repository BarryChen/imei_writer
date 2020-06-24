#include "adbcmd.h"
#include <QDebug>

AdbCmd::AdbCmd()
{
#ifdef Q_OS_WIN32
    QString cmd = "adb.exe";
#else
    QString cmd = "adb";
#endif

    QStringList opt;
    opt << "shell";
    opt << "\"echo 1 > /sys/sta_misc/usb_sw_cp\"";

    connect(this->mProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(AdbCmdStdOut()));
    connect(this->mProcess, SIGNAL(readyReadStandardError()), this, SLOT(AdbCmdStdErr()));
    ExeCmd(cmd, opt);
}

void AdbCmd::AdbCmdStdOut()
{
    QString data = this->mProcess->readAllStandardOutput();
    qDebug() << "adb cmd:" << data;
}

void AdbCmd::AdbCmdStdErr()
{
    QString data = this->mProcess->readAllStandardError();
    qDebug() << "adb cmd:" << data;
}
