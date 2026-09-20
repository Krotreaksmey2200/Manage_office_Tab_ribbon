#include "RibbonXmlParser.h"
#include <QDomDocument>
#include <QDebug>

static QString getLocalTagName(const QDomElement& elem) {
    QString tag = elem.tagName();
    int colonIdx = tag.indexOf(':');
    if (colonIdx >= 0) {
        return tag.mid(colonIdx + 1);
    }
    return tag;
}

bool RibbonXmlParser::parse(const QString& xmlContent, RibbonDefinition& outDefinition, QString& errorMessage) {
    if (xmlContent.trimmed().isEmpty()) {
        errorMessage = "XML content is empty";
        return false;
    }

    QDomDocument doc;
    QString parseError;
    int errorLine = 0;
    int errorCol = 0;

    if (!doc.setContent(xmlContent, true, &parseError, &errorLine, &errorCol)) {
        errorMessage = QString("XML Parse Error at line %1, col %2: %3").arg(errorLine).arg(errorCol).arg(parseError);
        return false;
    }

    QDomElement root = doc.documentElement();
    QString rootTag = getLocalTagName(root);
    if (rootTag != "customUI") {
        errorMessage = "Root element is not <customUI>";
        return false;
    }

    outDefinition.namespaceUri = root.attribute("xmlns", "http://schemas.microsoft.com/office/2009/07/customui");
    outDefinition.tabs.clear();

    // Look for <ribbon> child
    QDomElement ribbonElem;
    for (QDomNode n = root.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (n.isElement() && getLocalTagName(n.toElement()) == "ribbon") {
            ribbonElem = n.toElement();
            break;
        }
    }

    if (ribbonElem.isNull()) {
        // Maybe tabs are directly under root? (unlikely, but fallback)
        ribbonElem = root;
    }

    // Look for <tabs> child
    QDomElement tabsElem;
    for (QDomNode n = ribbonElem.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (n.isElement() && getLocalTagName(n.toElement()) == "tabs") {
            tabsElem = n.toElement();
            break;
        }
    }

    if (!tabsElem.isNull()) {
        for (QDomNode n = tabsElem.firstChild(); !n.isNull(); n = n.nextSibling()) {
            if (n.isElement() && getLocalTagName(n.toElement()) == "tab") {
                RibbonTab tab;
                parseTab(n.toElement(), tab);
                outDefinition.tabs.append(tab);
            }
        }
    }

    return true;
}

void RibbonXmlParser::parseTab(const QDomElement& tabElem, RibbonTab& outTab) {
    outTab.id = tabElem.attribute("id");
    if (outTab.id.isEmpty()) outTab.id = tabElem.attribute("idQ");
    if (outTab.id.isEmpty()) outTab.id = tabElem.attribute("idMso");

    outTab.label = tabElem.attribute("label");
    outTab.insertBeforeQ = tabElem.attribute("insertBeforeQ");
    outTab.insertAfterQ = tabElem.attribute("insertAfterQ");
    if (tabElem.hasAttribute("visible")) {
        outTab.visible = (tabElem.attribute("visible") != "false");
    }

    for (QDomNode n = tabElem.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (n.isElement() && getLocalTagName(n.toElement()) == "group") {
            RibbonGroup grp;
            parseGroup(n.toElement(), grp);
            outTab.groups.append(grp);
        }
    }
}

void RibbonXmlParser::parseGroup(const QDomElement& groupElem, RibbonGroup& outGroup) {
    outGroup.id = groupElem.attribute("id");
    if (outGroup.id.isEmpty()) outGroup.id = groupElem.attribute("idQ");
    if (outGroup.id.isEmpty()) outGroup.id = groupElem.attribute("idMso");

    outGroup.label = groupElem.attribute("label");
    if (groupElem.hasAttribute("autoScale")) {
        outGroup.autoScale = (groupElem.attribute("autoScale") != "false");
    }

    for (QDomNode n = groupElem.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (n.isElement()) {
            RibbonControl ctrl;
            parseControl(n.toElement(), ctrl);
            outGroup.controls.append(ctrl);
        }
    }
}

void RibbonXmlParser::parseControl(const QDomElement& ctrlElem, RibbonControl& outControl) {
    QString localTag = getLocalTagName(ctrlElem);

    if (localTag == "button") {
        outControl.type = RibbonControlType::Button;
    } else if (localTag == "toggleButton") {
        outControl.type = RibbonControlType::ToggleButton;
    } else if (localTag == "separator") {
        outControl.type = RibbonControlType::Separator;
    } else if (localTag == "menu") {
        outControl.type = RibbonControlType::Menu;
    } else if (localTag == "splitButton") {
        outControl.type = RibbonControlType::SplitButton;
    } else {
        outControl.type = RibbonControlType::Button;
    }

    outControl.id = ctrlElem.attribute("id");
    if (outControl.id.isEmpty()) outControl.id = ctrlElem.attribute("idQ");
    if (outControl.id.isEmpty()) outControl.id = ctrlElem.attribute("idMso");

    outControl.label = ctrlElem.attribute("label");
    outControl.image = ctrlElem.attribute("image");
    outControl.imageMso = ctrlElem.attribute("imageMso");
    outControl.size = ctrlElem.attribute("size", "large");
    outControl.onAction = ctrlElem.attribute("onAction");
    outControl.screentip = ctrlElem.attribute("screentip");
    outControl.supertip = ctrlElem.attribute("supertip");

    if (ctrlElem.hasAttribute("visible")) {
        outControl.visible = (ctrlElem.attribute("visible") != "false");
    }
    if (ctrlElem.hasAttribute("enabled")) {
        outControl.enabled = (ctrlElem.attribute("enabled") != "false");
    }

    // Children for menu / splitButton
    for (QDomNode n = ctrlElem.firstChild(); !n.isNull(); n = n.nextSibling()) {
        if (n.isElement()) {
            RibbonControl child;
            parseControl(n.toElement(), child);
            outControl.children.append(child);
        }
    }
}
