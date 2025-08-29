#include "ParticleSystem.h"
#include <cmath>
#include <random>
#include <algorithm>

ParticleSystem::ParticleSystem()
    : maxParticles(1000) {
    particles.reserve(maxParticles);
}

ParticleSystem::~ParticleSystem() = default;

void ParticleSystem::update(float deltaTime) {
    // Update all particles
    for (auto& particle : particles) {
        if (particle) {
            updateParticle(*particle, deltaTime);
        }
    }
    
    // Update continuous effects
    updateContinuousEffects(deltaTime);
    
    // Remove dead particles
    cleanupDeadParticles();
}

void ParticleSystem::render(sf::RenderWindow& window) {
    for (const auto& particle : particles) {
        if (particle && particle->life > 0) {
            renderParticle(*particle, window);
        }
    }
}

void ParticleSystem::clear() {
    particles.clear();
    continuousEffects.clear();
}

void ParticleSystem::addExplosion(sf::Vector2f position, int particleCount) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angleDist(0, 2 * 3.14159f);
    std::uniform_real_distribution<float> speedDist(50, 200);
    std::uniform_real_distribution<float> sizeDist(2, 8);
    std::uniform_real_distribution<float> lifeDist(0.5f, 2.0f);
    
    for (int i = 0; i < particleCount; ++i) {
        float angle = angleDist(gen);
        float speed = speedDist(gen);
        sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed);
        
        // Mix of orange, red, and yellow for explosion
        sf::Color colors[] = {
            sf::Color::Red,
            sf::Color::Yellow,
            sf::Color(255, 165, 0), // Orange
            sf::Color::White
        };
        sf::Color color = colors[i % 4];
        
        addParticle(position, velocity, color, lifeDist(gen), sizeDist(gen), ParticleType::Explosion);
    }
}

void ParticleSystem::addLaserEffect(sf::Vector2f position, int particleCount) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angleDist(0, 2 * 3.14159f);
    std::uniform_real_distribution<float> speedDist(20, 80);
    std::uniform_real_distribution<float> sizeDist(1, 4);
    
    for (int i = 0; i < particleCount; ++i) {
        float angle = angleDist(gen);
        float speed = speedDist(gen);
        sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed);
        
        sf::Color color = sf::Color::Red;
        color.r = 255;
        color.g = static_cast<sf::Uint8>(50 + rand() % 100);
        color.b = 0;
        
        addParticle(position, velocity, color, 0.8f, sizeDist(gen), ParticleType::Laser);
    }
}

void ParticleSystem::addPunchEffect(sf::Vector2f position, int particleCount) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angleDist(0, 2 * 3.14159f);
    std::uniform_real_distribution<float> speedDist(100, 300);
    std::uniform_real_distribution<float> sizeDist(3, 10);
    
    for (int i = 0; i < particleCount; ++i) {
        float angle = angleDist(gen);
        float speed = speedDist(gen);
        sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed);
        
        sf::Color color = sf::Color::Yellow;
        color.g = 255;
        color.r = static_cast<sf::Uint8>(200 + rand() % 55);
        color.b = 0;
        
        addParticle(position, velocity, color, 1.2f, sizeDist(gen), ParticleType::Punch);
    }
}

void ParticleSystem::addDamageEffect(sf::Vector2f position, int particleCount) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angleDist(-3.14159f/4, 3.14159f/4); // Upward spray
    std::uniform_real_distribution<float> speedDist(30, 120);
    std::uniform_real_distribution<float> sizeDist(2, 6);
    
    for (int i = 0; i < particleCount; ++i) {
        float angle = angleDist(gen) - 3.14159f/2; // Bias upward
        float speed = speedDist(gen);
        sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed);
        
        sf::Color color = sf::Color::Red;
        color.a = static_cast<sf::Uint8>(150 + rand() % 105);
        
        addParticle(position, velocity, color, 1.5f, sizeDist(gen), ParticleType::Damage);
    }
}

