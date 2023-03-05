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
#include <QThread>
#include <QtCore/QDateTime>
//#include <qmqtt.h>
#include <QtMqtt/qmqttclient.h>
//#include <QtMqtt/QtMqtt>

/******************************************初始化全局变量*********************************************/
bool isManu = true;                  //手动模式开关，默认手动模式
float fParaP = 1.0;                  //PID参数
float fParaI = 1.0;
float fParaD = 1.0;
int iParaV = 0;                      //初始流速
bool bSerialFlag = false;
bool bWirelessFlag = false;
int iAxisT = 0;
int iVelo;
double dVelo;
QByteArray baBuffer;
QString qstrSend;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    /******************************************初始化变量**********************************************/
    //SerialPort = new QSerialPort;
    chart = new QChart();
    chartView = new QChartView(chart);
    AxisXT = new QValueAxis();
    AxisYV = new QValueAxis();
    sSeriesV = new QSplineSeries();
    //curTime = QDateTime::currentDateTime();
    //strTime = curTime.toString("hh:mm:ss");
    ConnectSerialTimer = new QTimer();
    writetimer = new QTimer();
    painttimer = new QTimer();
    ASerialPort = new AsyncSerialPort();
    AMqtt = new AsyncMQTT();
    //qDebug() << "UI Thread: " << QThread::currentThread();
    /******************************************初始化控件*********************************************/
    ui->setupUi(this);

    ui->ManuRButton->setChecked(true);

    ui->PlineEdit->setText("1.0");
    ui->IlineEdit->setText("1.0");
    ui->DlineEdit->setText("1.0");
    ui->VlineEdit->setText("0");

    ui->CloseSerialButton->setEnabled(false);
    ui->WLDisconnectButton->setEnabled(false);

    //绘图区控件
    //时间轴
    AxisXT->setTitleText(tr("时间/s"));                                  // y轴显示标题
    QFont FontX_T("Times",12,2,false);                                 //设置YL轴字体样式
    AxisXT->setTitleFont(FontX_T);
    AxisXT->setRange(0, 2);                                          // 范围
    AxisXT->setTickCount(11);                                         // 轴上点的个数
    QFont FontX("Times",10,2,false);                                   //设置YL轴字体样式
    AxisXT->setLabelsFont(FontX);

    //流速轴
    AxisYV->setTitleText(tr("流速"));                                  // y轴显示标题
    QFont FontYL_T("Times",12,2,false);                                //设置YL轴字体样式
    AxisYV->setTitleFont(FontYL_T);
    AxisYV->setRange(0, 600);                                          // 范围
    AxisYV->setTickCount(11);                                         // 轴上点的个数
    QFont FontYL("Times",10,2,false);                                  //设置YL轴字体样式
    AxisYV->setLabelsFont(FontYL);
    AxisYV->setLabelsColor(Qt::blue);
    AxisYV->setGridLineVisible(true);                                 // 隐藏背景网格Y轴框线

    // 图表
    chart->setAnimationOptions(QChart::SeriesAnimations);              // 动画方式

    // 图表视图
    chartView->chart()->setTitle(tr("流速-时间 图，实时流速：NULL"));       // 设置标题
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
}

MainWindow::~MainWindow()
{
    delete ui;
    on_CloseButton_clicked();
}


