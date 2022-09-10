#include "QDisplayWidget.h"

#include <iostream>

#include <QTimer>

QDisplayWidget::QDisplayWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{

}

void QDisplayWidget::initializeGL()
{
    gladLoadGL();
    glViewport(0, 0, BIEngine::g_pApp->m_options.screenWidth, BIEngine::g_pApp->m_options.screenHeight);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //������������� ����
    if (!BIEngine::g_pApp->Init())
    {
        //�� �� ����� ������ � ���, ��� ��� ��������������, ��� ������������� ������� ����� ��������� ��������� �� ������
        return;
    }

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(OnRender()));
    timer->start(200);
}

void QDisplayWidget::resizeGL(int w, int h)
{

}

void QDisplayWidget::paintGL()
{
    //BIEngine::g_pGameApp->ProcessInput(deltaTime);
    //���������� ������
    //g_pGameApp->OnUpdate(deltaTime);

    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    //���������
    BIEngine::g_pApp->OnRender(1, 1);
}

void QDisplayWidget::OnRender()
{
    paintGL();
    update();
}
