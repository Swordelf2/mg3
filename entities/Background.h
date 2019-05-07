#ifndef ENTITIES_BACKGROUND_H
#define ENTITIES_BACKGROUND_H

#include "../graphics/Entity.h"

class Background : public Entity {
public:
    Background(Mesh *mesh = nullptr, Shader *shader = nullptr, Texture *texture = nullptr);
    virtual ~Background() = default;

    void Draw() const;
};

#endif