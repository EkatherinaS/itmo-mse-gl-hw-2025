#include <iostream>
#include <QVBoxLayout>
#include <QLabel>
#include <cstdlib>
#include <random>

#include "window.h"

namespace
{

constexpr std::array<GLfloat, 16u> vertices = {
    -0.5f, -0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, 1.0f, 0.0f,
    0.5f,  0.5f, 1.0f, 1.0f,
    -0.5f,  0.5f, 0.0f,  1.0f
};

constexpr std::array<GLuint, 6u> indices = {0, 1, 2, 2, 3, 0};

}// namespace


Window::Window() noexcept {
    auto *root = new QHBoxLayout();
    auto *leftLayout = new QVBoxLayout();

    // fps
    const auto formatFPS = [](const auto value) {
        return QString("FPS: %1").arg(QString::number(value));
    };

    auto fps = new QLabel(formatFPS(0), this);
    fps->setStyleSheet("QLabel { color : white; }");
    leftLayout->addWidget(fps);

    timer_.start();
    connect(this, &Window::updateUI, this, [this, fps, formatFPS] {
        fps->setText(formatFPS(ui_.fps));
    });

    leftLayout->addStretch();

    QWidget *leftContainer = new QWidget();
    leftContainer->setLayout(leftLayout);

    auto *sliders = new SliderGroup(this);

    root->addWidget(leftContainer);
    root->addWidget(sliders);

    root->setStretch(0, 8);
    root->setStretch(1, 2);

    setLayout(root);

    // ssao sliders

    connect(sliders, &SliderGroup::ssaoKernelRadiusChanged,
            this, [this](float v){ kernelRadius_ = v; update(); });

    connect(sliders, &SliderGroup::ssaoSampleRadiusChanged,
            this, [this](float v){ sampleRadius_ = v; update(); });

    connect(sliders, &SliderGroup::ssaoKernelSizeChanged,
            this, [this](float v){ kernelSize_ = v; update(); });

    connect(sliders, &SliderGroup::useTextureChanged,
            this, [this](bool v){ useTexture_ = v; update(); });

    // ambient light sliders

    connect(sliders, &SliderGroup::ambientIntensityChanged,
            this, [this](float v){ ambientIntensity_ = v; updateLights(); });

    connect(sliders, &SliderGroup::ambientColorChanged,
            this, [this](const QVector3D& v){ ambientColor_ = v; updateLights(); });

    // spotlight sliders

    connect(sliders, &SliderGroup::sl1IntensityChanged,
            this, [this](float v){ sl1Intensity_ = v; updateLights(); });

    connect(sliders, &SliderGroup::sl1ColorChanged,
            this, [this](const QVector3D& v){ sl1Color_ = v; updateLights(); });

    connect(sliders, &SliderGroup::sl2IntensityChanged,
            this, [this](float v){ sl2Intensity_ = v; updateLights(); });

    connect(sliders, &SliderGroup::sl2ColorChanged,
            this, [this](const QVector3D& v){ sl2Color_ = v; updateLights(); });

    // timers for mouse tracking

    deltaTimer_.start();

    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    inputTimer_ = new QTimer(this);
    connect(inputTimer_, &QTimer::timeout, this, &Window::processInput);
    inputTimer_->start(16);
}

Window::~Window()
{
    {
        ContextGuard guard = bindContext();
        program_.reset();
        context_.reset();
        camera_.reset();
        light_.reset();
        gltfmodel_.reset();
    }
}

void Window::updateLights() {
    light_.reset();
    light_ = std::make_unique<Light>();
    light_->setAmbientLight(ambientIntensity_, ambientColor_);

    sl2Position_ = QVector3D(xPos, yPos, zPos);
    sl2Target_ = QVector3D(xTarget, yTarget, zTarget);

    light_->addSpotLight(sl1Intensity_, sl1Color_,
                         sl1Position_, sl1Target_,
                         sl1Constant_, sl1Linear_, sl1Quadratic_, sl1Specular_,
                         sl1Cutoff_, sl1OuterCutoff);
    light_->addSpotLight(sl2Intensity_, sl2Color_,
                         sl2Position_, sl2Target_,
                         sl2Constant_, sl2Linear_, sl2Quadratic_, sl2Specular_,
                         sl2Cutoff_, sl2OuterCutoff);
    light_->initialize(program_.get());
    update();
}

