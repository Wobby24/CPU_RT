#pragma once

#include <string>
#include "Interface/IWindow.hpp"
#include <memory>
#include "TextureViewer/util/Vec.hpp"
#include "GLFWRenderSurface.hpp"

class GLFWwindow;

namespace Aero::GL::Window {

	//for fullscreen, pointer lock, and vsync
	struct WindowState {
		bool isFullscreen = false;
		bool isCursorLocked = false;
		bool vsyncEnabled = true;

		int windowedWidth = 1280;
		int windowedHeight = 720;
		int windowedPosX = 100;
		int windowedPosY = 100;
	};

	class GLFW_Window : public IWindow
	{
	public:
		GLFW_Window() {}
		GLFW_Window(int width, int height, const std::string& title,
			int glMajorVersion = 3, int glMinorVersion = 3);
		GLFW_Window(const std::string& title, int glMajorVersion = 3, int glMinorVersion = 3);
		~GLFW_Window() override;

		void PollEvents() override;
		void SwapBuffers() override;
		bool ShouldClose() const override;
		void SetTitle(const std::string& title) override;
		void SetSize(int width, int height) override;
		void GetSize(int& width, int& height) const override;
		Vec2<int> GetSize() const override;
		bool IsFullscreen() const;
		bool IsVSyncEnabled() const;
		bool IsPointerLocked() const;
		void SetPosition(int x, int y);
		void GetPosition(int& x, int& y);
		void ToggleVSync();
		void ToggleFullscreen();
		void TogglePointerLock();
		//void SetWindowIcon(const std::string& iconFilePath);
		Vec2<int> GetPosition() const;

		// Return native handle for internal use or advanced scenarios
		void* GetNativeHandle() const override { return static_cast<void*>(window_); }

		IRenderSurface* GetSurface() { return surface_.get(); }

	private:
		GLFWwindow* window_ = nullptr;
		int width_ = 800;
		int height_ = 600;
		std::string title_;
		bool shouldClose_ = false;
		bool initialized_ = false;
		bool isCleanedUp_ = false;

		WindowState state_;
		std::unique_ptr<GLFWRenderSurface> surface_;

		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		void HandleInternalInput();
	};
}
		
