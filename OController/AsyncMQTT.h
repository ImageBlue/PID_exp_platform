#ifndef ASYNCMQTT_H
#define ASYNCMQTT_H

#include <QMainWindow>
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
#include <QtMqtt/qmqttclient.h>
#include <QtMqtt/qmqttsubscription.h>

class AsyncMQTT: public QObject
{
    Q_OBJECT
    public:
      explicit AsyncMQTT(QObject *parent = NULL);
      ~AsyncMQTT();
    void Init_MQTT(QString HostName, int Port, QString SubTopic, QString PubTopic);
    void Disconnect_MQTT();
    void Set_Pub_Topic(QString Tpc);
    bool IsConnected();

    public slots:
      void Receive_Data(const QMqttMessage &message);  //处理接收到的数据
      void Write_Data(QString qstrSend);     //发送数据

    public:
      QThread *MThread;
      QMqttClient *MClient;

      QString PubTopic;

    signals:
      //接收数据
      void Rec_Data(QByteArray buf);
      void Cfg_Port();
};

#endif // ASYNCMQTT_H
