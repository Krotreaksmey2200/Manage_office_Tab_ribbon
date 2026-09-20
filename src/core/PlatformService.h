#ifndef PLATFORM_SERVICE_H
#define PLATFORM_SERVICE_H

#include <QString>
#include <QStringList>
#include <memory>

class IPlatformService {
public:
    virtual ~IPlatformService() = default;

    virtual QString getPlatformName() const = 0;
    virtual QString getWordStartupPath() const = 0;
    virtual QString getSystemWordStartupPath() const = 0;
    virtual QString getPowerPointAddInsPath() const = 0;
    virtual QString getWordOfficeUIPath() const = 0;
    virtual QString getPowerPointOfficeUIPath() const = 0;
    virtual QString getBackupDirectory() const = 0;
    virtual QString getWebExtensionPath(const QString& appName) const = 0;

    virtual bool isOfficeAppInstalled(const QString& appName) const = 0;
    virtual QStringList getOfficeInstallLocations() const = 0;

    static std::shared_ptr<IPlatformService> create();
};

#endif // PLATFORM_SERVICE_H
