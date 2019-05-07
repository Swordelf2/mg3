#ifndef GRAPHICS_TEXTURE_H
#define GRAPHICS_TEXTURE_H

#include <GL/glew.h>
#include <string>

class Texture
{
public:
    Texture(const std::string &path);
    Texture(const Texture &other) = delete;
    Texture(Texture &&other);
    ~Texture();

    void Bind();
    void Unbind();


private:
    void Load(const std::string &path);

private:
    int m_width;
    int m_height;
    GLuint m_id = 0;

};

#endif