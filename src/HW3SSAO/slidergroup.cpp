#include "slidergroup.h"
#include <QString>
#include <QColor>
#include <iostream>


SliderGroup::SliderGroup(QWidget * parent)
    : QWidget{parent}
{
    auto *layout = new QVBoxLayout(this);

    //
    // USE TEXTURE
    //
    {
        auto *v = new QHBoxLayout();
        _useTextureLabel = new QLabel("Use Texture: ON");
        _useTextureLabel->setStyleSheet("color:white;");
        _useTextureLabel->setAlignment(Qt::AlignCenter);

        _useTextureCheckBox = new QCheckBox();
        _useTextureCheckBox->setChecked(true);

        v->addWidget(_useTextureLabel);
        v->addWidget(_useTextureCheckBox);
        layout->addLayout(v);

        connect(_useTextureCheckBox, &QCheckBox::stateChanged,
                this, &SliderGroup::onUseTextureChanged);
    }

    //
    // SSAO KERNEL SIZE
    //
    {
        auto *v = new QHBoxLayout();
        _ssaoKernelSizeLabel = new QLabel("Kernel Size: 128");
        _ssaoKernelSizeLabel->setStyleSheet("color:white;");
        _ssaoKernelSizeLabel->setAlignment(Qt::AlignCenter);

        _ssaoKernelSizeSlider = new QSlider(Qt::Horizontal);
        _ssaoKernelSizeSlider->setRange(0, 256);
        _ssaoKernelSizeSlider->setValue(128);

        v->addWidget(_ssaoKernelSizeLabel);
        v->addWidget(_ssaoKernelSizeSlider);
        layout->addLayout(v);

        connect(_ssaoKernelSizeSlider, &QSlider::valueChanged,
                this, &SliderGroup::onSsaoKernelSizeChanged);
    }

    //
    // SSAO KERNEL RADIUS
    //
    {
        auto *v = new QHBoxLayout();
        _ssaoKernelRadiusLabel = new QLabel("SSAO Kernel Radius: 0.05");
        _ssaoKernelRadiusLabel->setStyleSheet("color:white;");
        _ssaoKernelRadiusLabel->setAlignment(Qt::AlignCenter);

        _ssaoKernelRadiusSlider = new QSlider(Qt::Horizontal);
        _ssaoKernelRadiusSlider->setRange(0, 100);
        _ssaoKernelRadiusSlider->setValue(5);

        v->addWidget(_ssaoKernelRadiusLabel);
        v->addWidget(_ssaoKernelRadiusSlider);
        layout->addLayout(v);

        connect(_ssaoKernelRadiusSlider, &QSlider::valueChanged,
                this, &SliderGroup::onSsaoKernelRadiusChanged);
    }

    //
    // SSAO SAMPLE RADIUS
    //
    {
        auto *v = new QHBoxLayout();
        _ssaoSampleRadiusLabel = new QLabel("SSAO Sample Radius: 0.5");
        _ssaoSampleRadiusLabel->setStyleSheet("color:white;");
        _ssaoSampleRadiusLabel->setAlignment(Qt::AlignCenter);

        _ssaoSampleRadiusSlider = new QSlider(Qt::Horizontal);
        _ssaoSampleRadiusSlider->setRange(0, 100);
        _ssaoSampleRadiusSlider->setValue(50);

        v->addWidget(_ssaoSampleRadiusLabel);
        v->addWidget(_ssaoSampleRadiusSlider);
        layout->addLayout(v);

        connect(_ssaoSampleRadiusSlider, &QSlider::valueChanged,
                this, &SliderGroup::onSsaoSampleRadiusChanged);
    }

    //
    // AMBIENT INTENSITY (with value)
    //
    {
        auto *v = new QHBoxLayout();
        ambientIntensityLabel_ = new QLabel("Ambient Intensity: 1.40");
        ambientIntensityLabel_->setStyleSheet("color:white;");
        ambientIntensityLabel_->setAlignment(Qt::AlignCenter);

        ambientIntensitySlider_ = new QSlider(Qt::Horizontal);
        ambientIntensitySlider_->setRange(0, 500);
        ambientIntensitySlider_->setValue(140);

        v->addWidget(ambientIntensityLabel_);
        v->addWidget(ambientIntensitySlider_);
        layout->addLayout(v);

        connect(ambientIntensitySlider_, &QSlider::valueChanged, this,
                [this](int v){
                    float f = v / 100.0f;
                    ambientIntensityLabel_->setText(
                        QString("Ambient Intensity: %1").arg(QString::number(f, 'f', 2))
                        );
                    emit ambientIntensityChanged(f);
                });
    }

    //
    // AMBIENT COLOR (HSV)
    //
    {
        auto *v = new QVBoxLayout();
        ambientColorLabel_ = new QLabel("Ambient Color (HSV)");
        ambientColorLabel_->setAlignment(Qt::AlignCenter);

        QColor start = QColor::fromHsv(0, 0, 255);
        ambientColorLabel_->setStyleSheet(
            QString("background-color: rgb(%1,%2,%3); color:white;")
                .arg(start.red()).arg(start.green()).arg(start.blue())
            );

        ambientColorSlider_ = new QSlider(Qt::Horizontal);
        ambientColorSlider_->setRange(0, 359);
        ambientColorSlider_->setValue(0);

        v->addWidget(ambientColorLabel_);
        v->addWidget(ambientColorSlider_);
        layout->addLayout(v);

        connect(ambientColorSlider_, &QSlider::valueChanged,
                this, &SliderGroup::onAmbientColorHSVChanged);
    }

    //
    // sl1 INTENSITY
    //
    {
        auto *v = new QHBoxLayout();
        sl1IntensityLabel_ = new QLabel("Intensity: 0.20");
        sl1IntensityLabel_->setStyleSheet("color:white;");
        sl1IntensityLabel_->setAlignment(Qt::AlignCenter);

        sl1IntensitySlider_ = new QSlider(Qt::Horizontal);
        sl1IntensitySlider_->setRange(0, 200);
        sl1IntensitySlider_->setValue(20);

        v->addWidget(sl1IntensityLabel_);
        v->addWidget(sl1IntensitySlider_);
        layout->addLayout(v);

        connect(sl1IntensitySlider_, &QSlider::valueChanged, this,
                [this](int v){
                    float f = v / 100.0f;
                    sl1IntensityLabel_->setText(
                        QString("Intensity: %1").arg(QString::number(f,'f',2))
                        );
                    emit sl1IntensityChanged(f);
                });
    }

    //
    // sl1 COLOR (HSV)
    //
    {
        auto *v = new QVBoxLayout();
        sl1ColorLabel_ = new QLabel("SL1 Color (HSV)");
        sl1ColorLabel_->setAlignment(Qt::AlignCenter);

        QColor base = QColor::fromHsv(42, 255, 255);
        sl1ColorLabel_->setStyleSheet(
            QString("background-color: rgb(%1,%2,%3); color:white;")
                .arg(base.red()).arg(base.green()).arg(base.blue())
            );

        sl1ColorSlider_ = new QSlider(Qt::Horizontal);
        sl1ColorSlider_->setRange(0, 359);
        sl1ColorSlider_->setValue(42);

        v->addWidget(sl1ColorLabel_);
        v->addWidget(sl1ColorSlider_);
        layout->addLayout(v);

        connect(sl1ColorSlider_, &QSlider::valueChanged,
                this, &SliderGroup::onSL1ColorHSVChanged);
    }


    //
    // SL2 INTENSITY
    //
    {
        auto *v = new QHBoxLayout();
        sl2IntensityLabel_ = new QLabel("Intensity: 0.20");
        sl2IntensityLabel_->setStyleSheet("color:white;");
        sl2IntensityLabel_->setAlignment(Qt::AlignCenter);

        sl2IntensitySlider_ = new QSlider(Qt::Horizontal);
        sl2IntensitySlider_->setRange(0, 200);
        sl2IntensitySlider_->setValue(20);

        v->addWidget(sl2IntensityLabel_);
        v->addWidget(sl2IntensitySlider_);
        layout->addLayout(v);

        connect(sl2IntensitySlider_, &QSlider::valueChanged, this,
                [this](int v){
                    float f = v / 100.0f;
                    sl2IntensityLabel_->setText(
                        QString("Intensity: %1").arg(QString::number(f,'f',2))
                        );
                    emit sl2IntensityChanged(f);
                });
    }

    //
    // SL2 COLOR (HSV)
    //
    {
        auto *v = new QVBoxLayout();
        sl2ColorLabel_ = new QLabel("SL2 Color (HSV)");
        sl2ColorLabel_->setAlignment(Qt::AlignCenter);

        QColor base = QColor::fromHsv(42, 255, 255);
        sl2ColorLabel_->setStyleSheet(
            QString("background-color: rgb(%1,%2,%3); color:white;")
                .arg(base.red()).arg(base.green()).arg(base.blue())
            );

        sl2ColorSlider_ = new QSlider(Qt::Horizontal);
        sl2ColorSlider_->setRange(0, 359);
        sl2ColorSlider_->setValue(42);

        v->addWidget(sl2ColorLabel_);
        v->addWidget(sl2ColorSlider_);
        layout->addLayout(v);

        connect(sl2ColorSlider_, &QSlider::valueChanged,
                this, &SliderGroup::onSL2ColorHSVChanged);
    }
}

