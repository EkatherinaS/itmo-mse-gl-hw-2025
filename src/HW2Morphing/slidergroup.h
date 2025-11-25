#pragma once

#ifndef SLIDERGROUP_H
#define SLIDERGROUP_H

#include <QWidget>
#include <QSlider>
#include <QCheckBox>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QColor>
#include <QVector3D>

class SliderGroup : public QWidget
{
    Q_OBJECT

public:
    explicit SliderGroup(QWidget * parent = nullptr);

signals:
    void useTextureChanged(bool value);
    void morphingScaleChanged(float value);

    void ambientIntensityChanged(float);
    void ambientColorChanged(const QVector3D&);

    void sl2IntensityChanged(float);
    void sl2ColorChanged(const QVector3D&);

    void sl2PositionChanged(float x, float y, float z);
    void sl2TargetChanged(float x, float y, float z);

    void sl2ConstantChanged(float);
    void sl2LinearChanged(float);
    void sl2QuadraticChanged(float);
    void sl2SpecularChanged(float);
    void sl2CutoffChanged(float);
    void sl2OuterCutoffChanged(float);

private slots:
    void onUseTextureChanged(bool value);
    void onMorphingScaleChanged(int value);

    // *** NEW HSV SLOT FUNCTIONS ***
    void onAmbientColorHSVChanged(int value);
    void onSL2ColorHSVChanged(int value);

private:
    QCheckBox *_useTextureCheckBox;
    QSlider *_morphingScaleSlider;
    QLabel *_useTextureLabel;
    QLabel *_morphingScaleLabel;

    // --- Ambient ---
    QSlider *ambientIntensitySlider_;
    QLabel *ambientIntensityLabel_;
    QSlider *ambientColorSlider_;  // HSV wheel
    QLabel *ambientColorLabel_;

    // --- SL2 ---
    QSlider *sl2IntensitySlider_;
    QLabel *sl2IntensityLabel_;
    QSlider *sl2ColorSlider_;      // HSV wheel
    QLabel *sl2ColorLabel_;

    // Position
    QSlider *sl2XPos_;
    QSlider *sl2YPos_;
    QSlider *sl2ZPos_;

    // Target
    QSlider *sl2XTarget_;
    QSlider *sl2YTarget_;
    QSlider *sl2ZTarget_;

    // Attenuation & specular
    QSlider *sl2Constant_;
    QSlider *sl2Linear_;
    QSlider *sl2Quadratic_;
    QSlider *sl2Specular_;
    QSlider *sl2Cutoff_;
    QSlider *sl2OuterCutoff_;
};

#endif // SLIDERGROUP_H
