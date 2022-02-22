#include <QMessageBox>
#include <QDir>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "static.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Static::setDatabasePath(QDir::homePath() + "/.GrowattMonitorDatabase/");
    QDir dir;
    dir.setPath(Static::getDatabasePath());
    if (!dir.exists()) {
        dir.mkpath(Static::getDatabasePath());
    }
    hideArrows();
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info: infos ) {
        ui->comboBoxPort->addItem(info.portName());
    }
    ui->comboBoxReadInterval->addItems({"0.5", "1", "2", "5", "10"});
    ui->comboBoxRecordInterval->addItems({"1", "2.5", "5", "10"});
    ui->comboBoxRecordInterval->setCurrentText("2.5");
    rfshTimer = new QTimer;
    saveTimer = new QTimer;
    appenddatabaseTimer = new QTimer;
    rfshPortStatusTimer = new QTimer;
    serialTimeoutTimer = new QTimer;
    animationTimer = new QTimer;

    connect(rfshPortStatusTimer, SIGNAL(timeout()), this, SLOT(rfshPortStatusTimerEvent()));
    rfshPortStatusTimer->start(500);
    connect(rfshTimer, SIGNAL(timeout()), this, SLOT(rfshTimerEvent()));
    connect(saveTimer, SIGNAL(timeout()), this, SLOT(saveTimerEvent()));
    saveTimer->start(1000);
    connect(serialTimeoutTimer, SIGNAL(timeout()), this, SLOT(serialTimeoutTimerEvent()));
    serial = new QSerialPort();
    serialTimeoutTimer->setInterval(10);
    connect(serial, SIGNAL(readyRead()), this, SLOT(serialData()));

    connect(animationTimer, SIGNAL(timeout()), this, SLOT(animationTimerEvent()));
    animationTimer->start(500);

    appenddatabaseTimer->setInterval(150000);
    connect(appenddatabaseTimer, SIGNAL(timeout()), this, SLOT(appenddatabaseTimerEvent()));


    setIcons();
}

MainWindow::~MainWindow()
{
    if (serial->isOpen())
        serial->close();
    delete ui;
}

void MainWindow::setIcons() {
    QIcon icon;
    QPixmap pixmap;
    QImage i;
    QColor color = QPalette().color(QPalette::Window);
    //QMessageBox messageBox;
    //messageBox.critical(0,"Error","Can't open " + QString::number(color.value(), 10));
    if(color.value() < 128) {
        ui->pushButtonGrid->setIcon(QIcon(":/png/resources/grid.png"));
        ui->pushButtonPv->setIcon(QIcon(":/png/resources/pannel.png"));
        ui->pushButtonHouse->setIcon(QIcon(":/png/resources/house.png"));
        ui->pushButtonBattery->setIcon(QIcon(":/png/resources/battery.png"));
        ui->pushButtonInverter->setIcon(QIcon(":/png/resources/inverter.png"));
        icon = ui->pushButtonGrid->icon();
        pixmap = icon.pixmap(71, 71);
        i = pixmap.toImage();
        i.invertPixels(QImage::InvertMode::InvertRgb);
        pixmap = pixmap.fromImage(i);
        ui->pushButtonGrid->setIcon(QIcon(pixmap));

        icon = ui->pushButtonPv->icon();
        pixmap = icon.pixmap(71, 71);
        i = pixmap.toImage();
        i.invertPixels(QImage::InvertMode::InvertRgb);
        pixmap = pixmap.fromImage(i);
        ui->pushButtonPv->setIcon(QIcon(pixmap));

        icon = ui->pushButtonHouse->icon();
        pixmap = icon.pixmap(71, 71);
        i = pixmap.toImage();
        i.invertPixels(QImage::InvertMode::InvertRgb);
        pixmap = pixmap.fromImage(i);
        ui->pushButtonHouse->setIcon(QIcon(pixmap));

        icon = ui->pushButtonBattery->icon();
        pixmap = icon.pixmap(71, 71);
        i = pixmap.toImage();
        i.invertPixels(QImage::InvertMode::InvertRgb);
        pixmap = pixmap.fromImage(i);
        ui->pushButtonBattery->setIcon(QIcon(pixmap));

        icon = ui->pushButtonInverter->icon();
        pixmap = icon.pixmap(71, 71);
        i = pixmap.toImage();
        i.invertPixels(QImage::InvertMode::InvertRgb);
        pixmap = pixmap.fromImage(i);
        ui->pushButtonInverter->setIcon(QIcon(pixmap));
    } else {
        ui->pushButtonGrid->setIcon(QIcon(":/png/resources/grid.png"));
        ui->pushButtonPv->setIcon(QIcon(":/png/resources/pannel.png"));
        ui->pushButtonHouse->setIcon(QIcon(":/png/resources/house.png"));
        ui->pushButtonBattery->setIcon(QIcon(":/png/resources/battery.png"));
        ui->pushButtonInverter->setIcon(QIcon(":/png/resources/inverter.png"));
    }
}

