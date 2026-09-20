#include "ExtensionManager.h"
#include "ZipHandler.h"
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QProcess>

ExtensionManager::ExtensionManager(std::shared_ptr<IPlatformService> platformService, QObject* parent)
    : QObject(parent), m_platformService(platformService) {
}

QList<ExtensionItem> ExtensionManager::scanWordExtensions() {
    QList<ExtensionItem> result;
    if (!m_platformService) return result;

    // 1. User Startup folder (.dotm, .dotx, .wll)
    QString startupPath = m_platformService->getWordStartupPath();
    result.append(scanDirectory(startupPath, "Word", "User Startup Template (.dotm)"));

    // 2. System / Global Startup folder (WordMat, MathCapture, etc.)
    QString sysStartupPath = m_platformService->getSystemWordStartupPath();
    if (!sysStartupPath.isEmpty() && QDir(sysStartupPath).exists()) {
        result.append(scanDirectory(sysStartupPath, "Word", "System Startup Template (.dotm)"));
    }

    // 3. Check Word.officeUI if exists
    QString officeUIPath = m_platformService->getWordOfficeUIPath();
    if (QFile::exists(officeUIPath)) {
        QFileInfo fi(officeUIPath);
        ExtensionItem item;
        item.fileName = fi.fileName();
        item.fullPath = fi.absoluteFilePath();
        item.appName = "Word";
        item.typeDescription = "Custom Ribbon UI (.officeUI)";
        item.enabled = true;
        item.fileSizeBytes = fi.size();
        item.lastModified = fi.lastModified();
        item.hasCustomRibbonXml = true;
        item.cachedRibbonXml = ZipHandler::extractCustomRibbonXml(item.fullPath);
        result.append(item);
    }

    return result;
}

QList<ExtensionItem> ExtensionManager::scanPowerPointExtensions() {
    QList<ExtensionItem> result;
    if (!m_platformService) return result;

    // 1. Add-Ins folder (.ppam, .ppa)
    QString addInsPath = m_platformService->getPowerPointAddInsPath();
    result.append(scanDirectory(addInsPath, "PowerPoint", "Add-in (.ppam)"));

    // 2. Check PowerPoint.officeUI if exists
    QString officeUIPath = m_platformService->getPowerPointOfficeUIPath();
    if (QFile::exists(officeUIPath)) {
        QFileInfo fi(officeUIPath);
        ExtensionItem item;
        item.fileName = fi.fileName();
        item.fullPath = fi.absoluteFilePath();
        item.appName = "PowerPoint";
        item.typeDescription = "Custom Ribbon UI (.officeUI)";
        item.enabled = true;
        item.fileSizeBytes = fi.size();
        item.lastModified = fi.lastModified();
        item.hasCustomRibbonXml = true;
        item.cachedRibbonXml = ZipHandler::extractCustomRibbonXml(item.fullPath);
        result.append(item);
    }

    return result;
}

QList<ExtensionItem> ExtensionManager::scanDirectory(const QString& dirPath, const QString& appName, const QString& typeDesc) {
    QList<ExtensionItem> result;
    QDir dir(dirPath);
    if (!dir.exists()) {
        return result;
    }

    // Filter files: .dotm, .ppam, .ppa, .wll, .dotx, including .disabled variants
    QFileInfoList entries = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot | QDir::Hidden);
    for (const QFileInfo& fi : entries) {
        QString name = fi.fileName();
        // Ignore Office temporary lock files starting with ~$
        if (name.startsWith("~$") || name == ".DS_Store") {
            continue;
        }

        bool isExtension = false;
        bool isEnabled = true;

        if (name.endsWith(".dotm", Qt::CaseInsensitive) ||
            name.endsWith(".ppam", Qt::CaseInsensitive) ||
            name.endsWith(".ppa", Qt::CaseInsensitive) ||
            name.endsWith(".wll", Qt::CaseInsensitive) ||
            name.endsWith(".dotx", Qt::CaseInsensitive)) {
            isExtension = true;
            isEnabled = true;
        } else if (name.endsWith(".disabled", Qt::CaseInsensitive)) {
            isExtension = true;
            isEnabled = false;
        }

        if (isExtension) {
            ExtensionItem item;
            item.fileName = name;
            item.fullPath = fi.absoluteFilePath();
            item.appName = appName;
            item.typeDescription = typeDesc;
            item.enabled = isEnabled;
            item.fileSizeBytes = fi.size();
            item.lastModified = fi.lastModified();
            item.hasCustomRibbonXml = ZipHandler::hasCustomRibbonXml(item.fullPath);
            if (item.hasCustomRibbonXml) {
                item.cachedRibbonXml = ZipHandler::extractCustomRibbonXml(item.fullPath);
            }
            result.append(item);
        }
    }
    return result;
}

