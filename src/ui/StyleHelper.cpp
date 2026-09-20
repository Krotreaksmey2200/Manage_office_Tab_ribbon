#include "StyleHelper.h"

QString StyleHelper::getModernLightTheme() {
    return R"(
        QMainWindow, QWidget#centralWidget {
            background-color: #f3f4f6;
            color: #1f2937;
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", "Khmer OS Battambang", Roboto, Helvetica, Arial, sans-serif;
            font-size: 13px;
        }

        QTabWidget::pane {
            border: 1px solid #d1d5db;
            background: #ffffff;
            border-radius: 8px;
            top: -1px;
        }

        QTabBar::tab {
            background: #e5e7eb;
            color: #4b5563;
            padding: 8px 18px;
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
            margin-right: 4px;
            font-weight: 600;
            font-size: 13px;
        }

        QTabBar::tab:selected {
            background: #ffffff;
            color: #0078d4;
            border-bottom: 3px solid #0078d4;
        }

        QTabBar::tab:hover {
            color: #111827;
            background: #f3f4f6;
        }

        QGroupBox {
            font-weight: bold;
            font-size: 13px;
            border: 1px solid #e5e7eb;
            border-radius: 8px;
            margin-top: 16px;
            padding-top: 14px;
            background-color: #ffffff;
            color: #0078d4;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 6px;
        }

        QPushButton {
            background-color: #0078d4;
            color: #ffffff;
            border: none;
            padding: 7px 16px;
            border-radius: 6px;
            font-weight: 600;
            font-size: 13px;
        }

        QPushButton:hover {
            background-color: #106ebe;
        }

        QPushButton:pressed {
            background-color: #005a9e;
        }

        QPushButton#btnDanger {
            background-color: #d83b01;
            color: #ffffff;
        }
        QPushButton#btnDanger:hover {
            background-color: #ea4a1f;
        }

        QPushButton#btnSuccess {
            background-color: #107c41;
            color: #ffffff;
        }
        QPushButton#btnSuccess:hover {
            background-color: #13914c;
        }

        QPushButton#btnSecondary {
            background-color: #e5e7eb;
            color: #374151;
            border: 1px solid #d1d5db;
        }
        QPushButton#btnSecondary:hover {
            background-color: #d1d5db;
            color: #111827;
        }

        QTableWidget, QTreeView, QListWidget {
            background-color: #ffffff;
            border: 1px solid #e5e7eb;
            border-radius: 6px;
            gridline-color: #f3f4f6;
            color: #1f2937;
            selection-background-color: #e0f2fe;
            selection-color: #0369a1;
            alternate-background-color: #f9fafb;
        }

        QHeaderView::section {
            background-color: #f3f4f6;
            color: #374151;
            padding: 8px;
            border: none;
            border-bottom: 2px solid #e5e7eb;
            font-weight: bold;
            font-size: 12px;
        }

        QLineEdit, QTextEdit, QPlainTextEdit, QComboBox {
            background-color: #ffffff;
            border: 1px solid #d1d5db;
            border-radius: 6px;
            padding: 6px 10px;
            color: #111827;
        }

        QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus, QComboBox:focus {
            border: 2px solid #0078d4;
        }

        QSplitter::handle {
            background-color: #e5e7eb;
        }

        QStatusBar {
            background-color: #ffffff;
            color: #4b5563;
            border-top: 1px solid #e5e7eb;
        }

        QScrollBar:vertical {
            border: none;
            background: #f3f4f6;
            width: 10px;
            margin: 0px;
        }
        QScrollBar::handle:vertical {
            background: #d1d5db;
            min-height: 20px;
            border-radius: 5px;
        }
        QScrollBar::handle:vertical:hover {
            background: #9ca3af;
        }
    )";
}

QString StyleHelper::getModernDarkTheme() {
    return R"(
        QMainWindow, QWidget#centralWidget {
            background-color: #1e1e24;
            color: #f3f4f6;
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", "Khmer OS Battambang", Roboto, Helvetica, Arial, sans-serif;
            font-size: 13px;
        }

        QTabWidget::pane {
            border: 1px solid #383a4c;
            background: #252631;
            border-radius: 8px;
            top: -1px;
        }

        QTabBar::tab {
            background: #181920;
            color: #cbd5e1;
            padding: 8px 18px;
            border-top-left-radius: 8px;
            border-top-right-radius: 8px;
            margin-right: 4px;
            font-weight: 600;
        }

        QTabBar::tab:selected {
            background: #252631;
            color: #ffffff;
            border-bottom: 3px solid #0078d4;
        }

        QTabBar::tab:hover {
            color: #ffffff;
            background: #2f303e;
        }

        QGroupBox {
            font-weight: bold;
            font-size: 13px;
            border: 1px solid #3b3e52;
            border-radius: 8px;
            margin-top: 16px;
            padding-top: 14px;
            background-color: #21222c;
            color: #38bdf8;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 12px;
            padding: 0 6px;
        }

        QPushButton {
            background-color: #0078d4;
            color: #ffffff;
            border: none;
            padding: 7px 16px;
            border-radius: 6px;
            font-weight: 600;
        }

        QPushButton:hover {
            background-color: #106ebe;
        }

        QPushButton:pressed {
            background-color: #005a9e;
        }

        QPushButton#btnDanger {
            background-color: #dc2626;
            color: #ffffff;
        }
        QPushButton#btnDanger:hover {
            background-color: #ef4444;
        }

        QPushButton#btnSuccess {
            background-color: #16a34a;
            color: #ffffff;
        }
        QPushButton#btnSuccess:hover {
            background-color: #22c55e;
        }

        QPushButton#btnSecondary {
            background-color: #333544;
            color: #f1f5f9;
            border: 1px solid #474a5f;
        }
        QPushButton#btnSecondary:hover {
            background-color: #43475d;
        }

        QTableWidget, QTreeView, QListWidget {
            background-color: #1c1d24;
            border: 1px solid #383a4c;
            border-radius: 6px;
            gridline-color: #2d2f3d;
            color: #f8fafc;
            selection-background-color: #0369a1;
            selection-color: #ffffff;
        }

        QHeaderView::section {
            background-color: #252631;
            color: #e2e8f0;
            padding: 8px;
            border: none;
            border-bottom: 2px solid #383a4c;
            font-weight: bold;
        }

        QLineEdit, QTextEdit, QPlainTextEdit, QComboBox {
            background-color: #15161b;
            border: 1px solid #434659;
            border-radius: 6px;
            padding: 6px 10px;
            color: #ffffff;
        }

        QLineEdit:focus, QTextEdit:focus, QPlainTextEdit:focus, QComboBox:focus {
            border: 2px solid #38bdf8;
        }

        QSplitter::handle {
            background-color: #383a4c;
        }

        QStatusBar {
            background-color: #181920;
            color: #cbd5e1;
            border-top: 1px solid #2d2f3d;
        }
    )";
}

