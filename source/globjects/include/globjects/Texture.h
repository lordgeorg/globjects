#pragma once

#include <glbinding/gl/types.h>

#include <vector>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <globjects/globjects_api.h>
#include <globjects/Object.h>
#include <globjects/TextureHandle.h>

namespace glo {

class Buffer;

/** \brief Wraps OpenGL texture objects.
 * A Texture provides both interfaces to bind them for the OpenGL pipeline:
 * binding and bindless texture. Bindless textures are only available if the
 * graphics driver supports them.
 *
 * \see http://www.opengl.org/wiki/Texture
 * \see http://www.opengl.org/registry/specs/NV/bindless_texture.txt
 */
class GLOBJECTS_API Texture : public Object
{
public:
    Texture();
    Texture(gl::GLenum target);
    static Texture * fromId(gl::GLuint id, gl::GLenum  target);

    static Texture * createDefault();
    static Texture * createDefault(gl::GLenum target);

    virtual void accept(ObjectVisitor & visitor) override;

    void bind() const;
    void unbind() const;
    static void unbind(gl::GLenum target);

    void bindActive(gl::GLenum texture) const;
    void unbindActive(gl::GLenum texture) const;

    void setParameter(gl::GLenum name, gl::GLenum value);
    void setParameter(gl::GLenum name, gl::GLint value);
    void setParameter(gl::GLenum name, gl::GLfloat value);

    gl::GLint getParameter(gl::GLenum pname) const;
    gl::GLint getLevelParameter(gl::GLint level, gl::GLenum pname) const;

    void getImage(gl::GLint level, gl::GLenum format, gl::GLenum type, gl::GLvoid * image) const;
    std::vector<unsigned char> getImage(gl::GLint level, gl::GLenum format, gl::GLenum type) const;

    void getCompressedImage(gl::GLint lod, gl::GLvoid * image) const;
    std::vector<unsigned char> getCompressedImage(gl::GLint lod = 0) const;

    gl::GLenum target() const;

    void image1D(gl::GLint level, gl::GLenum internalFormat, gl::GLsizei width, gl::GLint border, gl::GLenum format, gl::GLenum type, const gl::GLvoid * data);
    void compressedImage1D(gl::GLint level, gl::GLenum internalFormat, gl::GLsizei width, gl::GLint border, gl::GLsizei imageSize, const gl::GLvoid * data);
    void subImage1D(gl::GLint level, gl::GLint xOffset, gl::GLsizei width, gl::GLenum format, gl::GLenum type, const gl::GLvoid * data);

    void image2D(gl::GLint level, gl::GLenum internalFormat, gl::GLsizei width, gl::GLsizei height, gl::GLint border, gl::GLenum format, gl::GLenum type, const gl::GLvoid * data);
    void image2D(gl::GLint level, gl::GLenum internalFormat, const glm::ivec2 & size, gl::GLint border, gl::GLenum format, gl::GLenum type, const gl::GLvoid * data);
    void image2D(gl::GLenum target, gl::GLint level, gl::GLenum internalFormat, gl::GLsizei width, gl::GLsizei height, gl::GLint border, gl::GLenum format, gl::GLenum type, const gl::GLvoid * data);
    void image2D(gl::GLenum target, gl::GLint level, gl::GLenum internalFormat, const glm::ivec2 & size, gl::GLint border, gl::GLenum format, gl::GLenum type, const gl::GLvoid * data);
    void compressedImage2D(gl::GLint level, gl::GLenum internalFormat, gl::GLsizei width, gl::GLsizei height, gl::GLint border, gl::GLsizei imageSize, const gl::GLvoid * data);
    void compressedImage2D(gl::GLint level, gl::GLenum internalFormat, const glm::ivec2 & size, gl::GLint border, gl::GLsizei imageSize, const gl::GLvoid * data);
    void subImage2D(gl::GLint level, gl::GLint xOffset, gl::GLint yOffset, gl::GLsizei width, gl::GLsizei height, gl::GLenum format, gl::GLenum type, const gl::GLvoid * data);
    void subImage2D(gl::GLint level, const glm::ivec2& offset, const glm::ivec2& size, gl::GLenum format, gl::GLenum type, const gl::GLvoid * data);

    void image3D(gl::GLint level, gl::GLenum internalFormat, gl::GLsizei width, gl::GLsizei height, gl::GLsizei depth, gl::GLint border, gl::GLenum format, gl::GLenum type, const gl::GLvoid * data);
    void image3D(gl::GLint level, gl::GLenum internalFormat, const glm::ivec3 & size, gl::GLint border, gl::GLenum format, gl::GLenum type, const gl::GLvoid * data);
    void compressedImage3D(gl::GLint level, gl::GLenum internalFormat, gl::GLsizei width, gl::GLsizei height, gl::GLsizei depth, gl::GLint border, gl::GLsizei imageSize, const gl::GLvoid * data);
    void compressedImage3D(gl::GLint level, gl::GLenum internalFormat, const glm::ivec3 & size, gl::GLint border, gl::GLsizei imageSize, const gl::GLvoid * data);
    void subImage3D(gl::GLint level, gl::GLint xOffset, gl::GLint yOffset, gl::GLint zOffset, gl::GLsizei width, gl::GLsizei height, gl::GLsizei depth, gl::GLenum format, gl::GLenum type, const gl::GLvoid * data);
    void subImage3D(gl::GLint level, const glm::ivec3& offset, const glm::ivec3& size, gl::GLenum format, gl::GLenum type, const gl::GLvoid * data);

