#include "camera.h"


inline float degreesToRadians(float degrees)
{
    return degrees * static_cast<float>(M_PI) / 180.0f;
}

Camera::Camera() {
    view_.setToIdentity();
    model_.setToIdentity();
    projection_.setToIdentity();
}

const QMatrix4x4 & Camera::getMvp() {
    return projection_ * view_ * model_;
}

void Camera::setPosition(const QVector3D & position)
{
    position_ = position;
    updateViewMatrix();
}

const QVector3D & Camera::getPosition() {
    return position_;
}

void Camera::perspective(float fov, float aspect, float zNear, float zFar) {
    projection_.setToIdentity();
    projection_.perspective(fov, aspect, zNear, zFar);
}

void Camera::processKeyboardInput(const QSet<int> & pressedKeys, float deltaTime)
{
    float velocity = moveSpeed_ * deltaTime;

    if (pressedKeys.contains(Qt::Key_W))
    {
        position_ += front_ * velocity;
    }
    if (pressedKeys.contains(Qt::Key_S))
    {
        position_ -= front_ * velocity;
    }
    if (pressedKeys.contains(Qt::Key_A))
    {
        position_ -= right_ * velocity;
    }
    if (pressedKeys.contains(Qt::Key_D))
    {
        position_ += right_ * velocity;
    }
    if (pressedKeys.contains(Qt::Key_Space))
    {
        position_ += up_ * velocity;
    }
    if (pressedKeys.contains(Qt::Key_Shift))
    {
        position_ -= up_ * velocity;
    }

    updateViewMatrix();
}


void Camera::processMouseMovement(float xOffset, float yOffset)
{
    xOffset *= mouseSensitivity_;
    yOffset *= mouseSensitivity_;

    yaw_ += xOffset;
    pitch_ += yOffset;

    pitch_ = qBound(minPitch_, pitch_, maxPitch_);

    updateCameraVectors();
}

void Camera::updateCameraVectors()
{
    QVector3D front;
    front.setX(cos(degreesToRadians(yaw_)) * cos(degreesToRadians(pitch_)));
    front.setY(sin(degreesToRadians(pitch_)));
    front.setZ(sin(degreesToRadians(yaw_)) * cos(degreesToRadians(pitch_)));
    front_ = front.normalized();

    right_ = QVector3D::crossProduct(front_, worldUp_).normalized();
    up_ = QVector3D::crossProduct(right_, front_).normalized();

    updateViewMatrix();
}

void Camera::updateViewMatrix()
{
    view_.setToIdentity();
    view_.lookAt(position_, position_ + front_, up_);
}
