#pragma once
#include <vector>
#include <cstdint>
#include "util/Vec.hpp"
#include "window/GLFW_Window.hpp"
#include "util/Shader.hpp"
#include "util/Texture.hpp"

namespace Aero::GL {
	class OGL_RT_TextureViewer {
	public:
		OGL_RT_TextureViewer();

		// Add to OGL_RT_TextureView.hpp under public:
		bool shouldWindowClose() const { return window.ShouldClose(); }
		void renderToTexture(std::vector<uint32_t> buffer, int w, int h);
	private:
		const char* vertexShaderSource = "#version 330 core\n"
			"layout(location = 0) in vec2 aPos;\n"
			"layout(location = 1) in vec2 aUV;\n"
			"\n"
			"out vec2 uv;\n"
			"\n"
			"void main()\n"
			"{\n"
			"    uv = aUV;\n"
			"    gl_Position = vec4(aPos, 0.0, 1.0);\n"
			"}\0";

		const char* fragmentShaderSource = "#version 330 core\n"
			"\n"
			"in vec2 uv;\n"
			"out vec4 FragColor;\n"
			"\n"
			"uniform sampler2D uTexture;\n"
			"\n"
			"void main()\n"
			"{\n"
			"    // Flip the Y coordinate to match OpenGL's bottom-left origin\n"
			"    vec2 flippedUV = vec2(uv.x, 1.0 - uv.y);\n"
			"    FragColor = texture(uTexture, flippedUV);\n"
			"}\0";

		void initOGL();
		void initWindow();
		void createQuad();
		Window::GLFW_Window window;
		Vec2<int> framebufferSize;
		Texture output;
		Shader outputShader;

		unsigned int VAO, VBO, EBO;

		float vertices[16] = {
			// pos        // uv
			-0.5f, -0.5f,  0.0f, 0.0f,  // bottom-left
			 0.5f, -0.5f,  1.0f, 0.0f,  // bottom-right
			 0.5f,  0.5f,  1.0f, 1.0f,  // top-right
			-0.5f,  0.5f,  0.0f, 1.0f   // top-left
		};

		unsigned int indices[6] = {
			0, 1, 2,
			2, 3, 0
		};
	};
}