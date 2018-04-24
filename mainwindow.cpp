#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "filevariable.h"
#include "fastboot.h"

#include <QProcess>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QDate>
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("SFastboot EDA50/EDA50K"));

    ui->developer_widget->setVisible(false);

    // Set progress bar
    this->ui->progressBar->setValue(0);

    // Set the all default values in FileVariable
    FileVariable::InitFileVariable();

    DisplayDefValue();
    SetDefaultCheck();
    SetStatusLabColor(QColor(0xfd, 0xb9, 0x33));

    // Connect signal to slot
    connect(&updateThread, SIGNAL(updatingFile(QString)),
            this, SLOT(updatingFile(QString)));
    connect(&updateThread, SIGNAL(updateFileDone(QString,bool)),
            this, SLOT(updateFileDone(QString,bool)));
    connect(&updateThread, SIGNAL(updateComplete(bool)),
            this, SLOT(updateComplete(bool)));

    // Set windows size
    setWindowFlags(windowFlags()& ~Qt::WindowMaximizeButtonHint);
    setFixedSize(this->width(), this->height());
}

void MainWindow::SetAPCheckBoxStatus(bool check)
{
    // AP
    ui->appsboot_cb->setChecked(check);
    ui->boot_cb->setChecked(check);
    ui->cache_cb->setChecked(check);
    ui->splash_cb->setChecked(check);
    ui->system_cb->setChecked(check);
    ui->userdata_cb->setChecked(check);
    ui->recovery_cb->setChecked(check);
    ui->persist_cb->setChecked(check);
    ui->ipsm_cb->setChecked(check);

    ui->ap_selectall_cb->setChecked(check);
}

void MainWindow::SetMPCheckBoxStatus(bool check)
{
    // MP
    ui->modem_cb->setChecked(check);
    ui->sbl1_cb->setChecked(check);
    ui->rpm_cb->setChecked(check);
    ui->tz_cb->setChecked(check);

    ui->mp_select_all_cb->setChecked(check);
}

void MainWindow::SetDefaultCheck()
{
    // AP
    ui->appsboot_cb->setChecked(FileVariable::apCheckStatus.value("aboot"));
    ui->boot_cb->setChecked(FileVariable::apCheckStatus.value("boot"));
    ui->cache_cb->setChecked(FileVariable::apCheckStatus.value("cache"));
    ui->splash_cb->setChecked(FileVariable::apCheckStatus.value("splash"));
    ui->system_cb->setChecked(FileVariable::apCheckStatus.value("system"));
    ui->userdata_cb->setChecked(FileVariable::apCheckStatus.value("userdata"));
    ui->recovery_cb->setChecked(FileVariable::apCheckStatus.value("recovery"));
    ui->persist_cb->setChecked(FileVariable::apCheckStatus.value("persist"));
    ui->ipsm_cb->setChecked(FileVariable::apCheckStatus.value("IPSM"));

    // MP
    ui->modem_cb->setChecked(FileVariable::mpCheckStatus.value("modem"));
    ui->sbl1_cb->setChecked(FileVariable::mpCheckStatus.value("sbl1"));
    ui->rpm_cb->setChecked(FileVariable::mpCheckStatus.value("rpm"));
    ui->tz_cb->setChecked(FileVariable::mpCheckStatus.value("tz"));

    // Optional
    ui->autoreboot_cb->setChecked(FileVariable::opCheckStatus.value("autoreboot"));
}

void MainWindow::DisplayDefValue()
{
    // AP
    ui->ap_folder_le->setText(FileVariable::apFiles.value("folder"));
    ui->boot_le->setText(FileVariable::apFiles.value("boot"));
    ui->system_le->setText(FileVariable::apFiles.value("system"));
    ui->recovery_le->setText(FileVariable::apFiles.value("recovery"));
    ui->persist_le->setText(FileVariable::apFiles.value("persist"));
    ui->ipsm_le->setText(FileVariable::apFiles.value("IPSM"));
    ui->userdata_le->setText(FileVariable::apFiles.value("userdata"));
    ui->appsboot_le->setText(FileVariable::apFiles.value("aboot"));
    ui->cache_le->setText(FileVariable::apFiles.value("cache"));
    ui->splash_le->setText(FileVariable::apFiles.value("splash"));

    // MP
    ui->mp_folder_le->setText(FileVariable::mpFiles.value("folder"));
    ui->hlos_le->setText(FileVariable::mpFiles.value("modem"));
    ui->rpm_le->setText(FileVariable::mpFiles.value("rpm"));
    ui->sbl1_le->setText(FileVariable::mpFiles.value("sbl1"));
    ui->tz_le->setText(FileVariable::mpFiles.value("tz"));

    // OneKey
    ui->onekey_apfolder_le->setText(FileVariable::apFiles.value("folder"));
    ui->onekey_mpfolder_le->setText(FileVariable::mpFiles.value("folder"));
}

