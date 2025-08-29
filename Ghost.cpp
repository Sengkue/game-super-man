#include "Ghost.h"
#include <cmath>
#include <random>

const float Ghost::BASE_SIZE = 30.0f;
const float Ghost::COLLISION_RADIUS = 25.0f;

Ghost::Ghost(sf::Vector2f startPosition, GhostType ghostType)
    : position(startPosition)
    , velocity(0, 0)
    , basePosition(startPosition)
    , type(ghostType)
    , floatingTimer(0.0f)
    , floatingAmplitude(10.0f)
    , floatingFrequency(2.0f)
    , animationTimer(0.0f)
    , currentFrame(0)
    , transparency(0.8f)
    , aiUpdateTimer(0.0f)
    , damageTimer(0.0f)
    , bossAttackTimer(0.0f)
    , bossAttackCooldown(3.0f) {
    
    // Initialize random floating offset
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(0, 2 * 3.14159f);
    floatingTimer = dist(gen);
    
    initializeByType();
}

Ghost::~Ghost() = default;

void Ghost::initializeByType() {
    switch (type) {
        case GhostType::Regular:
            health = 20.0f;
            maxHealth = 20.0f;
            speed = 80.0f;
            aggroRange = 200.0f;
            damageValue = 10.0f;
            size = BASE_SIZE;
            ghostColor = sf::Color(255, 255, 255, static_cast<sf::Uint8>(255 * transparency));
            break;
            
        case GhostType::Fast:
            health = 15.0f;
            maxHealth = 15.0f;
            speed = 150.0f;
            aggroRange = 250.0f;
            damageValue = 15.0f;
            size = BASE_SIZE * 0.8f;
            ghostColor = sf::Color(200, 200, 255, static_cast<sf::Uint8>(255 * transparency));
            floatingFrequency = 4.0f; // Faster floating
            break;
            
        case GhostType::Boss:
            health = 50.0f;
            maxHealth = 50.0f;
            speed = 60.0f;
            aggroRange = 300.0f;
            damageValue = 20.0f;
            size = BASE_SIZE * 1.5f;
            ghostColor = sf::Color(255, 200, 200, static_cast<sf::Uint8>(255 * transparency));
            floatingAmplitude = 15.0f; // Bigger floating motion
            break;
    }
}

void Ghost::update(float deltaTime, sf::Vector2f supermanPosition) {
    // Update timers
    aiUpdateTimer += deltaTime;
    damageTimer += deltaTime;
    
    // AI behavior
    if (aiUpdateTimer >= 0.1f) { // Update AI 10 times per second
        moveTowardsSuperman(supermanPosition, deltaTime);
        aiUpdateTimer = 0.0f;
    }
    
    // Apply velocity
    position += velocity * deltaTime;
    
    // Update floating animation
    updateFloatingAnimation(deltaTime);
    
    // Update general animation
    updateAnimation(deltaTime);
    
    // Boss specific updates
    if (type == GhostType::Boss) {
        updateBossAttack(deltaTime, supermanPosition);
    }
}

void Ghost::render(sf::RenderWindow& window) {
    // Draw ghost body
    drawGhostBody(window);
    
    // Draw ghost face
    drawGhostFace(window);
    
    // Draw boss effects
    if (type == GhostType::Boss) {
        drawBossEffects(window);
    }
    
    // Draw health bar for boss
    if (type == GhostType::Boss) {
        float healthPercent = health / maxHealth;
        
        sf::RectangleShape healthBarBg(sf::Vector2f(size * 1.2f, 4));
        healthBarBg.setPosition(position.x - size * 0.6f, position.y - size * 0.8f);
        healthBarBg.setFillColor(sf::Color::Red);
        
        sf::RectangleShape healthBar(sf::Vector2f(size * 1.2f * healthPercent, 4));
        healthBar.setPosition(position.x - size * 0.6f, position.y - size * 0.8f);
        healthBar.setFillColor(sf::Color::Green);
        
        window.draw(healthBarBg);
        window.draw(healthBar);
    }
}

void Ghost::moveTowardsSuperman(sf::Vector2f supermanPosition, float deltaTime) {
    sf::Vector2f direction = supermanPosition - position;
    float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    if (distance > 0 && distance <= aggroRange) {
        // Normalize direction
        direction.x /= distance;
        direction.y /= distance;
        
        // Add some randomness to movement
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> randomness(-0.3f, 0.3f);
        
        direction.x += randomness(gen);
        direction.y += randomness(gen);
        
        // Apply movement
        velocity = direction * speed;
        
        // Boss ghosts have different movement patterns
        if (type == GhostType::Boss) {
            // Circular movement around Superman
            float angle = std::atan2(direction.y, direction.x);
            angle += deltaTime * 2.0f; // Orbit speed
            
            velocity.x = std::cos(angle) * speed;
            velocity.y = std::sin(angle) * speed;
        }
    } else {
        // Apply friction when not moving towards Superman
        velocity *= 0.9f;
    }
}

