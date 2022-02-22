#ifndef POWERCHART_H
#define POWERCHART_H

#include <QDialog>
#include <QChart>
#include <QChartView>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QLineSeries>
#include <QTimer>

namespace Ui {
class PowerChart;
}

class PowerChart : public QDialog {
    Q_OBJECT

public:
    explicit PowerChart(QWidget *parent = nullptr);
    ~PowerChart();

private slots:
    void on_radioButtonShowGridInPwr_1_toggled(bool checked);
    void on_radioButtonGridChgPwr_1_toggled(bool checked);
    void on_radioButtonPvPwr_1_toggled(bool checked);
    void on_radioButtonBatCap_1_toggled(bool checked);
    void on_radioButtonOutPwr_1_toggled(bool checked);
    void on_radioButtonShowGridInPwr_2_toggled(bool checked);
    void on_radioButtonGridChgPwr_2_toggled(bool checked);
    void on_radioButtonPvPwr_2_toggled(bool checked);
    void on_radioButtonBatCap_2_toggled(bool checked);
    void on_radioButtonOutPwr_2_toggled(bool checked);

    void on_comboBoxFileName_currentTextChanged(const QString &arg1);

    void on_checkBoxFixedChart_toggled(bool checked);

private:
    typedef enum {
        GRAPH_MODE_IN_PWR = 0x01,
        GRAPH_MODE_IN_CHG = 0x02,
        GRAPH_MODE_PV = 0x04,
        GRAPH_MODE_BAT_CAP = 0x08,
        GRAPH_MODE_OUT_PWR = 0x10,
        GRAPH_MODE_INV_TEMP = 0x20,
        GRAPH_MODE_DCDC_TEMP = 0x40,
    }graphMode_e;

    QChart *chartPwr;
    QChartView *chartViewPwr;
    QDateTimeAxis *dateTimeAxisXPwr;
    QValueAxis *valueAxisY_InPwr;
    QValueAxis *valueAxisY_ChgPwr;
    QValueAxis *valueAxisY_PvPwr;
    QValueAxis *valueAxisY_BatCap;
    QValueAxis *valueAxisY_OutPwr;

    QChart *chartTemp;
    QChartView *chartViewTemp;
    QDateTimeAxis *dateTimeAxisXTemp;
    QValueAxis *valueAxisY_InvTemp;
    QValueAxis *valueAxisY_DcDcTemp;

    QTimer rfshTimer;
    int databaseWrites = -1;
    int filesSaved = -1;

    Ui::PowerChart *ui;
    void refreshChart();
    void refreshFiles();
public slots:
    void rfshTimerEvent();
};

#endif // POWERCHART_H
