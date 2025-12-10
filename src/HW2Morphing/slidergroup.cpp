#include "slidergroup.h"
#include <QString>
#include <QColor>

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
    // MORPHING SCALE
    //
    {
        auto *v = new QHBoxLayout();
        _morphingScaleLabel = new QLabel("Morphing Scale: 0.00");
        _morphingScaleLabel->setStyleSheet("color:white;");
        _morphingScaleLabel->setAlignment(Qt::AlignCenter);

        _morphingScaleSlider = new QSlider(Qt::Horizontal);
        _morphingScaleSlider->setRange(0, 100);
        _morphingScaleSlider->setValue(0);

        v->addWidget(_morphingScaleLabel);
        v->addWidget(_morphingScaleSlider);
        layout->addLayout(v);

        connect(_morphingScaleSlider, &QSlider::valueChanged,
                this, &SliderGroup::onMorphingScaleChanged);
    }

    //
    // AMBIENT INTENSITY (with value)
    //
    {
        auto *v = new QHBoxLayout();
        ambientIntensityLabel_ = new QLabel("Ambient Intensity: 0.40");
        ambientIntensityLabel_->setStyleSheet("color:white;");
        ambientIntensityLabel_->setAlignment(Qt::AlignCenter);

        ambientIntensitySlider_ = new QSlider(Qt::Horizontal);
        ambientIntensitySlider_->setRange(0, 200);
        ambientIntensitySlider_->setValue(40);

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

        QColor start = QColor::fromHsv(40, 255, 255);
        ambientColorLabel_->setStyleSheet(
            QString("background-color: rgb(%1,%2,%3); color:white;")
                .arg(start.red()).arg(start.green()).arg(start.blue())
            );

        ambientColorSlider_ = new QSlider(Qt::Horizontal);
        ambientColorSlider_->setRange(0, 359);
        ambientColorSlider_->setValue(40);

        v->addWidget(ambientColorLabel_);
        v->addWidget(ambientColorSlider_);
        layout->addLayout(v);

        connect(ambientColorSlider_, &QSlider::valueChanged,
                this, &SliderGroup::onAmbientColorHSVChanged);
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

        QColor base = QColor::fromHsv(12, 255, 255);
        sl2ColorLabel_->setStyleSheet(
            QString("background-color: rgb(%1,%2,%3); color:white;")
                .arg(base.red()).arg(base.green()).arg(base.blue())
            );

        sl2ColorSlider_ = new QSlider(Qt::Horizontal);
        sl2ColorSlider_->setRange(0, 359);
        sl2ColorSlider_->setValue(12);

        v->addWidget(sl2ColorLabel_);
        v->addWidget(sl2ColorSlider_);
        layout->addLayout(v);

        connect(sl2ColorSlider_, &QSlider::valueChanged,
                this, &SliderGroup::onSL2ColorHSVChanged);
    }

    //
    // Helper: position & target sliders (displays values)
    //
    auto addPosSlider = [&](const char *name, float init, QSlider*& slider, auto callback)
    {
        auto *v = new QHBoxLayout();
        QLabel *lbl = new QLabel(QString("%1: 0.00").arg(name));
        lbl->setStyleSheet("color:white;");
        lbl->setAlignment(Qt::AlignCenter);

        slider = new QSlider(Qt::Horizontal);
        slider->setRange(-200, 200);
        slider->setValue(init);

        v->addWidget(lbl);
        v->addWidget(slider);
        layout->addLayout(v);

        connect(slider, &QSlider::valueChanged, this,
                [=](int){
                    float x = sl2XPos_     ? sl2XPos_->value()     / 100.0f : 0;
                    float y = sl2YPos_     ? sl2YPos_->value()     / 100.0f : 0;
                    float z = sl2ZPos_     ? sl2ZPos_->value()     / 100.0f : 0;
                    float tx = sl2XTarget_ ? sl2XTarget_->value()  / 100.0f : 0;
                    float ty = sl2YTarget_ ? sl2YTarget_->value()  / 100.0f : 0;
                    float tz = sl2ZTarget_ ? sl2ZTarget_->value()  / 100.0f : 0;

                    float f = slider->value() / 100.0f;
                    lbl->setText(QString("%1: %2")
                                     .arg(name).arg(QString::number(f,'f',2)));

                    callback(x, y, z, tx, ty, tz);
                });
    };

    // POSITION
    addPosSlider("X Pos", -100, sl2XPos_,
                 [this](float x, float y, float z, float, float, float){
                     emit sl2PositionChanged(x, y, z);
                 });
    addPosSlider("Y Pos", -50, sl2YPos_,
                 [this](float x, float y, float z, float, float, float){
                     emit sl2PositionChanged(x, y, z);
                 });
    addPosSlider("Z Pos", 100, sl2ZPos_,
                 [this](float x, float y, float z, float, float, float){
                     emit sl2PositionChanged(x, y, z);
                 });

    // TARGET
    addPosSlider("X Target", 0, sl2XTarget_,
                 [this](float, float, float, float tx, float ty, float tz){
                     emit sl2TargetChanged(tx, ty, tz);
                 });
    addPosSlider("Y Target", 0, sl2YTarget_,
                 [this](float, float, float, float tx, float ty, float tz){
                     emit sl2TargetChanged(tx, ty, tz);
                 });
    addPosSlider("Z Target", 0, sl2ZTarget_,
                 [this](float, float, float, float tx, float ty, float tz){
                     emit sl2TargetChanged(tx, ty, tz);
                 });

    //
    // Generic float slider (also shows value)
    //
    auto addFloatSlider = [&](const char *name, QSlider*& slider, float def, float rangeMin, float rangeMax, auto callback)
    {
        auto *v = new QHBoxLayout();
        QLabel *lbl = new QLabel(QString("%1: %2")
                                      .arg(name).arg(QString::number(def,'f',2)));
        lbl->setStyleSheet("color:white;");
        lbl->setAlignment(Qt::AlignCenter);

        slider = new QSlider(Qt::Horizontal);
        slider->setRange(rangeMin * 100, rangeMax * 100);
        slider->setValue(int(def * 100));

        v->addWidget(lbl);
        v->addWidget(slider);
        layout->addLayout(v);

        connect(slider, &QSlider::valueChanged, this,
                [=](int v){
                    float f = v / 100.0f;
                    lbl->setText(QString("%1: %2")
                                     .arg(name).arg(QString::number(f,'f',2)));
                    callback(f);
                });
    };
    auto addIntSlider = [&](const char *name, QSlider*& slider, float def, float rangeMin, float rangeMax, auto callback)
    {
        auto *v = new QHBoxLayout();
        QLabel *lbl = new QLabel(QString("%1: %2")
                                      .arg(name).arg(QString::number(def,'f',2)));
        lbl->setStyleSheet("color:white;");
        lbl->setAlignment(Qt::AlignCenter);

        slider = new QSlider(Qt::Horizontal);
        slider->setRange(rangeMin, rangeMax);
        slider->setValue(def);

        v->addWidget(lbl);
        v->addWidget(slider);
        layout->addLayout(v);

        connect(slider, &QSlider::valueChanged, this,
                [=](int v){
                    lbl->setText(QString("%1: %2")
                                     .arg(name).arg(v));
                    callback(v);
                });
    };

    addFloatSlider("Constant",    sl2Constant_,    1.0f,  0.0f, 1.0f, [this](float f){ emit sl2ConstantChanged(f); });
    addFloatSlider("Linear",      sl2Linear_,      0.09f, 0.0f, 1.0f, [this](float f){ emit sl2LinearChanged(f); });
    addFloatSlider("Quadratic",   sl2Quadratic_,   0.032f,0.0f, 1.0f, [this](float f){ emit sl2QuadraticChanged(f); });
    addFloatSlider("Specular",    sl2Specular_,    1.0f, 0.0f, 10.0f, [this](float f){ emit sl2SpecularChanged(f); });

    addIntSlider("Cutoff", sl2Cutoff_, 10, 0, 90,
                   [this](float f){ emit sl2CutoffChanged(f * 1.0f); });

    addIntSlider("OuterCutoff", sl2OuterCutoff_, 12, 0, 90,
                   [this](float f){ emit sl2OuterCutoffChanged(f * 1.0f); });
}

//
// SLOT FUNCTIONS
//

void SliderGroup::onUseTextureChanged(bool value)
{
    _useTextureLabel->setText(value ? "Use Texture: ON" : "Use Texture: OFF");
    emit useTextureChanged(value);
}

void SliderGroup::onMorphingScaleChanged(int value)
{
    float f = value / 100.0f;
    _morphingScaleLabel->setText(
        QString("Morphing Scale: %1").arg(QString::number(f,'f',2))
        );
    emit morphingScaleChanged(f);
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
