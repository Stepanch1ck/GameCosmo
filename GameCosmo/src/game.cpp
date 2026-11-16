#include "game.h"
#include <iostream>
#include <algorithm>
#include "shader.h"
#include "sprite_renderer.h"
#include <glm/gtc/matrix_transform.hpp>
#include "resource_manager.h"
#include "texture.h"
#include "bullet.h"
#include "player.h"
#include <string>
#include <sstream>
#include "text_renderer.h"

Game::Game(unsigned int width, unsigned int height)
    : State(GameState::GAME_MENU), Width(width), Height(height), Window(nullptr), SpawnCooldown(1.0f), Score(0), ShowHeartBonusText(false), HeartBonusTextTimer(0.0f)

{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    this->Window = glfwCreateWindow(this->Width, this->Height, "CosmoGame", nullptr, nullptr);
    if (this->Window == nullptr)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(this->Window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return;
    }

    glViewport(0, 0, this->Width, this->Height);


    for (int i = 0; i < 1024; ++i)
    {
        this->Keys[i] = false;
        this->KeysProcessed[i] = false;
    }

    glfwSetWindowUserPointer(this->Window, this);
    
    glfwSetKeyCallback(this->Window, key_callback);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

Game::~Game()
{
    this->CleanUp();
}

bool Game::CheckCollision(GameObject& one, GameObject& two)
{
    bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
        two.Position.x + two.Size.x >= one.Position.x;
    bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
        two.Position.y + two.Size.y >= one.Position.y;

    return collisionX && collisionY;
}

void Game::Run()
{
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    this->Init();

    while (!glfwWindowShouldClose(this->Window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwPollEvents();
        this->ProcessInput(deltaTime);

        this->Update(deltaTime);

        this->Render();

        glfwSwapBuffers(this->Window);
    }
}


void Game::Init()
{
    std::cout << "Game Init: Loading resources..." << std::endl;

    this->Text = new TextRenderer(this->Width, this->Height);
    this->Text->Load("fonts/Nasalization.otf", 24);
    ResourceManager::LoadShader("shaders/sprite.vert", "shaders/sprite.frag", "sprite");

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->Width),
        0.0f, static_cast<float>(this->Height), -1.0f, 1.0f);

    ResourceManager::GetShader("sprite").Use();
    ResourceManager::GetShader("sprite").SetInteger("image", 0);
    ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

    this->Renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));
    ResourceManager::LoadTexture("textures/bullet.png", true, "bullet");
    ResourceManager::LoadTexture("textures/player.png", true, "player");
    ResourceManager::LoadTexture("textures/player2.png", true, "player2");
    ResourceManager::LoadTexture("textures/enemy_bullet.png", true, "enemy_bullet");

    ResourceManager::LoadTexture("textures/background1.png", true, "background1");
    ResourceManager::LoadTexture("textures/background2.png", true, "background2");
    ResourceManager::LoadTexture("textures/background3.png", true, "background3");

    ResourceManager::LoadTexture("textures/enemy1.png", true, "enemy1");
    ResourceManager::LoadTexture("textures/enemy2.png", true, "enemy2");
    ResourceManager::LoadTexture("textures/enemy3.png", true, "enemy3");

    ResourceManager::LoadTexture("textures/boss1.png", true, "boss1");
    ResourceManager::LoadTexture("textures/boss2.png", true, "boss2");
    ResourceManager::LoadTexture("textures/boss3.png", true, "boss3");

    ResourceManager::LoadTexture("textures/shield.png", true, "shield_powerup");
    ResourceManager::LoadTexture("textures/heart.png", true, "heart_powerup");
    ResourceManager::LoadTexture("textures/spread_shot.png", true, "spread_shot_powerup");

    Texture2D whiteTexture;
    unsigned char whiteData[] = { 255, 255, 255, 255 };
    whiteTexture.Generate(1, 1, whiteData);
    ResourceManager::Textures["white"] = whiteTexture;

    glm::vec2 playerPos = glm::vec2(
        50.0f,
        this->Height / 2.0f - 50.0f
    );
    glm::vec2 playerSize = glm::vec2(100.0f, 100.0f);
    this->ThePlayer = new Player(playerPos, playerSize, ResourceManager::GetTexture("player"));
    glm::vec2 player2Pos = glm::vec2(50.0f, this->Height / 2.0f - 150.0f);
    this->Player2 = new Player(player2Pos, playerSize, ResourceManager::GetTexture("player2"));
}

