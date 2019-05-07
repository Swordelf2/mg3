#include <GL/glew.h>
#include <string>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#pragma GCC diagnostic ignored "-Wtype-limits"
#include "../deps/stb_image.h"
#pragma GCC diagnostic pop

#include "Texture.h"

Texture::Texture(const std::string &path)
{
    Load(path);
}

Texture::Texture(Texture &&other) :
    m_width(other.m_width),
    m_height(other.m_height),
    m_id(other.m_id)
{
    other.m_id = 0;
}

void Texture::Load(const std::string &path)
{
    int channels_num;
    stbi_set_flip_vertically_on_load(1);
    unsigned char *image = stbi_load(path.c_str(), &m_width, &m_height, &channels_num, 4);

    if (!image) {
        std::cerr << "Failed to load image: " << path << std::endl;
    }

    glGenTextures(1, &m_id);
    Bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

    /* Optional ? 
    glGenerateMipmap(GL_TEXTURE_2D);
    */

    stbi_image_free(image);

    Unbind();
}

Texture::~Texture()
{
    if (m_id) {
        glDeleteTextures(1, &m_id);
    }
}

void Texture::Bind()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_id);
}

void Texture::Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}