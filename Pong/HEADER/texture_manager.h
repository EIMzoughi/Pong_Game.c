#pragma once

#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <unordered_map>
#include <memory>

class TextureManager {
public:
    TextureManager(SDL_Renderer* renderer);
    ~TextureManager();
    
    // Text rendering with caching
    SDL_Texture* GetTextTexture(const std::string& text, TTF_Font* font, SDL_Color color = {255, 255, 255, 255});
    
    // Direct rendering (for dynamic text)
    bool RenderText(const std::string& text, TTF_Font* font, int x, int y, SDL_Color color = {255, 255, 255, 255});
    
    // Cache management
    void ClearCache();
    void RemoveTexture(const std::string& key);
    
    // Utility functions
    void GetTextSize(const std::string& text, TTF_Font* font, int& width, int& height);
    
private:
    SDL_Renderer* m_renderer;
    std::unordered_map<std::string, SDL_Texture*> m_textureCache;
    
    // Private methods
    std::string GenerateCacheKey(const std::string& text, TTF_Font* font, SDL_Color color);
    void CleanupTexture(SDL_Texture* texture);
};

#endif // TEXTURE_MANAGER_H
