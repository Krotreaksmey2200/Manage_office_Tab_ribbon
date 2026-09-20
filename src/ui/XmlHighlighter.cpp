#include "XmlHighlighter.h"
#include <QColor>

XmlHighlighter::XmlHighlighter(QTextDocument* parent)
    : QSyntaxHighlighter(parent) {
    // 1. Tags: <tag> </tag>
    m_tagFormat.setForeground(QColor("#007acc"));
    m_tagFormat.setFontWeight(QFont::Bold);

    // 2. Attributes: id= label= onAction=
    m_attributeFormat.setForeground(QColor("#d9822b"));

    // 3. Values: "myId" "OpenEditor"
    m_valueFormat.setForeground(QColor("#388a34"));

    // 4. Comments: <!-- comment -->
    m_commentFormat.setForeground(QColor("#808080"));
    m_commentFormat.setFontItalic(true);

    HighlightingRule rule;

    // Attributes
    rule.pattern = QRegularExpression("\\b[a-zA-Z_:][a-zA-Z0-9_.-]*(?=\\=)");
    rule.format = m_attributeFormat;
    m_rules.append(rule);

    // Attribute Values in quotes
    rule.pattern = QRegularExpression("\".*?\"");
    rule.format = m_valueFormat;
    m_rules.append(rule);

    // Tags
    rule.pattern = QRegularExpression("</?[a-zA-Z_:][a-zA-Z0-9_.:-]*");
    rule.format = m_tagFormat;
    m_rules.append(rule);

    rule.pattern = QRegularExpression("/?>");
    rule.format = m_tagFormat;
    m_rules.append(rule);

    // Comments
    rule.pattern = QRegularExpression("<!--.*?-->");
    rule.format = m_commentFormat;
    m_rules.append(rule);
}

void XmlHighlighter::highlightBlock(const QString& text) {
    for (const HighlightingRule& rule : m_rules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
}