void Game::ProcessInput(float dt)
{
    if (this->State == GameState::GAME_MENU)
    {
        if (this->Keys[GLFW_KEY_1] && !this->KeysProcessed[GLFW_KEY_1])
        {
            this->StartLevel();
            this->State = GameState::GAME_LEVEL_SELECT;
            this->KeysProcessed[GLFW_KEY_1] = true;
        }
        if (this->Keys[GLFW_KEY_ESCAPE] && !this->KeysProcessed[GLFW_KEY_ESCAPE])
            glfwSetWindowShouldClose(this->Window, true);
        if (this->Keys[GLFW_KEY_2] && !this->KeysProcessed[GLFW_KEY_2])
        {
            this->TwoPlayersMode = !this->TwoPlayersMode;
            this->KeysProcessed[GLFW_KEY_2] = true;
        }
    }
    // ÂÛÁÎÐ ÓÐÎÂÍß
    else if (this->State == GameState::GAME_LEVEL_SELECT)
    {
        if (this->Keys[GLFW_KEY_1] && !this->KeysProcessed[GLFW_KEY_1]) {
            this->CurrentLevel = 1;
            this->State = GameState::GAME_DIFFICULTY_SELECT;
            this->KeysProcessed[GLFW_KEY_1] = true;
        }
        if (this->Keys[GLFW_KEY_2] && !this->KeysProcessed[GLFW_KEY_2]) {
            this->CurrentLevel = 2;
            this->State = GameState::GAME_DIFFICULTY_SELECT;
            this->KeysProcessed[GLFW_KEY_2] = true;
        }
        if (this->Keys[GLFW_KEY_3] && !this->KeysProcessed[GLFW_KEY_3]) {
            this->CurrentLevel = 3;
            this->State = GameState::GAME_DIFFICULTY_SELECT;
            this->KeysProcessed[GLFW_KEY_3] = true;
        }
        if (this->Keys[GLFW_KEY_ESCAPE] && !this->KeysProcessed[GLFW_KEY_ESCAPE]) {
            this->State = GameState::GAME_MENU;
            this->KeysProcessed[GLFW_KEY_ESCAPE] = true;
        }
    }
    // ÂÛÁÎÐ ÑËÎÆÍÎÑÒÈ
    else if (this->State == GameState::GAME_DIFFICULTY_SELECT)
    {
        if (this->Keys[GLFW_KEY_1] && !this->KeysProcessed[GLFW_KEY_1]) {
            this->Difficulty = 1;
            this->StartLevel();
            this->KeysProcessed[GLFW_KEY_1] = true;
        }
        if (this->Keys[GLFW_KEY_2] && !this->KeysProcessed[GLFW_KEY_2]) {
            this->Difficulty = 2;
            this->StartLevel();
            this->KeysProcessed[GLFW_KEY_2] = true;
        }
        if (this->Keys[GLFW_KEY_3] && !this->KeysProcessed[GLFW_KEY_3]) {
            this->Difficulty = 3;
            this->StartLevel();
            this->KeysProcessed[GLFW_KEY_3] = true;
        }
        if (this->Keys[GLFW_KEY_ESCAPE] && !this->KeysProcessed[GLFW_KEY_ESCAPE]) {
            this->State = GameState::GAME_LEVEL_SELECT;
            this->KeysProcessed[GLFW_KEY_ESCAPE] = true;
        }
    }

    // ÈÄÅÒ ÈÃÐÀ
    if (this->State == GameState::GAME_ACTIVE)
    {
        float velocity = PLAYER_VELOCITY * dt;
        this->ThePlayer->Velocity.y = 0.0f;
        if (!this->ThePlayer->Destroyed)
        {
            if (glfwGetKey(this->Window, GLFW_KEY_W) == GLFW_PRESS)
            {
                if (this->ThePlayer->Position.y <= this->Height - this->ThePlayer->Size.y)
                    this->ThePlayer->Velocity.y = velocity;
                // std::cout << "W pressed" << std::endl;
            }
            if (glfwGetKey(this->Window, GLFW_KEY_S) == GLFW_PRESS)
            {
                if (this->ThePlayer->Position.y >= 0.0f)
                    this->ThePlayer->Velocity.y = -velocity;
                //std::cout << "S pressed" << std::endl;
            }

            if (this->Keys[GLFW_KEY_SPACE] && !this->KeysProcessed[GLFW_KEY_SPACE])
            {
                this->ThePlayer->Shoot(this->Bullets);
                this->KeysProcessed[GLFW_KEY_SPACE] = true;
                //std::cout << "SHOOT! Bullets on screen: " << this->Bullets.size() << std::endl;
            }
        }
        

        if (this->TwoPlayersMode && this->Player2 && !this->Player2->Destroyed)
        {
            this->Player2->Velocity.y = 0.0f;
            if (glfwGetKey(this->Window, GLFW_KEY_UP) == GLFW_PRESS) {
                if (this->Player2->Position.y <= this->Height - this->Player2->Size.y)
                    this->Player2->Velocity.y = velocity;
            }
            if (glfwGetKey(this->Window, GLFW_KEY_DOWN) == GLFW_PRESS) {
                if (this->Player2->Position.y >= 0.0f)
                    this->Player2->Velocity.y = -velocity;
            }
            if (this->Keys[GLFW_KEY_RIGHT_SHIFT] && !this->KeysProcessed[GLFW_KEY_RIGHT_SHIFT]) {
                this->Player2->Shoot(this->Bullets);
                this->KeysProcessed[GLFW_KEY_RIGHT_SHIFT] = true;
            }
        }

        if (this->Keys[GLFW_KEY_ESCAPE] && !this->KeysProcessed[GLFW_KEY_ESCAPE])
        {
            this->State = GameState::GAME_PAUSE;
            this->KeysProcessed[GLFW_KEY_ESCAPE] = true;
            std::cout << "State changed to: PAUSE" << std::endl;
        }
    }
    else if (this->State == GameState::GAME_VICTORY || this->State == GameState::GAME_OVER)
    {
        if (this->Keys[GLFW_KEY_ENTER] && !this->KeysProcessed[GLFW_KEY_ENTER])
        {
            this->State = GameState::GAME_MENU;
            this->KeysProcessed[GLFW_KEY_ENTER] = true;
        }
    }

    // ÏÀÓÇÀ
    if (this->State == GameState::GAME_PAUSE)
    {
        if (this->Keys[GLFW_KEY_1] && !this->KeysProcessed[GLFW_KEY_1])
        {
            this->State = GameState::GAME_ACTIVE;
            this->KeysProcessed[GLFW_KEY_1] = true;
            std::cout << "State changed to: ACTIVE" << std::endl;
        }
        if (this->Keys[GLFW_KEY_2] && !this->KeysProcessed[GLFW_KEY_2])
        {
            this->ResetPlayerAndLevel();
            this->State = GameState::GAME_MENU;
            this->KeysProcessed[GLFW_KEY_2] = true;
            std::cout << "State changed to: MENU" << std::endl;
        }
        if (this->Keys[GLFW_KEY_ESCAPE] && !this->KeysProcessed[GLFW_KEY_ESCAPE])
        {
            this->State = GameState::GAME_ACTIVE;
            this->KeysProcessed[GLFW_KEY_ESCAPE] = true;
            std::cout << "State changed to: ACTIVE" << std::endl;
        }
    }
}

