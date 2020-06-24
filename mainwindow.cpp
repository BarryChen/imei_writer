#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "windows.h"

#include <QAbstractNativeEventFilter>
#include <dbt.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->mAdbCmd   = nullptr;
    this->mAdbWatch = nullptr;
    this->mWriter   = nullptr;
    InitAdbWatch();
    this->mAdbIsConnected = false;
    this->mUsbIsConnected = false;
    this->mSerialIsConnected = false;
    //InitWriterProcess();

    this->mWritting = false;

    ui->setupUi(this);
    ui->label_state->setText("USB连接状态：未连接");
    ui->label_cp_uart ->setText("CP串口: 未连接");
    ui->lineEdit_sn->setFocus();
    connect(ui->lineEdit_sn, SIGNAL(returnPressed()), this, SLOT(NextLineEdit()));
    connect(ui->lineEdit_imei, SIGNAL(returnPressed()), this, SLOT(NextLineEdit()));

    QTimer::singleShot(1000, this, SLOT(ConnectStateLoop()));

}

MainWindow::~MainWindow()
{

    if(this->mAdbWatch != nullptr) {
        qDebug() << "will delete adbwatch";
        delete this->mAdbWatch;
        this->mAdbWatch = nullptr;
    }
    if(this->mAdbCmd != nullptr) {
        qDebug() << "will delete adbcmd" ;
        delete this->mAdbCmd;
        this->mAdbCmd = nullptr;
    }

    if(this->mWriter != nullptr) {
        qDebug() << "will delete mWriter" ;
        delete this->mWriter;
        this->mWriter = nullptr;
    }


    delete ui;
}

void MainWindow::StartSerialWriter()
{
    if(this->mWriter != nullptr) {
        this->mWriter = new SerialWriter();
        connect(this->mWriter, SIGNAL(PostSerialInfo(QStringList &)), this, SLOT(GetSerialInfo(QStringList &)));
    } else
        qDebug("请析构SerialWriter");

}

void MainWindow::StopSerialWriter()
{
    if(this->mWriter != nullptr) {
       disconnect(this->mWriter, SIGNAL(PostSerialInfo(QStringList &)), this, SLOT(GetSerialInfo(QStringList &)));
       delete this->mWriter;
       this->mWriter = nullptr;
    } else
        qDebug("已经结束了SerialWriter");

}


void MainWindow::GetSerialInfo(QStringList &s)
{
    if(!s.isEmpty()) {
        QStringList data;
        data << "CP串口:";
        data << *s.begin();
        qDebug() << "getsetialinfo";
        ui->label_cp_uart->setText(data.join(" "));
    }
}

void MainWindow::StartAdbCmd()
{
    qDebug() << "StartAdbCmd" ;
    if(this->mAdbCmd == nullptr)
        this->mAdbCmd = new AdbCmd();
}

void MainWindow::EndAdbCmd()
{
    if(this->mAdbCmd != nullptr)
    {
        delete this->mAdbCmd;
        this->mAdbCmd = nullptr;
    }
}

void MainWindow::InitAdbWatch()
{
    this->mAdbWatch = new AdbWatch();
    connect(this->mAdbWatch, SIGNAL(onUsbConnect()), this, SLOT(AdbConnect()));
    connect(this->mAdbWatch, SIGNAL(onUsbDisconnect()), this, SLOT(AdbDisconnect()));

}

void MainWindow::AdbConnect()
{
    ui->label_state->setText("USB连接状态：已经连接到AP");
    this->mAdbIsConnected = true;
    StartAdbCmd();
}

void MainWindow::AdbDisconnect()
{
    ui->label_state->setText(("USB连接状态：未连接"));
    this->mAdbIsConnected = false;
}

void MainWindow::NextLineEdit()
{
    if(ui->lineEdit_sn->hasFocus()){
        ui->lineEdit_imei->setFocus();
    } else if(ui->lineEdit_imei->hasFocus()){
        ui->pushButton_run->setFocus();
        ui->pushButton_run->setDefault(true);
    } else if(ui->pushButton_run->hasFocus()){
        on_pushButton_run_clicked();
    }

}


void MainWindow::on_pushButton_run_clicked()
{

    QString sn = ui->lineEdit_sn->text().trimmed();
    int len_sn = sn.count();
    QString imei = ui->lineEdit_imei->text().trimmed();
    int len_imei = imei.count();

    bool snIsOk = (len_sn > 0 && len_sn < 25);
    bool imeiIsOk = (len_imei > 0 && len_imei < 25);
    qDebug() << "run clicked sn:" << sn <<"  sn len:" << len_sn;
    qDebug() << "run clicked imei:" << imei <<"  sn len:" << len_imei;


    //test
    //StartWriterProcess(sn, imei);

    if(snIsOk && imeiIsOk)
    {
        if(sn.startsWith("QXTX")) {
            ui->pushButton_run->setText("号码输入有误");
            return;
        }
        if(this->mUsbIsConnected == false) {
            ui->pushButton_run->setText("没有检测到USB");
            //return;
        }
        ui->pushButton_run->setText("正在烧入");
        //TODO
        //切换usb到cp
        //this->mWritting = true;

    }
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
    MSG* msg = reinterpret_cast<MSG*>(message);
    unsigned int msgType = msg->message;

    if(msgType == WM_DEVICECHANGE) {
        //emit DeviceChangeCbk();
        if(msg->wParam == DBT_DEVICEARRIVAL)
        {
            qDebug("usb in");
            this->mUsbIsConnected = true;
        }
        if(msg->wParam == DBT_DEVICEREMOVECOMPLETE)
        {
            qDebug("usb out");
            this->mUsbIsConnected = false;
        }
    }
    return QWidget::nativeEvent(eventType, message, result);
}


void MainWindow::on_action_adbPath_triggered()
{

}

void MainWindow::on_action_run_triggered()
{

}

void MainWindow::ConnectStateLoop()
{
    if(this->mUsbIsConnected && this->mAdbIsConnected) {
        StartAdbCmd();
    } else if(this->mUsbIsConnected && !this->mAdbIsConnected) {
        EndAdbCmd();
        StartSerialWriter();
    }

    QTimer::singleShot(1000, this, SLOT(ConnectStateLoop()));
}
