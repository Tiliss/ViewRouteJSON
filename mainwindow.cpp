#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTcpSocket>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QWebEngineView>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    QWebEngineView *web = new QWebEngineView();
//    QWebEnginePage *pg = new QWebEnginePage();
//    pg->setUrl(QUrl("http://localhost:3000/"));
//    web->setPage(pg);
//    auto lay= (QGridLayout*)ui->centralwidget->layout();
//    lay->addWidget(web);
    //ui->quickWidget->setSource(QUrl("qrc:/web.qml"));
}

// Создаем объект QTcpSocket
QTcpSocket tcpSocket;

// Устанавливаем адрес и порт сервера
QString serverAddress = "127.0.0.1";
quint16 serverPort = 8080;

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onConnected() {
    ui->serverlb->setText("Connected to server");
}

void MainWindow::onError(QAbstractSocket::SocketError socketError) {
    ui->serverlb->setText("Error: " + tcpSocket.errorString());
}


void MainWindow::on_openBtn_clicked()
{
    // Подключаем сигналы к слотам
    connect(&tcpSocket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(&tcpSocket, SIGNAL(errorOccurred(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));

    // Подключаемся к серверу
    tcpSocket.connectToHost(serverAddress, serverPort);
}


void MainWindow::on_closeBtn_clicked()
{
    // Закрываем соединение
    tcpSocket.disconnectFromHost();
    ui->serverlb->setText("Server disconnect");
}


void MainWindow::on_sendPathBtn_clicked()
{
    QString jsonFilePath = "/home/user/Рабочий стол/3д/path.json";
    sendData(jsonFilePath);
}


void MainWindow::on_sendObjectBtn_clicked()
{
    QString jsonFilePath = "/home/user/Рабочий стол/3д/object.json";
    sendData(jsonFilePath);
}


void MainWindow::on_sendPosBtn_clicked()
{
    QString jsonFilePath = "/home/user/Рабочий стол/3д/position.json";
    sendData(jsonFilePath);
}

void MainWindow::sendData(QString(jsonFilePath))
{
    QFile file(jsonFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open file:" << jsonFilePath;
    }

    QByteArray jsonData = file.readAll();
    file.close();

if (tcpSocket.waitForConnected()) {
    qDebug() << "Connected to server";

// Отправляем данные на сервер
   tcpSocket.write(jsonData);

     // Ждем ответ от сервера
                    if (tcpSocket.waitForReadyRead()) {
                        QByteArray responseData = tcpSocket.readAll();
                        QString responseString(responseData);
                        qDebug() << "Received response from server:" << responseString;
                        ui->responcelb->setText("Received response from server: " + responseString);
                    } else {
                        qDebug() << "Error: Timeout waiting for response";
                        ui->responcelb->setText("Error: Timeout waiting for response");
                    }
                } else {
                    qDebug() << "Error: Unable to connect to the server";
                    ui->responcelb->setText("Error: Unable to connect to the server");
                }
}
