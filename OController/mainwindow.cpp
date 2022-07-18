#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QBitArray>
#include <QMessageBox>
#include <QByteArray>
#include <QString>

/******************************************初始化全局变量*********************************************/
bool isManu = true;                  //手动模式开关，默认手动模式
float fParaP = 1.0;                  //PID参数
float fParaI = 1.0;
float fParaD = 1.0;
int iParaV = 0;                      //初始流速
bool bSerialFlag = false;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    /******************************************初始化控件*********************************************/
    ui->setupUi(this);

    ui->ManuRButton->setChecked(true);

    ui->PlineEdit->setText("1.0");
    ui->IlineEdit->setText("1.0");
    ui->DlineEdit->setText("1.0");
    ui->VlineEdit->setText("0");

    ui->CloseSerialButton->setEnabled(false);

    /******************************************初始化变量**********************************************/
    SerialPort = new QSerialPort;
    /******************************************初始化槽函数********************************************/
    connect(ui->ManuRButton, &QRadioButton::clicked, [=](){
        isManu = true;
        ui->SetNewButton->setEnabled(isManu);
        //qDebug() << "Manu Mode!";
    });

    connect(ui->AutoRButton, &QRadioButton::clicked, [=](){
        isManu = false;
        ui->SetNewButton->setEnabled(isManu);
        //qDebug() << "Auto Mode!";
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_CloseButton_clicked()
{
    exit(0);
}


void MainWindow::on_SetNewButton_clicked()
{
    QString qstrP, qstrI, qstrD;          //读取LineEdit中的字符串
    qstrP.clear();
    qstrI.clear();
    qstrD.clear();

    qstrP = ui->PlineEdit->text();
    qstrI = ui->IlineEdit->text();
    qstrD = ui->DlineEdit->text();

    float fTempP, fTempI, fTempD;         //字符串转float（暂存，等待合法性判断）
    fTempP = qstrP.toFloat();
    fTempI = qstrI.toFloat();
    fTempD = qstrD.toFloat();

    if(fTempP >= 0.0 && fTempI >= 0.0 && fTempD >= 0.0)  //数据合法，更新参数
    {
        fParaP = fTempP;
        fParaI = fTempI;
        fParaD = fTempD;
        qDebug() << "P:" << fParaP << " I:" << fParaI << " D:" << fParaD;
        //TODO:串口发送参数
    }
    else
    {
        qDebug() << "Unable to set PID: Invalid Parameter(s)!";
    }
}


void MainWindow::on_VNewButton_clicked()
{
    QString qstrV;                       //读取LineEdit中的字符串
    qstrV.clear();

    qstrV = ui->VlineEdit->text();       //字符串转int（暂存，等待合法性判断）

    int iTempV = qstrV.toInt();

    if(iTempV >= 0)                       //数据合法，更新参数
    {
        iParaV = iTempV;
        qDebug() << "Velocity:" << iParaV;
        //TODO:串口发送参数
    }
    else
    {
        qDebug() << "Unable to set V: Invalid Parameter!";
    }
}


void MainWindow::on_SearchSeiralButton_clicked()
{
    //查找可用的串口
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        QSerialPort serial;
        serial.setPort(info);                           //设置串口
        if(serial.open(QIODevice::ReadWrite))
        {
            ui->COMCBox->addItem(serial.portName());    //显示串口name
            serial.close();
        }
    }
}


void MainWindow::on_ConnectSerialButton_clicked()
{
    if(ui->COMCBox->currentText() == "")                //未选择可用的串口，则连接失败
    {
        qDebug() << "No serialport selected! Fail to connect!";
        return;
    }

    //TODO: 连接串口
    bSerialFlag = true;
    if(bSerialFlag == true)
    {
        ui->CloseSerialButton->setEnabled(true);
    }
}


void MainWindow::on_CloseSerialButton_clicked()
{
    //TODO: 关闭串口
    bSerialFlag = false;
    ui->CloseSerialButton->setEnabled(false);
}

