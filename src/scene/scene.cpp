#include "scene/scene.hpp"
#include "scene/sceneObject.hpp"
#include <vector>

// literally nothing to do here for now
namespace Aero::RT::Scene {
	scene::scene() : allocator_(8192) {}

	scene::scene(std::vector<sceneObject*>& objs) : allocator_(8192) {}
}
