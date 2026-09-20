#ifndef ZIP_HANDLER_H
#define ZIP_HANDLER_H

#include <QString>
#include <QStringList>

class ZipHandler {
public:
    // Extract XML from .dotm / .ppam file
    static QString extractCustomRibbonXml(const QString& archivePath);

    // List all files in .dotm / .ppam
    static QStringList listArchiveContents(const QString& archivePath);

    // Inject or update Ribbon XML in .dotm / .ppam
    static bool updateCustomRibbonXml(const QString& archivePath, const QString& newXmlContent, QString& errorMessage);

    // Check if archive contains customUI
    static bool hasCustomRibbonXml(const QString& archivePath);

private:
    static QString runProcess(const QString& program, const QStringList& arguments, int timeoutMs = 10000);
};

#endif // ZIP_HANDLER_H
