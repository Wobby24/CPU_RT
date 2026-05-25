#pragma once
#include <memory>
#include <vector>
#include <cstdint>

namespace Aero::GL {
    class Shader;
    class Texture;
}

namespace Aero::GL::Window {
    class GLFW_Window;
}

namespace Aero::GL {

    class OGL_RT_TextureViewer {
    public:
        OGL_RT_TextureViewer();
        ~OGL_RT_TextureViewer();

        void renderToTexture(const std::vector<uint32_t>& buffer, int w, int h);
        bool shouldWindowClose() const;

    private:
        void initOGL();
        void initWindow();
        void createQuad();

        std::unique_ptr<Aero::GL::Window::GLFW_Window> window;
        std::unique_ptr<Shader> outputShader;
        std::unique_ptr<Texture> output;

        unsigned int VAO, VBO, EBO;
    };

}