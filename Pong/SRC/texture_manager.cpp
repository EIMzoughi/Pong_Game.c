#include "..\HEADER\texture_manager.h"
#include <iostream>
#include <sstream>

TextureManager::TextureManager(SDL_Renderer* renderer)
    : m_renderer(renderer)
{
}

TextureManager::~TextureManager() {
    ClearCache();
}

SDL_Texture* TextureManager::GetTextTexture(const std::string& text, TTF_Font* font, SDL_Color color) {
    if (!m_renderer || !font) {
        return nullptr;
    }
    
    std::string cacheKey = GenerateCacheKey(text, font, color);
    
    // Check if texture is already cached
    auto it = m_textureCache.find(cacheKey);
    if (it != m_textureCache.end()) {
        return it->second;
    }
    
    // Create new texture
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!textSurface) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return nullptr;
    }
    
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
    if (!textTexture) {
        std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
        return nullptr;
    }
    
    // Cache the texture
    m_textureCache[cacheKey] = textTexture;
    
    // Clean up surface
    SDL_FreeSurface(textSurface);
    
    return textTexture;
}

bool TextureManager::RenderText(const std::string& text, TTF_Font* font, int x, int y, SDL_Color color) {
    if (!m_renderer || !font) {
        return false;
    }
    
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!textSurface) {
        std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return false;
    }
    
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(m_renderer, textSurface);
    if (!textTexture) {
        std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(textSurface);
        return false;
    }
    
    SDL_Rect renderQuad = { x, y, textSurface->w, textSurface->h };
    SDL_RenderCopy(m_renderer, textTexture, NULL, &renderQuad);
    
    // Clean up
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
    
    return true;
}

void TextureManager::ClearCache() {
    for (auto& pair : m_textureCache) {
        CleanupTexture(pair.second);
    }
    m_textureCache.clear();
}

void TextureManager::RemoveTexture(const std::string& key) {
    auto it = m_textureCache.find(key);
    if (it != m_textureCache.end()) {
        CleanupTexture(it->second);
        m_textureCache.erase(it);
    }
}

void TextureManager::GetTextSize(const std::string& text, TTF_Font* font, int& width, int& height) {
    if (!font) {
        width = height = 0;
        return;
    }
    
    if (TTF_SizeText(font, text.c_str(), &width, &height) != 0) {
        std::cerr << "Unable to get text size! SDL_ttf Error: " << TTF_GetError() << std::endl;
        width = height = 0;
    }
}

std::string TextureManager::GenerateCacheKey(const std::string& text, TTF_Font* font, SDL_Color color) {
    std::ostringstream oss;
    oss << text << "_" << (void*)font << "_" << (int)color.r << "_" << (int)color.g << "_" << (int)color.b << "_" << (int)color.a;
    return oss.str();
}

void TextureManager::CleanupTexture(SDL_Texture* texture) {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}
