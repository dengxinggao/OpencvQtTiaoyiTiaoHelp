#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QTime>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&timer1,SIGNAL(timeout()),this,SLOT(tiao()));
    cishu = 0;
    stoptiao = false;
    QTime tm;
    tm = QTime::currentTime();
    srand(tm.hour()*1440000+tm.minute()*60000+tm.second()*1000+tm.msec());
    maxcishu = qrand()%15;
    ui->pushButton_start->setDisabled(true);
    ui->pushButton_stop->setDisabled(true);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    //timer1.start(2000);
    //cishu = 0;
    //stoptiao = false;
    EquipmentOk = false;
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
            ui->textBrowser->append("设备型号"+adbstring);
            process.start("adb shell wm size");
            process.waitForFinished();
            QString adbstring;
            adbstring = QString::fromLocal8Bit(process.readAllStandardOutput()    );
            if("error" != adbstring.mid(0,5))
            {
                adbstring.chop(3);
                if("Physical size: 1080x1920" == adbstring)
                {
                    LenMultiple = PS1080x1920Multiple;
                    EquipmentOk = true;
                    ui->textBrowser->append("设备分辨率:1080x1920");
                    ui->textBrowser->append("打开跳一跳，点击开始游戏，就可以点击<开始跳>了");
                }
                else
                {
                    ui->textBrowser->append("该手机分辨率"+ adbstring +"还未支持，请联系开发者添加");
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
    }
}

