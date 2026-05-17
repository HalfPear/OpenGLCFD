#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp> 
#include <glm/glm/gtc/type_ptr.hpp>    
#include"Grid3D.h"

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};


constexpr float CAM_YAW = -90.0f;
constexpr float CAM_PITCH = 0.0f;
constexpr float CAM_SPEED = 2.5f;
constexpr float CAM_SENSITIVITY = 0.1f;
constexpr float CAM_ZOOM = 45.0f;

class Camera {
public:
 
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    // Euler Angles
    float Yaw;
    float Pitch;

    // Options
    float Zoom;
    float Sensitivity;
    float MovementSpeed;

    Camera(glm::vec3 position = { 0.0f, 0.0f, 3.0f },
        glm::vec3 up = { 0.0f, 1.0f, 0.0f },
        float yaw = CAM_YAW,
        float pitch = CAM_PITCH);

    glm::mat4 GetViewMatrix() const;

    void ProcessKeyboard(Camera_Movement direction, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
    void ProcessMouseScroll(float yoffset);
    void FocusOnDomain(const Grid3D& g, float dist = 6.0f) {
        Position = glm::vec3(0.0f, 0.0f, dist);
        Yaw = -90.0f;
        Pitch = 0.0f;
        updateCameraVectors();
    }
private:
    void updateCameraVectors();
};
