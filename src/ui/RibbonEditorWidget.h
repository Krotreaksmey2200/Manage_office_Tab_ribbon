#ifndef RIBBON_EDITOR_WIDGET_H
#define RIBBON_EDITOR_WIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include <QPlainTextEdit>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include "RibbonModel.h"
#include "XmlHighlighter.h"

class RibbonEditorWidget : public QWidget {
    Q_OBJECT
public:
    explicit RibbonEditorWidget(QWidget* parent = nullptr);

    void loadXml(const QString& xmlContent, const QString& sourceFileName = "");
    QString getXml() const;

signals:
    void ribbonXmlSaved(const QString& newXml);

private slots:
    void onTreeItemSelectionChanged();
    void onAddTabClicked();
    void onAddGroupClicked();
    void onAddButtonClicked();
    void onAddSeparatorClicked();
    void onDeleteItemClicked();
    void onMoveUpClicked();
    void onMoveDownClicked();
    void onApplyPropertyChanges();
    void onSyncFromXmlClicked();
    void onInsertKhmerTemplateClicked();
    void onExportXmlClicked();
    void onCopyXmlClicked();

private:
    void setupUi();
    void populateTreeFromDefinition();
    RibbonDefinition buildDefinitionFromTree();
    void updateXmlPreview();

    // UI elements
    QTreeWidget* m_treeWidget;
    QPlainTextEdit* m_xmlEditor;
    XmlHighlighter* m_xmlHighlighter;

    // Property panel fields
    QWidget* m_propPanel;
    QLabel* m_lblItemType;
    QLineEdit* m_editId;
    QLineEdit* m_editLabel;
    QComboBox* m_comboSize;
    QLineEdit* m_editImageMso;
    QLineEdit* m_editCustomImage;
    QLineEdit* m_editOnAction;
    QLineEdit* m_editScreentip;
    QLineEdit* m_editSupertip;
    QCheckBox* m_chkVisible;
    QCheckBox* m_chkEnabled;
    QPushButton* m_btnApplyProps;

    QLabel* m_lblCurrentFile;
    QString m_currentFileName;
    RibbonDefinition m_definition;
    bool m_isUpdatingFromCode = false;
};

#endif // RIBBON_EDITOR_WIDGET_H
