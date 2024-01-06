#ifndef BIPROJECTWIZARD_H
#define BIPROJECTWIZARD_H

#include <QMainWindow>

class BIProjectWizard : public QMainWindow
{
    Q_OBJECT

public:
    BIProjectWizard(QWidget *parent = nullptr);
    ~BIProjectWizard();

private:
    void CreateProject(const QString& projPath);
    void OpenProject(const QString& projPath);

private:
    //Относительный путь до корневой папки
    const QString m_srcPath;

    //Путь, относильно основной папки, к ассетам
    const QString m_assetsPath;
    //Путь, относильно основной папки, к исходникам движка
    const QString m_engineSourcePath;
    //Путь, относительно основной папки, к исходникам игры
    const QString m_gameSourcePath;
    //Путь, относительно основной папки, к библиотекам
    const QString m_libPath;
};
#endif // BIPROJECTWIZARD_H
