#include "QColorPicker.h"

#include <QColorDialog>
#include <QHBoxLayout>

QColorPicker::QColorPicker( QWidget* parent )
    : QWidget(parent)
{
    QHBoxLayout *pHLayout = new QHBoxLayout(this);
    m_pColorLabel = new QLabel(this);
    m_pPushButton = new QPushButton(this);
    m_pPushButton->setText("Choose color");
    pHLayout->addWidget(m_pColorLabel);
    pHLayout->addWidget(m_pPushButton);
    setLayout(pHLayout);
    connect( m_pPushButton, SIGNAL(clicked()), this, SLOT(ChangeColor()) );
}

void QColorPicker::UpdateColor()
{
    m_pColorLabel->setStyleSheet( "background-color: " + m_color.name() );
}

void QColorPicker::ChangeColor()
{
    QColor newColor = QColorDialog::getColor(m_color, parentWidget());
    if ( newColor != m_color )
        SetColor(newColor);
}

void QColorPicker::SetColor( const QColor& color )
{
    m_color = color;
    UpdateColor();
    emit ColorChanged(this, color);
}

const QColor& QColorPicker::GetColor() const
{
    return m_color;
}
