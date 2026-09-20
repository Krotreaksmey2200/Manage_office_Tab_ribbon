#ifndef XML_HIGHLIGHTER_H
#define XML_HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>
#include <QVector>

class XmlHighlighter : public QSyntaxHighlighter {
    Q_OBJECT
public:
    explicit XmlHighlighter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString& text) override;

private:
    struct HighlightingRule {
        QRegularExpression pattern;
        QTextCharFormat format;
    };
    QVector<HighlightingRule> m_rules;

    QTextCharFormat m_tagFormat;
    QTextCharFormat m_attributeFormat;
    QTextCharFormat m_valueFormat;
    QTextCharFormat m_commentFormat;
};

#endif // XML_HIGHLIGHTER_H
