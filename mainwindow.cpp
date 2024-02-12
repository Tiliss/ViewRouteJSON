#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTcpSocket>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QWebEngineView>
#include <QTimer>
#include <QStringListModel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QWebEngineView *web = new QWebEngineView();
    QWebEnginePage *pg = new QWebEnginePage();
    pg->setUrl(QUrl("http://localhost:3000/"));
    web->setPage(pg);
    ui->gridLayout->addWidget(web);
    //auto lay= ui->gridLayout_3->layout();
    //lay->addWidget(web, 0, 1);
    //ui->quickWidget->setSource(QUrl("qrc:/web.qml"));

    // Добавление элементов в лист
    QStringList items;
    items << "Plane" << "Water" << "Mountains" << "Canyon" << "Snow";
    ui->listWidget->addItems(items);

    QStringList cam;
    cam << "Third Person" << "Orbital" << "FreeCam";
    ui->lv_cam->addItems(cam);
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

void MainWindow::on_sendPathBtn_clicked()
{
    QString jsonFilePath = "/home/user/Рабочий стол/3д/path.json";
    sendData(jsonFilePath);
}


void MainWindow::on_sendObjectBtn_clicked()
{
    QString jsonFilePath = "/home/user/Рабочий стол/3д/object.json";
    sendData(jsonFilePath);
//    QString obj = "{\"what\":\"update_objects\",\"objects\":[{\"id\":\"obj1\",\"type\":\"Plane\",\"isRemove\":false},{\"id\":\"obj2\",\"type\":\"Plane\",\"isRemove\":false}]}";
//    //QString pos = "{\"what\":\"update_positions\",\"positions\":[{\"id\":\"obj1\",\"lat\":125.98242506147994,\"lon\":29.66258160855277,\"alt\":200,\"roll\":30,\"pitch\":-10,\"yaw\":180},{\"id\":\"obj2\",\"lat\":125.98242506147994,\"lon\":29.66258160855277,\"alt\":150,\"roll\":100,\"pitch\":10,\"yaw\":10}]}";

//    tcpSocket.write(obj.toUtf8());

//    // Создаем таймер
//        QTimer *timer = new QTimer(this);

//        // Запускаем таймер с интервалом 1000 миллисекунд (1 секунда)
//        timer->start(6);
//        int roll = 0;
//        // Подключаем к таймеру функцию-обработчик срабатывания таймера
//        connect(timer, &QTimer::timeout, [=]() mutable {
//            QString pos;
//            if(roll < 360)
//            {
//                pos = QString("{\"what\":\"update_positions\",\"positions\":[{\"id\":\"obj1\",\"lat\":125.98242506147994,\"lon\":29.66258160855277,\"alt\":200,\"roll\":%1,\"pitch\":-10,\"yaw\":180},{\"id\":\"obj2\",\"lat\":125.98242506147994,\"lon\":29.66258160855277,\"alt\":150,\"roll\":100,\"pitch\":10,\"yaw\":10}]}").arg(roll);
//                roll++;
//            }
//            else
//            {
//                roll = 0;
//            }
//            // Отправляем данные через tcpSocket
//            tcpSocket.write(pos.toUtf8());
//            qDebug() << "Данные отправлены";
//        });
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

    // Отправляем данные на сервер
    tcpSocket.write(jsonData);

    GetResponce();
}

bool connected = false;
void MainWindow::on_cb_connect_clicked()
{
    if (ui->cb_connect->isChecked()) {
        if (!connected) { // Если еще не подключены
            tcpSocket.connectToHost(serverAddress, serverPort);
            // Проверяем, удалось ли подключиться
            if (tcpSocket.waitForConnected()) {
                qDebug() << "Connected to server";
                connected = true; // Устанавливаем флаг успешного подключения
            } else {
                qDebug() << "Failed to connect to server";
                ui->cb_connect->setChecked(false); // Возвращаем флажок в исходное состояние
            }
        }
    } else {
        if (connected) { // Если подключены, отключаемся
            tcpSocket.disconnectFromHost();
            connected = false; // Сбрасываем флаг успешного подключения
        }
    }
}

void MainWindow::GetResponce() {
    // Ждем ответ от сервера
    if (tcpSocket.waitForReadyRead()) {
        QByteArray responseData = tcpSocket.readAll();
        QString responseString(responseData);
        qDebug() << "Получен ответ от сервера: " << responseString;
        ui->responcelb->setText("Получен ответ от сервера: " + responseString);
    } else {
        qDebug() << "Error: Timeout waiting for response";
        ui->responcelb->setText("Ошибка: тайм-аут ожидания ответа");
    }
}

void MainWindow::on_btn_map_clicked()
{
    if(ui->listWidget->currentItem() != nullptr) {
        QString selectedItem = ui->listWidget->currentItem()->text();

        QJsonObject json;
        json["what"] = "update_map";
        json["name"] = selectedItem;

        QJsonDocument jsonDoc(json);
        QString jsonString = jsonDoc.toJson(QJsonDocument::Compact);

        tcpSocket.write(jsonString.toUtf8());
        GetResponce();
    }

}


void MainWindow::on_btn_GetClients_clicked()
{
    QJsonObject json;
    json["what"] = "get_clientsID";

    QJsonDocument jsonDoc(json);
    QString jsonString = jsonDoc.toJson(QJsonDocument::Compact);

    tcpSocket.write(jsonString.toUtf8());

    if (tcpSocket.waitForReadyRead()) {
        QByteArray responseData = tcpSocket.readAll();
        QString responseString(responseData);
        qDebug() << responseData;
        QStringList myList = responseString.split(";");
        ui->lv_clients->clear();
        ui->lv_clients->addItems(myList);
    }
}


void MainWindow::on_btn_camChange_clicked()
{
    if(ui->lv_cam->currentItem() != nullptr && ui->lv_clients->currentItem() != nullptr) {
        QString selectedItemCam = ui->lv_cam->currentItem()->text();
        QString selectedItemClients = ui->lv_clients->currentItem()->text();

        QJsonObject json;
        json["what"] = "update_cam";
        json["name"] = selectedItemCam;
        json["client"] = selectedItemClients;

        QJsonDocument jsonDoc(json);
        QString jsonString = jsonDoc.toJson(QJsonDocument::Compact);

        tcpSocket.write(jsonString.toUtf8());
        GetResponce();
    }
}

