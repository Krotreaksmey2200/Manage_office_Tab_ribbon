#ifndef EXTENSION_ITEM_H
#define EXTENSION_ITEM_H

#include <QString>
#include <QDateTime>

struct ExtensionItem {
    QString fileName;           // e.g., "Mathtype-kh.dotm"
    QString fullPath;           // Absolute path on disk
    QString appName;            // "Word" or "PowerPoint"
    QString typeDescription;    // "Startup Template (.dotm)", "Add-in (.ppam)", "OfficeUI"
    bool enabled = true;        // True if active, false if disabled (.disabled suffix)
    qint64 fileSizeBytes = 0;   // File size in bytes
    QDateTime lastModified;     // Last modified timestamp
    bool hasCustomRibbonXml = false; // True if customUI14.xml or customUI.xml was found
    QString cachedRibbonXml;    // Extracted Ribbon XML content
};

#endif // EXTENSION_ITEM_H
