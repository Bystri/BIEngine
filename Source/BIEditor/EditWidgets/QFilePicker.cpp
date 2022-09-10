#include "QFilePicker.h"

#include <QFileDialog>
#include <QHBoxLayout>

QFilePicker::QFilePicker(QWidget* parent, const QString& searchFileDirecotry , const QString& searchFilePattern)
    : QWidget(parent)
    , m_searchFileDirecotry(searchFileDirecotry)
    , m_searchFilePattern(searchFilePattern)
{
    QHBoxLayout *pHLayout = new QHBoxLayout(this);
    m_pPathLabel = new QLabel(this);
    m_pPushButton = new QPushButton(this);
    m_pPushButton->setText("Choose file");
    pHLayout->addWidget(m_pPathLabel);
    pHLayout->addWidget(m_pPushButton);
    setLayout(pHLayout);
    connect( m_pPushButton, SIGNAL(clicked()), this, SLOT(ChangePath()) );
}

void QFilePicker::UpdatePath()
{
    m_pPathLabel->setText(m_filePath);
}

void QFilePicker::ChangePath()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Choose file"), m_searchFileDirecotry, m_searchFilePattern);

    QDir dir(m_searchFileDirecotry);
    m_filePath = dir.relativeFilePath(fileName);

    SetFilePath(m_filePath);
}

void QFilePicker::SetFilePath( const QString& filePath )
{
    m_filePath = filePath;
    UpdatePath();
    emit PathChanged(this, m_filePath);
}

const QString& QFilePicker::GetFilePath() const
{
    return m_filePath;
}
