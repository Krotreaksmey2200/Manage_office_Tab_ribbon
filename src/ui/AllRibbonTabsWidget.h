#ifndef ALL_RIBBON_TABS_WIDGET_H
#define ALL_RIBBON_TABS_WIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "RibbonModel.h"
#include "ExtensionManager.h"
#include "RibbonEditorWidget.h"

class AllRibbonTabsWidget : public QWidget {
    Q_OBJECT
public:
    explicit AllRibbonTabsWidget(ExtensionManager* extManager, RibbonEditorWidget* ribbonEditor, QWidget* parent = nullptr);

    void reloadAllRibbonTabs();
    void setDarkMode(bool isDark);

private slots:
    void onTabButtonClicked(int index);
    void onToggleTabVisibility(int index);
    void onMoveTabLeft(int index);
    void onMoveTabRight(int index);
    void onAddNewTabClicked();
    void onDeleteTabClicked(int index);
    void onAddGroupToActiveTab();
    void onDeleteGroupFromActiveTab(int groupIndex);
    void onAddButtonToGroup(int groupIndex);
    void onDeleteButtonFromGroup(int groupIndex, int buttonIndex);
    void onEditTabInStudio(int index);
    void onSaveToOfficeUIClicked();
    void onOpenWordClicked();

private:
    void setupUi();
    void buildRibbonBarHeader();
    void displayActiveTabContent();
    void populateTabsTable();
    void loadOfficeData();

    ExtensionManager* m_extManager;
    RibbonEditorWidget* m_ribbonEditor;

    QList<RibbonTab> m_allTabs;
    int m_activeTabIndex = 0;
    bool m_isDarkMode = false;

    // Header strip widgets
    QWidget* m_ribbonStripContainer;
    QHBoxLayout* m_ribbonStripLayout;
    QScrollArea* m_ribbonStripScroll;

    // Active Tab content canvas
    QScrollArea* m_canvasScroll;
    QWidget* m_canvasContainer;
    QHBoxLayout* m_canvasLayout;

    // Tab Management table
    QTableWidget* m_tableTabs;
    QLabel* m_lblStatus;
    QPushButton* m_btnSaveToOfficeUI;
};

#endif // ALL_RIBBON_TABS_WIDGET_H
