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
    std::unique_ptr<QOpenGLShaderProgram> _program;

    QOpenGLVertexArrayObject _vao;
    QOpenGLBuffer _vbo{QOpenGLBuffer::Type::VertexBuffer};
    QOpenGLBuffer _ibo{QOpenGLBuffer::Type::IndexBuffer};

    QMatrix4x4 _model;
    QMatrix4x4 _view;
    QMatrix4x4 _projection;

    QVector2D _mouseNewPos;
    QVector2D _mouseOldPos;

    QVector2D _center;
    QVector2D _resolution;
    float _zoom;
    float _exponent;

    QVector3D _color;
    float _brightness;
    float _maxIteration;
    float _treshold;
    float _zoomSpeed;

    GLint _mvpUniform = -1;
    GLint _centerUniform = -1;
    GLint _zoomUniform = -1;

    GLint _colorUniform = -1;
    GLint _brightnessUniform = -1;
    GLint _maxIterationUniform = -1;
    GLint _tresholdUniform = -1;
    GLint _zoomSpeedUniform = -1;

//for fps

    QElapsedTimer _timer;
    size_t _frameCount = 0;

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
