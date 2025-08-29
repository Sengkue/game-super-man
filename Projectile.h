#pragma once

#include <SFML/Graphics.hpp>

enum class ProjectileType {
    Laser,
    SuperPunch,
    BossAttack
};

class Projectile {
public:
    Projectile(sf::Vector2f startPosition, sf::Vector2f targetPosition, ProjectileType type = ProjectileType::Laser);
    ~Projectile();
    
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    
    // State management
    bool isActive() const { return active; }
    void deactivate() { active = false; }
    bool isOffScreen(int windowWidth, int windowHeight) const;
    
    // Position and collision
    sf::Vector2f getPosition() const { return position; }
    sf::FloatRect getBounds() const;
    
    // Projectile properties
    ProjectileType getType() const { return type; }
    float getDamage() const { return damage; }
    
private:
    // Core properties
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f direction;
    ProjectileType type;
    
    // Physical properties
    float speed;
    float damage;
    float size;
    bool active;
    
    // Visual properties
    sf::Color color;
    float lifetime;
    float maxLifetime;
    
    // Animation
    float animationTimer;
    float pulseFrequency;
    
    // Trail effect
    struct TrailPoint {
        sf::Vector2f position;
        float alpha;
        float age;
    };
    std::vector<TrailPoint> trail;
    int maxTrailPoints;
    
    // Helper methods
    void initializeByType();
    void updateTrail(float deltaTime);
    void drawLaser(sf::RenderWindow& window);
    void drawTrail(sf::RenderWindow& window);
    void calculateDirection(sf::Vector2f startPos, sf::Vector2f targetPos);
    
    // Constants
    static const float LASER_SPEED;
    static const float LASER_DAMAGE;
    static const float LASER_SIZE;
};