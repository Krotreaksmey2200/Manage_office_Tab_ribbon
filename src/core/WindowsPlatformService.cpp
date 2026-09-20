#include "WindowsPlatformService.h"
#include <QDir>
#include <QProcessEnvironment>
#include <QFileInfo>

WindowsPlatformService::WindowsPlatformService() {
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    m_appDataRoaming = env.value("APPDATA");
    m_localAppData = env.value("LOCALAPPDATA");
    m_userProfile = env.value("USERPROFILE");
}

QString WindowsPlatformService::getPlatformName() const {
    return "Windows (x86_64 / ARM64)";
}

QString WindowsPlatformService::getWordStartupPath() const {
    return m_appDataRoaming + "/Microsoft/Word/STARTUP";
}

QString WindowsPlatformService::getSystemWordStartupPath() const {
    return QString();
}

QString WindowsPlatformService::getPowerPointAddInsPath() const {
    return m_appDataRoaming + "/Microsoft/AddIns";
}

QString WindowsPlatformService::getWordOfficeUIPath() const {
    return m_localAppData + "/Microsoft/Office/Word.officeUI";
}

QString WindowsPlatformService::getPowerPointOfficeUIPath() const {
    return m_localAppData + "/Microsoft/Office/PowerPoint.officeUI";
}

QString WindowsPlatformService::getBackupDirectory() const {
    QString backupDir = m_userProfile + "/Documents/OfficeAddinBackups";
    QDir dir;
    if (!dir.exists(backupDir)) {
        dir.mkpath(backupDir);
    }
    return backupDir;
}

QString WindowsPlatformService::getWebExtensionPath(const QString& appName) const {
    Q_UNUSED(appName);
    return m_localAppData + "/Microsoft/Office/16.0/Wef";
}

bool WindowsPlatformService::isOfficeAppInstalled(const QString& appName) const {
    Q_UNUSED(appName);
    // On Windows, can check Program Files / Office Click-to-run
    return true;
}

QStringList WindowsPlatformService::getOfficeInstallLocations() const {
    QStringList locations;
    locations.append("C:/Program Files/Microsoft Office/root/Office16");
    locations.append("C:/Program Files (x86)/Microsoft Office/root/Office16");
    return locations;
}
