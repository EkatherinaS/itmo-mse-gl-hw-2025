#include "light.h"
#include <iostream>

inline float degreesToRadians(float degrees)
{
    return degrees * static_cast<float>(M_PI) / 180.0f;
}

void Light::initialize(QOpenGLShaderProgram * program) {
    program_ = program;
}

void Light::setAmbientLight(float intensity, QVector3D color) {
    ambient_ = AmbientLight(intensity, color);
}

bool Light::addSpotLight(float intensity, QVector3D color,
                         QVector3D position, QVector3D target,
                         float constant, float linear, float quadratic, float specular,
                         float cutoff, float outerCutoff) {
    if (spotlight_.size() == MAX_LIGHT_NUMBER) return false;
    SpotLight source = SpotLight(intensity, color, position, target,
                                 constant, linear, quadratic, specular,
                                 degreesToRadians(cutoff), degreesToRadians(outerCutoff));
    spotlight_.push_back(source);
    return true;
}

void Light::render() {

    if (!program_) return;

    program_->bind();

    GLint ambientColorUniform = program_->uniformLocation("ambientLight.color");
    GLint ambientIntensityUniform = program_->uniformLocation("ambientLight.intensity");

    program_->setUniformValue(ambientColorUniform, ambient_.color);
    program_->setUniformValue(ambientIntensityUniform, ambient_.intensity);

    for (int i = 0; i < spotlight_.size(); i++) {
        QString base = QString("spotlights[%1].").arg(i);

        GLint colorUniform = program_->uniformLocation(base + "color");
        GLint intensityUniform = program_->uniformLocation(base + "intensity");
        GLint positionUniform = program_->uniformLocation(base + "position");
        GLint directionUniform = program_->uniformLocation(base + "direction");

        GLint cutoffUniform = program_->uniformLocation(base + "cutoff");
        GLint outerCutoffUniform = program_->uniformLocation(base + "outerCutoff");

        GLint constantUniform = program_->uniformLocation(base + "constant");
        GLint linearUniform = program_->uniformLocation(base + "linear");
        GLint quadraticUniform = program_->uniformLocation(base + "quadratic");
        GLint specularUniform = program_->uniformLocation(base + "specular");

        program_->setUniformValue(colorUniform, spotlight_[i].color);
        program_->setUniformValue(intensityUniform, spotlight_[i].intensity);
        program_->setUniformValue(positionUniform, spotlight_[i].position);
        program_->setUniformValue(directionUniform, spotlight_[i].direction);

        program_->setUniformValue(cutoffUniform, spotlight_[i].cutoff);
        program_->setUniformValue(outerCutoffUniform, spotlight_[i].outerCutoff);

        program_->setUniformValue(constantUniform, spotlight_[i].constant);
        program_->setUniformValue(linearUniform, spotlight_[i].linear);
        program_->setUniformValue(quadraticUniform, spotlight_[i].quadratic);
        program_->setUniformValue(specularUniform, spotlight_[i].specular);
    }

    GLint spotlightCountUniform = program_->uniformLocation("spotlightCount");
    program_->setUniformValue(spotlightCountUniform, int(spotlight_.size()));

    program_->release();
}
