#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTimer>
#include <QWebEnginePage>
#include <QWebEngineView>

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

     void positionSet();

     void on_sliderRoll_valueChanged(int value);

     void on_sliderPitch_valueChanged(int value);

     void on_sliderYaw_valueChanged(int value);

     void on_sbLat_valueChanged(double arg1);

     void on_sbLon_valueChanged(double arg1);

     void on_sbAlt_valueChanged(int arg1);

     void on_cbObject_currentIndexChanged(int index);

     void serverStart();

     void on_lineEditSite_editingFinished();

     void orbitalPosition();

     void on_spinBoxRadius_valueChanged(int arg1);

     void on_sliderTheta_valueChanged(int value);

     void on_sliderPhi_valueChanged(int value);

     void on_lv_cam_itemSelectionChanged();

     void on_cb_connect_stateChanged(int arg1);

private:
    Ui::MainWindow *ui;
    QWebEngineView *web;
    QWebEnginePage *pg;
};
#endif // MAINWINDOW_H