void Game::Update(float dt)
{
    if (this->State == GameState::GAME_ACTIVE)
    {
        this->ThePlayer->UpdateTimers(dt);
        if (this->TwoPlayersMode && this->Player2)
            this->Player2->UpdateTimers(dt);

        float backgroundSpeed = 50.0f;
        backgroundOffset += backgroundSpeed * dt;
        if (backgroundOffset > this->Width)
            backgroundOffset = 0.0f;

        if (!this->ThePlayer->Destroyed)
            this->ThePlayer->Position.y += this->ThePlayer->Velocity.y;
        if (this->TwoPlayersMode && this->Player2 && !this->Player2->Destroyed)
            this->Player2->Position.y += this->Player2->Velocity.y;

        for (int i = 0; i < this->Bullets.size(); ++i)
        {
            this->Bullets[i].Position.x += this->Bullets[i].Velocity.x * dt;
            if (this->Bullets[i].Position.x > this->Width)
            {
                this->Bullets.erase(this->Bullets.begin() + i);
                i--;
            }
        }
        std::vector<Bullet> Bullets;

        //áîíóñû
        this->PowerUpSpawnTimer -= dt;
        if (this->PowerUpSpawnTimer <= 0.0f)
        {
            this->PowerUpSpawnTimer = (15.0f + rand() % 10) / this->Difficulty;
            this->SpawnPowerUp();
        }
        for (int i = 0; i < this->PowerUps.size(); ++i)
        {
            this->PowerUps[i].Position += this->PowerUps[i].Velocity * dt;
            if (this->PowerUps[i].Position.x < -this->PowerUps[i].Size.x) {
                this->PowerUps.erase(this->PowerUps.begin() + i);
                i--;
            }
        }
        for (PowerUp& powerUp : this->PowerUps)
        {
            if (!powerUp.Destroyed && CheckCollision(*this->ThePlayer, powerUp))
            {
                this->ActivatePowerUp(powerUp);
                powerUp.Destroyed = true;
            }
            else if (this->TwoPlayersMode && this->Player2 && CheckCollision(*this->Player2, powerUp))
            {
                this->ActivatePowerUp(powerUp);
                powerUp.Destroyed = true;
            }
        }
        if (this->ShowHeartBonusText)
        {
            this->HeartBonusTextTimer -= dt;
            if (this->HeartBonusTextTimer <= 0.0f)
            {
                this->ShowHeartBonusText = false;
            }
        }


        this->SpawnEnemies(dt);
        for (int i = 0; i < this->Enemies.size(); ++i)
        {
            this->Enemies[i].Position.x += this->Enemies[i].Velocity.x * dt;

            if (this->Enemies[i].Position.x + this->Enemies[i].Size.x < 0.0f)
            {
                this->Enemies.erase(this->Enemies.begin() + i);
                i--;
            }
        }

        for (Enemy& enemy : this->Enemies)
        {
            if (!this->ThePlayer->Destroyed && CheckCollision(*this->ThePlayer, enemy))
            {
                if (!this->ThePlayer->IsShieldActive) {
                    this->ThePlayer->Lives--;
                }
                enemy.Destroyed = true;
            }

            if (this->TwoPlayersMode && this->Player2 && !this->Player2->Destroyed && CheckCollision(*this->Player2, enemy))
            {
                if (!this->Player2->IsShieldActive) {
                    this->Player2->Lives--;
                }
                enemy.Destroyed = true;
            }

            for (Bullet& bullet : this->Bullets)
            {
                if (CheckCollision(bullet, enemy))
                {
                    enemy.Destroyed = true;
                    bullet.Destroyed = true;
                    this->Score += 100;
                }
            }
        }
        this->Bullets.erase(
            std::remove_if(this->Bullets.begin(), this->Bullets.end(),
                [](const Bullet& bullet) { return bullet.Destroyed; }
            ),
            this->Bullets.end()
        );

        this->Enemies.erase(
            std::remove_if(this->Enemies.begin(), this->Enemies.end(),
                [](const Enemy& enemy) { return enemy.Destroyed; }
            ),
            this->Enemies.end()
        );
        this->PowerUps.erase(
            std::remove_if(this->PowerUps.begin(), this->PowerUps.end(),
                [](const PowerUp& powerUp) {return powerUp.Destroyed; }
            ),
            this->PowerUps.end()
        );
        if (this->EnemiesToSpawn <= 0 && this->Enemies.empty() && this->Bosses.empty())
        {
            this->SpawnBoss();
        }

        for (Boss& boss : this->Bosses)
        {
            for (Bullet& bullet : this->Bullets)
            {
                if (CheckCollision(bullet, boss))
                {
                    bullet.Destroyed = true;
                    boss.Lives--;
                    this->Score += 150;
                }
            }

            if (!this->ThePlayer->Destroyed && CheckCollision(*this->ThePlayer, boss))
            {
                if (!this->ThePlayer->IsShieldActive) {
                    this->ThePlayer->Lives--;
                }
                if (boss.CurrentState == STATE_ATTACK_CHARGE) {
                    boss.CurrentState = STATE_RETREATING;
                    boss.StateTimer = 3.0f;
                }
            }

            if (this->TwoPlayersMode && this->Player2 && !this->Player2->Destroyed && CheckCollision(*this->Player2, boss))
            {
                if (!this->Player2->IsShieldActive) {
                    this->Player2->Lives--;
                }
                if (boss.CurrentState == STATE_ATTACK_CHARGE) {
                    boss.CurrentState = STATE_RETREATING;
                    boss.StateTimer = 3.0f;
                }
            }
        }
        for (int i = 0; i < this->Bosses.size(); ++i)
        {
            if (this->Bosses[i].Lives <= 0)
            {
                this->Bosses.erase(this->Bosses.begin() + i);
                i--;
                this->State = GameState::GAME_VICTORY;
            }
        }
        for (Boss& boss : this->Bosses)
        {
            boss.Update(dt, this->Width, this->ThePlayer->Position);
            if (boss.CurrentState == STATE_ATTACK_SPREAD)
            {
                boss.AttackTimer -= dt;
                if (boss.AttackTimer <= 0.0f)
                {
                    boss.AttackTimer = 0.8f / this->Difficulty;

                    glm::vec2 bulletPos = boss.Position + glm::vec2(0.0f, boss.Size.y / 2.0f);
                    Texture2D bulletSprite = ResourceManager::GetTexture("enemy_bullet");

                    glm::vec2 toPlayer = glm::normalize(this->ThePlayer->Position - bulletPos);
                    this->EnemyBullets.push_back(EnemyBullet(bulletPos, bulletSprite, toPlayer * 400.0f));

                    glm::vec2 up(toPlayer.x * 0.866 - toPlayer.y * 0.5, toPlayer.x * 0.5 + toPlayer.y * 0.866);
                    this->EnemyBullets.push_back(EnemyBullet(bulletPos, bulletSprite, up * 400.0f));

                    glm::vec2 down(toPlayer.x * 0.866 + toPlayer.y * 0.5, -toPlayer.x * 0.5 + toPlayer.y * 0.866);
                    this->EnemyBullets.push_back(EnemyBullet(bulletPos, bulletSprite, down * 400.0f));
                }
            }
        }
        for (int i = 0; i < this->EnemyBullets.size(); ++i)
        {
            this->EnemyBullets[i].Position += this->EnemyBullets[i].Velocity * dt;

            if (!this->ThePlayer->Destroyed && CheckCollision(this->EnemyBullets[i], *this->ThePlayer))
            {
                this->EnemyBullets[i].Destroyed = true;
                if (!this->ThePlayer->IsShieldActive) {
                    this->ThePlayer->Lives--;
                }
            }

            if (this->TwoPlayersMode && this->Player2 && !this->Player2->Destroyed && CheckCollision(this->EnemyBullets[i], *this->Player2))
            {
                this->EnemyBullets[i].Destroyed = true;
                if (!this->Player2->IsShieldActive) {
                    this->Player2->Lives--;
                }
            }

            if (this->EnemyBullets[i].Position.x < 0) {
                this->EnemyBullets[i].Destroyed = true;
            }
        }
        if (this->ThePlayer->Lives <= 0)
            this->ThePlayer->Destroyed = true;

        if (this->TwoPlayersMode && this->Player2 && this->Player2->Lives <= 0)
            this->Player2->Destroyed = true;

        if (this->ThePlayer->Destroyed)
        {
            if (this->TwoPlayersMode)
            {
                if (this->Player2 && this->Player2->Destroyed)
                    this->State = GameState::GAME_OVER;
            }
            else {
                this->State = GameState::GAME_OVER;
            }
        }
    
    }
}