void Window::fillKernel() {
    QVector<QVector3D> kernelValues = {};

    std::uniform_real_distribution<float> randomFloats(0.0f, 1.0f);
    std::default_random_engine generator;

    kernelValues.reserve(kernelSize_);
    for (unsigned int i = 0; i < kernelSize_; ++i)
    {
        QVector3D sample(
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator) * 2.0f - 1.0f,
            randomFloats(generator)
            );
        sample.normalize();
        sample *= randomFloats(generator);
        float scale = static_cast<float>(i) / static_cast<float>(kernelSize_);
        scale = 0.1f + (scale * scale) * (1.0f - 0.1f);
        sample *= scale;

        kernelValues.push_back(sample);
    }

    ssaoProgram_->setUniformValueArray("gKernel", kernelValues.constData(), kernelSize_);
}

void Window::onInit()
{
    context_ = std::make_shared<QOpenGLContext>();
    camera_ = std::make_shared<Camera>();
    program_ = std::make_shared<QOpenGLShaderProgram>();
    ssaoProgram_ = std::make_shared<QOpenGLShaderProgram>();
    quadProgram_ = std::make_shared<QOpenGLShaderProgram>();
    geomProgram_ = std::make_shared<QOpenGLShaderProgram>();
    gltfmodel_ = std::make_unique<Model>();
    light_ = std::make_unique<Light>();


    geomProgram_->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/geometry.vs");
    geomProgram_->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/geometry.fs");
    geomProgram_->link();

    ssaoProgram_->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/ssao.vs");
    ssaoProgram_->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/ssao.fs");
    ssaoProgram_->link();

    program_->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/output.vs");
    program_->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/output.fs");
    program_->link();

    quadProgram_->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/quad.vs");
    quadProgram_->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/quad.fs");
    quadProgram_->link();


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

    quadProgram_->bind();
    quadProgram_->enableAttributeArray(0);
    quadProgram_->setAttributeBuffer(0, GL_FLOAT, 0, 2, static_cast<int>(4 * sizeof(GLfloat)));
    quadProgram_->enableAttributeArray(1);
    quadProgram_->setAttributeBuffer(1, GL_FLOAT, static_cast<int>(2 * sizeof(GLfloat)), 2, static_cast<int>(4 * sizeof(GLfloat)));
    quadProgram_->release();

    vao_.release();
    ibo_.release();
    vbo_.release();

    camera_->setPosition(QVector3D(0.0f, 0.0f, 2.0f));

    light_->setAmbientLight(ambientIntensity_, ambientColor_);
    light_->addSpotLight(sl1Intensity_, sl1Color_,
                         sl1Position_, sl1Target_,
                         sl1Constant_, sl1Linear_, sl1Quadratic_, sl1Specular_,
                         sl1Cutoff_, sl1OuterCutoff);
    light_->addSpotLight(sl2Intensity_, sl2Color_,
                         sl2Position_, sl2Target_,
                         sl2Constant_, sl2Linear_, sl2Quadratic_, sl2Specular_,
                         sl2Cutoff_, sl2OuterCutoff);
    light_->initialize(program_.get());

    gltfmodel_->loadGLTF(":/Models/portal_gun_with_download.glb");
    gltfmodel_->initialize(context_.get(), camera_.get());
    gltfmodel_->addQOpenGLShaderProgram(program_.get());
    gltfmodel_->addQOpenGLShaderProgram(geomProgram_.get());
    gltfmodel_->addQOpenGLShaderProgram(ssaoProgram_.get());

    // CREATE G_BUFFER
    glGenFramebuffers(1, &geometryBuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, geometryBuffer_);

    // POSITION COLOR TEXTURE
    glGenTextures(1, &geometryPosition_);
    glBindTexture(GL_TEXTURE_2D, geometryPosition_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width_, height_, 0, GL_RGB, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, geometryPosition_, 0);

    // CREATE DEPTH TEXTURE
    glGenTextures(1, &quadTexture_);
    glBindTexture(GL_TEXTURE_2D, quadTexture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, quadTexture_, 0);

    // CREATE SSAO_BUFFER
    glGenFramebuffers(1, &ssaoBuffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBuffer_);

    // CREATE SSAO TEXTURE
    glGenTextures(1, &ssaoTexture_);
    glBindTexture(GL_TEXTURE_2D, ssaoTexture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width_, height_, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoTexture_, 0);

    view_.setToIdentity();
    model_.setToIdentity();
    model_.scale(0.5);
    model_.scale(1.0, aspect_);
    model_.translate(-1.2 * aspect_, -1.2, 0.0f);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::onRender()
{
    const auto guard = captureMetrics();

    glBindFramebuffer(GL_FRAMEBUFFER, geometryBuffer_);
    glViewport(0, 0, width_, height_);

    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    gltfmodel_->render(geomProgram_.get());

    // -> geometryPosition_ with pos
    // -> quadTexture_ with depth

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBuffer_);
    glViewport(0, 0, width_, height_);

    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    ssaoProgram_->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, geometryPosition_);
    ssaoProgram_->setUniformValue("positionMap", 0);
    ssaoProgram_->setUniformValue("sampleRadius", sampleRadius_);
    ssaoProgram_->setUniformValue("kernelRadius", kernelRadius_);
    ssaoProgram_->setUniformValue("kernelSize", kernelSize_);
    fillKernel();
    gltfmodel_->render(ssaoProgram_.get());
    ssaoProgram_->release();

    // -> ssaoTexture_ with ssao

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width_, height_);

    glClear(GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);

    program_->bind();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ssaoTexture_);
    program_->setUniformValue("ssaoTexture", 0);
    program_->setUniformValue("useTexture", useTexture_);
    gltfmodel_->render(program_.get());
    light_->render();
    program_->release();

    const auto mvp = projection_ * view_ * model_;

    glDisable(GL_DEPTH_TEST);
    quadProgram_->bind();
    vao_.bind();
    glBindTexture(GL_TEXTURE_2D, ssaoTexture_);
    quadProgram_->setUniformValue("outputTexture", 0);
    quadProgram_->setUniformValue("mvp", mvp);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    vao_.release();
    quadProgram_->release();
    glEnable(GL_DEPTH_TEST);

    ++frameCount_;

    update();
}

