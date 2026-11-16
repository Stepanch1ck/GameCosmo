#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include "sprite_renderer.h"
#include "player.h"
#include <vector>
#include "bullet.h"
#include "enemy.h"
#include "game_object.h"
#include "text_renderer.h"
#include "boss.h"
#include "enemy_bullet.h"
#include "power_up.h"

enum class GameState {
    GAME_MENU,
    GAME_LEVEL_SELECT,
    GAME_DIFFICULTY_SELECT,
    GAME_ACTIVE,
    GAME_PAUSE,
    GAME_VICTORY,
    GAME_OVER
};

class Game
{
public:
    Player* ThePlayer;
    Player* Player2;
    bool TwoPlayersMode = false;

    std::vector<Bullet> Bullets;
    std::vector<Enemy> Enemies;
    std::vector<Boss> Bosses;
    std::vector<EnemyBullet> EnemyBullets;
    std::vector<PowerUp> PowerUps;

    SpriteRenderer* Renderer;
    TextRenderer* Text;

    int Score;
    int CurrentLevel;
    int Difficulty;
    
    int EnemiesToSpawn;

    bool Keys[1024];
    bool KeysProcessed[1024];
    

    GameState State;
    
    unsigned int Width, Height;
    
    GLFWwindow* Window;
    
    Game(unsigned int width, unsigned int height);
    ~Game();

    void Init();
    void Run();
    bool CheckCollision(GameObject& one, GameObject& two);

private:    
    Texture2D ActiveBackground;
    std::vector<Texture2D> ActiveEnemyTextures;
    Texture2D ActiveBossTexture;
    bool ShowHeartBonusText;

    float backgroundOffset = 0.0f;
    
    float HeartBonusTextTimer;
    float SpawnCooldown;
    float MinSpawnCooldown;
    float MaxSpawnCooldown;
    float EnemyBaseSpeed;
    float PowerUpSpawnTimer;

    void StartLevel();
    void ResetPlayerAndLevel();
    void SpawnBoss();
    void SpawnPowerUp();
    void ActivatePowerUp(PowerUp& powerUp);
    void SpawnEnemies(float dt);
    void ProcessInput(float dt);
    void Update(float dt);
    void Render();
    void CleanUp();
};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);