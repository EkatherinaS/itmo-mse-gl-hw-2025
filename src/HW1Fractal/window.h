#ifndef WINDOW_H
#define WINDOW_H

#pragma once

#include <Base/GLWidget.hpp>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QElapsedTimer>

class Window : public fgl::GLWidget
{
	Q_OBJECT

public:
    Window() noexcept;
    ~Window() override;

    // fgl::GLWidget
    void onInit() override;
    void onRender() override;
    void onResize(size_t width, size_t height) override;

    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent *e) override;

private:
    std::unique_ptr<QOpenGLShaderProgram> program_;

    QOpenGLVertexArrayObject vao_;
    QOpenGLBuffer vbo_{QOpenGLBuffer::Type::VertexBuffer};
    QOpenGLBuffer ibo_{QOpenGLBuffer::Type::IndexBuffer};

    QMatrix4x4 model_;
    QMatrix4x4 view_;
    QMatrix4x4 projection_;

    QVector2D mouseNewPos_;
    QVector2D mouseOldPos_;

    QVector2D center_;
    QVector2D resolution_;
    float zoom_;
    float exponent_;

    QVector3D color_;
    float brightness_;
    float maxIteration_;
    float treshold_;
    float zoomSpeed_;

    GLint mvpUniform_ = -1;
    GLint centerUniform_ = -1;
    GLint zoomUniform_ = -1;

    GLint colorUniform_ = -1;
    GLint brightnessUniform_ = -1;
    GLint maxIterationUniform_ = -1;
    GLint tresholdUniform_ = -1;
    GLint zoomSpeedUniform_ = -1;

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
