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

    //Валидатор может изменить и input и pos, которые ему дают.
    //Поэтому мы копируем нашу строку, чтобы валидот не изменял оригинал
    QString textCopy = textValue;
    int pos = 0;

    QValidator::State validState = pValidator->validate(textCopy, pos);
    if (pValidator == nullptr || (validState == QValidator::Acceptable))
        emit procChangedValue(this, textValue);
}
