#include "help.h"
#include "ui_help.h"

Help::Help(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Help)
{
    ui->setupUi(this);
    setWindowTitle("自定义设置说明");
}

Help::~Help()
{
    delete ui;
}
