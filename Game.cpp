#include "Game.h"
#include <iostream>
#include <random>
#include <algorithm>

const float Game::TARGET_FPS = 60.0f;

Game::Game() 
    : window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Superman vs Ghost", sf::Style::Titlebar | sf::Style::Close)
    , currentState(GameState::Menu)
    , score(0)
    , level(1)
    , isMousePressed(false) {
    
    window.setFramerateLimit(static_cast<unsigned int>(TARGET_FPS));
}

Game::~Game() = default;

bool Game::initialize() {
    // Initialize resource manager
    if (!resourceManager.initialize()) {
        std::cerr << "Failed to initialize resource manager!" << std::endl;
        return false;
    }
    
    // Load font
    if (!font.loadFromFile("assets/fonts/arial.ttf")) {
        // Try to use default font if custom font fails
        std::cout << "Warning: Could not load custom font, using default font" << std::endl;
    }
    
    // Initialize game objects
    superman = std::make_unique<Superman>(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
    particleSystem = std::make_unique<ParticleSystem>();
    
    // Initialize UI text
    titleText.setFont(font);
    titleText.setString("SUPERMAN VS GHOST");
    titleText.setCharacterSize(48);
    titleText.setFillColor(sf::Color::Yellow);
    titleText.setStyle(sf::Text::Bold);
    
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(10, 10);
    
    levelText.setFont(font);
    levelText.setCharacterSize(24);
    levelText.setFillColor(sf::Color::White);
    levelText.setPosition(10, 40);
    
    healthText.setFont(font);
    healthText.setCharacterSize(24);
    healthText.setFillColor(sf::Color::White);
    healthText.setPosition(10, 70);
    
    instructionText.setFont(font);
    instructionText.setString("WASD: Move  SPACE: Laser  CLICK: Punch  ESC: Pause\nPress ENTER to start!");
    instructionText.setCharacterSize(20);
    instructionText.setFillColor(sf::Color::Cyan);
    
    gameOverText.setFont(font);
    gameOverText.setString("GAME OVER\nPress ENTER to restart");
    gameOverText.setCharacterSize(36);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setStyle(sf::Text::Bold);
    
    pausedText.setFont(font);
    pausedText.setString("PAUSED\nPress ESC to resume");
    pausedText.setCharacterSize(36);
    pausedText.setFillColor(sf::Color::Yellow);
    pausedText.setStyle(sf::Text::Bold);
    
    // Center text elements
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setPosition((WINDOW_WIDTH - titleBounds.width) / 2, 150);
    
    sf::FloatRect instructionBounds = instructionText.getLocalBounds();
    instructionText.setPosition((WINDOW_WIDTH - instructionBounds.width) / 2, 300);
    
    sf::FloatRect gameOverBounds = gameOverText.getLocalBounds();
    gameOverText.setPosition((WINDOW_WIDTH - gameOverBounds.width) / 2, WINDOW_HEIGHT / 2 - 50);
    
    sf::FloatRect pausedBounds = pausedText.getLocalBounds();
    pausedText.setPosition((WINDOW_WIDTH - pausedBounds.width) / 2, WINDOW_HEIGHT / 2 - 50);
    
    // Load background
    sf::Texture* backgroundTexture = resourceManager.getTexture("background");
    if (backgroundTexture) {
        backgroundSprite.setTexture(*backgroundTexture);
        // Scale background to fit window
        sf::Vector2u textureSize = backgroundTexture->getSize();
        float scaleX = static_cast<float>(WINDOW_WIDTH) / textureSize.x;
        float scaleY = static_cast<float>(WINDOW_HEIGHT) / textureSize.y;
        backgroundSprite.setScale(scaleX, scaleY);
    }
    
    // Initialize audio
    if (!backgroundMusic.openFromFile("assets/sounds/background_music.ogg")) {
        std::cout << "Warning: Could not load background music" << std::endl;
    } else {
        backgroundMusic.setLoop(true);
        backgroundMusic.setVolume(30);
    }
    
    // Load sound effects
    sf::SoundBuffer* laserBuffer = resourceManager.getSoundBuffer("laser");
    sf::SoundBuffer* punchBuffer = resourceManager.getSoundBuffer("punch");
    sf::SoundBuffer* ghostDeathBuffer = resourceManager.getSoundBuffer("ghost_death");
    
    if (laserBuffer) laserSound.setBuffer(*laserBuffer);
    if (punchBuffer) punchSound.setBuffer(*punchBuffer);
    if (ghostDeathBuffer) ghostDeathSound.setBuffer(*ghostDeathBuffer);
    
    return true;
}

void Game::run() {
    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        
        handleEvents();
        update(deltaTime);
        render();
    }
}

