#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCloseEvent>
#include <QDebug>
#include <windows.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("跳一跳助攻Ver1.3.3");
    connect(&t,SIGNAL(adbstringsend(QString)),this,SLOT(textappen(QString)));
    connect(&t,SIGNAL(chengxujieshu()),this,SLOT(xianchengjieshu()));
    connect(&t,SIGNAL(displaymat()),this,SLOT(labechange()));
    connect(&t,SIGNAL(pianchange(bool)),this,SLOT(checkchange(bool)));
    ui->pushButton_start->setDisabled(true);
    ui->pushButton_stop->setDisabled(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_pushButton_stop_clicked()
{
    t.stop();
    ui->pushButton_stop->setDisabled(true);
    ui->pushButton_connect->setEnabled(true);
    ui->pushButton_start->setEnabled(true);
}

void MainWindow::textappen(QString adbstring)
{
    ui->textBrowser->append(adbstring);
}

void MainWindow::xianchengjieshu()
{
    ui->pushButton_connect->setEnabled(true);
    ui->pushButton_start->setEnabled(true);
}

void MainWindow::labechange()
{
    QImage img;
    if(dispimage.channels()==3)
    {
        cv::cvtColor(dispimage,dispimage,CV_BGR2RGB);
        img = QImage((const uchar*)(dispimage.data),dispimage.cols,dispimage.rows,dispimage.cols*dispimage.channels(),QImage::Format_RGB888);
    }
    else
    {
        img = QImage((const uchar*)(dispimage.data),dispimage.cols,dispimage.rows,dispimage.cols*dispimage.channels(),QImage::Format_Indexed8);
    }

    //ui->label->setPixmap(QPixmap::fromImage(img).scaledToHeight(ui->label->size().height(),Qt::FastTransformation));

    ui->label->setPixmap(QPixmap::fromImage(img).scaledToWidth(270,Qt::FastTransformation));
}

void MainWindow::checkchange(bool bb)
{
    if(bb)
        ui->checkBox->setCheckState(Qt::Checked);
    else
        ui->checkBox->setCheckState(Qt::Unchecked);
}

void MainWindow::on_pushButton_connect_clicked()
{
    EquipmentOk = false;
    QProcess process;
    process.start("adb version");
    process.waitForFinished();
    QString adbversionstring;
    adbversionstring = QString::fromLocal8Bit(process.readAllStandardOutput()    );
    if("Android Debug Bridge" == adbversionstring.mid(0,20))
    {
        ui->textBrowser->append("adb启动成功");
        process.start("adb shell getprop ro.product.model");
        process.waitForFinished();
        QString adbstring;
        adbstring = QString::fromLocal8Bit(process.readAllStandardError()    );
        if( "error" != adbstring.mid(0,5)  )
        {
            adbstring = QString::fromLocal8Bit(process.readAllStandardOutput()    );
            adbstring.chop(3);
            ui->textBrowser->append("设备型号:"+adbstring);

            process.start("adb shell screencap /sdcard/fbl.png");
            process.waitForFinished();
            adbstring = process.readAllStandardError();
            if ( "error" == adbstring.mid(0,5) )
            {
                //EquipmentOk = false;
                ui->textBrowser->append("数据获取失败，检查连接，重新开始");
                return;
             }

            process.start("adb pull /sdcard/fbl.png");
            process.waitForFinished();
            adbstring = QString::fromLocal8Bit(process.readAllStandardError()    );
            if ( "error" == adbstring.mid(0,5) )
            {
                   EquipmentOk = false;
                   ui->textBrowser->append("数据获取失败，检查连接，重新开始");
                   return;
            }

            fbl = new QImage("fbl.png");


            if(!fbl->isNull())
            {
                if(fbl->size().height()*10/fbl->size().width() == 17)
                {
                    EquipmentOk = true;

                    ui->pushButton_start->setEnabled(true);
                    ui->label->setPixmap(QPixmap("fbl.png").scaledToWidth(270,Qt::FastTransformation));

                    t.LocMaxX = fbl->size().width();
                    t.LocMaxY = fbl->size().height();
                    t.LenMultiple =  1080 * 1.365 / t.LocMaxX;

                    t.LocFeny = 290 * t.LocMaxX / 1080 + 2;
                    t.BoziY = 88 * t.LocMaxX / 1080;
                    t.JiaoX = 38 * t.LocMaxX / 1080;
                    t.JiaoY = 191 * t.LocMaxX / 1080;
                    t.TouchYMin = 1507 * t.LocMaxX / 1080;
                    t.TouchYMax = 1606 * t.LocMaxX / 1080;
                    t.RandMove = ui->checkBox->checkState();
                    t.AutoPian = ui->checkBox_2->checkState();
                    ui->lineEdit_LenMultiple->setText(QString::number(t.LenMultiple,10,3));
                    ui->textBrowser->append("设备分辨率:" + QString::number(t.LocMaxX,10) + "x" + QString::number(t.LocMaxY,10) );
                    ui->textBrowser->append("打开跳一跳，点击开始游戏，就可以点击<开始跳>了");
                }
                else
                {
                    ui->textBrowser->append("设备分辨率:" + QString::number(t.LocMaxX,10) + "x" + QString::number(t.LocMaxY,10) + "比例不支持");
                }
            }
            else
            {
                ui->textBrowser->append("数据获取失败，检查连接和驱动，重启程序");
            }
        }
        else
        {
            ui->textBrowser->append("手机未连接,检查手机数据线连接,或者手机ADB驱动未安装");
        }
    }
    else
    {
        ui->textBrowser->append("adbshell没能正常启动，请检查adb!");
    }
    if(EquipmentOk)
    {
        ui->pushButton_start->setEnabled(true);
        ui->pushButton_stop->setEnabled(true);
    }
}

void MainWindow::on_pushButton_start_clicked()
{
    if(EquipmentOk)
    {

        ui->pushButton_connect->setDisabled(true);

        ui->pushButton_stop->setEnabled(true);
        ui->pushButton_start->setDisabled(true);

        t.qidong();
        t.start();

    }
    else
    {
        ui->textBrowser->append("sorry,还未支持,不能启动");
    }

}

void MainWindow::on_checkBox_clicked()
{
    t.RandMove = ui->checkBox->checkState();;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QProcess process;
    process.start("taskkill /f /im adb.exe");
    process.waitForFinished();
    t.stop();
    event->accept();

}

void MainWindow::on_lineEdit_LenMultiple_textChanged(const QString &arg1)
{
    bool f;
    double beilv;
    beilv = arg1.toDouble(&f);
    if(!f)
    {
        ui->textBrowser->append("倍率设置无效");
    }
    else
    {
        t.LenMultiple=beilv;
    }
}

void MainWindow::on_checkBox_2_stateChanged(int arg1)
{
    if(ui->checkBox_2->checkState())
    {
        ui->checkBox->setDisabled(true);t.AutoPian=true;
    }
    else
    {
        ui->checkBox->setEnabled(true);t.AutoPian=false;
    }
}
