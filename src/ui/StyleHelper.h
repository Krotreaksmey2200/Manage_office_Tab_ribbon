#ifndef STYLE_HELPER_H
#define STYLE_HELPER_H

#include <QString>

class StyleHelper {
public:
    static QString getModernLightTheme();
    static QString getModernDarkTheme();
    static QString getStatusBadgeStyle(bool enabled, bool isDark = false);
    static QString getRibbonStripStyle(bool isDark = false);
    static QString getCanvasScrollStyle(bool isDark = false);
    static QString getTabButtonStyle(bool isActive, bool isVisible, bool isBuiltIn, bool isDark = false);
    static QString getGroupBoxCardStyle(bool isDark = false);
    static QString getActionButtonStyle(bool isDark = false);
};

#endif // STYLE_HELPER_H
