#include "player.h"

Player::Player() : GameObject(), Lives(3) { }

Player::Player(glm::vec2 pos, glm::vec2 size, Texture2D sprite)
    : GameObject(pos, size, sprite, glm::vec3(1.0f))
{
    IsShieldActive = false;
    ShieldTimer = 0.0f;
    IsSpreadShotActive = false;
    SpreadShotTimer = 0.0f;
    this->Lives = 3;
}

void Player::Shoot(std::vector<Bullet>& bullets)
{
    glm::vec2 bulletPos = this->Position + glm::vec2(this->Size.x, this->Size.y / 2.0f - 5.0f);
    if (this->IsSpreadShotActive)
    {
        float speed = BULLET_VELOCITY;
        bullets.push_back(Bullet(bulletPos, ResourceManager::GetTexture("bullet")));
        
        glm::vec2 upPos = this->Position + glm::vec2(this->Size.x - 20.0f, this->Size.y - 10.0f);
        bullets.push_back(Bullet(upPos, ResourceManager::GetTexture("bullet"), glm::vec2(speed * 0.9f, speed * 0.4f)));

        glm::vec2 downPos = this->Position + glm::vec2(this->Size.x - 20.0f, 0.0f);
        bullets.push_back(Bullet(downPos, ResourceManager::GetTexture("bullet"), glm::vec2(speed * 0.9f, -speed * 0.4f)));
    }
    else
    {
        bullets.push_back(Bullet(bulletPos, ResourceManager::GetTexture("bullet")));
    }
}

void Player::UpdateTimers(float dt)
{
    if (IsShieldActive) {
        ShieldTimer -= dt;
        if (ShieldTimer <= 0.0f) IsShieldActive = false;
    }
    if (IsSpreadShotActive) {
        SpreadShotTimer -= dt;
        if (SpreadShotTimer <= 0.0f) IsSpreadShotActive = false;
    }
}