void Game::handleEvents() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            if (currentState == GameState::Playing) {
                pauseGame();
            } else if (currentState == GameState::Paused) {
                resumeGame();
            }
        }
        
        switch (currentState) {
            case GameState::Menu:
                handleMenuInput(event);
                break;
            case GameState::Playing:
                handlePlayingInput(event);
                break;
            case GameState::Paused:
                handlePausedInput(event);
                break;
            case GameState::GameOver:
                handleGameOverInput(event);
                break;
        }
    }
}

void Game::update(float deltaTime) {
    switch (currentState) {
        case GameState::Menu:
            updateMenu(deltaTime);
            break;
        case GameState::Playing:
            updatePlaying(deltaTime);
            break;
        case GameState::Paused:
            updatePaused(deltaTime);
            break;
        case GameState::GameOver:
            updateGameOver(deltaTime);
            break;
    }
}

void Game::render() {
    window.clear(sf::Color::Black);
    
    // Draw background
    if (backgroundSprite.getTexture()) {
        window.draw(backgroundSprite);
    }
    
    switch (currentState) {
        case GameState::Menu:
            renderMenu();
            break;
        case GameState::Playing:
            renderPlaying();
            break;
        case GameState::Paused:
            renderPlaying(); // Show game behind pause menu
            renderPaused();
            break;
        case GameState::GameOver:
            renderPlaying(); // Show game behind game over screen
            renderGameOver();
            break;
    }
    
    window.display();
}

void Game::updateMenu(float deltaTime) {
    // Menu doesn't need updates, just wait for input
}

void Game::updatePlaying(float deltaTime) {
    // Update Superman
    superman->update(deltaTime, window);
    
    // Update ghosts
    for (auto& ghost : ghosts) {
        ghost->update(deltaTime, superman->getPosition());
    }
    
    // Update projectiles
    for (auto it = projectiles.begin(); it != projectiles.end();) {
        (*it)->update(deltaTime);
        if (!(*it)->isActive() || (*it)->isOffScreen(WINDOW_WIDTH, WINDOW_HEIGHT)) {
            it = projectiles.erase(it);
        } else {
            ++it;
        }
    }
    
    // Update particle system
    particleSystem->update(deltaTime);
    
    // Check collisions
    checkCollisions();
    
    // Check if level completed
    if (ghosts.empty()) {
        nextLevel();
    }
    
    // Check game over
    if (superman->getHealth() <= 0) {
        gameOver();
    }
    
    // Health regeneration
    superman->regenerateHealth(deltaTime);
}

void Game::updatePaused(float deltaTime) {
    // Game is paused, no updates needed
}

void Game::updateGameOver(float deltaTime) {
    // Game over, no updates needed
}

void Game::renderMenu() {
    window.draw(titleText);
    window.draw(instructionText);
}

void Game::renderPlaying() {
    // Draw Superman
    superman->render(window);
    
    // Draw ghosts
    for (const auto& ghost : ghosts) {
        ghost->render(window);
    }
    
    // Draw projectiles
    for (const auto& projectile : projectiles) {
        projectile->render(window);
    }
    
    // Draw particles
    particleSystem->render(window);
    
    // Draw UI
    renderUI();
}

void Game::renderPaused() {
    // Draw semi-transparent overlay
    sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    overlay.setFillColor(sf::Color(0, 0, 0, 128));
    window.draw(overlay);
    
    window.draw(pausedText);
}

void Game::renderGameOver() {
    // Draw semi-transparent overlay
    sf::RectangleShape overlay(sf::Vector2f(WINDOW_WIDTH, WINDOW_HEIGHT));
    overlay.setFillColor(sf::Color(0, 0, 0, 128));
    window.draw(overlay);
    
    window.draw(gameOverText);
}

void Game::renderUI() {
    scoreText.setString("Score: " + std::to_string(score));
    levelText.setString("Level: " + std::to_string(level));
    healthText.setString("Health: " + std::to_string(static_cast<int>(superman->getHealth())) + "/100");
    
    window.draw(scoreText);
    window.draw(levelText);
    window.draw(healthText);
    
    renderHealthBar();
}

void Game::renderHealthBar() {
    float healthPercent = superman->getHealth() / 100.0f;
    
    // Background bar
    sf::RectangleShape healthBarBg(sf::Vector2f(200, 20));
    healthBarBg.setPosition(WINDOW_WIDTH - 220, 10);
    healthBarBg.setFillColor(sf::Color::Red);
    healthBarBg.setOutlineThickness(2);
    healthBarBg.setOutlineColor(sf::Color::White);
    
    // Health bar
    sf::RectangleShape healthBar(sf::Vector2f(200 * healthPercent, 20));
    healthBar.setPosition(WINDOW_WIDTH - 220, 10);
    healthBar.setFillColor(sf::Color::Green);
    
    window.draw(healthBarBg);
    window.draw(healthBar);
}