void MainWindow::on_CloseButton_clicked()
{
    ASerialPort->~AsyncSerialPort();
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

    int iTempP, iTempI, iTempD;

    iTempP = fTempP*10;
    iTempI = fTempI*10;
    iTempD = fTempD*10;

    if(iTempP >= 0 && iTempI >= 0 && iTempD >= 0)  //数据合法，更新参数
    {
        fParaP = fTempP;
        fParaI = fTempI;
        fParaD = fTempD;
        //qDebug() << "P:" << fParaP << " I:" << fParaI << " D:" << fParaD;
        //TODO:串口发送参数
        qstrP = QString::number(iTempP);
        qstrI = QString::number(iTempI);
        qstrD = QString::number(iTempD);
        //qDebug() << qstrP << qstrI << qstrD;
        qstrSend = "PID P:";
        qstrSend.append(qstrP);
        qstrSend.append(" I:");
        qstrSend.append(qstrI);
        qstrSend.append(" D:");
        qstrSend.append(qstrD);
        //qDebug() << qstrSend.toLatin1();

        if(bSerialFlag == true)
        {
            emit Send_to_SerialPort(qstrSend);
        }
        else if(bWirelessFlag == true)
        {
            //QString PubTopic = ui->PubEdit->text();
            emit Send_to_MQTT(qstrSend);
        }
        else
        {
            QMessageBox::information(this,tr("写入失败"),tr("串口与无线均未连接到下位机！"),QMessageBox::Ok);
        }
    }
    else
    {
        QMessageBox::information(this,tr("写入失败"),tr("PID参数非法！"),QMessageBox::Ok);
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
        //qDebug() << "Velocity:" << iParaV;
        //TODO:串口发送参数
        qstrSend = "VELO ";
        qstrSend.append(qstrV);
        //qDebug() << qstrSend.toLatin1();

        if(bSerialFlag == true)
        {
            emit Send_to_SerialPort(qstrSend);
        }
        else if(bWirelessFlag == true)
        {
            emit Send_to_MQTT(qstrSend);
        }
        else
        {
            QMessageBox::information(this,tr("写入失败"),tr("串口与无线均未连接到下位机！"),QMessageBox::Ok);
        }
    }
    else
    {
        QMessageBox::information(this,tr("写入失败"),tr("速度数值非法！"),QMessageBox::Ok);
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
    QString curText = ui->COMCBox->currentText();                  //串口名
    int BaudRate = ui->BaudCBox->currentText().toInt();            //波特率
    QString EndBit = ui->EndCBox->currentText();                   //停止位
    int DataBit = ui->DataCBox->currentText().toInt();             //数据位
    QString CheckBit = ui->CheckCBox->currentText();               //校验位

    //SerialPort->close();
    ASerialPort->Init_Port(curText, BaudRate, EndBit, DataBit, CheckBit);

    bWirelessFlag = false;

    if(ASerialPort->IsConnected() == true)
    {
        bSerialFlag = true;
    }

    if(bSerialFlag == true)
    {
        qDebug()<< "SerialPort Connected";

        connect(ASerialPort, &AsyncSerialPort::Rec_Data, this, &MainWindow::SerialPort_Receive, Qt::QueuedConnection);
        connect(ASerialPort, &AsyncSerialPort::Rec_Data, this, &MainWindow::Paint_Data, Qt::QueuedConnection);
        connect(this, &MainWindow::Send_to_SerialPort, ASerialPort, &AsyncSerialPort::Write_Data, Qt::QueuedConnection);

        ui->CloseSerialButton->setEnabled(true);
        ui->ConnectSerialButton->setEnabled(false);

        ui->WLConnectButton->setEnabled(false);
    }
    else
    {
        QMessageBox::information(this,tr("串口通信错误"),tr("请检查连接与参数设置！"),QMessageBox::Ok);
        return;
    }
}


void MainWindow::on_CloseSerialButton_clicked()
{
    ASerialPort->Disconnect_Serial();

    disconnect(ASerialPort, &AsyncSerialPort::Rec_Data, this, &MainWindow::SerialPort_Receive);
    disconnect(ASerialPort, &AsyncSerialPort::Rec_Data, this, &MainWindow::Paint_Data);
    disconnect(this, &MainWindow::Send_to_SerialPort, ASerialPort, &AsyncSerialPort::Write_Data);

    bSerialFlag = false;
    ui->CloseSerialButton->setEnabled(false);
    ui->ConnectSerialButton->setEnabled(true);

    ui->WLConnectButton->setEnabled(true);
}


void MainWindow::sleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}


void MainWindow::SerialPort_Receive(QByteArray buffer)
{
    baBuffer.clear();
    baBuffer.append(buffer);
    //qDebug() << "main thread heard: " << baBuffer;
}


void MainWindow::on_ClearButton_clicked()
{
    sSeriesV->clear();
    iAxisT = 0;

    AxisXT->setRange(0, 2);
}

void MainWindow::Paint_Data()
{
    Sleep(15);
    QByteArray baStat, baVelo;
    if(bSerialFlag == true || bWirelessFlag == true)
    {
        baStat = baBuffer.mid(0,4);
        baVelo = baBuffer.mid(5,5);
        qDebug()<< baBuffer << baStat << baVelo;
        if(baStat == "VELO")
        {
            iVelo = baVelo.toInt();
            dVelo = iVelo/100.0;
            //qDebug() << "Plot" << iAxisT << ',' << iVelo;
            sSeriesV->append(iAxisT/5.0,dVelo);
            iAxisT++;
            if(iAxisT > 10)
            {
                AxisXT->setRange((iAxisT - 10)/5.0, iAxisT/5.0);
            }
            chartView->chart()->setTitle(tr("流速-时间 图，实时流速：")+QString::number(dVelo, 'f', 2));
        }
        else if(baStat == "CALL")
        {
            qstrSend = "ANSW";

            if(bWirelessFlag == false && bSerialFlag == true)  //串口回复
            {
                emit Send_to_SerialPort(qstrSend);
                qDebug() << "SerialPort Answer";
            }
        }
    }
    baBuffer.clear();
}