void ParticleSystem::addLevelCompleteEffect(sf::Vector2f position, int particleCount) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angleDist(0, 2 * 3.14159f);
    std::uniform_real_distribution<float> speedDist(80, 250);
    std::uniform_real_distribution<float> sizeDist(4, 12);
    std::uniform_real_distribution<float> lifeDist(2.0f, 4.0f);
    
    for (int i = 0; i < particleCount; ++i) {
        float angle = angleDist(gen);
        float speed = speedDist(gen);
        sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed);
        
        // Festive colors
        sf::Color colors[] = {
            sf::Color::Yellow,
            sf::Color::Cyan,
            sf::Color::Magenta,
            sf::Color::Green,
            sf::Color::White
        };
        sf::Color color = colors[i % 5];
        
        addParticle(position, velocity, color, lifeDist(gen), sizeDist(gen), ParticleType::LevelComplete);
    }
}

void ParticleSystem::addSmokeEffect(sf::Vector2f position, int particleCount) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angleDist(-3.14159f/6, 3.14159f/6); // Mostly upward
    std::uniform_real_distribution<float> speedDist(10, 50);
    std::uniform_real_distribution<float> sizeDist(5, 15);
    
    for (int i = 0; i < particleCount; ++i) {
        float angle = angleDist(gen) - 3.14159f/2; // Upward
        float speed = speedDist(gen);
        sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed);
        
        sf::Color color(128, 128, 128, 180); // Gray smoke
        
        addParticle(position, velocity, color, 3.0f, sizeDist(gen), ParticleType::Smoke);
    }
}

void ParticleSystem::addSparkEffect(sf::Vector2f position, sf::Vector2f direction, int particleCount) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angleDist(-0.5f, 0.5f); // Small spread around direction
    std::uniform_real_distribution<float> speedDist(100, 300);
    std::uniform_real_distribution<float> sizeDist(1, 3);
    
    float baseAngle = std::atan2(direction.y, direction.x);
    
    for (int i = 0; i < particleCount; ++i) {
        float angle = baseAngle + angleDist(gen);
        float speed = speedDist(gen);
        sf::Vector2f velocity(std::cos(angle) * speed, std::sin(angle) * speed);
        
        sf::Color color = sf::Color::White;
        color.r = 255;
        color.g = static_cast<sf::Uint8>(200 + rand() % 55);
        color.b = 0;
        
        addParticle(position, velocity, color, 0.5f, sizeDist(gen), ParticleType::Spark);
    }
}

void ParticleSystem::addContinuousEffect(sf::Vector2f position, ParticleType type, float duration) {
    ContinuousEffect effect;
    effect.position = position;
    effect.type = type;
    effect.duration = duration;
    effect.timer = 0;
    effect.spawnRate = 0.1f; // 10 particles per second
    effect.spawnTimer = 0;
    
    continuousEffects.push_back(effect);
}

void ParticleSystem::addParticle(sf::Vector2f position, sf::Vector2f velocity, sf::Color color, 
                                 float lifetime, float size, ParticleType type) {
    if (particles.size() < static_cast<size_t>(maxParticles)) {
        particles.push_back(std::make_unique<Particle>(position, velocity, color, lifetime, size, type));
    }
}

void ParticleSystem::updateParticle(Particle& particle, float deltaTime) {
    // Update life
    particle.life -= deltaTime;
    
    if (particle.life <= 0) return;
    
    // Update position
    particle.position += particle.velocity * deltaTime;
    
    // Update rotation
    particle.rotation += particle.rotationSpeed * deltaTime;
    
    // Apply gravity and air resistance based on particle type
    switch (particle.type) {
        case ParticleType::Explosion:
        case ParticleType::Punch:
        case ParticleType::Damage:
            particle.velocity.y += 150.0f * deltaTime; // Gravity
            particle.velocity *= 0.98f; // Air resistance
            break;
            
        case ParticleType::Laser:
        case ParticleType::Spark:
            particle.velocity *= 0.95f; // Light air resistance
            break;
            
        case ParticleType::Smoke:
            particle.velocity.y -= 20.0f * deltaTime; // Buoyancy
            particle.velocity *= 0.99f; // Slow drift
            break;
            
        case ParticleType::LevelComplete:
            particle.velocity.y += 80.0f * deltaTime; // Light gravity
            particle.velocity *= 0.99f;
            break;
    }
    
    // Update alpha based on remaining life
    float lifePercent = particle.life / particle.maxLife;
    sf::Uint8 alpha = static_cast<sf::Uint8>(255 * lifePercent);
    particle.color.a = alpha;
    
    // Scale down over time for some particle types
    if (particle.type == ParticleType::Smoke || particle.type == ParticleType::Explosion) {
        particle.size *= 1.01f; // Grow slightly
    }
}

