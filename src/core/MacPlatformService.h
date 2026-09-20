#ifndef MAC_PLATFORM_SERVICE_H
#define MAC_PLATFORM_SERVICE_H

#include "PlatformService.h"

class MacPlatformService : public IPlatformService {
public:
    MacPlatformService();
    ~MacPlatformService() override = default;

    QString getPlatformName() const override;
    QString getWordStartupPath() const override;
    QString getSystemWordStartupPath() const override;
    QString getPowerPointAddInsPath() const override;
    QString getWordOfficeUIPath() const override;
    QString getPowerPointOfficeUIPath() const override;
    QString getBackupDirectory() const override;
    QString getWebExtensionPath(const QString& appName) const override;

    bool isOfficeAppInstalled(const QString& appName) const override;
    QStringList getOfficeInstallLocations() const override;

private:
    QString m_homeDir;
    QString m_groupContainerDir;
};

#endif // MAC_PLATFORM_SERVICE_H
