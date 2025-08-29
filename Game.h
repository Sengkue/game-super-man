#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <memory>
#include "Superman.h"
#include "Ghost.h"
#include "Projectile.h"
#include "ParticleSystem.h"
#include "ResourceManager.h"

enum class GameState {
    Menu,
    Playing,
    Paused,
    GameOver
};

class Game {
public:
    Game();
    ~Game();
    
    bool initialize();
    void run();
    
private:
    // Core systems
    void handleEvents();
    void update(float deltaTime);
    void render();
    
    // Game state management
    void updateMenu(float deltaTime);
    void updatePlaying(float deltaTime);
    void updatePaused(float deltaTime);
    void updateGameOver(float deltaTime);
    
    void renderMenu();
    void renderPlaying();
    void renderPaused();
    void renderGameOver();
    
    // Game logic
    void startGame();
    void pauseGame();
    void resumeGame();
    void restartGame();
    void gameOver();
    void nextLevel();
    void spawnGhosts();
    
    // Input handling
    void handleMenuInput(sf::Event& event);
    void handlePlayingInput(sf::Event& event);
    void handlePausedInput(sf::Event& event);
    void handleGameOverInput(sf::Event& event);
    
    // Collision detection
    void checkCollisions();
    void checkProjectileCollisions();
    void checkSupermanGhostCollisions();
    
    // UI rendering
    void renderUI();
    void renderHealthBar();
    void renderScore();
    void renderLevel();
    
    // Window and graphics
    sf::RenderWindow window;
    sf::View gameView;
    sf::Clock clock;
    
    // Game state
    GameState currentState;
    int score;
    int level;
    bool isMousePressed;
    
    // Game objects
    std::unique_ptr<Superman> superman;
    std::vector<std::unique_ptr<Ghost>> ghosts;
    std::vector<std::unique_ptr<Projectile>> projectiles;
    std::unique_ptr<ParticleSystem> particleSystem;
    
    // Resources
    ResourceManager resourceManager;
    
    // UI elements
    sf::Font font;
    sf::Text titleText;
    sf::Text scoreText;
    sf::Text levelText;
    sf::Text healthText;
    sf::Text instructionText;
    sf::Text gameOverText;
    sf::Text pausedText;
    
    // Background
    sf::Sprite backgroundSprite;
    
    // Audio
    sf::Music backgroundMusic;
    sf::Sound laserSound;
    sf::Sound punchSound;
    sf::Sound ghostDeathSound;
    
    // Constants
    static const int WINDOW_WIDTH = 800;
    static const int WINDOW_HEIGHT = 600;
    static const float TARGET_FPS;
};