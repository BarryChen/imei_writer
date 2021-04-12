#ifndef STATUSMACHINE_H
#define STATUSMACHINE_H

#include <QObject>
#include "adbwatch.h"
#include "serialwatch.h"
#include "adbcmd.h"
#include "serialwriter.h"

typedef enum {
    STATUS_ADB_DISCONNECTED,
    STATUS_SERIAL_DISCONNECTED,
    STATUS_ADB_CONNECTED,
    STATUS_SERIAL_CONNECTED,
}USB_STATUS;

class StatusMachine : public QObject
{
    Q_OBJECT
public:
    SerialWriter    *mSerialWriter;

    explicit StatusMachine(QObject *parent = nullptr);
    ~StatusMachine();

    void SMWriteSnImei(QString sn, QString imei);

signals:
    void onSmSerialConnected();
    void onAdbConnected();
    void onUsbStatusChanged(USB_STATUS status);

public slots:
    void StatusLoop();

    void AdbConnect();
    void AdbDisconnect();

    void SerialDisconnected();
    void SerialConneceted(QString port);

private:
    USB_STATUS      mStatus;
    QString         mSerialPort;
    QString         mSn;
    QString         mImei;
    AdbWatch        *mAdbWatch;//Adb连接状态守护程序
    SerialWatch     *mSerialWatch;//串口状态守护程序


    AdbCmd      mAdbSwUsb;

    void InitAdbWatch();
    void InitSerialWatch();

};

#endif // STATUSMACHINE_H
