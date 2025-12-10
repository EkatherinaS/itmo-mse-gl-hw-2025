#include "model.h"
#include <iostream>
#include <QFile>
#include <QOpenGLShader>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <tinygltf/tiny_gltf.h>


Model::~Model() {
    vaos_.clear();
    vbos_.clear();
    ibos_.clear();
    textures_.clear();
    meshes_.clear();
}

void Model::initialize(QOpenGLShaderProgram * program, QOpenGLContext * context, Camera * camera) {
    program_ = program;
    context_ = context;
    camera_ = camera;

    if (!program_) return;

    program_->bind();

    program_->setUniformValue("diffuseTexture", 0);
    mvpUniform_ = program_->uniformLocation("mvp");
    modelUniform_ = program_->uniformLocation("model");
    maxSizeUniform_ = program_->uniformLocation("maxSize");
    viewUniform_ = program_->uniformLocation("viewPos");

    program_->release();

    for (const auto & mesh: meshes_)
    {
        auto vao = std::make_unique<QOpenGLVertexArrayObject>();
        vao->create();
        vao->bind();

        auto vbo = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::Type::VertexBuffer);
        vbo->create();
        vbo->bind();
        vbo->setUsagePattern(QOpenGLBuffer::StaticDraw);
        vbo->allocate(mesh.vertices.data(), static_cast<int>(mesh.vertices.size() * sizeof(Vertex)));

        auto ibo = std::make_unique<QOpenGLBuffer>(QOpenGLBuffer::Type::IndexBuffer);
        ibo->create();
        ibo->bind();
        ibo->setUsagePattern(QOpenGLBuffer::StaticDraw);
        ibo->allocate(mesh.indices.data(), static_cast<int>(mesh.indices.size() * sizeof(uint32_t)));

        program_->bind();
        program_->enableAttributeArray(0);
        program_->setAttributeBuffer(0, GL_FLOAT, offsetof(Vertex, position), 3, sizeof(Vertex));

        program_->enableAttributeArray(1);
        program_->setAttributeBuffer(1, GL_FLOAT, offsetof(Vertex, normal), 3, sizeof(Vertex));

        program_->enableAttributeArray(2);
        program_->setAttributeBuffer(2, GL_FLOAT, offsetof(Vertex, texCoord), 2, sizeof(Vertex));
        program_->release();

        vao->release();

        vaos_.push_back(std::move(vao));
        vbos_.push_back(std::move(vbo));
        ibos_.push_back(std::move(ibo));
    }
}

void Model::render() {

    program_->bind();

    QMatrix4x4 mvpCamera = camera_->getMvp();
    const auto mvp = mvpCamera * transform_;
    program_->setUniformValue(mvpUniform_, mvp);
    program_->setUniformValue(modelUniform_, transform_);
    program_->setUniformValue(viewUniform_, camera_->getPosition());
    program_->setUniformValue(maxSizeUniform_, maxSize_);

    for (size_t i = 0; i < meshes_.size(); ++i)
    {
        const auto & mesh = meshes_[i];

        vaos_[i]->bind();

        if (mesh.textureIndex >= 0)
        {
            textures_[mesh.textureIndex]->bind(0);
        }

        context_->functions()->glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(mesh.indices.size()), GL_UNSIGNED_INT, nullptr);

        if (mesh.textureIndex >= 0)
        {
            textures_[mesh.textureIndex]->release();
        }

        vaos_[i]->release();
    }
    program_->release();
}

