#include "powerchart.h"
#include "ui_powerchart.h"

#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QLineSeries>
#include <QDateTime>
#include <QMessageBox>

#include "static.h"

PowerChart::PowerChart(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PowerChart) {
    ui->setupUi(this);

    dateTimeAxisXPwr = new QDateTimeAxis();
    dateTimeAxisXTemp = new QDateTimeAxis();

    chartPwr = new QChart();
    chartViewPwr = new QChartView(this);
    chartViewPwr->setChart(chartPwr);
    chartViewPwr->setRenderHint(QPainter::Antialiasing);
    chartViewPwr->setGeometry(170, 5, 842, 430);

    chartTemp = new QChart();
    chartViewTemp = new QChartView(this);
    chartViewTemp->setChart(chartTemp);
    chartViewTemp->setRenderHint(QPainter::Antialiasing);
    chartViewTemp->setGeometry(170, 435, 842, 200);


    chartPwr->addAxis(dateTimeAxisXPwr, Qt::AlignBottom);
    dateTimeAxisXPwr->setTickCount(13);
    dateTimeAxisXPwr->setFormat("hh:mm:ss");

    valueAxisY_InPwr = new QValueAxis();
    valueAxisY_ChgPwr = new QValueAxis();
    valueAxisY_PvPwr = new QValueAxis();
    valueAxisY_BatCap = new QValueAxis();
    valueAxisY_OutPwr = new QValueAxis();

    valueAxisY_InPwr->setLabelFormat("%0.1fW");
    valueAxisY_ChgPwr->setLabelFormat("%0.1fW");
    valueAxisY_PvPwr->setLabelFormat("%0.1fW");
    valueAxisY_BatCap->setLabelFormat("%0.1f%");
    valueAxisY_OutPwr->setLabelFormat("%0.1fW");

    valueAxisY_InPwr->setTickCount(11);
    valueAxisY_ChgPwr->setTickCount(11);
    valueAxisY_PvPwr->setTickCount(11);
    valueAxisY_BatCap->setTickCount(11);
    valueAxisY_OutPwr->setTickCount(11);

    chartTemp->addAxis(dateTimeAxisXTemp, Qt::AlignBottom);
    dateTimeAxisXTemp->setTickCount(13);
    dateTimeAxisXTemp->setFormat("hh:mm:ss");

    valueAxisY_InvTemp = new QValueAxis();
    valueAxisY_DcDcTemp = new QValueAxis();

    valueAxisY_InvTemp->setLabelFormat("%0.1fC");
    valueAxisY_DcDcTemp->setLabelFormat("%0.1fC");

    valueAxisY_InvTemp->setTickCount(4);
    valueAxisY_DcDcTemp->setTickCount(4);

    ui->radioButtonPvPwr_2->setEnabled(false);
    ui->radioButtonOutPwr_1->setEnabled(false);

    connect(&rfshTimer, SIGNAL(timeout()), this, SLOT(rfshTimerEvent()));
    rfshTimer.start(1000);

    refreshFiles();
    refreshChart();
}

PowerChart::~PowerChart() {
    delete ui;
}

