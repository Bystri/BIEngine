#pragma once

#include <QPushButton>
#include <QLabel>
#include <QColor>

class QColorPicker : public QWidget
{
    Q_OBJECT
public:
    QColorPicker(QWidget* parent);

    void SetColor(const QColor& color);
    const QColor& GetColor() const;

public slots:
    void UpdateColor();
    void ChangeColor();

signals:
    void ColorChanged(const QColorPicker* sender, const QColor& color);

private:
    QColor m_color;
    QLabel* m_pColorLabel;
    QPushButton* m_pPushButton;
};
