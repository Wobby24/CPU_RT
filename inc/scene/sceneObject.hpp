#pragma once
#include "geometry/hittable.hpp"
#include "materials/material.hpp"

// sceneObject should contain a hittable and a material. we can set them via the constructor or set methods and then do all of the stuff in an internal function
namespace Aero::RT::Scene {
	class sceneObject {
	public:
		sceneObject() = default;
		sceneObject(Geometry::hittable& geo, Materials::material& mat) : hittable_(geo), material_(mat) {}
		sceneObject(Geometry::hittable& geo) : hittable_(geo) {}

		~sceneObject() = default;

		Geometry::hittable& getHittable() const { return hittable_; }
		Materials::material& getMaterial() const { return material_; }

		Geometry::hittable& getHittable() { return hittable_; }
		Materials::material& getMaterial() { return material_; }

		void setHittable(Geometry::hittable& hittable) const { hittable_ = hittable; }
		void setMaterial(Materials::material& material) const { material_ = material; }

		void setID(uint32_t newID) const { id_ = newID; }
		uint32_t getID() const { return id_; }
		uint32_t getID() { return id_; }
	private:
		Geometry::hittable hittable_;
		Materials::material material_;
		uint32_t id_;
	};
}