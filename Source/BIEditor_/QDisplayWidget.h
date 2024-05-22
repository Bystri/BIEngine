#pragma once

#include "BIEditor.h"
#include <QOpenGLWidget>

class QDisplayWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit QDisplayWidget(QWidget* parent = nullptr);

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private slots:
    void OnRender();
};
