#include "window.h"
#include "camera.h"
#include "model.h"
#include "slidergroup.h"

#include <iostream>
#include <QVBoxLayout>
#include <QLabel>


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

    // model sliders
    connect(sliders, &SliderGroup::morphingScaleChanged,
            this, [this](float v){ morphingScale_ = v; update(); });

    connect(sliders, &SliderGroup::useTextureChanged,
            this, [this](bool v){ useTexture_ = v; update(); });

    // ambient light sliders
    connect(sliders, &SliderGroup::ambientIntensityChanged,
            this, [this](float v){ ambientIntensity_ = v; updateLights(); });

    connect(sliders, &SliderGroup::ambientColorChanged,
            this, [this](const QVector3D& v){ ambientColor_ = v; updateLights(); });

    // spotlight sliders
    connect(sliders, &SliderGroup::sl2IntensityChanged,
            this, [this](float v){ sl2Intensity_ = v; updateLights(); });

    connect(sliders, &SliderGroup::sl2ColorChanged,
            this, [this](const QVector3D& v){ sl2Color_ = v; updateLights(); });

    connect(sliders, &SliderGroup::sl2PositionChanged,
            this, [this](float x, float y, float z){
                xPos=x; yPos=y; zPos=z;
                updateLights();
            });

    connect(sliders, &SliderGroup::sl2TargetChanged,
            this, [this](float x, float y, float z){
                xTarget=x; yTarget=y; zTarget=z;
                updateLights();
            });

    connect(sliders, &SliderGroup::sl2ConstantChanged,
            this, [this](float v){ sl2Constant_ = v; updateLights(); });

    connect(sliders, &SliderGroup::sl2LinearChanged,
            this, [this](float v){ sl2Linear_ = v; updateLights(); });

    connect(sliders, &SliderGroup::sl2QuadraticChanged,
            this, [this](float v){ sl2Quadratic_ = v; updateLights(); });

    connect(sliders, &SliderGroup::sl2SpecularChanged,
            this, [this](float v){ sl2Specular_ = v; updateLights(); });

    connect(sliders, &SliderGroup::sl2CutoffChanged,
            this, [this](float v){ sl2Cutoff_ = v; updateLights(); });

    connect(sliders, &SliderGroup::sl2OuterCutoffChanged,
            this, [this](float v){ sl2OuterCutoff = v; updateLights(); });

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
        model_.reset();
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
    light_->initialize(program_.get(), context_.get(), camera_.get());
    update();
}

void Window::onInit()
{
    context_ = std::make_shared<QOpenGLContext>();
    camera_ = std::make_shared<Camera>();
    program_ = std::make_shared<QOpenGLShaderProgram>();
    model_ = std::make_unique<Model>();
    light_ = std::make_unique<Light>();

    program_->addCacheableShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/morphing.vs");
    program_->addCacheableShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/morphing.fs");
    program_->link();

    program_->bind();
    useTextureUniform_ = program_->uniformLocation("useTexture");
    morphingScaleUniform_ = program_->uniformLocation("morphingScale");
    program_->release();

    light_->setAmbientLight(ambientIntensity_, ambientColor_);
    light_->addSpotLight(sl1Intensity_, sl1Color_,
                         sl1Position_, sl1Target_,
                         sl1Constant_, sl1Linear_, sl1Quadratic_, sl1Specular_,
                         sl1Cutoff_, sl1OuterCutoff);
    light_->addSpotLight(sl2Intensity_, sl2Color_,
                         sl2Position_, sl2Target_,
                         sl2Constant_, sl2Linear_, sl2Quadratic_, sl2Specular_,
                         sl2Cutoff_, sl2OuterCutoff);

    model_->loadGLTF(":/Models/cube.glb");
    camera_->setPosition(QVector3D(0.0f, 0.0f, 2.0f));

    model_->initialize(program_.get(), context_.get(), camera_.get());
    light_->initialize(program_.get(), context_.get(), camera_.get());

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT);
}

void Window::onRender()
{
    const auto guard = captureMetrics();

    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    program_->bind();
    program_->setUniformValue(morphingScaleUniform_, morphingScale_);
    program_->setUniformValue(useTextureUniform_, useTexture_);
    program_->release();

    ++frameCount_;

    model_->render();
    light_->render();

    update();
}

void Window::onResize(const size_t width, const size_t height)
{
    glViewport(0, 0, static_cast<GLint>(width), static_cast<GLint>(height));

    const auto aspect = static_cast<float>(width) / static_cast<float>(height);
    const auto zNear = 0.1f;
    const auto zFar = 1000.0f;
    const auto fov = 60.0f;
    camera_->perspective(fov, aspect, zNear, zFar);
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


