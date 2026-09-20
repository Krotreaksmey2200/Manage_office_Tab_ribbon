#include "RibbonXmlGenerator.h"
#include <QTextStream>

QString RibbonXmlGenerator::escapeXml(const QString& str) {
    QString res = str;
    res.replace("&", "&amp;");
    res.replace("<", "&lt;");
    res.replace(">", "&gt;");
    res.replace("\"", "&quot;");
    return res;
}

QString RibbonXmlGenerator::controlTypeToTagName(RibbonControlType type, bool isOfficeUI) {
    QString prefix = isOfficeUI ? "mso:" : "";
    switch (type) {
        case RibbonControlType::Button: return prefix + "button";
        case RibbonControlType::ToggleButton: return prefix + "toggleButton";
        case RibbonControlType::Separator: return prefix + "separator";
        case RibbonControlType::Menu: return prefix + "menu";
        case RibbonControlType::SplitButton: return prefix + "splitButton";
        default: return prefix + "button";
    }
}

QString RibbonXmlGenerator::generateXml(const RibbonDefinition& definition, bool asOfficeUI) {
    QString xml;
    QTextStream out(&xml);

    QString prefix = asOfficeUI ? "mso:" : "";

    if (asOfficeUI) {
        out << "<mso:customUI xmlns:mso=\"http://schemas.microsoft.com/office/2009/07/customui\">\n";
    } else {
        out << "<customUI xmlns=\"" << (definition.namespaceUri.isEmpty() ? "http://schemas.microsoft.com/office/2009/07/customui" : definition.namespaceUri) << "\">\n";
    }

    out << "  <" << prefix << "ribbon>\n";
    out << "    <" << prefix << "tabs>\n";

    for (const RibbonTab& tab : definition.tabs) {
        out << "      <" << prefix << "tab id=\"" << escapeXml(tab.id) << "\" label=\"" << escapeXml(tab.label) << "\"";
        if (!tab.insertBeforeQ.isEmpty()) {
            out << " insertBeforeQ=\"" << escapeXml(tab.insertBeforeQ) << "\"";
        }
        if (!tab.insertAfterQ.isEmpty()) {
            out << " insertAfterQ=\"" << escapeXml(tab.insertAfterQ) << "\"";
        }
        if (!tab.visible) {
            out << " visible=\"false\"";
        }
        out << ">\n";

        for (const RibbonGroup& grp : tab.groups) {
            out << "        <" << prefix << "group id=\"" << escapeXml(grp.id) << "\" label=\"" << escapeXml(grp.label) << "\"";
            if (grp.autoScale) {
                out << " autoScale=\"true\"";
            }
            out << ">\n";

            for (const RibbonControl& ctrl : grp.controls) {
                QString tagName = controlTypeToTagName(ctrl.type, asOfficeUI);
                if (ctrl.type == RibbonControlType::Separator) {
                    out << "          <" << tagName << " id=\"" << escapeXml(ctrl.id) << "\" />\n";
                    continue;
                }

                out << "          <" << tagName << "\n";
                out << "            id=\"" << escapeXml(ctrl.id) << "\"\n";
                out << "            label=\"" << escapeXml(ctrl.label) << "\"\n";
                if (!ctrl.image.isEmpty()) {
                    out << "            image=\"" << escapeXml(ctrl.image) << "\"\n";
                }
                if (!ctrl.imageMso.isEmpty()) {
                    out << "            imageMso=\"" << escapeXml(ctrl.imageMso) << "\"\n";
                }
                if (!ctrl.size.isEmpty()) {
                    out << "            size=\"" << escapeXml(ctrl.size) << "\"\n";
                }
                if (!ctrl.onAction.isEmpty()) {
                    out << "            onAction=\"" << escapeXml(ctrl.onAction) << "\"\n";
                }
                if (!ctrl.screentip.isEmpty()) {
                    out << "            screentip=\"" << escapeXml(ctrl.screentip) << "\"\n";
                }
                if (!ctrl.supertip.isEmpty()) {
                    out << "            supertip=\"" << escapeXml(ctrl.supertip) << "\"\n";
                }
                if (!ctrl.enabled) {
                    out << "            enabled=\"false\"\n";
                }
                if (!ctrl.visible) {
                    out << "            visible=\"false\"\n";
                }

                if (ctrl.children.isEmpty()) {
                    out << "          />\n";
                } else {
                    out << "          >\n";
                    for (const RibbonControl& child : ctrl.children) {
                        QString childTag = controlTypeToTagName(child.type, asOfficeUI);
                        out << "            <" << childTag << " id=\"" << escapeXml(child.id) << "\" label=\"" << escapeXml(child.label) << "\"";
                        if (!child.onAction.isEmpty()) out << " onAction=\"" << escapeXml(child.onAction) << "\"";
                        out << " />\n";
                    }
                    out << "          </" << tagName << ">\n";
                }
            }

            out << "        </" << prefix << "group>\n";
        }

        out << "      </" << prefix << "tab>\n";
    }

    out << "    </" << prefix << "tabs>\n";
    out << "  </" << prefix << "ribbon>\n";
    out << "</" << prefix << "customUI>\n";

    return xml;
}

QString RibbonXmlGenerator::generateDefaultKhmerTabXml() {
    RibbonDefinition def;
    RibbonTab tab;
    tab.id = "tabKhmerTools";
    tab.label = "ឧបករណ៍ខ្មែរ";

    RibbonGroup grp;
    grp.id = "grpKhmerMath";
    grp.label = "ម៉ាក្រូ & សមីការ";

    RibbonControl btn1;
    btn1.id = "btnKhmerEditor";
    btn1.label = "បើកកម្មវិធីកែប្រែ";
    btn1.imageMso = "MacroPlay";
    btn1.size = "large";
    btn1.onAction = "OpenKhmerTool";
    btn1.screentip = "បើកកម្មវិធីកែប្រែ";
    btn1.supertip = "ចុចដើម្បីបើកផ្ទាំងគ្រប់គ្រងសមីការ ឬឧបករណ៍ភាសាខ្មែរ";
    grp.controls.append(btn1);

    RibbonControl btn2;
    btn2.id = "btnFormatFont";
    btn2.label = "តម្រឹមពុម្ពអក្សរ";
    btn2.imageMso = "FontColorPicker";
    btn2.size = "large";
    btn2.onAction = "FormatKhmerFont";
    btn2.screentip = "កំណត់ពុម្ពអក្សរខ្មែរ";
    btn2.supertip = "អនុវត្តពុម្ពអក្សរខ្មែរទៅលើឯកសារ";
    grp.controls.append(btn2);

    tab.groups.append(grp);
    def.tabs.append(tab);

    return generateXml(def);
}