void Game::Render()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if (this->State == GameState::GAME_MENU)
    {
        this->Text->RenderText("COSMOGAME", this->Width / 2.0f - 150.0f, this->Height / 2.0f - 100.0f, 2.0f);
        this->Text->RenderText("[1] Start Game", this->Width / 2.0f - 100.0f, this->Height / 2.0f, 1.0f);
        std::string twoPlayersText = "[2] Two Players: ";
        if (this->TwoPlayersMode) {
            this->Text->RenderText(twoPlayersText + "ON", this->Width / 2.0f - 110.0f, this->Height / 2.0f + 30.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        }
        else {
            this->Text->RenderText(twoPlayersText + "OFF", this->Width / 2.0f - 110.0f, this->Height / 2.0f + 30.0f, 1.0f);
        }

        this->Text->RenderText("[ESC] Exit", this->Width / 2.0f - 80.0f, this->Height / 2.0f + 60.0f, 1.0f);
    }
    else if (this->State == GameState::GAME_LEVEL_SELECT)
    {
        this->Text->RenderText("Select Level", this->Width / 2.0f - 120.0f, this->Height / 2.0f - 100.0f, 1.5f);
        this->Text->RenderText("[1] Level 1", this->Width / 2.0f - 80.0f, this->Height / 2.0f, 1.0f);
        this->Text->RenderText("[2] Level 2", this->Width / 2.0f - 80.0f, this->Height / 2.0f + 30.0f, 1.0f);
        this->Text->RenderText("[3] Level 3", this->Width / 2.0f - 80.0f, this->Height / 2.0f + 60.0f, 1.0f);
    }
    else if (this->State == GameState::GAME_DIFFICULTY_SELECT)
    {
        this->Text->RenderText("Select Difficulty", this->Width / 2.0f - 150.0f, this->Height / 2.0f - 100.0f, 1.5f);
        this->Text->RenderText("[1] Easy", this->Width / 2.0f - 60.0f, this->Height / 2.0f, 1.0f);
        this->Text->RenderText("[2] Normal", this->Width / 2.0f - 70.0f, this->Height / 2.0f + 30.0f, 1.0f);
        this->Text->RenderText("[3] Hard", this->Width / 2.0f - 60.0f, this->Height / 2.0f + 60.0f, 1.0f);
    }
    else if (this->State == GameState::GAME_ACTIVE || this->State == GameState::GAME_PAUSE)
    {
        Texture2D bgTexture = ResourceManager::GetTexture("background");

        if (bgTexture.ID != 0)
        {
            glm::vec2 bgSize(this->Width, this->Height);
            this->Renderer->DrawSprite(this->ActiveBackground, glm::vec2(-backgroundOffset, 0.0f), bgSize);
            this->Renderer->DrawSprite(this->ActiveBackground, glm::vec2(this->Width - backgroundOffset, 0.0f), bgSize);
        }
        std::stringstream ss;
        ss << this->Score;
        this->Text->RenderText("Score: " + ss.str(), 5.0f, 5.0f, 1.0f);


        for (PowerUp& powerUp : this->PowerUps)
        {
            if (!powerUp.Destroyed)
            {
                powerUp.Draw(*this->Renderer);
            }
        }
        for (int i = 0; i < this->ThePlayer->Lives; ++i)
        {
            this->Renderer->DrawSprite(ResourceManager::GetTexture("heart_powerup"),
                glm::vec2(this->Width - 50.0f - i * 40.0f, 5.0f), glm::vec2(35.0f, 35.0f));
        }
        if (this->TwoPlayersMode && this->Player2)
        {
            for (int i = 0; i < this->Player2->Lives; ++i)
            {
                this->Renderer->DrawSprite(ResourceManager::GetTexture("heart_powerup"),
                    glm::vec2(this->Width - 50.0f - i * 40.0f, 45.0f), glm::vec2(35.0f, 35.0f));
            }
        }

        if (this->ShowHeartBonusText)
        {
            this->Text->RenderText("+1", this->Width - 55.0f, 45.0f, 1.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        }

        float ui_y_pos = this->Height - 60.0f;

        if (this->ThePlayer->IsShieldActive)
        {
            this->Renderer->DrawSprite(ResourceManager::GetTexture("shield_powerup"),
                glm::vec2(5.0f, ui_y_pos), glm::vec2(50.0f, 50.0f));
            this->Text->RenderText(std::to_string((int)this->ThePlayer->ShieldTimer + 1), 60.0f, ui_y_pos + 15.0f, 1.0f);

            ui_y_pos -= 60.0f;
        }

        if (this->ThePlayer->IsSpreadShotActive)
        {
            this->Renderer->DrawSprite(ResourceManager::GetTexture("spread_shot_powerup"),
                glm::vec2(5.0f, ui_y_pos), glm::vec2(50.0f, 50.0f));
            this->Text->RenderText(std::to_string((int)this->ThePlayer->SpreadShotTimer + 1), 60.0f, ui_y_pos + 15.0f, 1.0f);
        }

        if (!this->ThePlayer->Destroyed)
        {
            this->ThePlayer->Draw(*this->Renderer);
        }

        if (this->TwoPlayersMode && this->Player2 && !this->Player2->Destroyed)
        {
            this->Player2->Draw(*this->Renderer);
        }

        for (Bullet& bullet : this->Bullets)
        {
            bullet.Draw(*this->Renderer);
        }
        for (Enemy& enemy : this->Enemies)
            enemy.Draw(*this->Renderer);
        for (Boss& boss : this->Bosses)
            boss.Draw(*this->Renderer);
        for (EnemyBullet& bullet : this->EnemyBullets)
            bullet.Draw(*this->Renderer);

        if (this->State == GameState::GAME_PAUSE)
        {
            this->Text->RenderText("PAUSED", this->Width / 2.0f - 100.0f, this->Height / 2.0f - 100.0f, 2.0f);
            this->Text->RenderText("[1] Continue", this->Width / 2.0f - 90.0f, this->Height / 2.0f, 1.0f);
            this->Text->RenderText("[2] Exit to Menu", this->Width / 2.0f - 120.0f, this->Height / 2.0f + 30.0f, 1.0f);
        }
    }
    else if (this->State == GameState::GAME_VICTORY)
    {
        this->Text->RenderText("YOU WON!", this->Width / 2.0f - 100.0f, this->Height / 2.0f - 50.0f, 2.0f, glm::vec3(0.0f, 1.0f, 0.0f));
        this->Text->RenderText("Press ENTER to return to menu", this->Width / 2.0f - 220.0f, this->Height / 2.0f, 1.0f);
    }
    else if (this->State == GameState::GAME_OVER)
    {
        this->Text->RenderText("GAME OVER", this->Width / 2.0f - 120.0f, this->Height / 2.0f - 50.0f, 2.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        this->Text->RenderText("Press ENTER to return to menu", this->Width / 2.0f - 220.0f, this->Height / 2.0f, 1.0f);
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Game* game = static_cast<Game*>(glfwGetWindowUserPointer(window));
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            game->Keys[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            game->Keys[key] = false;
            game->KeysProcessed[key] = false;
        }
    }
}
void Game::SpawnEnemies(float dt)
{
    if (this->EnemiesToSpawn > 0 && !this->ActiveEnemyTextures.empty())
    {
        this->SpawnCooldown -= dt;
        if (this->SpawnCooldown <= 0.0f)
        {
            float newCooldown = this->MinSpawnCooldown + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (this->MaxSpawnCooldown - this->MinSpawnCooldown)));
            this->SpawnCooldown = newCooldown / this->Difficulty;

            this->EnemiesToSpawn--;

            Texture2D enemySprite = this->ActiveEnemyTextures[0];
            int yPos = rand() % (this->Height - 50);
            glm::vec2 pos(this->Width, yPos);
            glm::vec2 size(50.0f, 50.0f);
            glm::vec2 velocity(-this->EnemyBaseSpeed * this->Difficulty, 0.0f);

            this->Enemies.push_back(Enemy(pos, size, enemySprite, velocity));
        }
    }
}

