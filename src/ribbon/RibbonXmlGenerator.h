#ifndef RIBBON_XML_GENERATOR_H
#define RIBBON_XML_GENERATOR_H

#include <QString>
#include "RibbonModel.h"

class RibbonXmlGenerator {
public:
    static QString generateXml(const RibbonDefinition& definition, bool asOfficeUI = false);
    static QString generateDefaultKhmerTabXml();

private:
    static QString escapeXml(const QString& str);
    static QString controlTypeToTagName(RibbonControlType type, bool isOfficeUI);
};

#endif // RIBBON_XML_GENERATOR_H
