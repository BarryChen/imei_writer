#include "formabout.h"
#include "ui_formabout.h"

FormAbout::FormAbout(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormAbout)
{
    ui->setupUi(this);
    setWindowFlags(this->windowFlags() &
                   ~Qt::WindowMaximizeButtonHint & //隐藏最小化
                   ~Qt::WindowMinimizeButtonHint); //隐藏最大化
//    setWindowFlags(Qt::FramelessWindowHint);//隐藏边框
    setAttribute(Qt::WA_QuitOnClose, false);  //主窗口关闭时此窗口也关闭
    setMinimumSize(400, 300);
    setMaximumSize(400, 300);
    this->ui->sw_version->setText("version 1.2.10412");
}

FormAbout::~FormAbout()
{
    delete ui;
}
