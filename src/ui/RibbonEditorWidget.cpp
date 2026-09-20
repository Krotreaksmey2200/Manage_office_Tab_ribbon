#include "RibbonEditorWidget.h"
#include "RibbonXmlParser.h"
#include "RibbonXmlGenerator.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSplitter>
#include <QFormLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QClipboard>
#include <QApplication>
#include <QHeaderView>

RibbonEditorWidget::RibbonEditorWidget(QWidget* parent)
    : QWidget(parent) {
    setupUi();
    // Default initial template
    loadXml(RibbonXmlGenerator::generateDefaultKhmerTabXml(), "គំរូដើម (Default Khmer Tab)");
}

void RibbonEditorWidget::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(8);

    // Top Bar info
    auto* topBarLayout = new QHBoxLayout();
    m_lblCurrentFile = new QLabel("ឯកសារបច្ចុប្បន្ន: គ្មាន", this);
    m_lblCurrentFile->setStyleSheet("font-weight: bold; color: #0078d4; font-size: 14px;");
    topBarLayout->addWidget(m_lblCurrentFile);
    topBarLayout->addStretch();

    auto* btnKhmerTpl = new QPushButton("✨ បញ្ចូលគំរូខ្មែរ (Khmer Template)", this);
    btnKhmerTpl->setObjectName("btnSuccess");
    connect(btnKhmerTpl, &QPushButton::clicked, this, &RibbonEditorWidget::onInsertKhmerTemplateClicked);
    topBarLayout->addWidget(btnKhmerTpl);

    mainLayout->addLayout(topBarLayout);

    // Main horizontal splitter
    auto* splitter = new QSplitter(Qt::Horizontal, this);

    // ================= LEFT: Tree and controls =================
    auto* leftContainer = new QWidget(this);
    auto* leftLayout = new QVBoxLayout(leftContainer);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    auto* treeToolbar = new QHBoxLayout();
    auto* btnAddTab = new QPushButton("+ Tab", this);
    auto* btnAddGroup = new QPushButton("+ Group", this);
    auto* btnAddButton = new QPushButton("+ Button", this);
    auto* btnAddSep = new QPushButton("+ Sep", this);
    auto* btnDelete = new QPushButton("លុប", this);
    btnDelete->setObjectName("btnDanger");
    auto* btnUp = new QPushButton("▲", this);
    btnUp->setObjectName("btnSecondary");
    auto* btnDown = new QPushButton("▼", this);
    btnDown->setObjectName("btnSecondary");

    treeToolbar->addWidget(btnAddTab);
    treeToolbar->addWidget(btnAddGroup);
    treeToolbar->addWidget(btnAddButton);
    treeToolbar->addWidget(btnAddSep);
    treeToolbar->addWidget(btnDelete);
    treeToolbar->addWidget(btnUp);
    treeToolbar->addWidget(btnDown);
    leftLayout->addLayout(treeToolbar);

    connect(btnAddTab, &QPushButton::clicked, this, &RibbonEditorWidget::onAddTabClicked);
    connect(btnAddGroup, &QPushButton::clicked, this, &RibbonEditorWidget::onAddGroupClicked);
    connect(btnAddButton, &QPushButton::clicked, this, &RibbonEditorWidget::onAddButtonClicked);
    connect(btnAddSep, &QPushButton::clicked, this, &RibbonEditorWidget::onAddSeparatorClicked);
    connect(btnDelete, &QPushButton::clicked, this, &RibbonEditorWidget::onDeleteItemClicked);
    connect(btnUp, &QPushButton::clicked, this, &RibbonEditorWidget::onMoveUpClicked);
    connect(btnDown, &QPushButton::clicked, this, &RibbonEditorWidget::onMoveDownClicked);

    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setHeaderLabels({"រចនាសម្ព័ន្ធ Ribbon", "ប្រភេទ", "ID", "Macro Callback"});
    m_treeWidget->header()->resizeSection(0, 220);
    m_treeWidget->header()->resizeSection(1, 90);
    m_treeWidget->header()->resizeSection(2, 130);
    connect(m_treeWidget, &QTreeWidget::itemSelectionChanged, this, &RibbonEditorWidget::onTreeItemSelectionChanged);
    leftLayout->addWidget(m_treeWidget);

    // ================= CENTER: Property Editor =================
    m_propPanel = new QWidget(this);
    auto* propLayout = new QVBoxLayout(m_propPanel);
    propLayout->setContentsMargins(0, 0, 0, 0);

    auto* grpProps = new QGroupBox("ផ្ទាំងកែសម្រួលលក្ខណៈ (Property Inspector)", this);
    auto* formLayout = new QFormLayout(grpProps);

    m_lblItemType = new QLabel("ជ្រើសរើស Item ដើម្បីកែ", this);
    m_lblItemType->setStyleSheet("font-weight: bold; color: #ffb900;");
    formLayout->addRow("ប្រភេទ:", m_lblItemType);

    m_editId = new QLineEdit(this);
    formLayout->addRow("ID:", m_editId);

    m_editLabel = new QLineEdit(this);
    formLayout->addRow("ឈ្មោះ (Label - ខ្មែរ):", m_editLabel);

    m_comboSize = new QComboBox(this);
    m_comboSize->addItems({"large", "normal"});
    formLayout->addRow("ទំហំប៊ូតុង (Size):", m_comboSize);

    m_editImageMso = new QLineEdit(this);
    m_editImageMso->setPlaceholderText("ឧ. MacroPlay, FileSave, Help");
    formLayout->addRow("Office Icon (imageMso):", m_editImageMso);

    m_editCustomImage = new QLineEdit(this);
    m_editCustomImage->setPlaceholderText("ឧ. logo, edit_icon");
    formLayout->addRow("រូបតំណាងផ្ទាល់ខ្លួន (Image):", m_editCustomImage);

    m_editOnAction = new QLineEdit(this);
    m_editOnAction->setPlaceholderText("ឈ្មោះ Macro VBA");
    formLayout->addRow("Action / Macro Callback:", m_editOnAction);

    m_editScreentip = new QLineEdit(this);
    formLayout->addRow("Screentip (Tooltip ខ្លី):", m_editScreentip);

    m_editSupertip = new QLineEdit(this);
    formLayout->addRow("Supertip (ការពន្យល់លម្អិត):", m_editSupertip);

    m_chkVisible = new QCheckBox("បង្ហាញ (Visible)", this);
    m_chkVisible->setChecked(true);
    m_chkEnabled = new QCheckBox("បើកដំណើរការ (Enabled)", this);
    m_chkEnabled->setChecked(true);

    auto* chkLayout = new QHBoxLayout();
    chkLayout->addWidget(m_chkVisible);
    chkLayout->addWidget(m_chkEnabled);
    formLayout->addRow("", chkLayout);

    m_btnApplyProps = new QPushButton("💾 អនុវត្តការកែប្រែ (Save Properties)", this);
    connect(m_btnApplyProps, &QPushButton::clicked, this, &RibbonEditorWidget::onApplyPropertyChanges);
    formLayout->addRow("", m_btnApplyProps);

    propLayout->addWidget(grpProps);
    propLayout->addStretch();

    // ================= RIGHT: Live XML Editor =================
    auto* rightContainer = new QWidget(this);
    auto* rightLayout = new QVBoxLayout(rightContainer);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    auto* xmlHeaderLayout = new QHBoxLayout();
    auto* lblXml = new QLabel("កូដ Ribbon CustomUI XML (Live)", this);
    lblXml->setStyleSheet("font-weight: bold; color: #0078d4;");
    xmlHeaderLayout->addWidget(lblXml);
    xmlHeaderLayout->addStretch();

    auto* btnSyncXml = new QPushButton("🔄 Sync XML ទៅ Tree", this);
    btnSyncXml->setObjectName("btnSecondary");
    connect(btnSyncXml, &QPushButton::clicked, this, &RibbonEditorWidget::onSyncFromXmlClicked);
    xmlHeaderLayout->addWidget(btnSyncXml);

    auto* btnCopyXml = new QPushButton("📋 ចម្លង (Copy)", this);
    btnCopyXml->setObjectName("btnSecondary");
    connect(btnCopyXml, &QPushButton::clicked, this, &RibbonEditorWidget::onCopyXmlClicked);
    xmlHeaderLayout->addWidget(btnCopyXml);

    auto* btnExportXml = new QPushButton("💾 Export...", this);
    btnExportXml->setObjectName("btnSecondary");
    connect(btnExportXml, &QPushButton::clicked, this, &RibbonEditorWidget::onExportXmlClicked);
    xmlHeaderLayout->addWidget(btnExportXml);

    rightLayout->addLayout(xmlHeaderLayout);

    m_xmlEditor = new QPlainTextEdit(this);
    m_xmlEditor->setFont(QFont("Menlo", 12));
    m_xmlHighlighter = new XmlHighlighter(m_xmlEditor->document());
    rightLayout->addWidget(m_xmlEditor);

    // Add widgets to splitter
    splitter->addWidget(leftContainer);
    splitter->addWidget(m_propPanel);
    splitter->addWidget(rightContainer);
    splitter->setStretchFactor(0, 3);
    splitter->setStretchFactor(1, 2);
    splitter->setStretchFactor(2, 4);

    mainLayout->addWidget(splitter);
}

