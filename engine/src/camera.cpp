#include <engine/camera.hpp>

Camera::Camera(glm::vec3 position)
{
    this->position = position;

    front = glm::vec3(0.0f, 0.0f, -1.0f);

    worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    yaw = -90.0f;
    pitch = 0.0f;

    speed = 18.0f;
    sensitivity = 4.0f;

    calculateVectors();
}

void Camera::goTo(glm::vec3 position) { this->position = position; }

void Camera::move(Direction direction, float deltaTime)
{
    float velocity = speed * deltaTime;

    glm::vec3 flatFront = glm::normalize(glm::vec3(front.x, 0.0f, front.z));

    if (direction == Direction::Forward) position += flatFront * velocity;
    if (direction == Direction::Backward) position -= flatFront * velocity;
    if (direction == Direction::Left) position -= right * velocity;
    if (direction == Direction::Right) position += right * velocity;
    if (direction == Direction::Down) position -= worldUp * velocity;
    if (direction == Direction::Up) position += worldUp * velocity;
}

void Camera::rotate(float deltaX, float deltaY, float deltaTime)
{
    yaw += deltaX * sensitivity * deltaTime;
    pitch += deltaY * sensitivity * deltaTime;

    pitch = glm::clamp(pitch, -89.999f, 89.999f);

    calculateVectors();
}

glm::mat4 Camera::getViewMatrix() const { return glm::lookAt(position, position + front, up); }

glm::vec3 Camera::getPosition() const { return position; }

void Camera::calculateVectors()
{
    float radYaw = glm::radians(yaw);
    float radPitch = glm::radians(pitch);

    front = glm::normalize(glm::vec3(glm::cos(radYaw) * glm::cos(radPitch), glm::sin(radPitch), glm::sin(radYaw) * glm::cos(radPitch)));

    right = glm::normalize(glm::cross(front, worldUp));

    up = glm::normalize(glm::cross(right, front));
}