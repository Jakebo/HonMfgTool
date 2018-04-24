#include "filevariable.h"

#include <QFileInfo>

QMap<QString, QString> FileVariable::apFiles;
QMap<QString, QString> FileVariable::mpFiles;
QMap<QString, bool> FileVariable::apCheckStatus;
QMap<QString, bool> FileVariable::mpCheckStatus;
QMap<QString, bool> FileVariable::opCheckStatus;
const QString FileVariable::APSECTION = "[ap section]";
const QString FileVariable::MPSECTION = "[mp section]";
const QString FileVariable::OPSECTION = "[op section]";
const QString FileVariable::configureFile = "cfg.ini";

FileVariable::FileVariable()
{

}

void FileVariable::SetDefaultCheck()
{
    // AP
    apCheckStatus["boot"] = true;
    apCheckStatus["system"] = true;
    apCheckStatus["cache"] = true;
    apCheckStatus["splash"] = true;
    apCheckStatus["persist"] = true;
    apCheckStatus["IPSM"] = true;
    apCheckStatus["aboot"] = true;
    apCheckStatus["userdata"] = true;
    apCheckStatus["recovery"] = true;

    // MP
    mpCheckStatus["modem"] = true;
    mpCheckStatus["rpm"] = true;
    mpCheckStatus["sbl1"] = true;
    mpCheckStatus["dbi"] = false;
    mpCheckStatus["tz"] = true;

    opCheckStatus["autoreboot"] = true;
}

void FileVariable::InitFileVariable()
{
    /*
     * Set the initialize values
     */
    // AP
    apFiles.insert("boot", "boot.img");
    apFiles.insert("system", "system.img");
    apFiles.insert("cache", "cache.img");
    apFiles.insert("persist", "persist.img");
    apFiles.insert("IPSM", "IPSM.img");
    apFiles.insert("aboot", "emmc_appsboot.mbn");
    apFiles.insert("userdata", "userdata.img");
    apFiles.insert("recovery", "recovery.img");
    apFiles.insert("splash", "splash.img");

    // MP
    mpFiles.insert("modem", "NON-HLOS.bin");
    mpFiles.insert("rpm", "rpm.mbn");
    mpFiles.insert("sbl1", "sbl1.mbn");
    mpFiles.insert("dbi", "sdi.mbn");
    mpFiles.insert("tz", "tz.mbn");

    LoadConfigure(configureFile);
}

void FileVariable::LoadConfigure(const QString filePath)
{
    LoadSectionConfigure(filePath, APSECTION);
    LoadSectionConfigure(filePath, MPSECTION);
    LoadSectionConfigure(filePath, OPSECTION);
}

void FileVariable::LoadSectionConfigure(const QString filePath,
                                        const QString section)
{
    QFile file(filePath);
    QByteArray line;

    if (!file.open(QFile::ReadOnly))
        return;

    while (!file.atEnd()) {
        line = file.readLine();
        QString lineStr(line);
        if (lineStr.mid(0, section.length()).compare(section) || lineStr.at(0) == ';')
            continue;

        while (!file.atEnd()) {
            line = file.readLine();
            QString lineStr(line);

            if (lineStr.at(0) == ';')
                continue;
            if (lineStr.at(0) == '[')
                break;
            ParseLine(lineStr, section);
        }
        break;
    }

    file.close();
}

void FileVariable::ParseLine(const QString line, const QString section)
{
    if (line.isEmpty())
        return;

    QStringList keyValue = line.split("=");
    if (keyValue.count() < 2)
        return;

    QStringList nameChecked = QString(keyValue.at(1)).split(", ");

    if (section == APSECTION) {
        apFiles[keyValue.at(0)] = nameChecked.at(0);
        if (nameChecked.count() > 1)
            apCheckStatus[keyValue.at(0)] = QString(nameChecked.at(1)).at(0) == '1' ? true : false;
    } else if (section == MPSECTION) {
        mpFiles[keyValue.at(0)] = nameChecked.at(0);
        if (nameChecked.count() > 1)
            mpCheckStatus[keyValue.at(0)] = QString(nameChecked.at(1)).at(0) == '1' ? true : false;
    } else if (section == OPSECTION) {
        opCheckStatus[keyValue.at(0)] = QString(keyValue.at(1)).at(0) == '1' ? true : false;
    }
}

void FileVariable::UpdateConfigureFile()
{
    QFile file(configureFile);
    QString line;

    if (!file.open(QIODevice::WriteOnly | QFile::Truncate))
        return;

    // AP
    QMap<QString, QString>::Iterator it = apFiles.begin();
    QString checked;

    file.write(QString(APSECTION + "\r\n").toLatin1());
    while (it != apFiles.end()) {
        line = it.key() + "=" + it.value() + ", " +
                (apCheckStatus.value(it.key()) ? "1" : "0") + "\r\n";
        file.write(line.toLatin1());
        ++it;
    }

    // MP
    it = mpFiles.begin();

    file.write(QString(MPSECTION + "\r\n").toLatin1());
    while (it != mpFiles.end()) {
        line = it.key() + "=" + it.value() + ", " +
                (mpCheckStatus.value(it.key()) ? "1" : "0") + "\r\n";
        file.write(line.toLatin1());
        ++it;
    }

    // Optional selection
    QMap<QString, bool>::Iterator opIt = opCheckStatus.begin();

    file.write(QString(OPSECTION + "\r\n").toLatin1());
    while (opIt != opCheckStatus.end()) {
        line = opIt.key() + "=" + (opIt.value() ? "1" : "0") + "\r\n";
        file.write(line.toLatin1());
        ++opIt;
    }

    file.close();
}

bool FileVariable::CheckAPFile(const QString fileName)
{
    QFileInfo file(apFiles.value("folder") + "/" + apFiles.value(fileName));
    return file.exists();
}

bool FileVariable::CheckMPFile(const QString fileName)
{
    QFileInfo file(mpFiles.value("folder") + "/" + mpFiles.value(fileName));
    return file.exists();
}