void RibbonEditorWidget::loadXml(const QString& xmlContent, const QString& sourceFileName) {
    m_currentFileName = sourceFileName;
    if (!sourceFileName.isEmpty()) {
        m_lblCurrentFile->setText("ឯកសារបច្ចុប្បន្ន: " + sourceFileName);
    } else {
        m_lblCurrentFile->setText("ឯកសារបច្ចុប្បន្ន: គ្មាន");
    }

    m_isUpdatingFromCode = true;
    m_xmlEditor->setPlainText(xmlContent);
    m_isUpdatingFromCode = false;

    QString error;
    if (RibbonXmlParser::parse(xmlContent, m_definition, error)) {
        populateTreeFromDefinition();
    } else {
        m_treeWidget->clear();
    }
}

QString RibbonEditorWidget::getXml() const {
    return m_xmlEditor->toPlainText();
}

void RibbonEditorWidget::populateTreeFromDefinition() {
    m_treeWidget->clear();

    for (const RibbonTab& tab : m_definition.tabs) {
        auto* tabItem = new QTreeWidgetItem(m_treeWidget);
        tabItem->setText(0, "📑 " + (tab.label.isEmpty() ? tab.id : tab.label));
        tabItem->setText(1, "Tab");
        tabItem->setText(2, tab.id);
        tabItem->setData(0, Qt::UserRole, "tab");
        tabItem->setData(0, Qt::UserRole + 1, tab.id);
        tabItem->setData(0, Qt::UserRole + 2, tab.label);
        tabItem->setData(0, Qt::UserRole + 10, tab.visible);

        for (const RibbonGroup& grp : tab.groups) {
            auto* grpItem = new QTreeWidgetItem(tabItem);
            grpItem->setText(0, "📁 " + (grp.label.isEmpty() ? grp.id : grp.label));
            grpItem->setText(1, "Group");
            grpItem->setText(2, grp.id);
            grpItem->setData(0, Qt::UserRole, "group");
            grpItem->setData(0, Qt::UserRole + 1, grp.id);
            grpItem->setData(0, Qt::UserRole + 2, grp.label);

            for (const RibbonControl& ctrl : grp.controls) {
                auto* ctrlItem = new QTreeWidgetItem(grpItem);
                QString typeStr = (ctrl.type == RibbonControlType::Separator) ? "Separator" : "Button";
                QString iconPrefix = (ctrl.type == RibbonControlType::Separator) ? "➖ " : "🔘 ";
                ctrlItem->setText(0, iconPrefix + (ctrl.label.isEmpty() ? ctrl.id : ctrl.label));
                ctrlItem->setText(1, typeStr);
                ctrlItem->setText(2, ctrl.id);
                ctrlItem->setText(3, ctrl.onAction);

                ctrlItem->setData(0, Qt::UserRole, (ctrl.type == RibbonControlType::Separator) ? "separator" : "button");
                ctrlItem->setData(0, Qt::UserRole + 1, ctrl.id);
                ctrlItem->setData(0, Qt::UserRole + 2, ctrl.label);
                ctrlItem->setData(0, Qt::UserRole + 3, ctrl.size);
                ctrlItem->setData(0, Qt::UserRole + 4, ctrl.imageMso);
                ctrlItem->setData(0, Qt::UserRole + 5, ctrl.image);
                ctrlItem->setData(0, Qt::UserRole + 6, ctrl.onAction);
                ctrlItem->setData(0, Qt::UserRole + 7, ctrl.screentip);
                ctrlItem->setData(0, Qt::UserRole + 8, ctrl.supertip);
                ctrlItem->setData(0, Qt::UserRole + 9, ctrl.enabled);
                ctrlItem->setData(0, Qt::UserRole + 10, ctrl.visible);
            }
        }
    }
    m_treeWidget->expandAll();
}