void getModbus(quint16 *array, int len) {
    for (int i = 0; i < len; i++) {
        array[i] = ((array[i] >> 8) & 0xFF) | ((array[i] << 8) & 0xFF00);
    }
}

double MainWindow::rssf( quint16 *array, int index, int scale) {
    return (double) array[index] / scale;
}

double MainWindow::rsdf( quint16 *array, int index, int scale) {
    return (double) (array[index] << 16 | array[index + 1]) / scale;
}

QString strFormat(double value) {
    return QString::asprintf("%.1f", value);
}

void MainWindow::parseData(QByteArray holding, QByteArray input) {
    if(holdingReg.count() > 270 || holdingReg.count() < 270 || inputReg.count() > 180 || inputReg.count() < 180)
        return;
    quint16 holdingHalf[holding.size() / 2];
    memcpy(holdingHalf, holding, holding.size());
    getModbus(holdingHalf, holding.size() / 2);
    quint16 inputHalf[input.size() / 2];
    memcpy(inputHalf, input, input.size());
    getModbus(inputHalf, input.size() / 2);
    QString inverterFirmwareVersion;
    inverterFirmwareVersion.append((char)(holdingHalf[9] >> 8));
    inverterFirmwareVersion.append((char)(holdingHalf[9]));
    inverterFirmwareVersion.append((char)(holdingHalf[10] >> 8));
    inverterFirmwareVersion.append((char)(holdingHalf[10]));
    inverterFirmwareVersion.append((char)(holdingHalf[11] >> 8));
    inverterFirmwareVersion.append((char)(holdingHalf[11]));
    QString mpptFirmwareVersion;
    mpptFirmwareVersion.append((char)(holdingHalf[12] >> 8));
    mpptFirmwareVersion.append((char)(holdingHalf[12]));
    mpptFirmwareVersion.append((char)(holdingHalf[13] >> 8));
    mpptFirmwareVersion.append((char)(holdingHalf[13]));
    mpptFirmwareVersion.append((char)(holdingHalf[14] >> 8));
    mpptFirmwareVersion.append((char)(holdingHalf[14]));
    QString serialNumber;
    serialNumber.append((char)(holdingHalf[23] >> 8));
    serialNumber.append((char)(holdingHalf[23]));
    serialNumber.append((char)(holdingHalf[24] >> 8));
    serialNumber.append((char)(holdingHalf[24]));
    serialNumber.append((char)(holdingHalf[25] >> 8));
    serialNumber.append((char)(holdingHalf[25]));
    serialNumber.append((char)(holdingHalf[26] >> 8));
    serialNumber.append((char)(holdingHalf[26]));
    serialNumber.append((char)(holdingHalf[27] >> 8));
    serialNumber.append((char)(holdingHalf[27]));
    int modelNumberInt = holdingHalf[28] << 16 | holdingHalf[29];
    QString modelNumber =
             "T" + QString((QChar)((modelNumberInt & 0xF00000) >> 20)) +
            " Q" + QString((QChar)((modelNumberInt & 0x0F0000) >> 16)) +
            " P" + QString((QChar)((modelNumberInt & 0x00F000) >> 12)) +
            " U" + QString((QChar)((modelNumberInt & 0x000F00) >> 8)) +
            " M" + QString((QChar)((modelNumberInt & 0x0000F0) >> 4)) +
            " S" + QString((QChar)(modelNumberInt & 0x00000F));

    QString batteryVoltage = QString().number(rssf(inputHalf, 17, 100));
    QString batterySoc = QString().number(rssf(inputHalf, 18, 1));
    QString pvVoltage = QString().number(rssf(inputHalf, 1));
    pvChargePowerDouble = rsdf(inputHalf, 3);
    QString pvChargePower = QString().number(pvChargePowerDouble);
    outputActivePowerDouble = rsdf(inputHalf, 9);
    QString outputActivePower = QString().number(outputActivePowerDouble);
    QString outputAparentPower = QString().number(rsdf(inputHalf, 11));
    QString gridChargeActivePower = QString().number(rsdf(inputHalf, 13));
    QString gridChargeAparentPower = QString().number(rsdf(inputHalf, 15));
    QString gridVoltage = QString().number(rssf(inputHalf, 20));
    QString gridFrequency = QString().number(rssf(inputHalf, 21, 100));
    QString outputVoltage = QString().number(rssf(inputHalf, 22));
    QString outputFrequency = QString().number(rssf(inputHalf, 23, 100));
    QString inverterTemperature = QString().number(rssf(inputHalf, 25));
    QString dcDcTemperature = QString().number(rssf(inputHalf, 26));
    QString loadPercent = QString().number(rssf(inputHalf, 27));
    gridInputActivePowerDouble = rsdf(inputHalf, 36);
    QString gridInputActivePower = QString().number(gridInputActivePowerDouble);
    //QString gridInputAparentPower = QString().number(rsdf(inputHalf, 38));
    int faultCode = inputHalf[42];
    int warningCode = inputHalf[43];
    QString pvPowerTodayKw = QString().number(rsdf(inputHalf, 48));
    QString pvPowerTotalKw = QString().number(rsdf(inputHalf, 50));
    QString batteryDischargeTodayKw = QString().number(rsdf(inputHalf, 60));
    QString batteryDischargeTotalKw = QString().number(rsdf(inputHalf, 62));
    QString gridDischargeTodayKw = QString().number(rsdf(inputHalf, 64));
    QString gridDischargeTotalKw = QString().number(rsdf(inputHalf, 66));
    charging = rsdf(inputHalf, 77) < 0;
    QString batteryDisChrPower = 0;
    if(!charging)
        batteryDisChrPower = QString().number(rsdf(inputHalf, 77));
    else
        batteryDisChrPower = QString().number(0 - rsdf(inputHalf, 77));

    ui->lineEditGridDischargeToday->setText(gridDischargeTodayKw);
    ui->lineEditGridDischargeTotal->setText(gridDischargeTotalKw);
    ui->lineEditPvProductioToday->setText(pvPowerTodayKw);
    ui->lineEditPvProductionTotal->setText(pvPowerTotalKw);
    ui->lineEditBatteryDischargeToday->setText(batteryDischargeTodayKw);
    ui->lineEditBatteryDischargeTotal->setText(batteryDischargeTotalKw);
    ui->lineEditInverterTemperature->setText(inverterTemperature);
    ui->lineEditDcDcTemperature->setText(dcDcTemperature);

    ui->lineEditSerialNumber->setText(serialNumber);
    ui->lineEditInverterCpuVersion->setText(inverterFirmwareVersion);
    ui->lineEditMpptCpuVersion->setText(mpptFirmwareVersion);

    ui->lineEditGridVoltage->setText(gridVoltage);
    ui->lineEditGridFrequency->setText(gridFrequency);
    ui->lineEditGridInputPower->setText(gridInputActivePower);
    ui->lineEditGridChargeCurrent->setText(gridChargeActivePower);

    ui->lineEditBatteryVoltage->setText(batteryVoltage);
    ui->lineEditBatteryCapacity->setText(batterySoc);
    ui->lineEditBatteryCapacity_2->setText(batterySoc);
    ui->lineEditChargeCurrent->setText(charging ? strFormat(batteryDisChrPower.toDouble() / batteryVoltage.toDouble()) : "0");
    ui->lineEditDischargeCurrent->setText(charging ? "0" : strFormat(batteryDisChrPower.toDouble() / batteryVoltage.toDouble()));

    ui->lineEditPvVoltage->setText(pvVoltage);
    ui->lineEditPvChargeCurrent->setText(strFormat(pvChargePower.toDouble() / batteryVoltage.toDouble()));
    ui->lineEditPvPower->setText(pvChargePower);
    ui->lineEditPvPower_2->setText(pvChargePower);

    ui->lineEditOutputVoltage->setText(outputVoltage);
    ui->lineEditOutputFrequency->setText(outputFrequency);
    ui->lineEditOutputActivePower->setText(outputActivePower);
    ui->lineEditOutputActivePower_2->setText(outputActivePower);
    ui->lineEditOutputAparentPower->setText(outputAparentPower);
    ui->lineEditLoadPercent->setText(loadPercent);

    QString outputPowerSet = QString().number(rsdf(holdingHalf, 76));
    QString gridRatedVoltageSet = QString().number(rssf(holdingHalf, 90));
    QString gridRatedFrequencySet = QString().number(rssf(holdingHalf, 91));
    QString batteryVoltageSet = QString().number(rssf(holdingHalf, 92));
    QString maxChargeCurrentSet = QString().number(rssf(holdingHalf, 34, 1));
    QString acChargeCurrentSet = QString().number(rssf(holdingHalf, 99, 1));

    ui->lineEditGridRatedVoltage->setText(gridRatedVoltageSet);
    ui->lineEditGridRatedFrequency->setText(gridRatedFrequencySet);
    ui->lineEditRatedBatteryVoltage->setText(batteryVoltageSet);
    ui->lineEditMaxChargeCurrent->setText(maxChargeCurrentSet);

    switch(holdingHalf[18]) {
    case 0:
        ui->lineEditOutputVoltage_2->setText("208");
        break;
    case 1:
        ui->lineEditOutputVoltage_2->setText("230");
        break;
    case 2:
        ui->lineEditOutputVoltage_2->setText("240");
        break;
    default:
        ui->lineEditOutputVoltage_2->setText("?");
        break;
    }
    switch(holdingHalf[19]) {
    case 0:
        ui->lineEditOutputFrequency_2->setText("50");
        break;
    case 1:
        ui->lineEditOutputFrequency_2->setText("60");
        break;
    default:
        ui->lineEditOutputFrequency_2->setText("?");
        break;
    }
    ui->lineEditOutputPower->setText(outputPowerSet);
    ui->lineEditAcChargeCurrent_2->setText(acChargeCurrentSet);

    for (int i = 0; i < 32; i++) {
        lastFault++;
        if(lastFault >= 31) {
            lastFault = 0;
        }
        if(faultCode & (1 << lastFault)) {
            faultCode &= 1 << lastFault;
            break;
        }
    }

    QString faultStr;
    if (faultCode & 0x0001)
        faultStr.append("\\");
    if (faultCode & 0x0002)
        faultStr.append("CPU A->B Communication error");
    if (faultCode & 0x0004)
        faultStr.append("Battery sample inconsistent");
    if (faultCode & 0x0008)
        faultStr.append("BUCK over current");
    if (faultCode & 0x0010)
        faultStr.append("BMS communication fault");
    if (faultCode & 0x0020)
        faultStr.append("Battery unnormal");
    if (faultCode & 0x0040)
        faultStr.append("\\");
    if (faultCode & 0x0080)
        faultStr.append("Battery high voltage");
    if (faultCode & 0x0100)
        faultStr.append("Over temperature");
    if (faultCode & 0x0200)
        faultStr.append("Over load");
    if (faultCode & 0x0400)
        faultStr.append("\\");
    if (faultCode & 0x0800)
        faultStr.append("\\");
    if (faultCode & 0x1000)
        faultStr.append("\\");
    if (faultCode & 0x2000)
        faultStr.append("\\");
    if (faultCode & 0x4000)
        faultStr.append("\\");
    if (faultCode & 0x8000)
        faultStr.append("\\");
    if (faultCode & 0x00010000)
        faultStr.append("Battery reverse connaction");
    if (faultCode & 0x00020000)
        faultStr.append("BUS soft start fail");
    if (faultCode & 0x00040000)
        faultStr.append("DC-DC unnormal");
    if (faultCode & 0x00080000)
        faultStr.append("DC voltage high");
    if (faultCode & 0x00100000)
        faultStr.append("CT detect failed");
    if (faultCode & 0x00200000)
        faultStr.append("CPU B->A Communication error");
    if (faultCode & 0x00400000)
        faultStr.append("BUS voltage high");
    if (faultCode & 0x00800000)
        faultStr.append("\\");
    if (faultCode & 0x01000000)
        faultStr.append("MOV break");
    if (faultCode & 0x02000000)
        faultStr.append("Output short circuit");
    if (faultCode & 0x04000000)
        faultStr.append("Li-Battery over load");
    if (faultCode & 0x08000000)
        faultStr.append("Output voltage high");
    if (faultCode & 0x10000000)
        faultStr.append("\\");
    if (faultCode & 0x20000000)
        faultStr.append("\\");
    if (faultCode & 0x40000000)
        faultStr.append("\\");
    if (faultCode & 0x80000000)
        faultStr.append("\\");
    ui->lineEditError->setText(faultStr);


    for (int i = 0; i < 32; i++) {
        lastWarning++;
        if(lastWarning >= 31) {
            lastWarning = 0;
        }
        if(warningCode & (1 << lastWarning)) {
            warningCode &= 1 << lastWarning;
            break;
        }
    }

    QString warningStr;
    if (warningCode & 0x0001)
        warningStr.append("Battery voltage low");
    if (warningCode & 0x0002)
        warningStr.append("Over temperature warning");
    if (warningCode & 0x0004)
        warningStr.append("Over load warning");
    if (warningCode & 0x0008)
        warningStr.append("Fail to read EEPROM");
    if (warningCode & 0x0010)
        warningStr.append("Firmware version unmatch");
    if (warningCode & 0x0020)
        warningStr.append("Fail to write EEPROM");
    if (warningCode & 0x0040)
        warningStr.append("BMS warning");
    if (warningCode & 0x0080)
        warningStr.append("Li-Battery over load warninge");
    if (warningCode & 0x0100)
        warningStr.append("Li-Battery aging warning");
    if (warningCode & 0x0200)
        warningStr.append("Fan lock");
    if (warningCode & 0x0400)
        warningStr.append("\\");
    if (warningCode & 0x0800)
        warningStr.append("\\");
    if (warningCode & 0x1000)
        warningStr.append("\\");
    if (warningCode & 0x2000)
        warningStr.append("\\");
    if (warningCode & 0x4000)
        warningStr.append("\\");
    if (warningCode & 0x8000)
        warningStr.append("\\");
    ui->lineEditWarning->setText(warningStr);

    systemStatus = inputHalf[0];
    switch(systemStatus) {
    case 0:
        ui->lineEditState->setText("StandBy");
        break;
    case 1:
        ui->lineEditState->setText("NotUse");
        break;
    case 2:
        ui->lineEditState->setText("Discharge");
        break;
    case 3:
        ui->lineEditState->setText("Fault");
        break;
    case 4:
        ui->lineEditState->setText("Flash");
        break;
    case 5:
        ui->lineEditState->setText("PV Charge");
        break;
    case 6:
        ui->lineEditState->setText("AC Charge");
        break;
    case 7:
        ui->lineEditState->setText("Combine Charge");
        break;
    case 8:
        ui->lineEditState->setText("Combine Charge and Bypass");
        break;
    case 9:
        ui->lineEditState->setText("PV Charge and Bypass");
        break;
    case 10:
        ui->lineEditState->setText("AC Charge and Bypass");
        break;
    case 11:
        ui->lineEditState->setText("Bypass");
        break;
    case 12:
        ui->lineEditState->setText("PV Charge and Discharge");
        break;
    default:
        ui->lineEditState->setText("Not defined");
        break;
    }

    switch(holdingHalf[2]) {
    case 0:
        ui->lineEditChargePriority->setText("PV First");
        break;
    case 1:
        ui->lineEditChargePriority->setText("PV & Utility");
        break;
    case 2:
        ui->lineEditChargePriority->setText("PV Only");
        break;
    default:
        ui->lineEditChargePriority->setText("?");
        break;
    }

    Static::sampleAppend(gridInputActivePower, gridChargeActivePower, pvChargePower, batterySoc, outputActivePower,
                              inverterTemperature, dcDcTemperature,
                                 ui->lineEditState->text(),
                                 faultStr, warningStr);
}

