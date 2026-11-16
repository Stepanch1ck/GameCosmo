#include "boss.h"


Boss::Boss(glm::vec2 pos, glm::vec2 size, Texture2D sprite, glm::vec2 velocity, int difficulty)
    : Enemy(pos, size, sprite, velocity)
{
    this->IsBoss = true;
    this->IsHomePositionSet = false;
    this->Lives = 30 * difficulty;
    this->CurrentState = STATE_ENTERING;
    this->StateTimer = 3.0f;
    this->AttackTimer = 0.5f;
}
void Boss::Update(float dt, unsigned int window_width, glm::vec2 playerPos)
{
    this->StateTimer -= dt;

    // 1. ÂÛËÅÒ ÍÀ ÏÎÇÈÖÈÞ
    if (this->CurrentState == STATE_ENTERING)
    {
        if (!this->IsHomePositionSet)
        {
            this->HomePosition = glm::vec2(window_width - this->Size.x - 50.0f, this->Position.y);
            this->IsHomePositionSet = true;
        }

        if (this->Position.x > this->HomePosition.x)
        {
            this->Position += this->Velocity * dt;
        }
        else
        {
            this->Position = this->HomePosition;
            this->Velocity = glm::vec2(0.0f);
            this->CurrentState = STATE_IDLE;
            this->StateTimer = 2.0f;
        }
    }

    // 2. ÎÆÈÄÀÍÈÅ
    else if (this->CurrentState == STATE_IDLE)
    {
        if (this->StateTimer <= 0.0f)
        {
            if (rand() % 2 == 0)
            {
                this->CurrentState = STATE_ATTACK_SPREAD;
                this->StateTimer = 4.0f;
            }
            else
            {
                this->CurrentState = STATE_ATTACK_CHARGE;
                glm::vec2 direction = glm::normalize(playerPos - this->Position);
                this->Velocity = direction * 600.0f;
                this->StateTimer = 2.0f;
            }
        }
    }

    // 3. ÀÒÀÊÀ ÒÀÐÀÍ
    else if (this->CurrentState == STATE_ATTACK_CHARGE)
    {
        this->Position += this->Velocity * dt;
        if (this->StateTimer <= 0.0f || this->Position.x < -this->Size.x)
        {
            this->CurrentState = STATE_RETREATING;
        }
    }

    // 4. ÎÒÑÒÓÏËÅÍÈÅ
    else if (this->CurrentState == STATE_RETREATING)
    {
        glm::vec2 direction = glm::normalize(this->HomePosition - this->Position);
        float speed = 400.0f;
        this->Velocity = direction * speed;
        this->Position += this->Velocity * dt;

        if (glm::distance(this->Position, this->HomePosition) < 10.0f)
        {
            this->Position = this->HomePosition;
            this->Velocity = glm::vec2(0.0f);
            this->CurrentState = STATE_IDLE;
            this->StateTimer = 2.0f;
        }
    }

    // 5. ÀÒÀÊÀ ÐÀÇÁÐÎÑ ÏÓËÜ
    else if (this->CurrentState == STATE_ATTACK_SPREAD)
    {
        if (this->StateTimer <= 0.0f)
        {
            this->CurrentState = STATE_IDLE;
            this->StateTimer = 2.0f;
        }
    }
}