void Window::onResize(const size_t width, const size_t height)
{
    glViewport(0, 0, static_cast<GLint>(width), static_cast<GLint>(height));
    width_ = static_cast<GLint>(width);
    height_ = static_cast<GLint>(height);

    const auto aspect = static_cast<float>(width) / static_cast<float>(height);
    const auto zNear = 0.1f;
    const auto zFar = 1000.0f;
    const auto fov = 60.0f;

    camera_->perspective(fov, aspect, zNear, zFar);

    glBindTexture(GL_TEXTURE_2D, geometryPosition_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width_, height_, 0, GL_RGB, GL_FLOAT, 0);

    glBindTexture(GL_TEXTURE_2D, quadTexture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width_, height_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    glBindTexture(GL_TEXTURE_2D, ssaoTexture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width_, height_, 0, GL_RED, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Window::processInput()
{
    if (!camera_) return;
    float deltaTime = deltaTimer_.restart() / 1000.0f;
    deltaTime = qMin(deltaTime, 0.1f);
    camera_->processKeyboardInput(pressedKeys_, deltaTime);
}

void Window::keyPressEvent(QKeyEvent * event)
{
    pressedKeys_.insert(event->key());
    fgl::GLWidget::keyPressEvent(event);
}

void Window::keyReleaseEvent(QKeyEvent * event)
{
    pressedKeys_.remove(event->key());
    fgl::GLWidget::keyReleaseEvent(event);
}


void Window::mousePressEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton)
    {
        lastMousePos_ = event->pos();
        firstMouse_ = true;
    }
    fgl::GLWidget::mousePressEvent(event);
}

void Window::mouseMoveEvent(QMouseEvent * event)
{
    if (!camera_)
        return;

    if (event->buttons() & Qt::LeftButton)
    {
        if (firstMouse_)
        {
            lastMousePos_ = event->pos();
            firstMouse_ = false;
        }

        float xOffset = event->pos().x() - lastMousePos_.x();
        float yOffset = lastMousePos_.y() - event->pos().y();

        lastMousePos_ = event->pos();

        camera_->processMouseMovement(xOffset, yOffset);
    }

    fgl::GLWidget::mouseMoveEvent(event);
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


