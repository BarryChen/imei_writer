#ifndef PROCESS_H
#define PROCESS_H

#include <QObject>
#include <QProcess>
#include <QDebug>

class BaseProcess : public QObject
{
    Q_OBJECT
public:
    BaseProcess();
    ~BaseProcess();

    void ExeCmd(QString cmdline, QStringList opt);

protected:
    QProcess *mProcess;
private:
    void killProcess();

};

#endif // PROCESS_H
