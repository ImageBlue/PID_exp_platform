#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QChartView>
#include <QtCharts>
#include <QString>
#include <iostream>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QBitArray>
#include <QMessageBox>
#include <QByteArray>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_CloseButton_clicked();

    void on_SetNewButton_clicked();

    void on_VNewButton_clicked();

    void on_ConnectSerialButton_clicked();

    void on_SearchSeiralButton_clicked();

    void on_CloseSerialButton_clicked();

    void sleep(int msec);       //延时函数

    void on_ClearButton_clicked();

    void Paint_Data();

private:
    Ui::MainWindow *ui;
    QSerialPort* SerialPort;
    QByteArray baBuffer;

    QTimer* timer;

    QChart *chart;
    QChartView *chartView;
    QValueAxis *AxisXT;        //时间坐标
    QValueAxis *AxisYV;        //实时流速坐标
    QSplineSeries *sSeriesV;

    QDateTime curTime;
    QString strTime;
};
#endif // MAINWINDOW_H
