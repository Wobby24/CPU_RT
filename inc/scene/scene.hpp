#pragma once

#include "geometry/hittable_list.hpp"
#include "sceneObject.hpp"
#include <vector>
#include <memory>
#include <map>
// use smart pointers
namespace Aero::RT::Scene {
	class scene {
	public:
		scene() = default;
		scene(std::vector<std::shared_ptr<sceneObject>> objs);

		void add_object(std::shared_ptr<sceneObject> obj);
		
		std::shared_ptr<sceneObject>& getObj(size_t index) const;
		std::shared_ptr<sceneObject>& getObj(size_t index);
	private:
		//std::vector<std::shared_ptr<sceneObject>> objs;
		Geometry::hittable_list geometry;
		std::map<int, Materials::material> materials;
	};
}