void PowerChart::refreshChart() {
    chartPwr->removeAllSeries();
    chartPwr->removeAxis(valueAxisY_InPwr);
    chartPwr->removeAxis(valueAxisY_ChgPwr);
    chartPwr->removeAxis(valueAxisY_PvPwr);
    chartPwr->removeAxis(valueAxisY_BatCap);
    chartPwr->removeAxis(valueAxisY_OutPwr);

    QLineSeries *series_InPwr = new QLineSeries(this);
    QLineSeries *series_ChgPwr = new QLineSeries(this);
    QLineSeries *series_PvPwr = new QLineSeries(this);
    QLineSeries *series_BatCap = new QLineSeries(this);
    QLineSeries *series_OutPwr = new QLineSeries(this);

    series_InPwr->setName("Grid Input Power");
    series_ChgPwr->setName("Grid Charge Power");
    series_PvPwr->setName("PV Input Power");
    series_BatCap->setName("Battery Capacity");
    series_OutPwr->setName("Output Power");


    if(ui->radioButtonShowGridInPwr_1->isChecked() || ui->radioButtonShowGridInPwr_1->isChecked())
        chartPwr->addSeries(series_InPwr);
    if(ui->radioButtonGridChgPwr_1->isChecked() || ui->radioButtonGridChgPwr_2->isChecked())
        chartPwr->addSeries(series_ChgPwr);
    if(ui->radioButtonPvPwr_1->isChecked() || ui->radioButtonPvPwr_2->isChecked())
        chartPwr->addSeries(series_PvPwr);
    if(ui->radioButtonBatCap_1->isChecked() || ui->radioButtonBatCap_2->isChecked())
        chartPwr->addSeries(series_BatCap);
    if(ui->radioButtonOutPwr_1->isChecked() || ui->radioButtonOutPwr_2->isChecked())
        chartPwr->addSeries(series_OutPwr);

    //dateTimeAxisX->setRange(QDateTime().currentDateTime().date().startOfDay().addDays(-10), QDateTime().currentDateTime().date().endOfDay().addSecs(1).addDays(10));

    if(ui->radioButtonShowGridInPwr_1->isChecked())
        chartPwr->addAxis(valueAxisY_InPwr, Qt::AlignLeft);
    if(ui->radioButtonShowGridInPwr_2->isChecked())
        chartPwr->addAxis(valueAxisY_InPwr, Qt::AlignRight);

    if(ui->radioButtonGridChgPwr_1->isChecked())
        chartPwr->addAxis(valueAxisY_ChgPwr, Qt::AlignLeft);
    if(ui->radioButtonGridChgPwr_2->isChecked())
        chartPwr->addAxis(valueAxisY_ChgPwr, Qt::AlignRight);

    if(ui->radioButtonPvPwr_1->isChecked())
        chartPwr->addAxis(valueAxisY_PvPwr, Qt::AlignLeft);
    if(ui->radioButtonPvPwr_2->isChecked())
        chartPwr->addAxis(valueAxisY_PvPwr, Qt::AlignRight);

    if(ui->radioButtonBatCap_1->isChecked())
        chartPwr->addAxis(valueAxisY_BatCap, Qt::AlignLeft);
    if(ui->radioButtonBatCap_2->isChecked())
        chartPwr->addAxis(valueAxisY_BatCap, Qt::AlignRight);

    if(ui->radioButtonOutPwr_1->isChecked())
        chartPwr->addAxis(valueAxisY_OutPwr, Qt::AlignLeft);
    if(ui->radioButtonOutPwr_2->isChecked())
        chartPwr->addAxis(valueAxisY_OutPwr, Qt::AlignRight);

    series_InPwr->attachAxis(dateTimeAxisXPwr);
    series_InPwr->attachAxis(valueAxisY_InPwr);
    series_ChgPwr->attachAxis(dateTimeAxisXPwr);
    series_ChgPwr->attachAxis(valueAxisY_ChgPwr);
    series_PvPwr->attachAxis(dateTimeAxisXPwr);
    series_PvPwr->attachAxis(valueAxisY_PvPwr);
    series_BatCap->attachAxis(dateTimeAxisXPwr);
    series_BatCap->attachAxis(valueAxisY_BatCap);
    series_OutPwr->attachAxis(dateTimeAxisXPwr);
    series_OutPwr->attachAxis(valueAxisY_OutPwr);


    valueAxisY_InPwr->setLinePenColor(series_InPwr->pen().color());
    valueAxisY_ChgPwr->setLinePenColor(series_ChgPwr->pen().color());
    valueAxisY_PvPwr->setLinePenColor(series_PvPwr->pen().color());
    valueAxisY_BatCap->setLinePenColor(series_BatCap->pen().color());
    valueAxisY_OutPwr->setLinePenColor(series_OutPwr->pen().color());


    chartTemp->removeAllSeries();
    chartTemp->removeAxis(valueAxisY_InvTemp);
    chartTemp->removeAxis(valueAxisY_DcDcTemp);

    QLineSeries *series_InvTemp = new QLineSeries(this);
    QLineSeries *series_DcDcTemp = new QLineSeries(this);

    series_InvTemp->setName("DC-AC Inverter Temperature");
    series_DcDcTemp->setName("DC-DC Inverter Temperature");

    chartTemp->addSeries(series_InvTemp);
    chartTemp->addSeries(series_DcDcTemp);

    chartTemp->addAxis(valueAxisY_InvTemp, Qt::AlignLeft);
    chartTemp->addAxis(valueAxisY_DcDcTemp, Qt::AlignRight);

    series_InvTemp->attachAxis(dateTimeAxisXTemp);
    series_InvTemp->attachAxis(valueAxisY_InvTemp);
    series_DcDcTemp->attachAxis(dateTimeAxisXTemp);
    series_DcDcTemp->attachAxis(valueAxisY_DcDcTemp);

    valueAxisY_InvTemp->setLinePenColor(series_InvTemp->pen().color());
    valueAxisY_DcDcTemp->setLinePenColor(series_DcDcTemp->pen().color());

    //QList<QAbstractAxis *> abstract = chartPwr->axes(Qt::Vertical);
    //abstract.first()->setRange(0, 6000);

    QString val = "[]";
    QFile file;
    file.setFileName(Static::getDatabasePath() + ui->comboBoxFileName->currentText());
    if(file.exists()) {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        val = file.readAll();
        file.close();
    }
    QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
    QJsonArray arr = d.array();
    qlonglong timeStampStart = 0;
    qlonglong timeStamp = 0;
    double gridInputPower = 0.0;
    double gridChargePower = 0.0;
    double pvPower = 0.0;
    double batteryCapacity = 0.0;
    double outputPower = 0.0;
    double inverterTemperature = 0.0;
    double dcDcTemperature = 0.0;
    QString state;
    QString error;
    QString warning;
    double maximumGridInputPower = 5.0;
    double maximumGridChargePower = 5.0;
    double maximumPvPower = 5.0;
    double maximumBatteryCapacity = 1.0;
    double maximumOutputPower = 5.0;
    double maximumInverterTemperature = 0.0;
    double minimumInverterTemperature = 100.0;
    double maximumDcDcTemperature = 0.0;
    double minimumDcDcTemperature = 100.0;
    QList<QPointF> pointList_InPwr;
    QList<QPointF> pointList_ChgPwr;
    QList<QPointF> pointList_PvPwr;
    QList<QPointF> pointList_BatCap;
    QList<QPointF> pointList_OutPwr;
    QList<QPointF> pointList_InvTemp;
    QList<QPointF> pointList_DcDcTemp;
    for (int i = 0; i < arr.count(); i++) {
        QJsonObject obj = arr[i].toObject();
        timeStamp =  obj.value("TimeStamp").toVariant().toLongLong();
        if(i == 0) {
            timeStampStart = timeStamp;
        }
        gridInputPower = obj.value("GridInPwr").toDouble();
        if(gridInputPower > maximumGridInputPower) {
            maximumGridInputPower = gridInputPower;
        }
        gridChargePower = obj.value("GridChgPwr").toDouble();
        if(gridChargePower > maximumGridChargePower) {
            maximumGridChargePower = gridChargePower;
        }
        pvPower = obj.value("PvPwr").toDouble();
        if(pvPower > maximumPvPower) {
            maximumPvPower = pvPower;
        }
        batteryCapacity = obj.value("BatCap").toDouble();
        if(batteryCapacity > maximumBatteryCapacity) {
            maximumBatteryCapacity = batteryCapacity;
        }
        outputPower = obj.value("OutPwr").toDouble();
        if(outputPower > maximumOutputPower) {
            maximumOutputPower = outputPower;
        }
        inverterTemperature = obj.value("InvTemp").toDouble();
        if(inverterTemperature > maximumInverterTemperature) {
            maximumInverterTemperature = inverterTemperature;
        }
        if(inverterTemperature < minimumInverterTemperature) {
            minimumInverterTemperature = inverterTemperature;
        }
        dcDcTemperature = obj.value("DcDcTemp").toDouble();
        if(dcDcTemperature > maximumDcDcTemperature) {
            maximumDcDcTemperature = dcDcTemperature;
        }
        if(dcDcTemperature < minimumDcDcTemperature) {
            minimumDcDcTemperature = dcDcTemperature;
        }
        state = obj.value("State").toString();
        error = obj.value("Error").toString();
        warning = obj.value("Warning").toString();

        /*series_InPwr->append(timeStamp, gridInputPower);
        series_ChgPwr->append(timeStamp, gridChargePower);
        series_PvPwr->append(timeStamp, pvPower);
        series_BatCap->append(timeStamp, batteryCapacity);
        series_OutPwr->append(timeStamp, outputPower);
        series_InvTemp->append(timeStamp, inverterTemperature);
        series_DcDcTemp->append(timeStamp, dcDcTemperature);*/

        pointList_InPwr.append(QPointF(timeStamp, gridInputPower));
        pointList_ChgPwr.append(QPointF(timeStamp, gridChargePower));
        pointList_PvPwr.append(QPointF(timeStamp, pvPower));
        pointList_BatCap.append(QPointF(timeStamp, batteryCapacity));
        pointList_OutPwr.append(QPointF(timeStamp, outputPower));
        pointList_InvTemp.append(QPointF(timeStamp, inverterTemperature));
        pointList_DcDcTemp.append(QPointF(timeStamp, dcDcTemperature));
    }

    series_InPwr->append(pointList_InPwr);
    series_ChgPwr->append(pointList_ChgPwr);
    series_PvPwr->append(pointList_PvPwr);
    series_BatCap->append(pointList_BatCap);
    series_OutPwr->append(pointList_OutPwr);
    series_InvTemp->append(pointList_InvTemp);
    series_DcDcTemp->append(pointList_DcDcTemp);

    QDateTime dT;
    if(ui->checkBoxFixedChart->isChecked()) {
        dateTimeAxisXPwr->setRange(QDateTime().fromString(ui->comboBoxFileName->currentText(), "yyyy-MM-dd"), QDateTime().fromString(ui->comboBoxFileName->currentText(), "yyyy-MM-dd").addDays(1));
        dateTimeAxisXTemp->setRange(QDateTime().fromString(ui->comboBoxFileName->currentText(), "yyyy-MM-dd"), QDateTime().fromString(ui->comboBoxFileName->currentText(), "yyyy-MM-dd").addDays(1));
        valueAxisY_InPwr->setRange(0, 6000);
        valueAxisY_ChgPwr->setRange(0, 6000);
        valueAxisY_PvPwr->setRange(0, 6000);
        valueAxisY_BatCap->setRange(0, 100);
        valueAxisY_OutPwr->setRange(0, 6000);
    } else {
        dateTimeAxisXPwr->setRange(dT.fromMSecsSinceEpoch(timeStampStart), dT.fromMSecsSinceEpoch(timeStamp));
        dateTimeAxisXTemp->setRange(dT.fromMSecsSinceEpoch(timeStampStart), dT.fromMSecsSinceEpoch(timeStamp));
        valueAxisY_InPwr->setRange(0, maximumGridInputPower == 5.0 ? 0 : maximumGridInputPower);
        valueAxisY_ChgPwr->setRange(0, maximumGridChargePower == 5.0 ? 0 : maximumGridChargePower);
        valueAxisY_PvPwr->setRange(0, maximumPvPower == 5.0 ? 0 : maximumPvPower);
        valueAxisY_BatCap->setRange(0, maximumBatteryCapacity);
        valueAxisY_OutPwr->setRange(0, maximumOutputPower == 5.0 ? 0 : maximumOutputPower);
    }

    bool tempMinInvLower = minimumInverterTemperature < minimumDcDcTemperature;
    bool tempManInvBigget = maximumInverterTemperature > maximumDcDcTemperature;
    valueAxisY_InvTemp->setRange(tempMinInvLower ? minimumInverterTemperature : minimumDcDcTemperature,
                                 tempManInvBigget ? maximumInverterTemperature : maximumDcDcTemperature);
    valueAxisY_DcDcTemp->setRange(tempMinInvLower ? minimumInverterTemperature : minimumDcDcTemperature,
                                  tempManInvBigget ? maximumInverterTemperature : maximumDcDcTemperature);
}

