#include "ResourceManager.h"
#include <iostream>
#include <filesystem>

ResourceManager::ResourceManager()
    : assetsPath("assets/")
    , texturesPath("assets/textures/")
    , soundsPath("assets/sounds/")
    , fontsPath("assets/fonts/") {
}

ResourceManager::~ResourceManager() {
    cleanup();
}

bool ResourceManager::initialize() {
    logInfo("Initializing Resource Manager...");
    
    // Create asset directories if they don't exist
    try {
        std::filesystem::create_directories(texturesPath);
        std::filesystem::create_directories(soundsPath);
        std::filesystem::create_directories(fontsPath);
    } catch (const std::filesystem::filesystem_error& e) {
        logError("Failed to create asset directories: " + std::string(e.what()));
    }
    
    // Create default assets since we might not have actual asset files
    createDefaultTextures();
    createDefaultSounds();
    
    // Try to load actual assets if they exist
    loadAllAssets();
    
    logInfo("Resource Manager initialized successfully");
    return true;
}

void ResourceManager::cleanup() {
    logInfo("Cleaning up Resource Manager...");
    unloadAllAssets();
}

bool ResourceManager::loadTexture(const std::string& name, const std::string& filename) {
    // Check if texture already exists
    if (hasTexture(name)) {
        logInfo("Texture '" + name + "' already loaded");
        return true;
    }
    
    auto texture = std::make_unique<sf::Texture>();
    
    if (texture->loadFromFile(filename)) {
        textures[name] = std::move(texture);
        logInfo("Loaded texture: " + name + " from " + filename);
        return true;
    } else {
        logError("Failed to load texture: " + filename);
        return false;
    }
}

sf::Texture* ResourceManager::getTexture(const std::string& name) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        return it->second.get();
    }
    
    logError("Texture not found: " + name);
    return nullptr;
}

void ResourceManager::unloadTexture(const std::string& name) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        textures.erase(it);
        logInfo("Unloaded texture: " + name);
    }
}

bool ResourceManager::loadSoundBuffer(const std::string& name, const std::string& filename) {
    if (hasSoundBuffer(name)) {
        logInfo("Sound buffer '" + name + "' already loaded");
        return true;
    }
    
    auto soundBuffer = std::make_unique<sf::SoundBuffer>();
    
    if (soundBuffer->loadFromFile(filename)) {
        soundBuffers[name] = std::move(soundBuffer);
        logInfo("Loaded sound buffer: " + name + " from " + filename);
        return true;
    } else {
        logError("Failed to load sound buffer: " + filename);
        return false;
    }
}

sf::SoundBuffer* ResourceManager::getSoundBuffer(const std::string& name) {
    auto it = soundBuffers.find(name);
    if (it != soundBuffers.end()) {
        return it->second.get();
    }
    
    logError("Sound buffer not found: " + name);
    return nullptr;
}

void ResourceManager::unloadSoundBuffer(const std::string& name) {
    auto it = soundBuffers.find(name);
    if (it != soundBuffers.end()) {
        soundBuffers.erase(it);
        logInfo("Unloaded sound buffer: " + name);
    }
}

bool ResourceManager::loadFont(const std::string& name, const std::string& filename) {
    if (hasFont(name)) {
        logInfo("Font '" + name + "' already loaded");
        return true;
    }
    
    auto font = std::make_unique<sf::Font>();
    
    if (font->loadFromFile(filename)) {
        fonts[name] = std::move(font);
        logInfo("Loaded font: " + name + " from " + filename);
        return true;
    } else {
        logError("Failed to load font: " + filename);
        return false;
    }
}

sf::Font* ResourceManager::getFont(const std::string& name) {
    auto it = fonts.find(name);
    if (it != fonts.end()) {
        return it->second.get();
    }
    
    logError("Font not found: " + name);
    return nullptr;
}

void ResourceManager::unloadFont(const std::string& name) {
    auto it = fonts.find(name);
    if (it != fonts.end()) {
        fonts.erase(it);
        logInfo("Unloaded font: " + name);
    }
}

