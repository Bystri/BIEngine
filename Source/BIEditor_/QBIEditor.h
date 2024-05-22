#pragma once

#include <QMainWindow>
#include <QTreeView>
#include <QTreeWidget>
#include <QTableWidget>
#include <QKeyEvent>
#include <QFileSystemModel>

#include "QDisplayWidget.h"
#include "QActorComponentEditor.h"

class QBIEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit QBIEditor(QWidget *parent = nullptr);

    bool Init();

private:
    void InitializeMenu();

    void InitializeAssetTreeWidget();
    void InitializeActorTreeWidget();

private slots:
    void AssetsTreeDoubleClicked(const QModelIndex& index);
    void ActorTreeClicked(QTreeWidgetItem* item, int column);

    //Слоты меню
    void OpenWorld();
    void SaveWorld();

    void CreateEmptyActor();

    //Обработка нажатия клавиш
    void keyPressEvent(QKeyEvent *ev);
    void keyReleaseEvent(QKeyEvent *ev);

private:
    QString m_currentDirectory;
    QString m_parentDirectory;
    QString m_assetsDirectory;

    QMenu* m_pFileMenu;
    QMenu* m_pActorMenu;

    QDisplayWidget* m_pDisplayWidget;

    QTreeView* m_pAssetsTreeView;
    QFileSystemModel* m_pAssetDirModel;
    QActorComponentEditor* m_pActorComponentEditor;

    QTreeWidget* m_pActorsTreeWidget;
};