RibbonDefinition RibbonEditorWidget::buildDefinitionFromTree() {
    RibbonDefinition def;
    def.namespaceUri = m_definition.namespaceUri;

    for (int t = 0; t < m_treeWidget->topLevelItemCount(); ++t) {
        QTreeWidgetItem* tabItem = m_treeWidget->topLevelItem(t);
        RibbonTab tab;
        tab.id = tabItem->data(0, Qt::UserRole + 1).toString();
        tab.label = tabItem->data(0, Qt::UserRole + 2).toString();
        tab.visible = tabItem->data(0, Qt::UserRole + 10).toBool();

        for (int g = 0; g < tabItem->childCount(); ++g) {
            QTreeWidgetItem* grpItem = tabItem->child(g);
            RibbonGroup grp;
            grp.id = grpItem->data(0, Qt::UserRole + 1).toString();
            grp.label = grpItem->data(0, Qt::UserRole + 2).toString();

            for (int c = 0; c < grpItem->childCount(); ++c) {
                QTreeWidgetItem* ctrlItem = grpItem->child(c);
                RibbonControl ctrl;
                QString role = ctrlItem->data(0, Qt::UserRole).toString();
                if (role == "separator") {
                    ctrl.type = RibbonControlType::Separator;
                } else {
                    ctrl.type = RibbonControlType::Button;
                }
                ctrl.id = ctrlItem->data(0, Qt::UserRole + 1).toString();
                ctrl.label = ctrlItem->data(0, Qt::UserRole + 2).toString();
                ctrl.size = ctrlItem->data(0, Qt::UserRole + 3).toString();
                ctrl.imageMso = ctrlItem->data(0, Qt::UserRole + 4).toString();
                ctrl.image = ctrlItem->data(0, Qt::UserRole + 5).toString();
                ctrl.onAction = ctrlItem->data(0, Qt::UserRole + 6).toString();
                ctrl.screentip = ctrlItem->data(0, Qt::UserRole + 7).toString();
                ctrl.supertip = ctrlItem->data(0, Qt::UserRole + 8).toString();
                ctrl.enabled = ctrlItem->data(0, Qt::UserRole + 9).toBool();
                ctrl.visible = ctrlItem->data(0, Qt::UserRole + 10).toBool();

                grp.controls.append(ctrl);
            }
            tab.groups.append(grp);
        }
        def.tabs.append(tab);
    }
    return def;
}