bool ResourceManager::loadAllAssets() {
    logInfo("Loading all assets...");
    
    bool allLoaded = true;
    
    // Try to load textures
    std::vector<std::pair<std::string, std::string>> textureFiles = {
        {"superman", texturesPath + "superman.png"},
        {"ghost", texturesPath + "ghost.png"},
        {"background", texturesPath + "background.png"},
        {"effects", texturesPath + "effects.png"}
    };
    
    for (const auto& texture : textureFiles) {
        if (std::filesystem::exists(texture.second)) {
            if (!loadTexture(texture.first, texture.second)) {
                allLoaded = false;
            }
        } else {
            logInfo("Texture file not found: " + texture.second + " (using default)");
        }
    }
    
    // Try to load sounds
    std::vector<std::pair<std::string, std::string>> soundFiles = {
        {"laser", soundsPath + "laser.wav"},
        {"punch", soundsPath + "punch.wav"},
        {"ghost_death", soundsPath + "ghost_death.wav"},
        {"background_music", soundsPath + "background_music.ogg"}
    };
    
    for (const auto& sound : soundFiles) {
        if (std::filesystem::exists(sound.second)) {
            if (!loadSoundBuffer(sound.first, sound.second)) {
                allLoaded = false;
            }
        } else {
            logInfo("Sound file not found: " + sound.second + " (using default)");
        }
    }
    
    // Try to load fonts
    std::vector<std::pair<std::string, std::string>> fontFiles = {
        {"default", fontsPath + "arial.ttf"},
        {"title", fontsPath + "title.ttf"}
    };
    
    for (const auto& font : fontFiles) {
        if (std::filesystem::exists(font.second)) {
            if (!loadFont(font.first, font.second)) {
                allLoaded = false;
            }
        } else {
            logInfo("Font file not found: " + font.second + " (using system default)");
        }
    }
    
    return allLoaded;
}

void ResourceManager::unloadAllAssets() {
    textures.clear();
    soundBuffers.clear();
    fonts.clear();
    logInfo("All assets unloaded");
}

bool ResourceManager::createDefaultTextures() {
    logInfo("Creating default textures...");
    
    // Create default Superman texture
    createSupermanTexture();
    
    // Create default Ghost texture
    createGhostTexture();
    
    // Create default background texture
    createBackgroundTexture();
    
    // Create default effects texture
    createEffectsTexture();
    
    return true;
}

bool ResourceManager::createDefaultSounds() {
    logInfo("Creating default sound buffers...");
    
    // Create simple sound buffers (silent for now since we can't generate audio easily)
    // In a real implementation, you would either:
    // 1. Include actual sound files
    // 2. Use a sound generation library
    // 3. Load from embedded resources
    
    // For now, we'll create empty sound buffers to prevent crashes
    auto emptySoundBuffer = std::make_unique<sf::SoundBuffer>();
    // Note: Empty sound buffers will be silent but won't cause crashes
    
    soundBuffers["laser"] = std::make_unique<sf::SoundBuffer>(*emptySoundBuffer);
    soundBuffers["punch"] = std::make_unique<sf::SoundBuffer>(*emptySoundBuffer);
    soundBuffers["ghost_death"] = std::make_unique<sf::SoundBuffer>(*emptySoundBuffer);
    soundBuffers["background_music"] = std::make_unique<sf::SoundBuffer>(*emptySoundBuffer);
    
    return true;
}

sf::Texture* ResourceManager::createColorTexture(sf::Color color, int width, int height) {
    auto texture = std::make_unique<sf::Texture>();
    
    // Create image with solid color
    sf::Image image;
    image.create(width, height, color);
    
    if (texture->loadFromImage(image)) {
        std::string name = "color_" + std::to_string(color.toInteger());
        textures[name] = std::move(texture);
        return textures[name].get();
    }
    
    return nullptr;
}

bool ResourceManager::hasTexture(const std::string& name) const {
    return textures.find(name) != textures.end();
}

bool ResourceManager::hasSoundBuffer(const std::string& name) const {
    return soundBuffers.find(name) != soundBuffers.end();
}

bool ResourceManager::hasFont(const std::string& name) const {
    return fonts.find(name) != fonts.end();
}

void ResourceManager::createSupermanTexture() {
    sf::Image supermanImage;
    supermanImage.create(64, 64, sf::Color::Transparent);
    
    // Draw Superman sprite (simplified)
    // Body (blue)
    for (int x = 20; x < 44; ++x) {
        for (int y = 25; y < 50; ++y) {
            supermanImage.setPixel(x, y, sf::Color::Blue);
        }
    }
    
    // Cape (red)
    for (int x = 10; x < 25; ++x) {
        for (int y = 25; y < 55; ++y) {
            supermanImage.setPixel(x, y, sf::Color::Red);
        }
    }
    
    // Logo (red rectangle on chest)
    for (int x = 25; x < 39; ++x) {
        for (int y = 30; y < 40; ++y) {
            supermanImage.setPixel(x, y, sf::Color::Red);
        }
    }
    
    // Head (skin color)
    sf::Color skinColor(255, 220, 177);
    for (int x = 25; x < 39; ++x) {
        for (int y = 15; y < 28; ++y) {
            int dx = x - 32;
            int dy = y - 21;
            if (dx*dx + dy*dy < 49) { // Circular head
                supermanImage.setPixel(x, y, skinColor);
            }
        }
    }
    
    auto texture = std::make_unique<sf::Texture>();
    if (texture->loadFromImage(supermanImage)) {
        textures["superman"] = std::move(texture);
        logInfo("Created default Superman texture");
    }
}