void PowerChart::refreshFiles() {
    QDir directory(Static::getDatabasePath());
    QStringList files = directory.entryList(QDir::Files, QDir::Time/* | QDir::Reversed*/);
    QString selectedFileName = ui->comboBoxFileName->currentText();
    ui->comboBoxFileName->clear();
    ui->comboBoxFileName->addItems(files);
    //if(ui->comboBoxFileName->count() != 0) {
    //    if(ui->comboBoxFileName->currentIndex() == ui->comboBoxFileName->count() - 1) {
    //        ui->comboBoxFileName->setCurrentIndex(ui->comboBoxFileName->count() - 1);
    //    } else {
            //ui->comboBoxFileName->setCurrentText(selectedFileName);
    //    }
    //}
}

void PowerChart::on_radioButtonShowGridInPwr_1_toggled(bool checked) {
    if(checked) {
        ui->radioButtonShowGridInPwr_2->setEnabled(false);
    } else {
        ui->radioButtonShowGridInPwr_2->setEnabled(true);
    }
    refreshChart();
}


void PowerChart::on_radioButtonGridChgPwr_1_toggled(bool checked) {
    if(checked) {
        ui->radioButtonGridChgPwr_2->setEnabled(false);
    } else {
        ui->radioButtonGridChgPwr_2->setEnabled(true);
    }
    refreshChart();
}


