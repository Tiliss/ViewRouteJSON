#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
     void on_openBtn_clicked();

     void on_closeBtn_clicked();

     void on_sendPathBtn_clicked();

     void on_sendObjectBtn_clicked();

     void on_sendPosBtn_clicked();

     void sendData(QString(jsonFilePath));

     void onConnected(); // Слот для обработки успешного подключения

     void onError(QAbstractSocket::SocketError socketError); // Слот для обработки ошибок подключения
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
