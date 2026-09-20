#include "MacPlatformService.h"
#include <QDir>
#include <QStandardPaths>
#include <QFileInfo>

MacPlatformService::MacPlatformService() {
    m_homeDir = QDir::homePath();
    m_groupContainerDir = m_homeDir + "/Library/Group Containers/UBF8T346G9.Office";
}

QString MacPlatformService::getPlatformName() const {
    return "macOS (Apple Silicon / Intel)";
}

QString MacPlatformService::getWordStartupPath() const {
    return m_groupContainerDir + "/User Content.localized/Startup.localized/Word";
}

QString MacPlatformService::getSystemWordStartupPath() const {
    return "/Library/Application Support/Microsoft/Office365/User Content.localized/Startup.localized/Word";
}

QString MacPlatformService::getPowerPointAddInsPath() const {
    // Both Add-Ins.localized and Startup.localized/PowerPoint are used by Mac PowerPoint
    QString addins = m_groupContainerDir + "/User Content.localized/Add-Ins.localized";
    QDir dir(addins);
    if (dir.exists()) {
        return addins;
    }
    return m_groupContainerDir + "/User Content.localized/Startup.localized/PowerPoint";
}

QString MacPlatformService::getWordOfficeUIPath() const {
    return m_homeDir + "/Library/Containers/com.microsoft.Word/Data/Library/Preferences/Word.officeUI";
}

QString MacPlatformService::getPowerPointOfficeUIPath() const {
    return m_homeDir + "/Library/Containers/com.microsoft.Powerpoint/Data/Library/Preferences/PowerPoint.officeUI";
}

QString MacPlatformService::getBackupDirectory() const {
    QString backupDir = m_homeDir + "/Documents/OfficeAddinBackups";
    QDir dir;
    if (!dir.exists(backupDir)) {
        dir.mkpath(backupDir);
    }
    return backupDir;
}

QString MacPlatformService::getWebExtensionPath(const QString& appName) const {
    QString bundleId = (appName.toLower() == "powerpoint") ? "com.microsoft.Powerpoint" : "com.microsoft.Word";
    return m_homeDir + "/Library/Containers/" + bundleId + "/Data/Documents/wef";
}

bool MacPlatformService::isOfficeAppInstalled(const QString& appName) const {
    QString path = "/Applications/Microsoft " + appName + ".app";
    return QFileInfo::exists(path);
}

QStringList MacPlatformService::getOfficeInstallLocations() const {
    QStringList locations;
    if (QFileInfo::exists("/Applications/Microsoft Word.app")) {
        locations.append("Microsoft Word (/Applications/Microsoft Word.app)");
    }
    if (QFileInfo::exists("/Applications/Microsoft PowerPoint.app")) {
        locations.append("Microsoft PowerPoint (/Applications/Microsoft PowerPoint.app)");
    }
    if (QFileInfo::exists("/Applications/Microsoft Excel.app")) {
        locations.append("Microsoft Excel (/Applications/Microsoft Excel.app)");
    }
    return locations;
}
