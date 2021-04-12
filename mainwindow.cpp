#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "windows.h"

#include <QAbstractNativeEventFilter>
#include <dbt.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->mStatusMachine = nullptr;
    this->mWaitTimer = nullptr;
    this->mSerialIsConnected = false;
    this->fm = nullptr;

    ui->setupUi(this);
    this->setWindowFlag(Qt::WindowMaximizeButtonHint); //隐藏最大化，方式一
    //this->setWindowFlags(this->windowFlags()& ~Qt::WindowMaximizeButtonHint);//隐藏最大化，方式二
    this->setAttribute(Qt::WA_QuitOnClose, true); //主窗口退出时，其他子窗口也退出
    this->setMinimumSize(460, 700);//固定窗口大小
    this->setMaximumSize(460, 700);

    setTabOrder(this->ui->lineEdit_sn, this->ui->lineEdit_imei);
    setTabOrder(this->ui->lineEdit_imei, this->ui->pushButton_run);
    setTabOrder(this->ui->pushButton_run, this->ui->lineEdit_sn);

    connect(ui->lineEdit_sn, SIGNAL(returnPressed()), this, SLOT(NextLineEdit()));
    connect(ui->lineEdit_imei, SIGNAL(returnPressed()), this, SLOT(NextLineEdit()));

    this->show();//设置focus之前需要show一下，否则会被生效

    ui->lineEdit_sn->setFocus();
    ui->lineEdit_sn->selectAll();

    this->mStatusMachine = new StatusMachine(this);
    connect(this->mStatusMachine, SIGNAL(onSmSerialConnected()), this, SLOT(SerialConnected()));
    connect(this->mStatusMachine, SIGNAL(onUsbStatusChanged(USB_STATUS)), this, SLOT(UsbStatusChanged(USB_STATUS)));

    this->mWaitTimer = new QTimer(this);
    connect(this->mWaitTimer, SIGNAL(timeout()), this, SLOT(WaitTimerUpdate()));

    connect(this->mStatusMachine->mSerialWriter, SIGNAL(postImeiSn(QString)), this, SLOT(slotImeiSn(QString)));



}


MainWindow::~MainWindow()
{
    if(this->fm != nullptr) {
        delete this->fm;
    }
    if(this->mStatusMachine != nullptr) {
        qDebug() << "will delete mWriter" ;
        delete this->mStatusMachine;
        this->mStatusMachine = nullptr;
    }

    if(this->mWaitTimer != nullptr) {
        qDebug() << "will delete mWaitTimer" ;
        delete this->mWaitTimer;
        this->mWaitTimer = nullptr;
    }

    delete ui;
}

void MainWindow::slotImeiSn(QString data)
{
    this->ui->textEdit->append(data);
    this->ui->textEdit->append("================================");
}

void MainWindow::SerialConnected()
{
   this->mSerialIsConnected = true;
}

void MainWindow::UsbStatusChanged(USB_STATUS status)
{
   if(status == STATUS_ADB_CONNECTED)
   {
       this->ui->textEdit->append("当前USB状态为“ADB连接状态”");
   }
   else if(status == STATUS_ADB_DISCONNECTED)
   {
       this->ui->textEdit->append("当前USB状态为“ADB断开状态”");
   }
   else if(status == STATUS_SERIAL_CONNECTED)
   {
       this->ui->textEdit->append("当前USB状态为“串口连接状态”");
       on_pushButton_run_clicked();
   }
   else if(status == STATUS_SERIAL_DISCONNECTED)
   {
       this->ui->textEdit->append("当前USB状态为“串口断开状态”");
   }
}

void MainWindow::AdbConnect()
{
    //ui->label_state->setText("USB连接状态：已经连接到AP");
}

void MainWindow::AdbDisconnect()
{
   // ui->label_state->setText(("USB连接状态：未连接"));
}

void MainWindow::NextLineEdit()
{
    if(ui->lineEdit_sn->hasFocus())
    {
        ui->lineEdit_imei->setFocus();
        ui->lineEdit_imei->selectAll();
    }
    else if(ui->lineEdit_imei->hasFocus())
    {
        ui->lineEdit_sn->setFocus();
        ui->lineEdit_sn->selectAll();
        ui->pushButton_run->setDefault(true);
        on_pushButton_run_clicked();
    }
}

/***
  *判断一个字符串是否为纯数字
  */
static bool isDigitStr(QString src)
{
    QByteArray ba = src.toLatin1();//QString 转换为 char*
     const char *s = ba.data();

    while(*s && *s>='0' && *s<='9') s++;

    if (*s)
    { //不是纯数字
        return false;
    }
    else
    { //纯数字
        return true;
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
    qDebug() << "sn rigit4:"<< sn.right(7);
    if(snIsOk && imeiIsOk)
    {
        if(!sn.startsWith("QXTX") ||
                !isDigitStr(sn.right(7)) ||
                !isDigitStr(imei)) {
            this->mWaitTimer->stop();
            this->ui->textEdit->append("输出信息：<输入号码有误>");
            return;
        }

        this->mSn = sn;
        this->mImei = imei;
        if(this->mSerialIsConnected)
        {
            this->mStatusMachine->SMWriteSnImei(sn, imei);
        }
        else
        {
            this->mWaitTimer->start(500);
            this->ui->textEdit->append("输出信息：<等待串口连接中>");
        }
    }
}

void MainWindow::WaitTimerUpdate()
{
    qDebug() << "wait timer update";
    if(this->mSerialIsConnected)
    {
        this->mWaitTimer->stop();
        this->mStatusMachine->SMWriteSnImei(this->mSn, this->mImei);
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
        }
        if(msg->wParam == DBT_DEVICEREMOVECOMPLETE)
        {
            qDebug("usb out");
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




void MainWindow::on_action_about_triggered()
{
    this->fm = new FormAbout;
    fm->show();
}
