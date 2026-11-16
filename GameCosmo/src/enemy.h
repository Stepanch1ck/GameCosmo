#pragma once

#include "game_object.h"

class Enemy : public GameObject
{
public:
    Enemy(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec2 velocity)
        : GameObject(pos, size, sprite, glm::vec3(1.0f), velocity) { }
};