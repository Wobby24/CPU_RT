#include "scene/scene.hpp"
#include "scene/sceneObject.hpp"
#include <vector>

// literally nothing to do here for now
// allocate 64kb for testing room
namespace Aero::RT::Scene {
	scene::scene() : allocator_(64000) {}

	scene::scene(std::vector<sceneObject*>& objs) : allocator_(64000) {}
}