void RibbonEditorWidget::updateXmlPreview() {
    m_definition = buildDefinitionFromTree();
    QString xml = RibbonXmlGenerator::generateXml(m_definition);
    m_isUpdatingFromCode = true;
    m_xmlEditor->setPlainText(xml);
    m_isUpdatingFromCode = false;
    emit ribbonXmlSaved(xml);
}

void RibbonEditorWidget::onTreeItemSelectionChanged() {
    auto items = m_treeWidget->selectedItems();
    if (items.isEmpty()) return;

    QTreeWidgetItem* item = items.first();
    QString type = item->data(0, Qt::UserRole).toString();
    m_lblItemType->setText(type.toUpper());

    m_editId->setText(item->data(0, Qt::UserRole + 1).toString());
    m_editLabel->setText(item->data(0, Qt::UserRole + 2).toString());

    bool isButton = (type == "button");
    m_comboSize->setEnabled(isButton);
    m_comboSize->setCurrentText(item->data(0, Qt::UserRole + 3).toString().isEmpty() ? "large" : item->data(0, Qt::UserRole + 3).toString());
    m_editImageMso->setEnabled(isButton);
    m_editImageMso->setText(item->data(0, Qt::UserRole + 4).toString());
    m_editCustomImage->setEnabled(isButton);
    m_editCustomImage->setText(item->data(0, Qt::UserRole + 5).toString());
    m_editOnAction->setEnabled(isButton);
    m_editOnAction->setText(item->data(0, Qt::UserRole + 6).toString());
    m_editScreentip->setEnabled(isButton);
    m_editScreentip->setText(item->data(0, Qt::UserRole + 7).toString());
    m_editSupertip->setEnabled(isButton);
    m_editSupertip->setText(item->data(0, Qt::UserRole + 8).toString());

    m_chkEnabled->setChecked(item->data(0, Qt::UserRole + 9).isValid() ? item->data(0, Qt::UserRole + 9).toBool() : true);
    m_chkVisible->setChecked(item->data(0, Qt::UserRole + 10).isValid() ? item->data(0, Qt::UserRole + 10).toBool() : true);
}

