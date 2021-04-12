#include "serialwriter.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QThread>

SerialWriter::SerialWriter()
{
    this->mSerial = new QSerialPort(this);
    qDebug() << "new QSerialPort";
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

int SerialWriter::SerialOpen(QString &name)
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
        return -1;
    }
    this->mSerial->setDataTerminalReady(true);
    this->mSerial->setBaudRate(4000000);   //设置波特率
    this->mSerial->setDataBits(QSerialPort::Data8);  //设置数据位数
    this->mSerial->setParity(QSerialPort::NoParity); 	//设置奇偶校验
    this->mSerial->setStopBits(QSerialPort::OneStop);  	//设置停止位
    this->mSerial->setFlowControl(QSerialPort::NoFlowControl);	//设置流控制

    connect(this->mSerial, SIGNAL(readyRead()), this, SLOT(SerialReceive()));
    qDebug() << "open serial ok";
    return 0;
}

void SerialWriter::SerialReceive()
{
    QString data = this->mSerial->readAll();
    QStringList list = data.split("\r\n");
    
    for(int i = 0; i< list.size(); ++i)
    {
        QString tmp = list.at(i);
        if(tmp.contains("^DMSN"))
        {
            qDebug() << "###sn###:"<< "读取当前的SN为:" + tmp.right(tmp.size()-7);
            emit postImeiSn("读取当前的SN为:" + tmp.right(tmp.size()-7));
        }
        if(tmp.startsWith("^DGSN"))
        {
            qDebug() << "###imei###:"<< "读取当前的IMEI为:" + tmp.right(tmp.size()-7);
            emit postImeiSn("读取当前的IMEI为:" + tmp.right(tmp.size()-7));
        }
    }

    
    qDebug() << data;
}

void SerialWriter::WriteSnImei(QString port, const QString sn, const QString imei)
{
    if(SerialOpen(port) < 0)
        return;

    //sn:    AT^DMSN=QXTXV320200600041
    //imei:  AT^DCGSNW=863220040000630
//    if(sn.isEmpty() || imei.isEmpty())
//        return;

    QString end = "\r\n";
    QString buff = "AT\r\n";
    QString sn_cmd = "AT^DMSN=" + sn + end;
    QString imei_cmd = "AT^DCGSNW=" + imei + end;

    QString sn_query = "AT^DMSN" + end;
    QString imei_query = "AT^DGSN?" + end;

    qDebug() << sn_cmd.toLatin1().data() << "sn_cmd size:" << sn_cmd.size();
    if(this->mSerial->isOpen() && this->mSerial->isWritable())
    {
//         qDebug("will write at");
//        if(this->mSerial->write(buff.toLatin1().data(), buff.size()) > 0)
//        {
//            qDebug("write sn success");
//        }
//        while(!this->mSerial->waitForBytesWritten());
//        QThread::msleep(2000);

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


        //写完后查询下是否成功
        if(this->mSerial->write(sn_query.toLatin1().data(), sn_query.size()) > 0)
        {
            qDebug("write query sn success .... ");
        }
        while(!this->mSerial->waitForBytesWritten());

        if(this->mSerial->write(imei_query.toLatin1().data(), imei_query.size()) > 0)
        {
            qDebug("write query imei success ...");
        }
        while(!this->mSerial->waitForBytesWritten());
    }
    //QThread::msleep(2000);


    //this->mSerial->clear();
    //this->mSerial->close();
    //
}
