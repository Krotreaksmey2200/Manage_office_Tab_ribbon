#ifndef POWERPOINT_TAB_WIDGET_H
#define POWERPOINT_TAB_WIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include "ExtensionManager.h"
#include "RibbonEditorWidget.h"

class PowerPointTabWidget : public QWidget {
    Q_OBJECT
public:
    explicit PowerPointTabWidget(ExtensionManager* extManager, RibbonEditorWidget* ribbonEditor, QWidget* parent = nullptr);

    void refreshList();

private slots:
    void onSelectionChanged();
    void onToggleStateClicked();
    void onEditRibbonClicked();
    void onInstallClicked();
    void onUninstallClicked();
    void onBackupClicked();
    void onOpenPowerPointClicked();

private:
    void setupUi();

    ExtensionManager* m_extManager;
    RibbonEditorWidget* m_ribbonEditor;

    QTableWidget* m_table;
    QPushButton* m_btnToggle;
    QPushButton* m_btnEditRibbon;
    QPushButton* m_btnInstall;
    QPushButton* m_btnUninstall;
    QPushButton* m_btnBackup;
    QPushButton* m_btnOpenPPT;
    QLabel* m_lblStatus;

    QList<ExtensionItem> m_items;
};

#endif // POWERPOINT_TAB_WIDGET_H
