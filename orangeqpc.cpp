#include "orangeqpc.h"
#include "./ui_orangeqpc.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QProcess>
#include <QMessageBox>
#include <QString>
#include <QIcon>

OrangeQPC::OrangeQPC(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::OrangeQPC)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(":/assets/logo.svg"));

    this->setFixedSize(553, 652);

    // Установка стилей интерфейса
    this->setStyleSheet(
        "QMainWindow { background-color: #EFEFEF; }"

        // Стилизация контейнеров групп
        "QGroupBox { "
        "   background-color: #F8F9FA; "
        "   border: 1px solid #E5E5E5; "
        "   border-radius: 14px; "
        "   margin-top: 20px; "
        "   padding-left: 15px; "
        "   padding-right: 15px; "
        "   padding-bottom: 15px; "
        "   padding-top: 45px; "
        "   font-weight: bold; "
        "   color: #4A4A4A; "
        "}"

        // Позиционирование заголовка строго внутрь рамки блока
        "QGroupBox::title { "
        "   subcontrol-origin: padding; "
        "   subcontrol-position: top center; "
        "   top: 15px; "
        "   padding: 0px 10px; "
        "   font-size: 14px; "
        "   color: #333333; "
        "}"

        "QLineEdit { "
        "   background-color: #FFFFFF; "
        "   border: 2px solid #E0E0E0; "
        "   border-radius: 10px; "
        "   padding: 6px 12px; "
        "   color: #333333; "
        "}"
        "QLineEdit:focus { border: 2px solid #FF5E3A; }"
        "QLineEdit:disabled { background-color: #EAEAEA; color: #999999; border-color: #D6D6D6; }"

        // Основные градиентные кнопки (Create, Run)
        "QPushButton { "
        "   background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #FF9F43, stop:1 #FF3366); "
        "   color: white; "
        "   border: none; "
        "   border-radius: 10px; "
        "   padding: 8px 18px; "
        "   font-weight: bold; "
        "   min-width: 90px; "
        "}"
        "QPushButton:hover { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #FFAA55, stop:1 #FF4577); }"
        "QPushButton:pressed { background: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #E68A32, stop:1 #E62255); }"

        // Белые кнопки Browse с контуром
        "QPushButton#BrowseButton, QPushButton#BrowseDVDButton, QPushButton#BrowseQemuButton { "
        "   background: #FFFFFF; "
        "   color: #FF3366; "
        "   border: 2px solid #FF3366; "
        "   min-width: 95px; "
        "}"
        "QPushButton#BrowseButton:hover, QPushButton#BrowseDVDButton:hover, QPushButton#BrowseQemuButton:hover { background: #FFF0F2; }"

        // Текст радиокнопок
        "QRadioButton { spacing: 8px; color: #4A4A4A; }"

        // Неактивное состояние радиокнопки (серый пустой круг)
        "QRadioButton::indicator { "    
        "   width: 20px; "  
        "   height: 20px; "
        "   image: url(:/assets/radio_unchecked.svg); "
        "}"

        // Активное состояние: точный градиентный круг с белой точкой по центру
        "QRadioButton::indicator:checked { "
        "   width: 20px; "  
        "   height: 20px; "    
        "   image: url(:/assets/radio_checked.svg); "
        "}"

        "QLabel { color: #555555; }"
        );


    // Начальное состояние элементов DVD
    ui->WithoutDVDRadioButton->setChecked(true);
    ui->DVDLineEdit->setEnabled(false);
    ui->BrowseDVDButton->setEnabled(false);
}

OrangeQPC::~OrangeQPC()
{
    delete ui;
}

// Toggle: "Without DVD" mode selected
void OrangeQPC::on_WithoutDVDRadioButton_toggled(bool checked)
{
    if (checked) {
        ui->DVDLineEdit->setEnabled(false);
        ui->BrowseDVDButton->setEnabled(false);
    }
}

// Toggle: "Use DVD" mode selected
void OrangeQPC::on_UseDVDRadioButton_toggled(bool checked)
{
    if (checked) {
        ui->DVDLineEdit->setEnabled(true);
        ui->BrowseDVDButton->setEnabled(true);
    }
}

// Select ISO image for DVD
void OrangeQPC::on_BrowseDVDButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Select ISO Image of MacOS Installation Disk"),
        QDir::homePath(),
        tr("Disk Images (*.iso *.cdr);;All Files (*.*)")
        );

    if (!filePath.isEmpty()) {
        ui->DVDLineEdit->setText(filePath);
    }
}

// Select destination path to create virtual disk
void OrangeQPC::on_BrowseButton_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(
        this,
        tr("Create QEMU Virtual Disk"),
        QDir::homePath() + "/macos.qcow2",
        tr("QEMU Disk Images (*.qcow2);;All Files (*.*)")
        );

    if (!filePath.isEmpty()) {
        ui->DiskPathEdit->setText(filePath);
    }
}

