#include "slidergroup.h"


SliderGroup::SliderGroup(QWidget * parent)
    : QWidget{parent}
{
    auto *layout = new QHBoxLayout(this);

    {
        auto *vbox = new QVBoxLayout();
        _colorLabel = new QLabel("Color");
        _colorLabel->setStyleSheet("background-color: rgb(255, 170, 0); color: white;");
        _colorLabel->setAlignment(Qt::AlignCenter);

        _colorSlider = new QSlider(Qt::Horizontal);
        _colorSlider->setRange(0, 359);
        _colorSlider->setValue(40);

        vbox->addWidget(_colorLabel);
        vbox->addWidget(_colorSlider);
        layout->addLayout(vbox);
    }

    {
        auto *vbox = new QVBoxLayout();
        _brightnessLabel = new QLabel("Brightness: 10");
        _brightnessLabel->setStyleSheet("color: white;");
        _brightnessLabel->setAlignment(Qt::AlignCenter);

        _brightnessSlider = new QSlider(Qt::Horizontal);
        _brightnessSlider->setRange(0, 100);
        _brightnessSlider->setValue(10);

        vbox->addWidget(_brightnessLabel);
        vbox->addWidget(_brightnessSlider);
        layout->addLayout(vbox);
    }

    {
        auto *vbox = new QVBoxLayout();
        _tresholdLabel = new QLabel("Treshold: 4");
        _tresholdLabel->setStyleSheet("color: white;");
        _tresholdLabel->setAlignment(Qt::AlignCenter);

        _tresholdSlider = new QSlider(Qt::Horizontal);
        _tresholdSlider->setRange(0, 50);
        _tresholdSlider->setValue(40);

        vbox->addWidget(_tresholdLabel);
        vbox->addWidget(_tresholdSlider);
        layout->addLayout(vbox);
    }

    {
        auto *vbox = new QVBoxLayout();
        _maxIterationLabel = new QLabel("Max Iteration: 1000");
        _maxIterationLabel->setStyleSheet("color: white;");
        _maxIterationLabel->setAlignment(Qt::AlignCenter);

        _maxIterationSlider = new QSlider(Qt::Horizontal);
        _maxIterationSlider->setRange(1, 5000);
        _maxIterationSlider->setValue(1000);

        vbox->addWidget(_maxIterationLabel);
        vbox->addWidget(_maxIterationSlider);
        layout->addLayout(vbox);
    }

    {
        auto *vbox = new QVBoxLayout();
        _zoomSpeedLabel = new QLabel("Zoom Speed: 1.1");
        _zoomSpeedLabel->setStyleSheet("color: white;");
        _zoomSpeedLabel->setAlignment(Qt::AlignCenter);

        _zoomSpeedSlider = new QSlider(Qt::Horizontal);
        _zoomSpeedSlider->setRange(11, 50);
        _zoomSpeedSlider->setValue(11);

        vbox->addWidget(_zoomSpeedLabel);
        vbox->addWidget(_zoomSpeedSlider);
        layout->addLayout(vbox);
    }

    connect(_colorSlider, &QSlider::valueChanged, this, &SliderGroup::onColorSliderChanged);
    connect(_brightnessSlider, &QSlider::valueChanged, this, &SliderGroup::onBrightnessSliderChanged);
    connect(_tresholdSlider, &QSlider::valueChanged, this, &SliderGroup::onTresholdSliderChanged);
    connect(_maxIterationSlider, &QSlider::valueChanged, this, &SliderGroup::onMaxIterationSliderChanged);
    connect(_zoomSpeedSlider, &QSlider::valueChanged, this, &SliderGroup::onZoomSpeedSliderChanged);
}

void SliderGroup::onColorSliderChanged(int value)
{
    QColor hsv = QColor::fromHsv(value, 255, 255);
    QString style = QString("background-color: rgb(%1, %2, %3); color: white;")
                        .arg(hsv.red()).arg(hsv.green()).arg(hsv.blue());
    _colorLabel->setStyleSheet(style);
    emit colorChanged(hsv);
}

void SliderGroup::onBrightnessSliderChanged(int value)
{
    _brightnessLabel->setText(QString("Brightness: %1").arg(value));
    emit brightnessChanged(value);
}

void SliderGroup::onTresholdSliderChanged(int value)
{
    _tresholdLabel->setText(QString("Treshold: %1").arg(value / 10.0f));
    emit tresholdChanged(value / 10.0f);
}

void SliderGroup::onMaxIterationSliderChanged(int value)
{
    _maxIterationLabel->setText(QString("Max Iteration: %1").arg(value));
    emit maxIterationChanged(value);
}

void SliderGroup::onZoomSpeedSliderChanged(int value)
{
    _zoomSpeedLabel->setText(QString("Zoom Speed: %1").arg(value / 10.0f));
    emit zoomSpeedChanged(value / 10.0f);
}