void MainWindow::on_pushButtonApply_clicked()
{
    deviceId = ui->spinBoxDeviceId->value();
    rfshTimerInterval = (int)(ui->comboBoxReadInterval->itemText(ui->comboBoxReadInterval->currentIndex()).toDouble() * 1000);
    rfshTimer->setInterval(rfshTimerInterval);
    rfshTimer->start();
    saveTimerInterval = (int)(ui->comboBoxRecordInterval->itemText(ui->comboBoxRecordInterval->currentIndex()).toDouble() * 60000);
    saveTimer->setInterval(saveTimerInterval);
    QString newName = ui->comboBoxPort->itemText(ui->comboBoxPort->currentIndex());
    if(serial->isOpen() && QString::compare(serial->portName(), newName)) {
        serial->close();
    }
    if(!serial->isOpen() || QString::compare(serial->portName(), newName)) {
        if(!appenddatabaseTimer->isActive()) {
            appenddatabaseTimer->start();
        }
        serial->setPortName(newName);
        serial->setBaudRate(QSerialPort::Baud9600);
        serial->setDataBits(QSerialPort::Data8);
        serial->setParity(QSerialPort::NoParity);
        serial->setStopBits(QSerialPort::OneStop);
        serial->setFlowControl(QSerialPort::NoFlowControl);
        if (!serial->open(QIODevice::ReadWrite)) {
            QMessageBox messageBox;
            messageBox.critical(0,"Error","Can't open " + serial->portName() + ", error code: " + serial->errorString());
            return;
        }
    }
}

