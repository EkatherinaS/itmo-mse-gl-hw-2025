#ifndef MODEL_H
#define MODEL_H

#pragma once

#include <vector>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QFile>

#include "camera.h"

struct Vertex {
    float position[3];
    float normal[3];
    float texCoord[2];
};

struct Mesh {
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    int textureIndex = -1;
};

class Model
{

public:
    Model() = default;
    ~Model();
    void initialize(QOpenGLContext * context, Camera * camera);
    void addQOpenGLShaderProgram(QOpenGLShaderProgram * program);
    void loadGLTF(const QString &  path);
    void setVertexShader(const QString & path);
    void setFragmentShader(const QString &  path);
    void render(QOpenGLShaderProgram * program);

private:
    QOpenGLContext * context_;
    Camera * camera_;

    QMatrix4x4 transform_;
    QVector3D position_{0.0f, 0.0f, 0.0f};

    std::vector<std::unique_ptr<QOpenGLBuffer>> vbos_;
    std::vector<std::unique_ptr<QOpenGLBuffer>> ibos_;
    std::vector<std::unique_ptr<QOpenGLVertexArrayObject>> vaos_;

    std::vector<std::unique_ptr<QOpenGLTexture>> textures_;
    std::vector<Mesh> meshes_;

    float maxSize_ = -1;

    GLint mvpUniform_ = -1;
    GLint modelUniform_ = -1;
    GLint viewUniform_ = -1;
    GLint ambientColorUniform_ = -1;
    GLint ambientIntensityUniform_ = -1;
    GLint maxSizeUniform_ = -1;
};

#endif// MODEL_H
