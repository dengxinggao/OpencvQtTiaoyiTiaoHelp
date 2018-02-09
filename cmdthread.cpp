#include "cmdthread.h"

#include <QProcess>
#include <QDebug>
#include <QImage>
#include <QTime>
#include <string.h>


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <math.h>


//#define Debug_Dantiao//只跳一次

//#define Debug_NReadImg//不读图

//#define Debug_NoSendCmd//不发指令

cv::Mat dispimage ;

cmdthread::cmdthread()
{
    Stop = false;

    QTime tm;
    tm = QTime::currentTime();
    srand(tm.hour()*1440000+tm.minute()*60000+tm.second()*1000+tm.msec());
    maxcishu = 3+qrand()%6;
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
    QProcess process;QString adbstring ;
    while(!Stop)
    {
        #ifdef Debug_Dantiao
        Stop = true;
        #endif
        #ifndef Debug_NReadImg
        process.start("adb shell screencap /sdcard/screen.png");
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
        #endif

        xiaorenimage = cv::imread("xiaoren.png");
        cv::resize(xiaorenimage,xiaorenimage,cv::Size(76*LocMaxX/1080,209*LocMaxY/1920));


        qipanimage = cv::imread("screen.png");
        dispimage = cv::imread("screen.png");



        cv::matchTemplate(qipanimage,xiaorenimage,xiaorenmtresult,cv::TM_CCOEFF_NORMED);
        cv::normalize( xiaorenmtresult, xiaorenmtresult, 0, 1, cv::NORM_MINMAX, -1, cv::Mat() );

        cv::minMaxLoc( xiaorenmtresult, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat() );

        cv::GaussianBlur(qipanimage,qipanimagegs,cv::Size(7,7),0,0);//高斯模糊
        cv::Canny(qipanimagegs,qipanimage,1,10,3);


        cv::rectangle(qipanimage,cv::Point(maxLoc.x-2,maxLoc.y-2),cv::Point(maxLoc.x+(76*LocMaxX/1080)+2,maxLoc.y+(209*LocMaxY/1920)+2),(0,0,0),CV_FILLED);




        int i,j;
        int firstpoint_flag,firstpoint_x,firstpoint_x1,firstpoint_x2,firstpoint_y,rightpoint_x,rightpoint_y,minx,maxx;
        firstpoint_flag = 0;
        minx=9999,maxx=0;
        for(i=LocFeny;i<(maxLoc.y+BoziY);i++)//row,y只找到小人脖子下
        {
            if((maxLoc.x+JiaoX) < (LocMaxX/2))//在左搜右
            {
                for(j=(maxLoc.x+JiaoX);j<LocMaxX;j++)//col,x
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

            }
            else//在右搜左
            {
                //for(j=1;j<( maxLoc.x + (LocMaxX/2) )/2;j++)//col,x
                //for(j=1;j<( (maxLoc.x-JiaoX) + (LocMaxX/2) )/2;j++)//col,x
                for(j=1;j<(LocMaxX/2);j++)//col,x
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

            }

            //if(firstpoint_flag)break;//只找最高点
            if(firstpoint_flag)firstpoint_flag=2;
        }
        maxLoc.x = maxLoc.x +  JiaoX;
        maxLoc.y = maxLoc.y +  JiaoY;
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
            if((tiao_y - firstpoint_y) > (JiaoX*1.3) )
            {
                tiao_y = tiao_y - JiaoX*0.5;//qrand()%(JiaoX/2);
                if(tiao_x>maxLoc.x)
                {
                    tiao_x = tiao_x + JiaoX*0.5;//qrand()%(JiaoX/2);
                }
                else
                {
                    tiao_x = tiao_x - JiaoX*0.5;//qrand()%(JiaoX/2);
                }
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

        cv::line(dispimage,maxLoc,cv::Point(tiao_x,tiao_y),(255,255,255),3,8);

        quint64 maxduijiaoxianlenth;
        maxduijiaoxianlenth = sqrt(LocMaxX*LocMaxX + LocMaxY*LocMaxY);
        if(lenth<maxduijiaoxianlenth)
        {
            quint64 timems,ax,ay;
            ax = LocMaxX/2 + (qrand()%(TouchYMax-TouchYMin)) - (TouchYMax-TouchYMin)/2;
            ay = (qrand()%(TouchYMax-TouchYMin)) + TouchYMin;
            timems = LenMultiple * lenth ;
            QString s;
            s = "adb shell input swipe " + QString::number(ax, 10) + " " + QString::number(ay, 10) + " " +  QString::number(ax, 10) + " " +  QString::number(ay, 10) + " " + QString::number((int)timems, 10);
            #ifndef Debug_NoSendCmd
            process.start(s);
            process.waitForFinished();
            #endif

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
            xxm = 2000 + qrand()%8000;
            emit adbstringsend("跳了" + QString::number(cishu,10) + "下,休息"+ QString::number(xxm,10) +"毫秒再跳");
            cishu = 0;
            maxcishu = 3+qrand()%6;
            if(AutoPian)
            {
                RandMove = !RandMove;
                emit pianchange(RandMove);
            }
            #ifndef Debug_Dantiao
            msleep(xxm);
            #endif
        }
        else
        {
            #ifndef Debug_Dantiao
            msleep((qrand()%1000)+1000);
            #endif
        }//msleep(1);
    }
    emit chengxujieshu();
}
