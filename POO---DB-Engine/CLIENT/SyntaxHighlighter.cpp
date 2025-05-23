#include "SyntaxHighlighter.h"

QStringList SyntaxHighlighter::keywords = {
            "add", "alter", "and", "as", "asc","avg"
            "backup", "between", "by", "call", "case", "check",
            "column", "constraint", "create", "database", "default",
            "delete", "desc", "distinct", "drop", "exec",
            "exists", "foreign", "from", "full",
            "group", "having", "in", "index", "inner",
            "insert", "instruction","into", "is", "join", "key", "left",
            "like", "limit", "max","min","not", "null","number", "on",
            "or", "order", "outer", "primary", "procedure","references",
            "right", "rownum", "select", "set","string" "sum", "table",
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

    QRegularExpression commentPattern("^\\s*//.*");
    QRegularExpressionMatch match = commentPattern.match(text);
    if (match.hasMatch()) {
        QTextCharFormat commentFormat;
        commentFormat.setForeground(Qt::darkGreen);
        commentFormat.setFontWeight(QFont::Bold);
        setFormat(match.capturedStart(), match.capturedLength(), commentFormat);
    }
}