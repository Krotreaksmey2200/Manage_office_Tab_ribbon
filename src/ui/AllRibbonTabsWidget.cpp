#include "AllRibbonTabsWidget.h"
#include "RibbonXmlParser.h"
#include "RibbonXmlGenerator.h"
#include "ZipHandler.h"
#include "StyleHelper.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QGroupBox>
#include <QProcess>
#include <QFile>
#include <QFileInfo>
#include <QInputDialog>

AllRibbonTabsWidget::AllRibbonTabsWidget(ExtensionManager* extManager, RibbonEditorWidget* ribbonEditor, QWidget* parent)
    : QWidget(parent), m_extManager(extManager), m_ribbonEditor(ribbonEditor) {
    setupUi();
    reloadAllRibbonTabs();
}

void AllRibbonTabsWidget::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(12);

    // ================= 1. OFFICE RIBBON BAR (PREVIEW STRIP) =================
    auto* grpRibbonBar = new QGroupBox("ផ្ទាំង Ribbon Bar ពេញលេញ (Microsoft Word Ribbon Bar)", this);
    auto* ribbonBarLayout = new QVBoxLayout(grpRibbonBar);
    ribbonBarLayout->setContentsMargins(8, 12, 8, 8);
    ribbonBarLayout->setSpacing(6);

    // Horizontal Tab Strip
    m_ribbonStripScroll = new QScrollArea(this);
    m_ribbonStripScroll->setFixedHeight(48);
    m_ribbonStripScroll->setWidgetResizable(true);
    m_ribbonStripScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_ribbonStripScroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_ribbonStripScroll->setStyleSheet(StyleHelper::getRibbonStripStyle(m_isDarkMode));

    m_ribbonStripContainer = new QWidget(this);
    m_ribbonStripLayout = new QHBoxLayout(m_ribbonStripContainer);
    m_ribbonStripLayout->setContentsMargins(6, 2, 6, 2);
    m_ribbonStripLayout->setSpacing(4);
    m_ribbonStripLayout->setAlignment(Qt::AlignLeft);
    m_ribbonStripScroll->setWidget(m_ribbonStripContainer);

    ribbonBarLayout->addWidget(m_ribbonStripScroll);

    // Canvas showing controls of the currently active tab
    m_canvasScroll = new QScrollArea(this);
    m_canvasScroll->setFixedHeight(140);
    m_canvasScroll->setWidgetResizable(true);
    m_canvasScroll->setStyleSheet(StyleHelper::getCanvasScrollStyle(m_isDarkMode));

    m_canvasContainer = new QWidget(this);
    m_canvasLayout = new QHBoxLayout(m_canvasContainer);
    m_canvasLayout->setContentsMargins(10, 8, 10, 8);
    m_canvasLayout->setSpacing(12);
    m_canvasLayout->setAlignment(Qt::AlignLeft);
    m_canvasScroll->setWidget(m_canvasContainer);

    ribbonBarLayout->addWidget(m_canvasScroll);
    mainLayout->addWidget(grpRibbonBar);

    // ================= 2. TAB MANAGEMENT TABLE & TOOLBAR =================
    auto* grpManagement = new QGroupBox("គ្រប់គ្រង Ribbon Tabs ទាំងអស់ (Show/Hide, Reorder & Edit)", this);
    auto* mgmtLayout = new QVBoxLayout(grpManagement);
    mgmtLayout->setContentsMargins(8, 12, 8, 8);
    mgmtLayout->setSpacing(8);

    auto* toolbar = new QHBoxLayout();
    auto* btnAddNew = new QPushButton("➕ បន្ថែម Tab ថ្មី (New Tab)", this);
    btnAddNew->setObjectName("btnSuccess");
    connect(btnAddNew, &QPushButton::clicked, this, &AllRibbonTabsWidget::onAddNewTabClicked);

    m_btnSaveToOfficeUI = new QPushButton("💾 រក្សាទុកចូល Word (Apply to Word.officeUI)", this);
    connect(m_btnSaveToOfficeUI, &QPushButton::clicked, this, &AllRibbonTabsWidget::onSaveToOfficeUIClicked);

    auto* btnReload = new QPushButton("🔄 ផ្ទុកឡើងវិញ (Reload)", this);
    btnReload->setObjectName("btnSecondary");
    connect(btnReload, &QPushButton::clicked, this, &AllRibbonTabsWidget::reloadAllRibbonTabs);

    auto* btnOpenWord = new QPushButton("🚀 បើក Microsoft Word", this);
    btnOpenWord->setObjectName("btnSecondary");
    connect(btnOpenWord, &QPushButton::clicked, this, &AllRibbonTabsWidget::onOpenWordClicked);

    toolbar->addWidget(btnAddNew);
    toolbar->addWidget(m_btnSaveToOfficeUI);
    toolbar->addWidget(btnReload);
    toolbar->addStretch();
    toolbar->addWidget(btnOpenWord);
    mgmtLayout->addLayout(toolbar);

    // Table of tabs
    m_tableTabs = new QTableWidget(this);
    m_tableTabs->setColumnCount(6);
    m_tableTabs->setHorizontalHeaderLabels({
        "បង្ហាញ/លាក់ (Visible)",
        "ឈ្មោះ Ribbon Tab",
        "ប្រភព (Origin)",
        "ចំនួន Groups",
        "ចំនួន ប៊ូតុង",
        "សកម្មភាព (Actions)"
    });
    m_tableTabs->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableTabs->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableTabs->horizontalHeader()->setStretchLastSection(true);
    m_tableTabs->horizontalHeader()->resizeSection(0, 140);
    m_tableTabs->horizontalHeader()->resizeSection(1, 200);
    m_tableTabs->horizontalHeader()->resizeSection(2, 160);
    m_tableTabs->horizontalHeader()->resizeSection(3, 110);
    m_tableTabs->horizontalHeader()->resizeSection(4, 110);
    mgmtLayout->addWidget(m_tableTabs);

    m_lblStatus = new QLabel(this);
    m_lblStatus->setStyleSheet("color: #8a8d9b; font-size: 12px;");
    mgmtLayout->addWidget(m_lblStatus);

    mainLayout->addWidget(grpManagement);
}

