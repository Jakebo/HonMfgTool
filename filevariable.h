#ifndef FILEVARIABLE_H
#define FILEVARIABLE_H

#include <QString>
#include <QMap>

class FileVariable
{
public:
    FileVariable();

    // AP
    static QString apFolder;
    static QMap<QString, QString> apFiles;
    static QMap<QString, bool> apCheckStatus;

    // MP
    static QString mpFolder;
    static QMap<QString, QString> mpFiles;
    static QMap<QString, bool> mpCheckStatus;

    // Optional selection
    static QMap<QString, bool> opCheckStatus;

    static bool CheckAPFile(const QString fileName);
    static bool CheckMPFile(const QString fileName);
    static void InitFileVariable();
    static void SetDefaultCheck();
    static void UpdateConfigureFile();

private:
    static void LoadConfigure(const QString filePath);
    static void LoadSectionConfigure(const QString filePath,
                                     const QString section);
    static void ParseLine(const QString line, const QString section);

    static const QString configureFile;
    static const QString APSECTION;
    static const QString MPSECTION;
    static const QString OPSECTION;
};

#endif // FILEVARIABLE_H
