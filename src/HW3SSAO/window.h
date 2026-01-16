#ifndef WINDOW_H
#define WINDOW_H

#pragma once

#include <QOpenGLContext>
#include <Base/GLWidget.hpp>
#include <QElapsedTimer>
#include <QTimer>
#include <QKeyEvent>
#include <QPoint>
#include <QMouseEvent>

#include "window.h"
#include "camera.h"
#include "model.h"
#include "light.h"
#include "slidergroup.h"

class Window : public fgl::GLWidget
{
    Q_OBJECT

public:
    Window() noexcept;
    ~Window() override;

    // fgl::GLWidget
    void onInit() override;
    void onRender() override;
    void onResize(const size_t width, const size_t height) override;
    void processInput();

    void keyPressEvent(QKeyEvent * event) override;
    void keyReleaseEvent(QKeyEvent * event) override;

    void mousePressEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;

    void fillKernel();

private:
    void updateLights();

    std::shared_ptr<QOpenGLContext> context_;
    std::shared_ptr<QOpenGLShaderProgram> program_;
    std::shared_ptr<QOpenGLShaderProgram> geomProgram_;
    std::shared_ptr<QOpenGLShaderProgram> quadProgram_;
    std::shared_ptr<QOpenGLShaderProgram> ssaoProgram_;
    std::shared_ptr<Camera> camera_;
    std::unique_ptr<Model> gltfmodel_;
    std::unique_ptr<Light> light_;
    bool useTexture_{true};

    QOpenGLBuffer vbo_{QOpenGLBuffer::Type::VertexBuffer};
    QOpenGLBuffer ibo_{QOpenGLBuffer::Type::IndexBuffer};
    QOpenGLVertexArrayObject vao_;

    QMatrix4x4 model_;
    QMatrix4x4 view_;
    QMatrix4x4 projection_;

    bool firstMouse_{true};
    QPoint lastMousePos_;

    QSet<int> pressedKeys_;
    QTimer * inputTimer_;
    QElapsedTimer deltaTimer_;

    GLint useTextureUniform_ = -1;

    GLint width_ = 800;
    GLint height_ = 600;
    GLfloat aspect_ = 800.0 / 600.0;

    GLuint quadTexture_ = -1;

    GLuint geometryBuffer_ = -1;
    GLuint geometryPosition_ = -1;
    GLuint ssaoBuffer_ = -1;
    GLuint ssaoTexture_ = -1;

    GLint kernelSize_ = 128;
    GLfloat kernelRadius_ = 0.05;
    GLfloat sampleRadius_ = 0.5;

    GLfloat ambientIntensity_ = 1.4f;
    QVector3D ambientColor_ = QVector3D(1.0f, 1.0f, 1.0f);

    GLfloat sl1Intensity_ = 0.7f;
    QVector3D sl1Color_ = QVector3D(1.0f, 0.666666f, 0.0f);
    QVector3D sl1Position_ = QVector3D(1.0f, 0.5f, 1.1f);
    QVector3D sl1Target_ = QVector3D(0.0f, 0.0f, 0.0f);
    GLfloat sl1Constant_ = 1.0f;
    GLfloat sl1Linear_ = 0.09f;
    GLfloat sl1Quadratic_ = 0.032f;
    GLfloat sl1Specular_ = 0.5f;
    GLfloat sl1Cutoff_ = 10.0f;
    GLfloat sl1OuterCutoff = 20.0f;

    GLfloat sl2Intensity_ = 0.2f;
    QVector3D sl2Color_ = QVector3D(1.0f, 0.111111f, 0.0f);
    GLfloat xPos = -1.0f;
    GLfloat yPos = -0.5f;
    GLfloat zPos = 1.0f;
    GLfloat xTarget = 0.0f;
    GLfloat yTarget = 0.0f;
    GLfloat zTarget = 0.0f;
    GLfloat sl2Constant_ = 1.0f;
    GLfloat sl2Linear_ = 0.09f;
    GLfloat sl2Quadratic_ = 0.032f;
    GLfloat sl2Specular_ = 0.5f;
    GLfloat sl2Cutoff_ = 10.0f;
    GLfloat sl2OuterCutoff = 12.0f;

    QVector3D sl2Position_ = QVector3D(xPos, yPos, zPos);
    QVector3D sl2Target_ = QVector3D(xTarget, yTarget, zTarget);

    //for fps

    QElapsedTimer timer_;
    size_t frameCount_ = 0;

    struct {
        size_t fps = 0;
    } ui_;

private:

    class PerfomanceMetricsGuard final
    {
    public:
        explicit PerfomanceMetricsGuard(std::function<void()> callback);
        ~PerfomanceMetricsGuard();

        PerfomanceMetricsGuard(const PerfomanceMetricsGuard &) = delete;
        PerfomanceMetricsGuard(PerfomanceMetricsGuard &&) = delete;

        PerfomanceMetricsGuard & operator=(const PerfomanceMetricsGuard &) = delete;
        PerfomanceMetricsGuard & operator=(PerfomanceMetricsGuard &&) = delete;

    private:
        std::function<void()> callback_;
    };

private:
    [[nodiscard]] PerfomanceMetricsGuard captureMetrics();

signals:
    void updateUI();

};

#endif// WINDOW_H