void AllRibbonTabsWidget::loadOfficeData() {
    m_allTabs.clear();

    // Set of tab IDs explicitly marked as hidden in Word.officeUI
    QSet<QString> hiddenTabIdsInOfficeUI;
    // Set of tab IDs explicitly marked visible in Word.officeUI
    QMap<QString, RibbonTab> officeUiCustomTabs;

    if (m_extManager && m_extManager->platformService()) {
        QString uiPath = m_extManager->platformService()->getWordOfficeUIPath();
        if (QFile::exists(uiPath)) {
            QFile f(uiPath);
            if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QString xml = QString::fromUtf8(f.readAll());
                f.close();

                QDomDocument doc;
                if (doc.setContent(xml)) {
                    QDomNodeList tabNodes = doc.elementsByTagName("mso:tab");
                    if (tabNodes.isEmpty()) tabNodes = doc.elementsByTagName("tab");
                    for (int i = 0; i < tabNodes.size(); ++i) {
                        QDomElement el = tabNodes.at(i).toElement();
                        QString id = el.attribute("id");
                        if (id.isEmpty()) id = el.attribute("idQ");
                        if (id.startsWith("mso:")) id = id.mid(4);

                        if (el.attribute("visible") == "false") {
                            hiddenTabIdsInOfficeUI.insert(id);
                        } else if (!el.attribute("label").isEmpty()) {
                            // User-created custom tab in officeUI
                            RibbonTab t;
                            t.id = id;
                            t.label = el.attribute("label");
                            t.isBuiltIn = false;
                            t.visible = true;
                            t.sourceOrigin = "Word.officeUI";
                            officeUiCustomTabs.insert(id, t);
                        }
                    }
                }
            }
        }
    }

    // 1. Built-in Word standard tabs
    struct BuiltInTabInfo {
        QString id;
        QString label;
    };
    QList<BuiltInTabInfo> builtIns = {
        {"TabHome", "Home"},
        {"TabInsert", "Insert"},
        {"TabDrawInk", "Draw"},
        {"TabDesign", "Design"},
        {"TabLayoutPageLayout", "Layout"},
        {"TabReferences", "References"},
        {"TabMailings", "Mailings"},
        {"TabReview", "Review"},
        {"TabView", "View"},
        {"TabDeveloper", "Developer"}
    };

    for (const auto& b : builtIns) {
        RibbonTab tab;
        tab.id = b.id;
        tab.label = b.label;
        tab.isBuiltIn = true;
        tab.visible = !hiddenTabIdsInOfficeUI.contains(b.id);
        tab.sourceOrigin = "Microsoft Office Standard";

        RibbonGroup grp;
        grp.id = b.id + "Group";
        grp.label = b.label + " Tools";
        RibbonControl ctrl;
        ctrl.id = b.id + "Default";
        ctrl.label = b.label + " Commands";
        ctrl.imageMso = "FileSave";
        ctrl.size = "large";
        grp.controls.append(ctrl);
        tab.groups.append(grp);

        m_allTabs.append(tab);
    }

    // 2. Scan installed templates in Word Startup folders (both User Startup & System Startup)
    if (m_extManager) {
        auto items = m_extManager->scanWordExtensions();
        for (const auto& item : items) {
            // Ignore Word.officeUI here as it is handled separately
            if (item.fileName.endsWith(".officeUI", Qt::CaseInsensitive)) continue;

            if (item.hasCustomRibbonXml && !item.cachedRibbonXml.isEmpty()) {
                RibbonDefinition def;
                QString err;
                if (RibbonXmlParser::parse(item.cachedRibbonXml, def, err)) {
                    for (auto tab : def.tabs) {
                        tab.isBuiltIn = false;
                        tab.sourceOrigin = item.fileName;
                        tab.visible = item.enabled && !hiddenTabIdsInOfficeUI.contains(tab.id);

                        // Avoid duplicate tabs by ID or by Label
                        bool exists = false;
                        for (const auto& ex : m_allTabs) {
                            if (ex.id == tab.id || (ex.label.compare(tab.label, Qt::CaseInsensitive) == 0 && !ex.isBuiltIn)) {
                                exists = true;
                                break;
                            }
                        }
                        if (!exists && !tab.label.isEmpty()) {
                            m_allTabs.append(tab);
                        }
                    }
                }
            }
        }
    }

    // 3. Add custom tabs from Word.officeUI (like គូសរូបធរណីមាត្រ, ឡាតិច) that are not already from templates
    for (auto it = officeUiCustomTabs.begin(); it != officeUiCustomTabs.end(); ++it) {
        bool exists = false;
        for (const auto& ex : m_allTabs) {
            if (ex.id == it.key() || ex.label.compare(it.value().label, Qt::CaseInsensitive) == 0) {
                exists = true;
                break;
            }
        }
        if (!exists) {
            m_allTabs.append(it.value());
        }
    }
}

