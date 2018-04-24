#ifndef FASTBOOT_H
#define FASTBOOT_H

#include <QMap>

class Fastboot
{
public:
    Fastboot();

    bool UpdateFile(const QString partition, const QString file);
    bool FBReboot();

    static bool CheckDevice();

};

#endif // FASTBOOT_H
