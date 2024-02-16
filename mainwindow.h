#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>

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

     void on_sendPathBtn_clicked();

     void on_sendObjectBtn_clicked();

     void on_sendPosBtn_clicked();

     void sendData(QString(jsonFilePath));
     void GetResponce();

     void on_cb_connect_clicked();
     void on_btn_map_clicked();

     void on_btn_GetClients_clicked();

     void on_btn_camChange_clicked();

     void on_btnStart_clicked();

     void on_btnStop_clicked();

     void sendPosition();

     void Connect();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
