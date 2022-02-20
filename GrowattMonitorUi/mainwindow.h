#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QtSerialPort/QSerialPort"
#include <QSerialPortInfo>
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

private:
    Ui::MainWindow *ui;
    QSerialPort serial;
    QTimer rfshTimer;
    QTimer saveTimer;
    QTimer rfshPortStatusTimer;
    QTimer serialTimeoutTimer;
    QTimer animationTimer;

    int deviceId = 1;
    int rfshTimerInterval = 500;
    int saveTimerInterval = 1000;

    int sentCommand = 3;
    int sentAddress = 0;
    QByteArray receiveArray;
    QByteArray holdingReg;
    QByteArray inputReg;
    QByteArray msg3_1;
    QByteArray msg3_2;
    QByteArray msg3_3;
    QByteArray msg4_1;
    QByteArray msg4_2;

    bool charging = false;
    bool acDirectionOut = false;
    bool chargingLast = false;
    bool dataReceived = false;

    int lastFault = 0;
    int lastWarning = 0;
    int animationCount = 0;
    double gridInputActivePowerDouble = 0.0;
    double pvChargePowerDouble = 0.0;
    double outputActivePowerDouble = 0.0;
    quint16 systemStatus = 0;

    double rssf( quint16 *array, int index, int scale = 10);
    double rsdf( quint16 *array, int index, int scale = 10);
    void parseData(QByteArray array1, QByteArray array2);
    void hideArrows();


public slots:
    void rfshPortStatusTimerEvent();
    void rfshTimerEvent();
    void saveTimerEvent();
    void serialTimeoutTimerEvent();
    void animationTimerEvent();
private slots:
    void serialData();
    void on_pushButtonApply_clicked();
    void on_actionAbout_triggered();
};
#endif // MAINWINDOW_H
