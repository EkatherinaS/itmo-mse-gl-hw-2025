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

    // ssao sliders
    void ssaoKernelRadiusChanged(float value);
    void ssaoSampleRadiusChanged(float value);
    void ssaoKernelSizeChanged(float value);
    void useTextureChanged(bool value);

    // ambient light sliders
    void ambientIntensityChanged(float);
    void ambientColorChanged(const QVector3D&);

    // spotlight sliders
    void sl1IntensityChanged(float);
    void sl1ColorChanged(const QVector3D&);

    void sl2IntensityChanged(float);
    void sl2ColorChanged(const QVector3D&);


private slots:
    void onUseTextureChanged(bool value);
    void onSsaoKernelRadiusChanged(int value);
    void onSsaoSampleRadiusChanged(int value);
    void onSsaoKernelSizeChanged(int value);

    void onAmbientColorHSVChanged(int value);
    void onSL1ColorHSVChanged(int value);
    void onSL2ColorHSVChanged(int value);

private:
    QCheckBox *_useTextureCheckBox;
    QSlider *_ssaoKernelRadiusSlider;
    QSlider *_ssaoSampleRadiusSlider;
    QSlider *_ssaoKernelSizeSlider;

    QLabel *_useTextureLabel;
    QLabel *_ssaoKernelRadiusLabel;
    QLabel *_ssaoSampleRadiusLabel;
    QLabel *_ssaoKernelSizeLabel;

    // --- Ambient ---
    QSlider *ambientIntensitySlider_;
    QLabel *ambientIntensityLabel_;
    QSlider *ambientColorSlider_;
    QLabel *ambientColorLabel_;

    // --- SL1 ---
    QSlider *sl1IntensitySlider_;
    QLabel *sl1IntensityLabel_;
    QSlider *sl1ColorSlider_;
    QLabel *sl1ColorLabel_;

    // --- SL2 ---
    QSlider *sl2IntensitySlider_;
    QLabel *sl2IntensityLabel_;
    QSlider *sl2ColorSlider_;
    QLabel *sl2ColorLabel_;
};

#endif // SLIDERGROUP_H
