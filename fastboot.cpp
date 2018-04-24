#include "fastboot.h"

#include <QProcess>
#include <QFile>

Fastboot::Fastboot()
{

}

bool Fastboot::UpdateFile(const QString partition, const QString file)
{
    QProcess fbProcess;
    QFile logFile("fastboot.log");
    QString cmd = "fastboot flash " + partition + " " + file;
/*
    if (QProcess::execute(cmd) != 0)
        return false;
*/
    fbProcess.start(cmd);
    fbProcess.waitForFinished(-1);

    QString result = fbProcess.readAllStandardError();
    if (logFile.open(QIODevice::WriteOnly | QFile::Append)) {
        logFile.write(cmd.toLatin1() + "\r\n");
        logFile.write(result.toLatin1());
        logFile.close();
    }

    if (fbProcess.exitCode() != 0)
        return false;

    return true;
}

bool Fastboot::FBReboot()
{
    QString cmd = "fastboot reboot";

    if (QProcess::execute(cmd) != 0)
        return false;

    return true;
}

bool Fastboot::CheckDevice()
{
    QString result;
    QProcess fbProcess;

    fbProcess.start("fastboot devices");
    fbProcess.waitForFinished();

    result = fbProcess.readAllStandardOutput();
    if (result.length() > 0)
        return true;

    return false;
}
