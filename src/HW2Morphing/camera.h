#ifndef CAMERA_H
#define CAMERA_H

#pragma once

#include <QMatrix4x4>

class Camera
{
public:
    Camera();
    ~Camera() = default;
    void perspective(float fov, float aspect, float zNear, float zFar);
    const QMatrix4x4 & getMvp();
    const QVector3D & getPosition();
    void setPosition(const QVector3D & position);
    void processKeyboardInput(const QSet<int> & pressedKeys, float deltaTime);
    void processMouseMovement(float xOffset, float yOffset);

private:
    QMatrix4x4 model_;
    QMatrix4x4 view_;
    QMatrix4x4 projection_;

    void updateCameraVectors();
    void updateViewMatrix();

    QVector3D position_{0.0f, 0.0f, 0.0f};
    QVector3D front_{0.0f, 0.0f, -1.0f};
    QVector3D up_{0.0f, 1.0f, 0.0f};
    QVector3D right_{1.0f, 0.0f, 0.0f};
    QVector3D worldUp_{0.0f, 1.0f, 0.0f};

    float yaw_{-90.0f};
    float pitch_{0.0f};

    float maxPitch_{89.0f};
    float minPitch_{-89.0f};

    float mouseSensitivity_{0.1f};
    float moveSpeed_{1.0f};
};

#endif// CAMERA_H
