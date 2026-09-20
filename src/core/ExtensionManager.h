#ifndef EXTENSION_MANAGER_H
#define EXTENSION_MANAGER_H

#include <QObject>
#include <QList>
#include <memory>
#include "ExtensionItem.h"
#include "PlatformService.h"

class ExtensionManager : public QObject {
    Q_OBJECT
public:
    explicit ExtensionManager(std::shared_ptr<IPlatformService> platformService, QObject* parent = nullptr);

    // Refresh & scan extensions for Word & PowerPoint
    QList<ExtensionItem> scanWordExtensions();
    QList<ExtensionItem> scanPowerPointExtensions();

    // Enable / Disable toggles
    bool toggleExtensionState(ExtensionItem& item, QString& errorMessage);
    bool enableExtension(ExtensionItem& item, QString& errorMessage);
    bool disableExtension(ExtensionItem& item, QString& errorMessage);

    // Install / Uninstall
    bool installExtension(const QString& sourceFilePath, const QString& appName, QString& errorMessage);
    bool uninstallExtension(const ExtensionItem& item, QString& errorMessage);

    // Backup & Restore
    QString backupExtension(const ExtensionItem& item, QString& errorMessage);
    bool restoreExtension(const QString& backupFilePath, const QString& appName, QString& errorMessage);

    // Ribbon XML Extraction and Updating
    QString getRibbonXml(const ExtensionItem& item);
    bool saveRibbonXml(ExtensionItem& item, const QString& newXml, QString& errorMessage);

    std::shared_ptr<IPlatformService> platformService() const { return m_platformService; }

signals:
    void extensionsChanged();
    void logMessage(const QString& msg, bool isError = false);

private:
    QList<ExtensionItem> scanDirectory(const QString& dirPath, const QString& appName, const QString& typeDesc);
    std::shared_ptr<IPlatformService> m_platformService;
};

#endif // EXTENSION_MANAGER_H
