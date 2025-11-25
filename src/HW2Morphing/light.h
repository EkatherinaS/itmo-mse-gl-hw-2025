#ifndef LIGHT_H
#define LIGHT_H

#include <vector>
#include <optional>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include "camera.h"

class Light
{
public:
    Light() = default;
    ~Light() = default;

    void initialize(QOpenGLShaderProgram * program, QOpenGLContext * context, Camera * camera);
    void setAmbientLight(float intensity, QVector3D color);
    bool addSpotLight(float intensity, QVector3D color,
                      QVector3D position, QVector3D target,
                      float constant, float linear, float quadratic, float specular,
                      float cutoff, float outerCutoff);
    void render();

private:

    const int MAX_LIGHT_NUMBER = 16;

    struct Source {
        float intensity = 1.0f;
        QVector3D color {0, 0, 0};

        Source() {}

    };

    struct AmbientLight : Source {
        AmbientLight() = default;

        AmbientLight(float i, QVector3D c) {
            intensity = i;
            color = c;
        }
    };

    struct SpotLight : Source {
        QVector3D position {0, 0, 0};
        QVector3D direction {0, 0, 0};
        float constant = 0.0f;
        float linear = 0.0f;
        float quadratic = 0.0f;
        float specular = 0.0f;
        float cutoff = 0.0f;
        float outerCutoff = 0.0f;

        SpotLight() = default;

        SpotLight(float intensity_, QVector3D color_,
                  QVector3D position_, QVector3D direction_,
                  float constant_, float linear_, float quadratic_, float specular_,
                  float cutoff_, float outerCutoff_) {
            intensity = intensity_;
            color = color_;
            position = position_;
            direction = direction_;
            constant = constant_;
            linear = linear_;
            quadratic = quadratic_;
            specular = specular_;
            cutoff = cutoff_;
            outerCutoff = outerCutoff_;
        }
    };

    QOpenGLShaderProgram * program_;
    QOpenGLContext * context_;
    Camera * camera_;
    AmbientLight ambient_;
    std::vector<SpotLight> spotlight_ = {};

    GLint lightCountUniform_ = -1;
    GLint lightsUniform_ = -1;
};

#endif// LIGHT_H
