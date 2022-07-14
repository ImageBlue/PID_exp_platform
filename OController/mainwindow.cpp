#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QBitArray>
#include <QMessageBox>
#include <QByteArray>

bool isManu = true;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setFixedSize(750,500);

    ui->tabWidget->setFixedSize(180,460);

    ui->ManuRButton->setChecked(true);
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

