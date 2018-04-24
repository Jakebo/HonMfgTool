#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>
#include "updatethread.h"

namespace Ui {
class MainWindow;
}

typedef enum {
    DEFAULT,
    START,
    ERROR,
    WARNING,
    OKAY
} MSG_TYPE;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_ap_selectall_cb_stateChanged(int arg1);

    void on_ap_folder_btn_clicked();

    void on_boot_le_textChanged(const QString &arg1);

    void on_persist_le_textChanged(const QString &arg1);

    void on_system_le_textChanged(const QString &arg1);

    void on_recovery_le_textChanged(const QString &arg1);

    void on_cache_le_textChanged(const QString &arg1);

    void on_splash_le_textChanged(const QString &arg1);

    void on_ipsm_le_textChanged(const QString &arg1);

    void on_userdata_le_textChanged(const QString &arg1);

    void on_appsboot_le_textChanged(const QString &arg1);

    void on_start_btn_clicked();

private:
    void SetAPCheckBoxStatus(bool check);
    void SetMPCheckBoxStatus(bool check);
    void DisplayDefValue();
    void SetDefaultCheck();
    void GetAPCheckBoxStatus();
    bool CheckAPSelectFiles();
    bool UpdateAPFileList();
    void GetMPCheckBoxStatus();
    bool CheckMPSelectFiles();
    bool UpdateMPFileList();
    void GetOPCheckBoxStatus();
    void LogAppend(const QString log, MSG_TYPE type = DEFAULT);
    void SetStatusLabColor(QColor color);
    void ResetProgressBar();
    void UpdateProgressBar();

private:
    Ui::MainWindow *ui;
    QStringList updateAPList;
    QStringList updateMPList;
    UpdateThread updateThread;

private slots:
    void updatingFile(const QString file);
    void updateFileDone(const QString file, bool result);
    void updateComplete(bool result);
    void on_mp_folder_btn_clicked();
    void on_exit_btn_clicked();
    void on_save_log_btn_clicked();
    void on_mp_select_all_cb_stateChanged(int arg1);
    void on_develop_btn_clicked();
    void on_onekeymode_btn_clicked();
    void on_onekey_btn_clicked();
    void on_onekey_ap_browse_btn_clicked();
    void on_onekey_mp_browse_btn_clicked();
    void on_actionUser_Guid_triggered();
};

#endif // MAINWINDOW_H
