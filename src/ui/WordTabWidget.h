#ifndef WORD_TAB_WIDGET_H
#define WORD_TAB_WIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <memory>
#include "ExtensionManager.h"
#include "RibbonEditorWidget.h"

class WordTabWidget : public QWidget {
    Q_OBJECT
public:
    explicit WordTabWidget(ExtensionManager* extManager, RibbonEditorWidget* ribbonEditor, QWidget* parent = nullptr);

    void refreshList();

private slots:
    void onSelectionChanged();
    void onToggleStateClicked();
    void onEditRibbonClicked();
    void onInstallClicked();
    void onUninstallClicked();
    void onBackupClicked();
    void onOpenWordClicked();

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
    QPushButton* m_btnOpenWord;
    QLabel* m_lblStatus;

    QList<ExtensionItem> m_items;
};

#endif // WORD_TAB_WIDGET_H