void RibbonEditorWidget::onApplyPropertyChanges() {
    auto items = m_treeWidget->selectedItems();
    if (items.isEmpty()) return;

    QTreeWidgetItem* item = items.first();
    QString type = item->data(0, Qt::UserRole).toString();

    item->setData(0, Qt::UserRole + 1, m_editId->text());
    item->setData(0, Qt::UserRole + 2, m_editLabel->text());
    item->setText(2, m_editId->text());

    if (type == "tab") {
        item->setText(0, "📑 " + m_editLabel->text());
    } else if (type == "group") {
        item->setText(0, "📁 " + m_editLabel->text());
    } else if (type == "button") {
        item->setText(0, "🔘 " + m_editLabel->text());
        item->setText(3, m_editOnAction->text());
        item->setData(0, Qt::UserRole + 3, m_comboSize->currentText());
        item->setData(0, Qt::UserRole + 4, m_editImageMso->text());
        item->setData(0, Qt::UserRole + 5, m_editCustomImage->text());
        item->setData(0, Qt::UserRole + 6, m_editOnAction->text());
        item->setData(0, Qt::UserRole + 7, m_editScreentip->text());
        item->setData(0, Qt::UserRole + 8, m_editSupertip->text());
        item->setData(0, Qt::UserRole + 9, m_chkEnabled->isChecked());
    }
    item->setData(0, Qt::UserRole + 10, m_chkVisible->isChecked());

    updateXmlPreview();
}

void RibbonEditorWidget::onAddTabClicked() {
    int index = m_treeWidget->topLevelItemCount() + 1;
    auto* tabItem = new QTreeWidgetItem(m_treeWidget);
    QString id = QString("customTab_%1").arg(index);
    QString label = QString("ផ្ទាំងថ្មី %1").arg(index);

    tabItem->setText(0, "📑 " + label);
    tabItem->setText(1, "Tab");
    tabItem->setText(2, id);
    tabItem->setData(0, Qt::UserRole, "tab");
    tabItem->setData(0, Qt::UserRole + 1, id);
    tabItem->setData(0, Qt::UserRole + 2, label);
    tabItem->setData(0, Qt::UserRole + 10, true);

    m_treeWidget->setCurrentItem(tabItem);
    updateXmlPreview();
}