void PowerChart::on_radioButtonPvPwr_1_toggled(bool checked) {
    if(checked) {
        ui->radioButtonPvPwr_2->setEnabled(false);
    } else {
        ui->radioButtonPvPwr_2->setEnabled(true);
    }
    refreshChart();
}


void PowerChart::on_radioButtonBatCap_1_toggled(bool checked) {
    if(checked) {
        ui->radioButtonBatCap_2->setEnabled(false);
    } else {
        ui->radioButtonBatCap_2->setEnabled(true);
    }
    refreshChart();
}


void PowerChart::on_radioButtonOutPwr_1_toggled(bool checked) {
    if(checked) {
        ui->radioButtonOutPwr_2->setEnabled(false);
    } else {
        ui->radioButtonOutPwr_2->setEnabled(true);
    }
    refreshChart();
}


void PowerChart::on_radioButtonShowGridInPwr_2_toggled(bool checked) {
    if(checked) {
        ui->radioButtonShowGridInPwr_1->setEnabled(false);
    } else {
        ui->radioButtonShowGridInPwr_1->setEnabled(true);
    }
    refreshChart();
}


void PowerChart::on_radioButtonGridChgPwr_2_toggled(bool checked) {
    if(checked) {
        ui->radioButtonGridChgPwr_1->setEnabled(false);
    } else {
        ui->radioButtonGridChgPwr_1->setEnabled(true);
    }
    refreshChart();
}