void MainWindow::SetStatusLabColor(QColor color)
{
    // Set status of label background
    QPalette palette = ui->status_lab->palette();
    palette.setColor(QPalette::Background, color);
    ui->status_lab->setPalette(palette);
}

void MainWindow::GetAPCheckBoxStatus()
{
    FileVariable::apCheckStatus["aboot"] = ui->appsboot_cb->isChecked();
    FileVariable::apCheckStatus["boot"] = ui->boot_cb->isChecked();
    FileVariable::apCheckStatus["cache"] = ui->cache_cb->isChecked();
    FileVariable::apCheckStatus["splash"] = ui->splash_cb->isChecked();
    FileVariable::apCheckStatus["system"] = ui->system_cb->isChecked();
    FileVariable::apCheckStatus["userdata"] = ui->userdata_cb->isChecked();
    FileVariable::apCheckStatus["recovery"] = ui->recovery_cb->isChecked();
    FileVariable::apCheckStatus["persist"] = ui->persist_cb->isChecked();
    FileVariable::apCheckStatus["IPSM"] = ui->ipsm_cb->isChecked();
}

bool MainWindow::CheckAPSelectFiles()
{
    QMap<QString, bool>::Iterator i = FileVariable::apCheckStatus.begin();

    while (i != FileVariable::apCheckStatus.end()) {
        if (i.value()== true) { // If the checkbox of this fils was checked
            if (!FileVariable::CheckAPFile(i.key())) { // Check this file is existent
                if (i.key() == "IPSM") {
                    LogAppend("[" + FileVariable::apFiles.value(i.key()) + "] is non-existent, ignore this file", WARNING);
                    FileVariable::apCheckStatus["IPSM"] = false;
                } else {
                    LogAppend("[" + FileVariable::apFiles.value(i.key()) + "] is non-existent", WARNING);
                    QMessageBox::warning(this,
                                         tr("Warning"),
                                         FileVariable::apFiles.value(i.key()) + " is non-existent, stop update now",
                                         QMessageBox::Ok);
                    return false;
                }
            }
        }
        ++i;
    }

    return true;
}

bool MainWindow::UpdateAPFileList()
{
    QMap<QString, bool>::Iterator i = FileVariable::apCheckStatus.begin();

    updateAPList.clear();

    while (i != FileVariable::apCheckStatus.end()) {
        if (i.value() == true)
            updateAPList.append(i.key());

        ++i;
    }

    return true;
}

void MainWindow::GetMPCheckBoxStatus()
{
    FileVariable::mpCheckStatus["modem"] = ui->modem_cb->isChecked();
    FileVariable::mpCheckStatus["rpm"] = ui->rpm_cb->isChecked();
    FileVariable::mpCheckStatus["sbl1"] = ui->sbl1_cb->isChecked();
    FileVariable::mpCheckStatus["tz"] = ui->tz_cb->isChecked();
}

bool MainWindow::CheckMPSelectFiles()
{
    QMap<QString, bool>::Iterator i = FileVariable::mpCheckStatus.begin();

    while (i != FileVariable::mpCheckStatus.end()) {
        if (i.value()== true) { // If the checkbox of this fils was checked
            if (!FileVariable::CheckMPFile(i.key())) { // Check this file is existent
                LogAppend("[" + FileVariable::mpFiles.value(i.key()) + "] is non-existent", WARNING);
                QMessageBox::warning(this,
                                     tr("Warning"),
                                     FileVariable::mpFiles.value(i.key()) + " is non-existent, stop update now",
                                     QMessageBox::Ok);
                return false;
            }
        }
        ++i;
    }

    return true;
}

bool MainWindow::UpdateMPFileList()
{
    QMap<QString, bool>::Iterator i = FileVariable::mpCheckStatus.begin();

    updateMPList.clear();

    while (i != FileVariable::mpCheckStatus.end()) {
        if (i.value() == true)
            updateMPList.append(i.key());

        ++i;
    }

    return true;
}

void MainWindow::GetOPCheckBoxStatus()
{
    FileVariable::opCheckStatus["autoreboot"] = ui->autoreboot_cb->isChecked();
}

