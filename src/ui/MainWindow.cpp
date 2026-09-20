#include "MainWindow.h"
#include "StyleHelper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QStatusBar>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QGroupBox>
#include <QFormLayout>
#include <QPushButton>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
    m_platformService = IPlatformService::create();
    m_extManager = new ExtensionManager(m_platformService, this);

    m_isDarkMode = false; // Default to clear, readable Office Light Theme
    setupUi();
    createMenus();

    connect(m_extManager, &ExtensionManager::logMessage, this, [this](const QString& msg, bool isError) {
        m_lblStatusMsg->setText(msg);
        if (isError) {
            m_lblStatusMsg->setStyleSheet("color: #d83b01; font-weight: bold;");
        } else {
            m_lblStatusMsg->setStyleSheet("color: #107c41;");
        }
    });

    setStyleSheet(StyleHelper::getModernLightTheme());
    m_allRibbonTabsWidget->setDarkMode(false);
}

void MainWindow::setupUi() {
    setWindowTitle("កម្មវិធីគ្រប់គ្រង Extension និង Ribbon Tab - Microsoft Office (Word & PowerPoint)");
    resize(1200, 780);

    auto* centralWidget = new QWidget(this);
    centralWidget->setObjectName("centralWidget");
    auto* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(12, 12, 12, 12);

    m_mainTabWidget = new QTabWidget(this);

    // 1. Ribbon Editor Studio (Visual and XML)
    m_ribbonEditorWidget = new RibbonEditorWidget(this);

    // 2. All Ribbon Tabs Manager (Shows full ribbon bar matching user's Word)
    m_allRibbonTabsWidget = new AllRibbonTabsWidget(m_extManager, m_ribbonEditorWidget, this);

    // 3. Word Tab
    m_wordTabWidget = new WordTabWidget(m_extManager, m_ribbonEditorWidget, this);

    // 4. PowerPoint Tab
    m_powerPointTabWidget = new PowerPointTabWidget(m_extManager, m_ribbonEditorWidget, this);

    // 5. Diagnostics & Path Inspector Tab
    QWidget* diagTab = createDiagnosticsTab();

    m_mainTabWidget->addTab(m_allRibbonTabsWidget, "🌟 ផ្ទាំង Ribbon ទាំងអស់ (Full Ribbon Bar)");
    m_mainTabWidget->addTab(m_wordTabWidget, "📝 Microsoft Word");
    m_mainTabWidget->addTab(m_powerPointTabWidget, "📊 Microsoft PowerPoint");
    m_mainTabWidget->addTab(m_ribbonEditorWidget, "🎨 Ribbon XML Studio");
    m_mainTabWidget->addTab(diagTab, "⚙️ ព័ត៌មានប្រព័ន្ធ & Backup");

    mainLayout->addWidget(m_mainTabWidget);
    setCentralWidget(centralWidget);

    // Status bar
    auto* sBar = statusBar();
    m_lblStatusMsg = new QLabel("កម្មវិធីរួចរាល់សម្រាប់ការប្រើប្រាស់", this);
    m_lblPlatformInfo = new QLabel("ប្រព័ន្ធ: " + m_platformService->getPlatformName(), this);
    m_lblPlatformInfo->setStyleSheet("color: #0078d4; font-weight: bold; margin-right: 8px;");

    auto* btnThemeSwitch = new QPushButton("🌗 ប្ដូរពណ៌ (Theme)", this);
    btnThemeSwitch->setObjectName("btnSecondary");
    btnThemeSwitch->setFixedWidth(120);
    connect(btnThemeSwitch, &QPushButton::clicked, this, &MainWindow::onThemeToggle);

    sBar->addWidget(m_lblStatusMsg, 1);
    sBar->addPermanentWidget(btnThemeSwitch);
    sBar->addPermanentWidget(m_lblPlatformInfo);
}

void MainWindow::onThemeToggle() {
    m_isDarkMode = !m_isDarkMode;
    if (m_isDarkMode) {
        setStyleSheet(StyleHelper::getModernDarkTheme());
        m_allRibbonTabsWidget->setDarkMode(true);
    } else {
        setStyleSheet(StyleHelper::getModernLightTheme());
        m_allRibbonTabsWidget->setDarkMode(false);
    }
}

