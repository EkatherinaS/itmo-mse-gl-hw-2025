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

    timer_.start();

    connect(this, &Window::updateUI, this, [this, fps, formatFPS] {
        fps->setText(formatFPS(ui_.fps));
    });

    //add sliders

    layout->addStretch();

    auto *sliders = new SliderGroup(this);
    layout->addWidget(sliders);

    connect(sliders, &SliderGroup::colorChanged, this, [this](const QColor &c) {
        color_ = QVector3D(c.redF(), c.greenF(), c.blueF());
        update();
    });
    connect(sliders, &SliderGroup::brightnessChanged, this, [this](float v) {
        brightness_ = v;
        update();
    });
    connect(sliders, &SliderGroup::tresholdChanged, this, [this](float v) {
        treshold_ = v;
        update();
    });
    connect(sliders, &SliderGroup::maxIterationChanged, this, [this](float v) {
        maxIteration_ = v;
        update();
    });
    connect(sliders, &SliderGroup::zoomSpeedChanged, this, [this](float v) {
        zoomSpeed_ = v;
        update();
    });

    setLayout(layout);
}

Window::~Window()
{
    {
        ContextGuard guard = bindContext();
        //reset all unique ptrs
        program_.reset();
    }
}

void Window::onInit()
{
    // Configure shaders
    program_ = std::make_unique<QOpenGLShaderProgram>(this);
    program_->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/fractal.vs");
    program_->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/fractal.fs");
    bool shadersLinkSuccessful = program_->link();
    if (!shadersLinkSuccessful) {
        std::cout << "shaders link failed: "
                  << program_->log().toStdString()
                  << std::endl;
    } else {
        std::cout << "shaders link successful" << std::endl;
    }

    // Create VAO object
    vao_.create();
    vao_.bind();

    // Create VBO
    vbo_.create();
    vbo_.bind();
    vbo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
    vbo_.allocate(vertices.data(), static_cast<int>(vertices.size() * sizeof(GLfloat)));

    // Create IBO
    ibo_.create();
    ibo_.bind();
    ibo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
    ibo_.allocate(indices.data(), static_cast<int>(indices.size() * sizeof(GLuint)));

    // Bind attributes

    //connection with vertex shader
    program_->bind();

    //layout(location=0) in vec2 pos;
    program_->enableAttributeArray(0);
    program_->setAttributeBuffer(0, GL_FLOAT, 0, 2, static_cast<int>(2*sizeof(GLfloat)));

    //set uniforms
    mvpUniform_ = program_->uniformLocation("mvp");
    centerUniform_ = program_->uniformLocation("center");
    zoomUniform_ = program_->uniformLocation("zoom");

    colorUniform_ = program_->uniformLocation("color");
    maxIterationUniform_ = program_->uniformLocation("maxIteration");
    brightnessUniform_ = program_->uniformLocation("brightness");
    tresholdUniform_ = program_->uniformLocation("treshold");

    // Release all
    program_->release();
    vao_.release();
    ibo_.release();
    vbo_.release();

    // Default values
    zoom_ = 1.0f;
    exponent_ = 0.0f;
    center_ = QVector2D(0, 0);

    color_ = QVector3D(1.0f, 0.666666f, 0.0f);
    brightness_ = 10.0f;
    maxIteration_ = 1000;
    treshold_ = 4.0f;
    zoomSpeed_ = 1.1f;

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
    view_.setToIdentity();
    model_.setToIdentity();
    model_.translate(0, 0, -1);

    const QMatrix4x4 mvp = projection_ * view_ * model_;

    // Bind VAO and shader program
    program_->bind();

    // Update uniform value
    program_->setUniformValue(mvpUniform_, mvp);
    program_->setUniformValue(zoomUniform_, zoom_);
    program_->setUniformValue(centerUniform_, center_);

    program_->setUniformValue(colorUniform_, color_);
    program_->setUniformValue(maxIterationUniform_, maxIteration_);
    program_->setUniformValue(brightnessUniform_, brightness_);
    program_->setUniformValue(tresholdUniform_, treshold_);

    vao_.bind();

    // Draw
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    // Release VAO and shader program
    vao_.release();
    program_->release();

    ++frameCount_;

    update();
}

void Window::onResize(const size_t width, const size_t height)
{
    resolution_ = QVector2D(float(width), float(height));

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

    projection_.setToIdentity();
    projection_.ortho(left, right, top, bottom, nearPlane, farPlane);

    update();
}

void Window::wheelEvent(QWheelEvent *e)
{
    const qreal dpr = devicePixelRatio();
    const float aspect = resolution_.y() / resolution_.x();

    QPoint numDegrees = e->angleDelta();
    exponent_ += float(numDegrees.y()) / 360.0;
    float zoomNew = std::pow(zoomSpeed_, exponent_);

    QVector2D mouse = QVector2D(e->position() * dpr);
    QVector2D normMouse = (2.0 * (mouse - resolution_ * 0.5) / resolution_);
    QVector2D fragMouse = QVector2D(normMouse.x(), -normMouse.y() * aspect);

    if (zoomNew > 0.0) {
        center_ += fragMouse / zoom_ - fragMouse / zoomNew;
        zoom_ = zoomNew;
    }

    update();
    e->accept();
}

void Window::mousePressEvent(QMouseEvent *e)
{
    mouseNewPos_ = QVector2D(e->position());
    e->accept();
}

void Window::mouseMoveEvent(QMouseEvent *e)
{
    mouseOldPos_ = mouseNewPos_;
    mouseNewPos_ = QVector2D(e->position());
    QVector2D diff = mouseNewPos_ - mouseOldPos_;

    const qreal dpr = devicePixelRatio();
    QVector2D normDiff = 2.0 * diff * dpr / resolution_ / zoom_;

    const float aspect = resolution_.y() / resolution_.x();
    center_ -= QVector2D(normDiff.x(), -normDiff.y() * aspect);

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
            if (timer_.elapsed() >= 1000)
            {
                const auto elapsedSeconds = static_cast<float>(timer_.restart()) / 1000.0f;
                ui_.fps = static_cast<size_t>(std::round(frameCount_ / elapsedSeconds));
                frameCount_ = 0;
                emit updateUI();
            }
        }
    };
}

