#pragma once

#include <map>
#include <string>
#include <QWidget>
#include <QLayout>
#include <QComboBox>

#include <tinyxml2.h>

#include "EditWidgets/QRealTimeLineEdit.h"
#include "EditWidgets/QFilePicker.h"
#include "EditWidgets/QColorPicker.h"

class QActorComponentEditor : public QWidget
{
    Q_OBJECT

public:
    QActorComponentEditor(QWidget *parent, QString assetsDirectory);

    void ShowActorComponents(int selectedActorId);

private:
    void AddComboBoxComponent();

    void AddComponentUI(QGridLayout* pComponentLayout, tinyxml2::XMLElement* actorComponentValues, tinyxml2::XMLElement* editorComponentValues);

    void AddElementLabel(QGridLayout* pComponentLayout, const QString& labelText);
    void AddNumEdit(QGridLayout* pComponentLayout, const QString& elementName, tinyxml2::XMLElement* pActorValues, QValidator* pValidator);
    void AddVec2(QGridLayout* pComponentLayout, const QString& elementName, tinyxml2::XMLElement* pActorValues);
    void AddRGB(QGridLayout* pComponentLayout, const QString& elementName, tinyxml2::XMLElement* pActorValues);
    void AddFileElement(QGridLayout* pComponentLayout, const QString& elementName, tinyxml2::XMLElement* pActorValues);
    void AddCompobox(QGridLayout* pComponentLayout, const QString& elementName, tinyxml2::XMLElement* pActorValues, const QStringList& items);

    //На основе XML элемента создает путь, по которому будет воссоздан этот самый XML-элемент
    QString GetXmlParameterPath(tinyxml2::XMLElement* pActorValXml);

private slots:
    void ButtonAddComponentClicked();

    void NumElementEdited(const QRealTimeLineEdit* sender, const QString& textValue);
    void ColorElementEdited(const QColorPicker* sender, const QColor& color);
    void PathElementEdited(const QFilePicker* sender, const QString& path);

private:
    QString m_assetsDirectory;

    QWidget* m_pMainWidget;
    QBoxLayout* m_pMainLayout;

    std::map<std::string, tinyxml2::XMLElement*> m_componentsSettings;
    tinyxml2::XMLDocument m_xmlComponentDoc;

    QLabel* m_pLabelAddComponent;
    QComboBox* m_pComboBoxAddComponent; //Выпадающий список компонентов для добавления
    QPushButton* m_pPushButtonAddComponent;

    int m_selectedActorId;
    tinyxml2::XMLDocument m_selectedActorComponents;
    int m_nComponents;
};
