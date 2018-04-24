#include "updatethread.h"
#include "fastboot.h"
#include "filevariable.h"

#include <QList>
#include <QFile>

UpdateThread::UpdateThread()
{
}

UpdateThread::UpdateThread(const QStringList updateAPList,
                           const QStringList updateMPList)
    : apList(updateAPList), mpList(updateMPList)
{

}

void UpdateThread::SetUpdateList(const QStringList updateAPList,
                                 const QStringList updateMPList)
{
    this->apList = updateAPList;
    this->mpList = updateMPList;
}

int UpdateThread::GetFilesCount()
{
    return (apList.count() + mpList.count());
}

void UpdateThread::run()
{
    Fastboot fb;
    QList<QString>::Iterator it;
    bool result = true;

    QFile::remove(tr("fastboot.log"));

    // Update AP files
    it = apList.begin();
    for ( ; it != apList.end(); ++it) {
        emit updatingFile(FileVariable::apFiles.value(*it));
        result = fb.UpdateFile(*it, "\"" + FileVariable::apFiles.value("folder") + "/" + FileVariable::apFiles.value(*it) + "\"");
        emit updateFileDone(FileVariable::apFiles.value(*it), result);
        if (!result)
            break;
    }

    // Update MP files
    if (result) {
        it = mpList.begin();
        for ( ; it != mpList.end(); ++it) {
            emit updatingFile(FileVariable::mpFiles.value(*it));
            result = fb.UpdateFile(*it, "\"" + FileVariable::mpFiles.value("folder") + "/" + FileVariable::mpFiles.value(*it) + "\"");
            emit updateFileDone(FileVariable::mpFiles.value(*it), result);
            if (!result)
                break;
        }
    }

    emit updateComplete(result);

    // Auto reboot
    if (result)
        if (FileVariable::opCheckStatus.value("autoreboot"))
            fb.FBReboot();
}
