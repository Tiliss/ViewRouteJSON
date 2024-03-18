#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QTcpSocket>
#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QWebEngineView>
#include <QTimer>
#include <QStringListModel>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

// Создаем объект QTcpSocket
QTcpSocket tcpSocket;
QTimer *timer = new QTimer();
// Устанавливаем адрес и порт сервера
QString serverAddress;
quint16 serverPort;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    web = new QWebEngineView();
    pg = new QWebEnginePage();
    web->setPage(pg);
    ui->gridLayout->addWidget(web);
    pg->setUrl(QUrl(ui->lineEditSite->text()));

    serverAddress = ui->lineEditIp->text();
    serverPort = (ui->lineEditPort->text()).toInt();
    // Добавление элементов в лист
    QStringList items;
    items << "Plane" << "Water" << "Mountains" << "Canyon" << "Snow";
    ui->listWidget->addItems(items);

    QStringList cam;
    cam << "Third Person" << "Orbital" << "FreeCam";
    ui->lv_cam->addItems(cam);

    QStringList obj;
    obj << "obj1" << "obj2";
    ui->cbObject->addItems(obj);

    QRegularExpression ipRegex("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    QRegularExpressionValidator *ipValidator = new QRegularExpressionValidator(ipRegex, this);
    ui->lineEditIp->setValidator(ipValidator);

    QIntValidator *portValidator = new QIntValidator(0, 65535, this); // диапазон портов от 0 до 65535
    ui->lineEditPort->setValidator(portValidator);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::serverStart()
{
    serverAddress = ui->lineEditIp->text();
    serverPort = (ui->lineEditPort->text()).toInt();
}

void MainWindow::on_sendPathBtn_clicked()
{
    QString jsonFilePath = "/home/user/Рабочий стол/3д/JSON/path.json";
    sendData(jsonFilePath);
    Connect();
}


void MainWindow::on_sendObjectBtn_clicked()
{
    QString jsonFilePath = "/home/user/Рабочий стол/3д/JSON/object.json";
    sendData(jsonFilePath);
    ui->btnStart->setEnabled(true);
    Connect();
}


void MainWindow::on_sendPosBtn_clicked()
{
    QString jsonFilePath = "/home/user/Рабочий стол/3д/JSON/position.json";
    sendData(jsonFilePath);
    Connect();
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
    Connect();
}

bool connected = false;
void MainWindow::on_cb_connect_clicked()
{
    serverStart();
    if (ui->cb_connect->isChecked()) {
        if (!connected) { // Если еще не подключены
            tcpSocket.connectToHost(serverAddress, serverPort);
            // Проверяем, удалось ли подключиться
            if (tcpSocket.waitForConnected()) {
                qDebug() << "Connected to server";
                connected = true; // Устанавливаем флаг успешного подключения
                ui->groupBox_2->setEnabled(true);
                ui->groupBox_5->setEnabled(true);
                ui->groupBox_3->setEnabled(true);
                ui->groupBox_4->setEnabled(true);
            } else {
                qDebug() << "Failed to connect to server";
                ui->cb_connect->setChecked(false); // Возвращаем флажок в исходное состояние
            }
        }
    } else {
        if (connected) { // Если подключены, отключаемся
            tcpSocket.disconnectFromHost();
            connected = false; // Сбрасываем флаг успешного подключения
            ui->groupBox_2->setEnabled(false);
            ui->groupBox_5->setEnabled(false);
            ui->groupBox_3->setEnabled(false);
            ui->groupBox_4->setEnabled(false);
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
        qDebug() << "Ошибка: тайм-аут ожидания ответа";
        ui->responcelb->setText("Ошибка: тайм-аут ожидания ответа");
    }
    Connect();
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
        Connect();
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
        Connect();
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
        Connect();
    }
}


void MainWindow::on_btnStart_clicked()
{
    ui->btnStop->setEnabled(true);
        ui->btnStart->setEnabled(false);
        int time = ui->spinBox->value();
        timer->start(time);
        // Подключаем к таймеру функцию-обработчик срабатывания таймера
        connect(timer, &QTimer::timeout, this, &MainWindow::sendPosition);
        Connect();
}


void MainWindow::on_btnStop_clicked()
{
    ui->btnStart->setEnabled(true);
    ui->btnStop->setEnabled(false);
    timer->stop();
    // Отключаем обработчик таймера
    disconnect(timer, &QTimer::timeout, this, &MainWindow::sendPosition);
    Connect();
}

double roll = 0;
void MainWindow::sendPosition()
{
    QString pos;
//    if(roll < 360)
//    {
//        pos = QString("{\"what\":\"update_positions\",\"positions\":[{\"id\":\"obj1\",\"lat\":125.98242506147994,\"lon\":29.66258160855277,\"alt\":200,\"roll\":%1,\"pitch\":-10,\"yaw\":180},{\"id\":\"obj2\",\"lat\":125.98242506147994,\"lon\":29.66258160855277,\"alt\":150,\"roll\":100,\"pitch\":10,\"yaw\":10}]}\0").arg(roll);
//        roll++;
//    }
//    else
//    {
//        roll = 0;
//    }
    pos = QString("{\"what\":\"update_positions\",\"positions\":[{\"id\":\"obj1\",\"lat\":%1,\"lon\":29.66258160855277,\"alt\":5000,\"roll\":0,\"pitch\":-10,\"yaw\":80},{\"id\":\"obj2\",\"lat\":125.98242506147994,\"lon\":29.66258160855277,\"alt\":150,\"roll\":100,\"pitch\":10,\"yaw\":10}]}\0").arg(roll);
    roll += 0.001;
    // Отправляем данные через tcpSocket
    tcpSocket.write(pos.toUtf8());
    qDebug() << "Данные отправлены";
    QByteArray responseData = tcpSocket.readAll();
    Connect();
}

void MainWindow::Connect()
{
    if (!connected){
         ui->cb_connect->setChecked(false);
    }
}

void MainWindow::positionSet()
{
    QJsonObject json;
    json["what"] = "update_positions";

    QJsonArray positionsArray;

    QJsonObject positionObject;
    positionObject["id"] = ui->cbObject->currentText();
    positionObject["lat"] = ui->sbLat->value();
    positionObject["lon"] = ui->sbLon->value();
    positionObject["alt"] = ui->sbAlt->value();
    positionObject["roll"] = ui->sliderRoll->value();
    positionObject["pitch"] = ui->sliderPitch->value();
    positionObject["yaw"] = ui->sliderYaw->value();

    positionsArray.append(positionObject);

    json["positions"] = positionsArray;

    QJsonDocument jsonDoc(json);
    QString jsonString = jsonDoc.toJson(QJsonDocument::Compact);

    qDebug() << jsonString;
    tcpSocket.write(jsonString.toUtf8());
    GetResponce();
    Connect();
}

void MainWindow::orbitalPosition()
{
    if(ui->lv_cam->currentItem() != nullptr && ui->lv_clients->currentItem() != nullptr) {
        QString selectedItemClients = ui->lv_clients->currentItem()->text();

        QJsonObject json;
        json["what"] = "OrbitalCam_position";
        json["client"] = selectedItemClients;
        json["radius"] = ui->spinBoxRadius->value();
        json["horizontalValue"] = ui->sliderTheta->value();
        json["verticalValue"] = ui->sliderPhi->value();

        QJsonDocument jsonDoc(json);
        QString jsonString = jsonDoc.toJson(QJsonDocument::Compact);
        tcpSocket.write(jsonString.toUtf8());
        GetResponce();
        Connect();
    }
}

void MainWindow::on_sliderRoll_valueChanged(int value)
{
    positionSet();
}


void MainWindow::on_sliderPitch_valueChanged(int value)
{
    positionSet();
}


void MainWindow::on_sliderYaw_valueChanged(int value)
{
    positionSet();
}


void MainWindow::on_sbLat_valueChanged(double arg1)
{
    positionSet();
}


void MainWindow::on_sbLon_valueChanged(double arg1)
{
    positionSet();
}


void MainWindow::on_sbAlt_valueChanged(int arg1)
{
    positionSet();
}


void MainWindow::on_cbObject_currentIndexChanged(int index)
{
    positionSet();
}


void MainWindow::on_lineEditSite_editingFinished()
{
    pg->setUrl(QUrl(ui->lineEditSite->text()));
}

void MainWindow::on_spinBoxRadius_valueChanged(int arg1)
{
    orbitalPosition();
}



void MainWindow::on_sliderTheta_valueChanged(int value)
{
    orbitalPosition();
}


void MainWindow::on_sliderPhi_valueChanged(int value)
{
    orbitalPosition();
}


void MainWindow::on_lv_cam_itemSelectionChanged()
{
    if(ui->lv_cam->currentItem()->text() == "Orbital")
    {
        ui->sliderPhi->setEnabled(true);
        ui->sliderTheta->setEnabled(true);
        ui->spinBoxRadius->setEnabled(true);
    }
    else
    {
        ui->sliderPhi->setEnabled(false);
        ui->sliderTheta->setEnabled(false);
        ui->spinBoxRadius->setEnabled(false);
    }
}


void MainWindow::on_cb_connect_stateChanged(int arg1)
{

}

