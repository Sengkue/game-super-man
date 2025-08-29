#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <memory>

enum class ParticleType {
    Explosion,
    Laser,
    Punch,
    Damage,
    LevelComplete,
    Smoke,
    Spark
};

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float life;
    float maxLife;
    float size;
    float rotation;
    float rotationSpeed;
    ParticleType type;
    
    Particle(sf::Vector2f pos, sf::Vector2f vel, sf::Color col, float lifetime, float particleSize, ParticleType particleType)
        : position(pos)
        , velocity(vel)
        , color(col)
        , life(lifetime)
        , maxLife(lifetime)
        , size(particleSize)
        , rotation(0)
        , rotationSpeed((rand() % 360 - 180) * 2.0f) // Random rotation speed
        , type(particleType) {}
};

class ParticleSystem {
public:
    ParticleSystem();
    ~ParticleSystem();
    
    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    void clear();
    
    // Effect creation methods
    void addExplosion(sf::Vector2f position, int particleCount = 20);
    void addLaserEffect(sf::Vector2f position, int particleCount = 8);
    void addPunchEffect(sf::Vector2f position, int particleCount = 15);
    void addDamageEffect(sf::Vector2f position, int particleCount = 10);
    void addLevelCompleteEffect(sf::Vector2f position, int particleCount = 50);
    void addSmokeEffect(sf::Vector2f position, int particleCount = 12);
    void addSparkEffect(sf::Vector2f position, sf::Vector2f direction, int particleCount = 6);
    
    // Continuous effects
    void addContinuousEffect(sf::Vector2f position, ParticleType type, float duration);
    
    // System management
    int getParticleCount() const { return static_cast<int>(particles.size()); }
    void setMaxParticles(int maxCount) { maxParticles = maxCount; }
    
private:
    std::vector<std::unique_ptr<Particle>> particles;
    int maxParticles;
    
    // Helper methods
    void addParticle(sf::Vector2f position, sf::Vector2f velocity, sf::Color color, 
                     float lifetime, float size, ParticleType type);
    void updateParticle(Particle& particle, float deltaTime);
    void renderParticle(const Particle& particle, sf::RenderWindow& window);
    
    // Effect-specific helpers
    sf::Vector2f getRandomVelocity(float minSpeed, float maxSpeed);
    sf::Color getRandomColor(sf::Color baseColor, int variance = 50);
    void cleanupDeadParticles();
    
    // Continuous effect data
    struct ContinuousEffect {
        sf::Vector2f position;
        ParticleType type;
        float duration;
        float timer;
        float spawnRate;
        float spawnTimer;
    };
    
    std::vector<ContinuousEffect> continuousEffects;
    void updateContinuousEffects(float deltaTime);
};