void PowerChart::on_radioButtonPvPwr_2_toggled(bool checked) {
    if(checked) {
        ui->radioButtonPvPwr_1->setEnabled(false);
    } else {
        ui->radioButtonPvPwr_1->setEnabled(true);
    }
    refreshChart();
}


void PowerChart::on_radioButtonBatCap_2_toggled(bool checked) {
    if(checked) {
        ui->radioButtonBatCap_1->setEnabled(false);
    } else {
        ui->radioButtonBatCap_1->setEnabled(true);
    }
    refreshChart();
}


void PowerChart::on_radioButtonOutPwr_2_toggled(bool checked) {
    if(checked) {
        ui->radioButtonOutPwr_1->setEnabled(false);
    } else {
        ui->radioButtonOutPwr_1->setEnabled(true);
    }
    refreshChart();
}

void PowerChart::rfshTimerEvent() {
    if (databaseWrites != Static::getDatabaseWrites()) {
        databaseWrites = Static::getDatabaseWrites();
        refreshChart();
    }
    if(filesSaved != Static::getFilesSaved()) {
        filesSaved = Static::getFilesSaved();
        refreshFiles();
    }
}


void PowerChart::on_comboBoxFileName_currentTextChanged(const QString &) {
    refreshChart();
}


void PowerChart::on_checkBoxFixedChart_toggled(bool) {
    refreshChart();
}

