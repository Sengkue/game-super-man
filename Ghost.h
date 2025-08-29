#pragma once

#include <SFML/Graphics.hpp>

enum class GhostType {
    Regular,
    Fast,
    Boss
};

class Ghost {
public:
    Ghost(sf::Vector2f startPosition, GhostType type = GhostType::Regular);
    ~Ghost();
    
    void update(float deltaTime, sf::Vector2f supermanPosition);
    void render(sf::RenderWindow& window);
    
    // AI Behavior
    void moveTowardsSuperman(sf::Vector2f supermanPosition, float deltaTime);
    void avoidOtherGhosts(float deltaTime);
    void updateFloatingAnimation(float deltaTime);
    
    // Combat
    void takeDamage(float damage);
    bool isDead() const { return health <= 0; }
    float getDamage() const { return damageValue; }
    
    // Position and bounds
    sf::Vector2f getPosition() const { return position; }
    sf::FloatRect getBounds() const;
    void setPosition(sf::Vector2f newPosition) { position = newPosition; }
    
    // Game mechanics
    int getScoreValue() const;
    GhostType getType() const { return type; }
    
private:
    // Core properties
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f basePosition; // For floating animation
    float health;
    float maxHealth;
    GhostType type;
    
    // Movement and AI
    float speed;
    float aggroRange;
    float damageValue;
    
    // Animation
    float floatingTimer;
    float floatingAmplitude;
    float floatingFrequency;
    float animationTimer;
    int currentFrame;
    
    // Visual properties
    sf::Color ghostColor;
    float transparency;
    float size;
    
    // Behavior timers
    float aiUpdateTimer;
    float damageTimer;
    
    // Boss specific
    float bossAttackTimer;
    float bossAttackCooldown;
    
    // Helper methods
    void initializeByType();
    void updateAnimation(float deltaTime);
    void updateBossAttack(float deltaTime, sf::Vector2f supermanPosition);
    void drawGhostBody(sf::RenderWindow& window);
    void drawGhostFace(sf::RenderWindow& window);
    void drawBossEffects(sf::RenderWindow& window);
    
    // Constants
    static const float BASE_SIZE;
    static const float COLLISION_RADIUS;
};