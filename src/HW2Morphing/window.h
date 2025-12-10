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

#include "camera.h"
#include "light.h"
#include "model.h"

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

    void mousePressEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);

private:
    void updateLights();

    std::shared_ptr<QOpenGLContext> context_;
    std::shared_ptr<QOpenGLShaderProgram> program_;
    std::shared_ptr<Camera> camera_;
    std::unique_ptr<Model> model_;
    std::unique_ptr<Light> light_;
    bool useTexture_{true};
    float morphingScale_{0.0};

    bool firstMouse_{true};
    QPoint lastMousePos_;

    QSet<int> pressedKeys_;
    QTimer * inputTimer_;
    QElapsedTimer deltaTimer_;

    GLint useTextureUniform_ = -1;
    GLint morphingScaleUniform_ = -1;

    float ambientIntensity_ = 0.4f;
    QVector3D ambientColor_ = QVector3D(1.0f, 0.666666f, 0.0f);

    bool sl1InUse = true;

    float sl1Intensity_ = 0.7f;
    QVector3D sl1Color_ = QVector3D(1.0f, 0.666666f, 0.0f);
    QVector3D sl1Position_ = QVector3D(1.0f, 0.5f, 1.1f);
    QVector3D sl1Target_ = QVector3D(0.0f, 0.0f, 0.0f);
    float sl1Constant_ = 1.0f;
    float sl1Linear_ = 0.09f;
    float sl1Quadratic_ = 0.032f;
    float sl1Specular_ = 0.5f;
    float sl1Cutoff_ = 10.0f;
    float sl1OuterCutoff = 20.0f;

    float sl2Intensity_ = 0.2f;
    QVector3D sl2Color_ = QVector3D(1.0f, 0.111111f, 0.0f);
    float xPos = -1.0f;
    float yPos = -0.5f;
    float zPos = 1.0f;
    float xTarget = 0.0f;
    float yTarget = 0.0f;
    float zTarget = 0.0f;
    float sl2Constant_ = 1.0f;
    float sl2Linear_ = 0.09f;
    float sl2Quadratic_ = 0.032f;
    float sl2Specular_ = 0.5f;
    float sl2Cutoff_ = 10.0f;
    float sl2OuterCutoff = 12.0f;

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
