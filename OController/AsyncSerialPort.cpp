#include "AsyncSerialPort.h"

extern QString qstrSend;
AsyncSerialPort::AsyncSerialPort(QObject *parent):QObject(parent)
{
    PortThread = new QThread();
    Port = new QSerialPort();

    //使用多线程读取串口，保证串口可以同时收发数据
    this->moveToThread(PortThread);
    Port->moveToThread(PortThread);

    PortThread->start();
}

AsyncSerialPort::~AsyncSerialPort()
{
    if(Port->isOpen())
    {
        Port->close();
        Port->clear();
    }

    PortThread->terminate();
}

void AsyncSerialPort::Init_Port(QString ComText, int BaudRate, QString EndBit, int DataBit, QString CheckBit)
{
    Port->setPortName(ComText);                                //设置串口名

    Port->setBaudRate(BaudRate);                               //设置波特率

    if(EndBit == "1")                                          //设置停止位
    {
        Port->setStopBits(QSerialPort::OneStop);
    }
    else if(EndBit == "2")
    {
        Port->setStopBits(QSerialPort::TwoStop);
    }
    else
    {
        Port->setStopBits(QSerialPort::OneAndHalfStop);
    }

    switch(DataBit)                                            //设置数据位
    {
        case 8:
            Port->setDataBits(QSerialPort::Data8);
            break;
        case 7:
            Port->setDataBits(QSerialPort::Data7);
            break;
        case 6:
            Port->setDataBits(QSerialPort::Data6);
            break;
        default:
            Port->setDataBits(QSerialPort::Data5);
    }

    if(CheckBit == "None")                                     //设置校验位
    {
        Port->setParity(QSerialPort::NoParity);
    }
    else if(CheckBit == "Odd")
    {
        Port->setParity(QSerialPort::OddParity);
    }
    else
    {
        Port->setParity(QSerialPort::EvenParity);
    }


    if (Port->open(QIODevice::ReadWrite))
    {
        qDebug() << "Connect SerialPort:" << Port->portName() << Port->baudRate()
                 << Port->dataBits() << Port->stopBits() << Port->parity();
    }
    connect(Port, SIGNAL(readyRead()), this, SLOT(Receive_Data()), Qt::QueuedConnection); //Qt::DirectConnection
}

void AsyncSerialPort::Disconnect_Serial()
{
    Port->close();
}

void AsyncSerialPort::Receive_Data()
{
    Sleep(10);
    QByteArray buffer = Port->readAll();
    //Available
    //qDebug() << QStringLiteral("Data received:") << buffer;
    //qDebug() << "Receive thread is:" << QThread::currentThreadId();
    emit Rec_Data(buffer);
}

void AsyncSerialPort::Write_Data(QString qstrSend)
{
    //qDebug() << "Write thread is:" << QThread::currentThreadId();
    //qDebug() << "Data send:" << qstrSend.toLatin1();
    Port->write(qstrSend.toLatin1());
}
