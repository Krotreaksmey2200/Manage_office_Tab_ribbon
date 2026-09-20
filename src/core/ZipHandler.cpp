#include "ZipHandler.h"
#include <QProcess>
#include <QTemporaryDir>
#include <QFile>
#include <QDir>
#include <QDirIterator>
#include <QRegularExpression>
#include <QDebug>

QString ZipHandler::runProcess(const QString& program, const QStringList& arguments, int timeoutMs) {
    QProcess process;
    process.start(program, arguments);
    if (!process.waitForStarted(timeoutMs)) {
        return QString();
    }
    if (!process.waitForFinished(timeoutMs)) {
        process.kill();
        return QString();
    }
    return QString::fromUtf8(process.readAllStandardOutput());
}

QString ZipHandler::extractCustomRibbonXml(const QString& archivePath) {
    if (!QFile::exists(archivePath)) {
        return QString();
    }

#if defined(Q_OS_MACOS) || defined(Q_OS_LINUX)
    // 1. Try customUI/customUI14.xml (Office 2010+)
    QString xml = runProcess("/usr/bin/unzip", {"-p", archivePath, "customUI/customUI14.xml"});
    if (!xml.trimmed().isEmpty()) {
        return xml;
    }

    // 2. Try customUI/customUI.xml (Office 2007)
    xml = runProcess("/usr/bin/unzip", {"-p", archivePath, "customUI/customUI.xml"});
    if (!xml.trimmed().isEmpty()) {
        return xml;
    }

    // 3. Fallback: Check if file itself is already XML (.officeUI or plain XML)
    if (archivePath.endsWith(".officeUI", Qt::CaseInsensitive) || archivePath.endsWith(".xml", Qt::CaseInsensitive)) {
        QFile file(archivePath);
        if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            return QString::fromUtf8(file.readAll());
        }
    }
#elif defined(Q_OS_WIN)
    // On Windows, use powershell to read or extract
    QTemporaryDir tempDir;
    if (tempDir.isValid()) {
        QString powershellCmd = QString("tar -xf \"%1\" -C \"%2\" customUI/customUI14.xml customUI/customUI.xml").arg(archivePath, tempDir.path());
        QProcess::execute("powershell.exe", {"-NoProfile", "-Command", powershellCmd});

        QString path14 = tempDir.path() + "/customUI/customUI14.xml";
        QString path07 = tempDir.path() + "/customUI/customUI.xml";
        if (QFile::exists(path14)) {
            QFile f(path14);
            if (f.open(QIODevice::ReadOnly | QIODevice::Text)) return QString::fromUtf8(f.readAll());
        } else if (QFile::exists(path07)) {
            QFile f(path07);
            if (f.open(QIODevice::ReadOnly | QIODevice::Text)) return QString::fromUtf8(f.readAll());
        }
    }
#endif

    return QString();
}

QStringList ZipHandler::listArchiveContents(const QString& archivePath) {
    QStringList result;
    if (!QFile::exists(archivePath)) return result;

#if defined(Q_OS_MACOS) || defined(Q_OS_LINUX)
    QString output = runProcess("/usr/bin/unzip", {"-l", archivePath});
    QStringList lines = output.split('\n');
    for (const QString& line : lines) {
        QString trimmed = line.trimmed();
        // Skip header lines and separator dashes
        if (trimmed.startsWith("Length") || trimmed.startsWith("---") || trimmed.startsWith("Archive:") || trimmed.isEmpty()) {
            continue;
        }
        QStringList tokens = trimmed.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        if (tokens.size() >= 4) {
            result.append(tokens.last());
        }
    }
#endif
    return result;
}

bool ZipHandler::hasCustomRibbonXml(const QString& archivePath) {
    QString xml = extractCustomRibbonXml(archivePath);
    return !xml.trimmed().isEmpty();
}

bool ZipHandler::updateCustomRibbonXml(const QString& archivePath, const QString& newXmlContent, QString& errorMessage) {
    if (!QFile::exists(archivePath)) {
        errorMessage = "File does not exist: " + archivePath;
        return false;
    }

    // Direct .officeUI or .xml replacement
    if (archivePath.endsWith(".officeUI", Qt::CaseInsensitive) || archivePath.endsWith(".xml", Qt::CaseInsensitive)) {
        QFile file(archivePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
            errorMessage = "Cannot open file for writing: " + archivePath;
            return false;
        }
        file.write(newXmlContent.toUtf8());
        file.close();
        return true;
    }

    // OpenXML Archive (.dotm or .ppam)
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        errorMessage = "Failed to create temporary directory for repacking";
        return false;
    }

