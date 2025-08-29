#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <unordered_map>
#include <string>
#include <memory>

class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();
    
    // Initialization
    bool initialize();
    void cleanup();
    
    // Texture management
    bool loadTexture(const std::string& name, const std::string& filename);
    sf::Texture* getTexture(const std::string& name);
    void unloadTexture(const std::string& name);
    
    // Sound management
    bool loadSoundBuffer(const std::string& name, const std::string& filename);
    sf::SoundBuffer* getSoundBuffer(const std::string& name);
    void unloadSoundBuffer(const std::string& name);
    
    // Font management
    bool loadFont(const std::string& name, const std::string& filename);
    sf::Font* getFont(const std::string& name);
    void unloadFont(const std::string& name);
    
    // Batch loading
    bool loadAllAssets();
    void unloadAllAssets();
    
    // Utility functions
    bool createDefaultTextures();
    bool createDefaultSounds();
    sf::Texture* createColorTexture(sf::Color color, int width = 32, int height = 32);
    
    // Asset existence checking
    bool hasTexture(const std::string& name) const;
    bool hasSoundBuffer(const std::string& name) const;
    bool hasFont(const std::string& name) const;
    
    // Statistics
    size_t getTextureCount() const { return textures.size(); }
    size_t getSoundBufferCount() const { return soundBuffers.size(); }
    size_t getFontCount() const { return fonts.size(); }
    
private:
    // Resource containers
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;
    std::unordered_map<std::string, std::unique_ptr<sf::SoundBuffer>> soundBuffers;
    std::unordered_map<std::string, std::unique_ptr<sf::Font>> fonts;
    
    // Helper methods
    bool directoryExists(const std::string& path);
    bool createDirectory(const std::string& path);
    std::string getExecutablePath();
    
    // Asset paths
    std::string assetsPath;
    std::string texturesPath;
    std::string soundsPath;
    std::string fontsPath;
    
    // Default asset creation
    void createSupermanTexture();
    void createGhostTexture();
    void createBackgroundTexture();
    void createEffectsTexture();
    
    // Error handling
    void logError(const std::string& message);
    void logInfo(const std::string& message);
};