void Game::handleMenuInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
        startGame();
    }
}

void Game::handlePlayingInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
        // Fire laser
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        auto laser = std::make_unique<Projectile>(
            superman->getPosition(), 
            sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)),
            ProjectileType::Laser
        );
        projectiles.push_back(std::move(laser));
        
        laserSound.play();
        
        // Add laser particles
        particleSystem->addLaserEffect(superman->getPosition());
    }
    
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        // Super punch
        sf::Vector2f mousePos(static_cast<float>(event.mouseButton.x), static_cast<float>(event.mouseButton.y));
        superman->performPunch(mousePos);
        
        punchSound.play();
        
        // Check punch collision with ghosts
        for (auto it = ghosts.begin(); it != ghosts.end();) {
            if (superman->isPunching() && superman->getPunchRange().intersects((*it)->getBounds())) {
                // Add explosion effect
                particleSystem->addExplosion((*it)->getPosition());
                
                // Add score based on ghost type
                score += (*it)->getScoreValue();
                
                ghostDeathSound.play();
                
                it = ghosts.erase(it);
            } else {
                ++it;
            }
        }
        
        // Add punch particles
        particleSystem->addPunchEffect(mousePos);
    }
}

void Game::handlePausedInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
        restartGame();
    }
}

void Game::handleGameOverInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
        restartGame();
    }
}

void Game::startGame() {
    currentState = GameState::Playing;
    score = 0;
    level = 1;
    superman->reset(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
    ghosts.clear();
    projectiles.clear();
    particleSystem->clear();
    spawnGhosts();
    
    backgroundMusic.play();
}

void Game::pauseGame() {
    currentState = GameState::Paused;
    backgroundMusic.pause();
}

void Game::resumeGame() {
    currentState = GameState::Playing;
    backgroundMusic.play();
}

void Game::restartGame() {
    startGame();
}

void Game::gameOver() {
    currentState = GameState::GameOver;
    backgroundMusic.stop();
}

void Game::nextLevel() {
    level++;
    spawnGhosts();
    
    // Add level completion particles
    particleSystem->addLevelCompleteEffect(sf::Vector2f(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2));
}

void Game::spawnGhosts() {
    ghosts.clear();
    
    int ghostCount = 3 + (level - 1) * 2; // 3, 5, 7, 9, etc.
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> xDist(50, WINDOW_WIDTH - 50);
    std::uniform_real_distribution<float> yDist(50, WINDOW_HEIGHT - 50);
    std::uniform_real_distribution<float> typeDist(0.0f, 1.0f);
    
    for (int i = 0; i < ghostCount; ++i) {
        float x, y;
        // Ensure ghosts don't spawn too close to Superman
        do {
            x = xDist(gen);
            y = yDist(gen);
        } while (std::abs(x - superman->getPosition().x) < 100 && 
                 std::abs(y - superman->getPosition().y) < 100);
        
        // Determine ghost type
        GhostType type = GhostType::Regular;
        float typeRoll = typeDist(gen);
        
        if (level % 3 == 0 && i == ghostCount - 1) {
            type = GhostType::Boss; // Boss ghost every 3rd level
        } else if (typeRoll < 0.3f) {
            type = GhostType::Fast;
        }
        
        ghosts.push_back(std::make_unique<Ghost>(sf::Vector2f(x, y), type));
    }
}

void Game::checkCollisions() {
    checkProjectileCollisions();
    checkSupermanGhostCollisions();
}

void Game::checkProjectileCollisions() {
    for (auto projIt = projectiles.begin(); projIt != projectiles.end();) {
        bool projDestroyed = false;
        
        for (auto ghostIt = ghosts.begin(); ghostIt != ghosts.end();) {
            if ((*projIt)->getBounds().intersects((*ghostIt)->getBounds())) {
                // Projectile hit ghost
                particleSystem->addExplosion((*ghostIt)->getPosition());
                
                score += (*ghostIt)->getScoreValue();
                ghostDeathSound.play();
                
                ghostIt = ghosts.erase(ghostIt);
                projDestroyed = true;
                break;
            } else {
                ++ghostIt;
            }
        }
        
        if (projDestroyed) {
            projIt = projectiles.erase(projIt);
        } else {
            ++projIt;
        }
    }
}

void Game::checkSupermanGhostCollisions() {
    sf::FloatRect supermanBounds = superman->getBounds();
    
    for (const auto& ghost : ghosts) {
        if (supermanBounds.intersects(ghost->getBounds())) {
            superman->takeDamage(ghost->getDamage());
            
            // Add damage effect
            particleSystem->addDamageEffect(superman->getPosition());
        }
    }
}