void Ghost::avoidOtherGhosts(float deltaTime) {
    // This would require access to other ghosts - implement in Game class collision detection
}

void Ghost::updateFloatingAnimation(float deltaTime) {
    floatingTimer += deltaTime * floatingFrequency;
    
    // Update base position for floating effect
    float floatingOffset = std::sin(floatingTimer) * floatingAmplitude;
    position.y = basePosition.y + floatingOffset;
    
    // Update base position to follow movement
    basePosition.x = position.x;
}

void Ghost::takeDamage(float damage) {
    health -= damage;
    if (health < 0) {
        health = 0;
    }
    
    damageTimer = 0.0f; // Reset damage timer for visual feedback
}

sf::FloatRect Ghost::getBounds() const {
    return sf::FloatRect(
        position.x - COLLISION_RADIUS,
        position.y - COLLISION_RADIUS,
        COLLISION_RADIUS * 2,
        COLLISION_RADIUS * 2
    );
}

int Ghost::getScoreValue() const {
    switch (type) {
        case GhostType::Regular:
            return 10;
        case GhostType::Fast:
            return 20;
        case GhostType::Boss:
            return 50;
        default:
            return 10;
    }
}

void Ghost::updateAnimation(float deltaTime) {
    animationTimer += deltaTime;
    
    // Simple animation cycling for transparency effect
    transparency = 0.7f + 0.3f * std::sin(animationTimer * 3.0f);
    
    // Update ghost color with new transparency
    sf::Uint8 alpha = static_cast<sf::Uint8>(255 * transparency);
    
    switch (type) {
        case GhostType::Regular:
            ghostColor = sf::Color(255, 255, 255, alpha);
            break;
        case GhostType::Fast:
            ghostColor = sf::Color(200, 200, 255, alpha);
            break;
        case GhostType::Boss:
            ghostColor = sf::Color(255, 200, 200, alpha);
            break;
    }
}

void Ghost::updateBossAttack(float deltaTime, sf::Vector2f supermanPosition) {
    bossAttackTimer += deltaTime;
    
    if (bossAttackTimer >= bossAttackCooldown) {
        // Boss special attack - could spawn projectiles or create area effects
        bossAttackTimer = 0.0f;
        
        // For now, just increase damage temporarily
        damageValue = 30.0f;
        
        // Reset damage after short time
        if (damageTimer > 0.5f) {
            damageValue = 20.0f;
        }
    }
}

void Ghost::drawGhostBody(sf::RenderWindow& window) {
    // Main ghost body (circle)
    sf::CircleShape body(size * 0.8f);
    body.setFillColor(ghostColor);
    body.setPosition(position.x - size * 0.8f, position.y - size * 0.8f);
    window.draw(body);
    
    // Ghost tail (wavy bottom part)
    for (int i = 0; i < 5; ++i) {
        float waveOffset = std::sin(animationTimer * 4.0f + i * 0.5f) * 3.0f;
        sf::CircleShape tailPart(size * 0.15f);
        tailPart.setFillColor(ghostColor);
        tailPart.setPosition(
            position.x - size * 0.6f + i * (size * 0.3f),
            position.y + size * 0.4f + waveOffset
        );
        window.draw(tailPart);
    }
}

void Ghost::drawGhostFace(sf::RenderWindow& window) {
    // Eyes
    sf::CircleShape leftEye(size * 0.1f);
    sf::CircleShape rightEye(size * 0.1f);
    
    leftEye.setFillColor(sf::Color::Black);
    rightEye.setFillColor(sf::Color::Black);
    
    leftEye.setPosition(position.x - size * 0.3f, position.y - size * 0.2f);
    rightEye.setPosition(position.x + size * 0.1f, position.y - size * 0.2f);
    
    window.draw(leftEye);
    window.draw(rightEye);
    
    // Mouth (simple oval)
    sf::CircleShape mouth(size * 0.15f);
    mouth.setFillColor(sf::Color::Black);
    mouth.setPosition(position.x - size * 0.15f, position.y + size * 0.1f);
    mouth.setScale(1.0f, 0.5f); // Flatten to make oval
    window.draw(mouth);
}

void Ghost::drawBossEffects(sf::RenderWindow& window) {
    // Glowing aura effect for boss
    sf::CircleShape aura(size * 1.2f);
    aura.setFillColor(sf::Color(255, 100, 100, 32)); // Very transparent red
    aura.setPosition(position.x - size * 1.2f, position.y - size * 1.2f);
    window.draw(aura);
    
    // Pulsing effect
    float pulse = 1.0f + 0.2f * std::sin(animationTimer * 6.0f);
    sf::CircleShape pulseEffect(size * 0.5f * pulse);
    pulseEffect.setFillColor(sf::Color(255, 0, 0, 64));
    pulseEffect.setPosition(position.x - size * 0.5f * pulse, position.y - size * 0.5f * pulse);
    window.draw(pulseEffect);
}