void AllRibbonTabsWidget::reloadAllRibbonTabs() {
    loadOfficeData();
    buildRibbonBarHeader();
    displayActiveTabContent();
    populateTabsTable();
    m_lblStatus->setText(QString("បានរកឃើញ និងរៀបចំផ្ទាំង Ribbon សរុបចំនួន %1 Tabs").arg(m_allTabs.size()));
}

void AllRibbonTabsWidget::setDarkMode(bool isDark) {
    m_isDarkMode = isDark;
    m_ribbonStripScroll->setStyleSheet(StyleHelper::getRibbonStripStyle(m_isDarkMode));
    m_canvasScroll->setStyleSheet(StyleHelper::getCanvasScrollStyle(m_isDarkMode));
    buildRibbonBarHeader();
    displayActiveTabContent();
    populateTabsTable();
}

void AllRibbonTabsWidget::buildRibbonBarHeader() {
    // Clear old buttons
    QLayoutItem* child;
    while ((child = m_ribbonStripLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    for (int i = 0; i < m_allTabs.size(); ++i) {
        const auto& tab = m_allTabs[i];
        auto* btnTab = new QPushButton(tab.label, m_ribbonStripContainer);
        btnTab->setCheckable(true);
        btnTab->setChecked(i == m_activeTabIndex);

        // Styling based on state and origin using StyleHelper
        QString style = StyleHelper::getTabButtonStyle(i == m_activeTabIndex, tab.visible, tab.isBuiltIn, m_isDarkMode);
        btnTab->setStyleSheet(style);
        btnTab->setCursor(Qt::PointingHandCursor);

        connect(btnTab, &QPushButton::clicked, this, [this, i]() {
            onTabButtonClicked(i);
        });

        m_ribbonStripLayout->addWidget(btnTab);
    }
}

void AllRibbonTabsWidget::displayActiveTabContent() {
    // Clear old canvas
    QLayoutItem* child;
    while ((child = m_canvasLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }

    if (m_activeTabIndex < 0 || m_activeTabIndex >= m_allTabs.size()) {
        return;
    }

    auto& tab = m_allTabs[m_activeTabIndex];

    // 1. Tab Action Bar (Header on the left of canvas)
    auto* tabHeaderWidget = new QWidget(m_canvasContainer);
    tabHeaderWidget->setStyleSheet(m_isDarkMode ? "background: #252631; border-radius: 6px; padding: 4px;" : "background: #e5e7eb; border-radius: 6px; padding: 4px;");
    auto* tabHeaderLayout = new QVBoxLayout(tabHeaderWidget);
    tabHeaderLayout->setContentsMargins(6, 6, 6, 6);
    tabHeaderLayout->setSpacing(6);

    auto* lblTabTitle = new QLabel("📑 " + tab.label, tabHeaderWidget);
    lblTabTitle->setStyleSheet("font-weight: bold; font-size: 13px; color: #0078d4;");
    tabHeaderLayout->addWidget(lblTabTitle);

    auto* btnAddGrp = new QPushButton("➕ បន្ថែម Group", tabHeaderWidget);
    btnAddGrp->setObjectName("btnSuccess");
    btnAddGrp->setStyleSheet("font-size: 11px; padding: 4px 8px;");
    connect(btnAddGrp, &QPushButton::clicked, this, &AllRibbonTabsWidget::onAddGroupToActiveTab);
    tabHeaderLayout->addWidget(btnAddGrp);

    auto* btnDelTab = new QPushButton("🗑️ លុប Tab នេះ", tabHeaderWidget);
    btnDelTab->setObjectName("btnDanger");
    btnDelTab->setStyleSheet("font-size: 11px; padding: 4px 8px;");
    connect(btnDelTab, &QPushButton::clicked, this, [this]() {
        onDeleteTabClicked(m_activeTabIndex);
    });
    tabHeaderLayout->addWidget(btnDelTab);

    tabHeaderLayout->addStretch();
    m_canvasLayout->addWidget(tabHeaderWidget);

    if (tab.groups.isEmpty()) {
        auto* lblEmpty = new QLabel("គ្មាន Group នៅក្នុង Tab នេះទេ (ចុច \"+ បន្ថែម Group\" ដើម្បីបង្កើត)", m_canvasContainer);
        lblEmpty->setStyleSheet(m_isDarkMode ? "color: #94a3b8; font-style: italic;" : "color: #6b7280; font-style: italic;");
        m_canvasLayout->addWidget(lblEmpty);
        m_canvasLayout->addStretch();
        return;
    }

    for (int g = 0; g < tab.groups.size(); ++g) {
        auto& grp = tab.groups[g];
        auto* grpBox = new QGroupBox(grp.label, m_canvasContainer);
        grpBox->setStyleSheet(StyleHelper::getGroupBoxCardStyle(m_isDarkMode));
        auto* grpLayout = new QVBoxLayout(grpBox);
        grpLayout->setContentsMargins(6, 10, 6, 6);
        grpLayout->setSpacing(6);

        // Group Toolbar (Add Button & Delete Group)
        auto* grpToolbar = new QHBoxLayout();
        grpToolbar->setContentsMargins(0, 0, 0, 0);

        auto* btnAddBtn = new QPushButton("➕ ប៊ូតុង", grpBox);
        btnAddBtn->setStyleSheet("background-color: #107c41; color: white; border-radius: 3px; font-size: 10px; padding: 2px 6px;");
        connect(btnAddBtn, &QPushButton::clicked, this, [this, g]() {
            onAddButtonToGroup(g);
        });

        auto* btnDelGrp = new QPushButton("🗑️", grpBox);
        btnDelGrp->setToolTip("លុប Group នេះ");
        btnDelGrp->setStyleSheet("background-color: #d83b01; color: white; border-radius: 3px; font-size: 10px; padding: 2px 6px;");
        connect(btnDelGrp, &QPushButton::clicked, this, [this, g]() {
            onDeleteGroupFromActiveTab(g);
        });

        grpToolbar->addWidget(btnAddBtn);
        grpToolbar->addWidget(btnDelGrp);
        grpToolbar->addStretch();
        grpLayout->addLayout(grpToolbar);

        // Buttons row
        auto* buttonsRow = new QHBoxLayout();
        buttonsRow->setContentsMargins(0, 0, 0, 0);
        buttonsRow->setSpacing(6);

        for (int c = 0; c < grp.controls.size(); ++c) {
            const auto& ctrl = grp.controls[c];
            if (ctrl.type == RibbonControlType::Separator) {
                auto* sep = new QFrame(grpBox);
                sep->setFrameShape(QFrame::VLine);
                sep->setFrameShadow(QFrame::Sunken);
                sep->setStyleSheet(m_isDarkMode ? "color: #3b3e52;" : "color: #e5e7eb;");
                buttonsRow->addWidget(sep);
                continue;
            }

            auto* btnContainer = new QWidget(grpBox);
            auto* btnLayout = new QHBoxLayout(btnContainer);
            btnLayout->setContentsMargins(0, 0, 0, 0);
            btnLayout->setSpacing(2);

            auto* btn = new QPushButton(ctrl.label, btnContainer);
            btn->setStyleSheet(StyleHelper::getActionButtonStyle(m_isDarkMode));
            if (!ctrl.screentip.isEmpty() || !ctrl.supertip.isEmpty()) {
                btn->setToolTip(ctrl.screentip + "\n" + ctrl.supertip);
            }
            btnLayout->addWidget(btn);

            // Small Delete button for this control
            auto* btnDelCtrl = new QPushButton("✕", btnContainer);
            btnDelCtrl->setFixedSize(18, 18);
            btnDelCtrl->setToolTip("លុបប៊ូតុង " + ctrl.label);
            btnDelCtrl->setStyleSheet("background-color: #fee2e2; color: #dc2626; border: 1px solid #fca5a5; border-radius: 9px; font-size: 10px; font-weight: bold;");
            connect(btnDelCtrl, &QPushButton::clicked, this, [this, g, c]() {
                onDeleteButtonFromGroup(g, c);
            });
            btnLayout->addWidget(btnDelCtrl);

            buttonsRow->addWidget(btnContainer);
        }

        grpLayout->addLayout(buttonsRow);
        m_canvasLayout->addWidget(grpBox);
    }
    m_canvasLayout->addStretch();
}

void AllRibbonTabsWidget::populateTabsTable() {
    m_tableTabs->setRowCount(m_allTabs.size());

    for (int i = 0; i < m_allTabs.size(); ++i) {
        const auto& tab = m_allTabs[i];

        // 0. Visibility checkbox
        auto* chkWidget = new QWidget(this);
        auto* chkLayout = new QHBoxLayout(chkWidget);
        chkLayout->setContentsMargins(0, 0, 0, 0);
        chkLayout->setAlignment(Qt::AlignCenter);
        auto* chk = new QCheckBox(chkWidget);
        chk->setChecked(tab.visible);
        connect(chk, &QCheckBox::toggled, this, [this, i]() {
            onToggleTabVisibility(i);
        });
        chkLayout->addWidget(chk);
        m_tableTabs->setCellWidget(i, 0, chkWidget);

        // 1. Label
        auto* lblItem = new QTableWidgetItem(tab.label);
        if (i == m_activeTabIndex) {
            lblItem->setBackground(QColor("#0078d4"));
            lblItem->setForeground(QColor("#ffffff"));
        }
        m_tableTabs->setItem(i, 1, lblItem);

        // 2. Origin
        auto* origItem = new QTableWidgetItem(tab.sourceOrigin);
        if (tab.isBuiltIn) {
            origItem->setForeground(QColor("#888888"));
        } else {
            origItem->setForeground(QColor("#0078d4"));
        }
        m_tableTabs->setItem(i, 2, origItem);

        // 3. Number of Groups
        auto* grpItem = new QTableWidgetItem(QString::number(tab.groups.size()));
        grpItem->setTextAlignment(Qt::AlignCenter);
        m_tableTabs->setItem(i, 3, grpItem);

        // 4. Number of Controls
        int ctrlCount = 0;
        for (const auto& g : tab.groups) ctrlCount += g.controls.size();
        auto* ctrlItem = new QTableWidgetItem(QString::number(ctrlCount));
        ctrlItem->setTextAlignment(Qt::AlignCenter);
        m_tableTabs->setItem(i, 4, ctrlItem);

        // 5. Actions (Move Left, Move Right, Edit)
        auto* actionWidget = new QWidget(this);
        auto* actLayout = new QHBoxLayout(actionWidget);
        actLayout->setContentsMargins(2, 2, 2, 2);
        actLayout->setSpacing(4);

        auto* btnLeft = new QPushButton("◀", actionWidget);
        btnLeft->setFixedWidth(30);
        btnLeft->setEnabled(i > 0);
        connect(btnLeft, &QPushButton::clicked, this, [this, i]() {
            onMoveTabLeft(i);
        });

        auto* btnRight = new QPushButton("▶", actionWidget);
        btnRight->setFixedWidth(30);
        btnRight->setEnabled(i < m_allTabs.size() - 1);
        connect(btnRight, &QPushButton::clicked, this, [this, i]() {
            onMoveTabRight(i);
        });

        auto* btnEdit = new QPushButton("✏️ កែ", actionWidget);
        connect(btnEdit, &QPushButton::clicked, this, [this, i]() {
            onEditTabInStudio(i);
        });

        auto* btnDelete = new QPushButton("🗑️ លុប", actionWidget);
        btnDelete->setObjectName("btnDanger");
        connect(btnDelete, &QPushButton::clicked, this, [this, i]() {
            onDeleteTabClicked(i);
        });

        actLayout->addWidget(btnLeft);
        actLayout->addWidget(btnRight);
        actLayout->addWidget(btnEdit);
        actLayout->addWidget(btnDelete);
        actLayout->addStretch();
        m_tableTabs->setCellWidget(i, 5, actionWidget);
    }
}

void AllRibbonTabsWidget::onTabButtonClicked(int index) {
    if (index >= 0 && index < m_allTabs.size()) {
        m_activeTabIndex = index;
        buildRibbonBarHeader();
        displayActiveTabContent();
        populateTabsTable();
    }
}

void AllRibbonTabsWidget::onToggleTabVisibility(int index) {
    if (index >= 0 && index < m_allTabs.size()) {
        m_allTabs[index].visible = !m_allTabs[index].visible;
        buildRibbonBarHeader();
        displayActiveTabContent();
    }
}

void AllRibbonTabsWidget::onMoveTabLeft(int index) {
    if (index > 0 && index < m_allTabs.size()) {
        m_allTabs.swapItemsAt(index, index - 1);
        m_activeTabIndex = index - 1;
        buildRibbonBarHeader();
        displayActiveTabContent();
        populateTabsTable();
    }
}

void AllRibbonTabsWidget::onMoveTabRight(int index) {
    if (index >= 0 && index < m_allTabs.size() - 1) {
        m_allTabs.swapItemsAt(index, index + 1);
        m_activeTabIndex = index + 1;
        buildRibbonBarHeader();
        displayActiveTabContent();
        populateTabsTable();
    }
}

void AllRibbonTabsWidget::onAddNewTabClicked() {
    bool ok = false;
    QString tabName = QInputDialog::getText(this, "បង្កើត Tab ថ្មី", "ឈ្មោះ Ribbon Tab (អក្សរខ្មែរ ឬ អង់គ្លេស):", QLineEdit::Normal, "ឧបករណ៍ថ្មី", &ok);
    if (!ok || tabName.trimmed().isEmpty()) return;

    RibbonTab tab;
    tab.id = "customTab_" + QString::number(m_allTabs.size() + 1);
    tab.label = tabName.trimmed();
    tab.visible = true;
    tab.isBuiltIn = false;
    tab.sourceOrigin = "User Created";

    RibbonGroup grp;
    grp.id = "grp_" + QString::number(m_allTabs.size() + 1);
    grp.label = "ឧបករណ៍ទូទៅ";

    RibbonControl btn;
    btn.id = "btnAction_1";
    btn.label = "ដំណើរការ";
    btn.imageMso = "MacroPlay";
    btn.size = "large";
    btn.onAction = "MyMacro";
    grp.controls.append(btn);

    tab.groups.append(grp);

    m_allTabs.append(tab);
    m_activeTabIndex = m_allTabs.size() - 1;

    buildRibbonBarHeader();
    displayActiveTabContent();
    populateTabsTable();
}

void AllRibbonTabsWidget::onDeleteTabClicked(int index) {
    if (index < 0 || index >= m_allTabs.size()) return;

    const auto& tab = m_allTabs[index];
    if (tab.isBuiltIn) {
        auto reply = QMessageBox::question(this, "លាក់ Tab ស្តង់ដារ Office", 
            "ផ្ទាំង \"" + tab.label + "\" គឺជា Tab ស្តង់ដាររបស់ Microsoft Office។\nតើអ្នកចង់លាក់ (Hide) Tab នេះពី Ribbon ដែរឬទេ?",
            QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            m_allTabs[index].visible = false;
            onSaveToOfficeUIClicked();
            buildRibbonBarHeader();
            displayActiveTabContent();
            populateTabsTable();
        }
        return;
    }

    // Custom or Template tab
    bool isFromTemplate = tab.sourceOrigin.endsWith(".dotm", Qt::CaseInsensitive) || tab.sourceOrigin.contains("Startup") || tab.sourceOrigin == "Office Add-in";

    QString msg;
    if (isFromTemplate) {
        msg = "ផ្ទាំង \"" + tab.label + "\" នេះដំណើរការចេញពី Add-in Template (" + tab.sourceOrigin + ")។\n\n"
              "តើអ្នកចង់លាក់ (Hide) Tab នេះពី Ribbon របស់ Word មែនទេ?\n"
              "(កម្មវិធីនឹងកំណត់វាជា Hidden ក្នុង Word.officeUI ធ្វើឱ្យវាមិនបង្ហាញលើ Word ទៀតឡើយ)";
    } else {
        msg = "តើអ្នកប្រាកដជាចង់លុប Tab \"" + tab.label + "\" នេះមែនទេ?";
    }

    auto reply = QMessageBox::question(this, "បញ្ជាក់ការលុប/លាក់ Tab", msg, QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (isFromTemplate) {
            m_allTabs[index].visible = false;
        } else {
            m_allTabs.removeAt(index);
        }
        if (m_activeTabIndex >= m_allTabs.size()) {
            m_activeTabIndex = qMax(0, m_allTabs.size() - 1);
        }
        onSaveToOfficeUIClicked();
        buildRibbonBarHeader();
        displayActiveTabContent();
        populateTabsTable();
        m_lblStatus->setText(QString("បានធ្វើបច្ចុប្បន្នភាព Tab រួចរាល់។"));
    }
}

void AllRibbonTabsWidget::onAddGroupToActiveTab() {
    if (m_activeTabIndex < 0 || m_activeTabIndex >= m_allTabs.size()) return;

    bool ok = false;
    QString grpName = QInputDialog::getText(this, "បង្កើត Group ថ្មី", "ឈ្មោះ Group (អក្សរខ្មែរ):", QLineEdit::Normal, "ក្រុមឧបករណ៍ថ្មី", &ok);
    if (!ok || grpName.trimmed().isEmpty()) return;

    RibbonGroup grp;
    grp.id = "grp_" + QString::number(m_allTabs[m_activeTabIndex].groups.size() + 1);
    grp.label = grpName.trimmed();

    RibbonControl btn;
    btn.id = "btn_" + QString::number(m_allTabs[m_activeTabIndex].groups.size() + 1) + "_1";
    btn.label = "ប៊ូតុង ១";
    btn.imageMso = "MacroPlay";
    btn.size = "large";
    btn.onAction = "MyMacroAction";
    grp.controls.append(btn);

    m_allTabs[m_activeTabIndex].groups.append(grp);
    displayActiveTabContent();
    populateTabsTable();
}

void AllRibbonTabsWidget::onDeleteGroupFromActiveTab(int groupIndex) {
    if (m_activeTabIndex < 0 || m_activeTabIndex >= m_allTabs.size()) return;
    auto& groups = m_allTabs[m_activeTabIndex].groups;
    if (groupIndex < 0 || groupIndex >= groups.size()) return;

    auto reply = QMessageBox::question(this, "បញ្ជាក់ការលុប Group", 
        "តើអ្នកប្រាកដជាចង់លុប Group \"" + groups[groupIndex].label + "\" និងប៊ូតុងទាំងអស់ក្នុង Group នេះមែនទេ?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        groups.removeAt(groupIndex);
        displayActiveTabContent();
        populateTabsTable();
    }
}

void AllRibbonTabsWidget::onAddButtonToGroup(int groupIndex) {
    if (m_activeTabIndex < 0 || m_activeTabIndex >= m_allTabs.size()) return;
    auto& groups = m_allTabs[m_activeTabIndex].groups;
    if (groupIndex < 0 || groupIndex >= groups.size()) return;

    QDialog dlg(this);
    dlg.setWindowTitle("បន្ថែមប៊ូតុងថ្មីលើ Ribbon (Add Button)");
    dlg.resize(420, 240);

    auto* layout = new QVBoxLayout(&dlg);
    auto* form = new QFormLayout();

    auto* editLabel = new QLineEdit(&dlg);
    editLabel->setText("ប៊ូតុងថ្មី");
    form->addRow("ឈ្មោះប៊ូតុង (Label ខ្មែរ):", editLabel);

    auto* editAction = new QLineEdit(&dlg);
    editAction->setText("MyMacroCallback");
    form->addRow("Macro Callback (onAction):", editAction);

    auto* comboIcon = new QComboBox(&dlg);
    comboIcon->setEditable(true);
    comboIcon->addItems({"MacroPlay", "FileSave", "Info", "Help", "FontColorPicker", "DiagramTargetInsertClassic", "BlackAndWhiteInverseGrayscale", "Pushpin", "Lock"});
    form->addRow("Office Icon (imageMso):", comboIcon);

    auto* editTip = new QLineEdit(&dlg);
    editTip->setText("ចុចដើម្បីដំណើរការ");
    form->addRow("Screentip (Tooltip ខ្លី):", editTip);

    layout->addLayout(form);

    auto* btnBox = new QHBoxLayout();
    auto* btnOk = new QPushButton("✅ បន្ថែម", &dlg);
    auto* btnCancel = new QPushButton("បោះបង់", &dlg);
    btnCancel->setObjectName("btnSecondary");
    btnBox->addStretch();
    btnBox->addWidget(btnOk);
    btnBox->addWidget(btnCancel);
    layout->addLayout(btnBox);

    connect(btnOk, &QPushButton::clicked, &dlg, &QDialog::accept);
    connect(btnCancel, &QPushButton::clicked, &dlg, &QDialog::reject);

    if (dlg.exec() == QDialog::Accepted) {
        RibbonControl btn;
        int count = groups[groupIndex].controls.size() + 1;
        btn.id = "btnCustom_" + QString::number(groupIndex + 1) + "_" + QString::number(count);
        btn.label = editLabel->text().trimmed();
        btn.onAction = editAction->text().trimmed();
        btn.imageMso = comboIcon->currentText().trimmed();
        btn.screentip = editTip->text().trimmed();
        btn.size = "large";

        groups[groupIndex].controls.append(btn);
        displayActiveTabContent();
        populateTabsTable();
    }
}

void AllRibbonTabsWidget::onDeleteButtonFromGroup(int groupIndex, int buttonIndex) {
    if (m_activeTabIndex < 0 || m_activeTabIndex >= m_allTabs.size()) return;
    auto& groups = m_allTabs[m_activeTabIndex].groups;
    if (groupIndex < 0 || groupIndex >= groups.size()) return;
    if (buttonIndex < 0 || buttonIndex >= groups[groupIndex].controls.size()) return;

    QString btnName = groups[groupIndex].controls[buttonIndex].label;
    auto reply = QMessageBox::question(this, "បញ្ជាក់ការលុបប៊ូតុង", 
        "តើអ្នកប្រាកដជាចង់លុបប៊ូតុង \"" + btnName + "\" នេះមែនទេ?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        groups[groupIndex].controls.removeAt(buttonIndex);
        displayActiveTabContent();
        populateTabsTable();
    }
}

void AllRibbonTabsWidget::onEditTabInStudio(int index) {
    if (index < 0 || index >= m_allTabs.size()) return;

    const auto& tab = m_allTabs[index];
    RibbonDefinition def;
    def.tabs.append(tab);

    QString xml = RibbonXmlGenerator::generateXml(def);
    if (m_ribbonEditor) {
        m_ribbonEditor->loadXml(xml, tab.label);
        QMessageBox::information(this, "បើក Ribbon Studio", 
            "បានបញ្ជូន Tab \"" + tab.label + "\" ទៅកាន់ផ្ទាំង Ribbon XML Studio រួចរាល់!\nសូមចុចទៅ Tab \"Ribbon XML Studio\" ដើម្បីកែប្រែប៊ូតុង ឬកូដ XML។");
    }
}

void AllRibbonTabsWidget::onSaveToOfficeUIClicked() {
    if (!m_extManager || !m_extManager->platformService()) return;

    QString officeUIPath = m_extManager->platformService()->getWordOfficeUIPath();
    if (officeUIPath.isEmpty()) {
        QMessageBox::warning(this, "កំហុស", "រកមិនឃើញទីតាំង Word.officeUI លើប្រព័ន្ធនេះទេ!");
        return;
    }

    QString xml;
    QTextStream out(&xml);
    out << "<mso:customUI xmlns:mso=\"http://schemas.microsoft.com/office/2009/07/customui\">\n";
    out << "  <mso:ribbon>\n";
    out << "    <mso:tabs>\n";

    for (const auto& tab : m_allTabs) {
        if (tab.isBuiltIn) {
            // Built-in tabs (Home, Insert, Draw, etc.): only write if explicitly hidden
            if (!tab.visible) {
                out << "      <mso:tab idQ=\"mso:" << tab.id << "\" visible=\"false\"/>\n";
            }
        } else if (tab.sourceOrigin.endsWith(".dotm", Qt::CaseInsensitive) || tab.sourceOrigin.contains("Startup") || tab.sourceOrigin == "Office Add-in") {
            // Tabs loaded from template add-ins (Mathtype_kh.dotm, WordMat, MathCapture, MathEditor)
            if (!tab.visible) {
                // EXPLICITLY tell Word to HIDE this template tab!
                out << "      <mso:tab id=\"" << tab.id << "\" visible=\"false\"/>\n";
            }
            // IMPORTANT: If tab.visible is true, DO NOT write <mso:tab> into Word.officeUI!
            // The template already creates the tab. Writing it here creates a duplicate tab!
        } else {
            // User-created tab or custom OfficeUI tab (e.g. គូសរូបធរណីមាត្រ, ឡាតិច)
            out << "      <mso:tab id=\"" << tab.id << "\" label=\"" << tab.label << "\"";
            if (!tab.visible) {
                out << " visible=\"false\"";
            }
            out << ">\n";

            for (const auto& grp : tab.groups) {
                out << "        <mso:group id=\"" << grp.id << "\" label=\"" << grp.label << "\" autoScale=\"true\">\n";
                for (const auto& ctrl : grp.controls) {
                    if (ctrl.type == RibbonControlType::Separator) {
                        out << "          <mso:separator id=\"" << ctrl.id << "\"/>\n";
                    } else {
                        out << "          <mso:button id=\"" << ctrl.id << "\" label=\"" << ctrl.label << "\"";
                        if (!ctrl.imageMso.isEmpty()) out << " imageMso=\"" << ctrl.imageMso << "\"";
                        if (!ctrl.onAction.isEmpty()) out << " onAction=\"" << ctrl.onAction << "\"";
                        if (!ctrl.screentip.isEmpty()) out << " screentip=\"" << ctrl.screentip << "\"";
                        out << " size=\"large\" visible=\"true\"/>\n";
                    }
                }
                out << "        </mso:group>\n";
            }
            out << "      </mso:tab>\n";
        }
    }

    out << "    </mso:tabs>\n";
    out << "  </mso:ribbon>\n";
    out << "</mso:customUI>\n";

    QFile file(officeUIPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QMessageBox::critical(this, "កំហុសរក្សាទុក", "មិនអាចសរសេរចូល file:\n" + officeUIPath);
        return;
    }

    file.write(xml.toUtf8());
    file.close();

}

void AllRibbonTabsWidget::onOpenWordClicked() {
#ifdef Q_OS_MACOS
    QProcess::startDetached("/usr/bin/open", {"-a", "Microsoft Word"});
#elif defined(Q_OS_WIN)
    QProcess::startDetached("winword.exe", {});
#endif
}