void MainWindow::LogAppend(const QString log, MSG_TYPE type)
{
    QString displayString = "<font ";

    switch (type) {
    case START:
        displayString += "color=#8A2BE2><b>" + log + "</b>";
        break;
    case ERROR:
        displayString += "color=#FF0000>" + log;
        break;
    case WARNING:
        displayString += "color=#FFA500>" + log;
        break;
    case OKAY:
        displayString += "color=#008000>" + log;
        break;
    case DEFAULT:
    default:
        displayString += ">" + log;
        break;
    }

    displayString += "</font>";
    ui->log_tb->append(displayString);
}

void MainWindow::updatingFile(const QString file)
{
    LogAppend("Updating [" + file + "]...");
}

void MainWindow::updateFileDone(const QString file, bool result)
{
    UpdateProgressBar();

    if (result)
        LogAppend("Update [" + file + "] OKAY", OKAY);
    else {
        LogAppend("Update [" + file + "] FAILED", ERROR);

        QMessageBox::warning(this,
                             tr("Error"),
                             "Failed to update " + file + ", stop now",
                             QMessageBox::Ok);
    }
}

void MainWindow::updateComplete(bool result)
{
    ui->start_btn->setEnabled(true);
    ui->onekey_btn->setEnabled(true);

    // Set status of label background
    if (result) {
        SetStatusLabColor(QColor(0x45, 0xb9, 0x7c));
        ui->status_lab->setText(tr("   Okay"));
    } else {
        SetStatusLabColor(Qt::red);
        ui->status_lab->setText(tr("  Failed"));
        ui->progressBar->setStyleSheet("QProgressBar::chunk {background-color: red;} QProgressBar{text-align: center;}");
    }
}

void MainWindow::ResetProgressBar()
{
    this->ui->progressBar->setRange(0, updateThread.GetFilesCount());
    this->ui->progressBar->setValue(0);
    ui->progressBar->setStyleSheet("QProgressBar::chunk {background-color: #45b97c;} QProgressBar{text-align: center;}");
}

void MainWindow::UpdateProgressBar()
{
    this->ui->progressBar->setValue(this->ui->progressBar->value() + 1);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ap_selectall_cb_stateChanged(int arg1)
{
    if (arg1)
        this->SetAPCheckBoxStatus(true);
    else
        this->SetAPCheckBoxStatus(false);
}

void MainWindow::on_ap_folder_btn_clicked()
{
    QString path = QFileDialog::getExistingDirectory(NULL, "", FileVariable::apFiles.value("folder"));
    if (path.isEmpty())
        return;

    ui->ap_folder_le->setText(path);
    ui->onekey_apfolder_le->setText(path);
    FileVariable::apFiles["folder"] = path;
}

void MainWindow::on_boot_le_textChanged(const QString &arg1)
{
    FileVariable::apFiles["boot"] = arg1;
}

void MainWindow::on_persist_le_textChanged(const QString &arg1)
{
    FileVariable::apFiles["persist"] = arg1;
}

void MainWindow::on_system_le_textChanged(const QString &arg1)
{
    FileVariable::apFiles["system"] = arg1;
}

void MainWindow::on_recovery_le_textChanged(const QString &arg1)
{
    FileVariable::apFiles["recovery"] = arg1;
}

void MainWindow::on_cache_le_textChanged(const QString &arg1)
{
    FileVariable::apFiles["cache"] = arg1;
}

void MainWindow::on_splash_le_textChanged(const QString &arg1)
{
    FileVariable::apFiles["splash"] = arg1;
}

void MainWindow::on_ipsm_le_textChanged(const QString &arg1)
{
    FileVariable::apFiles["IPSM"] = arg1;
}

void MainWindow::on_userdata_le_textChanged(const QString &arg1)
{
    FileVariable::apFiles["userdata"] = arg1;
}

void MainWindow::on_appsboot_le_textChanged(const QString &arg1)
{
    FileVariable::apFiles["aboot"] = arg1;
}

void MainWindow::on_start_btn_clicked()
{
    if (!Fastboot::CheckDevice()) {
        QMessageBox::information(this,
                                 tr("Error"),
                                 tr("Not device found, please connect your phone to PC in fastboot mode"),
                                 QMessageBox::Ok);
        return;
    }

    LogAppend("");
    LogAppend(QDate::currentDate().toString() + " " + QTime::currentTime().toString(), START);
    LogAppend("Update Start>>>>>>>>", START);

    // Collect the AP files
    GetAPCheckBoxStatus();

    // Collect the MP files
    GetMPCheckBoxStatus();

    // Collect optional selections
    GetOPCheckBoxStatus();

    // Update configure file
    FileVariable::UpdateConfigureFile();

    // Check files
    if (!CheckAPSelectFiles())
        return;
    if (!CheckMPSelectFiles())
        return;

    UpdateAPFileList();
    UpdateMPFileList();

    // Disable some GUI components
    ui->start_btn->setEnabled(false);
    ui->onekey_btn->setEnabled(false);

    // Set status lab
    SetStatusLabColor(QColor(0xff, 0xc2, 0x0e));
    ui->status_lab->setText(tr(" Waiting"));

    // Start update thread
    updateThread.SetUpdateList(updateAPList, updateMPList);
    ResetProgressBar();
    updateThread.start();
}

void MainWindow::on_mp_folder_btn_clicked()
{
    QString path = QFileDialog::getExistingDirectory(NULL,
                                                     "",
                                                     FileVariable::mpFiles.value("folder"));
    if (path.isEmpty())
        return;

    ui->mp_folder_le->setText(path);
    ui->onekey_mpfolder_le->setText(path);
    FileVariable::mpFiles["folder"] = path;
}

void MainWindow::on_exit_btn_clicked()
{
    updateThread.terminate();
    updateThread.wait();
    QApplication::exit(0);
}

void MainWindow::on_save_log_btn_clicked()
{
    QString filename = QFileDialog::getSaveFileName(this,
                                                    tr("Save log"),
                                                    "",
                                                    tr("*.txt"));
    if (filename.isEmpty())
        return;

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly | QFile::Truncate))
        return;

    file.write(ui->log_tb->toPlainText().toLatin1());

    file.close();
}

