#include "BIEditorController.h"

#include <QKeyEvent>

#include "../BIEngine/EventManager/EventManager.h"

bool BIEditorController::OnKeyDown(int key, int scancode)
{
    const float CAMERA_SPEED = 1.0f;

    //Обновляем таблицу клавиш
    m_isKeyPressed[key] = true;

    if (key == Qt::Key_A || key == Qt::Key_D)
    {
        std::shared_ptr<BIEngine::TransformComponent> pTransformComponent = m_cameraActor->GetComponent<BIEngine::TransformComponent>(BIEngine::TransformComponent::g_CompId).lock();
        if (pTransformComponent)
        {
            glm::vec2 curPos = pTransformComponent->GetPosition();
            curPos.x += CAMERA_SPEED;
            pTransformComponent->SetPosition(curPos);
        }
    }

    if (key == Qt::Key_S || key == Qt::Key_W)
    {
        std::shared_ptr<BIEngine::TransformComponent> pTransformComponent = m_cameraActor->GetComponent<BIEngine::TransformComponent>(BIEngine::TransformComponent::g_CompId).lock();
        if (pTransformComponent)
        {
            glm::vec2 curPos = pTransformComponent->GetPosition();
            curPos.y += CAMERA_SPEED;
            pTransformComponent->SetPosition(curPos);
        }
    }

    return true;
}

bool BIEditorController::OnKeyUp(int key, int scancode)
{
    //Обновляем таблицу клавиш
    m_isKeyPressed[key] = false;

    const float CAMERA_SPEED = 50.0f;

    if (key == Qt::Key_A)
    {
        std::shared_ptr<BIEngine::TransformComponent> pTransformComponent = m_cameraActor->GetComponent<BIEngine::TransformComponent>(BIEngine::TransformComponent::g_CompId).lock();
        if (pTransformComponent)
        {
            glm::vec2 curPos = pTransformComponent->GetPosition();
            curPos.x += CAMERA_SPEED;
            pTransformComponent->SetPosition(curPos);
        }
    }

    if (key == Qt::Key_D)
    {
        std::shared_ptr<BIEngine::TransformComponent> pTransformComponent = m_cameraActor->GetComponent<BIEngine::TransformComponent>(BIEngine::TransformComponent::g_CompId).lock();
        if (pTransformComponent)
        {
            glm::vec2 curPos = pTransformComponent->GetPosition();
            curPos.x -= CAMERA_SPEED;
            pTransformComponent->SetPosition(curPos);
        }
    }

    if (key == Qt::Key_S)
    {
        std::shared_ptr<BIEngine::TransformComponent> pTransformComponent = m_cameraActor->GetComponent<BIEngine::TransformComponent>(BIEngine::TransformComponent::g_CompId).lock();
        if (pTransformComponent)
        {
            glm::vec2 curPos = pTransformComponent->GetPosition();
            curPos.y += CAMERA_SPEED;
            pTransformComponent->SetPosition(curPos);
        }
    }

    if (key == Qt::Key_W)
    {
        std::shared_ptr<BIEngine::TransformComponent> pTransformComponent = m_cameraActor->GetComponent<BIEngine::TransformComponent>(BIEngine::TransformComponent::g_CompId).lock();
        if (pTransformComponent)
        {
            glm::vec2 curPos = pTransformComponent->GetPosition();
            curPos.y -= CAMERA_SPEED;
            pTransformComponent->SetPosition(curPos);
        }
    }


    return true;
}