void ResourceManager::createGhostTexture() {
    sf::Image ghostImage;
    ghostImage.create(64, 64, sf::Color::Transparent);
    
    // Ghost body (white/transparent)
    sf::Color ghostColor(255, 255, 255, 200);
    
    // Main body (circle)
    for (int x = 15; x < 49; ++x) {
        for (int y = 15; y < 45; ++y) {
            int dx = x - 32;
            int dy = y - 30;
            if (dx*dx + dy*dy < 256) { // Circular body
                ghostImage.setPixel(x, y, ghostColor);
            }
        }
    }
    
    // Wavy bottom
    for (int x = 15; x < 49; ++x) {
        for (int y = 45; y < 55; ++y) {
            int wave = static_cast<int>(3 * std::sin((x - 15) * 0.5f));
            if (y < 50 + wave) {
                ghostImage.setPixel(x, y, ghostColor);
            }
        }
    }
    
    // Eyes (black)
    for (int x = 26; x < 30; ++x) {
        for (int y = 25; y < 29; ++y) {
            ghostImage.setPixel(x, y, sf::Color::Black);
        }
    }
    for (int x = 34; x < 38; ++x) {
        for (int y = 25; y < 29; ++y) {
            ghostImage.setPixel(x, y, sf::Color::Black);
        }
    }
    
    auto texture = std::make_unique<sf::Texture>();
    if (texture->loadFromImage(ghostImage)) {
        textures["ghost"] = std::move(texture);
        logInfo("Created default Ghost texture");
    }
}

void ResourceManager::createBackgroundTexture() {
    sf::Image backgroundImage;
    backgroundImage.create(800, 600, sf::Color(10, 10, 30)); // Dark night sky
    
    // Add some stars
    for (int i = 0; i < 200; ++i) {
        int x = rand() % 800;
        int y = rand() % 400; // Only in upper part
        backgroundImage.setPixel(x, y, sf::Color::White);
    }
    
    // Add city silhouette at bottom
    sf::Color buildingColor(5, 5, 15);
    for (int x = 0; x < 800; ++x) {
        int buildingHeight = 100 + (rand() % 150);
        for (int y = 600 - buildingHeight; y < 600; ++y) {
            backgroundImage.setPixel(x, y, buildingColor);
        }
        
        // Add some random windows
        if (rand() % 20 == 0) {
            for (int wy = 600 - buildingHeight + 20; wy < 600 - 20; wy += 25) {
                for (int wx = x; wx < x + 8 && wx < 800; ++wx) {
                    backgroundImage.setPixel(wx, wy, sf::Color::Yellow);
                }
            }
        }
    }
    
    auto texture = std::make_unique<sf::Texture>();
    if (texture->loadFromImage(backgroundImage)) {
        textures["background"] = std::move(texture);
        logInfo("Created default Background texture");
    }
}

void ResourceManager::createEffectsTexture() {
    sf::Image effectsImage;
    effectsImage.create(128, 128, sf::Color::Transparent);
    
    // Create various effect patterns
    // Explosion effect (circular gradient)
    for (int x = 0; x < 64; ++x) {
        for (int y = 0; y < 64; ++y) {
            int dx = x - 32;
            int dy = y - 32;
            float distance = std::sqrt(dx*dx + dy*dy);
            if (distance < 32) {
                sf::Uint8 alpha = static_cast<sf::Uint8>(255 * (1.0f - distance / 32.0f));
                effectsImage.setPixel(x, y, sf::Color(255, 100, 0, alpha));
            }
        }
    }
    
    // Laser effect (horizontal beam)
    for (int x = 64; x < 128; ++x) {
        for (int y = 30; y < 34; ++y) {
            effectsImage.setPixel(x, y, sf::Color::Red);
        }
        for (int y = 28; y < 36; ++y) {
            effectsImage.setPixel(x, y, sf::Color(255, 0, 0, 128));
        }
    }
    
    auto texture = std::make_unique<sf::Texture>();
    if (texture->loadFromImage(effectsImage)) {
        textures["effects"] = std::move(texture);
        logInfo("Created default Effects texture");
    }
}

void ResourceManager::logError(const std::string& message) {
    std::cerr << "[ResourceManager ERROR] " << message << std::endl;
}

void ResourceManager::logInfo(const std::string& message) {
    std::cout << "[ResourceManager INFO] " << message << std::endl;
}