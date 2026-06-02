#include <engine/camera.hpp>

Camera::Camera(glm::vec3 position)
{
    this->position = position;

    front = glm::vec3(0.0f, 0.0f, -1.0f);

    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    yaw = -90.0f;
    pitch = 0.0f;

    speed = 4.5f;
    sensitivity = 0.03f;

    calculateVectors();
}

void Camera::goTo(glm::vec3 position)
{
    this->position = position;
}

void Camera::move(int direction, float deltaTime)
{
    float velocity = speed * deltaTime;

    glm::vec3 flatFront = glm::normalize(glm::vec3(front.x, 0.0f, front.z));

    if (direction == 0)
        position += flatFront * velocity;
    if (direction == 1)
        position -= flatFront * velocity;
    if (direction == 2)
        position -= right * velocity;
    if (direction == 3)
        position += right * velocity;
    if (direction == 4)
        position -= worldUp * velocity;
    if (direction == 5)
        position += worldUp * velocity;
}

void Camera::rotate(float deltaX, float deltaY)
{
    yaw += deltaX * sensitivity;
    pitch += deltaY * sensitivity;

    pitch = glm::clamp(pitch, -90.0f, 90.0f);

    calculateVectors();
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(position, position + front, up);
}

void Camera::calculateVectors()
{
    float radYaw = glm::radians(yaw);
    float radPitch = glm::radians(pitch);

    front = glm::normalize(glm::vec3(
        glm::cos(radYaw) * glm::cos(radPitch),
        glm::sin(radPitch),
        glm::sin(radYaw) * glm::cos(radPitch)));

    right = glm::normalize(glm::cross(front, worldUp));

    up = glm::normalize(glm::cross(right, front));
}