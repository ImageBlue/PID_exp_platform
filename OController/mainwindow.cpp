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
int iAxisT = 0;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    /******************************************初始化变量**********************************************/
    SerialPort = new QSerialPort;
    chart = new QChart();
    chartView = new QChartView(chart);
    AxisXT = new QValueAxis();
    AxisYV = new QValueAxis();
    sSeriesV = new QSplineSeries();
    curTime = QDateTime::currentDateTime();
    strTime = curTime.toString("hh:mm:ss");
    /******************************************初始化控件*********************************************/
    ui->setupUi(this);

    ui->ManuRButton->setChecked(true);

    ui->PlineEdit->setText("1.0");
    ui->IlineEdit->setText("1.0");
    ui->DlineEdit->setText("1.0");
    ui->VlineEdit->setText("0");

    ui->CloseSerialButton->setEnabled(false);

    //绘图区控件
    //时间轴
    AxisXT->setTitleText(tr("时间/s"));                                  // y轴显示标题
    QFont FontX_T("Times",12,2,false);                                 //设置YL轴字体样式
    AxisXT->setTitleFont(FontX_T);
    AxisXT->setRange(0, 10);                                          // 范围
    AxisXT->setTickCount(11);                                         // 轴上点的个数
    QFont FontX("Times",10,2,false);                                   //设置YL轴字体样式
    AxisXT->setLabelsFont(FontX);

    //流速轴
    AxisYV->setTitleText(tr("流速"));                                  // y轴显示标题
    QFont FontYL_T("Times",12,2,false);                                //设置YL轴字体样式
    AxisYV->setTitleFont(FontYL_T);
    AxisYV->setRange(0, 100);                                          // 范围
    AxisYV->setTickCount(11);                                         // 轴上点的个数
    QFont FontYL("Times",10,2,false);                                  //设置YL轴字体样式
    AxisYV->setLabelsFont(FontYL);
    AxisYV->setLabelsColor(Qt::blue);
    AxisYV->setGridLineVisible(true);                                 // 隐藏背景网格Y轴框线

    // 图表
    chart->setAnimationOptions(QChart::SeriesAnimations);              // 动画方式

    // 图表视图
    chartView->chart()->setTitle(strTime+tr("实时 流速-时间 图"));       // 设置标题
    chartView->setRenderHint(QPainter::Antialiasing);                  // 反锯齿绘制
    chartView->chart()->addSeries(sSeriesV);                           // 添加线段
    chartView->chart()->setTheme(QChart::ChartThemeHighContrast);      // 设置主题
    QFont Font_Title("Times",11,2,false);
    chartView->chart()->setTitleFont(Font_Title);
    chartView->chart()->addAxis(AxisXT, Qt::AlignBottom);              // 设置X轴位置
    chartView->chart()->addAxis(AxisYV, Qt::AlignLeft);               // 设置YV轴位置
    // 流速数据线段
    sSeriesV->setPen(QPen(Qt::blue, 2, Qt::SolidLine));                 // 设置线段pen
    sSeriesV->attachAxis(AxisXT);                                      // 线段依附的X轴
    sSeriesV->attachAxis(AxisYV);                                     // 线段依附的YT轴
    sSeriesV->setName(tr("流速"));                                      // 线段名称，在图例会显示
    sSeriesV->setPointsVisible(true);

    // 图例
    chart->legend()->setVisible(true);                                  // 图例显示
    chart->legend()->setAlignment(Qt::AlignTop);                        // 图例向下居中

    // 将图表扔进QWidget
    ui->graphicsView->setChart(chart);                                  // 将图表对象设置到graphicsView上进行显示
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);            // 设置渲染：抗锯齿，如果不设置那么曲线就显得不平滑


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
    SerialPort->close();                  //养成好习惯，退出前关闭串口
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
    /******************************************
    if(ui->COMCBox->currentText() == "")                //未选择可用的串口，则连接失败
    {
        QMessageBox::information(this,tr("串口通信错误"),tr("无可用串口！"),QMessageBox::Ok);
        return;
    }
    ******************************************/
    SerialPort->setPortName(ui->COMCBox->currentText());               //设置串口名
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

        bSerialFlag = true;
    }

    connect(SerialPort, &QSerialPort::readyRead, this, &MainWindow::Paint_Data);

    if(bSerialFlag == true)
    {
        ui->CloseSerialButton->setEnabled(true);
        ui->ConnectSerialButton->setEnabled(false);
        qDebug() << "Connect SerialPort:" << SerialPort->portName() << SerialPort->baudRate()
                 << SerialPort->dataBits() << SerialPort->stopBits() << SerialPort->parity();
    }
    else
    {
        QMessageBox::information(this,tr("串口通信错误"),tr("请检查连接与参数设置！"),QMessageBox::Ok);
        return;
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


void MainWindow::on_ClearButton_clicked()
{
    sSeriesV->clear();
    iAxisT = 0;

    curTime = QDateTime::currentDateTime();
    strTime = curTime.toString("hh:mm:ss");
    chartView->chart()->setTitle(strTime+tr("实时 流速-时间 图"));
}

void MainWindow::Paint_Data()
{
    QByteArray baBuffer;
    baBuffer = SerialPort->readAll();
    qDebug() << baBuffer;
}

