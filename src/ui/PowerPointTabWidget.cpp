#include "PowerPointTabWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>

PowerPointTabWidget::PowerPointTabWidget(ExtensionManager* extManager, RibbonEditorWidget* ribbonEditor, QWidget* parent)
    : QWidget(parent), m_extManager(extManager), m_ribbonEditor(ribbonEditor) {
    setupUi();
    refreshList();
}

void PowerPointTabWidget::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // Toolbar
    auto* toolbar = new QHBoxLayout();
    m_btnInstall = new QPushButton("➕ ដំឡើង PowerPoint Add-in (.ppam)", this);
    m_btnInstall->setObjectName("btnSuccess");
    connect(m_btnInstall, &QPushButton::clicked, this, &PowerPointTabWidget::onInstallClicked);

    m_btnToggle = new QPushButton("⚡ បើក/បិទ (Toggle)", this);
    m_btnToggle->setEnabled(false);
    connect(m_btnToggle, &QPushButton::clicked, this, &PowerPointTabWidget::onToggleStateClicked);

    m_btnEditRibbon = new QPushButton("🎨 កែសម្រួល Ribbon XML", this);
    m_btnEditRibbon->setEnabled(false);
    connect(m_btnEditRibbon, &QPushButton::clicked, this, &PowerPointTabWidget::onEditRibbonClicked);

    m_btnBackup = new QPushButton("📦 Backup Add-in", this);
    m_btnBackup->setObjectName("btnSecondary");
    m_btnBackup->setEnabled(false);
    connect(m_btnBackup, &QPushButton::clicked, this, &PowerPointTabWidget::onBackupClicked);

    m_btnUninstall = new QPushButton("🗑️ លុប (Delete)", this);
    m_btnUninstall->setObjectName("btnDanger");
    m_btnUninstall->setEnabled(false);
    connect(m_btnUninstall, &QPushButton::clicked, this, &PowerPointTabWidget::onUninstallClicked);

    auto* btnRefresh = new QPushButton("🔄 Refresh", this);
    btnRefresh->setObjectName("btnSecondary");
    connect(btnRefresh, &QPushButton::clicked, this, &PowerPointTabWidget::refreshList);

    m_btnOpenPPT = new QPushButton("🚀 បើក PowerPoint", this);
    m_btnOpenPPT->setObjectName("btnSecondary");
    connect(m_btnOpenPPT, &QPushButton::clicked, this, &PowerPointTabWidget::onOpenPowerPointClicked);

    toolbar->addWidget(m_btnInstall);
    toolbar->addWidget(m_btnToggle);
    toolbar->addWidget(m_btnEditRibbon);
    toolbar->addWidget(m_btnBackup);
    toolbar->addWidget(m_btnUninstall);
    toolbar->addWidget(btnRefresh);
    toolbar->addStretch();
    toolbar->addWidget(m_btnOpenPPT);

    mainLayout->addLayout(toolbar);

    // Table
    m_table = new QTableWidget(this);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({
        "ឈ្មោះ Add-in (File Name)",
        "ស្ថានភាព",
        "ប្រភេទ",
        "ទំហំ",
        "Ribbon CustomUI",
        "កាលបរិច្ឆេទកែប្រែ"
    });
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->resizeSection(0, 240);
    m_table->horizontalHeader()->resizeSection(1, 110);
    m_table->horizontalHeader()->resizeSection(2, 170);
    m_table->horizontalHeader()->resizeSection(3, 90);
    m_table->horizontalHeader()->resizeSection(4, 130);

    connect(m_table, &QTableWidget::itemSelectionChanged, this, &PowerPointTabWidget::onSelectionChanged);
    mainLayout->addWidget(m_table);

    // Status label at bottom
    m_lblStatus = new QLabel(this);
    m_lblStatus->setStyleSheet("color: #8a8d9b; font-size: 12px;");
    mainLayout->addWidget(m_lblStatus);
}

void PowerPointTabWidget::refreshList() {
    if (!m_extManager) return;

    m_items = m_extManager->scanPowerPointExtensions();
    m_table->setRowCount(m_items.size());

    for (int i = 0; i < m_items.size(); ++i) {
        const auto& item = m_items[i];

        auto* nameItem = new QTableWidgetItem("📊 " + item.fileName);
        m_table->setItem(i, 0, nameItem);

        auto* statusItem = new QTableWidgetItem(item.enabled ? "● កំពុងបើក (Active)" : "○ បានបិទ (Disabled)");
        statusItem->setTextAlignment(Qt::AlignCenter);
        if (item.enabled) {
            statusItem->setForeground(QColor("#107c41"));
        } else {
            statusItem->setForeground(QColor("#888888"));
        }
        m_table->setItem(i, 1, statusItem);

        m_table->setItem(i, 2, new QTableWidgetItem(item.typeDescription));

        double kb = item.fileSizeBytes / 1024.0;
        m_table->setItem(i, 3, new QTableWidgetItem(QString::number(kb, 'f', 1) + " KB"));

        auto* ribbonItem = new QTableWidgetItem(item.hasCustomRibbonXml ? "✓ មាន Custom Ribbon" : "- គ្មាន");
        ribbonItem->setTextAlignment(Qt::AlignCenter);
        if (item.hasCustomRibbonXml) {
            ribbonItem->setForeground(QColor("#d83b01"));
        }
        m_table->setItem(i, 4, ribbonItem);

        m_table->setItem(i, 5, new QTableWidgetItem(item.lastModified.toString("yyyy-MM-dd hh:mm:ss")));
    }

    m_lblStatus->setText(QString("បានរកឃើញ PowerPoint Add-in ចំនួន %1").arg(m_items.size()));
    onSelectionChanged();
}