void Game::ResetPlayerAndLevel()
{
    this->Enemies.clear();
    this->Bullets.clear();
    this->EnemyBullets.clear();
    this->Bosses.clear();
    this->PowerUps.clear();
    
    this->SpawnCooldown = 1.0f;
    this->PowerUpSpawnTimer = 10.0f;

    glm::vec2 playerPos = glm::vec2(50.0f, this->Height / 2.0f + 50.0f);
    this->ThePlayer->Position = playerPos;
    this->ThePlayer->Lives = 3;
    this->ThePlayer->Destroyed = false;

    if (this->TwoPlayersMode)
    {
        glm::vec2 player2Pos = glm::vec2(50.0f, this->Height / 2.0f - 150.0f);
        if (!this->Player2)
            this->Player2 = new Player(player2Pos, glm::vec2(100.0f, 100.0f), ResourceManager::GetTexture("player2"));

        this->Player2->Position = player2Pos;
        this->Player2->Lives = 3;
        this->Player2->Destroyed = false;
    }

    this->ThePlayer->IsShieldActive = false;
    this->ThePlayer->ShieldTimer = 0.0f;
    this->ThePlayer->IsSpreadShotActive = false;
    this->ThePlayer->SpreadShotTimer = 0.0f;

    this->ShowHeartBonusText = false;
    this->HeartBonusTextTimer = 0.0f;
    this->Score = 0;
}

