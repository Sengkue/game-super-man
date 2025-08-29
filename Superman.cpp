#include "Superman.h"
#include <cmath>
#include <algorithm>

const float Superman::MAX_SPEED = 300.0f;
const float Superman::PUNCH_DAMAGE = 30.0f;
const float Superman::SIZE = 40.0f;

Superman::Superman(float x, float y)
    : position(x, y)
    , velocity(0, 0)
    , health(100.0f)
    , maxHealth(100.0f)
    , speed(250.0f)
    , acceleration(800.0f)
    , friction(0.85f)
    , punchTimer(0.0f)
    , punchDuration(0.3f)
    , punchRange(80.0f)
    , animationTimer(0.0f)
    , currentFrame(0)
    , maxFrames(4)
    , capeAnimationTimer(0.0f)
    , regenTimer(0.0f)
    , regenRate(1.0f)  // 1 second between regeneration
    , regenAmount(1.0f) // 1 health per regeneration
    , texture(nullptr) {
    
    // Create superman visual representation (colored rectangles since we may not have sprites)
    sprite.setPosition(position);
    
    // Cape setup
    cape.setSize(sf::Vector2f(15, 25));
    cape.setFillColor(sf::Color::Red);
    cape.setOrigin(7.5f, 0);
}

Superman::~Superman() = default;

void Superman::update(float deltaTime, const sf::RenderWindow& window) {
    handleInput(deltaTime);
    
    // Apply velocity
    position += velocity * deltaTime;
    
    // Update timers
    if (punchTimer > 0) {
        punchTimer -= deltaTime;
    }
    
    regenTimer += deltaTime;
    
    // Update animations
    updateAnimation(deltaTime);
    updateCape(deltaTime);
    
    // Keep Superman in window bounds
    constrainToWindow(window);
    
    // Update sprite position
    sprite.setPosition(position);
}

void Superman::render(sf::RenderWindow& window) {
    // Draw cape first (behind Superman)
    window.draw(cape);
    
    // Draw Superman body
    sf::RectangleShape body(sf::Vector2f(SIZE * 0.6f, SIZE * 0.8f));
    body.setFillColor(sf::Color::Blue);
    body.setPosition(position.x - SIZE * 0.3f, position.y - SIZE * 0.4f);
    window.draw(body);
    
    // Draw Superman logo (chest)
    sf::RectangleShape logo(sf::Vector2f(SIZE * 0.4f, SIZE * 0.3f));
    logo.setFillColor(sf::Color::Red);
    logo.setPosition(position.x - SIZE * 0.2f, position.y - SIZE * 0.25f);
    window.draw(logo);
    
    // Draw S symbol
    sf::Text sSymbol;
    sf::Font font; // Note: In real implementation, load font from ResourceManager
    sSymbol.setFont(font);
    sSymbol.setString("S");
    sSymbol.setCharacterSize(static_cast<unsigned int>(SIZE * 0.4f));
    sSymbol.setFillColor(sf::Color::Yellow);
    sSymbol.setPosition(position.x - SIZE * 0.1f, position.y - SIZE * 0.2f);
    // window.draw(sSymbol); // Commented out since font might not load
    
    // Draw head
    sf::CircleShape head(SIZE * 0.25f);
    head.setFillColor(sf::Color(255, 220, 177)); // Skin color
    head.setPosition(position.x - SIZE * 0.25f, position.y - SIZE * 0.6f);
    window.draw(head);
    
    // Draw eyes (laser vision effect)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        sf::RectangleShape laserEye1(sf::Vector2f(3, 1));
        sf::RectangleShape laserEye2(sf::Vector2f(3, 1));
        laserEye1.setFillColor(sf::Color::Red);
        laserEye2.setFillColor(sf::Color::Red);
        laserEye1.setPosition(position.x - 8, position.y - SIZE * 0.45f);
        laserEye2.setPosition(position.x + 5, position.y - SIZE * 0.45f);
        window.draw(laserEye1);
        window.draw(laserEye2);
    }
    
    // Draw punch effect
    if (isPunching()) {
        sf::CircleShape punchEffect(punchRange);
        punchEffect.setFillColor(sf::Color(255, 255, 0, 64)); // Semi-transparent yellow
        punchEffect.setPosition(position.x - punchRange, position.y - punchRange);
        window.draw(punchEffect);
    }
    
    // Draw health bar above Superman
    drawHealthBar(window);
}

