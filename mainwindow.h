#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "cmdthread.h"


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
    bool EquipmentOk;
    QImage *fbl;

private slots:

    void on_pushButton_stop_clicked();

    void textappen(QString adbstring);

    void xianchengjieshu();

    void labechange();

    void checkchange(bool bb);

    void on_pushButton_connect_clicked();

    void on_pushButton_start_clicked();

    void on_checkBox_clicked();

    void on_lineEdit_LenMultiple_textChanged(const QString &arg1);

    void on_checkBox_2_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