void RibbonEditorWidget::onAddGroupClicked() {
    auto items = m_treeWidget->selectedItems();
    QTreeWidgetItem* targetTab = nullptr;

    if (!items.isEmpty()) {
        QTreeWidgetItem* cur = items.first();
        if (cur->data(0, Qt::UserRole).toString() == "tab") {
            targetTab = cur;
        } else if (cur->parent() && cur->parent()->data(0, Qt::UserRole).toString() == "tab") {
            targetTab = cur->parent();
        }
    }

    if (!targetTab && m_treeWidget->topLevelItemCount() > 0) {
        targetTab = m_treeWidget->topLevelItem(0);
    }

    if (!targetTab) {
        onAddTabClicked();
        targetTab = m_treeWidget->topLevelItem(0);
    }

    int index = targetTab->childCount() + 1;
    auto* grpItem = new QTreeWidgetItem(targetTab);
    QString id = QString("grp_%1").arg(index);
    QString label = QString("ក្រុមឧបករណ៍ %1").arg(index);

    grpItem->setText(0, "📁 " + label);
    grpItem->setText(1, "Group");
    grpItem->setText(2, id);
    grpItem->setData(0, Qt::UserRole, "group");
    grpItem->setData(0, Qt::UserRole + 1, id);
    grpItem->setData(0, Qt::UserRole + 2, label);

    targetTab->setExpanded(true);
    m_treeWidget->setCurrentItem(grpItem);
    updateXmlPreview();
}

void RibbonEditorWidget::onAddButtonClicked() {
    auto items = m_treeWidget->selectedItems();
    QTreeWidgetItem* targetGroup = nullptr;

    if (!items.isEmpty()) {
        QTreeWidgetItem* cur = items.first();
        if (cur->data(0, Qt::UserRole).toString() == "group") {
            targetGroup = cur;
        } else if (cur->parent() && cur->parent()->data(0, Qt::UserRole).toString() == "group") {
            targetGroup = cur->parent();
        }
    }

    if (!targetGroup) {
        if (m_treeWidget->topLevelItemCount() == 0) {
            onAddTabClicked();
        }
        QTreeWidgetItem* tab = m_treeWidget->topLevelItem(0);
        if (tab->childCount() == 0) {
            onAddGroupClicked();
        }
        targetGroup = tab->child(0);
    }

    int index = targetGroup->childCount() + 1;
    auto* btnItem = new QTreeWidgetItem(targetGroup);
    QString id = QString("btnAction_%1").arg(index);
    QString label = QString("ប៊ូតុងថ្មី %1").arg(index);

    btnItem->setText(0, "🔘 " + label);
    btnItem->setText(1, "Button");
    btnItem->setText(2, id);
    btnItem->setText(3, "Macro" + QString::number(index));

    btnItem->setData(0, Qt::UserRole, "button");
    btnItem->setData(0, Qt::UserRole + 1, id);
    btnItem->setData(0, Qt::UserRole + 2, label);
    btnItem->setData(0, Qt::UserRole + 3, "large");
    btnItem->setData(0, Qt::UserRole + 4, "MacroPlay");
    btnItem->setData(0, Qt::UserRole + 6, "Macro" + QString::number(index));
    btnItem->setData(0, Qt::UserRole + 7, label);
    btnItem->setData(0, Qt::UserRole + 8, "ពន្យល់អំពី " + label);
    btnItem->setData(0, Qt::UserRole + 9, true);
    btnItem->setData(0, Qt::UserRole + 10, true);

    targetGroup->setExpanded(true);
    m_treeWidget->setCurrentItem(btnItem);
    updateXmlPreview();
}

void RibbonEditorWidget::onAddSeparatorClicked() {
    auto items = m_treeWidget->selectedItems();
    QTreeWidgetItem* targetGroup = nullptr;

    if (!items.isEmpty()) {
        QTreeWidgetItem* cur = items.first();
        if (cur->data(0, Qt::UserRole).toString() == "group") {
            targetGroup = cur;
        } else if (cur->parent()) {
            targetGroup = cur->parent();
        }
    }
    if (!targetGroup) return;

    int index = targetGroup->childCount() + 1;
    auto* sepItem = new QTreeWidgetItem(targetGroup);
    QString id = QString("sep_%1").arg(index);

    sepItem->setText(0, "➖ បន្ទាត់ខណ្ឌ (Separator)");
    sepItem->setText(1, "Separator");
    sepItem->setText(2, id);
    sepItem->setData(0, Qt::UserRole, "separator");
    sepItem->setData(0, Qt::UserRole + 1, id);

    targetGroup->setExpanded(true);
    m_treeWidget->setCurrentItem(sepItem);
    updateXmlPreview();
}

