#pragma once

#include "game_object.h"
#include <string>

enum PowerUpType {
    SHIELD,
    HEART,
    SPREAD_SHOT
};

class PowerUp : public GameObject
{

public:
    PowerUpType Type;
    float Duration;
    bool Activated;

    PowerUp(PowerUpType type, float duration, glm::vec2 pos, Texture2D sprite)
        : GameObject(pos, glm::vec2(50.0f, 50.0f), sprite, glm::vec3(1.0f), glm::vec2(-150.0f, 0.0f)),
        Type(type), Duration(duration), Activated(false)
    { }
};