void Game::StartLevel()
{
    this->ActiveEnemyTextures.clear();

    switch (this->CurrentLevel)
    {
    case 1:
        this->EnemiesToSpawn = 15;
        this->MinSpawnCooldown = 1.2f;
        this->MaxSpawnCooldown = 2.0f;
        this->EnemyBaseSpeed = 200.0f;
        this->ActiveBackground = ResourceManager::GetTexture("background1");
        this->ActiveEnemyTextures.push_back(ResourceManager::GetTexture("enemy1"));
        this->ActiveBossTexture = ResourceManager::GetTexture("boss1");
        break;
    case 2:
        this->EnemiesToSpawn = 25;
        this->MinSpawnCooldown = 0.8f;
        this->MaxSpawnCooldown = 1.5f;
        this->EnemyBaseSpeed = 250.0f;
        this->ActiveBackground = ResourceManager::GetTexture("background2");
        this->ActiveEnemyTextures.push_back(ResourceManager::GetTexture("enemy2"));
        this->ActiveBossTexture = ResourceManager::GetTexture("boss2");
        break;
    case 3:
        this->EnemiesToSpawn = 40;
        this->MinSpawnCooldown = 0.5f;
        this->MaxSpawnCooldown = 1.0f;
        this->EnemyBaseSpeed = 300.0f;
        this->ActiveBackground = ResourceManager::GetTexture("background3");
        this->ActiveEnemyTextures.push_back(ResourceManager::GetTexture("enemy3"));
        this->ActiveBossTexture = ResourceManager::GetTexture("boss3");
        break;
    default:
        this->CurrentLevel = 1;
        this->EnemiesToSpawn = 15;
        this->MinSpawnCooldown = 1.2f;
        this->MaxSpawnCooldown = 2.0f;
        this->EnemyBaseSpeed = 200.0f;
        this->ActiveBackground = ResourceManager::GetTexture("background1");
        this->ActiveEnemyTextures.push_back(ResourceManager::GetTexture("enemy1"));
        this->ActiveBossTexture = ResourceManager::GetTexture("boss1");
        break;
    }

    this->ResetPlayerAndLevel();
    this->State = GameState::GAME_ACTIVE;
}