bool ExtensionManager::toggleExtensionState(ExtensionItem& item, QString& errorMessage) {
    if (item.enabled) {
        return disableExtension(item, errorMessage);
    } else {
        return enableExtension(item, errorMessage);
    }
}

bool ExtensionManager::enableExtension(ExtensionItem& item, QString& errorMessage) {
    if (item.enabled) return true;

    if (!item.fullPath.endsWith(".disabled", Qt::CaseInsensitive)) {
        errorMessage = "File does not have .disabled suffix";
        return false;
    }

    QString newPath = item.fullPath;
    newPath.chop(QString(".disabled").length());

    if (QFile::exists(newPath)) {
        errorMessage = "A file with name " + QFileInfo(newPath).fileName() + " already exists!";
        return false;
    }

    if (!QFile::rename(item.fullPath, newPath)) {
#ifdef Q_OS_MACOS
        QString srcEscaped = item.fullPath;
        srcEscaped.replace("\"", "\\\"");
        QString dstEscaped = newPath;
        dstEscaped.replace("\"", "\\\"");
        QString script = QString("do shell script \"mv \\\"%1\\\" \\\"%2\\\"\" with administrator privileges").arg(srcEscaped, dstEscaped);
        int exitCode = QProcess::execute("/usr/bin/osascript", QStringList() << "-e" << script);
        if (exitCode != 0 || !QFile::exists(newPath)) {
            errorMessage = "Failed to rename file to enable add-in (Admin authorization required): " + item.fullPath;
            return false;
        }
#else
        errorMessage = "Failed to rename file to enable add-in";
        return false;
#endif
    }

    item.fullPath = newPath;
    item.fileName = QFileInfo(newPath).fileName();
    item.enabled = true;
    emit extensionsChanged();
    emit logMessage("Enabled add-in: " + item.fileName);
    return true;
}

bool ExtensionManager::disableExtension(ExtensionItem& item, QString& errorMessage) {
    if (!item.enabled) return true;

    QString newPath = item.fullPath + ".disabled";
    if (QFile::exists(newPath)) {
        errorMessage = "Disabled file already exists: " + newPath;
        return false;
    }

    if (!QFile::rename(item.fullPath, newPath)) {
#ifdef Q_OS_MACOS
        QString srcEscaped = item.fullPath;
        srcEscaped.replace("\"", "\\\"");
        QString dstEscaped = newPath;
        dstEscaped.replace("\"", "\\\"");
        QString script = QString("do shell script \"mv \\\"%1\\\" \\\"%2\\\"\" with administrator privileges").arg(srcEscaped, dstEscaped);
        int exitCode = QProcess::execute("/usr/bin/osascript", QStringList() << "-e" << script);
        if (exitCode != 0 || !QFile::exists(newPath)) {
            errorMessage = "Failed to rename file to disable add-in (Admin authorization required): " + item.fullPath;
            return false;
        }
#else
        errorMessage = "Failed to rename file to disable add-in";
        return false;
#endif
    }

    item.fullPath = newPath;
    item.fileName = QFileInfo(newPath).fileName();
    item.enabled = false;
    emit extensionsChanged();
    emit logMessage("Disabled add-in: " + item.fileName);
    return true;
}

