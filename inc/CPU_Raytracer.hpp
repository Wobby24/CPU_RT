#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <sstream>

#include "geometry/hittable_list.hpp"
#include "camera/camera.hpp"
#include "renderer/renderOptions.hpp"
#include "renderer/tile.hpp"

namespace Aero::RT {

	class Aero_CPU_RT {
	public:
		Aero_CPU_RT();
		~Aero_CPU_RT();

		void render(const std::vector<Renderer::tile>& tiles, const Aero::RT::Camera::camera& cam,
			const Aero::RT::Geometry::hittable_list& world, renderOptions& ctx);

		void copy_framebuffer(std::vector<uint32_t>& dest_buffer) {
			std::lock_guard<std::mutex> lock(framebufferMutex);
			dest_buffer = framebuffer; // Fast vector assignment copy
		}

		void stop_rendering() { isRendering = false; }
		bool is_rendering() const { return isRendering; }
	private:
		std::vector<uint32_t> framebuffer;
		std::mutex framebufferMutex;
		std::atomic<bool> isRendering{ false };

		// main render method will use these after determining which it needs to do 
		void renderNewScanline(const Aero::RT::Camera::camera& cam,
			const Aero::RT::Geometry::hittable_list& world, renderOptions& ctx);
		void resumeRender(const Aero::RT::Camera::camera& cam,
			const Aero::RT::Geometry::hittable_list& world, renderOptions& ctx);
		void renderNew(const std::vector<Renderer::tile>& tiles, const Aero::RT::Camera::camera& cam,
			const Aero::RT::Geometry::hittable_list& world, renderOptions& ctx);
	};
}