void MainWindow::tiao()
{
    timer1.stop();
    process.start("adb shell screencap /sdcard/screen.png");
    process.waitForFinished();
    QString adbstring;
    adbstring = QString::fromLocal8Bit(process.readAllStandardError()    );
    if ( "error" == adbstring.mid(0,5) )
    {
        EquipmentOk = false;
        ui->pushButton_start->setDisabled(true);
        ui->pushButton_stop->setDisabled(true);
        ui->textBrowser->append("数据获取失败，检查连接，重新开始");
        return;
    }



    process.start("adb pull /sdcard/screen.png");
    process.waitForFinished();
    adbstring = QString::fromLocal8Bit(process.readAllStandardError()    );

    if ( "error" == adbstring.mid(0,5) )
    {
        EquipmentOk = false;
        ui->pushButton_start->setDisabled(true);
        ui->pushButton_stop->setDisabled(true);
        ui->textBrowser->append("数据获取失败，检查连接，重新开始");
        return;
    }




    QImage img;
    cv::Mat qipanimage = cv::imread("screen.png");
    cv::Mat xiaorenimage = cv::imread("xiaoren.png");
    cv::Mat xiaorenmtresult;

    if(qipanimage.channels()==3)
    {
        cv::cvtColor(qipanimage,qipanimage,CV_BGR2RGB);
        img = QImage((const uchar*)(qipanimage.data),qipanimage.cols,qipanimage.rows,qipanimage.cols*qipanimage.channels(),QImage::Format_RGB888);
    }
    else
    {
        img = QImage((const uchar*)(qipanimage.data),qipanimage.cols,qipanimage.rows,qipanimage.cols*qipanimage.channels(),QImage::Format_Indexed8);
    }
    //ui->label->setPixmap(QPixmap::fromImage(img).scaledToWidth(270,Qt::SmoothTransformation));



    cv::matchTemplate(qipanimage,xiaorenimage,xiaorenmtresult,cv::TM_CCOEFF_NORMED);
    cv::normalize( xiaorenmtresult, xiaorenmtresult, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );
    double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
    cv::minMaxLoc( xiaorenmtresult, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );

    maxLoc.x = maxLoc.x + 39;
    maxLoc.y = maxLoc.y + 193;
    //cv::circle(qipanimage,maxLoc,5,(255,255,255),-1);//标出人位置
    cv::Mat qipanimagegs;
    cv::GaussianBlur(qipanimage,qipanimagegs,cv::Size(7,7),0,0);//高斯模糊
    cv::Canny(qipanimagegs,qipanimage,1,10,3);

    int i,j;
    for(i=0;i<78;i++)
    {
        for(j=0;j<211;j++)
        {
            qipanimage.at<uchar>(maxLoc.y-211+j,maxLoc.x-39+i) = 0;
        }
    }

    int firstpoint_flag,firstpoint_x,firstpoint_x1,firstpoint_x2,firstpoint_y,leftpoint_x,leftpoint_y,rightpoint_x,rightpoint_y,minx,maxx;
    firstpoint_flag = 0;
    minx=2000,maxx=0;
    for(i=300;i<(maxLoc.y);i++)//row,y
    {
        for(j=1;j<1080;j++)//col,x
        {
            if( qipanimage.at<uchar>(i,j) == 255)
            {
                if(firstpoint_flag<2)
                {
                    if( firstpoint_flag == 0)
                    {
                        firstpoint_flag = 1;firstpoint_x1 = j;firstpoint_y=i;
                    }
                    if( firstpoint_flag == 1)
                    {
                        firstpoint_x2 = j;firstpoint_y=i;
                    }
                }
                if(j<minx)
                {
                    minx=j;leftpoint_x = j;leftpoint_y = i;
                }
                if(j>maxx)
                {
                    maxx=j;rightpoint_x = j;rightpoint_y = i;
                }

            }

        }
        //if(firstpoint_flag)break;//只找最高点
        if(firstpoint_flag)firstpoint_flag=2;


    }
    //找出最高点
    firstpoint_x = (firstpoint_x2 + firstpoint_x1)/2;
    int tiao_x,tiao_y;
    tiao_x = firstpoint_x;
    //计算目标中心点，距离
    if(tiao_x>maxLoc.x)
    {
        tiao_y = maxLoc.y - (tiao_x - maxLoc.x)*tan(30*M_PI/180);
        lenth = (tiao_x - maxLoc.x)/cos(30*M_PI/180);
    }
    else
    {
        tiao_y = maxLoc.y - (maxLoc.x - tiao_x)*tan(30*M_PI/180);
        lenth = (maxLoc.x - tiao_x)/cos(30*M_PI/180);
    }
    lenth = LenMultiple * lenth ;
    //
    qipanimage = cv::imread("screen.png");
    cv::circle(qipanimage,maxLoc,6,(255,255,255),-1);//标出人位置
    //cv::circle(qipanimage,cv::Point(firstpoint_x,firstpoint_y),5,(255,255,255),-1);//标出最高位置
    cv::circle(qipanimage,cv::Point(tiao_x,tiao_y),6,(255,255,255),-1);//标出目标位置
    cv::line(qipanimage,maxLoc,cv::Point(tiao_x,tiao_y),(255,255,255),3,8);

    if(qipanimage.channels()==3)
    {
        cv::cvtColor(qipanimage,qipanimage,CV_BGR2RGB);
        img = QImage((const uchar*)(qipanimage.data),qipanimage.cols,qipanimage.rows,qipanimage.cols*qipanimage.channels(),QImage::Format_RGB888);
    }
    else
    {
        img = QImage((const uchar*)(qipanimage.data),qipanimage.cols,qipanimage.rows,qipanimage.cols*qipanimage.channels(),QImage::Format_Indexed8);
    }
    ui->label->setPixmap(QPixmap::fromImage(img).scaledToWidth(270,Qt::SmoothTransformation));


    QString s;
    //s = "adb shell input swipe " + QString::number((int)maxLoc.x, 10) + " " + QString::number((int)maxLoc.y, 10) + " " +  QString::number((int)tiao_x, 10) + " " +  QString::number((int)tiao_y, 10) + " " + QString::number((int)lenth, 10);
    s = "adb shell input swipe " + QString::number(470+(qrand()%140), 10) + " " + QString::number(1460+(qrand()%140), 10) + " " +  QString::number(470+(qrand()%140), 10) + " " +  QString::number(1461+(qrand()%140), 10) + " " + QString::number((int)lenth, 10);

    process.start(s);
    process.waitForFinished();
    ui->textBrowser->append(s.mid(16));

    if(stoptiao==false)
    {
        cishu++;
        if(cishu>maxcishu)
        {
            quint16 xxm;
            xxm = 5000 + qrand()%5000;
            timer1.start(xxm);
            ui->textBrowser->append("跳了" + QString::number(cishu,10) + "下,休息"+ QString::number(xxm,10) +"毫秒再跳");
            cishu = 0;
            maxcishu = qrand()%15;
        }
        else
        {
            timer1.start((qrand()%500)+1500);
        }
    }

}

void MainWindow::on_pushButton_stop_clicked()
{
    timer1.stop();
    stoptiao = true;
    ui->pushButton_stop->setDisabled(true);
    ui->pushButton_start->setEnabled(true);
}

void MainWindow::on_pushButton_start_clicked()
{
    timer1.start(2000);
    cishu = 0;
    stoptiao = false;
    ui->pushButton_start->setDisabled(true);
    ui->pushButton_stop->setEnabled(true);
}
