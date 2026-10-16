#pragma once

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
#include "util/thread_pool.hpp"
#include "util/color.hpp"
#include <scene/scene.hpp>

namespace Aero::RT {

	class Aero_CPU_RT {
	public:
		Aero_CPU_RT();
		~Aero_CPU_RT();

		// test both the current render function and a new one taking the scene ref as a parameter
		void render(const Aero::RT::Camera::camera& cam,
			const Aero::RT::Geometry::hittable_list& world, const renderOptions& ctx);

		void render(Scene::scene& scene, const renderOptions& ctx);

		void copy_framebuffer(std::vector<uint32_t>& dest_buffer) {
			std::lock_guard<std::mutex> lock(framebufferMutex);
			dest_buffer = framebuffer; // Fast vector assignment copy
		}

		void stop_rendering() { isRendering = false; }
		bool is_rendering() const { return isRendering; }
	private:
		std::vector<uint32_t> framebuffer;
		std::vector<Utils::color> ppmFramebuffer;

		std::queue<int> finishedTileQueue;
		std::mutex queueMutex;
		std::condition_variable queueCV;

		std::thread fileWriterThread;
		std::atomic<int> completedTileCount{ 0 };   // Shared progress tracker
		std::atomic<bool> isFileWriterRunning{ false };

		std::mutex framebufferMutex;

		std::atomic<bool> isRendering{ false };
		
		// scanline code (old, not recommeneded)
		void resumeRender(const Aero::RT::Camera::camera& cam,
			const Aero::RT::Geometry::hittable_list& world, const renderOptions& ctx);
		void renderNew(const Aero::RT::Camera::camera& cam,
				const Aero::RT::Geometry::hittable_list& world, const renderOptions& ctx);

		void renderTiles(int tileIndex, const Aero::RT::Renderer::tile& t, const Aero::RT::Camera::camera& cam,
			const Aero::RT::Geometry::hittable_list& world, const renderOptions& ctx,
			std::vector<uint32_t>& framebuffer,
			std::vector<Utils::color>& ppmFramebuffer);

		Aero::RT::Geometry::hittable_list convertSceneObjsGeo(Scene::scene& scene);

		void fileWriterLoop(const std::vector<Renderer::tile>& tiles, const renderOptions& ctx, int startingTileIdx);
		void writeToPPM(const renderOptions& ctx);
		int recoveryIndex(const renderOptions& ctx);
	};
}
