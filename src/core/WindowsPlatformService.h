#ifndef WINDOWS_PLATFORM_SERVICE_H
#define WINDOWS_PLATFORM_SERVICE_H

#include "PlatformService.h"

class WindowsPlatformService : public IPlatformService {
public:
    WindowsPlatformService();
    ~WindowsPlatformService() override = default;

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
    QString m_appDataRoaming;
    QString m_localAppData;
    QString m_userProfile;
};

#endif // WINDOWS_PLATFORM_SERVICE_H