void Model::loadGLTF(const QString & path) {

    QFile modelFile(path);
    if (!modelFile.open(QIODevice::ReadOnly)) return;

    QByteArray modelData = modelFile.readAll();
    modelFile.close();
    if (modelData.isEmpty()) return;

    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err, warn;
    const unsigned char * data = reinterpret_cast<const unsigned char *>(modelData.data());
    unsigned int size = static_cast<unsigned int>(modelData.size());

    bool success = loader.LoadBinaryFromMemory(&model, &err, &warn, data, size);
    if (!success) return;

    for (const auto & texture: model.textures)
    {
        if (texture.source >= 0 && texture.source < static_cast<int>(model.images.size()))
        {
            const auto & image = model.images[texture.source];

            QImage qimg;
            if (image.component == 3)
            {
                qimg = QImage(image.image.data(), image.width, image.height, QImage::Format_RGB888);
            }
            else if (image.component == 4)
            {
                qimg = QImage(image.image.data(), image.width, image.height, QImage::Format_RGBA8888);
            }

            auto tex = std::make_unique<QOpenGLTexture>(qimg);
            tex->setMinMagFilters(QOpenGLTexture::LinearMipMapLinear, QOpenGLTexture::Linear);
            tex->setWrapMode(QOpenGLTexture::Repeat);
            tex->generateMipMaps();

            textures_.push_back(std::move(tex));
        }
    }

    float xMin = INFINITY;
    float yMin = INFINITY;
    float zMin = INFINITY;
    float xMax = -INFINITY;
    float yMax = -INFINITY;
    float zMax = -INFINITY;
    for (const auto & mesh: model.meshes)
    {
        for (const auto & primitive: mesh.primitives)
        {
            Mesh meshData;

            if (primitive.attributes.find("POSITION") != primitive.attributes.end())
            {
                const auto & accessor = model.accessors[primitive.attributes.at("POSITION")];
                const auto & bufferView = model.bufferViews[accessor.bufferView];
                const auto & buffer = model.buffers[bufferView.buffer];

                const float * positions = reinterpret_cast<const float *>(
                    &buffer.data[bufferView.byteOffset + accessor.byteOffset]);

                meshData.vertices.resize(accessor.count);
                for (size_t i = 0; i < accessor.count; ++i)
                {
                    if (positions[i * 3 + 0] > xMax) xMax = positions[i * 3 + 0];
                    if (positions[i * 3 + 0] < xMin) xMin = positions[i * 3 + 0];
                    if (positions[i * 3 + 1] > yMax) yMax = positions[i * 3 + 1];
                    if (positions[i * 3 + 1] < yMin) yMin = positions[i * 3 + 1];
                    if (positions[i * 3 + 2] > zMax) zMax = positions[i * 3 + 2];
                    if (positions[i * 3 + 2] < zMin) zMin = positions[i * 3 + 2];

                    meshData.vertices[i].position[0] = positions[i * 3 + 0];
                    meshData.vertices[i].position[1] = positions[i * 3 + 1];
                    meshData.vertices[i].position[2] = positions[i * 3 + 2];
                }
            }

            if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
            {
                const auto & accessor = model.accessors[primitive.attributes.at("NORMAL")];
                const auto & bufferView = model.bufferViews[accessor.bufferView];
                const auto & buffer = model.buffers[bufferView.buffer];

                const float * normals = reinterpret_cast<const float *>(
                    &buffer.data[bufferView.byteOffset + accessor.byteOffset]);

                for (size_t i = 0; i < accessor.count && i < meshData.vertices.size(); ++i)
                {
                    meshData.vertices[i].normal[0] = normals[i * 3 + 0];
                    meshData.vertices[i].normal[1] = normals[i * 3 + 1];
                    meshData.vertices[i].normal[2] = normals[i * 3 + 2];
                }
            }

            if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
            {
                const auto & accessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
                const auto & bufferView = model.bufferViews[accessor.bufferView];
                const auto & buffer = model.buffers[bufferView.buffer];

                const float * texCoords = reinterpret_cast<const float *>(
                    &buffer.data[bufferView.byteOffset + accessor.byteOffset]);

                for (size_t i = 0; i < accessor.count && i < meshData.vertices.size(); ++i)
                {
                    meshData.vertices[i].texCoord[0] = texCoords[i * 2 + 0];
                    meshData.vertices[i].texCoord[1] = texCoords[i * 2 + 1];
                }
            }
            if (primitive.indices >= 0)
            {
                const auto & accessor = model.accessors[primitive.indices];
                const auto & bufferView = model.bufferViews[accessor.bufferView];
                const auto & buffer = model.buffers[bufferView.buffer];

                meshData.indices.resize(accessor.count);

                if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                {
                    const uint16_t * indices = reinterpret_cast<const uint16_t *>(
                        &buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                    for (size_t i = 0; i < accessor.count; ++i)
                    {
                        meshData.indices[i] = indices[i];
                    }
                }
                else if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
                {
                    const uint32_t * indices = reinterpret_cast<const uint32_t *>(
                        &buffer.data[bufferView.byteOffset + accessor.byteOffset]);
                    for (size_t i = 0; i < accessor.count; ++i)
                    {
                        meshData.indices[i] = indices[i];
                    }
                }
            }
            if (primitive.material >= 0 && primitive.material < static_cast<int>(model.materials.size()))
            {
                const auto & material = model.materials[primitive.material];
                if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
                {
                    meshData.textureIndex = material.pbrMetallicRoughness.baseColorTexture.index;
                }
            }

            meshes_.push_back(std::move(meshData));
        }

        maxSize_ = qMax(qMax(xMax - xMin, yMax - yMin), zMax - zMin);
    }
}
