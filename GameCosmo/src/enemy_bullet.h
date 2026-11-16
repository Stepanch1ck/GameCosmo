#pragma once
#include "game_object.h"

class EnemyBullet : public GameObject
{
public:
    EnemyBullet(glm::vec2 pos, Texture2D sprite, glm::vec2 velocity)
        : GameObject(pos, glm::vec2(20.0f, 20.0f), sprite, glm::vec3(1.0f, 0.2f, 0.2f)) // Сделаем их красноватыми
    {
        this->Velocity = velocity;
    }
};