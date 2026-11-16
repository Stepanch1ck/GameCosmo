// GameCosmo/src/boss.h
#pragma once

#include "enemy.h"

enum BossState {
    STATE_ENTERING,
    STATE_IDLE,
    STATE_ATTACK_SPREAD,
    STATE_ATTACK_CHARGE,
    STATE_RETREATING
};

class Boss : public Enemy
{
public:
    BossState CurrentState;
    float StateTimer;
    float AttackTimer;
    glm::vec2 HomePosition;
    bool IsHomePositionSet;

    Boss(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec2 velocity, int difficulty);

    void Update(float dt, unsigned int window_width, glm::vec2 playerPos);
};
