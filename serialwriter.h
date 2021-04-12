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

    void WriteSnImei(QString port, const QString sn, const QString imei);

private:
    QSerialPort *mSerial;

    int SerialOpen(QString &name);
signals:
    void postImeiSn(QString num);

public slots:
    void SerialReceive();

signals:

};

#endif // WRITERPROCESS_H