unsigned int CRC16_2(unsigned char *buf, int len)
{
  unsigned int crc = 0xFFFF;
  for (int pos = 0; pos < len; pos++)
  {
  crc ^= (unsigned int)buf[pos];    // XOR byte into least sig. byte of crc

  for (int i = 8; i != 0; i--) {    // Loop over each bit
    if ((crc & 0x0001) != 0) {      // If the LSB is set
      crc >>= 1;                    // Shift right and XOR 0xA001
      crc ^= 0xA001;
    }
    else                            // Else LSB is not set
      crc >>= 1;                    // Just shift right
    }
  }

  return crc;
}

void MainWindow::rfshTimerEvent() {
    rfshTimer->stop();
    if(serial->isOpen()) {
        holdingReg.clear();
        inputReg.clear();
        QByteArray array;
        array.resize(8);
        array[0] = deviceId;
        array[1] = 0x03;
        array[2] = 0x00;
        array[3] = 0x00;
        array[4] = 0x00;
        array[5] = 0x2D;
        quint16 crc = CRC16_2((unsigned char *)array.mid(0, 6).data(), 6);
        array[6] = crc;
        array[7] = crc >> 8;
        serial->write(array);
    }
}

void MainWindow::saveTimerEvent() {

}

void MainWindow::rfshPortStatusTimerEvent() {
    if (serial->isOpen())
        ui->labelPortStatus->setText("Oppened");
    else {
        ui->labelPortStatus->setText("Closed");
        dataReceived = false;
    }
}

