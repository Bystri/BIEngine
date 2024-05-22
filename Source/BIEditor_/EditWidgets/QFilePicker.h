#pragma once

#include <QPushButton>
#include <QLabel>

class QFilePicker : public QWidget
{
    Q_OBJECT
public:
    QFilePicker(QWidget* parent, const QString& searchFileDirecotry , const QString& searchFilePattern);

    void SetFilePath(const QString& filePath);
    const QString& GetFilePath() const;

public slots:
    void UpdatePath();
    void ChangePath();

signals:
    void PathChanged(const QFilePicker* sender, const QString& path);

private:
    QString m_searchFileDirecotry;
    QString m_searchFilePattern;

    QString m_filePath;
    QLabel* m_pPathLabel;
    QPushButton* m_pPushButton;
};