void MainWindow::on_WLConnectButton_clicked()
{
    QString HostName = ui->IPEdit->text();
    int MPort = ui->PortEdit->text().toInt();
    QString SubTopic = ui->SubEdit->text();
    QString PubTopic = ui->PubEdit->text();
    qDebug() << HostName << MPort << SubTopic << PubTopic;

    AMqtt->Init_MQTT(HostName, MPort, SubTopic, PubTopic);

    bSerialFlag = false;

    sleep(1000);
    if(AMqtt->IsConnected() == true)
    {
        bWirelessFlag = true;
    }

    if(bWirelessFlag == true)
    {
        connect(AMqtt, &AsyncMQTT::Rec_Data, this, &MainWindow::MQTT_Receive, Qt::QueuedConnection);
        connect(AMqtt, &AsyncMQTT::Rec_Data, this, &MainWindow::Paint_Data, Qt::QueuedConnection);
        connect(this, &MainWindow::Send_to_MQTT, AMqtt, &AsyncMQTT::Write_Data, Qt::QueuedConnection);

        ui->WLConnectButton->setEnabled(false);
        ui->WLDisconnectButton->setEnabled(true);

        ui->ConnectSerialButton->setEnabled(false);
    }
    else
    {
        QMessageBox::information(this,tr("MQTT通信错误"),tr("请检查连接与参数设置！"),QMessageBox::Ok);
        return;
    }
}


void MainWindow::on_WLDisconnectButton_clicked()
{
    bWirelessFlag = false;

    AMqtt->Disconnect_MQTT();

    disconnect(AMqtt, &AsyncMQTT::Rec_Data, this, &MainWindow::MQTT_Receive);
    disconnect(AMqtt, &AsyncMQTT::Rec_Data, this, &MainWindow::Paint_Data);
    disconnect(this, &MainWindow::Send_to_MQTT, AMqtt, &AsyncMQTT::Write_Data);

    ui->WLConnectButton->setEnabled(true);
    ui->WLDisconnectButton->setEnabled(false);

    ui->ConnectSerialButton->setEnabled(true);
}

void MainWindow::MQTT_Receive(QByteArray buffer)
{
    baBuffer.clear();
    baBuffer.append(buffer);
    //qDebug() << "main thread heard: " << baBuffer;
}

void MainWindow::on_ManuRButton_clicked()
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

    int iTempP, iTempI, iTempD;

    iTempP = fTempP*10;
    iTempI = fTempI*10;
    iTempD = fTempD*10;

    if(iTempP >= 0 && iTempI >= 0 && iTempD >= 0)  //数据合法，更新参数
    {
        fParaP = fTempP;
        fParaI = fTempI;
        fParaD = fTempD;
        //qDebug() << "P:" << fParaP << " I:" << fParaI << " D:" << fParaD;
        //TODO:串口发送参数
        qstrP = QString::number(iTempP);
        qstrI = QString::number(iTempI);
        qstrD = QString::number(iTempD);
        //qDebug() << qstrP << qstrI << qstrD;
        qstrSend = "PID P:";
        qstrSend.append(qstrP);
        qstrSend.append(" I:");
        qstrSend.append(qstrI);
        qstrSend.append(" D:");
        qstrSend.append(qstrD);

        if(bWirelessFlag == false && bSerialFlag == true)  //串口发送
        {
            emit Send_to_SerialPort(qstrSend);
            qDebug() << "SerialPort MANU";
        }
        else if(bSerialFlag == false && bWirelessFlag == true)    //MQTT发送
        {
            emit Send_to_MQTT(qstrSend);
            qDebug() << "MQTT MANU";
        }
        else
        {
            qDebug() << "No Connection";
        }

        isManu = true;
        ui->SetNewButton->setEnabled(isManu);
    }
    else
    {
        isManu = false;
        ui->ManuRButton->setChecked(false);
        ui->AutoRButton->setChecked(true);
        QMessageBox::information(this,tr("写入失败"),tr("PID参数非法！"),QMessageBox::Ok);
    }
}


void MainWindow::on_AutoRButton_clicked()
{
    isManu = false;
    ui->SetNewButton->setEnabled(isManu);

    qstrSend = "AUTO";

    if(bWirelessFlag == false && bSerialFlag == true)  //串口发送
    {
        emit Send_to_SerialPort(qstrSend);
        qDebug() << "SerialPort AUTO";
    }
    else if(bSerialFlag == false && bWirelessFlag == true)    //MQTT发送
    {
        emit Send_to_MQTT(qstrSend);
        qDebug() << "MQTT AUTO";
    }
    else
    {
        qDebug() << "No Connection";
    }
}

