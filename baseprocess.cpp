#include "baseprocess.h"
#include <QDebug>

BaseProcess::BaseProcess() : QObject()
{
    mProcess = new QProcess(this);
}

BaseProcess::~BaseProcess()
{
    qDebug() << "class BaseProcess delete";
    killProcess();
    delete this->mProcess;
}

void BaseProcess::killProcess()
{
    if(mProcess->state() == QProcess::Running)
    {
        mProcess->kill();
        if(!mProcess->waitForFinished())
            qDebug() << "kill process error" << mProcess->errorString();
    }
}
void BaseProcess::ExeCmd(QString cmdline, QStringList opt)
{
    killProcess();

    //qDebug() << "cmd start:" << cmdline << opt;
    mProcess->start(cmdline, opt);
    if(!mProcess->waitForStarted())
    {
        qDebug() << "start cmd" << cmdline << "error" << mProcess->errorString();
        return;
    }

}
