#ifndef RIBBON_XML_PARSER_H
#define RIBBON_XML_PARSER_H

#include <QString>
#include <QDomElement>
#include "RibbonModel.h"

class RibbonXmlParser {
public:
    static bool parse(const QString& xmlContent, RibbonDefinition& outDefinition, QString& errorMessage);

private:
    static void parseTab(const QDomElement& tabElem, RibbonTab& outTab);
    static void parseGroup(const QDomElement& groupElem, RibbonGroup& outGroup);
    static void parseControl(const QDomElement& ctrlElem, RibbonControl& outControl);
};

#endif // RIBBON_XML_PARSER_H
