#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "adbwatch.h"
#include "adbcmd.h"
#include "serialwriter.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


protected:
    bool nativeEvent(const QByteArray &eventType, void *message, long *result);

private:
    AdbWatch        *mAdbWatch;//Adb连接状态守护程序
    AdbCmd          *mAdbCmd;//Adb命令程序
    SerialWriter    *mWriter;//串口写串号程序
    bool        mUsbIsConnected;
    bool        mAdbIsConnected;
    bool        mSerialIsConnected;
    bool        mWritting;//指示正在烧写

    void InitAdbWatch();

    void StartAdbCmd();    
    void EndAdbCmd();

    void StartSerialWriter();
    void StopSerialWriter();

    Ui::MainWindow *ui;

public slots:
    void AdbConnect();
    void AdbDisconnect();
    void NextLineEdit();
    void GetSerialInfo(QStringList &s);



private slots:
    void on_pushButton_run_clicked();
    void on_action_adbPath_triggered();
    void on_action_run_triggered();
    void ConnectStateLoop();
};
#endif // MAINWINDOW_H
