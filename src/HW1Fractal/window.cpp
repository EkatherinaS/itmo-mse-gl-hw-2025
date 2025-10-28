#include "window.h"
#include "slidergroup.h"

#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QVBoxLayout>
#include <QScreen>
#include <iostream>
#include <array>

namespace
{

constexpr std::array<GLfloat, 8u> vertices = {
    -1.0f, -1.0f,
    1.0f, -1.0f,
    1.0f,  1.0f,
    -1.0f,  1.0f
};

constexpr std::array<GLuint, 6u> indices = {0, 1, 2, 2, 3, 0};

}// namespace

Window::Window() noexcept
{
    QVBoxLayout *layout = new QVBoxLayout();

    //add fps label

    const auto formatFPS = [](const auto value) {
        return QString("FPS: %1").arg(QString::number(value));
    };

    auto fps = new QLabel(formatFPS(0), this);
    fps->setStyleSheet("QLabel { color : white; }");
    layout->addWidget(fps, 1);

    _timer.start();

    connect(this, &Window::updateUI, this, [this, fps, formatFPS] {
        fps->setText(formatFPS(ui_.fps));
    });

    //add sliders

    layout->addStretch();

    auto *sliders = new SliderGroup(this);
    layout->addWidget(sliders);

    connect(sliders, &SliderGroup::colorChanged, this, [this](const QColor &c) {
        _color = QVector3D(c.redF(), c.greenF(), c.blueF());
        update();
    });
    connect(sliders, &SliderGroup::brightnessChanged, this, [this](float v) {
        _brightness = v;
        update();
    });
    connect(sliders, &SliderGroup::tresholdChanged, this, [this](float v) {
        _treshold = v;
        update();
    });
    connect(sliders, &SliderGroup::maxIterationChanged, this, [this](float v) {
        _maxIteration = v;
        update();
    });
    connect(sliders, &SliderGroup::zoomSpeedChanged, this, [this](float v) {
        _zoomSpeed = v;
        update();
    });

    setLayout(layout);
}

Window::~Window()
{
    {
        ContextGuard guard = bindContext();
        //reset all unique ptrs
        _program.reset();
    }
}

void Window::onInit()
{
    // Configure shaders
    _program = std::make_unique<QOpenGLShaderProgram>(this);
    _program->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/fractal.vs");
    _program->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/fractal.fs");
    bool shadersLinkSuccessful = _program->link();
    if (!shadersLinkSuccessful) {
        std::cout << "shaders link failed: "
                  << _program->log().toStdString()
                  << std::endl;
    } else {
        std::cout << "shaders link successful" << std::endl;
    }

    // Create VAO object
    _vao.create();
    _vao.bind();

    // Create VBO
    _vbo.create();
    _vbo.bind();
    _vbo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    _vbo.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(GLfloat)));

    // Create IBO
    _ibo.create();
    _ibo.bind();
    _ibo.setUsagePattern(QOpenGLBuffer::StaticDraw);
    _ibo.allocate(indices.data(), static_cast<int>(indices.size() * sizeof(GLuint)));

    // Bind attributes

    //connection with vertex shader
    _program->bind();

    //layout(location=0) in vec2 pos;
    _program->enableAttributeArray(0);
    _program->setAttributeBuffer(0, GL_FLOAT, 0, 2, static_cast<int>(2*sizeof(GLfloat)));

    //set uniforms
    _mvpUniform = _program->uniformLocation("mvp");
    _centerUniform = _program->uniformLocation("center");
    _zoomUniform = _program->uniformLocation("zoom");

    _colorUniform = _program->uniformLocation("color");
    _maxIterationUniform = _program->uniformLocation("maxIteration");
    _brightnessUniform = _program->uniformLocation("brightness");
    _tresholdUniform = _program->uniformLocation("treshold");

    // Release all
    _program->release();
    _vao.release();
    _ibo.release();
    _vbo.release();

    // Default values
    _zoom = 1.0f;
    _exponent = 0.0f;
    _center = QVector2D(0, 0);

    _color = QVector3D(1.0f, 0.666666f, 0.0f);
    _brightness = 10.0f;
    _maxIteration = 1000;
    _treshold = 4.0f;
    _zoomSpeed = 1.1f;

    // Еnable face culling
    glEnable(GL_CULL_FACE);

    // Clear all FBO buffers
    glClear(GL_COLOR_BUFFER_BIT);
}