void Game::SpawnBoss()
{
    glm::vec2 pos(this->Width, this->Height / 2.0f - 150.0f);
    glm::vec2 size(300.0f, 300.0f);
    glm::vec2 velocity(-100.0f, 0.0f);

    this->Bosses.push_back(Boss(pos, size, this->ActiveBossTexture, velocity, this->Difficulty));
}

void Game::SpawnPowerUp()
{
    int randType = rand() % 3;
    int yPos = rand() % (this->Height - 50);
    glm::vec2 pos(this->Width, yPos);

    if (randType == 0)
        this->PowerUps.push_back(PowerUp(SHIELD, 5.0f, pos, ResourceManager::GetTexture("shield_powerup")));
    else if (randType == 1)
        this->PowerUps.push_back(PowerUp(HEART, 0.0f, pos, ResourceManager::GetTexture("heart_powerup")));
    else
        this->PowerUps.push_back(PowerUp(SPREAD_SHOT, 10.0f, pos, ResourceManager::GetTexture("spread_shot_powerup")));
}
void Game::ActivatePowerUp(PowerUp& powerUp)
{
    if (powerUp.Type == SHIELD)
    {
        this->ThePlayer->IsShieldActive = true;
        this->ThePlayer->ShieldTimer = powerUp.Duration;
        if (this->TwoPlayersMode && this->Player2)
        {
            this->Player2->IsShieldActive = true;
            this->Player2->ShieldTimer = powerUp.Duration;
        }
    }
    else if (powerUp.Type == HEART)
    {
        if (this->ThePlayer->Lives < 5)
            this->ThePlayer->Lives++;

        if (this->TwoPlayersMode && this->Player2 && this->Player2->Lives < 5)
            this->Player2->Lives++;

        this->ShowHeartBonusText = true;
        this->HeartBonusTextTimer = 1.5f;
    }
    else if (powerUp.Type == SPREAD_SHOT)
    {
        this->ThePlayer->IsSpreadShotActive = true;
        this->ThePlayer->SpreadShotTimer = powerUp.Duration;
        if (this->TwoPlayersMode && this->Player2)
        {
            this->Player2->IsSpreadShotActive = true;
            this->Player2->SpreadShotTimer = powerUp.Duration;
        }
    }
}



void Game::CleanUp()
{
    delete this->Renderer;
    if (this->ThePlayer)
        delete this->ThePlayer;
    if (this->Player2)
        delete this->Player2;
    delete this->Text;
    std::cout << "Game CleanUp: Releasing resources..." << std::endl;
    glfwTerminate();
}