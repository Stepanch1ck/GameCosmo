#pragma once

#include <vector>
#include "game_object.h"
#include "bullet.h"
#include "resource_manager.h"

const float PLAYER_VELOCITY(500.0f);

class Player : public GameObject
{
public:
    int Lives;
    bool IsShieldActive;
    float ShieldTimer;
    bool IsSpreadShotActive;
    float SpreadShotTimer;
    Player* ThePlayer;
    Player* Player2;
    bool TwoPlayersMode = false;

    Player();
    Player(glm::vec2 pos, glm::vec2 size, Texture2D sprite);
    void Shoot(std::vector<Bullet>& bullets);
    void UpdateTimers(float dt);
};
