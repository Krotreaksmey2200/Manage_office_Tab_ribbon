#ifndef RIBBON_MODEL_H
#define RIBBON_MODEL_H

#include <QString>
#include <QList>

enum class RibbonControlType {
    Button,
    ToggleButton,
    Separator,
    Menu,
    SplitButton,
    Group,
    Tab
};

struct RibbonControl {
    RibbonControlType type = RibbonControlType::Button;
    QString id;
    QString label;              // Supports Khmer Unicode!
    QString image;              // Custom image name in customUI/images/
    QString imageMso;           // Built-in Office icon name
    QString size = "large";     // "normal" or "large"
    QString onAction;           // VBA Macro or callback function
    QString screentip;          // Short tooltip title
    QString supertip;           // Long tooltip description
    bool visible = true;
    bool enabled = true;
    QList<RibbonControl> children; // For menus, splitButtons, etc.
};

struct RibbonGroup {
    QString id;
    QString label;
    bool autoScale = true;
    QList<RibbonControl> controls;
};

struct RibbonTab {
    QString id;
    QString label;
    QString insertBeforeQ;
    QString insertAfterQ;
    bool visible = true;
    bool isBuiltIn = false;         // Built-in Office tab (Home, Insert, Draw, etc.)
    QString sourceOrigin;          // "Built-in", "Word.officeUI", "Mathtype-kh.dotm", etc.
    QList<RibbonGroup> groups;
};

struct RibbonDefinition {
    QString namespaceUri = "http://schemas.microsoft.com/office/2009/07/customui";
    QList<RibbonTab> tabs;
};

#endif // RIBBON_MODEL_H
