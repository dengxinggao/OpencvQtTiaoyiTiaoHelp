#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <windows.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("跳一跳助攻Ver1802021601");
    connect(&t,SIGNAL(adbstringsend(QString)),this,SLOT(textappen(QString)));
    connect(&t,SIGNAL(chengxujieshu()),this,SLOT(xianchengjieshu()));
    connect(&t,SIGNAL(displaymat()),this,SLOT(labechange()));
    ui->pushButton_start->setDisabled(true);
    ui->pushButton_stop->setDisabled(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::on_pushButton_stop_clicked()
{
    QProcess process;
    process.start("taskkill /f /im adb.exe");
    process.waitForFinished();
    t.stop();
    ui->pushButton_stop->setDisabled(true);
    ui->pushButton_connect->setEnabled(true);
}

void MainWindow::textappen(QString adbstring)
{
    ui->textBrowser->append(adbstring);
}

void MainWindow::xianchengjieshu()
{

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
    ui->label->setPixmap(QPixmap::fromImage(img).scaledToWidth(270,Qt::SmoothTransformation));

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

            process.start("adb shell screencap /sdcard/screen.png");
            process.waitForFinished();
            adbstring = process.readAllStandardError();
            if ( "error" == adbstring.mid(0,5) )
            {
                //EquipmentOk = false;
                ui->textBrowser->append("数据获取失败，检查连接，重新开始");
                return;
             }

            process.start("adb pull /sdcard/screen.png");
            process.waitForFinished();
            adbstring = QString::fromLocal8Bit(process.readAllStandardError()    );
            if ( "error" == adbstring.mid(0,5) )
            {
                   //EquipmentOk = false;
                   ui->textBrowser->append("数据获取失败，检查连接，重新开始");
                   return;
            }

            fbl = new QImage("screen.png");


            if(!fbl->isNull())
            {
                ui->pushButton_connect->setDisabled(true);
                ui->pushButton_start->setEnabled(true);
                ui->lineEdit_Beilv->setEnabled(true);
                ui->lineEdit_Feny->setEnabled(true);
                ui->label->setPixmap(QPixmap("screen.png").scaledToWidth(270,Qt::FastTransformation));
                if((fbl->size().width()/10 == 108)&(fbl->size().height()/10 == 192))
                {
                    LenMultiple = PS1080x1920Multiple;
                    EquipmentOk = true;
                    ui->textBrowser->append("设备分辨率:1080x1920");
                    ui->textBrowser->append("打开跳一跳，点击开始游戏，就可以点击<开始跳>了");
                    ui->lineEdit_Beilv->setText(QString::number(PS1080x1920Multiple,'f',2));
                    ui->lineEdit_Feny->setText(QString::number(PS1080x1920Feny,10));
                    LenMultiple = PS1080x1920Multiple;
                    LocFeny = PS1080x1920Feny;
                    LocMaxX=1080;
                    LocMaxY=1920;
                    WeiZhuangfanwei = 80;
                }
                else
                {
                    LocMaxX=fbl->size().width();
                    LocMaxY=fbl->size().height();
                    ui->textBrowser->append(QString::number(LocMaxX,10) + "x" + QString::number(LocMaxY,10) + "还未支持，需要手工设置调试");
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
    //if(EquipmentOk)
    {

        bool f1,f2;
        t.WeiZhuangfanwei = WeiZhuangfanwei;
        LocMaxX=fbl->size().width();
        LocMaxY=fbl->size().height();
        LenMultiple = ui->lineEdit_Beilv->text().toDouble(& f1);
        LocFeny = ui->lineEdit_Feny->text().toDouble(& f2);

        if( f1 & f2 )
        {
            t.LocFeny=LocFeny;
            t.LocMaxX=LocMaxX;
            t.LocMaxY=LocMaxY;
            t.RandMove = ui->checkBox->checkState();;
            t.LenMultiple = LenMultiple;
            ui->lineEdit_Beilv->setDisabled(true);
            ui->lineEdit_Feny->setDisabled(true);
            //ui->checkBox->setDisabled(true);
            ui->pushButton_start->setDisabled(true);
            t.qidong();
            t.start();
        }
        else
        {
            ui->textBrowser->append("配置失败,无法启动");

        }


    }
}

void MainWindow::on_checkBox_clicked()
{
    t.RandMove = ui->checkBox->checkState();;
}
