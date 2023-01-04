#ifndef ASYNCSERIALPORT_H
#define ASYNCSERIALPORT_H

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
#include <QThread>

class AsyncSerialPort: public QObject
{
    Q_OBJECT
    public:
      explicit AsyncSerialPort(QObject *parent = NULL);
      ~AsyncSerialPort();

      void Init_Port(QString ComText, int BaudRate, QString EndBit, int DataBit, QString CheckBit);  //初始化串口
      void Disconnect_Serial();

    public slots:
      void Receive_Data();  //处理接收到的数据
      void Write_Data(QString qstrSend);     //发送数据

    signals:
      //接收数据
      void Rec_Data(QByteArray buf);
      void Cfg_Port();

    public:
      QThread *PortThread;
      QSerialPort *Port;
};

#endif // ASYNCSERIALPORT_H
