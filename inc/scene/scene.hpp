#pragma once

#include "sceneObject.hpp"
#include "camera/camera.hpp"
#include <vector>
#include <util/ArenaAllocator.hpp>
#include <cassert>

// reason why we don't use smart pointers for the scene objects is to ensure we can control the memory of the objects in each scene. We can use arena allocations to tightly pack them together in memory.
// scene will also handle cameras. we can add cameras and then set which one we want during rendering
namespace Aero::RT::Scene {
	class scene {
	public:
		scene();
		scene(std::vector<sceneObject*>& objs);

		void add_object(sceneObject* obj) {
			sceneObject* iobj = allocator_.allocate<sceneObject>(obj->getHittable(), obj->getMaterial());
			objs_.push_back(iobj);
		}

		void add_camera(Camera::camera* cam) { cams_.push_back(cam); }
		void set_current_camera(Camera::camera& cam) {
			current_cam_ = &cam;
		}
		
	//	[[nodiscard]] sceneObject& getSceneObj(const size_t index) const { return *objs_[index];}
		sceneObject& getSceneObj(const size_t index) { return *objs_[index];}
		std::vector<sceneObject*>& getSceneObjs() { return objs_; }

		Camera::camera& getCurrentCam() {
			assert(current_cam_ != nullptr);
			return *current_cam_;
		}
		Camera::camera& getSceneCam(const size_t index) { return *cams_[index];}
	private:
		// just this for now
		Utils::ArenaAllocator allocator_;
		std::vector<sceneObject*> objs_;
		std::vector<Camera::camera*> cams_;
		Camera::camera* current_cam_ = nullptr;
	};
}