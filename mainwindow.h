#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cmdthread.h"


#define PS1080x1920Multiple 1.38
#define PS1080x1920Feny 300


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    cmdthread t;
    quint16 LocFeny,LocMaxX,LocMaxY;
    bool EquipmentOk;
    double LenMultiple;
    QImage *fbl;
    qint64 WeiZhuangfanwei;

private slots:

    void on_pushButton_stop_clicked();

    void textappen(QString adbstring);

    void xianchengjieshu();

    void labechange();

    void on_pushButton_connect_clicked();

    void on_pushButton_start_clicked();

    void on_checkBox_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