void Window::onRender()
{
    const auto guard = captureMetrics();

    // Clear buffers
    glClear(GL_COLOR_BUFFER_BIT);

    // Calculate MVP matrix
    _view.setToIdentity();
    _model.setToIdentity();
    _model.translate(0, 0, -1);

    const QMatrix4x4 mvp = _projection * _view * _model;

    // Bind VAO and shader program
    _program->bind();

    // Update uniform value
    _program->setUniformValue(_mvpUniform, mvp);
    _program->setUniformValue(_zoomUniform, _zoom);
    _program->setUniformValue(_centerUniform, _center);

    _program->setUniformValue(_colorUniform, _color);
    _program->setUniformValue(_maxIterationUniform, _maxIteration);
    _program->setUniformValue(_brightnessUniform, _brightness);
    _program->setUniformValue(_tresholdUniform, _treshold);

    _vao.bind();

    // Draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // Release VAO and shader program
    _vao.release();
    _program->release();

    ++_frameCount;

    update();
}

void Window::onResize(const size_t width, const size_t height)
{
    _resolution = QVector2D(float(width), float(height));

    // Configure viewport
    glViewport(0, 0, static_cast<GLint>(width), static_cast<GLint>(height));

    // Configure matrix
    const float aspect = float(height) / float(width);
    const float nearPlane = 0.1f;
    const float farPlane = 100.0f;
    const float left = -1.0f;
    const float right = 1.0f;
    const float top = -1.0f * aspect;
    const float bottom = 1.0f * aspect;

    _projection.setToIdentity();
    _projection.ortho(left, right, top, bottom, nearPlane, farPlane);

    update();
}

void Window::wheelEvent(QWheelEvent *e)
{
    const qreal dpr = devicePixelRatio();
    const float aspect = _resolution.y() / _resolution.x();

    QPoint numDegrees = e->angleDelta();
    _exponent += float(numDegrees.y()) / 360.0;
    float zoomNew = std::pow(_zoomSpeed, _exponent);

    QVector2D mouse = QVector2D(e->position() * dpr);
    QVector2D normMouse = (2.0 * (mouse - _resolution * 0.5) / _resolution);
    QVector2D fragMouse = QVector2D(normMouse.x(), -normMouse.y() * aspect);

    _center += fragMouse / _zoom - fragMouse / zoomNew;
    _zoom = zoomNew;

    update();
    e->accept();
}

void Window::mousePressEvent(QMouseEvent *e)
{
    _mouseNewPos = QVector2D(e->position());
    e->accept();
}

void Window::mouseMoveEvent(QMouseEvent *e)
{
    _mouseOldPos = _mouseNewPos;
    _mouseNewPos = QVector2D(e->position());
    QVector2D diff = _mouseNewPos - _mouseOldPos;

    const qreal dpr = devicePixelRatio();
    QVector2D normDiff = 2.0 * diff * dpr / _resolution / _zoom;

    const float aspect = _resolution.y() / _resolution.x();
    _center -= QVector2D(normDiff.x(), -normDiff.y() * aspect);

    update();
    e->accept();
}

Window::PerfomanceMetricsGuard::PerfomanceMetricsGuard(std::function<void()> callback)
    : callback_{ std::move(callback) }
{
}

Window::PerfomanceMetricsGuard::~PerfomanceMetricsGuard()
{
    if (callback_)
    {
        callback_();
    }
}

auto Window::captureMetrics() -> PerfomanceMetricsGuard
{
    return PerfomanceMetricsGuard{
        [&] {
            if (_timer.elapsed() >= 1000)
            {
                const auto elapsedSeconds = static_cast<float>(_timer.restart()) / 1000.0f;
                ui_.fps = static_cast<size_t>(std::round(_frameCount / elapsedSeconds));
                _frameCount = 0;
                emit updateUI();
            }
        }
    };
}

