#ifndef CMDTHREAD_H
#define CMDTHREAD_H

#include <QObject>
#include <QProcess>
#include <QThread>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

extern cv::Mat dispimage ;

class cmdthread : public QThread
{
    Q_OBJECT

public:
    cmdthread();
    void stop();
    void qidong();
    bool Stop,RandMove;
    cv::Mat qipanimage ;
    cv::Mat xiaorenimage ;
    cv::Mat xiaorenmtresult;
    cv::Mat qipanimagegs;
    double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
    int cnt;
    quint64 cishu,maxcishu,LocFeny,LocMaxX,LocMaxY;
    double LenMultiple,lenth;
    //小人参数
    quint64 xiaorenw;
    quint64 BoziY,JiaoX,JiaoY;
    quint64 TouchYMax,TouchYMin;
signals:
    void adbstringsend(QString);
    void chengxujieshu();
    void displaymat();
protected:
    void run();

};

#endif // CMDTHREAD_H