void MainWindow::on_mp_select_all_cb_stateChanged(int arg1)
{
    if (arg1)
        this->SetMPCheckBoxStatus(true);
    else
        this->SetMPCheckBoxStatus(false);
}

void MainWindow::on_develop_btn_clicked()
{
    ui->onekey_widget->setVisible(false);
    ui->developer_widget->setVisible(true);
}

void MainWindow::on_onekeymode_btn_clicked()
{
    ui->onekey_widget->setVisible(true);
    ui->developer_widget->setVisible(false);
}

void MainWindow::on_onekey_btn_clicked()
{
    if (!Fastboot::CheckDevice()) {
        QMessageBox::information(this,
                                 tr("Error"),
                                 tr("Not device found, please connect your phone to PC in fastboot mode"),
                                 QMessageBox::Ok);
        return;
    }

    LogAppend("");
    LogAppend(QDate::currentDate().toString() + " " + QTime::currentTime().toString(), START);
    LogAppend("Update Start>>>>>>>>", START);

    FileVariable::SetDefaultCheck();

    // Update configure file
    FileVariable::UpdateConfigureFile();

    if (!CheckAPSelectFiles())
        return;
    if (!CheckMPSelectFiles())
        return;

    UpdateAPFileList();
    UpdateMPFileList();

    ui->start_btn->setEnabled(false);
    ui->onekey_btn->setEnabled(false);

    // Set status lab
    SetStatusLabColor(QColor(0xff, 0xc2, 0x0e));
    ui->status_lab->setText(tr(" Waiting"));

    // Start update thread
    updateThread.SetUpdateList(updateAPList, updateMPList);
    ResetProgressBar();
    updateThread.start();
}

void MainWindow::on_onekey_ap_browse_btn_clicked()
{
    QString path = QFileDialog::getExistingDirectory(NULL, "", FileVariable::apFiles.value("folder"));
    if (path.isEmpty())
        return;

    ui->ap_folder_le->setText(path);
    ui->onekey_apfolder_le->setText(path);
    FileVariable::apFiles["folder"] = path;
}

void MainWindow::on_onekey_mp_browse_btn_clicked()
{
    QString path = QFileDialog::getExistingDirectory(NULL, "", FileVariable::mpFiles.value("folder"));
    if (path.isEmpty())
        return;

    ui->mp_folder_le->setText(path);
    ui->onekey_mpfolder_le->setText(path);
    FileVariable::mpFiles["folder"] = path;
}

void MainWindow::on_actionUser_Guid_triggered()
{
    QDesktopServices::openUrl(QUrl(tr("Honeywell-Fastboot_UserGuide.pdf")));
}