// Create virtual disk utilizing qemu-img tool
void OrangeQPC::on_CreateDiskButton_clicked()
{
    QString filePath = ui->DiskPathEdit->text().trimmed();
    QString diskSize = ui->DiskSizeEdit->text().trimmed();

    if (filePath.isEmpty()) {
        QMessageBox::warning(this, tr("Error"), tr("Please select a disk save location using the Browse button!"));
        return;
    }
    if (diskSize.isEmpty() || diskSize.toInt() <= 0) {
        QMessageBox::warning(this, tr("Error"), tr("Please specify a valid disk size in GB!"));
        return;
    }

    QStringList arguments;
    arguments << "create" << "-f" << "qcow2" << filePath << (diskSize + "G");

    QProcess *qemuImgProcess = new QProcess(this);
    qemuImgProcess->start("qemu-img", arguments);

    if (qemuImgProcess->waitForFinished(10000)) {
        if (qemuImgProcess->exitCode() == 0) {
            QMessageBox::information(this, tr("Success"), tr("Virtual disk successfully created!\n%1").arg(filePath));
        } else {
            QString errorOutput = qemuImgProcess->readAllStandardError();
            QMessageBox::critical(this, tr("QEMU Error"), tr("Failed to create disk:\n") + errorOutput);
        }
    } else {
        QMessageBox::critical(this, tr("Error"), tr("The disk creation process timed out or the qemu-img utility was not found."));
    }

    qemuImgProcess->deleteLater();
}

// Select existing virtual disk to run
void OrangeQPC::on_BrowseQemuButton_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        tr("Open QEMU Virtual Disk"),
        QDir::homePath(),
        tr("Disk Images (*.qcow2 *.img *.iso);;All Files (*.*)")
        );

    if (!filePath.isEmpty()) {
        ui->QemuPathEdit->setText(filePath);
    }
}

// Main execution function for running QEMU
void OrangeQPC::on_RunButton_clicked()
{

    // 1. Extract and validate data from the interface fields
    QString hdaPath = ui->QemuPathEdit->text().trimmed();
    QString ramMbStr = ui->RamSizeEdit->text().trimmed();

    if (hdaPath.isEmpty()) {
        QMessageBox::warning(this, tr("Launch Error"), tr("Please specify the path to the virtual disk in the Qemu Drive field!"));
        return;
    }
    if (ramMbStr.isEmpty() || ramMbStr.toInt() <= 0) {
        QMessageBox::warning(this, tr("Launch Error"), tr("Please enter a valid RAM size in megabytes!"));
        return;
    }

    // 2. Configure variables based on target PowerPC Mac architecture
    QString executableName = "qemu-system-ppc";
    QString machineConfig = "mac99,via=pmu";
    QStringList extraArguments;

    // Matching radio buttons naming according to your .ui layout
    if (ui->MacOS9RadioButton->isChecked()) {
        machineConfig = "mac99,via=pmu";
    }
    else if (ui->MacOS10PublicBetaRadioButton->isChecked() ||
             ui->MacOS100RadioButton->isChecked() ||
             ui->MacOS101RadioButton->isChecked() ||
             ui->MacOS102RadioButton->isChecked() ||
             ui->MacOS103RadioButton->isChecked()) {
        machineConfig = "mac99,via=pmu";
        extraArguments << "-net" << "nic,model=sungem" << "-net" << "user";
    }
    else if (ui->MacOS104RadioButton->isChecked() || ui->MacOS105RadioButton->isChecked()) {
        machineConfig = "mac99"; // Disable PMU for Tiger/Leopard stability
        extraArguments << "-net" << "nic,model=sungem" << "-net" << "user";
    }
    else {
        QMessageBox::warning(this, tr("Launch Error"), tr("Please select a MacOS version from the list!"));
        return;
    }

    // 3. Form execution arguments with proper drive indexing syntax
    QStringList arguments;
    arguments << "-M" << machineConfig
              << "-m" << ramMbStr
              << "-boot" << "c"
              << "-drive" << QString("file=%1,format=qcow2,index=0,media=disk").arg(hdaPath);

    // Append network and system configurations
    arguments << extraArguments;

    // 4. Handle installation DVD/ISO usage logic
    if (ui->UseDVDRadioButton->isChecked()) {
        QString isoPath = ui->DVDLineEdit->text().trimmed();
        if (isoPath.isEmpty()) {
            QMessageBox::warning(this, tr("Launch Error"), tr("You selected 'Use DVD' mode but did not specify the path to the ISO image!"));
            return;
        }

        arguments << "-drive" << QString("file=%1,format=raw,index=2,media=cdrom").arg(isoPath);

        // Modify boot priority to CD-ROM ('d') instead of hard drive ('c')
        int bootIndex = arguments.indexOf("-boot");
        if (bootIndex != -1 && bootIndex + 1 < arguments.size()) {
            arguments[bootIndex + 1] = "d";
        }
    }

    // 5. Launch independent (Detached) QEMU process in Linux
    qint64 pid;
    bool success = QProcess::startDetached(executableName, arguments, QString(), &pid);

    if (!success) {
        // If start fails, perform diagnostics via terminal utility 'which'
        QProcess checkProcess;
        checkProcess.start("which", QStringList() << executableName);
        checkProcess.waitForFinished(1000);
        QString whichOutput = checkProcess.readAllStandardOutput().trimmed();

        if (whichOutput.isEmpty()) {
            QMessageBox::critical(this, tr("Emulator Not Found"),
                                  tr("The command '%1' was not found on your Linux system.\n\n"
                                     "Please install the emulator package. Example for Ubuntu/Debian:\n"
                                     "sudo apt install qemu-system-ppc\n\n"
                                     "Example for Arch Linux:\n"
                                     "sudo pacman -S qemu-system-ppc").arg(executableName));
        } else {
            QMessageBox::critical(this, tr("Execution Error"),
                                  tr("Binary file found at %1, but Linux denied process initialization.\n"
                                     "Please check access permissions for the qcow2 virtual disk file.").arg(whichOutput));
        }
    }
}
