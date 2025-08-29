#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

class Superman {
public:
    Superman(float x, float y);
    ~Superman();
    
    void update(float deltaTime, const sf::RenderWindow& window);
    void render(sf::RenderWindow& window);
    
    // Movement and controls
    void handleInput(float deltaTime);
    void move(sf::Vector2f direction, float deltaTime);
    
    // Combat
    void performPunch(sf::Vector2f targetPosition);
    bool isPunching() const { return punchTimer > 0; }
    sf::FloatRect getPunchRange() const;
    
    // Health system
    void takeDamage(float damage);
    void regenerateHealth(float deltaTime);
    float getHealth() const { return health; }
    
    // Position and bounds
    sf::Vector2f getPosition() const { return position; }
    sf::FloatRect getBounds() const;
    void setPosition(sf::Vector2f newPosition) { position = newPosition; }
    
    // Game state
    void reset(float x, float y);
    
private:
    // Core properties
    sf::Vector2f position;
    sf::Vector2f velocity;
    float health;
    float maxHealth;
    
    // Movement
    float speed;
    float acceleration;
    float friction;
    
    // Combat
    float punchTimer;
    float punchDuration;
    float punchRange;
    sf::Vector2f punchTarget;
    
    // Animation
    sf::Sprite sprite;
    sf::Texture* texture;
    float animationTimer;
    int currentFrame;
    int maxFrames;
    
    // Cape animation
    float capeAnimationTimer;
    sf::RectangleShape cape;
    
    // Health regeneration
    float regenTimer;
    float regenRate;
    float regenAmount;
    
    // Constants
    static const float MAX_SPEED;
    static const float PUNCH_DAMAGE;
    static const float SIZE;
    
    // Helper methods
    void updateAnimation(float deltaTime);
    void updateCape(float deltaTime);
    void constrainToWindow(const sf::RenderWindow& window);
    void drawHealthBar(sf::RenderWindow& window);
};