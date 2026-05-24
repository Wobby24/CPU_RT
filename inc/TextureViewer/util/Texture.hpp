#ifndef TEXTURE_H
#define TEXTURE_H

#include "Shader.hpp"
#include <cstdint>
#include <vector>

class Texture
{
public:
    enum class TextureWrap {
        REPEAT,
        MIRRORED_REPEAT,
        CLAMP_TO_EDGE,
        CLAMP_TO_BORDER
    };

    enum class TextureFilter {
        NEAREST,
        LINEAR
    };

    Texture();
    ~Texture();

    void loadTexture(int w, int h, std::vector<uint32_t> data, Shader& shader, const std::string& uniformName, unsigned int unit,
        TextureWrap wrapS = TextureWrap::REPEAT,
        TextureWrap wrapT = TextureWrap::REPEAT,
        TextureFilter minFilter = TextureFilter::LINEAR,
        TextureFilter magFilter = TextureFilter::LINEAR);
    void updateTexture(int w, int h, std::vector<uint32_t> data);

    unsigned int getTextureID() const;

    void bind(Shader& shader, const std::string& uniformName, unsigned int unit);

private:
    // Fallback checkerboard texture data (256x256 RGBA)
    static const unsigned char FBcheckerboardTexture[256 * 256 * 4];
    // Helper methods for setting texture parameters
    void setTextureWrapping(TextureWrap wrapS, TextureWrap wrapT);
    void setTextureFiltering(TextureFilter minFilter, TextureFilter magFilter);

    unsigned int textureID;
    bool isBound;
};

#endif // TEXTURE_H
