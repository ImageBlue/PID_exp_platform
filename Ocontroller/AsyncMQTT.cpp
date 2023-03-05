#include "AsyncMQTT.h"

AsyncMQTT::AsyncMQTT(QObject *parent):QObject(parent)
{
    MThread = new QThread();
    MClient = new QMqttClient(this);

    isFirst = false;

    //使用多线程读取串口，保证串口可以同时收发数据
    this->moveToThread(MThread);
    MClient->moveToThread(MThread);

    MThread->start();

    connect(MClient, &QMqttClient::connected, this, &AsyncMQTT::Client_Connected, Qt::QueuedConnection);
}

AsyncMQTT::~AsyncMQTT()
{
    if(MClient->state() == QMqttClient::Connected)
    {
        MClient->disconnectFromHost();
    }
    MThread->terminate();
}

void AsyncMQTT::Init_MQTT(QString HostName, int Port, QString SubTopic, QString PubTopic)
{
    //qDebug() << "MThread is" << QThread::currentThreadId();
    if(isFirst == false)
    {
        MClient->setHostname(HostName);
        MClient->setPort(Port);

        Set_Sub_Topic(SubTopic);
        Set_Pub_Topic(PubTopic);
    }
    MClient->connectToHost();
}

void AsyncMQTT::Disconnect_MQTT()
{
    MClient->disconnectFromHost();
}

void AsyncMQTT::Client_Connected()
{
    auto Subscription = MClient->subscribe(SubTopic);
    if(!Subscription)
    {
        qDebug() << "Receiver topic suscribe Error";
    }

    //auto Publication = MClient->subscribe(PubTopic);
    //if(!Publication)
    //{
    //    qDebug() << "Writer topic suscribe Error";
    //}

    connect(Subscription, &QMqttSubscription::messageReceived, this, &AsyncMQTT::Receive_Data, Qt::QueuedConnection);
}

void AsyncMQTT::Receive_Data(const QMqttMessage &message)
{
    QByteArray buffer = message.payload();
    emit Rec_Data(buffer);
}

void AsyncMQTT::Set_Sub_Topic(QString &Topic)
{
    SubTopic = Topic;
}

void AsyncMQTT::Set_Pub_Topic(QString &Topic)
{
    PubTopic = Topic;
}

void AsyncMQTT::Write_Data(QString qstrSend)
{
    MClient->publish(PubTopic, qstrSend.toUtf8());
}

bool AsyncMQTT::IsConnected()
{
    if(MClient->state() == QMqttClient::Connected)
    {
        return true;
    }
    else
    {
        return false;
    }
}
