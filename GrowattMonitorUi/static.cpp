#include "static.h"

#include <QString>
#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


static QList<QJsonObject> Samples;
static QString DatabasePath;
static int DatabaseWrites = 0;
static int FilesSaved = 0;

QString Static::strFormat(double value) {
    return QString::asprintf("%.1f", value);
}

void Static::setDatabasePath(QString path) { DatabasePath = path; }
QString Static::getDatabasePath() { return DatabasePath; }

void Static::sampleClear() { Samples.clear(); };
void Static::sampleAppend(QString gridInputPower, QString gridChargePower, QString pvPower, QString batteryCapacity, QString outputPower,
                          QString inverterTemperature, QString dcDcTemperature,
                             QString state,
                             QString error, QString warning) {
    QJsonObject obj;
    obj.insert("TimeStamp", QDateTime::currentDateTime().toMSecsSinceEpoch());
    obj.insert("GridInPwr", gridInputPower.toDouble());
    obj.insert("GridChgPwr", gridChargePower.toDouble());
    obj.insert("PvPwr", pvPower.toDouble());
    obj.insert("BatCap", batteryCapacity.toDouble());
    obj.insert("OutPwr", outputPower.toDouble());
    obj.insert("InvTemp", inverterTemperature.toDouble());
    obj.insert("DcDcTemp", dcDcTemperature.toDouble());
    obj.insert("State", state);
    obj.insert("Error", error);
    obj.insert("Warning", warning);
    Samples.append(obj);
}

void Static::databaseAppent() {
    QList<QJsonObject> samples = Samples;
    Static::sampleClear();

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

    foreach (QJsonObject obj, samples) {
        gridInputPower += obj.value("GridInPwr").toDouble();
        gridChargePower += obj.value("GridChgPwr").toDouble();
        pvPower += obj.value("PvPwr").toDouble();
        batteryCapacity += obj.value("BatCap").toDouble();
        outputPower += obj.value("OutPwr").toDouble();
        inverterTemperature += obj.value("InvTemp").toDouble();
        dcDcTemperature += obj.value("DcDcTemp").toDouble();
        if (!state.length())
            state = obj.value("State").toString();
        if (!error.length())
            error = obj.value("Error").toString();
        if (!warning.length())
            warning = obj.value("Warning").toString();
    }
    gridInputPower /= samples.count();
    gridChargePower /= samples.count();
    pvPower /= samples.count();
    batteryCapacity /= samples.count();
    outputPower /= samples.count();
    inverterTemperature /= samples.count();
    dcDcTemperature /= samples.count();

    QJsonObject obj;
    obj.insert("TimeStamp", QDateTime::currentDateTime().toMSecsSinceEpoch());
    obj.insert("GridInPwr", Static::strFormat(gridInputPower).toDouble());
    obj.insert("GridChgPwr", Static::strFormat(gridChargePower).toDouble());
    obj.insert("PvPwr", Static::strFormat(pvPower).toDouble());
    obj.insert("BatCap", Static::strFormat(batteryCapacity).toDouble());
    obj.insert("OutPwr", Static::strFormat(outputPower).toDouble());
    obj.insert("InvTemp", Static::strFormat(inverterTemperature).toDouble());
    obj.insert("DcDcTemp", Static::strFormat(dcDcTemperature).toDouble());
    obj.insert("State", state);
    obj.insert("Error", error);
    obj.insert("Warning", warning);

    QString val = "[]";
    QFile file;
    file.setFileName(Static::getDatabasePath() + QDateTime::currentDateTime().date().toString("yyyy-MM-dd"));
    bool newFile = false;
    if(file.exists()) {
        file.open(QIODevice::ReadOnly | QIODevice::Text);
        val = file.readAll();
        file.close();
    } else {
        newFile = true;
    }
    QJsonDocument d = QJsonDocument::fromJson(val.toUtf8());
    QJsonArray arr = d.array();
    arr.append(obj);
    QJsonDocument wd(arr);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(wd.toJson());
    file.flush();
    file.close();
    DatabaseWrites++;
    if(newFile) {
        FilesSaved++;
    }
}

int Static::getDatabaseWrites() {
    return DatabaseWrites;
}
int Static::getFilesSaved() {
    return FilesSaved;
}
