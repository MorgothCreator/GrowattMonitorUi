#ifndef STATICS_H
#define STATICS_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>

class Static {
public:
    static QString strFormat(double value);

    static void setDatabasePath(QString path);
    static QString getDatabasePath();

    static void setSamplePath(QString path);
    static QString getSamplePath();

    static void setSettingsPath(QString path);
    static QString getSettingsPath();

    static void setLastSample(QString sample);
    static QString getLastSample();

    static void sampleClear();
    static void sampleAppend(QString gridInputPower, QString gridChargePower, QString pvPower, QString batteryCapacity, QString outputPower,
                             QString inverterTemperature, QString dcDcTemperature,
                             QString state,
                             QString error, QString warning);

    static QString database;
    static void databaseAppent();
    static int getDatabaseWrites();
    static int getFilesSaved();
};

#endif // STATICS_H