void MainWindow::createMenus() {
    auto* menuFile = menuBar()->addMenu("ឯកសារ (File)");

    auto* actRefresh = menuFile->addAction("🔄 ផ្ទុកឡើងវិញទាំងអស់ (Refresh All)");
    connect(actRefresh, &QAction::triggered, this, [this]() {
        m_allRibbonTabsWidget->reloadAllRibbonTabs();
        m_wordTabWidget->refreshList();
        m_powerPointTabWidget->refreshList();
        m_lblStatusMsg->setText("បានផ្ទុកទិន្នន័យឡើងវិញជោគជ័យ");
    });

    auto* actBackupFolder = menuFile->addAction("📂 បើកថត Backup (Open Backup Folder)");
    connect(actBackupFolder, &QAction::triggered, this, &MainWindow::onOpenBackupFolder);

    menuFile->addSeparator();
    auto* actExit = menuFile->addAction("🚪 ចាកចេញ (Exit)");
    connect(actExit, &QAction::triggered, this, &QWidget::close);

    auto* menuView = menuBar()->addMenu("ទិដ្ឋភាព (View)");
    auto* actToggleTheme = menuView->addAction("🌗 ប្ដូរពណ៌ផ្ទៃ (Toggle Dark/Light Theme)");
    connect(actToggleTheme, &QAction::triggered, this, &MainWindow::onThemeToggle);

    auto* menuHelp = menuBar()->addMenu("ជំនួយ (Help)");
    auto* actAbout = menuHelp->addAction("ℹ️ អំពីកម្មវិធី (About)");
    connect(actAbout, &QAction::triggered, this, &MainWindow::onAbout);
}

QWidget* MainWindow::createDiagnosticsTab() {
    auto* tab = new QWidget(this);
    auto* layout = new QVBoxLayout(tab);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(15);

    auto* grpPaths = new QGroupBox("ទីតាំងថតឯកសារ Office (Detected Office Paths)", tab);
    auto* formLayout = new QFormLayout(grpPaths);

    auto* lblWordStartup = new QLabel(m_platformService->getWordStartupPath(), tab);
    lblWordStartup->setTextInteractionFlags(Qt::TextSelectableByMouse);
    formLayout->addRow("Word Startup Folder:", lblWordStartup);

    auto* lblPptAddins = new QLabel(m_platformService->getPowerPointAddInsPath(), tab);
    lblPptAddins->setTextInteractionFlags(Qt::TextSelectableByMouse);
    formLayout->addRow("PowerPoint Add-Ins Folder:", lblPptAddins);

    auto* lblWordUI = new QLabel(m_platformService->getWordOfficeUIPath(), tab);
    lblWordUI->setTextInteractionFlags(Qt::TextSelectableByMouse);
    formLayout->addRow("Word Ribbon UI (.officeUI):", lblWordUI);

    auto* lblPptUI = new QLabel(m_platformService->getPowerPointOfficeUIPath(), tab);
    lblPptUI->setTextInteractionFlags(Qt::TextSelectableByMouse);
    formLayout->addRow("PowerPoint Ribbon UI (.officeUI):", lblPptUI);

    auto* lblBackup = new QLabel(m_platformService->getBackupDirectory(), tab);
    lblBackup->setTextInteractionFlags(Qt::TextSelectableByMouse);
    formLayout->addRow("Backup Storage Folder:", lblBackup);

    layout->addWidget(grpPaths);

    auto* btnLayout = new QHBoxLayout();
    auto* btnOpenWordStartup = new QPushButton("📂 បើកថត Word Startup", tab);
    btnOpenWordStartup->setObjectName("btnSecondary");
    connect(btnOpenWordStartup, &QPushButton::clicked, this, &MainWindow::onOpenWordStartupFolder);

    auto* btnOpenBackup = new QPushButton("📂 បើកថត Backup", tab);
    btnOpenBackup->setObjectName("btnSecondary");
    connect(btnOpenBackup, &QPushButton::clicked, this, &MainWindow::onOpenBackupFolder);

    btnLayout->addWidget(btnOpenWordStartup);
    btnLayout->addWidget(btnOpenBackup);
    btnLayout->addStretch();
    layout->addLayout(btnLayout);

    layout->addStretch();
    return tab;
}



void MainWindow::onOpenBackupFolder() {
    QString path = m_platformService->getBackupDirectory();
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void MainWindow::onOpenWordStartupFolder() {
    QString path = m_platformService->getWordStartupPath();
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void MainWindow::onAbout() {
    QMessageBox::about(this, "អំពីកម្មវិធី", 
        "<h3>Office Extension & Ribbon Tab Manager</h3>"
        "<p>កម្មវិធី C++ / Qt សម្រាប់គ្រប់គ្រង Extensions/Add-ins និង Ribbon Tabs របស់ Microsoft Word និង PowerPoint។</p>"
        "<p><b>លក្ខណៈពិសេស៖</b></p>"
        "<ul>"
        "<li>គ្រប់គ្រង Word Templates (.dotm) & PowerPoint Add-ins (.ppam)</li>"
        "<li>បើក/បិទ (Enable/Disable) និងដំឡើង Add-in ដោយសុវត្ថិភាព</li>"
        "<li>បង្កើត និងកែប្រែ Custom Ribbon Tabs & Buttons (គាំទ្រអក្សរខ្មែរពេញលេញ)</li>"
        "<li>ដំណើរការទាំងលើ macOS និង Windows</li>"
        "</ul>");
}
