#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum class Direction
{
    Forward,
    Backward,
    Left,
    Right,
    Down,
    Up,
};

class Camera
{
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;

    float speed;
    float sensitivity;

    Camera(glm::vec3 position);

    void goTo(glm::vec3 position);

    void move(Direction direction, float deltaTime);

    void rotate(float x, float y, float deltaTime);

    glm::mat4 getViewMatrix() const;

    glm::vec3 getPosition() const;

private:
    void calculateVectors();
};