void PowerPointTabWidget::onSelectionChanged() {
    int row = m_table->currentRow();
    bool hasSel = (row >= 0 && row < m_items.size());

    m_btnToggle->setEnabled(hasSel);
    m_btnBackup->setEnabled(hasSel);
    m_btnUninstall->setEnabled(hasSel);

    if (hasSel) {
        const auto& item = m_items[row];
        m_btnToggle->setText(item.enabled ? "⚡ បិទ (Disable)" : "⚡ បើក (Enable)");
        m_btnEditRibbon->setEnabled(item.hasCustomRibbonXml);
    } else {
        m_btnEditRibbon->setEnabled(false);
    }
}

void PowerPointTabWidget::onToggleStateClicked() {
    int row = m_table->currentRow();
    if (row < 0 || row >= m_items.size()) return;

    QString error;
    if (!m_extManager->toggleExtensionState(m_items[row], error)) {
        QMessageBox::critical(this, "កំហុស", "មិនអាចប្តូរស្ថានភាព Add-in:\n" + error);
    }
    refreshList();
}

void PowerPointTabWidget::onEditRibbonClicked() {
    int row = m_table->currentRow();
    if (row < 0 || row >= m_items.size()) return;

    const auto& item = m_items[row];
    QString xml = m_extManager->getRibbonXml(item);
    if (xml.isEmpty()) {
        QMessageBox::warning(this, "គ្មាន Ribbon XML", "មិនអាចទាញយក Ribbon XML ពី Add-in នេះបានទេ!");
        return;
    }

    if (m_ribbonEditor) {
        m_ribbonEditor->loadXml(xml, item.fileName);
        QMessageBox::information(this, "បើក Ribbon Editor", 
            "បានផ្ទុក Ribbon XML របស់ \"" + item.fileName + "\" ចូលទៅក្នុងផ្ទាំង Ribbon XML Studio រួចរាល់!\nសូមចូលទៅផ្ទាំង Ribbon Studio ដើម្បីកែប្រែ។");
    }
}

void PowerPointTabWidget::onInstallClicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "ជ្រើសរើស PowerPoint Add-in", "", "PowerPoint Add-ins (*.ppam *.ppa *.potm);;All Files (*)");
    if (filePath.isEmpty()) return;

    QString error;
    if (!m_extManager->installExtension(filePath, "PowerPoint", error)) {
        QMessageBox::critical(this, "កំហុសដំឡើង", error);
    } else {
        QMessageBox::information(this, "ជោគជ័យ", "បានដំឡើង Add-in ចូល PowerPoint រួចរាល់!");
        refreshList();
    }
}

void PowerPointTabWidget::onUninstallClicked() {
    int row = m_table->currentRow();
    if (row < 0 || row >= m_items.size()) return;

    const auto& item = m_items[row];
    auto reply = QMessageBox::question(this, "បញ្ជាក់ការលុប", 
        "តើអ្នកប្រាកដជាចង់លុប Add-in \"" + item.fileName + "\" នេះមែនទេ?\n(កម្មវិធីនឹងធ្វើ Backup ដោយស្វ័យប្រវត្តិទុកជាមុន)",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QString error;
        if (!m_extManager->uninstallExtension(item, error)) {
            QMessageBox::critical(this, "កំហុស", error);
        } else {
            QMessageBox::information(this, "លុបបានជោគជ័យ", "បានលុប Add-in រួចរាល់!");
            refreshList();
        }
    }
}

void PowerPointTabWidget::onBackupClicked() {
    int row = m_table->currentRow();
    if (row < 0 || row >= m_items.size()) return;

    QString error;
    QString backupPath = m_extManager->backupExtension(m_items[row], error);
    if (backupPath.isEmpty()) {
        QMessageBox::critical(this, "កំហុស Backup", error);
    } else {
        QMessageBox::information(this, "ជោគជ័យ Backup", "បាន Backup រក្សាទុកនៅ:\n" + backupPath);
    }
}

void PowerPointTabWidget::onOpenPowerPointClicked() {
#ifdef Q_OS_MACOS
    QProcess::startDetached("/usr/bin/open", {"-a", "Microsoft PowerPoint"});
#elif defined(Q_OS_WIN)
    QProcess::startDetached("powerpnt.exe", {});
#endif
}