void MainWindow::serialData() {
    receiveArray.append(serial->readAll());
    serialTimeoutTimer->stop();
    serialTimeoutTimer->start();
}

void MainWindow::serialTimeoutTimerEvent() {
    serialTimeoutTimer->stop();
    //QMessageBox messageBox;
    //messageBox.information(0,"Information", receiveArray.toHex());
    if (sentCommand == 3) {
        if (sentAddress == 0) {
            sentAddress = 0x2D;
            holdingReg.append(receiveArray.mid(3, 90));
            msg3_1.clear();
            msg3_1.append(receiveArray);
            receiveArray.clear();
            QByteArray array;
            array.resize(8);
            array[0] = deviceId;
            array[1] = 0x03;
            array[2] = 0x00;
            array[3] = 0x2D;
            array[4] = 0x00;
            array[5] = 0x2D;
            quint16 crc = CRC16_2((unsigned char *)array.mid(0, 6).data(), 6);
            array[6] = crc;
            array[7] = crc >> 8;
            serial->write(array);
        } else if (sentAddress == 0x2D) {
            sentAddress = 0x54;
            holdingReg.append(receiveArray.mid(3, 90));
            msg3_2.clear();
            msg3_2.append(receiveArray);
            receiveArray.clear();
            QByteArray array;
            array.resize(8);
            array[0] = deviceId;
            array[1] = 0x03;
            array[2] = 0x00;
            array[3] = 0x54;
            array[4] = 0x00;
            array[5] = 0x2D;
            quint16 crc = CRC16_2((unsigned char *)array.mid(0, 6).data(), 6);
            array[6] = crc;
            array[7] = crc >> 8;
            serial->write(array);
        } else if (sentAddress == 0x54) {
            sentCommand = 4;
            sentAddress = 0x0;
            holdingReg.append(receiveArray.mid(3, 90));
            msg3_3.clear();
            msg3_3.append(receiveArray);
            receiveArray.clear();
            QByteArray array;
            array.resize(8);
            array[0] = deviceId;
            array[1] = 0x04;
            array[2] = 0x00;
            array[3] = 0x00;
            array[4] = 0x00;
            array[5] = 0x2D;
            quint16 crc = CRC16_2((unsigned char *)array.mid(0, 6).data(), 6);
            array[6] = crc;
            array[7] = crc >> 8;
            serial->write(array);
        }
    } else if (sentCommand == 4) {
        if (sentAddress == 0) {
            sentAddress = 0x2D;
            inputReg.append(receiveArray.mid(3, 90));
            msg4_1.clear();
            msg4_1.append(receiveArray);
            receiveArray.clear();
            QByteArray array;
            array.resize(8);
            array[0] = deviceId;
            array[1] = 0x04;
            array[2] = 0x00;
            array[3] = 0x2D;
            array[4] = 0x00;
            array[5] = 0x2D;
            quint16 crc = CRC16_2((unsigned char *)array.mid(0, 6).data(), 6);
            array[6] = crc;
            array[7] = crc >> 8;
            serial->write(array);
        } else if (sentAddress == 0x2D) {
            sentCommand = 3;
            sentAddress = 0x0;
            inputReg.append(receiveArray.mid(3, 90));
            msg4_2.clear();
            msg4_2.append(receiveArray);
            receiveArray.clear();
            parseData(holdingReg, inputReg);
            rfshTimer->start();
            dataReceived = true;
        }
    }
}