void RibbonEditorWidget::onDeleteItemClicked() {
    auto items = m_treeWidget->selectedItems();
    if (items.isEmpty()) return;

    QTreeWidgetItem* item = items.first();
    delete item;
    updateXmlPreview();
}

void RibbonEditorWidget::onMoveUpClicked() {
    auto items = m_treeWidget->selectedItems();
    if (items.isEmpty()) return;

    QTreeWidgetItem* item = items.first();
    QTreeWidgetItem* parent = item->parent();

    if (parent) {
        int idx = parent->indexOfChild(item);
        if (idx > 0) {
            parent->takeChild(idx);
            parent->insertChild(idx - 1, item);
            m_treeWidget->setCurrentItem(item);
            updateXmlPreview();
        }
    } else {
        int idx = m_treeWidget->indexOfTopLevelItem(item);
        if (idx > 0) {
            m_treeWidget->takeTopLevelItem(idx);
            m_treeWidget->insertTopLevelItem(idx - 1, item);
            m_treeWidget->setCurrentItem(item);
            updateXmlPreview();
        }
    }
}

void RibbonEditorWidget::onMoveDownClicked() {
    auto items = m_treeWidget->selectedItems();
    if (items.isEmpty()) return;

    QTreeWidgetItem* item = items.first();
    QTreeWidgetItem* parent = item->parent();

    if (parent) {
        int idx = parent->indexOfChild(item);
        if (idx < parent->childCount() - 1) {
            parent->takeChild(idx);
            parent->insertChild(idx + 1, item);
            m_treeWidget->setCurrentItem(item);
            updateXmlPreview();
        }
    } else {
        int idx = m_treeWidget->indexOfTopLevelItem(item);
        if (idx < m_treeWidget->topLevelItemCount() - 1) {
            m_treeWidget->takeTopLevelItem(idx);
            m_treeWidget->insertTopLevelItem(idx + 1, item);
            m_treeWidget->setCurrentItem(item);
            updateXmlPreview();
        }
    }
}

void RibbonEditorWidget::onSyncFromXmlClicked() {
    QString xml = m_xmlEditor->toPlainText();
    QString error;
    RibbonDefinition def;
    if (RibbonXmlParser::parse(xml, def, error)) {
        m_definition = def;
        populateTreeFromDefinition();
        QMessageBox::information(this, "ជោគជ័យ", "បានធ្វើសមកាលកម្ម XML ទៅកាន់ Visual Tree ដោយជោគជ័យ!");
    } else {
        QMessageBox::warning(this, "កំហុស XML", "មិនអាច parse XML បានទេ:\n" + error);
    }
}

void RibbonEditorWidget::onInsertKhmerTemplateClicked() {
    loadXml(RibbonXmlGenerator::generateDefaultKhmerTabXml(), "គំរូឧបករណ៍ខ្មែរ (Khmer Tools Tab)");
}

void RibbonEditorWidget::onExportXmlClicked() {
    QString fileName = QFileDialog::getSaveFileName(this, "Export Ribbon CustomUI XML", "customUI14.xml", "XML Files (*.xml);;Office UI Files (*.officeUI)");
    if (!fileName.isEmpty()) {
        QFile f(fileName);
        if (f.open(QIODevice::WriteOnly | QIODevice::Text)) {
            f.write(m_xmlEditor->toPlainText().toUtf8());
            f.close();
            QMessageBox::information(this, "Export", "បានរក្សាទុកឯកសារ XML ដោយជោគជ័យ!");
        }
    }
}

void RibbonEditorWidget::onCopyXmlClicked() {
    QApplication::clipboard()->setText(m_xmlEditor->toPlainText());
    QMessageBox::information(this, "Copy", "បានចម្លងកូដ XML ទៅកាន់ Clipboard រួចរាល់!");
}