void ParticleSystem::renderParticle(const Particle& particle, sf::RenderWindow& window) {
    if (particle.life <= 0) return;
    
    switch (particle.type) {
        case ParticleType::Explosion:
        case ParticleType::Punch:
        case ParticleType::Damage:
        case ParticleType::LevelComplete: {
            sf::CircleShape shape(particle.size);
            shape.setFillColor(particle.color);
            shape.setOrigin(particle.size, particle.size);
            shape.setPosition(particle.position);
            window.draw(shape);
            break;
        }
        
        case ParticleType::Laser:
        case ParticleType::Spark: {
            // Draw as small rectangles for laser/spark effects
            sf::RectangleShape shape(sf::Vector2f(particle.size * 2, particle.size));
            shape.setFillColor(particle.color);
            shape.setOrigin(particle.size, particle.size * 0.5f);
            shape.setPosition(particle.position);
            shape.setRotation(particle.rotation);
            window.draw(shape);
            break;
        }
        
        case ParticleType::Smoke: {
            // Draw smoke as semi-transparent circles that grow
            sf::CircleShape shape(particle.size);
            shape.setFillColor(particle.color);
            shape.setOrigin(particle.size, particle.size);
            shape.setPosition(particle.position);
            window.draw(shape);
            break;
        }
    }
}

void ParticleSystem::cleanupDeadParticles() {
    particles.erase(
        std::remove_if(particles.begin(), particles.end(),
            [](const std::unique_ptr<Particle>& particle) {
                return !particle || particle->life <= 0;
            }),
        particles.end()
    );
}

void ParticleSystem::updateContinuousEffects(float deltaTime) {
    for (auto it = continuousEffects.begin(); it != continuousEffects.end();) {
        it->timer += deltaTime;
        it->spawnTimer += deltaTime;
        
        // Spawn particles at regular intervals
        if (it->spawnTimer >= it->spawnRate) {
            switch (it->type) {
                case ParticleType::Smoke:
                    addSmokeEffect(it->position, 2);
                    break;
                case ParticleType::Spark:
                    addSparkEffect(it->position, sf::Vector2f(0, -1), 1);
                    break;
                // Add other continuous effects as needed
                default:
                    break;
            }
            it->spawnTimer = 0;
        }
        
        // Remove expired effects
        if (it->timer >= it->duration) {
            it = continuousEffects.erase(it);
        } else {
            ++it;
        }
    }
}

sf::Vector2f ParticleSystem::getRandomVelocity(float minSpeed, float maxSpeed) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> angleDist(0, 2 * 3.14159f);
    std::uniform_real_distribution<float> speedDist(minSpeed, maxSpeed);
    
    float angle = angleDist(gen);
    float speed = speedDist(gen);
    
    return sf::Vector2f(std::cos(angle) * speed, std::sin(angle) * speed);
}

sf::Color ParticleSystem::getRandomColor(sf::Color baseColor, int variance) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> colorDist(-variance, variance);
    
    int r = std::max(0, std::min(255, static_cast<int>(baseColor.r) + colorDist(gen)));
    int g = std::max(0, std::min(255, static_cast<int>(baseColor.g) + colorDist(gen)));
    int b = std::max(0, std::min(255, static_cast<int>(baseColor.b) + colorDist(gen)));
    
    return sf::Color(static_cast<sf::Uint8>(r), static_cast<sf::Uint8>(g), static_cast<sf::Uint8>(b), baseColor.a);
}