#pragma once

#include <QLineEdit>

class QRealTimeLineEdit : public QLineEdit
{
    Q_OBJECT

public:
    QRealTimeLineEdit(QWidget* parent);

private slots:
    void numEdited(const QString& textValue);

signals:
    void procChangedValue(const QRealTimeLineEdit*, const QString&);
};
