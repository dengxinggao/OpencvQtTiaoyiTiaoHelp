#include "cmdthread.h"

#include <QProcess>
#include <QDebug>
#include <QImage>
#include <QTime>


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>

cv::Mat dispimage ;

cmdthread::cmdthread()
{
    Stop = false;
    xiaorenimage = cv::imread("xiaoren.png");
    QTime tm;
    tm = QTime::currentTime();
    srand(tm.hour()*1440000+tm.minute()*60000+tm.second()*1000+tm.msec());
    maxcishu = qrand()%20;
    cishu = 0;
}

void cmdthread::stop()
{
    Stop = true;
}

void cmdthread::qidong()
{
    Stop = false;
}

void cmdthread::run()
{
    QProcess process;
    while(!Stop)
    {
        process.start("adb shell screencap /sdcard/screen.png");
        QString adbstring ;
        process.waitForFinished();
        adbstring = process.readAllStandardError();
        adbstring.chop(2);
        if ( "error" == adbstring.mid(0,5) )
        {
            //EquipmentOk = false;
            emit adbstringsend("数据获取失败，检查连接，重新开始");
            emit chengxujieshu();
            return;
         }

        process.start("adb pull /sdcard/screen.png");
        process.waitForFinished();
        adbstring = QString::fromLocal8Bit(process.readAllStandardError()    );
        if ( "error" == adbstring.mid(0,5) )
        {
               //EquipmentOk = false;
               emit adbstringsend("数据获取失败，检查连接，重新开始");
               emit chengxujieshu();
               return;
        }
        //emit adbstringsend("截图成功");

        qipanimage = cv::imread("screen.png");
        dispimage = cv::imread("screen.png");



        cv::matchTemplate(qipanimage,xiaorenimage,xiaorenmtresult,cv::TM_CCOEFF_NORMED);
        cv::normalize( xiaorenmtresult, xiaorenmtresult, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );

        cv::minMaxLoc( xiaorenmtresult, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );
        //qDebug()<<minVal<<maxVal;

        cv::GaussianBlur(qipanimage,qipanimagegs,cv::Size(7,7),0,0);//高斯模糊
        cv::Canny(qipanimagegs,qipanimage,1,10,3);

        cv::rectangle(qipanimage,maxLoc,cv::Point(maxLoc.x+78,maxLoc.y+211),(0,0,0),CV_FILLED);

        maxLoc.x = maxLoc.x + 39;
        maxLoc.y = maxLoc.y + 193;

        int i,j;
        int firstpoint_flag,firstpoint_x,firstpoint_x1,firstpoint_x2,firstpoint_y,rightpoint_x,rightpoint_y,minx,maxx;
        firstpoint_flag = 0;
        minx=9999,maxx=0;
        for(i=LocFeny;i<(maxLoc.y-123);i++)//row,y只找到小人脖子下
        {
            for(j=1;j<LocMaxX;j++)//col,x
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
                    if(j>(maxx+3))
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
        //根据小人位置和最高点计算目标中心点，距离
        if(tiao_x>maxLoc.x)
        {
            tiao_y = maxLoc.y - (tiao_x - maxLoc.x)*tan(30*M_PI/180);
            lenth = (tiao_x - maxLoc.x )/cos(30*M_PI/180);
        }
        else
        {
            tiao_y = maxLoc.y - (maxLoc.x - tiao_x)*tan(30*M_PI/180);
            lenth = (maxLoc.x - tiao_x)/cos(30*M_PI/180);
        }
        //用最右坐标修正小人位置
        tiao_y = (tiao_y + rightpoint_y)/2;
        cv::circle(dispimage,cv::Point(tiao_x,tiao_y),5,(255,255,255),-1);//标出人位置
        //伪装小人位置
        if(RandMove)
        {
            quint64 weizhuangchanshu;
            weizhuangchanshu = tiao_y - firstpoint_y;

            if(weizhuangchanshu>WeiZhuangfanwei)
            {
                tiao_y = tiao_y - qrand()%(WeiZhuangfanwei/2);
                tiao_x = tiao_x - qrand()%(WeiZhuangfanwei/2);
                //cv::circle(dispimage,cv::Point(tiao_x,tiao_y),5,(255,255,255),-1);//标出人位置
                emit adbstringsend("偏一下");
            }

        }

        //计算目标中心点，距离
        if(tiao_x>maxLoc.x)
        {
            lenth = (tiao_x - maxLoc.x)*(tiao_x - maxLoc.x) +  (tiao_y - maxLoc.y)*(tiao_y - maxLoc.y);
        }
        else
        {
            lenth = (maxLoc.x - tiao_x)*(maxLoc.x - tiao_x) + (tiao_y - maxLoc.y)*(tiao_y - maxLoc.y);
        }
        lenth = sqrt(lenth);

        //cv::circle(dispimage,maxLoc,5,(255,255,255),-1);//标出人位置
        cv::line(dispimage,maxLoc,cv::Point(tiao_x,tiao_y),(255,255,255),3,8);
        //cv::circle(dispimage,cv::Point(firstpoint_x,firstpoint_y),5,(255,255,255),-1);//标出人位置
        //cv::circle(dispimage,cv::Point(tiao_x,tiao_y),5,(255,255,255),-1);//标出人位置
        //cv::circle(dispimage,cv::Point(rightpoint_x,rightpoint_y),5,(255,255,255),-1);//标出人位置

        quint64 maxduijiaoxianlenth;
        maxduijiaoxianlenth = sqrt(LocMaxX*LocMaxX + LocMaxY*LocMaxY);
        if(lenth<maxduijiaoxianlenth)
        {
            quint64 timems,ax,ay;
            ax = LocMaxX/2 + qrand()%140;
            ay = LocMaxY - 200 + qrand()%120;
            timems = LenMultiple * lenth ;//- 20 +  qrand()%40;
            QString s;
            s = "adb shell input swipe " + QString::number(ax, 10) + " " + QString::number(ay, 10) + " " +  QString::number(ax, 10) + " " +  QString::number(ay, 10) + " " + QString::number((int)timems, 10);
            process.start(s);
            process.waitForFinished();

            emit adbstringsend("距离"+QString::number((int)lenth,10)+",按压" + QString::number(timems,10) + "ms");
        }
        else
        {
            emit adbstringsend("测量错误！");
        }

        emit displaymat();

        cishu++;
        if(cishu>maxcishu)
        {
            quint16 xxm;
            xxm = 5000 + qrand()%5000;
            emit adbstringsend("跳了" + QString::number(cishu,10) + "下,休息"+ QString::number(xxm,10) +"毫秒再跳");
            msleep(xxm);
            cishu = 0;
            maxcishu = qrand()%20;
        }
        else
        {
            msleep((qrand()%1000)+1500);
        }
    }
}
