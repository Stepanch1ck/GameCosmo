#pragma once

#include "game_object.h"

const float BULLET_VELOCITY(800.0f);

class Bullet : public GameObject
{
public:
    
    Bullet(glm::vec2 pos, Texture2D sprite, glm::vec2 velocity = glm::vec2(BULLET_VELOCITY, 0.0f))
        : GameObject(pos, glm::vec2(30.0f, 10.0f), sprite)
    {
        this->Velocity = velocity;
    }
};