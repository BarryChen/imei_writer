#ifndef WRITERPROCESS_H
#define WRITERPROCESS_H
#include "baseprocess.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>


class SerialWriter : public QObject
{
    Q_OBJECT
public:
    SerialWriter();
    ~SerialWriter();

    void WriteSnImei(const QString sn, const QString imei);

private:
    QStringList mExistPort;
    QSerialPort *mSerial;

    void SerialOpen(QString &name);


public slots:
    void SerialPostInfo();
    void SerialReceive();

signals:
    void PostSerialInfo(QStringList&);
};

#endif // WRITERPROCESS_H
