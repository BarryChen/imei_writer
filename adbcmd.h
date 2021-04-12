#ifndef ADBCMD_H
#define ADBCMD_H
#include "baseprocess.h"

/*
 * Adb 控制命令类
*/
class AdbCmd : public BaseProcess
{
    Q_OBJECT
public:
    AdbCmd();
    void AdbSwUsb();

public slots:
    void AdbCmdStdOut();
    void AdbCmdStdErr();
};

#endif // ADBCMD_H
