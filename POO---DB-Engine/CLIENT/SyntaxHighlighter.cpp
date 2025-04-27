#include "SyntaxHighlighter.h"

QStringList SyntaxHighlighter::keywords = {
            "add", "alter", "and", "as", "asc",
            "backup", "between", "by", "case", "check",
            "column", "constraint", "create", "database", "default",
            "delete", "desc", "distinct", "drop", "exec",
            "exists", "foreign", "from", "full",
            "group", "having", "in", "index", "inner",
            "insert", "into", "is", "join", "left",
            "like", "limit", "not", "null", "on",
            "or", "order", "outer", "primary", "procedure",
            "right", "rownum", "select", "set", "table",
            "top", "trigger", "union", "unique", "update",
            "use", "values", "view", "where", "with"
};

SyntaxHighlighter::SyntaxHighlighter(QTextDocument * parent) : QSyntaxHighlighter(parent)
{
    this->keywordFormat.setForeground(Qt::blue);
    this->keywordFormat.setFontWeight(QFont::Bold);
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    for (const QString& keyword : SyntaxHighlighter::keywords) {

        QRegularExpression expression("\\b" + QRegularExpression::escape(keyword) + "\\b", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatchIterator iterator = expression.globalMatch(text);

        while (iterator.hasNext()) {
            QRegularExpressionMatch match = iterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), keywordFormat);
        }
    }
}