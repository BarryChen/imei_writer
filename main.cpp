#include "mainwindow.h"

#include <QApplication>
//adb root
//adb remount
//adb shell "echo 1 > /sys/sta_misc/usb_sw_cp"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