bool ExtensionManager::installExtension(const QString& sourceFilePath, const QString& appName, QString& errorMessage) {
    if (!QFile::exists(sourceFilePath)) {
        errorMessage = "Source file does not exist: " + sourceFilePath;
        return false;
    }

    QFileInfo fi(sourceFilePath);
    QString targetDir = (appName.toLower() == "powerpoint") 
        ? m_platformService->getPowerPointAddInsPath() 
        : m_platformService->getWordStartupPath();

    QDir dir(targetDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString destination = targetDir + "/" + fi.fileName();
    if (QFile::exists(destination)) {
        errorMessage = "Add-in already exists in destination: " + destination;
        return false;
    }

    if (!QFile::copy(sourceFilePath, destination)) {
        errorMessage = "Failed to copy file to: " + destination;
        return false;
    }

    emit extensionsChanged();
    emit logMessage("Installed new add-in: " + fi.fileName() + " to " + appName);
    return true;
}

bool ExtensionManager::uninstallExtension(const ExtensionItem& item, QString& errorMessage) {
    if (!QFile::exists(item.fullPath)) {
        errorMessage = "File not found: " + item.fullPath;
        return false;
    }

    // Always create a backup before removing
    backupExtension(item, errorMessage);

    if (!QFile::remove(item.fullPath)) {
#ifdef Q_OS_MACOS
        QString escapedPath = item.fullPath;
        escapedPath.replace("\"", "\\\"");
        QString script = QString("do shell script \"rm -f \\\"%1\\\"\" with administrator privileges").arg(escapedPath);
        int exitCode = QProcess::execute("/usr/bin/osascript", QStringList() << "-e" << script);
        if (exitCode != 0 || QFile::exists(item.fullPath)) {
            errorMessage = "មិនអាចលុបឯកសារបានទេ (ត្រូវការសិទ្ធិ Administrator ឬត្រូវបានបដិសេធ): " + item.fullPath;
            return false;
        }
#else
        errorMessage = "Failed to remove file: " + item.fullPath;
        return false;
#endif
    }

    emit extensionsChanged();
    emit logMessage("Uninstalled add-in: " + item.fileName);
    return true;
}

QString ExtensionManager::backupExtension(const ExtensionItem& item, QString& errorMessage) {
    QString backupDir = m_platformService->getBackupDirectory();
    QDir dir(backupDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss");
    QString backupFileName = item.appName + "_" + timestamp + "_" + item.fileName;
    QString backupPath = backupDir + "/" + backupFileName;

    if (!QFile::copy(item.fullPath, backupPath)) {
        errorMessage = "Failed to create backup at: " + backupPath;
        return QString();
    }

    emit logMessage("Created backup: " + backupFileName);
    return backupPath;
}

bool ExtensionManager::restoreExtension(const QString& backupFilePath, const QString& appName, QString& errorMessage) {
    if (!QFile::exists(backupFilePath)) {
        errorMessage = "Backup file not found: " + backupFilePath;
        return false;
    }
    return installExtension(backupFilePath, appName, errorMessage);
}

QString ExtensionManager::getRibbonXml(const ExtensionItem& item) {
    return ZipHandler::extractCustomRibbonXml(item.fullPath);
}

bool ExtensionManager::saveRibbonXml(ExtensionItem& item, const QString& newXml, QString& errorMessage) {
    // Backup original first
    backupExtension(item, errorMessage);

    bool success = ZipHandler::updateCustomRibbonXml(item.fullPath, newXml, errorMessage);
    if (success) {
        item.cachedRibbonXml = newXml;
        item.hasCustomRibbonXml = true;
        emit logMessage("Successfully updated Ribbon XML for: " + item.fileName);
    }
    return success;
}
