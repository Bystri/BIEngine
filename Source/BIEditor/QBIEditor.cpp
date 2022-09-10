#include "QBIEditor.h"

#include <QDesktopServices>
#include <QAction>
#include <QMenuBar>
#include <QFileDialog>
#include <QBoxLayout>
#include <QUrl>

QBIEditor::QBIEditor(QWidget *parent)
    : QMainWindow(parent)
    , m_currentDirectory(QDir::currentPath())
    , m_parentDirectory()
    , m_assetsDirectory("Assets")
    , m_pFileMenu(nullptr)
    , m_pDisplayWidget(nullptr)
    , m_pAssetsTreeView(nullptr)
    , m_pAssetDirModel(nullptr)
    , m_pActorComponentEditor(nullptr)
    , m_pActorsTreeWidget(nullptr)
{

    QDir parentDir = QDir::current();
    parentDir.cdUp();
    m_parentDirectory = parentDir.path();

    setFixedSize(1680, 600);

    m_pActorsTreeWidget = new QTreeWidget(this);
    m_pActorsTreeWidget->setGeometry(10, 30, 400, 270);

    m_pAssetsTreeView = new QTreeView(this);
    m_pAssetsTreeView->setGeometry(10, 300, 400, 270);

    m_pDisplayWidget = new QDisplayWidget(this);
    m_pDisplayWidget->setGeometry(420, 30, 720, 540);

    m_pActorComponentEditor = new QActorComponentEditor(this, m_parentDirectory + "/" + m_assetsDirectory);
    m_pActorComponentEditor->setGeometry(1130, 30, 540, 540);
}

bool QBIEditor::Init()
{
    InitializeMenu();
    InitializeAssetTreeWidget();
    InitializeActorTreeWidget();

    return true;
}

void QBIEditor::InitializeMenu()
{
    m_pFileMenu = menuBar()->addMenu(tr("&File"));

    QAction* pOpenWorldAct = new QAction(tr("&Open World"), this);
    pOpenWorldAct->setStatusTip(tr("Open world from xml-file"));
    connect(pOpenWorldAct, &QAction::triggered, this, &QBIEditor::OpenWorld);
    m_pFileMenu->addAction(pOpenWorldAct);

    QAction* pSaveWorldAct = new QAction(tr("&Save World"), this);
    pSaveWorldAct->setStatusTip(tr("Save world to xml-file"));
    connect(pSaveWorldAct, &QAction::triggered, this, &QBIEditor::SaveWorld);
    m_pFileMenu->addAction(pSaveWorldAct);
}

void QBIEditor::InitializeAssetTreeWidget()
{
    m_pAssetDirModel = new QFileSystemModel(this);
    m_pAssetDirModel->setRootPath(m_parentDirectory + "/" + m_assetsDirectory);

    m_pAssetsTreeView->setModel(m_pAssetDirModel);
    m_pAssetsTreeView->setRootIndex(m_pAssetDirModel->setRootPath(m_parentDirectory + "/" + m_assetsDirectory));
    m_pAssetsTreeView->setColumnWidth(0, 200);

    connect(m_pAssetsTreeView, SIGNAL(doubleClicked(const QModelIndex&)), this, SLOT(AssetsTreeDoubleClicked(const QModelIndex&)));
}

void QBIEditor::AssetsTreeDoubleClicked(const QModelIndex &index)
{
    if (m_pAssetDirModel->size(index) != 0 && !m_pAssetDirModel->isDir(index))
    {
        QDesktopServices::openUrl(QUrl(m_pAssetDirModel->filePath(index)));
    }
}

void QBIEditor::InitializeActorTreeWidget()
{
    m_pActorsTreeWidget->clear();

    QList<QTreeWidgetItem *> items;
    auto actors = std::dynamic_pointer_cast<BIEditorLogic>(BIEngine::g_pApp->m_pGameLogic)->GetActorMap();
    for (const auto& actor : actors)
        items.append(new QTreeWidgetItem(static_cast<QTreeWidget *>(nullptr), QStringList{QString(std::to_string(actor.second->GetId()).c_str()), QString(actor.second->GetName().c_str())}));
    m_pActorsTreeWidget->setHeaderLabels(QStringList{QString("ID"), QString("Name")});
    m_pActorsTreeWidget->insertTopLevelItems(0, items);

    connect(m_pActorsTreeWidget, SIGNAL(itemClicked(QTreeWidgetItem*, int)), this, SLOT(ActorTreeClicked(QTreeWidgetItem*, int)), Qt::UniqueConnection);
}

void QBIEditor::ActorTreeClicked(QTreeWidgetItem* item, int column)
{
    if (m_pActorComponentEditor)
    {
        delete m_pActorComponentEditor;
    }

    m_pActorComponentEditor = new QActorComponentEditor(this, m_parentDirectory + "/" + m_assetsDirectory);
    m_pActorComponentEditor->setGeometry(1130, 30, 540, 540);

    int actorId = std::atoi(item->text(0).toStdString().c_str());
    auto pEditorLogic = std::dynamic_pointer_cast<BIEditorLogic>(BIEngine::g_pApp->m_pGameLogic);
    auto actorMap = pEditorLogic->GetActorMap();
    tinyxml2::XMLDocument actorXmlDoc;
    actorMap[actorId]->ToXML(&actorXmlDoc);
    m_pActorComponentEditor->ShowActorComponents(actorId, actorXmlDoc);
    m_pActorComponentEditor->show();
}

void QBIEditor::OpenWorld()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open world xml-file"), m_parentDirectory + "/" + m_assetsDirectory, tr("XML files (*.xml)"));

    QDir dir(m_parentDirectory + "/" + m_assetsDirectory);
    fileName = dir.relativeFilePath(fileName);
    BIEngine::g_pApp->m_pGameLogic->LoadLevel(fileName.toStdString());

    m_pDisplayWidget->paintGL();
    InitializeActorTreeWidget();
}

void QBIEditor::SaveWorld()
{
    QString fileName = QFileDialog::getSaveFileName(this,
        tr("Save world xml-file"), m_parentDirectory + "/" + m_assetsDirectory, tr("XML files (*.xml)"));

    tinyxml2::XMLDocument worldDoc;
    tinyxml2::XMLElement* pWorldRootElement = worldDoc.NewElement("World");
    tinyxml2::XMLElement* pActorsRoot = worldDoc.NewElement("Actors");
    auto pEditorLogic = std::dynamic_pointer_cast<BIEditorLogic>(BIEngine::g_pApp->m_pGameLogic);
    for (const auto& actor : pEditorLogic->GetActorMap())
    {
        pActorsRoot->LinkEndChild(actor.second->ToXML(&worldDoc));
    }
    pWorldRootElement->LinkEndChild(pActorsRoot);
    worldDoc.LinkEndChild(pWorldRootElement);

    worldDoc.SaveFile(fileName.toStdString().c_str());
}

void QBIEditor::keyPressEvent(QKeyEvent *ev)
{
    BIEngine::g_pApp->InputProc(ev->key(), 0, true);
}

void QBIEditor::keyReleaseEvent(QKeyEvent *ev)
{
    BIEngine::g_pApp->InputProc(ev->key(), 0, false);
}