void MainWindow::hideArrows() {
    ui->pushButtonArrowBatteryD_1->setVisible(false);
    ui->pushButtonArrowBatteryD_2->setVisible(false);
    ui->pushButtonArrowBatteryD_3->setVisible(false);
    ui->pushButtonArrowBatteryD_4->setVisible(false);
    ui->pushButtonArrowBatteryU_1->setVisible(false);
    ui->pushButtonArrowBatteryU_2->setVisible(false);
    ui->pushButtonArrowBatteryU_3->setVisible(false);
    ui->pushButtonArrowBatteryU_4->setVisible(false);
    ui->pushButtonArrowPannel_1->setVisible(false);
    ui->pushButtonArrowPannel_2->setVisible(false);
    ui->pushButtonArrowPannel_3->setVisible(false);
    ui->pushButtonArrowPannel_4->setVisible(false);
    ui->pushButtonArrowGridR_1->setVisible(false);
    ui->pushButtonArrowGridR_2->setVisible(false);
    ui->pushButtonArrowGridR_3->setVisible(false);
    ui->pushButtonArrowGridR_4->setVisible(false);
    ui->pushButtonArrowGridL_1->setVisible(false);
    ui->pushButtonArrowGridL_2->setVisible(false);
    ui->pushButtonArrowGridL_3->setVisible(false);
    ui->pushButtonArrowGridL_4->setVisible(false);
    ui->pushButtonArrowHouse_1->setVisible(false);
    ui->pushButtonArrowHouse_2->setVisible(false);
    ui->pushButtonArrowHouse_3->setVisible(false);
    ui->pushButtonArrowHouse_4->setVisible(false);
}

