#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

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

private:
    Ui::MainWindow *ui;
    QSerialPort* SerialPort;
};
#endif // MAINWINDOW_H
