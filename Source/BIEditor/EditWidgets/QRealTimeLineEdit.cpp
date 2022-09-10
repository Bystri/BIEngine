#include "QRealTimeLineEdit.h"

#include <QValidator>

QRealTimeLineEdit::QRealTimeLineEdit(QWidget* parent)
    : QLineEdit(parent)
{
    connect(this, SIGNAL(textChanged(const QString&)), this, SLOT(numEdited(const QString&)));
}

void QRealTimeLineEdit::numEdited(const QString& textValue)
{
    const QValidator* pValidator = validator();
    QString textCopy = textValue;
    int pos = 0;
    if (pValidator == nullptr || (pValidator->validate(textCopy, pos) == QValidator::Acceptable))
        emit procChangedValue(this, textValue);
}