void Superman::handleInput(float deltaTime) {
    sf::Vector2f inputDirection(0, 0);
    
    // WASD movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        inputDirection.y -= 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        inputDirection.y += 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        inputDirection.x -= 1.0f;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        inputDirection.x += 1.0f;
    }
    
    // Normalize diagonal movement
    float length = std::sqrt(inputDirection.x * inputDirection.x + inputDirection.y * inputDirection.y);
    if (length > 0) {
        inputDirection.x /= length;
        inputDirection.y /= length;
    }
    
    // Apply acceleration
    velocity += inputDirection * acceleration * deltaTime;
    
    // Apply friction
    velocity *= friction;
    
    // Limit maximum speed
    float currentSpeed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
    if (currentSpeed > MAX_SPEED) {
        velocity.x = (velocity.x / currentSpeed) * MAX_SPEED;
        velocity.y = (velocity.y / currentSpeed) * MAX_SPEED;
    }
}

void Superman::move(sf::Vector2f direction, float deltaTime) {
    velocity += direction * acceleration * deltaTime;
}

void Superman::performPunch(sf::Vector2f targetPosition) {
    punchTimer = punchDuration;
    punchTarget = targetPosition;
}

sf::FloatRect Superman::getPunchRange() const {
    return sf::FloatRect(
        position.x - punchRange,
        position.y - punchRange,
        punchRange * 2,
        punchRange * 2
    );
}

void Superman::takeDamage(float damage) {
    health -= damage;
    if (health < 0) {
        health = 0;
    }
}

void Superman::regenerateHealth(float deltaTime) {
    if (regenTimer >= regenRate && health < maxHealth) {
        health += regenAmount;
        if (health > maxHealth) {
            health = maxHealth;
        }
        regenTimer = 0.0f;
    }
}

sf::FloatRect Superman::getBounds() const {
    return sf::FloatRect(
        position.x - SIZE * 0.5f,
        position.y - SIZE * 0.5f,
        SIZE,
        SIZE
    );
}

void Superman::reset(float x, float y) {
    position = sf::Vector2f(x, y);
    velocity = sf::Vector2f(0, 0);
    health = maxHealth;
    punchTimer = 0;
    animationTimer = 0;
    capeAnimationTimer = 0;
    regenTimer = 0;
}

void Superman::updateAnimation(float deltaTime) {
    animationTimer += deltaTime;
    
    // Simple animation cycling
    if (animationTimer >= 0.2f) { // Change frame every 0.2 seconds
        currentFrame = (currentFrame + 1) % maxFrames;
        animationTimer = 0.0f;
    }
}

void Superman::updateCape(float deltaTime) {
    capeAnimationTimer += deltaTime * 3.0f; // Cape animation speed
    
    // Calculate cape position with flowing effect
    float capeOffset = std::sin(capeAnimationTimer) * 5.0f;
    cape.setPosition(position.x - 20 + capeOffset, position.y - SIZE * 0.3f);
    
    // Cape follows movement
    if (velocity.x < 0) {
        cape.setPosition(cape.getPosition().x + 10, cape.getPosition().y);
    } else if (velocity.x > 0) {
        cape.setPosition(cape.getPosition().x - 10, cape.getPosition().y);
    }
}

void Superman::constrainToWindow(const sf::RenderWindow& window) {
    sf::Vector2u windowSize = window.getSize();
    
    // Keep Superman within window bounds
    position.x = std::max(SIZE * 0.5f, std::min(static_cast<float>(windowSize.x) - SIZE * 0.5f, position.x));
    position.y = std::max(SIZE * 0.5f, std::min(static_cast<float>(windowSize.y) - SIZE * 0.5f, position.y));
}

void Superman::drawHealthBar(sf::RenderWindow& window) {
    float healthPercent = health / maxHealth;
    
    // Health bar background
    sf::RectangleShape healthBarBg(sf::Vector2f(SIZE, 6));
    healthBarBg.setPosition(position.x - SIZE * 0.5f, position.y - SIZE * 0.7f);
    healthBarBg.setFillColor(sf::Color::Red);
    healthBarBg.setOutlineThickness(1);
    healthBarBg.setOutlineColor(sf::Color::White);
    
    // Health bar foreground
    sf::RectangleShape healthBar(sf::Vector2f(SIZE * healthPercent, 6));
    healthBar.setPosition(position.x - SIZE * 0.5f, position.y - SIZE * 0.7f);
    healthBar.setFillColor(sf::Color::Green);
    
    window.draw(healthBarBg);
    window.draw(healthBar);
}