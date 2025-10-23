#pragma once

#ifndef SLIDERGROUP_H
#define SLIDERGROUP_H

#include <QWidget>
#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QColor>

class SliderGroup : public QWidget
{
	Q_OBJECT

public:
    explicit SliderGroup(QWidget * parent = nullptr);

signals:
    void colorChanged(const QColor &color);
    void tresholdChanged(float value);
    void maxIterationChanged(float value);
    void zoomSpeedChanged(float value);
    void brightnessChanged(float value);

private slots:
    void onColorSliderChanged(int value);
    void onTresholdSliderChanged(int value);
    void onMaxIterationSliderChanged(int value);
    void onZoomSpeedSliderChanged(int value);
    void onBrightnessSliderChanged(int value);

private:
    QSlider *_colorSlider;
    QSlider *_tresholdSlider;
    QSlider *_maxIterationSlider;
    QSlider *_zoomSpeedSlider;
    QSlider *_brightnessSlider;

    QLabel *_colorLabel;
    QLabel *_tresholdLabel;
    QLabel *_maxIterationLabel;
    QLabel *_zoomSpeedLabel;
    QLabel *_brightnessLabel;
};

#endif// SLIDERGROUP_H