    void image2DMultisample(gl::GLsizei samples, gl::GLenum internalFormat, gl::GLsizei width, gl::GLsizei height, gl::GLboolean fixedSamplesLocations);
    void image2DMultisample(gl::GLsizei samples, gl::GLenum internalFormat, const glm::ivec2 & size, gl::GLboolean fixedSamplesLocations);
    void image3DMultisample(gl::GLsizei samples, gl::GLenum internalFormat, gl::GLsizei width, gl::GLsizei height, gl::GLsizei depth, gl::GLboolean fixedSamplesLocations);
    void image3DMultisample(gl::GLsizei samples, gl::GLenum internalFormat, const glm::ivec3 & size, gl::GLboolean fixedSamplesLocations);

    void storage1D(gl::GLsizei levels, gl::GLenum internalFormat, gl::GLsizei width);
    void storage2D(gl::GLsizei levels, gl::GLenum internalFormat, gl::GLsizei width, gl::GLsizei height);
    void storage2D(gl::GLsizei levels, gl::GLenum internalFormat, const glm::ivec2 & size);
    void storage3D(gl::GLsizei levels, gl::GLenum internalFormat, gl::GLsizei width, gl::GLsizei height, gl::GLsizei depth);
    void storage3D(gl::GLsizei levels, gl::GLenum internalFormat, const glm::ivec3 & size);

    void textureView(gl::GLuint originalTexture, gl::GLenum internalFormat, gl::GLuint minLevel, gl::GLuint numLevels, gl::GLuint minLayer, gl::GLuint numLayers);

    void texBuffer(gl::GLenum internalFormat, Buffer * buffer);
    void texBuffer(gl::GLenum activeTexture, gl::GLenum internalFormat, Buffer * buffer);
    void texBufferRange(gl::GLenum internalFormat, Buffer * buffer, gl::GLintptr offset, gl::GLsizeiptr size);
    void texBufferRange(gl::GLenum activeTexture, gl::GLenum internalFormat, Buffer * buffer, gl::GLintptr offset, gl::GLsizeiptr size);

    void clearImage(gl::GLint level, gl::GLenum format, gl::GLenum type, const void * data);
    void clearImage(gl::GLint level, gl::GLenum format, gl::GLenum type, const glm::vec4 & value);
    void clearImage(gl::GLint level, gl::GLenum format, gl::GLenum type, const glm::ivec4 & value);
    void clearImage(gl::GLint level, gl::GLenum format, gl::GLenum type, const glm::uvec4 & value);

    void clearSubImage(gl::GLint level, gl::GLint xOffset, gl::GLint yOffset, gl::GLint zOffset, gl::GLsizei width, gl::GLsizei height, gl::GLsizei depth, gl::GLenum format, gl::GLenum type, const void * data);
    void clearSubImage(gl::GLint level, const glm::ivec3 & offset, const glm::ivec3 & size, gl::GLenum format, gl::GLenum type, const void * data);
    void clearSubImage(gl::GLint level, const glm::ivec3 & offset, const glm::ivec3 & size, gl::GLenum format, gl::GLenum type, const glm::vec4 & value);
    void clearSubImage(gl::GLint level, const glm::ivec3 & offset, const glm::ivec3 & size, gl::GLenum format, gl::GLenum type, const glm::ivec4 & value);
    void clearSubImage(gl::GLint level, const glm::ivec3 & offset, const glm::ivec3 & size, gl::GLenum format, gl::GLenum type, const glm::uvec4 & value);

    void bindImageTexture(gl::GLuint unit, gl::GLint level, gl::GLboolean layered, gl::GLint layer, gl::GLenum access, gl::GLenum format) const;
    static void unbindImageTexture(gl::GLuint unit);

    void generateMipmap();

    TextureHandle textureHandle() const;
    bool isResident() const;
    TextureHandle makeResident() const;
    void makeNonResident() const;

    void pageCommitment(gl::GLint level, gl::GLint xOffset, gl::GLint yOffset, gl::GLint zOffset, gl::GLsizei width, gl::GLsizei height, gl::GLsizei depth, gl::GLboolean commit) const;
    void pageCommitment(gl::GLint level, const glm::ivec3& offset, const glm::ivec3& size, gl::GLboolean commit) const;

    virtual gl::GLenum objectType() const override;

protected:
    Texture(IDResource * resource, gl::GLenum target);
    virtual ~Texture();

protected:
    gl::GLenum m_target;
};

} // namespace glo