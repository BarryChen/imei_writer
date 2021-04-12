#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "formabout.h"
#include "adbcmd.h"
#include "serialwriter.h"
#include "statusmachine.h"


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
    StatusMachine   *mStatusMachine;
    bool        mSerialIsConnected;
    QString     mSn;
    QString     mImei;
    QTimer      *mWaitTimer;
    FormAbout   *fm;

//    void StartSerialWriter();
//    void StopSerialWriter();

    Ui::MainWindow *ui;

public slots:
    void AdbConnect();
    void AdbDisconnect();
    void NextLineEdit();
    void SerialConnected();
    void UsbStatusChanged(USB_STATUS status);

    void WaitTimerUpdate();
    void slotImeiSn(QString data);

private slots:
    void on_pushButton_run_clicked();
    void on_action_adbPath_triggered();
    void on_action_run_triggered();

    void on_action_about_triggered();

signals:
    void postSerialSnImei(QString sn, QString imei);
};
#endif // MAINWINDOW_H