void MainWindow::animationTimerEvent() {
    hideArrows();
    if(serial->isOpen() && dataReceived) {
        animationCount++;
        if(animationCount > 7)
            animationCount = 0;
        /*if(chargingLast != charging) {
            chargingLast = charging;
            animationCount = 0;
        }*/
        if(systemStatus != 11) {
            if(charging) {
                switch (animationCount) {
                case 0:
                    ui->pushButtonArrowBatteryD_1->setVisible(true);
                case 1:
                    ui->pushButtonArrowBatteryD_2->setVisible(true);
                case 2:
                    ui->pushButtonArrowBatteryD_3->setVisible(true);
                case 3:
                    ui->pushButtonArrowBatteryD_4->setVisible(true);
                    break;
                case 7:
                    ui->pushButtonArrowBatteryD_4->setVisible(true);
                case 6:
                    ui->pushButtonArrowBatteryD_3->setVisible(true);
                case 5:
                    ui->pushButtonArrowBatteryD_2->setVisible(true);
                case 4:
                    ui->pushButtonArrowBatteryD_1->setVisible(true);
               }
            } else {
                switch (animationCount) {
                case 4:
                    ui->pushButtonArrowBatteryU_1->setVisible(true);
                case 5:
                    ui->pushButtonArrowBatteryU_2->setVisible(true);
                case 6:
                    ui->pushButtonArrowBatteryU_3->setVisible(true);
                case 7:
                    ui->pushButtonArrowBatteryU_4->setVisible(true);
                    break;
                case 3:
                    ui->pushButtonArrowBatteryU_4->setVisible(true);
                case 2:
                    ui->pushButtonArrowBatteryU_3->setVisible(true);
                case 1:
                    ui->pushButtonArrowBatteryU_2->setVisible(true);
                case 0:
                    ui->pushButtonArrowBatteryU_1->setVisible(true);
                }
            }
        }

        if(pvChargePowerDouble != 0.0) {
            switch (animationCount) {
            case 4:
                ui->pushButtonArrowPannel_1->setVisible(true);
            case 5:
                ui->pushButtonArrowPannel_2->setVisible(true);
            case 6:
                ui->pushButtonArrowPannel_3->setVisible(true);
            case 7:
                ui->pushButtonArrowPannel_4->setVisible(true);
                break;
            case 3:
                ui->pushButtonArrowPannel_4->setVisible(true);
            case 2:
                ui->pushButtonArrowPannel_3->setVisible(true);
            case 1:
                ui->pushButtonArrowPannel_2->setVisible(true);
            case 0:
                ui->pushButtonArrowPannel_1->setVisible(true);
            }
        }
        if(systemStatus == 6 || systemStatus == 10 || systemStatus == 11) {
            if (acDirectionOut) {
                switch (animationCount) {
                case 0:
                    ui->pushButtonArrowGridL_1->setVisible(true);
                case 1:
                    ui->pushButtonArrowGridL_2->setVisible(true);
                case 2:
                    ui->pushButtonArrowGridL_3->setVisible(true);
                case 3:
                    ui->pushButtonArrowGridL_4->setVisible(true);
                    break;
                case 7:
                    ui->pushButtonArrowGridL_4->setVisible(true);
                case 6:
                    ui->pushButtonArrowGridL_3->setVisible(true);
                case 5:
                    ui->pushButtonArrowGridL_2->setVisible(true);
                case 4:
                    ui->pushButtonArrowGridL_1->setVisible(true);
                }
            } else {
                switch (animationCount) {
                case 4:
                    ui->pushButtonArrowGridR_1->setVisible(true);
                case 5:
                    ui->pushButtonArrowGridR_2->setVisible(true);
                case 6:
                    ui->pushButtonArrowGridR_3->setVisible(true);
                case 7:
                    ui->pushButtonArrowGridR_4->setVisible(true);
                    break;
                case 3:
                    ui->pushButtonArrowGridR_4->setVisible(true);
                case 2:
                    ui->pushButtonArrowGridR_3->setVisible(true);
                case 1:
                    ui->pushButtonArrowGridR_2->setVisible(true);
                case 0:
                    ui->pushButtonArrowGridR_1->setVisible(true);
                }
            }
        }
        if(outputActivePowerDouble != 0.0) {
            switch (animationCount) {
            case 0:
                ui->pushButtonArrowHouse_1->setVisible(true);
            case 1:
                ui->pushButtonArrowHouse_2->setVisible(true);
            case 2:
                ui->pushButtonArrowHouse_3->setVisible(true);
            case 3:
                ui->pushButtonArrowHouse_4->setVisible(true);
                break;
            case 7:
                ui->pushButtonArrowHouse_4->setVisible(true);
            case 6:
                ui->pushButtonArrowHouse_3->setVisible(true);
            case 5:
                ui->pushButtonArrowHouse_2->setVisible(true);
            case 4:
                ui->pushButtonArrowHouse_1->setVisible(true);
            }
        }
    }
}

void MainWindow::appenddatabaseTimerEvent() {
    Static::databaseAppent();
}


bool MainWindow::event(QEvent *event) {
    if (event->type() == QEvent::ApplicationPaletteChange) {
        setIcons();
        event->ignore();
    }
    return QMainWindow::event(event);
}

void MainWindow::on_actionAbout_triggered() {
    about = new About();
    about->exec();
}

void MainWindow::on_actionChart_triggered() {
    chart = new PowerChart();
    chart->show();
}