void SliderGroup::onUseTextureChanged(bool value)
{
    _useTextureLabel->setText(value ? "Use Texture: ON" : "Use Texture: OFF");
    emit useTextureChanged(value);
}

void SliderGroup::onSsaoKernelRadiusChanged(int value)
{
    float f = value / 100.0f;
    _ssaoKernelRadiusLabel->setText(
        QString("SSAO Kernel Radius: %1").arg(QString::number(f,'f',2))
        );
    emit ssaoKernelRadiusChanged(f);
}

void SliderGroup::onSsaoSampleRadiusChanged(int value)
{
    float f = value / 100.0f;
    _ssaoSampleRadiusLabel->setText(
        QString("SSAO Sample Radius: %1").arg(QString::number(f,'f',2))
        );
    emit ssaoSampleRadiusChanged(f);
}

void SliderGroup::onSsaoKernelSizeChanged(int value)
{
    _ssaoKernelSizeLabel->setText(
        QString("SSAO Kernel Size: %1").arg(value)
        );
    emit ssaoKernelSizeChanged(value);
}

//
// HSV COLOR HANDLERS
//

void SliderGroup::onAmbientColorHSVChanged(int value)
{
    QColor hsv = QColor::fromHsv(value, 255, 255);

    ambientColorLabel_->setStyleSheet(
        QString("background-color: rgb(%1,%2,%3); color:white;")
            .arg(hsv.red()).arg(hsv.green()).arg(hsv.blue())
        );

    emit ambientColorChanged(QVector3D(
        hsv.redF(), hsv.greenF(), hsv.blueF()
        ));
}

void SliderGroup::onSL1ColorHSVChanged(int value)
{
    QColor hsv = QColor::fromHsv(value, 255, 255);

    sl1ColorLabel_->setStyleSheet(
        QString("background-color: rgb(%1,%2,%3); color:white;")
            .arg(hsv.red()).arg(hsv.green()).arg(hsv.blue())
        );

    emit sl1ColorChanged(QVector3D(
        hsv.redF(), hsv.greenF(), hsv.blueF()
        ));
}

void SliderGroup::onSL2ColorHSVChanged(int value)
{
    QColor hsv = QColor::fromHsv(value, 255, 255);

    sl2ColorLabel_->setStyleSheet(
        QString("background-color: rgb(%1,%2,%3); color:white;")
            .arg(hsv.red()).arg(hsv.green()).arg(hsv.blue())
        );

    emit sl2ColorChanged(QVector3D(
        hsv.redF(), hsv.greenF(), hsv.blueF()
        ));
}
