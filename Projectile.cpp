#include "Projectile.h"
#include <cmath>
#include <algorithm>

const float Projectile::LASER_SPEED = 500.0f;
const float Projectile::LASER_DAMAGE = 25.0f;
const float Projectile::LASER_SIZE = 6.0f;

Projectile::Projectile(sf::Vector2f startPosition, sf::Vector2f targetPosition, ProjectileType projectileType)
    : position(startPosition)
    , type(projectileType)
    , active(true)
    , animationTimer(0.0f)
    , pulseFrequency(10.0f)
    , maxTrailPoints(10) {
    
    calculateDirection(startPosition, targetPosition);
    initializeByType();
    
    // Initialize trail
    trail.reserve(maxTrailPoints);
}

Projectile::~Projectile() = default;

void Projectile::calculateDirection(sf::Vector2f startPos, sf::Vector2f targetPos) {
    direction = targetPos - startPos;
    
    // Normalize direction
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 0) {
        direction.x /= length;
        direction.y /= length;
    }
}

void Projectile::initializeByType() {
    switch (type) {
        case ProjectileType::Laser:
            speed = LASER_SPEED;
            damage = LASER_DAMAGE;
            size = LASER_SIZE;
            color = sf::Color::Red;
            maxLifetime = 3.0f;
            lifetime = maxLifetime;
            break;
            
        case ProjectileType::SuperPunch:
            speed = 300.0f;
            damage = 40.0f;
            size = 12.0f;
            color = sf::Color::Yellow;
            maxLifetime = 1.0f;
            lifetime = maxLifetime;
            break;
            
        case ProjectileType::BossAttack:
            speed = 200.0f;
            damage = 30.0f;
            size = 15.0f;
            color = sf::Color::Magenta;
            maxLifetime = 5.0f;
            lifetime = maxLifetime;
            break;
    }
    
    // Set initial velocity
    velocity = direction * speed;
}

void Projectile::update(float deltaTime) {
    if (!active) return;
    
    // Update position
    position += velocity * deltaTime;
    
    // Update lifetime
    lifetime -= deltaTime;
    if (lifetime <= 0) {
        active = false;
    }
    
    // Update animation
    animationTimer += deltaTime;
    
    // Update trail
    updateTrail(deltaTime);
    
    // Special updates based on projectile type
    switch (type) {
        case ProjectileType::Laser:
            // Lasers maintain constant speed and direction
            break;
            
        case ProjectileType::SuperPunch:
            // Punch projectiles slow down over time
            velocity *= 0.98f;
            break;
            
        case ProjectileType::BossAttack:
            // Boss attacks might have homing behavior
            // Add slight wobble effect
            float wobble = std::sin(animationTimer * 8.0f) * 20.0f;
            velocity.y += wobble * deltaTime;
            break;
    }
}

void Projectile::render(sf::RenderWindow& window) {
    if (!active) return;
    
    // Draw trail first (behind projectile)
    drawTrail(window);
    
    // Draw main projectile
    drawLaser(window);
}

void Projectile::updateTrail(float deltaTime) {
    // Add current position to trail
    TrailPoint newPoint;
    newPoint.position = position;
    newPoint.alpha = 1.0f;
    newPoint.age = 0.0f;
    
    trail.insert(trail.begin(), newPoint);
    
    // Update existing trail points
    for (auto it = trail.begin(); it != trail.end();) {
        it->age += deltaTime;
        it->alpha = 1.0f - (it->age / 0.5f); // Trail fades over 0.5 seconds
        
        if (it->alpha <= 0 || it->age > 0.5f) {
            it = trail.erase(it);
        } else {
            ++it;
        }
    }
    
    // Limit trail size
    if (trail.size() > static_cast<size_t>(maxTrailPoints)) {
        trail.resize(maxTrailPoints);
    }
}

void Projectile::drawLaser(sf::RenderWindow& window) {
    // Calculate pulsing effect
    float pulseIntensity = 1.0f + 0.3f * std::sin(animationTimer * pulseFrequency);
    float currentSize = size * pulseIntensity;
    
    // Fade based on lifetime
    float fadeAlpha = std::min(1.0f, lifetime / maxLifetime);
    sf::Color currentColor = color;
    currentColor.a = static_cast<sf::Uint8>(255 * fadeAlpha);
    
    switch (type) {
        case ProjectileType::Laser: {
            // Draw laser as elongated rectangle
            sf::RectangleShape laser(sf::Vector2f(currentSize * 3, currentSize));
            laser.setFillColor(currentColor);
            laser.setOrigin(currentSize * 1.5f, currentSize * 0.5f);
            laser.setPosition(position);
            
            // Rotate to match direction
            float angle = std::atan2(direction.y, direction.x) * 180.0f / 3.14159f;
            laser.setRotation(angle);
            
            window.draw(laser);
            
            // Add glow effect
            sf::CircleShape glow(currentSize * 2);
            glow.setFillColor(sf::Color(currentColor.r, currentColor.g, currentColor.b, 64));
            glow.setOrigin(currentSize * 2, currentSize * 2);
            glow.setPosition(position);
            window.draw(glow);
            break;
        }
        
        case ProjectileType::SuperPunch: {
            // Draw punch as expanding circle
            sf::CircleShape punch(currentSize);
            punch.setFillColor(currentColor);
            punch.setOrigin(currentSize, currentSize);
            punch.setPosition(position);
            window.draw(punch);
            
            // Add impact rings
            for (int i = 1; i <= 3; ++i) {
                sf::CircleShape ring(currentSize * (1 + i * 0.5f));
                ring.setFillColor(sf::Color::Transparent);
                ring.setOutlineThickness(2);
                ring.setOutlineColor(sf::Color(currentColor.r, currentColor.g, currentColor.b, 128 / i));
                ring.setOrigin(ring.getRadius(), ring.getRadius());
                ring.setPosition(position);
                window.draw(ring);
            }
            break;
        }
        
        case ProjectileType::BossAttack: {
            // Draw boss attack as dark energy ball
            sf::CircleShape energyBall(currentSize);
            energyBall.setFillColor(currentColor);
            energyBall.setOrigin(currentSize, currentSize);
            energyBall.setPosition(position);
            window.draw(energyBall);
            
            // Add dark aura
            sf::CircleShape aura(currentSize * 1.5f);
            aura.setFillColor(sf::Color(currentColor.r, currentColor.g, currentColor.b, 32));
            aura.setOrigin(currentSize * 1.5f, currentSize * 1.5f);
            aura.setPosition(position);
            window.draw(aura);
            break;
        }
    }
}

void Projectile::drawTrail(sf::RenderWindow& window) {
    if (trail.size() < 2) return;
    
    // Draw trail segments
    for (size_t i = 1; i < trail.size(); ++i) {
        sf::Color trailColor = color;
        trailColor.a = static_cast<sf::Uint8>(255 * trail[i].alpha * 0.5f);
        
        float trailSize = size * trail[i].alpha;
        sf::CircleShape trailPoint(trailSize * 0.5f);
        trailPoint.setFillColor(trailColor);
        trailPoint.setOrigin(trailSize * 0.5f, trailSize * 0.5f);
        trailPoint.setPosition(trail[i].position);
        
        window.draw(trailPoint);
    }
}

bool Projectile::isOffScreen(int windowWidth, int windowHeight) const {
    return (position.x < -size || position.x > windowWidth + size ||
            position.y < -size || position.y > windowHeight + size);
}

sf::FloatRect Projectile::getBounds() const {
    return sf::FloatRect(
        position.x - size,
        position.y - size,
        size * 2,
        size * 2
    );
}