#if defined(Q_OS_MACOS) || defined(Q_OS_LINUX)
    // 1. Unzip archive into tempDir
    int code = QProcess::execute("/usr/bin/unzip", {"-q", archivePath, "-d", tempDir.path()});
    if (code != 0) {
        errorMessage = "Failed to extract archive: " + archivePath;
        return false;
    }

    // 2. Ensure customUI directory exists
    QDir customUiDir(tempDir.path() + "/customUI");
    if (!customUiDir.exists()) {
        customUiDir.mkpath(".");
    }

    // 3. Write new customUI14.xml
    QString targetXmlPath = tempDir.path() + "/customUI/customUI14.xml";
    QFile xmlFile(targetXmlPath);
    if (!xmlFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        errorMessage = "Failed to write customUI14.xml to temp archive";
        return false;
    }
    xmlFile.write(newXmlContent.toUtf8());
    xmlFile.close();

    // 4. Ensure [Content_Types].xml includes the customUI part if not present
    QString contentTypesPath = tempDir.path() + "/[Content_Types].xml";
    if (QFile::exists(contentTypesPath)) {
        QFile ctFile(contentTypesPath);
        if (ctFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString ctContent = QString::fromUtf8(ctFile.readAll());
            ctFile.close();
            if (!ctContent.contains("customui", Qt::CaseInsensitive)) {
                // Add Override for customUI14.xml
                QString overrideTag = "<Override PartName=\"/customUI/customUI14.xml\" ContentType=\"application/xml\"/>";
                ctContent.replace("</Types>", overrideTag + "\n</Types>");
                if (ctFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
                    ctFile.write(ctContent.toUtf8());
                    ctFile.close();
                }
            }
        }
    }

    // 5. Ensure _rels/.rels references customUI
    QString relsPath = tempDir.path() + "/_rels/.rels";
    if (QFile::exists(relsPath)) {
        QFile relsFile(relsPath);
        if (relsFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString relsContent = QString::fromUtf8(relsFile.readAll());
            relsFile.close();
            if (!relsContent.contains("customui", Qt::CaseInsensitive)) {
                QString relTag = "<Relationship Id=\"rIdCustomUI\" Type=\"http://schemas.microsoft.com/office/2007/relationships/ui/extensibility\" Target=\"customUI/customUI14.xml\"/>";
                relsContent.replace("</Relationships>", relTag + "\n</Relationships>");
                if (relsFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
                    relsFile.write(relsContent.toUtf8());
                    relsFile.close();
                }
            }
        }
    }

    // 6. Repack using zip in the temp directory
    QString tempZipPath = tempDir.path() + "/updated_archive.zip";
    QProcess zipProcess;
    zipProcess.setWorkingDirectory(tempDir.path());
    // Exclude the temp zip itself
    zipProcess.start("/usr/bin/zip", {"-r", "-q", tempZipPath, ".", "-x", "updated_archive.zip"});
    if (!zipProcess.waitForFinished(15000) || zipProcess.exitCode() != 0) {
        errorMessage = "Failed to repack OpenXML zip: " + QString::fromUtf8(zipProcess.readAllStandardError());
        return false;
    }

    // 7. Atomic replace original archive
    if (QFile::exists(tempZipPath)) {
        if (!QFile::remove(archivePath) || !QFile::copy(tempZipPath, archivePath)) {
#if defined(Q_OS_MACOS)
            QString srcEscaped = tempZipPath;
            srcEscaped.replace("\"", "\\\"");
            QString dstEscaped = archivePath;
            dstEscaped.replace("\"", "\\\"");
            QString script = QString("do shell script \"cp -f \\\"%1\\\" \\\"%2\\\"\" with administrator privileges").arg(srcEscaped, dstEscaped);
            int exitCode = QProcess::execute("/usr/bin/osascript", QStringList() << "-e" << script);
            if (exitCode != 0) {
                errorMessage = "Failed to copy updated archive back to " + archivePath + " (Administrator permission required)";
                return false;
            }
#else
            errorMessage = "Failed to copy updated archive back to " + archivePath;
            return false;
#endif
        }
    }
    return true;

#elif defined(Q_OS_WIN)
    errorMessage = "Windows OpenXML repacking is supported via PowerShell or miniz.";
    return false;
#else
    errorMessage = "Unsupported platform";
    return false;
#endif
}