QString StyleHelper::getStatusBadgeStyle(bool enabled, bool isDark) {
    if (enabled) {
        if (isDark) {
            return "background-color: #064e3b; color: #6ee7b7; border: 1px solid #059669; padding: 4px 10px; border-radius: 4px; font-weight: bold;";
        } else {
            return "background-color: #dcfce7; color: #15803d; border: 1px solid #86efac; padding: 4px 10px; border-radius: 4px; font-weight: bold;";
        }
    } else {
        if (isDark) {
            return "background-color: #334155; color: #94a3b8; border: 1px solid #475569; padding: 4px 10px; border-radius: 4px;";
        } else {
            return "background-color: #f1f5f9; color: #64748b; border: 1px solid #cbd5e1; padding: 4px 10px; border-radius: 4px;";
        }
    }
}

QString StyleHelper::getRibbonStripStyle(bool isDark) {
    if (isDark) {
        return "background-color: #181920; border: 1px solid #383a4c; border-radius: 6px;";
    } else {
        return "background-color: #ffffff; border: 1px solid #e5e7eb; border-radius: 6px;";
    }
}

QString StyleHelper::getCanvasScrollStyle(bool isDark) {
    if (isDark) {
        return "background-color: #21222c; border: 1px solid #383a4c; border-radius: 6px;";
    } else {
        return "background-color: #f9fafb; border: 1px solid #e5e7eb; border-radius: 6px;";
    }
}

QString StyleHelper::getTabButtonStyle(bool isActive, bool isVisible, bool isBuiltIn, bool isDark) {
    if (!isVisible) {
        return isDark 
            ? "color: #64748b; text-decoration: line-through; padding: 7px 16px; border: none; font-size: 13px;"
            : "color: #9ca3af; text-decoration: line-through; padding: 7px 16px; border: none; font-size: 13px;";
    }

    if (isActive) {
        return isDark
            ? "color: #38bdf8; font-weight: bold; border: none; border-bottom: 3px solid #38bdf8; background-color: #2b2d3c; padding: 7px 16px; font-size: 13px;"
            : "color: #0078d4; font-weight: bold; border: none; border-bottom: 3px solid #0078d4; background-color: #eff6ff; padding: 7px 16px; font-size: 13px;";
    }

    if (isBuiltIn) {
        return isDark
            ? "color: #cbd5e1; border: none; padding: 7px 16px; font-size: 13px; font-weight: 500;"
            : "color: #1f2937; border: none; padding: 7px 16px; font-size: 13px; font-weight: 500;";
    } else {
        // Custom add-in tabs
        return isDark
            ? "color: #60a5fa; font-weight: 600; border: none; padding: 7px 16px; font-size: 13px;"
            : "color: #0284c7; font-weight: 600; border: none; padding: 7px 16px; font-size: 13px;";
    }
}

QString StyleHelper::getGroupBoxCardStyle(bool isDark) {
    if (isDark) {
        return "QGroupBox { border: 1px solid #3b3e52; border-radius: 6px; margin-top: 10px; font-size: 11px; color: #94a3b8; padding: 6px; background-color: #1c1d24; } "
               "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 4px; }";
    } else {
        return "QGroupBox { border: 1px solid #d1d5db; border-radius: 6px; margin-top: 10px; font-size: 11px; color: #4b5563; padding: 6px; background-color: #ffffff; } "
               "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 4px; }";
    }
}

QString StyleHelper::getActionButtonStyle(bool isDark) {
    if (isDark) {
        return "background-color: #2a2c3a; color: #f8fafc; border: 1px solid #43475d; border-radius: 4px; padding: 8px 14px; font-size: 12px; font-weight: 500;";
    } else {
        return "background-color: #ffffff; color: #1f2937; border: 1px solid #d1d5db; border-radius: 4px; padding: 8px 14px; font-size: 12px; font-weight: 500;";
    }
}
