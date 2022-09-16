#include "QActorComponentEditor.h"

#include <QGroupBox>
#include <QValidator>
#include <QComboBox>
#include <QMessageBox>
#include <QLabel>

#include "QBIEditor.h"

QActorComponentEditor::QActorComponentEditor(QWidget *parent, QString assetsDirectory)
    : QWidget(parent)
    , m_assetsDirectory(assetsDirectory)
    , m_pMainWidget(nullptr)
    , m_pMainLayout(nullptr)
    , m_componentsSettings()
    , m_xmlComponentDoc()
    , m_pLabelAddComponent(nullptr)
    , m_pComboBoxAddComponent(nullptr)
    , m_selectedActorId(-1)
    , m_nComponents(0)
{
    //Создаем layout
    m_pMainWidget = new QWidget(this);
    m_pMainLayout = new QBoxLayout(QBoxLayout::Up, m_pMainWidget);
    m_pMainWidget->setLayout(m_pMainLayout);

    tinyxml2::XMLError eResult = m_xmlComponentDoc.LoadFile(std::string(m_assetsDirectory.toStdString() + "/Editor/components.xml").c_str());
    if (eResult != tinyxml2::XML_SUCCESS)
        return;

    tinyxml2::XMLElement* pComponentsNode = m_xmlComponentDoc.FirstChildElement("Components");
    if (pComponentsNode)
    {
        for (tinyxml2::XMLElement* pNode = pComponentsNode->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
        {
            //Добавляем структуру для создания панели компнента
            const char *namecstr;
            pNode->QueryStringAttribute("name", &namecstr);
            std::string elementName = namecstr;
            m_componentsSettings[elementName] = pNode;
        }
    }
}

void QActorComponentEditor::ShowActorComponents(int selectedActorId)
{
    delete m_pMainLayout;
    delete m_pMainWidget;
    m_pMainWidget = new QWidget(this);
    m_pMainLayout = new QBoxLayout(QBoxLayout::Up, m_pMainWidget);
    m_pMainWidget->setLayout(m_pMainLayout);

    auto pEditorLogic = std::dynamic_pointer_cast<BIEditorLogic>(BIEngine::g_pApp->m_pGameLogic);
    auto actorMap = pEditorLogic->GetActorMap();
    tinyxml2::XMLDocument actorXmlDoc;
    actorMap[selectedActorId]->ToXML(&actorXmlDoc);

    AddComboBoxComponent();

    m_selectedActorId = selectedActorId;

    actorXmlDoc.DeepCopy(&m_selectedActorComponents);

    for (tinyxml2::XMLElement* pNode = m_selectedActorComponents.RootElement()->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
    {
        QGroupBox* pComponentGroupBox = new QGroupBox(pNode->Name(), m_pMainWidget);
        QGridLayout* pComponentLayout = new QGridLayout();
        std::string name = pNode->Name();
        AddComponentUI(pComponentLayout, pNode, m_componentsSettings[name]);
        pComponentGroupBox->setLayout(pComponentLayout);
        m_pMainLayout->addWidget(pComponentGroupBox);
    }

    m_pMainWidget->setGeometry(0, 0, geometry().width(), geometry().height());
    m_pMainWidget->show();
}

void QActorComponentEditor::AddComboBoxComponent()
{
    //Создаем все необходимое для кнопки добавления компонентов

    //Текст
    QGridLayout* pLayoutAddComponent = new QGridLayout();
    m_pLabelAddComponent = new QLabel(m_pMainWidget);
    m_pLabelAddComponent->setText(tr("Add component"));

    //Выпадающий список
    m_pComboBoxAddComponent = new QComboBox(m_pMainWidget);

    //Кнопка
    m_pPushButtonAddComponent = new QPushButton(m_pMainWidget);
    m_pPushButtonAddComponent->setText(tr("Add"));
    connect(m_pPushButtonAddComponent, SIGNAL (clicked()), this, SLOT (ButtonAddComponentClicked()));

    //Размещаем все в layout
    pLayoutAddComponent->addWidget(m_pLabelAddComponent, 1, 0);
    pLayoutAddComponent->addWidget(m_pComboBoxAddComponent, 1, 1);
    pLayoutAddComponent->addWidget(m_pPushButtonAddComponent, 1, 2);
    m_pMainLayout->addLayout(pLayoutAddComponent);

    m_pComboBoxAddComponent->clearEditText();

    for (const auto& elemName : m_componentsSettings)
    {
        //Добавляем опцию для добавления данного компонента в QComboBox
        m_pComboBoxAddComponent->addItem(elemName.first.c_str());
    }
}

void QActorComponentEditor::ButtonAddComponentClicked()
{
    QString componentName = m_pComboBoxAddComponent->currentText();

    //Проверяем, что компонент с таким именем вооще существует в природе
    if (m_componentsSettings.find(componentName.toStdString()) == m_componentsSettings.end())
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText(tr("No such component element with this name"));
        msgBox.exec();
        return;
    }

    //Проверяем, что компонента с таким именем у актера еще нет
    if (m_selectedActorComponents.RootElement()->FirstChildElement(componentName.toStdString().c_str()) != nullptr)
    {
        QMessageBox msgBox;
        msgBox.setIcon(QMessageBox::Critical);
        msgBox.setText(tr("Cannot add another component with this type.\nThe actor already has it"));
        msgBox.exec();
        return;
    }

    tinyxml2::XMLDocument changedActorParametrs;
    tinyxml2::XMLElement* pRoot = changedActorParametrs.NewElement("Actor");
    tinyxml2::XMLElement* pComponentElement = changedActorParametrs.NewElement(componentName.toStdString().c_str());
    pRoot->LinkEndChild(pComponentElement);
    BIEngine::g_pApp->m_pGameLogic->ModifyActor(m_selectedActorId, pRoot);

    ShowActorComponents(m_selectedActorId);
}

void QActorComponentEditor::AddComponentUI(QGridLayout* pComponentLayout, tinyxml2::XMLElement* actorComponentValues, tinyxml2::XMLElement* editorComponentValues)
{
    QString componentName = actorComponentValues->Name();

    m_nComponents = 0;

    for (tinyxml2::XMLElement* pNode = editorComponentValues->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
    {
        const char *namecstr, *typecstr;
        pNode->QueryStringAttribute("name", &namecstr);
        pNode->QueryStringAttribute("type", &typecstr);
        QString elementName = QString(namecstr);
        QString elementType = QString(typecstr);

        tinyxml2::XMLElement* pActorValues = actorComponentValues->FirstChildElement(elementName.toStdString().c_str());

        if (elementType == "int")
        {
            AddNumEdit(pComponentLayout, elementName, pActorValues, new QIntValidator());
        }
        else if (elementType == "float")
        {
            AddNumEdit(pComponentLayout, elementName, pActorValues, new QDoubleValidator());
        }
        else if (elementType == "Vec2")
        {
            AddVec2(pComponentLayout, elementName, pActorValues);
        }
        else if (elementType == "RGB")
        {
            AddRGB(pComponentLayout, elementName, pActorValues);
        }
        else if (elementType == "File")
        {
            AddFileElement(pComponentLayout, elementName, pActorValues);
        }
        else if (elementType == "PhysicsShape")
        {
            QStringList physicsShapes = QStringList{ "Box", "Circle" };
            AddCompobox(pComponentLayout, elementName, pActorValues, physicsShapes);
        }
        else if (elementType == "PhysicsDensity" || elementType == "PhysicsMaterial" || elementType == "boolean")
        {
            AddElementLabel(pComponentLayout, "   " + elementName + ": " + elementType);
        }
        else
        {
            AddElementLabel(pComponentLayout, "   " + elementName + ": " + elementType + " (unknown!)");
        }

        ++m_nComponents;
    }
}

void QActorComponentEditor::AddElementLabel(QGridLayout* pComponentLayout, const QString& labelText)
{
    QLabel *pLabel = new QLabel(m_pMainWidget);
    pLabel->setText(labelText);
    pComponentLayout->addWidget(pLabel, m_nComponents, 0);
}

void QActorComponentEditor::AddNumEdit(QGridLayout* pComponentLayout, const QString& elementName, tinyxml2::XMLElement* pActorValues, QValidator* pValidator)
{
    QLabel *pLabel = new QLabel(m_pMainWidget);
    pLabel->setText(elementName);
    pComponentLayout->addWidget(pLabel, m_nComponents, 0);

    QRealTimeLineEdit *pRTLineEdit = new QRealTimeLineEdit(m_pMainWidget);
    pRTLineEdit->setValidator(pValidator);
    pRTLineEdit->setText(std::to_string(pActorValues->FirstAttribute()->DoubleValue()).c_str());

    //Путь различает элементы ввода между собой и по нему мы потом понимает, какой именно параметр из всего XML-файла актера мы меняем
    pRTLineEdit->setAccessibleName(GetXmlParameterPath(pActorValues));

    pComponentLayout->addWidget(pRTLineEdit, m_nComponents, 1);

    connect(pRTLineEdit, SIGNAL(procChangedValue(const QRealTimeLineEdit*, const QString&)), this, SLOT(NumElementEdited(const QRealTimeLineEdit*, const QString&)));
}

#include <QDebug>

void QActorComponentEditor::NumElementEdited(const QRealTimeLineEdit* sender, const QString& textValue)
{
    tinyxml2::XMLDocument changedActorParametrs;
    tinyxml2::XMLElement* pRoot = changedActorParametrs.NewElement("Actor");

    QString senderXmlPathName = sender->accessibleName();
    auto xmlPathElements = senderXmlPathName.split(".");

    for (tinyxml2::XMLElement* pComponentNode = m_selectedActorComponents.RootElement()->FirstChildElement(); pComponentNode; pComponentNode = pComponentNode->NextSiblingElement())
    {
        for (tinyxml2::XMLElement* pParameterNode = pComponentNode->FirstChildElement(); pParameterNode; pParameterNode = pParameterNode->NextSiblingElement())
        {
            if (pParameterNode->Name() == xmlPathElements[0])
            {
                tinyxml2::XMLElement* pComponentElement = pComponentNode->DeepClone(&changedActorParametrs)->ToElement();

                //Меняем настройки и в облегченной копии, которая уйдет актера и в полной копии XML-описания актера
                pComponentElement->FirstChildElement(xmlPathElements[0].toStdString().c_str())->SetAttribute(xmlPathElements[1].toStdString().c_str(), textValue.toStdString().c_str());
                pComponentNode->FirstChildElement(xmlPathElements[0].toStdString().c_str())->SetAttribute(xmlPathElements[1].toStdString().c_str(), textValue.toStdString().c_str());

                pRoot->LinkEndChild(pComponentElement);
                BIEngine::g_pApp->m_pGameLogic->ModifyActor(m_selectedActorId, pRoot);
                return;
            }
        }
    }
}

void QActorComponentEditor::AddVec2(QGridLayout* pComponentLayout, const QString& elementName, tinyxml2::XMLElement* pActorValues)
{
    QLabel *pLabel = new QLabel(m_pMainWidget);
    pLabel->setText(elementName);
    pComponentLayout->addWidget(pLabel, m_nComponents, 0);

    int i = 1;
    for (auto pAttribute = pActorValues->FirstAttribute(); pAttribute; pAttribute = pAttribute->Next())
    {
        if (!pAttribute)
            break;

        QRealTimeLineEdit *pRTLineEdit = new QRealTimeLineEdit(m_pMainWidget);
        QDoubleValidator* pValidator = new QDoubleValidator(pRTLineEdit);
        pValidator->setNotation(QDoubleValidator::StandardNotation);
        pRTLineEdit->setValidator(pValidator);
        pRTLineEdit->setText(std::to_string(pAttribute->DoubleValue()).c_str());

        //Назначаем правильные имена для каждого поля, чтобы они потом корректно нашлись в XML-документе актера
        QString xmlParameterPath = pActorValues->Name();
        xmlParameterPath += ".";
        xmlParameterPath += pAttribute->Name();
        pRTLineEdit->setAccessibleName(xmlParameterPath);

        pComponentLayout->addWidget(pRTLineEdit, m_nComponents, i);

        connect(pRTLineEdit, SIGNAL(procChangedValue(const QRealTimeLineEdit*, const QString&)), this, SLOT(NumElementEdited(const QRealTimeLineEdit*, const QString&)));

        ++i;
    }
}

void QActorComponentEditor::AddRGB(QGridLayout* pComponentLayout, const QString& elementName, tinyxml2::XMLElement* pActorValues)
{
    QLabel *pLabel = new QLabel(m_pMainWidget);
    pLabel->setText(elementName);
    pComponentLayout->addWidget(pLabel, m_nComponents, 0);

    QColorPicker *pColorPicker = new QColorPicker(m_pMainWidget);
    double r, g, b;
    pActorValues->QueryDoubleAttribute("r", &r);
    pActorValues->QueryDoubleAttribute("g", &g);
    pActorValues->QueryDoubleAttribute("b", &b);
    pColorPicker->SetColor(QColor(r * 255.0, g * 255.0, b * 255.0));

    //Путь различает элементы ввода между собой и по нему мы потом понимает, какой именно параметр из всего XML-файла актера мы меняем
    pColorPicker->setAccessibleName(GetXmlParameterPath(pActorValues));

    pComponentLayout->addWidget(pColorPicker, m_nComponents, 1);

    connect(pColorPicker, SIGNAL(ColorChanged(const QColorPicker*, const QColor&)), this, SLOT(ColorElementEdited(const QColorPicker*, const QColor&)));

}

void QActorComponentEditor::ColorElementEdited(const QColorPicker* sender, const QColor& color)
{
    tinyxml2::XMLDocument changedActorParametrs;
    tinyxml2::XMLElement* pRoot = changedActorParametrs.NewElement("Actor");

    QString senderXmlPathName = sender->accessibleName();
    auto xmlPathElements = senderXmlPathName.split(".");

    for (tinyxml2::XMLElement* pComponentNode = m_selectedActorComponents.RootElement()->FirstChildElement(); pComponentNode; pComponentNode = pComponentNode->NextSiblingElement())
    {
        for (tinyxml2::XMLElement* pParameterNode = pComponentNode->FirstChildElement(); pParameterNode; pParameterNode = pParameterNode->NextSiblingElement())
        {
            if (pParameterNode->Name() == xmlPathElements[0])
            {
                tinyxml2::XMLElement* pComponentElement = pComponentNode->DeepClone(&changedActorParametrs)->ToElement();

                //Меняем настройки и в облегченной копии, которая уйдет актера и в полной копии XML-описания актера
                pComponentElement->FirstChildElement(xmlPathElements[0].toStdString().c_str())->SetAttribute("r", color.red() / 255.0);
                pComponentNode->FirstChildElement(xmlPathElements[0].toStdString().c_str())->SetAttribute("r", color.red() / 255.0);

                pComponentElement->FirstChildElement(xmlPathElements[0].toStdString().c_str())->SetAttribute("g", color.green() / 255.0);
                pComponentNode->FirstChildElement(xmlPathElements[0].toStdString().c_str())->SetAttribute("g", color.green() / 255.0);

                pComponentElement->FirstChildElement(xmlPathElements[0].toStdString().c_str())->SetAttribute("b", color.blue() / 255.0);
                pComponentNode->FirstChildElement(xmlPathElements[0].toStdString().c_str())->SetAttribute("b", color.blue() / 255.0);


                pRoot->LinkEndChild(pComponentElement);
                BIEngine::g_pApp->m_pGameLogic->ModifyActor(m_selectedActorId, pRoot);
                return;
            }
        }
    }
}

void QActorComponentEditor::AddFileElement(QGridLayout* pComponentLayout, const QString& elementName, tinyxml2::XMLElement* pActorValues)
{
    QLabel *pLabel = new QLabel(m_pMainWidget);
    pLabel->setText(elementName);
    pComponentLayout->addWidget(pLabel, m_nComponents, 0);

    QFilePicker *pFilePicker = new QFilePicker(m_pMainWidget, m_assetsDirectory, tr("PNG files (*.png)"));
    pFilePicker->SetFilePath(pActorValues->FirstAttribute()->Value());

    //Путь различает элементы ввода между собой и по нему мы потом понимает, какой именно параметр из всего XML-файла актера мы меняем
    pFilePicker->setAccessibleName(GetXmlParameterPath(pActorValues));

    pComponentLayout->addWidget(pFilePicker, m_nComponents, 1);

    connect(pFilePicker, SIGNAL(PathChanged(const QFilePicker*, const QString&)), this, SLOT(PathElementEdited(const QFilePicker*, const QString&)));

}

void QActorComponentEditor::PathElementEdited(const QFilePicker* sender, const QString& path)
{
    tinyxml2::XMLDocument changedActorParametrs;
    tinyxml2::XMLElement* pRoot = changedActorParametrs.NewElement("Actor");

    QString senderXmlPathName = sender->accessibleName();
    auto xmlPathElements = senderXmlPathName.split(".");

    for (tinyxml2::XMLElement* pComponentNode = m_selectedActorComponents.RootElement()->FirstChildElement(); pComponentNode; pComponentNode = pComponentNode->NextSiblingElement())
    {
        for (tinyxml2::XMLElement* pParameterNode = pComponentNode->FirstChildElement(); pParameterNode; pParameterNode = pParameterNode->NextSiblingElement())
        {
            if (pParameterNode->Name() == xmlPathElements[0])
            {
                tinyxml2::XMLElement* pComponentElement = pComponentNode->DeepClone(&changedActorParametrs)->ToElement();

                //Меняем настройки и в облегченной копии, которая уйдет актера и в полной копии XML-описания актера
                pComponentElement->FirstChildElement(xmlPathElements[0].toStdString().c_str())->SetAttribute(xmlPathElements[1].toStdString().c_str(), path.toStdString().c_str());
                pComponentNode->FirstChildElement(xmlPathElements[0].toStdString().c_str())->SetAttribute(xmlPathElements[1].toStdString().c_str(), path.toStdString().c_str());

                pRoot->LinkEndChild(pComponentElement);
                BIEngine::g_pApp->m_pGameLogic->ModifyActor(m_selectedActorId, pRoot);
                return;
            }
        }
    }
}

void QActorComponentEditor::AddCompobox(QGridLayout* pComponentLayout, const QString& elementName, tinyxml2::XMLElement* pActorValues, const QStringList& items)
{
    QLabel *pLabel = new QLabel(m_pMainWidget);
    pLabel->setText(elementName);
    pComponentLayout->addWidget(pLabel, m_nComponents, 0);

    QComboBox* pComboBox = new QComboBox(m_pMainWidget);
    pComboBox->addItems(items);
    pComboBox->setCurrentText(pActorValues->Value());
    pComponentLayout->addWidget(pComboBox, m_nComponents, 1);
}

QString QActorComponentEditor::GetXmlParameterPath(tinyxml2::XMLElement* pActorValXml)
{
    QString xmlParameterPath = pActorValXml->Name();
    xmlParameterPath += ".";
    xmlParameterPath += pActorValXml->FirstAttribute()->Name();

    return xmlParameterPath;
}



