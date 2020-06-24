#include "serialwriter.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>

SerialWriter::SerialWriter()
{
    this->mExistPort.clear();
    this->mSerial = new QSerialPort(this);

    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        qDebug() <<"name        :" << info.portName();
        qDebug() <<"description :" << info.description();
        if(info.description() == "LeadCore CMCC AT Interface")
            this->mExistPort << info.portName();
    }
    QTimer::singleShot(20, this, SLOT(SerialPostInfo()));
    SerialOpen(*this->mExistPort.begin());
}


SerialWriter::~SerialWriter()
{
    qDebug() << "~SerialWriter";
    if(this->mSerial->isOpen())
    {
        this->mSerial->close();
    }
    delete this->mSerial;
    this->mSerial = nullptr;
}

void SerialWriter::SerialPostInfo()
{
    if(!this->mExistPort.isEmpty())
    {
        qDebug() << "postserialinfo";
        emit PostSerialInfo(this->mExistPort);
    }
}

void SerialWriter::SerialOpen(QString &name)
{
    if(this->mSerial->isOpen())
    {
        this->mSerial->clear();
        this->mSerial->close();
    }
    this->mSerial->setPortName(name);	 //	选取串口
    if(!this->mSerial->open(QIODevice::ReadWrite))  	//打开串口
    {
        qDebug() << "open serial error:" << name;
        return;
    }
    this->mSerial->setBaudRate(4000000);   //设置波特率
    this->mSerial->setDataBits(QSerialPort::Data8);  //设置数据位数
    this->mSerial->setParity(QSerialPort::NoParity); 	//设置奇偶校验
    this->mSerial->setStopBits(QSerialPort::OneStop);  	//设置停止位
    this->mSerial->setFlowControl(QSerialPort::NoFlowControl);	//设置流控制

    connect(this->mSerial, SIGNAL(readyRead()), this, SLOT(SerialReceive()));
    //WriteSnImei();

}

void SerialWriter::SerialReceive()
{
    QString data = this->mSerial->readAll();
    qDebug() << data;
}

void SerialWriter::WriteSnImei(const QString sn, const QString imei)
{
    //sn:    AT^DMSN=QXTXV320200600041
    //imei:  AT^DCGSNW=863220040000630
    if(sn.isEmpty() || imei.isEmpty())
        return;

    QString end = "\r\n";
    QString buff = "AT\r\n";
    QString sn_cmd = "AT^DMSN=" + sn + end;
    QString imei_cmd = "AT^DCGSNW=" + imei + end;

    qDebug() << sn_cmd.toLatin1().data() << "sn_cmd size:" << sn_cmd.size();
    if(this->mSerial->isOpen() && this->mSerial->isWritable())
    {
        if(this->mSerial->write(sn_cmd.toLatin1().data(), sn_cmd.size()) > 0)
        {
            qDebug("write sn success");
        }
        while(!this->mSerial->waitForBytesWritten());

        if(this->mSerial->write(imei_cmd.toLatin1().data(), imei_cmd.size()) > 0)
        {
            qDebug("write imei success");
        }
        while(!this->mSerial->waitForBytesWritten());
    }

    this->mSerial->clear();
    this->mSerial->close();
    //
}
