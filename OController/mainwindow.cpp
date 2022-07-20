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
    sleep(100);                                         //延时100ms
    if(ui->COMCBox->currentText() == "")                //未选择可用的串口，则连接失败
    {
        QMessageBox::information(this,tr("Error"),tr("No serialport is available!"),QMessageBox::Ok);
        return;
    }

    SerialPort->setPortName(ui->COMCBox->currentText());
    if(SerialPort->open(QIODevice::ReadWrite))
    {
        SerialPort->setBaudRate(ui->BaudCBox->currentText().toInt());  //设置波特率

        QString EndBit = ui->EndCBox->currentText();                   //设置停止位
        if(EndBit == "1")
        {
            SerialPort->setStopBits(QSerialPort::OneStop);
        }
        else if(EndBit == "2")
        {
            SerialPort->setStopBits(QSerialPort::TwoStop);
        }
        else
        {
            SerialPort->setStopBits(QSerialPort::OneAndHalfStop);
        }

        int DataBit = ui->DataCBox->currentText().toInt();
        switch(DataBit)                                                //设置数据位
        {
            case 8:
                SerialPort->setDataBits(QSerialPort::Data8);
                break;
            case 7:
                SerialPort->setDataBits(QSerialPort::Data7);
                break;
            case 6:
                SerialPort->setDataBits(QSerialPort::Data6);
                break;
            default:
                SerialPort->setDataBits(QSerialPort::Data5);
        }

        QString CheckBit = ui->CheckCBox->currentText();               //设置校验位
        if(CheckBit == "None")
        {
            SerialPort->setParity(QSerialPort::NoParity);
        }
        else if(CheckBit == "Odd")
        {
            SerialPort->setParity(QSerialPort::OddParity);
        }
        else
        {
            SerialPort->setParity(QSerialPort::EvenParity);
        }

        SerialPort->setFlowControl(QSerialPort::NoFlowControl);         //设置流控制
    }
    //TODO: 完成 QObject::connect(SerialPort, &QSerialPort::readyRead, this, &MainWindow::Read_Date); 语句
    bSerialFlag = true;
    if(bSerialFlag == true)
    {
        ui->CloseSerialButton->setEnabled(true);
        ui->ConnectSerialButton->setEnabled(false);
        qDebug() << "Connect SerialPort:" << SerialPort->portName() << SerialPort->baudRate()
                 << SerialPort->dataBits() << SerialPort->stopBits() << SerialPort->parity();
    }
}


void MainWindow::on_CloseSerialButton_clicked()
{
    SerialPort->clear();                        //清空缓存区
    SerialPort->close();                        //关闭串口

    bSerialFlag = false;
    ui->CloseSerialButton->setEnabled(false);
    ui->ConnectSerialButton->setEnabled(true);
}


void MainWindow::sleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

