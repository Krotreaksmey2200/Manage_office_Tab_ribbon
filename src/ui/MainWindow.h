#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QLabel>
#include <memory>
#include "PlatformService.h"
#include "ExtensionManager.h"
#include "AllRibbonTabsWidget.h"
#include "WordTabWidget.h"
#include "PowerPointTabWidget.h"
#include "RibbonEditorWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onThemeToggle();
    void onOpenBackupFolder();
    void onOpenWordStartupFolder();
    void onAbout();

private:
    void setupUi();
    void createMenus();
    QWidget* createDiagnosticsTab();

    std::shared_ptr<IPlatformService> m_platformService;
    ExtensionManager* m_extManager;

    QTabWidget* m_mainTabWidget;
    AllRibbonTabsWidget* m_allRibbonTabsWidget;
    WordTabWidget* m_wordTabWidget;
    PowerPointTabWidget* m_powerPointTabWidget;
    RibbonEditorWidget* m_ribbonEditorWidget;

    QLabel* m_lblStatusMsg;
    QLabel* m_lblPlatformInfo;
    bool m_isDarkMode = true;
};

#endif // MAIN_WINDOW_H
