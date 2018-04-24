#ifndef UPDATETHREAD_H
#define UPDATETHREAD_H

#include <QStringList>
#include <QThread>

class UpdateThread : public QThread
{
    Q_OBJECT
public:
    UpdateThread();
    UpdateThread(const QStringList updateAPList,
                 const QStringList updateMPList);
    void SetUpdateList(const QStringList updateAPList,
                       const QStringList updateMPList);
    int GetFilesCount();

private:
    void run();

    QStringList apList;
    QStringList mpList;

signals:
    void updatingFile(const QString file);
    void updateFileDone(const QString file, bool result);
    void updateComplete(bool result);
};

#endif // UPDATETHREAD_H
